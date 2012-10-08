/**
 * \file Console.cpp
 * \brief Virtual console
 * \version 0.5.98
 * \date 01/12/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Console.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/CVars.h"
#include "Core/Linker.h"
#include "Core/ScriptCompiler.h"
#include "Core/Logger.h"
#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"
#include "Resources/ResourcesManager.h"
#include "Core/VFS/iIStream.h"
#include "Core/VFS/iOStream.h"

#include "LKeys.h"
#include "LColors.h"

#include <algorithm>

const LString HistoryFileName = "console.history";

void clConsole::clCommandsHistory::LoadFromStream( iIStream* IStream )
{
	while ( !IStream->Eof() )
	{
		LString Cmd = IStream->ReadLine();

		PutCommand( Cmd );
	}
}

void clConsole::clCommandsHistory::SaveToStream( iOStream* OStream )
{
	for ( int i = GetCommandsCount(); i != 0; --i )
	{
		OStream->WriteLine( GetCommand( i ) );
	}
}

clConsole::clConsole(): FKeyBindings(),
	FMessagesHistory(),
	FCommandsQueue(),
	FCommandsHistory( new clConsole::clCommandsHistory() ),
	FHUDVisible( false ),
	FTimeVisible( 0 ),
	FSendCommandResult( "" ),
	FScriptCompiler( NULL )
{
	memset( FKeyPressed, 0, sizeof( FKeyPressed ) );
}

bool clConsole::IsKeyPressedAny()
{
	for ( size_t i = 0; i != 256; ++i )
	{
		if ( FKeyPressed[i] ) { return true; }
	}

	return false;
}

void clConsole::Event_KEY( LEvent Event, const LEventArgs& Args )
{
	int Key      = static_cast<int>( Args.FFloatArg );
	int KeyState = Args.FBoolArg;

	FKeyPressed[Key] = KeyState;
}

void clConsole::AfterConstruction()
{
	RegisterCommand( "Quit",             Utils::Bind( &clConsole::QuitC,          this ) );
	RegisterCommand( "Exit",             Utils::Bind( &clConsole::QuitC,          this ) );
	RegisterCommand( "Exec",             Utils::Bind( &clConsole::ExecC,          this ) );
	RegisterCommand( "Bind",             Utils::Bind( &clConsole::BindC,          this ) );
	RegisterCommand( "Alias",            Utils::Bind( &clConsole::AliasC,         this ) );
	RegisterCommand( "ToggleConsole",    Utils::Bind( &clConsole::ToggleConsoleC, this ) );
	RegisterCommand( "Mount",            Utils::Bind( &clConsole::MountC,         this ) );
	RegisterCommand( "Clear",            Utils::Bind( &clConsole::ClearC,         this ) );
	RegisterCommand( "Version",          Utils::Bind( &clConsole::VersionC,       this ) );
	RegisterCommand( "Help",             Utils::Bind( &clConsole::HelpC,          this ) );
	RegisterCommand( "CmdList",          Utils::Bind( &clConsole::HelpC,          this ) );
	RegisterCommand( "TogglePause",      Utils::Bind( &clConsole::TogglePauseC,   this ) );
	RegisterCommand( "Toggle",           Utils::Bind( &clConsole::ToggleC,        this ) );
	RegisterCommand( "Set",              Utils::Bind( &clConsole::SetC,           this ) );
	RegisterCommand( "SetProperty",      Utils::Bind( &clConsole::SetPropertyC,   this ) );
	RegisterCommand( "Show",             Utils::Bind( &clConsole::ShowC,          this ) );
	RegisterCommand( "Menu",             Utils::Bind( &clConsole::MenuC,          this ) );
	RegisterCommand( "SaveVars",         Utils::Bind( &clConsole::SaveVarsC,      this ) );
	RegisterCommand( "RefreshScreen",    Utils::Bind( &clConsole::RefreshScreenC, this ) );
	RegisterCommand( "Sleep",            Utils::Bind( &clConsole::SleepC,         this ) );
	RegisterCommand( "SetLogLevel",      Utils::Bind( &clConsole::SetLogLevelC,   this ) );
	RegisterCommand( "PrintLogLevel",    Utils::Bind( &clConsole::PrintLogLevelC, this ) );

	RegisterCommand( "AddDirWatch",      Utils::Bind( &clFileSystem::AddDirWatchC,     Env->FileSystem ) );
	RegisterCommand( "AddFileWatch",     Utils::Bind( &clFileSystem::AddFileWatchC,    Env->FileSystem ) );
	RegisterCommand( "RemoveDirWatch",   Utils::Bind( &clFileSystem::RemoveDirWatchC,  Env->FileSystem ) );
	RegisterCommand( "RemoveFileWatch",  Utils::Bind( &clFileSystem::RemoveFileWatchC, Env->FileSystem ) );

	Env->Logger->Connect( L_EVENT_ERROR,       BIND( &clConsole::Event_ERROR ) );
	Env->Connect( L_EVENT_TIMER,       BIND( &clConsole::Event_TIMER ) );
	Env->ConnectWithPriority( L_EVENT_KEY,         BIND( &clConsole::Event_KEY         ),  -1 );

	FConsoleHUDFileName = GetVar( "GUI.ConsoleHUD" );
	FConsoleHUDFileName->SetString( "Interface/Console.gui" );

	FShouldSaveHistory = GetVar ( "Console.SaveHistory" );
	FShouldSaveHistory->SetBool( true );

	GetVarDefault( "Core.EngineVersion", ENGINE_VERSION );

	if ( Env->FileSystem->FileExists( HistoryFileName ) )
	{
		iIStream* Input = Env->FileSystem->CreateFileReader( HistoryFileName );

		FCommandsHistory->LoadFromStream( Input );

		delete( Input );
	}

	// instantiate script compiler
	// its initialization is delayed until some script is actually compiled
	FScriptCompiler = Env->Linker->Instantiate( "clScriptCompiler" );
}

clScriptCompiler* clConsole::GetScriptCompiler() const
{
	return FScriptCompiler;
}

clConsole::~clConsole()
{
	Env->DisconnectObject( this );
	Env->Logger->DisconnectObject( this );

	if ( FShouldSaveHistory->GetBool() )
	{
		iOStream* Output = Env->FileSystem->CreateFileWriter( HistoryFileName );

		FCommandsHistory->SaveToStream( Output );

		delete( Output );
	}

	// vars management
	Utils::DeallocateAll( FCVars );

	// free script compiler
	delete FScriptCompiler;
	//
	delete( FCommandsHistory );

	UnRegisterCommand( "Quit",             Utils::Bind( &clConsole::QuitC,          this ) );
	UnRegisterCommand( "Exec",             Utils::Bind( &clConsole::ExecC,          this ) );
	UnRegisterCommand( "Bind",             Utils::Bind( &clConsole::BindC,          this ) );
	UnRegisterCommand( "Alias",            Utils::Bind( &clConsole::AliasC,         this ) );
	UnRegisterCommand( "ToggleConsole",    Utils::Bind( &clConsole::ToggleConsoleC, this ) );
	UnRegisterCommand( "Mount",            Utils::Bind( &clConsole::MountC,         this ) );
	UnRegisterCommand( "Clear",            Utils::Bind( &clConsole::ClearC,         this ) );
	UnRegisterCommand( "Version",          Utils::Bind( &clConsole::VersionC,       this ) );
	UnRegisterCommand( "Help",             Utils::Bind( &clConsole::HelpC,          this ) );
	UnRegisterCommand( "CmdList",          Utils::Bind( &clConsole::HelpC,          this ) );
	UnRegisterCommand( "TogglePause",      Utils::Bind( &clConsole::TogglePauseC,   this ) );
	UnRegisterCommand( "Toggle",           Utils::Bind( &clConsole::ToggleC,        this ) );
	UnRegisterCommand( "SetProperty",      Utils::Bind( &clConsole::SetPropertyC,   this ) );
	UnRegisterCommand( "Set",              Utils::Bind( &clConsole::SetC,           this ) );
	UnRegisterCommand( "Show",             Utils::Bind( &clConsole::ShowC,          this ) );
	UnRegisterCommand( "Menu",             Utils::Bind( &clConsole::MenuC,          this ) );
	UnRegisterCommand( "SaveVars",         Utils::Bind( &clConsole::SaveVarsC,      this ) );
	UnRegisterCommand( "RefreshScreen",    Utils::Bind( &clConsole::RefreshScreenC, this ) );
	UnRegisterCommand( "Sleep",            Utils::Bind( &clConsole::SleepC,         this ) );
	UnRegisterCommand( "SetLogLevel",      Utils::Bind( &clConsole::SetLogLevelC,   this ) );
	UnRegisterCommand( "PrintLogLevel",    Utils::Bind( &clConsole::PrintLogLevelC, this ) );

	UnRegisterCommand( "AddDirWatch",      Utils::Bind( &clFileSystem::AddDirWatchC,     Env->FileSystem ) );
	UnRegisterCommand( "AddFileWatch",     Utils::Bind( &clFileSystem::AddFileWatchC,    Env->FileSystem ) );
	UnRegisterCommand( "RemoveDirWatch",   Utils::Bind( &clFileSystem::RemoveDirWatchC,  Env->FileSystem ) );
	UnRegisterCommand( "RemoveFileWatch",  Utils::Bind( &clFileSystem::RemoveFileWatchC, Env->FileSystem ) );
}

void clConsole::InitializeScript()
{
	// initialize script
	double ScriptCompileTime = Env->GetSeconds();

	FScriptCompiler->ClearSettings();

	// .rs-source was updated. recompile everything
	FScriptCompiler->AddExportDirectory( "RS/natives" );
	FScriptCompiler->AddLibDirectory( "RS/lib" );
	FScriptCompiler->SetOutputDirectory( "RS/classes" );
	FScriptCompiler->Reinitialize();

	// process the directory with source files
	FScriptCompiler->ProcessSourceDirectory( "RS/scripts", true );
	FScriptCompiler->RecompileClasses();
	FScriptCompiler->RecompileMethods();

	ScriptCompileTime = Env->GetSeconds() - ScriptCompileTime;

	Env->Logger->Log( L_LOG, "Script compilation took " + LStr::ToStr( ScriptCompileTime, 3 ) + " seconds" );
	Env->Logger->Log( L_LOG, "Reloading script directory" );

	double ClassesLoadTime = Env->GetSeconds();
	Env->Linker->LoadScriptDirectory( "RS/Classes" );
	ClassesLoadTime = Env->GetSeconds() - ClassesLoadTime;
	Env->Logger->Log( L_LOG, "Class/method reloading took " + LStr::ToStr( ClassesLoadTime, 3 ) + " seconds" );

	Env->Logger->Log( L_LOG, "Done reloading" );
}

void clConsole::SaveVarsC( const LString& Param )
{
	if ( Param.empty() )
	{
		DisplayInfoTip( "Usage: SAVEVARS <FileName>" );
		return;
	}

	iOStream* Output = Env->FileSystem->CreateFileWriter( Param );

	for ( size_t i = 0; i != FCVars.size(); ++i )
	{
		Output->WriteLine( FCVars[i]->GetObjectID() + " \"" + FCVars[i]->GetString() + "\"" );
	}

	delete( Output );

	DisplayInfoTip( LStr::ToStr( ( int )FCVars.size() ) + " vars were saved into " + Param );
}


#define DECLARE_DEFAULT_GET_METHOD(TypeName, RetTypeName, CTypeName, VARGetMethod, VARSetMethod) \
RetTypeName clConsole::GetVarValue##TypeName(const LString& VarName, const CTypeName Default) \
{                                           \
   clCVar* CVar = FindVar( VarName );       \
                                            \
   if ( CVar ) return CVar->VARGetMethod(); \
                                            \
   CVar = GetVar( VarName );                \
   CVar->VARSetMethod( Default );           \
                                            \
   return Default;                          \
}


DECLARE_DEFAULT_GET_METHOD( Float, float, float, GetFloat, SetFloat )
DECLARE_DEFAULT_GET_METHOD( Int, int, int, GetInt, SetInt )
DECLARE_DEFAULT_GET_METHOD( Str, LString, LString& , GetString, SetString )
DECLARE_DEFAULT_GET_METHOD( Bool, bool, bool , GetBool, SetBool )

#undef DECLARE_DEFAULT_GET_METHOD

clCVar* clConsole::GetVarDefault( const LString& VarName, const LString& Default )
{
	clCVar* CVar = FindVar( VarName );

	if ( CVar ) { return CVar; }

	CVar = GetVar( VarName );
	CVar->SetString( Default );

	return CVar;
}

clCVar* clConsole::GetVarDefaultFloat( const LString& VarName, float Default )
{
	clCVar* CVar = FindVar( VarName );

	if ( CVar ) { return CVar; }

	CVar = GetVar( VarName );
	CVar->SetFloat( Default );

	return CVar;
}

clCVar* clConsole::FindVar( const LString& VarName )
{
	LMutex Lock( &FCVarsAccessMutex );

	const LString VName = LStr::GetUpper( VarName );

	for ( size_t i = 0; i != FCVars.size(); ++i )
	{
		if ( FCVars[i]->GetObjectID() == VName ) { return FCVars[i]; }
	}

	return NULL;
}

clCVar* clConsole::GetVar( const LString& VarName )
{
	const LString VName = LStr::GetUpper( VarName );

	clCVar* CVar = FindVar( VarName );

	if ( !CVar )
	{
		LMutex Lock( &FCVarsAccessMutex );

		CVar = new clCVar( VarName );

		CVar->Env = Env;
		CVar->AfterConstruction();

		FCVars.push_back( CVar );
	}

	return CVar;
}

void clConsole::SetPropertyC( const LString& Param )
{
	LString ObjID = LStr::GetToken( Param, 1 );
	LString PropID = LStr::GetToken( Param, 2 );
	LString PropVal = LStr::GetToken( Param, 3 );

	if ( ObjID.empty() || PropID.empty() || PropVal.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: SETPROPERTY <ObjectID> <Property name> <Property value>" );
		return;
	}

	Env->Linker->SetObjectProperty( ObjID, PropID, PropVal );
}

void clConsole::SetC( const LString& Param )
{
	LString Param1 = LStr::GetToken( Param, 1 );
	LString Param2 = LStr::GetToken( Param, 2 );

	if ( Param1.empty() || Param2.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: SET <Var name> <Var value>" );
		return;
	}

	clCVar* V = GetVar( Param1 );
	V->SetString( Param2 );
}

void clConsole::ShowC( const LString& Param )
{
	LString Param1 = LStr::GetToken( Param, 1 );

	if ( Param1.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: SHOW <Var name>" );
		return;
	}

	Display( GetVar( Param1 )->GetString() );
}

void clConsole::MenuC( const LString& Param )
{
	LString Param1 = LStr::GetToken( Param, 1 );

	if ( Param1.empty() || !LStr::IsCorrectInt( Param1 ) )
	{
		Env->Console->DisplayInfoTip( "Usage: MENU <MenuID>" );
		return;
	}

	Env->SendSync( L_EVENT_MENU, LEventArgs( LStr::ToInt( Param1 ) ), false );
}

void clConsole::ToggleC( const LString& Param )
{
	LString Param1 = LStr::GetToken( Param, 1 );

	if ( Param1.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: TOGGLE <Var name>" );
		return;
	}

	clCVar* V = GetVar( Param1 );

	V->SetBool( !V->GetBool() );
}

void clConsole::UnbindAll()
{
	for ( int i = 0; i != 255; ++i )
	{
		UnBind( i );
	}
}

void clConsole::ClearC( const LString& Param )
{
	FMessagesHistory.clear();
}

void clConsole::VersionC( const LString& Param )
{
	Display( ENGINE_NAME );
	Display( "Version: " + ENGINE_VERSION );
	Display( "Copyright " + ENGINE_COPYRIGHT1 );
	Display( "Copyright " + ENGINE_COPYRIGHT2 );
	Display( "Compiled on " + ENGINE_COMPILE_TIME );
}

void clConsole::HelpC( const LString& Param )
{
	int     Col  = 0;
	LString Line( "" );

	for ( clCommandsList::iterator i = FCommandsList.begin();
	      i != FCommandsList.end();
	      ++i )
	{
		LString Cmd = i->first;
		LStr::PadRight( &Cmd, 20, ' ' );

		Line += Cmd;
		Col++;

		// display commands in 3 columns
		if ( Col > 4 )
		{
			Display( Line );
			Col = 0;
			Line.assign( "" );
		}
	}
}

void clConsole::TogglePauseC( const LString& Param )
{
	LString Value = LStr::GetUpper( LStr::GetToken( Param, 1 ) );

	if ( Value == "TRUE" )
	{
		Env->Pause();
	}
	else if ( Value == "FALSE" )
	{
		Env->Resume();
	}
	else if ( Env->IsPaused() )
	{
		Env->Resume();
	}
	else
	{
		Env->Pause();
	}
}

void clConsole::MountC( const LString& Param )
{
	guard();

	LString Param1 = LStr::GetToken( Param, 1 );

	if ( Param1.empty() )
	{
		DisplayInfoTip( "Usage: MOUNT <physical path>" );
		return;
	}

	Env->FileSystem->Mount( Param1 );

	unguard();
}

void clConsole::Bind( const int Key, const LString& Cmd )
{
	FKeyBindings[Key] = Cmd;
}

void clConsole::UnBind( const int Key )
{
	FKeyBindings[Key].clear();
}

void clConsole::RegisterCommand( const LString& CMDName, const clConsoleProc& CmdProc )
{
	guard();

	LString Command = LStr::GetUpper( CMDName );

	FATAL( FCommandsList.find( Command ) != FCommandsList.end(),
	       "Duplicate command registraction: " + CMDName );

	clConsoleCommand Cmd( Command, CmdProc );
	FCommandsList[Command] = Cmd;

	unguard();
}

void clConsole::UnRegisterCommand( const LString& CMDName, const clConsoleProc& CmdProc )
{
	guard();

	LString Command = LStr::GetUpper( CMDName );

	clCommandsList::iterator i = FCommandsList.find( Command );

	FATAL( i == FCommandsList.end(),
	       "Command not found: " + CMDName );

	FATAL( !( i->second ).IsEqual( CmdProc ),
	       "Class cann''t unregister command it didn''t register: " + CMDName );

	FCommandsList.erase( i );

	unguard();
}

void clConsole::ExecuteBinding( const int Key, const bool KeyState )
{
	if ( !FKeyBindings[Key].empty() )
	{
		if ( LStr::IsFirstChar( FKeyBindings[Key], '+' ) )
		{
			// The "+" command is processed separatly and doesn't need to be registred
			LString VarName = FKeyBindings[Key].substr( 1, FKeyBindings[Key].length() - 1 );

			clCVar* CVar = GetVar( VarName );
			CVar->SetBool( KeyState );
		}
		else
		{
			if ( KeyState )
			{
				SendCommand( FKeyBindings[Key] );
			}
		}
	}

	// autorelease mouse wheel
	if ( ( Key == LK_WHEELUP || Key == LK_WHEELDOWN ) && ( KeyState ) )
	{
		ExecuteBinding( Key, false );
	}
}

void clConsole::QueryCommand( const LString& CMDName )
{
	FCommandsQueueMutex.Lock();
	FCommandsQueue.push_back( CMDName );
	FCommandsQueueMutex.Unlock();
}

void clConsole::QuitC( const LString& Param )
{
	Env->RequestExit();
}

void clConsole::ExecC( const LString& Param )
{
	guard();

	LString Parameter = Param;
	LStr::TrimSpaces( &Parameter );

	if ( Parameter.empty() )
	{
		DisplayInfoTip( "Usage: EXEC <File Name>" );
		return;
	}

	LString FName = LStr::GetToken( Parameter, 1 );

	Env->Logger->LogP( L_DEBUG, "Exec: %s", FName.c_str() );

	if ( Env->FileSystem->FileExists( FName ) )
	{
		iIStream* FCFGStream = Env->FileSystem->CreateFileReader( FName );

		while ( !FCFGStream->Eof() )
		{
			LString Cmd = FCFGStream->ReadLine();
			LStr::TrimSpaces( &Cmd );

			if ( !Cmd.empty() && Cmd.at( 0 ) != ';' )
			{
				SendCommand( Cmd );
			}
		}

		delete( FCFGStream );
	}
	else
	{
		Env->Logger->LogP( L_WARNING, "Config file %s not found", FName.c_str() );

		DisplayError( "Config file \"" + FName + "\" not found" );
	}

	unguard();
}

void clConsole::BindC( const LString& Param )
{
	guard();

	LString Param1 = LStr::GetUpper( LStr::GetToken( Param, 1 ) );
	LString Param2 = LStr::GetToken( Param, 2 );

	if ( Param1.empty() || Param2.empty() )
	{
		DisplayInfoTip( "Usage: BIND <Key Name> <Command>" );
		return;
	}

	for ( int Key = 0; Key != 255; ++Key )
	{
		if ( Utils::KeyNames[Key] == Param1 )
		{
			Bind( Key, Param2 );
			return;
		}
	}

	DisplayError( "Unknown key name '" + Param1 + "'" );

	unguard();
}

void clConsole::AliasC( const LString& Param )
{
	guard();

	LString Alias   = LStr::GetUpper( LStr::GetToken( Param, 1 ) );
	LString Command = LStr::GetToken( Param, 2 );

	if ( Alias.empty() || Command.empty() )
	{
		DisplayInfoTip( "Usage: ALIAS <Alias name> <Command>" );
		return;
	}

	FATAL( FCommandsList.find( Command ) != FCommandsList.end(), "Aliases cann't override commands: " + Alias );

	FAliasesList[ Alias ] = Command;

	unguard();
}

void clConsole::ToggleConsoleC( const LString& Param )
{
	if ( FHUDVisible )
	{
		SendCommand( "CloseConsole" );
	}
	else
	{
		ShowConsole();
	}
}

void clConsole::ShowConsole()
{
	if ( !FHUDVisible )
	{
		FHUDVisible = true; // to prevent infinite loop
		SendCommand( "AddGUI " + FConsoleHUDFileName->GetString() );
	}
}

void clConsole::Event_ERROR( LEvent Event, const LEventArgs& Args )
{
	// display error message
	DisplayError( *( LString* )Args.FPtrArg );

	ShowConsole();
}

void clConsole::Event_TIMER( LEvent Event, const LEventArgs& Args )
{
	guard( "%f", Args.FFloatArg );

	// process commands
	if ( !FCommandsQueue.empty() )
	{
		FCommandsQueueMutex.Lock();

		LString Cmd = FCommandsQueue.front();
		FCommandsQueue.pop_front();

		FCommandsQueueMutex.Unlock();

		SendCommand( Cmd );
	}

	// process message visibility
	if ( FTimeVisible > 0 )
	{
		FTimeVisible -= Args.FFloatArg;
	}

	unguard();
}

size_t clConsole::FindCommandsSeparator( const LString& CMDString ) const
{
	bool InToken = false;

	for ( size_t i = 0; i != CMDString.length(); ++i )
	{
		if ( CMDString[i] == '"' )
		{
			InToken = !InToken;
		}

		if ( CMDString[i] == ';' && !InToken )
		{
			return i;
		}
	}

	return 0;
}

LString clConsole::ExecuteScriptCode( const LString& ScriptCode, bool DisplayResult )
{
	// delayed initilaization :
	if ( !FScriptCompiler->IsInitialized() )
	{
		InitializeScript();
	}

	LString Code = FScriptCompiler->CompileStatement( ScriptCode );

	if ( !Code.empty() )
	{
		std::vector<LString> CodeLines;
		LStr::LineToMultiline( Code, CodeLines );

		LString Result = Env->Linker->ExecuteCode( CodeLines );

		if ( DisplayResult )
		{
			Display( "Result = " + Result );
		}

		FSendCommandResult = Result;
	}
	else
	{
		if ( DisplayResult )
		{
			Display( "Error while compiling statement" );
		}

		FSendCommandResult.assign( "" );
	}

	return FSendCommandResult;
}

void clConsole::ExecuteStatement( const LString& Script )
{
	ExecuteScriptCode( Script, true );
}

LStr::clStringsVector clConsole::GetAutocompleteCommand( const LString& CMDName ) const
{
	LStr::clStringsVector Result;

	if ( CMDName.empty() ) { return Result; }

	LString CMD = LStr::GetUpper( CMDName );
	size_t Len = CMD.length();

	bool Lower = LStr::IsLower( CMDName[CMDName.size()-1] );

	for ( clCommandsList::const_iterator i = FCommandsList.begin(); i != FCommandsList.end(); ++i )
	{
		if ( LStr::StartsWith( i->first, CMD ) )
		{
			LString Tail = LStr::CopyFromPosToEnd( i->first, Len );
			Result.push_back( CMDName + ( Lower ? LStr::GetLower( Tail ) : Tail ) );
		}
	}

	for ( clAliasesList::const_iterator i = FAliasesList.begin(); i != FAliasesList.end(); ++i )
	{
		if ( LStr::StartsWith( i->first, CMD ) )
		{
			LString Tail = LStr::CopyFromPosToEnd( i->first, Len );
			Result.push_back( CMDName + ( Lower ? LStr::GetLower( Tail ) : Tail ) );
		}
	}

	return Result;
}

void clConsole::SendCommand( const LString& CMDName )
{
	guard( "%s", CMDName.c_str() );

	FSendCommandResult.assign( "" );

	if ( CMDName.size() == 0 )
	{
		return;
	}

	bool IsScript = ( LStr::GetUpper( LStr::GetToken( CMDName, 1 ) ) == "RS" );

	// split command
	size_t CommandsSeparator = FindCommandsSeparator( CMDName );

	LString ThisCMDString = CMDName;
	LString RestCMDString( "" );

	if ( !IsScript )
	{
		if ( CommandsSeparator )
		{
			ThisCMDString = CMDName.substr( 0, CommandsSeparator );
			RestCMDString = CMDName.substr( CommandsSeparator,
			                                CMDName.length() - CommandsSeparator );

			if ( RestCMDString.length() > 0 )
			{
				LStr::pop_front( &RestCMDString );
			}
		}
	}

	// extract command name and parameters
	LString ThisCMDName  = LStr::GetToken( ThisCMDString, 1 );
	LString ThisCMDParam = ThisCMDString.substr( ThisCMDName.length(),
	                                             ThisCMDString.length() - ThisCMDName.length() );

	if ( ThisCMDParam.length() > 0 )
	{
		LStr::pop_front( &ThisCMDParam );
	}

	LStr::ToUpper( &ThisCMDName );

	if ( IsScript )
	{
		// execute script and return
		ExecuteStatement( ThisCMDParam );

		return;
	}

	if ( ThisCMDName.find( "WAIT" ) == 0 )
	{
		// Leave the rest of the command until the next frame
		if ( CommandsSeparator )
		{
			QueryCommand( RestCMDString );
		}

		return;
	}
	else
	{
		clCommandsList::const_iterator i = FCommandsList.find( ThisCMDName );

		if ( i != FCommandsList.end() )
		{
			( i->second ).Exec( clFileSystem::ReplaceEnvVars( ThisCMDParam ) );
		}
		else
		{
			clAliasesList::const_iterator j = FAliasesList.find( ThisCMDName );

			if ( j != FAliasesList.end() )
			{
				QueryCommand( j->second );
			}
			else
			{
				DisplayError( "Unknown command: " + ThisCMDName );
			}
		}
	}

	if ( CommandsSeparator )
	{
		SendCommand( RestCMDString );
	}

	unguard();
}

const float MESSAGE_VISIBILITY_TIME = 2.0f;

void clConsole::DisplayMessage( const LString& Message, const LConsoleMessageType MSGType ) const
{
	if ( Env )
	{
		Env->SendSync( L_EVENT_CONSOLELOG, LEventArgs( &Message ), false );
	}

	FTimeVisible = MESSAGE_VISIBILITY_TIME;

	LVector4 Color;

	switch ( MSGType )
	{
		case CMSG_ENGINEMESSAGE:
			Color = LC_LinderdaumConsoleEM;
			break;
		case CMSG_ERRORMESSAGE:
			Color = LC_LinderdaumConsoleERR;
			break;
		case CMSG_INFOTIP:
			Color = LC_LinderdaumConsoleIT;
			break;
		default:
			FATAL_MSG( "Unknown message type" );
	}

	LString Msg = Message;

	LMutex Lock( &FMessagesHistoryMutex );

	while ( true )
	{
		size_t Separation = Msg.find( "\n" );

		if ( Separation == LString::npos )
		{
			break;
		}

		LString NewMsg = Msg.substr( 0, Separation );

		FMessagesHistory.push_back( sConsoleMessage( NewMsg, Color ) );

		if ( FMessagesHistory.size() > MAX_HISTORY_SIZE )
		{
			FMessagesHistory.pop_front();
		}

		Msg = Msg.substr( Separation + 1, Message.length() - 1 );
	}

	FMessagesHistory.push_back( sConsoleMessage( Msg, Color ) );
}

std::list<sConsoleMessage>* clConsole::LockConsoleHistory()
{
	FMessagesHistoryMutex.Lock();

	return &FMessagesHistory;
}

void clConsole::UnlockConsoleHistory()
{
	FMessagesHistoryMutex.Unlock();
}

void clConsole::RefreshScreenC( const LString& Param )
{
	guard();

	Env->RenderFrame( 0, 0 );

	unguard();
}

void clConsole::SleepC( const LString& Param )
{
	LString Param1 = LStr::GetToken( Param, 1 );

	if ( Param1.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: SLEEP <Seconds>" );
		return;
	}

	if ( !LStr::IsCorrectDouble( Param1 ) )
	{
		Env->Console->DisplayInfoTip( LString( "Invalid argument for SLEEP: " ) + Param1 );

		return;
	}

	Env->SleepSeconds( LStr::ToDouble( Param1 ) );
}

void clConsole::SetLogLevelC(const LString& Params)
{
	if(Params.empty())
	{
		Env->Console->DisplayInfoTip( "Usage: SetLogLevel <Level: Paranoid, Debug, Notice, Log, Warning>" );
		return;
	}

	Env->SetLogLevel( StringToLogLevel( LStr::GetTrimmedSpaces(Params) ) );
}

void clConsole::PrintLogLevelC(const LString& Params)
{
	(void)Params;

	Env->Console->Display( LString("Current log level: ") + LogLevelToString( Env->GetLogLevel() ) );
}

/*
 * 01/12/2010
     GetAutocompleteCommand()
 * 05/04/2009
     Classes load time logging
 * 13/07/2007
     Execute RS statement
 * 30/07/2007
     LoadAsmC() instead of LoadClass/LoadMethod
 * 06/07/2007
     SaveVarsC()
 * 29/05/2007
     Merged with CVarsManager
 * 02/05/2007
     DisplayMessage() accepts multiline messages
 * 21/03/2007
     Aliases support
 * 10/04/2005
     VersionC()
 * 22/02/2005
     ClearC()
 * 17/02/2005
     "+" command processing reimplemented
 * 31/01/2005
     ToggleConsole()
 * 25/01/2005
     SendCommand()
     FindCommandsSeparator()
     ExecC()
     Keys binding mechanism works at last
 * 24/01/2005
     SendKey()
 * 21/01/2005
     It's here
*/
