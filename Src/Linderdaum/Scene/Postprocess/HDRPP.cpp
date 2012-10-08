/**
 * \file HDRPP.cpp
 * \brief High dynamic range postprocessor
 * \version 0.5.74
 * \date 20/04/2007
 * \author Sergey Kosarevsky, 2007
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

#include "HDRPP.h"

clHDRPostprocessor::clHDRPostprocessor()
{
}

void clHDRPostprocessor::AfterConstruction()
{
	guard();

	// rendertargets for bloom & brightpass
	const int BloomBufferWidth  = 256;
	const int BloomBufferHeight = 256;

	FBloomX     = Env->Renderer->CreateRenderTarget( BloomBufferWidth, BloomBufferHeight, 0, 16, false, 1 );
	FBloomY     = Env->Renderer->CreateRenderTarget( BloomBufferWidth, BloomBufferHeight, 0, 16, false, 1 );
	FBrightPass = Env->Renderer->CreateRenderTarget( BloomBufferWidth, BloomBufferHeight, 0, 16, false, 1 );

	// array of rendertargets for average luminance
	FAvgLuminance[0] = Env->Renderer->CreateRenderTarget( 64, 64, 0, 16, false, 1 );
	FAvgLuminance[1] = Env->Renderer->CreateRenderTarget( 16, 16, 0, 16, false, 1 );
	FAvgLuminance[2] = Env->Renderer->CreateRenderTarget(  4,  4, 0, 16, false, 1 );
	FAvgLuminance[3] = Env->Renderer->CreateRenderTarget(  1,  1, 0, 16, false, 1 );

	// rendertargets for luminance adaptation
	FCurrentLuminance = Env->Renderer->CreateRenderTarget( 1, 1, 0, 16, false, 1 );
	FAdaptedLuminance = Env->Renderer->CreateRenderTarget( 1, 1, 0, 16, false, 1 );

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

	// initialize luminance adaptation chain with realy bright white color
	FCurrentLuminance->Bind( 0 );

	Env->Renderer->GetCanvas()->FullscreenRect( FFillHDRWhiteShader );

	FCurrentLuminance->UnBind();

	SetEnableVarName( "EnablePostProcessing" );

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

	unguard();
}

clHDRPostprocessor::~clHDRPostprocessor()
{
	// shaders are managed
	delete( FBloomX );
	delete( FBloomY );
	delete( FBrightPass );
	delete( FCurrentLuminance );
	delete( FAdaptedLuminance );

	for ( int i = 0; i != MAX_LUMINANCE_MIPMAPS; ++i )
	{
		delete( FAvgLuminance[i] );
	}
}

bool clHDRPostprocessor::Render( iRenderTarget* RenderTarget, iRenderTarget* Input, iRenderTarget* Output )
{
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
	Output->Bind( 0 );

	FFinalPassShader->UpdateTextures( RenderTarget );

	FFinalPassShader->SetTexture( 1, FAdaptedLuminance->GetColorTexture( 0 ), false );
	FFinalPassShader->SetTexture( 2, FBloomY->GetColorTexture( 0 ), false );

	Cnv->FullscreenRect( FFinalPassShader );

	Output->UnBind();

	// swap luminances
	iRenderTarget* Temp = FCurrentLuminance;

	FCurrentLuminance = FAdaptedLuminance;

	FAdaptedLuminance = Temp;

	return true;
}

/*
 * 28/04/2007
     It's here
*/
