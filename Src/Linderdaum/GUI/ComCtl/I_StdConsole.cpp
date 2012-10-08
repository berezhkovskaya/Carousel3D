/*
   (Part of Linderdaum Engine)
   Version 0.5.91
   (05/07/2010)
   (C) Sergey Kosarevsky, 2005-2010
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_StdConsole.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/Console.h"
#include "GUI/GUIManager.h"
#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iShaderProgram.h"
#include "Geometry/GeomServ.h"
#include "Resources/ResourcesManager.h"

#include "LKeys.h"
#include "LColors.h"

// console history window size in lines
static const int ConsoleHistorySize = clConsole::MAX_HISTORY_SIZE;

void clGUIStdConsole::Event_Registered( iGUIResponder* Source )
{
	FCurrentPosition = Env->GUI->GetStringHeight() * ( ConsoleHistorySize + 3 );

	FConsoleShader = Env->Resources->LoadShader( "Interface/Shaders/Console.shader" );
	FTextBarShader = Env->Resources->LoadShader( "Interface/Shaders/Console_textbar.shader" );

	SetOpacity( 1.0f );

	FConsoleGeometry = Env->Renderer->AllocateEmptyVA();
	FTextBarGeometry = Env->Renderer->AllocateEmptyVA();

	SetRegionState( REGION_MAXIMIZING );

	Env->Console->SetHUDVisibility( true );
	Env->Console->RegisterCommand( "CloseConsole", ::Utils::Bind( &clGUIStdConsole::CloseConsoleC, this ) );

	FTextBuffer = Env->Console->GetCommandsHistory()->GetCurrentCommand();

	static const float FontHeight = Env->GUI->GetStringHeight();

	FConsoleGeometry->SetVertexAttribs( clGeomServ::CreateRect2D( 0, 0, 1, FontHeight*( ConsoleHistorySize + 2 ), 0, false, 0 ) );
	FTextBarGeometry->SetVertexAttribs( clGeomServ::CreateRect2D( 0, FontHeight*( ConsoleHistorySize - 2 ) + FontHeight * 3.0f, 1, FontHeight*( ConsoleHistorySize - 1 ) + FontHeight * 3.5f, 0, false, 0 ) );
}

void clGUIStdConsole::Event_UnRegistered( iGUIResponder* Source )
{
}

clGUIStdConsole::~clGUIStdConsole()
{
	FIXME( "fix RemoveAllChildViews ..." )
	Env->Console->GetCommandsHistory()->SetCurrentCommand( FTextBuffer );

	Env->Console->UnRegisterCommand( "CloseConsole", ::Utils::Bind( &clGUIStdConsole::CloseConsoleC, this ) );
	Env->Console->SetHUDVisibility( false );

	delete( FConsoleGeometry );
	delete( FTextBarGeometry );
}

void clGUIStdConsole::Event_Timer( iGUIResponder* Source, float DeltaTime )
{
	FCursorBlinking += DeltaTime;

	if ( FCursorBlinking > 0.5f )
	{
		FCursorBlinking = 0;
	}

	static const float FontHeight = Env->GUI->GetStringHeight();

	switch ( GetRegionState() )
	{
		case REGION_MINIMIZED:
			Env->GUI->RequestClose( this );
			break;
		case REGION_MINIMIZING:

			if ( FCurrentPosition < FontHeight*( ConsoleHistorySize + 3 ) )
			{
				float D = FontHeight * ( ConsoleHistorySize + 3 ) - FCurrentPosition;
				FCurrentPosition += FontHeight + D * 0.75f;
//                                FCurrentPosition += FontHeight;
			}
			else
			{
				Event_Minimized( Source );
			}

			break;
		case REGION_MAXIMIZING:

			if ( FCurrentPosition > FontHeight )
			{
				float D = FCurrentPosition - FontHeight;
				FCurrentPosition -= FontHeight + D * 0.75f;
//                                FCurrentPosition -= FontHeight;
			}
			else
			{
				Event_Maximized( Source );
			}

			break;
	}

	switch ( GetRegionState() )
	{
		case REGION_MINIMIZING:
		case REGION_MAXIMIZING:
		case REGION_SHRINKING:
			Env->GUI->RecheckMouse();
	}
}

void clGUIStdConsole::PreRender()
{
	// empty
}

void clGUIStdConsole::PostRender()
{
	static const float FontHeight = Env->GUI->GetStringHeight();

	UpdateUniformsV( FConsoleShader, LVector4( GetX1(), GetY1() - FCurrentPosition, GetX2(), GetY2() - FCurrentPosition ) );
	Env->Renderer->AddBuffer( FConsoleGeometry, FConsoleShader, 1, false );

	UpdateUniformsV( FTextBarShader, LVector4( GetX1(), GetY1() + FCurrentPosition, GetX2(), GetY2() + FCurrentPosition ) );
	Env->Renderer->AddBuffer( FTextBarGeometry, FTextBarShader, 1, false );

	int MsgNum = 1;

	static const float StrHeight = Env->GUI->GetStringHeight();

	Env->Renderer->GetCanvas()->BeginTextPacket();

	std::list<sConsoleMessage>* ConsoleHistory = Env->Console->LockConsoleHistory();

	for ( std::list<sConsoleMessage>::reverse_iterator i = ConsoleHistory->rbegin(); i != ConsoleHistory->rend(); ++i )
	{
		const float VerticalPos = ( ConsoleHistorySize - 0.40f - MsgNum ) * StrHeight - FCurrentPosition;

		OutTextXY( 0, VerticalPos, ( *i ).FMessage, ( *i ).FColor );

		MsgNum++;

		if ( VerticalPos < -StrHeight )
		{
			break;
		}
	}

	Env->Console->UnlockConsoleHistory();

	const float VerticalPos = ( ConsoleHistorySize - 0.40f ) * StrHeight + FCurrentPosition;

	OutTextXY( 0,     VerticalPos, '>' + FTextBuffer, LC_White );

	OutTextXY( 0.92f, VerticalPos, ENGINE_VERSION,  LC_LimeGreen );

	// draw blinking cursor
	LString CursorLine;

	LStr::PadRight( &CursorLine, FCursorPos + 1, ' ' );

	CursorLine += ( ( FCursorBlinking > 0.25f ) ? '_' : ' ' );

	OutTextXY( 0, VerticalPos, CursorLine, LC_White );

	Env->Renderer->GetCanvas()->EndTextPacket();
}

void clGUIStdConsole::CloseConsoleC( const LString& Param )
{
	SetRegionState( REGION_MINIMIZING );
}

void clGUIStdConsole::ExecuteCommand()
{
	if ( !FTextBuffer.empty() )
	{
		Env->Console->GetCommandsHistory()->PutCommand( FTextBuffer );
		Env->Console->DisplayInfoTip( '/' + FTextBuffer );
		Env->Console->QueryCommand( FTextBuffer );
		FTextBuffer.clear();
		FCursorPos = 0;
	}
}

bool clGUIStdConsole::Event_Key( iGUIResponder* Source, const int Key, const bool KeyState )
{
	iGUIView::Event_Key( Source, Key, KeyState );

	const float MaxScrLength = 0.79f;

	if ( KeyState )
	{
		if ( Utils::IsKeyAlphaNumeric( Key ) && !Env->Console->IsKeyPressed( LK_CONTROL ) )
		{
			if ( Env->GUI->GetStringWidth( FTextBuffer, NULL ) < MaxScrLength )
			{
				int Ch = Utils::TranslateKey( Key, Env->Console->IsKeyPressed( LK_SHIFT ) );

				FTextBuffer.insert( FCursorPos, 1, static_cast<Lubyte>( Ch ) );

				FCursorPos++;

				Env->Console->GetCommandsHistory()->SetCurrentCommand( FTextBuffer );
			}
		}
		else
		{
			if ( Key == LK_TAB && !FTextBuffer.empty() )
			{
				LStr::clStringsVector AutoComplete = Env->Console->GetAutocompleteCommand( FTextBuffer );

				if ( AutoComplete.size() == 1 )
				{
					FTextBuffer = *AutoComplete.begin();
					FCursorPos = FTextBuffer.length();
				}
				else
				{
					if ( AutoComplete.size() > 0 ) { Env->Console->DisplayInfoTip( "Suggested commands:" ); }

					for ( size_t i = 0; i != AutoComplete.size(); i++ ) { Env->Console->DisplayError( "/" + AutoComplete[i] ); }
				}
			}
			else if ( Key == LK_TAB || Key == LK_COMMA2 )
			{
				CloseConsoleC( "" );
			}

			if ( Key == LK_ESCAPE && FTextBuffer.empty() )
			{
				CloseConsoleC( "" );
			}

			if ( Key == LK_RETURN )
			{
				ExecuteCommand();
			}

			Env->GUI->ProcessInputLineKey( Key, FTextBuffer, FCursorPos, MaxScrLength );
		}
	}

	// always kill all keys
	return false;
}

/*
 * 06/07/2010
     Unified edit line
 * 05/07/2010
     Text insertion from clipboard via Shift+Ins
 * 02/03/2005
     Cursor blinking
 * 19/02/2005
     QueryCommand() used instead of SendCommand()
 * 04/02/2005
     Completely reimplemented
 * 03/02/2005
     Event_Key()
 * 02/02/2005
     Event_Timer()
 * 30/01/2005
     It's here
*/
