/**
 * \file Environment.cpp
 * \brief Engine environment implementation
 * \version 0.5.99c ($Revision: 3157 $)
 * \date 25/04/2011
 * \author Sergey Kosarevsky, 2009-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Platform.h"
#include "Audio/OGG.h"
#include "Utils/Utils.h"
#include "Utils/Localizer.h"
#include "Images/FI_Utils.h"
#include "Core/CVars.h"
#include "Core/Script.h"
#include "Resources/iVideoEncoder.h"
#include "Resources/ResourcesManager.h"

#include "Input/Input.h"
#include "Utils/Viewport.h"

using namespace ::Linderdaum;

#ifdef _MSC_VER
//#   define _CRTDBG_MAP_ALLOC
//#   include <stdlib.h>
//#   include <crtdbg.h>
#endif

#ifdef OS_WINDOWS
#  include <conio.h>
#  include <windows.h>
#  undef min
#  undef max
#endif

#if defined(OS_LINUX) && !defined(OS_ANDROID)
// defined in SysEnv.cpp
int kbhit( void );
int getch( );
#endif

#include <iostream>

#include "Core/Logger.h"
#include "Core/Linker.h"
#include "Core/VFS/FileSystem.h"
#include "GUI/GUIManager.h"
//#include "Network/Network.h"
#include "Core/Logger.h"
#include "Core/Console.h"
#include "Utils/Screen.h"
#include "Utils/Library.h"
#include "Renderer/iRenderContext.h"
#include "World/World.h"
#include "Audio/Audio.h"
#include "Math/LRandom.h"

#include "Generated/Exports/Exports_Core.h"
#include "Generated/Globals/ColorRegistration.h"
#include "Generated/Globals/KeySymbolsRegistration.h"

#include "Environment.h"

// From Network.h, to avoid redundant includes
void InitializeNetwork();
void ShutdownNetwork();

void sEnvironment::Event_BEGINPLAY( LEvent Event, const LEventArgs& Args )
{
	// reset timers
	FOldTime = Env->GetSeconds();
	FNewTime = FOldTime;
	FSecondStartTime = FOldTime;
}

void sEnvironment::Event_PAUSE( LEvent Event, const LEventArgs& Args )
{
	FEnginePaused->SetBool( Args.FBoolArg );
}

void sEnvironment::Initialize( bool ContextTakeover,
                               bool OpenViewport,
                               bool CreateRenderer,
                               const std::vector<LString>* CommandLine,
                               void* ExternalWndHandle )
{
	guard();

	// nice logo ;)
	Env->Logger->Log( L_LOG, ENGINE_NAME );
	Env->Logger->Log( L_LOG, ENGINE_COPYRIGHT1 );
	Env->Logger->Log( L_LOG, ENGINE_COPYRIGHT2 );
	Env->Logger->Log( L_LOG, "Version: " + ENGINE_VERSION );
	Env->Logger->Log( L_LOG, "Compiled on: " + ENGINE_COMPILE_TIME );
	//

	FFPS = Env->Console->GetVar( "Stats.FPS" );
	FFPS->SetDouble( 0.0 );

	FRecordScreenshotSequence = Env->Console->GetVar( "RecordScreenshotSequence" );
	FRecordScreenshotSequence->SetBool( false );

	FUpdatesPerFrame = Env->Console->GetVar( "UpdatesPerFrame" );
	FUpdatesPerFrame->SetInt( 0 );

	//
	Env->Console->RegisterCommand( "SetViewportTitle", Utils::Bind( &sEnvironment::SetViewportTitleC, this ) );
	Env->Console->RegisterCommand( "StartVideoRec",    Utils::Bind( &sEnvironment::StartVideoRecC, this ) );
	Env->Console->RegisterCommand( "StopVideoRec",     Utils::Bind( &sEnvironment::StopVideoRecC, this ) );

	FOldTime = Env->GetSeconds();
	FNewTime = FOldTime;
	FSecondStartTime = FOldTime;

	Env->Connect( L_EVENT_BEGINPLAY, BIND( &sEnvironment::Event_BEGINPLAY ) );
	Env->Connect( L_EVENT_PAUSE,     BIND( &sEnvironment::Event_PAUSE     ) );

//  Env->Renderer->DeleteContext();

	delete( Viewport );

	Viewport = NULL;

	if ( ContextTakeover )
	{
		Viewport = Env->Screen->TakeoverViewport( Utils::ultimate_cast<LWindowHandle>( ExternalWndHandle ) );
		Env->Renderer->InitContext( Viewport, 0, 0, 0, 0, true );
	}
	else if ( OpenViewport )
	{
		int  Width        = Env->Console->GetVarValueInt( "Viewport.Width", 1024 );
		int  Height       = Env->Console->GetVarValueInt( "Viewport.Height", 768 );
		bool Fullscreen   = Env->Console->GetVarValueBool( "Viewport.Fullscreen", false );
		int  Frequency    = Env->Console->GetVarValueInt( "Viewport.Frequency", 60 );
		int  BitsPerPixel = Env->Console->GetVarValueInt( "Viewport.BitsPerPixel", 32 );
		int  zBufferBits  = Env->Console->GetVarValueInt( "Viewport.zBufferBits", 32 );
		int  StencilBits  = Env->Console->GetVarValueInt( "Viewport.StencilBits", 0 );
		int  Multisample  = Env->Console->GetVarValueInt( "Viewport.Multisample", 0 );

		if ( Fullscreen )
		{
			Env->Screen->SetDisplayMode( Width, Height, BitsPerPixel, Frequency );
		}

		LWindowHandle WndHandle = Utils::ultimate_cast<LWindowHandle>( ExternalWndHandle );

		FIXME( "clean this (screensaver functionality)" )

		if ( CommandLine && !CommandLine->empty() )
		{
			Env->Logger->Log( L_NOTICE, "CommandLine: " + CommandLine->at(0) );

			if ( CommandLine->at(0).length() > 2 )
			{
				if ( CommandLine->at(0).at( 0 ) == '/' && CommandLine->at(0).at( 1 ) == 'p' )
				{
					const LString HandleStr = CommandLine->at(0).substr( 2, CommandLine->at(0).length() - 2 );

					Env->Logger->Log( L_NOTICE, "Handle: " + HandleStr );

					WndHandle = Utils::ultimate_cast<LWindowHandle>( LStr::ToInt( HandleStr ) );
				}
			}
		}

		Viewport = Env->Screen->OpenViewport( "Linderdaum", WndHandle, ENGINE_NAME, Width, Height, Fullscreen, true, true, false );

		if ( CreateRenderer )
		{
			Env->Renderer->InitContext( Viewport, BitsPerPixel, zBufferBits, StencilBits, Multisample, false );
		}
	}
	else
	{
		if ( CreateRenderer )
		{
			Viewport = Env->Screen->OpenViewport( "Linderdaum", 0, ENGINE_NAME, 0, 0, false, true, false, false );

			Env->Renderer->InitContext( Viewport, 32, 32, 0, 0, false );
		}
	}

	FEngineTimeVar = Env->Console->GetVar( "Engine.EngineTime" );

	unguard()
}

sEnvironment::sEnvironment(): sSysEnv(),
	Screen( NULL ),
	Renderer( NULL ),
	Console( NULL ),
	GUI( NULL ),
	World( NULL ),
	Audio( NULL ),
	Input( NULL ),
	Viewport( NULL ),
	Resources( NULL ),
	//
	FTimeQuantum( 0.04f ), // in seconds
	//
	FVideoEncoder( NULL ),
	FEngineTime( 0 ),
	FOldTime( 0 ),
	FNewTime( 0 ),
	FDeltaSeconds( 0 ),
	FTickCount( 0 ),
	FSecondStartTime( 0 ),
	FExecutionTime( 0 ),
	FNumUpdatesPerFrame( 0 ),
	FRequestingExit( false ),
	FSuspended( false ),
	// additional event handler
	FExternalInitializer( NULL ),
	FExternalFinalizer( NULL ),
	//
	FLogLevel( L_DEBUG )
#if defined( OS_ANDROID )
	, FUseUnpackedResources( false )
#endif
#if !defined( OS_ANDROID )
	// we have static field for Android assigned in LAndroid.cpp
	, FLocaleName( "en" )
#endif
{
	Env = this;

	SetObjectID( "sEnvironment" );
};

sEnvironment::~sEnvironment()
{
}

void sEnvironment::SetUseUpackedResources()
{
#if defined( OS_ANDROID )
	FUseUnpackedResources = true;
#endif
}

bool sEnvironment::IsPaused() const
{
	return FEnginePaused->GetBool();
}

void sEnvironment::Pause()
{
	SendAsync( L_EVENT_PAUSE, LEventArgs( true ), false );
}

void sEnvironment::Resume()
{
	SendAsync( L_EVENT_PAUSE, LEventArgs( false ), false );
}

float sEnvironment::GetTimeQuantum() const
{
	return FTimeQuantum;
}

void sEnvironment::SetTimeQuantum( float TimeQuantum )
{
	FTimeQuantum = TimeQuantum;
}

int sEnvironment::GetMaxTickRate() const
{
	const int MAX_TICKRATE = 1000;

	return MAX_TICKRATE;
}

void sEnvironment::SetExternalInitializer( LEventCallback Proc )
{
	FExternalInitializer = Proc;
}

void sEnvironment::SetExternalFinalizer( LEventCallback Proc )
{
	FExternalFinalizer = Proc;
}

/**
   FileSystem initialization notes:

   Root folder is the folder where engine can create temp files. The folders path setup is performed as follows:

      - on Windows:
         1. root folder is set to DEFAULT_ENGINE_ROOT_DIR
         2. CommonMediaDir is mounted to the root

      - on Android:
         1. root folder is set to FCacheDir (comes from JNI, something like /cache/com.linderdaum.engine.yourproject/cache)
         2. FExternalDir/CommonMedia.tar is mounted to the root
         3. FExternalDir/Data is mounted to the root

**/
void sEnvironment::DeployDefaultEnvironment( const std::vector<LString>* CommandLine, const LString& CommonMediaDir )
{
#if defined( OS_ANDROID )
	DeployEnvironment( NULL,
	                   FCacheDir + "/engine.log",
	                   FCacheDir,
	                   FUseUnpackedResources ? ( FExternalDir + "/CommonMedia.tar" ) : FAPKFullName,
	                   "",
	                   true,
	                   true,
	                   true,
	                   &FAndroidViewportSize );
#else
	DeployEnvironment( CommandLine,
	                   DEFAULT_ENGINE_LOG_FILE,
	                   DEFAULT_ENGINE_ROOT_DIR,
	                   CommonMediaDir,
	                   DEFAULT_ENGINE_INI_FILE,
	                   true,
	                   true,
	                   false,
	                   NULL );
#endif
}

