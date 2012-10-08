/**
 * \file SoftFrameBuffer.cpp
 * \brief MRTs via software rendering
 * \version 0.5.96
 * \date 10/06/2010
 * \author Viktor Latypov, 2010
 * \author viktor@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "SoftFrameBuffer.h"
//#include "SoftRenderContext.h"

#include "Engine.h"
#include "Core/Linker.h"
#include "Core/Logger.h"
#include "Core/Console.h"
#include "Utils/Viewport.h"
#include "Renderer/iTexture.h"
#include "Renderer/iRenderContext.h"
#include "Images/Image.h"

void clSoftFrameBuffer::InitRenderTarget( const int  Width,
                                          const int  Height,
                                          const int  Depth,
                                          const int  BitsPerChannel,
                                          const bool HasDepthBuffer,
                                          const int  MRTCount )
{
	LArray<LVector4i> Params;

	for ( int i = 0; i != MRTCount; i++ )
	{
		Params.push_back( LVector4i( Width, Height, Depth, BitsPerChannel ) );
	}

	InitRenderTargetV( Params, HasDepthBuffer );
}

void clSoftFrameBuffer::InitRenderTargetV( const LArray<LVector4i>& WidthHeightDepthBitsPerChannel,
                                           const bool HasDepthBuffer )
{
	guard();

	FColorBuffersParams = WidthHeightDepthBitsPerChannel;

	FColorBuffers.clear();

	for ( size_t i = 0; i != FColorBuffersParams.size(); i++ )
	{
		// new color buffer
		iTexture* ColorBuffer = Env->Renderer->CreateTexture();

		clImage* ColorImage = NULL; //...

//      ColorImage->SetBitmap( attach some bitmap with specified dimensions);

		// the image defines everything
		ColorBuffer->SetImage( ColorImage );

		FColorBuffers.push_back( ColorBuffer );
	}

	// depth buffer
	if ( HasDepthBuffer )
	{
		FDepthBuffer = Env->Renderer->CreateTexture();

		clImage* DepthImage = NULL; //...

//      ColorImage->SetBitmap( attach some bitmap with specified dimensions);

		// the image defines everything
		FDepthBuffer->SetImage( DepthImage );

//      int Width  = FColorBuffersParams[0][0];
//      int Height = FColorBuffersParams[0][1];
	}

	unguard();
}

clSoftFrameBuffer::~clSoftFrameBuffer()
{
	delete( FDepthBuffer  );

	for ( size_t i = 0; i != FColorBuffers.size(); ++i )
	{
		delete( FColorBuffers[i] );
	}
}

void clSoftFrameBuffer::Bind( int TargetIndex ) const
{
//   glBindFramebuffer( GL_FRAMEBUFFER, FFrameBuffer );

	Env->Renderer->SetViewport( 0, 0, FColorBuffersParams[0][0], FColorBuffersParams[0][1] );
	/*
	   if ( FColorBuffers.size() > 1 )
	   {
	       static const GLenum Buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

	       glDrawBuffers( FColorBuffers.size(), Buffers );
	   }
	*/
}

void clSoftFrameBuffer::BindReadFrom( int TargetIndex ) const
{
	// WTF ?
//   Env->Renderer->SetViewport( 0, 0, FColorBuffersParams[0][0], FColorBuffersParams[0][1] );
}

void clSoftFrameBuffer::UnBind() const
{
	// tell context we are detaching ?

	// restore view params
	Env->Renderer->RestoreViewport();
}

/*
 * 10/06/2010
     Initial version
*/
