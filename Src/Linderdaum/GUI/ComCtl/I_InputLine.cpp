/*
   (Part of Linderdaum Engine)
   Version 0.5.72
   (02/03/2007)
   (C) Sergey Kosarevsky, 2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_InputLine.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/Linker.h"
#include "Core/Console.h"
#include "GUI/iValidators.h"
#include "GUI/GUIManager.h"
#include "Resources/ResourcesManager.h"

#include "LKeys.h"
#include "LColors.h"

clGUIInputLine::clGUIInputLine(): FCursorPos( 0 ),
	FEditable( true ),
	FCursorBlinking( 0 )
{
}

void clGUIInputLine::AfterConstruction()
{
	iGUIButton::AfterConstruction();

	FValidator = Env->Linker->Instantiate( "clValidator" );
}

clGUIInputLine::~clGUIInputLine()
{
	if ( FValidator )
	{
		delete( FValidator );
	}
}

void clGUIInputLine::SetValidator( clValidator* Validator )
{
	if ( FValidator )
	{
		delete( FValidator );
	}

	FValidator = Validator;
}

void clGUIInputLine::Event_Registered( iGUIResponder* Source )
{
	iGUIButton::Event_Registered( Source );

	SetShader( Env->Resources->LoadShader( "interface/shaders/inputline.shader" ) );
}

void clGUIInputLine::PreRender()
{
	iGUIButton::PreRender();
}

void clGUIInputLine::PostRender()
{
	iGUIButton::PostRender();

	float X = CenterTextHorizontal( GetTitle() + "  " );
	float Y = CenterTextVertical() + Env->GUI->GetDefaultButtonVerticalTitleOffset();

	if ( IsMouseOver() || Env->GUI->IsTouchScreen() )
	{
		OutTextXY( X, Y, GetTitle() + ( ( FCursorBlinking > 0.25f ) ? '_' : ' ' ), LC_Gray80 );
	}
	else
	{
		OutTextXY( X, Y, GetTitle(), LC_Gray40 );
	}
}

bool clGUIInputLine::Event_Key( iGUIResponder* Source, const int Key, const bool KeyState )
{
	iGUIButton::Event_Key( Source, Key, KeyState );

	LString TextBuffer = GetTitle();

	float Length = ( 0.79f * GetWidth() ) / Env->GUI->GetStringWidth( " ", NULL );

	FValidator->SetMaxLength( static_cast<int>( Length ) );

	if ( KeyState )
	{
		if ( Utils::IsKeyAlphaNumeric( Key ) && !Env->Console->IsKeyPressed( LK_CONTROL ) )
		{
			if ( FValidator->IsValid( TextBuffer + ' ' ) )
			{
				int Ch = Utils::TranslateKey( Key, Env->Console->IsKeyPressed( LK_SHIFT ) );

				TextBuffer.push_back( static_cast<Lubyte>( Ch ) );
			}
		}
		else
		{
			Env->GUI->ProcessInputLineKey( Key, TextBuffer, FCursorPos, Length );
		}
	}

	SetTitle( TextBuffer );

	// always kill all keys
	return false;
}

void clGUIInputLine::Event_Timer( iGUIResponder* Source, float DeltaTime )
{
	FCursorBlinking += DeltaTime;

	if ( FCursorBlinking > 0.5f )
	{
		FCursorBlinking = 0;
	}

	iGUIButton::Event_Timer( Source, DeltaTime );
}

/*
 * 02/03/2007
     It's here
*/