void sEnvironment::DeployEnvironment( const std::vector<LString>* CommandLine,
                                      const LString& LogFileName,
                                      const LString& RootDir,
                                      const LString& CommonMediaDir,
                                      const LString& ConfigFile,
                                      const bool OpenViewport,
                                      const bool CreateRenderer,
                                      const bool ContextTakeover,
                                      void* ExternalWndHandle )
{
	InitializeLowLevel();

	double StartupTime = GetSeconds();

	Logger->SetLogLevel( FLogLevel );
	Logger->EnableFileLogging( LogFileName );

	clExecutionThread::ClearExecutionStats();

	FileSystem->SetRootDir( RootDir.empty() ? "." : RootDir );

	if ( !CommonMediaDir.empty() )
	{
		if ( FileSystem->FileExistsPhys( CommonMediaDir ) ) { FileSystem->Mount( CommonMediaDir ); }
	}

#if defined( OS_ANDROID )

	if ( FUseUnpackedResources )
	{
		// load from SD card
		FileSystem->Mount( FExternalDir + "/Data" );
	}
	else
	{
		// load from .apk
		Logger->LogP( L_DEBUG, "APKPath=%s", FAPKFullName.c_str() );

//		Already mounted as CommonMedia
//		FileSystem->Mount( FAPKFullName );
		FileSystem->AddAliasMountPoint( FAPKFullName, "assets/Data" );
	}

#endif

	Localizer = Linker->Instantiate( "clLocalizer" );
	Console = Linker->Instantiate( "clConsole" );
	Console->SendCommand( "exec " + ConfigFile );

	Resources = Linker->Instantiate( "clResourcesManager" );

	Screen   = Linker->Instantiate( "clScreen" );
	Renderer = NULL;

	if ( OpenViewport && CreateRenderer ) { Renderer = Linker->Instantiate( Console->GetVarValueStr( "Renderer.Renderer", "clGLRenderContext" ) ); }

#ifdef OS_ANDROID
	Audio = Linker->Instantiate( "clAudioSubSystem_OpenAL" );
#else
	Audio = Linker->Instantiate( Console->GetVarValueStr( "Audio.AudioSubSystem", "iAudioSubSystem" ) );
#endif

	Math::Randomize( static_cast<int>( GetSeconds() * 1000.0f ) );

	Initialize( ContextTakeover, OpenViewport, CreateRenderer, CommandLine, ExternalWndHandle );

	if ( OpenViewport && CreateRenderer )
	{
		GUI = Linker->Instantiate( "clGUIManager" );
	}

	InitializeNetwork();

	if ( OpenViewport )
	{
		Input = Linker->Instantiate( "LWinDeviceList" );

		Input->SetWindowHandle( reinterpret_cast<LWindowHandle>( Viewport->GetWindow() ) );

		Input->ReenumerateDevices();
	}

	if ( OpenViewport && CreateRenderer )
	{
		World = Linker->Instantiate( "clWorld" );
	}

	// from here we can initialize all other high-level subsystems
	SendSync( L_EVENT_POSTINIT, LEventArgs(), false );

	Console->SendCommand( "ShowLogo Linderdaum startup..." );
	Console->SendCommand( "Bind VK_ESCAPE Quit" );
	Console->SendCommand( "Bind VK_W \"Toggle WireFrame\"" );
	Console->SendCommand( "Bind VK_TAB ToggleConsole" );

	// report startup time
	StartupTime = GetSeconds() - StartupTime;
	Logger->Log( L_LOG,    "Startup took " + LStr::ToStr( StartupTime, 3 ) + " seconds" );
	Logger->Log( L_NOTICE, "clDeployer::DeployEnvironment()->Environment deployed" );

	FEnginePaused = Env->Console->GetVar( "EnginePaused" );
	FEnginePaused->SetBool( false );
}

