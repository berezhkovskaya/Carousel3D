/**
 * \file Environment.h
 * \brief Engine environment
 *
 * Engine initialization and shutdown procedures
 *
 * \version 0.5.99c
 * \date 25/04/2011
 * \author Sergey Kosarevsky, 2007-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LEnvironment_
#define _LEnvironment_

#include "SysEnv.h"
#include "Core/Logger.h"

#include <list>

class clCVar;
class clScreen;
class iRenderContext;
class clConsole;
class clGUIManager;
class clWorld;
class iAudioSubSystem;
class clLibrary;
class iInputDeviceList;
class iVideoEncoder;
class clViewport;
class clResourcesManager;
class clLocalizer;

/*! \mainpage Linderdaum Engine Documentation
 *
 * \section intro_sec Introduction
 *
 * The Linderdaum Engine is an open source purely object-oriented 3D gaming engine written in C++. It is designed to be not only the class library, but also an integrated out-of-the box solution for the development of interactive 3D application.
 *
 *
 * \section tutorials_sec Tutorials
 *
 * Different tutorials are available at: http://www.linderdaum.com/home/index.php?title=Tutorials
 */

/// Engine entry point
class scriptfinal sEnvironment : public sSysEnv, public iAsyncQueue
{
public:
	typedef void ( *LEventCallback )( void );
public:
	sEnvironment();
	virtual ~sEnvironment();

	NET_EXPORTABLE()

	//
	// sEnvironment
	//

	/**
	  Deploys all engine's subsystems using default parameters.

	  \param CommandLine - application's command line
	  \param CommonMediaDir - an OS path to the engine's "CommonMedia" folder (mounted before startup)
	**/
	virtual void    DeployDefaultEnvironment( const std::vector<LString>* CommandLine, const LString& CommonMediaDir );

	/**
	  Deploys all engine's subsystems. This call is a shortcut and should not be replaced
	  by a custom deployment process.

	   \param CommandLine - application's command line
	   \param LogFileName - an OS path to a log file, empty string will disable logging
	   \param RootDir - an OS path to the engine's "Data" folder
	   \param CommonMediaDir - an OS path to the engine's "CommonMedia" folder (mounted before startup)
	   \param ConfigFile - a path to an "Engine.ini" file relative to RootDir
	   \param OpenViewport - create new window
	   \param CreateRenderer - create rendering context (FIXME: there once were the flags for GUI etc. TODO : split initialization to 'low-' and 'high-' levels and split Environment)
	   \param ContextTakeover - use existing rendering context created in current application
	   \param ExternalWndHandle - handle to a window to render into
	**/
	virtual void    DeployEnvironment( const std::vector<LString>* CommandLine,
	                                   const LString& LogFileName,
	                                   const LString& RootDir,
	                                   const LString& CommonMediaDir,
	                                   const LString& ConfigFile,
	                                   const bool OpenViewport,
	                                   const bool CreateRenderer,
	                                   const bool ContextTakeover,
	                                   void* ExternalWndHandle );

	/// Shutdowns specified environment. Order of shutdown does matter.
	virtual void    ShutdownEnvironment();

	/// Set user-defined initialization routine
	virtual void    SetExternalInitializer( LEventCallback Proc );

	/// Set user-defined deinitialization routine
	virtual void    SetExternalFinalizer( LEventCallback Proc );

	/// Internal initialization. Can be used for lazy initialization from a console application
	virtual void    Initialize( bool ContextTakeover,
	                            bool OpenViewport,
	                            bool CreateRenderer,
	                            const std::vector<LString>* CommandLine,
	                            void* ExternalWndHandle );

	/**
	  Default main loop

	  \param ConsoleCfg - name of console command file to be executed before main loop start
	**/
	virtual void    RunApplication( const LString& ConsoleCfg );

	/// Internal utility function to handle user input in console
	virtual void    HandleInput( bool ShowCommandLine, LString& Input );

	/// Pause current thread for 'Amount' msecs
	virtual void    SleepSeconds( double Amount ) const;

	/// Waste some time
	virtual void    ReleaseTimeslice( int Milliseconds ) const;

	/// Turn on memory leaks detection (VisualC++ only)
	virtual void    EnableMemoryLeaksDetection( int BlockIndex );

