/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_PushButton.h"

#include "Engine.h"
#include "Environment.h"
#include "GUI/GUIManager.h"
#include "Core/Console.h"
#include "Resources/ResourcesManager.h"
#include "Utils/Localizer.h"

#include "LColors.h"

clGUIPushButton::clGUIPushButton():
	FNormalShader( NULL ),
	FPushedShader( NULL ),
	FHighlightShader( NULL ),
	FDisabledShader( NULL ),
	FButtonPushed( false ),
	FButtonDisabled( false )
{
	FButtonPushed = false;
	SetDefaultColor( LC_Black );
}

void clGUIPushButton::Event_Registered( iGUIResponder* Source )
{
	iGUIButton::Event_Registered( Source );

	FNormalShader = Env->Resources->LoadShader( "interface/shaders/button.shader" );
	FHighlightShader = Env->Resources->LoadShader( "interface/shaders/button_selected.shader" );
	FPushedShader = Env->Resources->LoadShader( "interface/shaders/button_pushed.shader" );
	FDisabledShader = Env->Resources->LoadShader( "interface/shaders/button_disabled.shader" );

	SetShader( FNormalShader );
};

void clGUIPushButton::PreRender()
{
	if ( FButtonDisabled )
	{
		SetShader( FDisabledShader );
	}
	else if ( FButtonPushed )
	{
		IsMouseOver() ? SetShader( FPushedShader ) : SetShader( FNormalShader );
	}
	else
	{
		( IsMouseOver() && !Env->GUI->IsTouchScreen() ) ? SetShader( FHighlightShader ) : SetShader( FNormalShader );
	}

	iGUIButton::PreRender();

	OutTextXY( CenterTextHorizontal( GetTitle() ),
	           CenterTextVertical() + Env->GUI->GetDefaultButtonVerticalTitleOffset(),
	           GetTitle(),
	           GetDefaultColor() );
}

void clGUIPushButton::Event_CaptureChanged( iGUIResponder* Source )
{
	iGUIButton::Event_CaptureChanged( Source );

	FButtonPushed = false;
}

void clGUIPushButton::Event_MouseInside( iGUIResponder* Source, const LVector2& Pnt )
{
	iGUIButton::Event_MouseInside( Source, Pnt );
}

void clGUIPushButton::Event_MouseOutside( iGUIResponder* Source, const LVector2& Pnt )
{
	iGUIButton::Event_MouseOutside( Source, Pnt );
}

void clGUIPushButton::Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState )
{
	iGUIButton::Event_MouseLeft( Source, Pnt, KeyState );

	if ( KeyState )
	{
		Env->Console->SendCommand( "Announce sounds/interface/click.wav GUI.SoundEnabled" );
		Env->GUI->SetCapture( this );
	}
	else
	{
		if ( Contains( Pnt ) && FButtonPushed )
		{
			ExecuteCommandsStack();
		}

		Env->GUI->ReleaseCapture();
	}

	FButtonPushed = KeyState;
}

/*
 * 09/02/2005
     It's here
*/
