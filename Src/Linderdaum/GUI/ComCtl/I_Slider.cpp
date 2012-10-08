/*
   (Part of Linderdaum Engine)
   Version 0.5.73
   (05/04/2007)
   (C) Sergey Kosarevsky, 2006-2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_Slider.h"

#include "Environment.h"
#include "Math/LMath.h"
#include "Core/Logger.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "GUI/GUIManager.h"
#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"
#include "Resources/ResourcesManager.h"

clGUISlider::clGUISlider(): FCurrentValue( 0.5f ),
	FMinValue( 0.0f ),
	FMaxValue( 1.0f ),
	FSliderIsMoving( false ),
	FShowValue( true )
{
}

void clGUISlider::AfterConstruction()
{
	clGUIPanel::AfterConstruction();

	SetShader( Env->Resources->LoadShader( "interface/shaders/slider_h.shader" ) );

	FSlideBarShader = Env->Resources->LoadShader( "interface/shaders/slider.shader" );
	FSlideBarGeometry = Env->GUI->GetDefaultBevelRect();
}

void clGUISlider::Event_Timer( iGUIResponder* Source, float DeltaTime )
{
	if ( GetConsoleVariable() ) { SetCurrentValue( GetConsoleVariable()->GetFloat() ); }

	clGUIPanel::Event_Timer( Source, DeltaTime );
}

void clGUISlider::PostRender()
{
	clGUIPanel::PostRender();

	UpdateUniformsV( FSlideBarShader, GetSliderCoords() );

	Env->Renderer->AddBuffer( FSlideBarGeometry, FSlideBarShader, 1, false );

	if ( FShowValue )
	{
		OutTextXY( 0.0, 0.0, LStr::ToStr( GetCurrentValue() ), LVector4( 1.0f, 0.0f, 0.0f, 0.5f ) );
	}
}

void clGUISlider::Event_CaptureChanged( iGUIResponder* Source )
{
	clGUIPanel::Event_CaptureChanged( Source );

	FSliderIsMoving = false;
}

static const float SliderWidth = 0.01f;

LVector4 clGUISlider::GetSliderCoords()
{
	if ( FCurrentValue < 0 ) { FCurrentValue = 0; }

	if ( FCurrentValue > GetWidth() - SliderWidth )
	{
		FCurrentValue = GetWidth() - SliderWidth;
	}

	float dX1 = FCurrentValue;
	float dX2 = FCurrentValue + SliderWidth;

	return LVector4( GetX1() + dX1, GetY1(),
	                 GetX1() + dX2, GetY2() );
}

void clGUISlider::UpdateCVar()
{
	if ( GetConsoleVariable() )
	{
		GetConsoleVariable()->SetDouble( GetCurrentValue() );
	}
}

bool clGUISlider::ContainsSlider( const LVector2& Pnt )
{
	LVector4 Slider = GetSliderCoords();

	return Pnt.X > Slider[0] &&
	       Pnt.Y > Slider[1] &&
	       Pnt.X < Slider[2] &&
	       Pnt.Y < Slider[3];
}
/*
LVector4 clGUIRadioButton::GetUniformsVec() const
{

   return GetSliderCoords();
}
*/

float clGUISlider::GetCurrentValue() const
{
	// map current value to 0..1 range
	float CtlSize = GetWidth() - SliderWidth;

	if ( CtlSize < ::Linderdaum::Math::EPSILON )
	{
		ERROR_MSG( "Slider size is too small. Should be > SliderWidth" );

		return 0.0f;
	}

	float Value = FCurrentValue / CtlSize;

	// map to FMinValue..FMaxValue
	float Range = FMaxValue - FMinValue;

	CHECK_ERROR( Range < ::Linderdaum::Math::EPSILON, "Slider (MaxValue-MinValue) should be greater than zero" );

	Value = FMinValue + Value * Range;

	return Value;
}

void clGUISlider::SetCurrentValue( float Value )
{
	// map to 0..1
	float Range = FMaxValue - FMinValue;

	if ( Range < ::Linderdaum::Math::EPSILON )
	{
		ERROR_MSG( "Slider (MaxValue-MinValue) should be greater than zero" );

		return;
	}

	FCurrentValue = ( Value - FMinValue ) / Range;

	// map current value to 0..CtlSize range
	float CtlSize = GetWidth() - SliderWidth;

	CHECK_ERROR( CtlSize < ::Linderdaum::Math::EPSILON, "Slider size is too small. Should be > SliderWidth" );

	FCurrentValue = FCurrentValue * CtlSize;
}

void clGUISlider::Event_MouseInside( iGUIResponder* Source, const LVector2& Pnt )
{
	clGUIPanel::Event_MouseInside( Source, Pnt );

	if ( FSliderIsMoving )
	{
		LVector2 Delta = Pnt - FSliderStartPoint;

		FCurrentValue = FSliderStartValue + Delta.X;

		GetSliderCoords();

		UpdateCVar();
	}
}

void clGUISlider::Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState )
{
	clGUIPanel::Event_MouseLeft( Source, Pnt, KeyState );

	if ( KeyState )
	{
		if ( !ContainsSlider( Pnt ) )
		{
			// move the slider into picked position
			FCurrentValue = Pnt.X - GetX1() - SliderWidth * 0.5f;
		}

		if ( !FSliderIsMoving )
		{
			Env->GUI->SetCapture( this );

			FSliderIsMoving = true;

			FSliderStartPoint = Pnt;

			FSliderStartValue = FCurrentValue;
		}
	}
	else
	{
		Env->GUI->ReleaseCapture();

		FSliderIsMoving = false;
	}
}

void clGUISlider::Event_LDoubleClick( iGUIResponder* Source, const LVector2& Pnt )
{
	clGUIPanel::Event_LDoubleClick( Source, Pnt );

	Event_MouseLeft( Source, Pnt, true );
}

/*
 * 05/04/2007
     SaveToXLMLStream()
 * 25/11/2006
     Control layout changed
 * 14/11/2006
     It's here
*/
