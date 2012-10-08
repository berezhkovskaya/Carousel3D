/**
 * \file Console.h
 * \brief Virtual console
 * \version 0.5.98
 * \date 01/12/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clConsole_
#define _clConsole_

#include "Core/iObject.h"
#include "VFS/FileSystem.h"
#include "VFS/iIStream.h"
#include "Math/LVector.h"

#include <map>
#include <deque>
#include <list>

#include "Generated/Functors.h"

enum LConsoleMessageType
{
	CMSG_ENGINEMESSAGE,
	CMSG_ERRORMESSAGE,
	CMSG_INFOTIP
};

struct sConsoleMessage
{
	sConsoleMessage(): FMessage(),
		FColor() {};
	sConsoleMessage( const LString& Message, const LVector4& Color ): FMessage( Message ),
		FColor( Color ) {};
	LString  FMessage;
	LVector4 FColor;
};

class clCVar;
class clConsole;
class clConsoleCommand;
class iIStream;
class iOStream;
class clConsoleHistory;
class clCommandsHistory;
class clCommandsQueue;
class iCfgExecutor;
class clScriptCompiler;

/**
   \brief Virtual console

      - Provides a simple config files interpreter
      - Tracks the history of executed commands
      - Stores the list of global untyped variables (clCVar)
      - Handles the delayed initialization of scripts compiler
**/
class scriptfinal clConsole: public iObject
{
public:
	static const int MAX_HISTORY_SIZE = 25;

	typedef ::Linderdaum::Utils::clFunctor<void, TYPELIST_1( const LString& ) > clConsoleProc;

	/// History of console commands
	class clCommandsHistory
	{
	public:
		clCommandsHistory(): FCurrentCommand(),
			FOffset( 0 ),
			FTotalCommands( 0 ),
			FHead( MAX_HISTORY_SIZE - 1 )
		{
			FCommands[FHead] = "";
		};
		inline void PutCommand( const LString& Command )
		{
			FOffset = 0;

			FCommands[FHead] = Command;

			if ( FHead == 1 )
			{
				FHead = MAX_HISTORY_SIZE - 1;
			}
			else
			{
				FHead--;
			}

			if ( FTotalCommands < MAX_HISTORY_SIZE - 1 )
			{
				FTotalCommands++;
			}
		}
		inline void SetCurrentCommand( const LString& Cmd )
		{
			FCurrentCommand = Cmd;
		}
		inline LString GetCurrentCommand()
		{
			FOffset = 0;

			return FCurrentCommand;
		}
		inline LString MoveBack()
		{
			if ( FOffset < FTotalCommands )
			{
				FOffset++;
			}

			return GetCommand( FOffset );

		}
		inline LString MoveForward()
		{
			if ( FOffset > 0 )
			{
				FOffset--;
			}

			if ( FOffset == 0 )
			{
				return "";
			}

			return GetCommand( FOffset );
		}
		inline LString GetCommand( int I )
		{
			int Index = FHead + I;

			if ( Index > MAX_HISTORY_SIZE - 1 )
			{
				Index = Index % ( MAX_HISTORY_SIZE - 1 );
			}

			return FCommands[Index];
		}
		int GetCommandsCount() const
		{
			return FTotalCommands;
		};
		//
		// iSerializable
		//
		void LoadFromStream( iIStream* IStream );
		void SaveToStream( iOStream* OStream );
	private:
		LString    FCommands[MAX_HISTORY_SIZE];
		LString    FCurrentCommand;
		int        FTotalCommands;
		int        FOffset;
		int        FHead;
	};
public:
	clConsole();
	virtual ~clConsole();

	NET_EXPORTABLE()

	//
	// iObject interface
	//
	scriptmethod void               AfterConstruction();
	//
	// clConsole
	//
	scriptmethod void               UnpressAllKeys()
	{
		memset( FKeyPressed, 0, sizeof( FKeyPressed ) );
	};
	scriptmethod bool               IsKeyPressed( const int Key )
	{
		return FKeyPressed[Key];
	};
	scriptmethod bool               IsKeyPressedAny();
	scriptmethod void               ExecuteBinding( const int Key, const bool KeyState );
	scriptmethod void               UnbindAll();
	scriptmethod void               Bind( const int Key, const LString& Cmd );
	scriptmethod void               UnBind( const int Key );
	scriptmethod void               DisplayMessage( const LString& Message, const LConsoleMessageType MSGType ) const;
	scriptmethod void               RegisterCommand( const LString& CMDName, const clConsoleProc& CmdProc );
	scriptmethod void               UnRegisterCommand( const LString& CMDName, const clConsoleProc& CmdProc );
	scriptmethod void               SendCommand( const LString& CMDName );
	scriptmethod void               QueryCommand( const LString& CMDName );
	scriptmethod LStr::clStringsVector GetAutocompleteCommand( const LString& CMDName ) const;
	scriptmethod void               GetSendCommandResult( LString& Result ) const { Result = FSendCommandResult; };
	scriptmethod void               SetHUDVisibility( const bool HUDVisible ) { FHUDVisible = HUDVisible; };
	scriptmethod void               ShowConsole();
	scriptmethod std::list<sConsoleMessage>*  LockConsoleHistory();
	scriptmethod void               UnlockConsoleHistory();
	scriptmethod clCommandsHistory* GetCommandsHistory() { return FCommandsHistory; };
	scriptmethod clCVar*            GetVar( const LString& VarName );
	scriptmethod clCVar*            GetVarDefault( const LString& VarName, const LString& Default );
	scriptmethod clCVar*            GetVarDefaultFloat( const LString& VarName, float Default );
	scriptmethod clCVar*            FindVar( const LString& VarName );
	scriptmethod LString            GetVarValueStr( const LString& VarName, const LString& Default );
	scriptmethod int                GetVarValueInt( const LString& VarName, const int Default );
	scriptmethod float              GetVarValueFloat( const LString& VarName, const float Default );
	scriptmethod bool               GetVarValueBool( const LString& VarName, const bool Default );
	scriptmethod float              GetTimeVisible() const { return FTimeVisible; };
	scriptmethod LString            ExecuteScriptCode( const LString& ScriptCode, bool DisplayResult );
	scriptmethod clScriptCompiler*  GetScriptCompiler() const;
	/// explicit script initialization
	scriptmethod void               InitializeScript();

#pragma region Message helpers
	scriptmethod void Display( const LString& Message ) { DisplayMessage( Message, CMSG_ENGINEMESSAGE ); };
	scriptmethod void DisplayError( const LString& Message ) { DisplayMessage( Message, CMSG_ERRORMESSAGE ); /*ShowConsole();*/ };
	scriptmethod void DisplayWarning( const LString& Message ) { DisplayMessage( Message, CMSG_ERRORMESSAGE ); };
	scriptmethod void DisplayInfoTip( const LString& Message ) { DisplayMessage( Message, CMSG_INFOTIP ); };
#pragma endregion