	/// get the time quantum for subsystems updating
	virtual float   GetTimeQuantum() const;
	virtual int     GetMaxTickRate() const;

	/// set the time quantum for subsystems updating
	virtual void    SetTimeQuantum( float TimeQuantum );

	/// Enter 'pause' mode - no internal ticks are performed, but the Draw events are generated
	scriptmethod void Pause();

	/// Unpause the engine
	scriptmethod void Resume();

	/// Check if the engine is paused
	scriptmethod bool IsPaused() const;

	/// Stop any event processing (zero-cpu mode)
	scriptmethod void Suspend()     { FSuspended = true; }

	/// Check if this environment is suspended
	scriptmethod bool IsSuspended() { return FSuspended; }

	/// Resume event processing
	scriptmethod void Unsuspend() { FSuspended = false; }

	/// Main thread termination flag
	scriptmethod bool IsRequestingExit() const   { return FRequestingExit; }

	/// Set termination flag to stop the RunApplication() loop
	scriptmethod void RequestExit() { FRequestingExit = true; }

	virtual void            GenerateTicks();
	virtual void            RenderFrame( float DeltaSeconds, int UpdatesPerFrame );
	virtual Luint64         GetCurrentTick() const;
	virtual double          GetEngineTime() const;
	virtual double          GetCurrentTickRate() const;
	virtual double          GetDeltaSeconds() const;
	virtual void            SetCurrentTickRate( const double TickRate );

#pragma region Clipboard access (WinAPI only)
	scriptmethod void       SetClipboardText( const LString& Str ) const;
	scriptmethod LString    GetClipboardText() const;
	scriptmethod void       ClearClipboard() const;
	scriptmethod bool       IsClipboardTextAvailable() const;
#pragma endregion

#pragma region Startup parameters configuration (should be set before call to DeployEnvironment/DeployDefaultEnvironment)
	/// log messagess only with log level equal or higher will be put into Engine.log
	scriptmethod void       SetLogLevel( LLogLevel LogLevel ) { FLogLevel = LogLevel; };
	/// Get current log level
	scriptmethod LLogLevel  GetLogLevel() const { return FLogLevel; }
	/// 2-char locale (language) ID, used to initialize localizer
	scriptmethod void       SetLocaleName( const LString& LocaleName ) { FLocaleName = LocaleName; };
	/// On Android all resources will not be loaded from .apk but from external SD folder
	scriptmethod void       SetUseUpackedResources();
#pragma endregion

#pragma region Properties
	/* Property(Description="Native viewport", Category="Subobjects", Name=Viewport, Type=clViewport, FieldName=Viewport) */

	/* Property(Description="Current engine time in seconds", Category="State", Type=double, Name=Time, Getter=GetEngineTime ) */
	/* Property(Description="Current time quantum", Category="Parameters", Type=float, Name=TimeQuantum, Getter=GetTimeQuantum ) */
	/* Property(Description="Current update rate", Category="State", Type=double, Name=DeltaSeconds, Getter=GetDeltaSeconds ) */
	/* Property(Description="Maximum ticking rate", Category="Parameters", Type=int, Name=MaxTickRate, Getter=GetMaxTickRate ) */
	/* Property(Description="Current ticking rate", Category="State", Type=double, Name=TickRate, Getter=GetCurrentTickRate, Setter=SetCurrentTickRate ) */

