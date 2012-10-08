/**
 * \file ASSAO.cpp
 * \brief Accumulative Screen Space Ambient Occlusion
 * \version 0.6.00
 * \date 18/02/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "Engine.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "Utils/Viewport.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iRenderTarget.h"
#include "Renderer/Canvas.h"
#include "Renderer/RenderState.h"
#include "Renderer/iShaderProgram.h"
#include "Resources/ResourcesManager.h"

#include "ASSAO.h"

clASSAOPostprocessor::clASSAOPostprocessor()
{
}

void clASSAOPostprocessor::AfterConstruction()
{
	guard();
	/*
	   const int BufferWidth  = iRenderTarget::FULLSCREEN;
	   const int BufferHeight = iRenderTarget::FULLSCREEN;

	   FOutput = Env->Renderer->CreateRenderTarget( BufferWidth, BufferHeight, 0, 16, false, 1 );

	   // create LSS for postprocessing
	   FBypassShader          = Env->Resources->CreateCustomShader( "Programs\\PostProcessing\\HDR\\Bypass.sp", "" );
	   FBypassDepthShader     = Env->Resources->CreateCustomShader( "Programs\\PostProcessing\\HDR\\BypassDepth.sp", "" );
	   FFinalPassShader       = Env->Resources->CreateCustomShader( "Programs\\PostProcessing\\HDR\\FinalPass.sp", "" );
	   FToLuminanceShader     = Env->Resources->CreateCustomShader( "Programs\\PostProcessing\\HDR\\ToLuminance.sp", "" );
	   FDownScaleShader       = Env->Resources->CreateCustomShader( "Programs\\PostProcessing\\HDR\\DownScale.sp", "" );
	   FBloomXShader          = Env->Resources->CreateCustomShader( "Programs\\PostProcessing\\HDR\\BloomX.sp", "" );
	   FBloomYShader          = Env->Resources->CreateCustomShader( "Programs\\PostProcessing\\HDR\\BloomY.sp", "" );
	   FStreakShader          = Env->Resources->LoadShader( "Programs\\PostProcessing\\HDR\\Streaks.shader" );
	   FBrightPassShader      = Env->Resources->CreateCustomShader( "Programs\\PostProcessing\\HDR\\BrightPass.sp", "" );
	   FLightAdaptationShader = Env->Resources->CreateCustomShader( "Programs\\PostProcessing\\HDR\\Adaptation.sp", "" );
	   FFillHDRWhiteShader    = Env->Resources->CreateCustomShader( "Programs\\PostProcessing\\HDR\\FillHDRWhite.sp", "" );

	   FBypassDepthShader->FDepthWrites = true;

	   FEnabled = Env->Console->GetVar( "Postprocessing.ASSAO" );

	   //
	   // create params
	   //

	   // BloomStrength
	   clCVar* BloomStrength = Env->Console->GetVar( "BloomStrength" );
	   BloomStrength->SetDouble( 0.3f );

	   FFinalPassShader->GetShaderProgram()->BindConsoleVariable( "BloomStrength", BloomStrength );

	   // MidGray
	   clCVar* MidGray = Env->Console->GetVar( "MidGray" );
	   MidGray->SetDouble( 0.2f );

	   FFinalPassShader->GetShaderProgram()->BindConsoleVariable( "MidGray", MidGray );

	   // White
	   clCVar* White = Env->Console->GetVar( "White" );
	   White->SetDouble( 1.0f );

	   FFinalPassShader->GetShaderProgram()->BindConsoleVariable( "White", White );
	*/
	unguard();
}

clASSAOPostprocessor::~clASSAOPostprocessor()
{
	/*
	   delete( FOutput );

	   delete( FBloomX );
	   delete( FBloomY );
	   delete( FBrightPass );
	   delete( FCurrentLuminance );
	   delete( FAdaptedLuminance );
	*/
}

