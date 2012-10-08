/*
   (Part of Linderdaum Engine)
   Version 0.5.73
   (05/04/2007)
   (C) Sergey Kosarevsky, 2005-2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_Gauge.h"

#include "Engine.h"
#include "Environment.h"
#include "GUI/GUIManager.h"
#include "Renderer/iRenderContext.h"
#include "Resources/ResourcesManager.h"
#include "Core/CVars.h"

void clGUIGauge::PreRender()
{
	clGUIBorderPanel::PreRender();

	if ( !GetTitle().empty() )
	{
		OutTextXY( CenterTextHorizontal( GetTitle() ),
		           Env->GUI->GetDefaultGaugeTextYOffset(),
		           GetTitle(),
		           GetDefaultColor() );
	}
}

void clGUIGauge::Event_Registered( iGUIResponder* Source )
{
	clGUIBorderPanel::Event_Registered( Source );

	SetShader( Env->Resources->LoadShader( "interface/shaders/gauge.shader" ) );
}

void clGUIGauge::Event_Timer( iGUIResponder* Source, float DeltaTime )
{
	clGUIBorderPanel::Event_Timer( Source, DeltaTime );

	if ( GetConsoleVariable() )
	{
		SetCurrentValue( static_cast<int>( GetConsoleVariable()->GetFloat() * 1000.0f ) );
	}
}

LVector4 clGUIGauge::GetUniformsVec() const
{
	static const float OffsetX = Env->GUI->GetDefaultGaugeXOffset();
	static const float OffsetY = Env->GUI->GetDefaultGaugeYOffset();

	float Percent = 1.0f;

	if ( FMaxValue > 0 )
	{
		Percent = static_cast<float>( FCurrentValue ) / static_cast<float>( FMaxValue );
	}

	float BarLen = ( GetX2() - GetX1() ) * Percent;

	return LVector4( GetX1() + OffsetX, GetY1() + OffsetY, GetX1() + BarLen/*-OffsetX*/, GetY2() - OffsetY );
}

void clGUIGauge::Increment( int Delta )
{
	SetCurrentValue( GetCurrentValue() + Delta );
}

/*
 * 05/04/2007
     SaveToXLMLStream()
 * 06/05/2005
     It's here
*/