void sEnvironment::ShutdownEnvironment()
{
	Resources->StopLoaderThread();

	Console->SendCommand( "ResetGUI" );

	Console->SendCommand( "StopWebServers" );

	DisconnectAll();

	if ( FVideoEncoder )
	{
		delete( FVideoEncoder );
	}

	Env->Console->UnRegisterCommand( "StopVideoRec",     Utils::Bind( &sEnvironment::StopVideoRecC, this ) );
	Env->Console->UnRegisterCommand( "StartVideoRec",    Utils::Bind( &sEnvironment::StartVideoRecC, this ) );
	Env->Console->UnRegisterCommand( "SetViewportTitle", Utils::Bind( &sEnvironment::SetViewportTitleC, this ) );

	ShutdownNetwork();

	Utils::PostClean( World );
	Utils::PostClean( GUI );
	Utils::PostClean( Audio );

	Resources->PurgeAll();

	// OGG-streaming can be shutdown correctly only after all data providers were cleaned in resources
	clWaveDataProvider_OGG::ShutdownOGG();

	Env->Logger->Log( L_DEBUG, "Shutting down high-level subsystems" );

	Utils::PostClean( Resources );

	Env->Logger->Log( L_DEBUG, "Destroying renderer" );

	/// There can be no renderer for console apps. The explicit Canvas deinitialization here makes no sense
	if ( Env->Renderer ) { Env->Renderer->GetCanvas()->Cleanup(); }

	Utils::PostClean( Renderer );

	Env->Logger->Log( L_DEBUG, "Closing screen and input" );

	Utils::PostClean( Viewport );
	Utils::PostClean( Screen );
	Utils::PostClean( Input );
	Utils::PostClean( Console );
	Utils::PostClean( Localizer );

	Env->Logger->Log( L_DEBUG, "Unloading external dlls" );

	FreeImage_Unload();

	Env->Logger->Log( L_DEBUG, "Done high-level shutdown" );

	DeinitializeLowLevel();
}

