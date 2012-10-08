/**
 * \file FeedbackScreen.cpp
 * \brief Image-space feedback effect
 * \version 0.5.74
 * \date 06/06/2007
 * \author Sergey Kosarevsky, 2007-2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Engine.h"
#include "Environment.h"
#include "Core/Console.h"
#include "Utils/Viewport.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iRenderTarget.h"
#include "Renderer/Canvas.h"
#include "Renderer/RenderState.h"
#include "Renderer/iShaderProgram.h"
#include "Resources/ResourcesManager.h"
#include "Math/LMathStrings.h"

#include "FeedbackScreen.h"

clFeedbackScreenPostprocessor::clFeedbackScreenPostprocessor(): FEnabled( false )
{
	FUnitHitDuration   = 0.3f;
	FUnitHitColor1     = LVector4( 1, 0, 0, 0 );
	FUnitHitColor2     = LVector4( 1, 0, 0, 0 );
	FUnitHitShrinkCoef = -0.2f;

	FPowerUpDuration   = 0.3f;
	FPowerUpColor1     = LVector4( 0, 1, 0, 0 );
	FPowerUpColor2     = LVector4( 0, 1, 0, 0 );
	FPowerUpShrinkCoef = -0.2f;

	FPerkDuration      = 0.3f;
	FPerkColor1        = LVector4( 1, 1, 0, 0 );
	FPerkColor2        = LVector4( 1, 1, 0, 0 );
	FPerkShrinkCoef    = +0.2f;

	FShotDuration      = 0.3f;
	FShotColor1        = LVector4( 0, 0, 0, 0 );
	FShotColor2        = LVector4( 0, 0, 0, 0 );
	FShotShrinkCoef    = +0.2f;
}

void clFeedbackScreenPostprocessor::AfterConstruction()
{
	guard();

	iPostprocessor::AfterConstruction();

	FShader = Env->Resources->LoadShader( "Programs/PostProcessing/FeedbackScreen/FeedbackScreen.shader" );
	FShaderAdditive = Env->Resources->LoadShader( "Programs/PostProcessing/FeedbackScreen/FeedbackScreen_Additive.shader" );

	Env->Console->RegisterCommand( "FeedbackScreenEffect", Utils::Bind( &clFeedbackScreenPostprocessor::FeedbackScreenEffectC, this ) );

	unguard();
}

clFeedbackScreenPostprocessor::~clFeedbackScreenPostprocessor()
{
	Env->Console->UnRegisterCommand( "FeedbackScreenEffect", Utils::Bind( &clFeedbackScreenPostprocessor::FeedbackScreenEffectC, this ) );
}

void clFeedbackScreenPostprocessor::FeedbackScreenEffectC( const LString& Param )
{
	iShaderProgram* SP = FShader->GetShaderProgram();

	if ( Param == "UNITHIT" )
	{
		FEffectDuration = FUnitHitDuration;
		SP->BindUniforms();
		SP->SetUniformNameVec4Array( "EFFECT_COLOR1", 1 , FUnitHitColor1 );
		SP->SetUniformNameVec4Array( "EFFECT_COLOR2", 1 , FUnitHitColor2 );
		SP->SetUniformNameFloat( "SHRINK_COEF",  FUnitHitShrinkCoef );
	}
	else if ( Param == "POWERUP" )
	{
		FEffectDuration = FPowerUpDuration;
		SP->BindUniforms();
		SP->SetUniformNameVec4Array( "EFFECT_COLOR1", 1, FPowerUpColor1 );
		SP->SetUniformNameVec4Array( "EFFECT_COLOR2", 1, FPowerUpColor2 );
		SP->SetUniformNameFloat( "SHRINK_COEF",  FPowerUpShrinkCoef );
	}
	else if ( Param == "PERK" )
	{
		FEffectDuration = FPerkDuration;
		SP->BindUniforms();
		SP->SetUniformNameVec4Array( "EFFECT_COLOR1", 1, FPerkColor1 );
		SP->SetUniformNameVec4Array( "EFFECT_COLOR2", 1, FPerkColor2 );
		SP->SetUniformNameFloat( "SHRINK_COEF",  FPerkShrinkCoef );
	}
	else if ( Param == "SHOT" )
	{
		FEffectDuration = FShotDuration;
		SP->BindUniforms();
		SP->SetUniformNameVec4Array( "EFFECT_COLOR1", 1, FShotColor1 );
		SP->SetUniformNameVec4Array( "EFFECT_COLOR2", 1, FShotColor2 );
		SP->SetUniformNameFloat( "SHRINK_COEF",  FShotShrinkCoef );
	}

	if ( Param == "UNITHIT" ||
	     Param == "POWERUP" ||
	     Param == "PERK"    ||
	     Param == "SHOT" )
	{
		FEnabled = true;

		FEffectStartTime = static_cast<float>( Env->GetEngineTime() );

		SP->BindUniforms();
		SP->SetUniformNameFloat( "EFFECT_START_TIME", FEffectStartTime );
		SP->SetUniformNameFloat( "EFFECT_DURATION",   FEffectDuration );
	}
}

bool clFeedbackScreenPostprocessor::IsEnabled() const
{
	return FEnabled;
}

bool clFeedbackScreenPostprocessor::Render( iRenderTarget* RenderTarget, iRenderTarget* Input, iRenderTarget* Output )
{
	if ( !FEnabled ) { return false; }

	if ( Env->GetEngineTime() - FEffectStartTime > FEffectDuration )
	{
		FEnabled = false;

		return false;
	}

	if ( Output && RenderTarget )
	{
		Output->Bind( 0 );

		FShader->UpdateTextures( RenderTarget );

		Env->Renderer->GetCanvas()->FullscreenRect( FShader );

		Output->UnBind();
	}
	else
	{
		Env->Renderer->GetCanvas()->FullscreenRect( FShaderAdditive );
	}

	return true;
}

/*
 * 25/06/2007
     Two colors for each effect state
 * 23/06/2007
     It's here
*/
