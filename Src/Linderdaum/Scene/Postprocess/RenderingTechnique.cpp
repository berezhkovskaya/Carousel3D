/**
 * \file RenderingTechnique.cpp
 * \brief Default OpenGL rendering technique implementation
 * \version 0.5.88
 * \date 03/04/2008
 * \author Sergey Kosarevsky, 2005-2008
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "RenderingTechnique.h"
#include "Environment.h"

#include "Engine.h"
#include "Core/Linker.h"
#include "Core/Logger.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "Utils/Viewport.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/Canvas.h"
#include "Renderer/iRenderTarget.h"
#include "Geometry/GeomServ.h"
#include "Scene/Postprocess/iPostprocessor.h"
#include "Scene/Scene.h"

#include "LColors.h"

#include "HDRPP.h"
#include "Filter.h"

clRenderingTechnique::clRenderingTechnique(): FDebugShowTexture( NULL ),
	FDebugShowDepth( false ),
	FDebugEnablePostProcessing( true )
{
}

void clRenderingTechnique::AfterConstruction()
{
	FPostProcessing = Env->Console->GetVar( "EnablePostProcessingPipeline" );
	FPostProcessing->SetBool( true );
}

clRenderingTechnique::~clRenderingTechnique()
{
	ClearPostprocessingPipeline();

	Reset();
}

void clRenderingTechnique::Reset()
{
	for ( size_t i = 0; i != FRenderTargets.size(); ++i )
	{
		delete( FRenderTargets[i] );
	}

	FRenderTargets.clear();
}

void clRenderingTechnique::AddPostprocessor( iPostprocessor* Postprocessor )
{
	FPostprocessingPipeline.push_back( Postprocessor );
}

void clRenderingTechnique::ClearPostprocessingPipeline()
{
	while ( !FPostprocessingPipeline.empty() )
	{
		delete( FPostprocessingPipeline.back() );

		FPostprocessingPipeline.pop_back();
	}
}

void clRenderingTechnique::Debug_ShowDepth( bool ShowDepth )
{
	FDebugShowTexture = NULL;
	FDebugShowDepth   = ShowDepth;
}

void clRenderingTechnique::Debug_ShowTexture( iTexture* Texture )
{
	FDebugShowDepth   = false;
	FDebugShowTexture = Texture;
}

iRenderTarget* clRenderingTechnique::GetCustomRenderTarget( const LString& Name )
{
	guard();

	for ( size_t i = 0; i != FRenderTargets.size(); ++i )
	{
		if ( FRenderTargets[i]->GetObjectID() == Name )
		{
			return FRenderTargets[i];
		}
	}

	FATAL_MSG( "Unable to find custom render target: " + Name );

	return NULL;

	unguard();
}

void clRenderingTechnique::ProcessBuffer( iRenderTarget* InBuffer, iRenderTarget* OutBuffer )
{
	const int BufferWidth  = iRenderTarget::FULLSCREEN;
	const int BufferHeight = iRenderTarget::FULLSCREEN;

	// create 2 ping-pong buffers
	while ( FRenderTargets.size() < 2 )
	{
		FRenderTargets.push_back( Env->Renderer->CreateRenderTarget( BufferWidth, BufferHeight, 0, 16, false, 1 ) );
	}

	iRenderTarget* In  = InBuffer;
	iRenderTarget* Out = FRenderTargets[0];

	int ID = 0;

	for ( size_t i = 0; i != FPostprocessingPipeline.size(); ++i )
	{
		iPostprocessor* Processor = FPostprocessingPipeline[i];

		if ( Processor->IsEnabled() )
		{
			//iRenderTarget* CustomInput  = NULL;
			//iRenderTarget* CustomOutput = NULL;

			//LString CustomInputName  = Processor->GetInputRenderTargetName();
			//LString CustomOutputName = Processor->GetOutputRenderTargetName();

			//if ( !CustomInputName.empty()  ) { CustomInput  = GetCustomRenderTarget( CustomInputName ); }
			//if ( !CustomOutputName.empty() ) { CustomOutput = GetCustomRenderTarget( CustomOutputName ); }

			//Processor->Render( PrevRenderTarget, CustomInput, CustomOutput );
			if ( Processor->Render( In, NULL, Out ) )
			{
				ID = ( ID + 1 ) % 2;

				In  = Out;
				Out = FRenderTargets[ ID ];
			}

//			if ( !CustomOutput )
//			{
//				PrevRenderTarget = Processor->GetOutputRenderTarget();
//			}
		}
	}

	if ( OutBuffer ) { OutBuffer->Bind( 0 ); }

	Env->Renderer->GetCanvas()->TexturedRect( 0, 1.0f, 1.0f, 0.0f, In->GetColorTexture( 0 ), NULL, LC_White );

	if ( OutBuffer ) { OutBuffer->UnBind(); }
}

void clRenderingTechnique::Render( clScene* Scene )
{
	guard();

	Scene->RenderForward();
	//Scene->RenderDeferred();

	iRenderTarget* RenderTarget = Scene->GetRenderTarget();

	Env->Renderer->RestoreViewport();
	Env->Renderer->SetPolygonFillMode( true );

	if ( !RenderTarget ) { return; }

	if ( !Scene->GetUseOffscreenBuffer() ) { return; }

	// final pass: do postprocessing and render composite screenbuffer
	if ( FDebugShowDepth || FDebugShowTexture )
	{
		if ( FDebugShowDepth   ) { Env->Renderer->GetCanvas()->TexturedRect( 0, 1.0f, 1.0f, 0.0f, RenderTarget->GetDepthTexture(),  NULL, LC_White ); }

		if ( FDebugShowTexture ) { Env->Renderer->GetCanvas()->TexturedRect( 0, 1.0f, 1.0f, 0.0f, RenderTarget->GetColorTexture( 1 ), NULL, LC_White ); }

		return;
	}

	if ( !FPostProcessing->GetBool() )
	{
		Env->Renderer->GetCanvas()->TexturedRect( 0, 0.0f, 1.0f, 1.0f, RenderTarget->GetColorTexture( 0 ), NULL, LC_White );

		return;
	}

	RenderPostprocessingPipeline( RenderTarget );

	unguard();
}

/*
 * 03/04/2008
     Usage of viewport params from the current camera
 * 29/04/2007
     Postprocessing pipeline
 * 05/03/2007
     Major multipass rendering speedup
 * 09/01/2007
     Two pass bloom
 * 01/01/2007
     Brightpass
 * 21/11/2006
     Deallocation of render targets and custom LSSs
 * 15/11/2006
     Rendering order
 * 12/11/2006
     Code cleanup & refactoring
 * 09/11/2006
     HDR rendering, bloom and light adaptation
 * 21/08/2006
     Checking if RenderingSequence is empty
 * 03/12/2005
     Rendering into offscreen render target
 * 27/06/2005
     It's here
*/