void sEnvironment::HandleInput( bool ShowCommandLine, LString& Input )
{
	if ( Screen->IsFullscreen() ) { return; }

#if !defined(NO_CONSOLE_WINDOW)

	// kbhit() and getch() are emulated through direct console access on linux
	if ( kbhit() )
	{
		if ( ShowCommandLine )
		{
//			std::cout << ">" << Input;
			ShowCommandLine = false;
		}

		char Ch = ( char )getch();

		if ( Ch == 13 )
		{
			Console->SendCommand( Input );

			std::cout << "cmd : " << Input << std::endl;

			Input.clear();
		}
		else
		{
			std::cout << Ch;
			Input.push_back( Ch );
		}
	}

#endif
}

void sEnvironment::RunApplication( const LString& ConsoleCfg )
{
	// autoexecute startup script
	if ( !ConsoleCfg.empty() )
	{
		Console->SendCommand( "exec " + ConsoleCfg );
	}

	// preliminary initialization (goes after "Mount" instructions)

	// override locale from cvar, if any
	if ( clCVar* Locale = Console->FindVar( "Env.Locale" ) )
	{
		FLocaleName = Locale->GetString();
	}

	Localizer->SetLocale( FLocaleName );

	/////// Run the application ///////

	if ( FExternalInitializer )
	{
		FExternalInitializer();
	}

	LString Input;
	bool    ShowCommandLine = true;

	SendSync( L_EVENT_BEGINPLAY, LEventArgs(), false );

	// ping everything
	SendSync( L_EVENT_TIMER, LEventArgs( 0.0f ), false );

#if !defined( OS_ANDROID )

	while ( !IsRequestingExit() )
	{
		if ( Viewport && !Viewport->DispatchEvents() )
		{
			RequestExit();
		}

		GenerateTicks();

		HandleInput( ShowCommandLine, Input );
	}

	if ( FExternalFinalizer )
	{
		FExternalFinalizer();
	}

	Console->SendCommand( "ShowLogo Shutting down..." );

	Logger->Log( L_LOG, "Shutting down..." );
#endif
}