	// events
	FWD_EVENT_HANDLER( Event_TIMER );
	FWD_EVENT_HANDLER( Event_KEY );
	FWD_EVENT_HANDLER( Event_ERROR );
private:
	typedef std::map<LString, clConsoleCommand> clCommandsList;
	typedef std::map<LString, LString>          clAliasesList;

	/// Script compiler
	clScriptCompiler*   FScriptCompiler;
private:
	void             ExecuteStatement( const LString& Script );
	inline size_t    FindCommandsSeparator( const LString& CMDString ) const;
private:
	/// storage for command result
	LString                               FSendCommandResult;
	LString                               FKeyBindings[256];
	bool                                  FKeyPressed[256];
	clCommandsList                        FCommandsList;
	clAliasesList                         FAliasesList;
	std::vector<clCVar*>                  FCVars;
	/// commands processing
	std::deque<LString>                   FCommandsQueue;
	mutable std::list<sConsoleMessage>    FMessagesHistory;
	clCommandsHistory*                    FCommandsHistory;
	clMutex                               FMessagesHistoryMutex;
	clMutex                               FCVarsAccessMutex;
	clMutex                               FCommandsQueueMutex;

#pragma region Console HUD
	bool                                  FHUDVisible;
	mutable float                         FTimeVisible;
	clCVar*                               FConsoleHUDFileName;
	clCVar*                               FShouldSaveHistory;
#pragma endregion

private:
	// commands handlers
	scriptmethod void    QuitC( const LString& Param );
	scriptmethod void    ExecC( const LString& Param );
	scriptmethod void    BindC( const LString& Param );
	scriptmethod void    AliasC( const LString& Param );
	scriptmethod void    ToggleConsoleC( const LString& Param );
	scriptmethod void    MountC( const LString& Param );
	scriptmethod void    ClearC( const LString& Param );
	scriptmethod void    VersionC( const LString& Param );
	scriptmethod void    HelpC( const LString& Param );
	scriptmethod void    TogglePauseC( const LString& Param );
	scriptmethod void    ToggleC( const LString& Param );
	scriptmethod void    SetC( const LString& Param );
	scriptmethod void    SetPropertyC( const LString& Param );
	scriptmethod void    ShowC( const LString& Param );
	scriptmethod void    MenuC( const LString& Param );
	scriptmethod void    SaveVarsC( const LString& Param );
	scriptmethod void    RefreshScreenC( const LString& Param );
	scriptmethod void    SleepC( const LString& Param );
	scriptmethod void    PrintLogLevelC( const LString& Params );
	scriptmethod void    SetLogLevelC( const LString& Params );
};

/**
   \brief Console command executor for function pointer wrapping
**/
class clConsoleCommand
{
public:
	clConsoleCommand(): FCommandName(), FCommandProc() {};
	clConsoleCommand( const LString& CommandName,
	                  const clConsole::clConsoleProc& CommandProc ): FCommandName( CommandName ), FCommandProc( CommandProc ) {};
	virtual ~clConsoleCommand() {};
	//
	// clConsoleCommand
	//
	scriptmethod void Exec( const LString& Param ) const { FCommandProc( Param ); };
	scriptmethod bool IsEqual( const clConsole::clConsoleProc& Proc ) const
	{
		// NOTE: test this
		return FCommandProc.IsEqual( Proc );
	}
private:
	LString                             FCommandName;
	mutable clConsole::clConsoleProc    FCommandProc;
};

#endif

/*
 * 01/12/2010
     GetAutocompleteCommand()
 * 03/05/2009
     Merged with iConsole
 * 01/12/2007
     GetSendCommandResult()
 * 13/08/2007
     ExecuteStatement() + SendCommand support for "RS"
     RefreshScreenC()
 * 30/07/2007
     LoadAsm, LoadScriptDirectory
 * 06/07/2007
     SaveVarsC()
     FConsoleHUDFileName
 * 29/05/2007
     Merged with CVarsManager
 * 21/03/2007
     AliasC()
 * 15/01/2007
     TogglePause()
 * 27/11/2006
     LoadClassC()
 * 22/01/2005
     FKeyBindings
 * 20/01/2005
     It's here
*/