bool clASSAOPostprocessor::Render( iRenderTarget* RenderTarget, iRenderTarget* Input, iRenderTarget* Output )
{
	return false;

	/*
	FTransientRenderTarget = RenderTarget;

	if ( !IsEnabled() ) { return; }

	iRenderTarget* RT = ( Output ? Output : FRenderTarget );

	RT->Bind( 0 );

	FFinalPassShader->UpdateTextures( RenderTarget );

	if ( Input )
	{
	   FFilterShader->SetTexture( 2, Input->GetColorTexture( 0 ), false );
	   FFilterShader->SetTexture( 3, Input->GetDepthTexture(), false );
	}

	Env->Renderer->GetCanvas()->FullscreenRect( FFinalPassShader );

	RT->UnBind();
	*/


	/*
	   clCanvas* Cnv = Env->Renderer->GetCanvas();

	   FDownScaleShader->UpdateTextures( RenderTarget );

	   // 1. Downscale from 64x64 to 1x1 and convert to luminance
	   for ( int i = 0; i != MAX_LUMINANCE_MIPMAPS; ++i )
	   {
	      FAvgLuminance[i]->Bind( 0 );

	      Cnv->FullscreenRect( ( i == MAX_LUMINANCE_MIPMAPS - 1 ) ? FToLuminanceShader : FDownScaleShader );

	      FAvgLuminance[i]->UnBind();

	      FDownScaleShader->UpdateTextures( FAvgLuminance[i] );
	      FToLuminanceShader->UpdateTextures( FAvgLuminance[i] );
	   }

	   // 2. Convert to luminance
	   // 3. Light adaptation
	   FLightAdaptationShader->SetTexture( 0, FAvgLuminance[MAX_LUMINANCE_MIPMAPS-1]->GetColorTexture( 0 ), false );
	   FLightAdaptationShader->SetTexture( 1, FCurrentLuminance->GetColorTexture( 0 ), false );

	   FAdaptedLuminance->Bind( 0 );

	   Cnv->FullscreenRect( FLightAdaptationShader );

	   FAdaptedLuminance->UnBind();

	   // 4. Bright pass
	   FBrightPassShader->SetTexture( 0, RenderTarget->GetColorTexture( 0 ), false );
	   FBrightPass->Bind( 0 );
	   Cnv->FullscreenRect( FBrightPassShader );
	   FBrightPass->UnBind();

	   // 5. X bloom
	   FBloomXShader->SetTexture( 0, FBrightPass->GetColorTexture( 0 ), false );
	   FBloomX->Bind( 0 );
	   Cnv->FullscreenRect( FBloomXShader );
	   FBloomX->UnBind();

	   // 6. Y bloom
	   FBloomYShader->SetTexture( 0, FBloomX->GetColorTexture( 0 ), false );
	   FBloomY->Bind( 0 );
	   Cnv->FullscreenRect( FBloomYShader );
	   FBloomY->UnBind();

	   // 7. X bloom 2x
	   FBloomXShader->SetTexture( 0, FBloomY->GetColorTexture( 0 ), false );
	   FBloomX->Bind( 0 );
	   Cnv->FullscreenRect( FBloomXShader );
	   FBloomX->UnBind();

	   // 8. Y bloom 2x
	   FBloomYShader->SetTexture( 0, FBloomX->GetColorTexture( 0 ), false );
	   FBloomY->Bind( 0 );
	   Cnv->FullscreenRect( FBloomYShader );
	   FBloomY->UnBind();

	   FBloomXShader->SetTexture( 0, FBloomY->GetColorTexture( 0 ), false );
	   FBloomX->Bind( 0 );
	   Cnv->FullscreenRect( FStreakShader );
	   FBloomX->UnBind();

	   FStreakShader->SetTexture( 0, FBloomX->GetColorTexture( 0 ), false );
	   FBloomY->Bind( 0 );
	   Cnv->FullscreenRect( FStreakShader );
	   FBloomY->UnBind();


	   // 7. Tone mapping
	   FOutput->Bind( 0 );

	   FFinalPassShader->UpdateTextures( RenderTarget );

	   FFinalPassShader->SetTexture( 1, FAdaptedLuminance->GetColorTexture( 0 ), false );
	   FFinalPassShader->SetTexture( 2, FBloomY->GetColorTexture( 0 ), false );

	   Cnv->FullscreenRect( FFinalPassShader );

	   FOutput->UnBind();

	   // swap luminances
	   iRenderTarget* Temp = FCurrentLuminance;

	   FCurrentLuminance = FAdaptedLuminance;

	   FAdaptedLuminance = Temp;
	*/
}

/*
 * 18/02/2011
     It's here
*/