void sEnvironment::SetClipboardText( const LString& str ) const
{
#ifdef OS_WINDOWS
	HGLOBAL hString = ::GlobalAlloc( GMEM_MOVEABLE, sizeof( char ) * ( str.length() + 1 ) );

	if ( hString )
	{
		if ( ::OpenClipboard ( NULL /*hWindow*/  ) )
		{
			LPVOID szString = ::GlobalLock ( hString );

			if ( szString )
			{
				memcpy( szString, str.c_str(), sizeof( char ) * ( str.length() + 1 ) );
				::GlobalUnlock ( hString );
				::EmptyClipboard();
				::SetClipboardData( CF_TEXT, hString );
			}

			::CloseClipboard();
		}

		::GlobalFree( hString );
	}

#endif
}

void sEnvironment::ClearClipboard() const
{
#ifdef OS_WINDOWS

	if ( ::OpenClipboard ( NULL ) ) { ::EmptyClipboard(); }

#endif
}

LString sEnvironment::GetClipboardText() const
{
	LString str( "" );
#ifdef OS_WINDOWS

	if ( ::OpenClipboard ( NULL /* hWindow is optional here */ ) )
	{
		HGLOBAL hString = ::GetClipboardData( CF_TEXT );

		if ( hString )
		{
			str.assign( ( char* )::GlobalLock( hString ) );
			::GlobalUnlock( hString );
		}
	}

	::CloseClipboard();
#endif
	return str;
}

bool sEnvironment::IsClipboardTextAvailable() const
{
#ifdef OS_WINDOWS
	return ::IsClipboardFormatAvailable( CF_TEXT );
#else
	return false;
#endif
}

