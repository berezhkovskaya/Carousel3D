/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_Checkable.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/CVars.h"
#include "GUI/GUIManager.h"
#include "Resources/ResourcesManager.h"
#include "Utils/Viewport.h"

#include "LColors.h"

void iGUICheckable::AfterConstruction()
{
	iGUIButton::AfterConstruction();

	FHDelta1   = Env->GUI->GetDefaultCheckBoxHeight_Delta1();
	FHDelta2   = Env->GUI->GetDefaultCheckBoxHeight_Delta2();
	FBoxWidth  = Env->GUI->GetDefaultCheckBoxWidth();
}

void iGUICheckable::Event_Timer( iGUIResponder* Source, float DeltaTime )
{
	if ( GetConsoleVariable() )
	{
		// check if the variable was changed by external events
		if ( FChecked != GetConsoleVariable()->GetBool() )
		{
			FChecked = GetConsoleVariable()->GetBool();
			IsChecked() ? Check() : UnCheck();
		}
	}

	iGUIButton::Event_Timer( Source, DeltaTime );
}

void iGUICheckable::Check()
{
	if ( GetConsoleVariable() )
	{
		GetConsoleVariable()->SetBool( true );
	}

	FChecked = true;
	SetShader( FCheckedShader );
}

void iGUICheckable::UnCheck()
{
	if ( GetConsoleVariable() )
	{
		GetConsoleVariable()->SetBool( false );
	}

	FChecked = false;
	SetShader( FUnCheckedShader );
}

void iGUICheckable::Toggle()
{
	IsChecked() ? UnCheck() : Check();
}

void iGUICheckable::SetCheckedShaderName( const LString& FileName )
{
	SetCheckedShader( Env->Resources->LoadShader( FileName ) );
}

void iGUICheckable::SetUnCheckedShaderName( const LString& FileName )
{
	SetUnCheckedShader( Env->Resources->LoadShader( FileName ) );
}

void iGUICheckable::SetCheckedShader( clRenderState* Shader )
{
	FCheckedShader = Shader;

	if ( IsChecked() )
	{
		SetShader( FCheckedShader );
	}
}

void iGUICheckable::SetUnCheckedShader( clRenderState* Shader )
{
	FUnCheckedShader = Shader;

	if ( !IsChecked() )
	{
		SetShader( FUnCheckedShader );
	}
}

LVector4 iGUICheckable::GetUniformsVec() const
{
	return LVector4( GetX1(), GetY1() + FHDelta1, GetX1() + FBoxWidth, GetY1() + FHDelta2 );
}

void iGUICheckable::PreRender()
{
	iGUIButton::PreRender();

	LVector4 Color = IsMouseOver() ? GetDefaultColor() : LC_Gray70;

	static const float TitleOffset = Env->GUI->GetDefaultCheckBoxHorizontalTitleOffset();

	OutTextXY( TitleOffset,
	           CenterTextVertical() + Env->GUI->GetDefaultButtonVerticalTitleOffset(),
	           GetTitle(),
	           Color );
}

void iGUICheckable::SetBoxWidth( float W )
{
	FBoxWidth = W / Env->Viewport->GetAspectRatio();
}

float iGUICheckable::GetBoxWidth() const
{
	return FBoxWidth * Env->Viewport->GetAspectRatio();
}

/*
 * 19/04/2005
     Event_Registered() deprecated
 * 08/04/2005
     PreRender()
 * 12/01/2005
     It's here
*/
