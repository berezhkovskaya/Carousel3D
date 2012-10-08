#include "MLAA.h"

#include "Environment.h"
#include "Utils/Viewport.h"
#include "Renderer/RenderState.h"
#include "Renderer/iShaderProgram.h"
#include "Renderer/iRenderTarget.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/Canvas.h"

void clMLAAPostprocessor::AfterConstruction()
{
	guard();

	const int BufferWidth  = iRenderTarget::FULLSCREEN;
	const int BufferHeight = iRenderTarget::FULLSCREEN;

	Init( BufferWidth, BufferHeight );

	unguard();
}

bool clMLAAPostprocessor::Render( iRenderTarget* RenderTarget, iRenderTarget* Input, iRenderTarget* Output )
{
	if ( !IsEnabled() ) { return false; }

	clCanvas* C = Env->Renderer->GetCanvas();

	iRenderTarget* SourceRT = RenderTarget;

	// 0. Optional preprocess
	if ( FUsePrepass )
	{
		FPrepassRT->Bind( 0 );
		FPrepassShader->SetTexture( 0, SourceRT->GetColorTexture( 0 ), false );
		FPrepassShader->SetTexture( 1, SourceRT->GetDepthTexture(), false );
		C->FullscreenRect( FPrepassShader );
		FPrepassRT->UnBind();

		SourceRT = FPrepassRT;
	}

	// 1. Calculate discontinuities

	FTmpLineColRT->Bind( 0 );
	FTmpLinesRT[0]->Bind( 1 );
	FLineColShader->SetTexture( 0, SourceRT->GetColorTexture( 0 ), false );
	C->FullscreenRect( FLineColShader );
	FTmpLinesRT[0]->UnBind();
	FTmpLineColRT->UnBind();

	// 2. Detect vertical/horizontal lines
	float BaseLevel = 1.;
	int Dest = 0;

	// do not rebind four times
	iShaderProgram* DistSP = FDistanceCalcShader->GetShaderProgram();
	DistSP->BindUniforms();

	for ( int i = 0 ; i < 4 ; i++ )
	{
		FTmpLinesRT[1 - Dest]->Bind( 0 );
		FDistanceCalcShader->SetTexture( 0, FTmpLinesRT[Dest]->GetColorTexture( 0 ), false );
		DistSP->SetUniformFloat( FThreshold_Uniform, BaseLevel / 255.0f );
		C->FullscreenRect( FDistanceCalcShader );
		FTmpLinesRT[1 - Dest]->UnBind();

		Dest = 1 - Dest;
		BaseLevel *= 4.0f;
	}

	// 3. Calculate alpha mask
	FTmpAlphaRT->Bind( 0 );
	FCalcAlphaShader->SetTexture( 1, FTmpLinesRT[Dest]->GetColorTexture( 0 ), false );
	C->FullscreenRect( FCalcAlphaShader );
	FTmpAlphaRT->UnBind();

	// 4. Form the final image
	Output->Bind( 0 );
	FBlendShader->SetTexture( 0, SourceRT->GetColorTexture( 0 ), false );
	C->FullscreenRect( FBlendShader );
	Output->UnBind();

	return true;
}

void LoadImageSize( iShaderProgram* SP, int W, int H )
{
	LVector4 Size( 1.0f / ( float )W, 1.0f / ( float )H, 255.0f / ( float )W, 255.0f / ( float )H );

	SP->BindUniforms();
	SP->SetUniformNameVec4Array( "coeff_image", 1, Size );
}

void clMLAAPostprocessor::SetImageSize( int W, int H )
{
	LoadImageSize( FPrepassShader->GetShaderProgram(),      W, H );
	LoadImageSize( FBlendShader->GetShaderProgram(),        W, H );
	LoadImageSize( FCalcAlphaShader->GetShaderProgram(),    W, H );
	LoadImageSize( FDistanceCalcShader->GetShaderProgram(), W, H );
	LoadImageSize( FLineColShader->GetShaderProgram(),      W, H );
}

float clMLAAPostprocessor::GetDiscontinuity() const
{
	return FDiscontinuity;
}

void clMLAAPostprocessor::SetDiscontinuity( float D )
{
	FDiscontinuity = D;
	FPrepassShader->GetShaderProgram()->BindUniforms();
	FPrepassShader->GetShaderProgram()->SetUniformNameFloat( "disc_factor", FDiscontinuity );
	FLineColShader->GetShaderProgram()->BindUniforms();
	FLineColShader->GetShaderProgram()->SetUniformNameFloat( "disc_factor", FDiscontinuity );
}

void clMLAAPostprocessor::Init( int W, int H )
{
	FPrepassRT = Env->Renderer->CreateRenderTarget( W, H, 1, 8, false, 1 );
	FTmpLineColRT = Env->Renderer->CreateRenderTarget( W, H, 1, 8, false, 1 );
	FTmpAlphaRT = Env->Renderer->CreateRenderTarget( W, H, 1, 8, false, 1 );
	FTmpLinesRT[0] = Env->Renderer->CreateRenderTarget( W, H, 1, 8, false, 1 );
	FTmpLinesRT[1] = Env->Renderer->CreateRenderTarget( W, H, 1, 8, false, 1 );

//	FPrepassShader = Env->Renderer->CreateCustomShader_VS_PS("prepass", "connexity_C8", "USE_DEPTH");
	FPrepassShader->GetShaderProgram()->BindUniforms();
	FPrepassShader->GetShaderProgram()->SetUniformNameVec4Array( "col1", 1, vec4( 1, 2, 4, 8 ) );
	FPrepassShader->GetShaderProgram()->SetUniformNameVec4Array( "col2", 1, vec4( 16, 32, 64, 128 ) );

//	FCalcAlphaShader    = Env->Renderer->CreateCustomShader_VS_PS("calc_alpha_common", "connexity_C4", "");
	FCalcAlphaShader->SetTexture( 0, FTmpLineColRT->GetColorTexture( 0 ), false );

//	FBlendShader        = Env->Renderer->CreateCustomShader_VS_PS("final_blend","connexity_C4", "");
	FBlendShader->SetTexture( 1, FTmpAlphaRT->GetColorTexture( 0 ), false );

//	FLineColShader      = Env->Renderer->CreateCustomShader_VS_PS("line_col_detect_color_optim","connexity_C4", "");

//	FDistanceCalcShader = Env->Renderer->CreateCustomShader_VS_PS("distance_computation_optim","connexity_C4", "");
	FDistanceCalcShader->SetTexture( 0, FTmpLineColRT->GetColorTexture( 0 ), false );
	FThreshold_Uniform = FDistanceCalcShader->GetShaderProgram()->CreateUniform( "threshold" );

	SetImageSize( W, H );
}

void clMLAAPostprocessor::Destroy()
{
	FPrepassRT->DisposeObject();
	FTmpLineColRT->DisposeObject();
	FTmpAlphaRT->DisposeObject();
	FTmpLinesRT[0]->DisposeObject();
	FTmpLinesRT[1]->DisposeObject();

	FPrepassShader->DisposeObject();
	FCalcAlphaShader->DisposeObject();
	FBlendShader->DisposeObject();
	FLineColShader->DisposeObject();
	FDistanceCalcShader->DisposeObject();

	delete( FRenderTarget );
}
/*
iRenderTarget* clMLAAPostprocessor::GetOutputRenderTarget() const
{
   return FRenderTarget;
}
*/
/*
 * 24/02/2011
     Implementing...
*/