double sEnvironment::GetCurrentTickRate() const
{
	return FFPS->GetDouble();
}

void sEnvironment::SetCurrentTickRate( const double TickRate )
{
	FFPS->SetDouble( TickRate );
}

double sEnvironment::GetEngineTime() const
{
	return FEngineTime;
}

double sEnvironment::GetDeltaSeconds() const
{
	return FDeltaSeconds;
}

void sEnvironment::SleepSeconds( double Amount ) const
{
	double Seconds = GetSeconds();

	while ( GetSeconds() - Seconds <= Amount )
	{
		ReleaseTimeslice( 1 );
	}
}

void sEnvironment::RenderFrame( float DeltaSeconds, int UpdatesPerFrame )
{
//	CheckRendererError( "" );

	if ( Env->Renderer )
	{
		Env->Renderer->BeginFrame();

		// render each subsystem
		if ( DeltaSeconds != 0 || UpdatesPerFrame != 0 )
		{
			Env->SendSync( L_EVENT_DRAWOVERLAY, LEventArgs( DeltaSeconds ), false );
		}
		else
		{
			if ( Env->GUI ) { Env->GUI->Event_DRAWOVERLAY( L_EVENT_DRAWOVERLAY, LEventArgs( DeltaSeconds ) ); }
		}

		// swap buffers
		Env->Renderer->EndFrame( true );

		// write another video frame, if the capture is running
		ProcessVideoCapture();
	}
	else
	{
		// just send the event
		if ( DeltaSeconds != 0 || UpdatesPerFrame != 0 )
		{
			Env->SendSync( L_EVENT_DRAWOVERLAY, LEventArgs( DeltaSeconds ), false );
		}
	}
}

Luint64 sEnvironment::GetCurrentTick() const
{
	return FCurrentTick;
}

void sEnvironment::GenerateTicks()
{
	guard();

	// update time
	FNewTime = Env->GetSeconds();
	float DeltaSeconds = static_cast<float>( FNewTime - FOldTime );
	FDeltaSeconds = DeltaSeconds;
	FOldTime = FNewTime;
	FTickCount++;
	FCurrentTick++;

	FUpdatesPerFrame->SetInt( FNumUpdatesPerFrame );

	if ( !IsSuspended() )
	{
		RenderFrame( DeltaSeconds, FNumUpdatesPerFrame );
	}

	FNumUpdatesPerFrame = 0;

	const float TIME_QUANTUM = GetTimeQuantum();
	const float MAX_EXECUTION_TIME = 10.0f * TIME_QUANTUM;

	// update world
	FEngineTime += DeltaSeconds;
	FEngineTimeVar->SetDouble( FEngineTime );

	if ( !IsSuspended() )
	{
		/// Increase execution time counter (only in non-suspened mode, to avoid problems after Unsuspend() )
		FExecutionTime += DeltaSeconds;

		/// execution limit
		FExecutionTime = Math::Clamp( FExecutionTime, 0.0f, MAX_EXECUTION_TIME );

//		Env->Logger->LogP( L_DEBUG, "FExecutionTime = %f", FExecutionTime );

		/// Generate internal time quanta
		while ( FExecutionTime > TIME_QUANTUM )
		{
			FExecutionTime -= TIME_QUANTUM;    // devour this timeslice
			FNumUpdatesPerFrame++;
			// ping everything
			Env->SendSync( L_EVENT_TIMER, LEventArgs( TIME_QUANTUM ), false );
			Env->DemultiplexEvents( Env->GetSeconds() );
		}
	}

	if ( IsPaused() || IsSuspended() )
	{
		Env->ReleaseTimeslice( 10 );   // don't waste system resources
	}

	// update FPS
	if ( FOldTime > FSecondStartTime + 0.1 )
	{
		SetCurrentTickRate( Utils::Max( FTickCount / ( FOldTime - FSecondStartTime ), 0.001 ) );
		FSecondStartTime = FOldTime;
		FTickCount = 0;
	}

	// enforce optional tick rate limit
	/*
   if ( double MaxTickRate = GetMaxTickRate() )
   {
      double Delta = ( 1.0 / MaxTickRate ) - ( GetSeconds()-FOldTime );

      ReleaseTimeslice( static_cast<int>( (Utils::Max(0.0, Delta) * 1000.0 ) ) );
   }
	*/
	unguard();
}

