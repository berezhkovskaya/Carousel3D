#include "I_ColorDial.h"

#include "Core/Console.h"
#include "Environment.h"
#include "Resources/ResourcesManager.h"

#include "Math/LMath.h"
#include "Renderer/iShaderProgram.h"
#include "Renderer/RenderState.h"

#include "LColors.h"

clGUIColorDial::clGUIColorDial():
	FRMin( 0.5f ),
	FRMax( 0.75f ),
	FMousePoint( 0, 0, 1 ),
	FAlpha( 0.0f ),
	FPressed( false ),
	FOutVarName( "TheEntityColor" )
{
}

void clGUIColorDial::SetAlpha( const LVector2& Pnt )
{
	LVector2 dP = Screen2Local( Pnt );
	dP = LVector2( dP.X / GetWidth(), dP.Y / GetHeight() );

	float nx = ( dP.X - 0.5f ) * 2.0f;
	float ny = ( dP.Y - 0.5f ) * 2.0f;

	float r = sqrt( nx * nx + ny * ny );

	float r_min = FRMin;

	LVector2 p_color = -Math::ToPolar( r_min, FAlpha );
	LVector2 p_white = -Math::ToPolar( r_min, FAlpha + 120.0f );
	LVector2 p_black = -Math::ToPolar( r_min, FAlpha + 240.0f );

	LVector3 Mark = LVector3( FMousePoint.X * p_black + FMousePoint.Y * p_white + FMousePoint.Z * p_color, 1.0f );

	DialSP->BindUniforms();
	DialSP->SetUniformVec3Array( FMousePtUniform, 1, Mark );

	if ( r < FRMax && r > FRMin )
	{
		FAlpha = 180.0f + Linderdaum::Math::RadToDeg( atan2( ny, nx ) );

		DialSP->SetUniformFloat( FAlphaUniform, FAlpha );
	}
	else
	{
		LVector3 NewB = Math::Barycentric2D( nx, ny, p_black.X, p_black.Y, p_white.X, p_white.Y, p_color.X, p_color.Y );

		if ( NewB.X >= 0 && NewB.X <= 1 && NewB.Y >= 0 && NewB.Y <= 1 && NewB.Z >= 0 && NewB.Z <= 1 )
		{
			FMousePoint = NewB;
		}
	}

	LVector3 CC = FMousePoint.X * LC_Black.ToVector3() + FMousePoint.Y * LC_White.ToVector3() + FMousePoint.Z * Math::ColorFromAngle( FAlpha );

	Env->Console->GetVar( FOutVarName )->SetVector4( LVector4( CC, 1.0f ) );
}

void clGUIColorDial::AfterConstruction()
{
	clGUIPanel::AfterConstruction();

	DialShader = Env->Resources->CreateCustomShader( "Interface/Programs/ColorDial.sp", "" );

	SetShader( DialShader );
	DialSP = DialShader->GetShaderProgram();

	FAlphaUniform   = DialSP->CreateUniform( "DialAlpha" );
	FMousePtUniform = DialSP->CreateUniform( "MousePoint" );
	FRMinUniform    = DialSP->CreateUniform( "r_min" );
	FRMaxUniform    = DialSP->CreateUniform( "r_max" );

	DialSP->SetUniformFloat( FRMinUniform,  FRMin );
	DialSP->SetUniformFloat( FRMaxUniform,  FRMax );
	DialSP->SetUniformFloat( FAlphaUniform, FAlpha );
	DialSP->SetUniformVec3Array( FMousePtUniform, 1, FMousePoint );

	DialShader->FBlended = true;
	DialShader->SetBlending( L_SRC_ALPHA, L_ONE_MINUS_SRC_ALPHA );
}

/*
 * 12/01/2011
     It's here
*/