	/* Pro_perty(Description="Is the engine active", Category="State", Name=Active, Type=bool, Getter=IsActive) */
	/* Pro_perty(Description="Is the engine paused", Category="State", Name=Paused, Type=bool, Getter=IsPaused) */
	/* Pro_perty(Description="Is the engine pending", Category="State", Name=Pending, Type=bool, Getter=IsPending) */
	/* Property(Description="Rendering context", Category="Core", Name=Renderer, Type=iRenderContext, FieldName=Renderer) */
	/* Property(Description="Console", Category="Core", Name=Console, Type=clConsole, FieldName=Console) */
	/* Property(Description="Resources manager", Category="Core", Name=Resources, Type=clResourcesManager, FieldName=Resources) */
	/* Property(Description="Input system", Category="Core", Name=Input, Type=iInputDeviceList, FieldName=Input) */
	/* Property(Description="Audio system", Category="Core", Name=Audio, Type=iAudioSubSystem, FieldName=Audio) */
	/* Property(Description="GUI", Category="Core", Name=GUI, Type=clGUIManager, FieldName=GUI) */
#pragma endregion

#pragma region Events
	FWD_EVENT_HANDLER( Event_BEGINPLAY );
	FWD_EVENT_HANDLER( Event_PAUSE );
#pragma endregion

private:
#pragma region Commands handlers
	virtual void     SetViewportTitleC( const LString& Param );
	virtual void     StartVideoRecC( const LString& Param );
	virtual void     StopVideoRecC( const LString& Param );
#pragma endregion

private:
	friend class iObject;
	void    AddToObjectsGraph( iObject* Obj );
	void    RemoveFromObjectsGraph( iObject* Obj );
	clMutex FObjectsGraphMutex;

	/// Grab another video frame
	void    ProcessVideoCapture();

	iVideoEncoder*     FVideoEncoder;

	/// Set to 'true' if are about to exit the application
	bool               FRequestingExit;

	/// Set to 'true' if no internal updates/repaints and event handling are done
	bool               FSuspended;

	double             FEngineTime;
	double             FOldTime;
	double             FNewTime;
	double             FDeltaSeconds;
	double             FSecondStartTime;
	Luint64            FTickCount;
	Luint64            FCurrentTick;
	float              FExecutionTime;

	clCVar*            FEnginePaused;
	clCVar*            FFPS;
	clCVar*            FEngineTimeVar;
	clCVar*            FUpdatesPerFrame;
	int                FNumUpdatesPerFrame;

	/// External initialization done in RunApplication
	LEventCallback     FExternalInitializer;

	/// External deinitialization done right after Main loop
	LEventCallback     FExternalFinalizer;

#pragma region Startup parameters
	LLogLevel          FLogLevel;
#pragma endregion

protected:
	clCVar*            FRecordScreenshotSequence;

public:

	/// OS'es desktop screen
	nativefield    clScreen*               Screen;

	/// rendering API interface and context
	nativefield    iRenderContext*       Renderer;

	/// console
	nativefield    clConsole*             Console;

	/// internal graphical user interface
	nativefield    clGUIManager*              GUI;

	/// game world
	nativefield    clWorld*                 World;

	/// audio subsystem
	nativefield    iAudioSubSystem*         Audio;

	/// input devices list
	nativefield    iInputDeviceList*        Input;

	/// OS-specific viewport
	nativefield    clViewport*           Viewport;

	/// resources manager
	nativefield    clResourcesManager*  Resources;

	/// localization information
	nativefield    clLocalizer*         Localizer;

	/// defenes how often L_EVENT_TIMER is sent - the global update speed of all subsystems, in seconds
	float FTimeQuantum;

#if defined( OS_ANDROID )
	static LString    FCacheDir;
	static LString    FExternalDir;
	static LString    FAPKFullName;
	/// 2-char locale (language) ID, used to initialize localizer
	static LString    FLocaleName;
	/// package public key
	static LString    FKeyValue;
	/// file name can be obtained from an Intent that started our activity
	static LString    FCommandLine;

	static struct sAndroid_ViewportSize
	{
		int FWidth;
		int FHeight;
		LVector2i FDPI;
	} FAndroidViewportSize;

	bool FUseUnpackedResources;
#else
	/// 2-char locale (language) ID, used to initialize localizer
	LString           FLocaleName;
#endif
};

/// can be used to instantiate iObjects, however Env->Linker is preferred
template <typename T> inline T* Construct( sEnvironment* Env )
{
	T* Result = new T();
	Result->Env = Env;
	Result->AfterConstruction();

	return Result;
}

#endif

/*
 * 25/04/2011
     Localizer added
 * 24/06/2010
     Merged with clEngine
 * 15/05/2009
     Some functionality from SystemLayer moved here
 * 26/04/2009
     Merged with clDeployer
 * 21/04/2009
     ResourcesManager renamed to Resources
 * 03/03/2009
     Network
 * 24/12/2007
     It's here
*/