void sEnvironment::ReleaseTimeslice( int Milliseconds ) const
{
#if defined OS_WINDOWS
	Sleep( Milliseconds );
#else
	// mu-sleep supports microsecond-precision
	usleep( static_cast<useconds_t>( Milliseconds ) * 1000 );
#endif
}

void sEnvironment::SetViewportTitleC( const LString& Param )
{
	if ( Param.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: SetViewportTitle <Title>" );
		return;
	}

	Viewport->SetViewportTitle( Param );
}

void sEnvironment::StartVideoRecC( const LString& Param )
{
	FRecordScreenshotSequence->SetBool( false );

	if ( FVideoEncoder )
	{
		delete( FVideoEncoder );
	}

#ifdef OS_WINDOWS
	FVideoEncoder = Env->Linker->Instantiate( "clVFWVideoEncoder" );

	if ( !FVideoEncoder->CreateStream( Param,
	                                   Viewport->GetWidth(),
	                                   Viewport->GetHeight(),
	                                   15 ) ) // constant 15 frames per second
	{
		delete( FVideoEncoder );

		FVideoEncoder = NULL;
	}

#endif
}

void sEnvironment::StopVideoRecC( const LString& Param )
{
	if ( FVideoEncoder )
	{
		delete( FVideoEncoder );
	}

	FVideoEncoder = NULL;
}

void sEnvironment::ProcessVideoCapture()
{
	if ( FVideoEncoder )
	{
		Lubyte* RawBGRBitmap = static_cast<Lubyte*>( malloc( Env->Renderer->GetScreenshotSize() ) );

		Env->Renderer->GetScreenshot( RawBGRBitmap );

		FVideoEncoder->AddVideoFrame( RawBGRBitmap );

		free( RawBGRBitmap );
	}
	else if ( FRecordScreenshotSequence->GetBool() )
	{
		Env->Console->SendCommand( "SaveScreenshot" );
	}
}

void sEnvironment::EnableMemoryLeaksDetection( int BlockIndex )
{
#if defined( PLATFORM_MSVC )
	// enable memory leaks detection
#if !defined(NDEBUG)
	HANDLE hLogFile = CreateFile( "log.txt", GENERIC_WRITE, FILE_SHARE_WRITE,
	                              NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#endif

	_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_WNDW  | _CRTDBG_MODE_DEBUG );
	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE   | _CRTDBG_MODE_DEBUG );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE  | _CRTDBG_MODE_WNDW  | _CRTDBG_MODE_DEBUG );

	_CrtSetReportFile( _CRT_ASSERT, hLogFile );
	_CrtSetReportFile( _CRT_WARN,   hLogFile );
	_CrtSetReportFile( _CRT_ERROR,  hLogFile );

	int tmpDbgFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
	tmpDbgFlag |= _CRTDBG_ALLOC_MEM_DF;
	tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag( tmpDbgFlag );

	if ( BlockIndex > 0 )
	{
		_CrtSetBreakAlloc( BlockIndex );
	}

#endif
}

void sEnvironment::AddToObjectsGraph( iObject* Obj )
{
	LMutex Lock( &FObjectsGraphMutex );

	Env->ObjectsGraph.push_back( Obj );
}

void sEnvironment::RemoveFromObjectsGraph( iObject* Obj )
{
	LMutex Lock( &FObjectsGraphMutex );

	Env->ObjectsGraph.remove( Obj );
}

/*
 * 25/04/2011
     Localizer initialization and shutdown
 * 07/11/2010
     Input system shutdown call
 * 24/06/2010
     Merged with clEngine
 * 31/03/2010
     kbhit()/getch() for linux
 * 26/03/2010
     Timing routines for linux, from http://forums.tigsource.com/index.php?topic=6230.0
 * 15/05/2009
     LoadDynamicLib, DispatchEvents, SetHighDPIAware moved here from SystemLayer
 * 26/04/2009
     It's here
*/

