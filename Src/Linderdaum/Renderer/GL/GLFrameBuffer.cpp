/**
 * \file GLFrameBuffer.cpp
 * \brief MRTs via GL framebuffer object
 * \version 0.5.91
 * \date 16/02/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "GLFrameBuffer.h"

#include "Engine.h"
#include "Core/Linker.h"
#include "Core/Logger.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "Utils/Viewport.h"
#include "Renderer/Canvas.h"
#include "Renderer/iTexture.h"
#include "Renderer/iShaderProgram.h"
#include "GLRenderContext.h"

#include "Generated/LGL/LGL.h"

#include <algorithm>

void clGLFrameBuffer::EVENT_HANDLER( Event_SURFACE_DETACHED )
{
	Env->Logger->LogP( L_DEBUG, "clGLFrameBuffer::EVENT_HANDLER( Event_SURFACE_DETACHED )" );

	FFrameBuffer = 0;

	LGL3->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void clGLFrameBuffer::EVENT_HANDLER( Event_SURFACE_ATTACHED )
{
	Env->Logger->LogP( L_DEBUG, "clGLFrameBuffer::EVENT_HANDLER( Event_SURFACE_ATTACHED )" );


//	delete( FDepthBuffer  );
	FDepthBuffer = NULL;

	for ( size_t i = 0; i != FColorBuffers.size(); ++i )
	{
		//delete( FColorBuffers[i] );
		FColorBuffers[i] = NULL;
	}

	InitRenderTargetV( FColorBuffersParams, FHasDepthBuffer );
}

void clGLFrameBuffer::InitRenderTarget( const int  Width,
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

void clGLFrameBuffer::InitRenderTargetV( const LArray<LVector4i>& WidthHeightDepthBitsPerChannel,
                                         const bool HasDepthBuffer )
{
	guard();

	CheckRendererError( "" );

	FColorBuffersParams = WidthHeightDepthBitsPerChannel;
	FHasDepthBuffer     = HasDepthBuffer;

	for ( size_t i = 0; i != FColorBuffersParams.size(); i++ )
	{
//#if defined( OS_ANDROID )
//      FColorBuffersParams[i][0] = 256;
//      FColorBuffersParams[i][1] = 256;
//      FColorBuffersParams[i][2] = 0;
//      FColorBuffersParams[i][0] = ::Linderdaum::Math::GetNextPowerOf2( FColorBuffersParams[i][0] );
//      FColorBuffersParams[i][1] = ::Linderdaum::Math::GetNextPowerOf2( FColorBuffersParams[i][1] );
//      FColorBuffersParams[i][2] = ::Linderdaum::Math::GetNextPowerOf2( FColorBuffersParams[i][2] );
//#endif
	}

	LGL3->glGenFramebuffers( 1, &FFrameBuffer );

	Bind( 0 );

	FColorBuffers.clear();

	CheckRendererError( "" );

	FMemSize = 0;

	for ( size_t i = 0; i != FColorBuffersParams.size(); i++ )
	{
		// new color buffer
		iTexture* ColorBuffer = Env->Renderer->CreateTexture();

		int Width       = FColorBuffersParams[i][0];
		int Height      = FColorBuffersParams[i][1];
		int Depth       = FColorBuffersParams[i][2];
		int BitsPerChan = FColorBuffersParams[i][3];

		if ( Width  == -1 ) { Width  = Env->Viewport->GetWidth(); }

		if ( Height == -1 ) { Height = Env->Viewport->GetHeight(); }

#if defined( OS_ANDROID )
		Lenum InternalFormat = GL_RGBA;

		// force low-quality buffer
		BitsPerChan = 8;
#else
		Lenum InternalFormat = GL_RGBA8;
#endif

		FMemSize += Width * Height * std::max( 1, Depth ) * BitsPerChan / 2;

		Env->Logger->Log( L_NOTICE, "RT: " + LStr::ToStr( Width ) + "x" + LStr::ToStr( Height ) + "x" + LStr::ToStr( Depth ) + "@" + LStr::ToStr( BitsPerChan ) );

		const Lenum Target = ( Depth > 1 ) ? GL_TEXTURE_3D : GL_TEXTURE_2D;

		if ( BitsPerChan == 8  )
		{
#if defined( OS_ANDROID )
			InternalFormat = GL_RGBA;
#else
			InternalFormat = GL_RGBA8;
#endif
		}

		if ( BitsPerChan == 16 )
		{
			InternalFormat = GL_RGBA16F;
		}

		if ( BitsPerChan == 32 )
		{
			InternalFormat = GL_RGBA32F;
		}

		ColorBuffer->SetFormat( Target, InternalFormat, GL_RGBA, Width, Height, Depth );
		ColorBuffer->AttachToCurrentFB( GL_COLOR_ATTACHMENT0 + static_cast<int>( i ), 0 );

		FColorBuffers.push_back( ColorBuffer );
	}

	CheckRendererError( "" );

#if !defined( OS_ANDROID )

	// depth buffer
	if ( HasDepthBuffer )
	{
		FDepthBuffer = Env->Renderer->CreateTexture();

		int Width  = FColorBuffersParams[0][0];
		int Height = FColorBuffersParams[0][1];

		if ( Width  == -1 ) { Width  = Env->Viewport->GetWidth(); }

		if ( Height == -1 ) { Height = Env->Viewport->GetHeight(); }

		FMemSize += Width * Height * 3;

#if defined( USE_OPENGL_3 )
		FDepthBuffer->SetFormat( GL_TEXTURE_2D, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, Width, Height, 1 );
#else
		Lenum BufferBits = ( LGL3->Is24DepthBufferSupported ) ? GL_DEPTH_COMPONENT24 : GL_DEPTH_COMPONENT16;
		FDepthBuffer->SetFormat( GL_TEXTURE_2D, BufferBits , GL_DEPTH_COMPONENT, Width, Height, 1 );
#endif
		FDepthBuffer->AttachToCurrentFB( GL_DEPTH_ATTACHMENT, 0 );
	}

#endif

	CheckRendererError( "" );

	CheckFrameBuffer();

	UnBind();

	CheckRendererError( "" );

	clCVar* AllocatedBytesRT = Env->Console->GetVar( "AllocatedBytesRT" );

	AllocatedBytesRT->AddInt( FMemSize );

	unguard();
}

clGLFrameBuffer::~clGLFrameBuffer()
{
	Env->DisconnectObject( this );

	clCVar* AllocatedBytesRT = Env->Console->GetVar( "AllocatedBytesRT" );

	AllocatedBytesRT->AddInt( -FMemSize );

	LGL3->glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	LGL3->glDeleteFramebuffers( 1, &FFrameBuffer );

	delete( FDepthBuffer  );

	for ( size_t i = 0; i != FColorBuffers.size(); ++i )
	{
		delete( FColorBuffers[i] );
	}
}

void clGLFrameBuffer::AfterConstruction()
{
	iRenderTarget::AfterConstruction();

	GetAPI( Env );

	Env->Connect( L_EVENT_SURFACE_ATTACHED, BIND( &clGLFrameBuffer::Event_SURFACE_ATTACHED ) );
	Env->Connect( L_EVENT_SURFACE_DETACHED, BIND( &clGLFrameBuffer::Event_SURFACE_DETACHED ) );
}

void clGLFrameBuffer::CheckFrameBuffer() const
{
	guard();

	CheckRendererError( "" );

	Bind( 0 );

	CheckRendererError( "" );

	GLenum FBStatus = LGL3->glCheckFramebufferStatus( GL_FRAMEBUFFER );

	CheckRendererError( "" );

	switch ( FBStatus )
	{
		case GL_FRAMEBUFFER_COMPLETE: // Everything's OK
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			FATAL_MSG( "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" );
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			FATAL_MSG( "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" );
			break;
#if !defined( OS_ANDROID )
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			FATAL_MSG( "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" );
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			FATAL_MSG( "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" );
			break;
#endif
#if defined( USE_OPENGL_ES_2 )
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
			FATAL_MSG( "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS" );
			break;
#endif
		case GL_FRAMEBUFFER_UNSUPPORTED:
			FATAL_MSG( "GL_FRAMEBUFFER_UNSUPPORTED" );
			break;
		default:
			FATAL_MSG( "Unknown framebuffer error: " + LStr::ToStrHex( FBStatus ) );
	}

	CheckRendererError( "" );

	UnBind();

	CheckRendererError( "" );

	unguard();
}

void clGLFrameBuffer::Bind( int TargetIndex ) const
{
	LGL3->glBindFramebuffer( GL_FRAMEBUFFER, FFrameBuffer );

	int Width  = FColorBuffersParams[0][0];
	int Height = FColorBuffersParams[0][1];

	if ( Width == -1 ) { Width = Env->Viewport->GetWidth(); }

	if ( Height == -1 ) { Height = Env->Viewport->GetHeight(); }

	Env->Renderer->SetViewport( 0, 0, Width, Height );

#if defined( USE_OPENGL_3 )

	if ( FColorBuffers.size() > 1 )
	{
		static const GLenum Buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

		LGL3->glDrawBuffers( static_cast<GLsizei>( FColorBuffers.size() ), Buffers );
	}

#endif
}

void clGLFrameBuffer::BindReadFrom( int TargetIndex ) const
{
	LGL3->glBindFramebuffer( GL_FRAMEBUFFER, FFrameBuffer );

	int Width  = FColorBuffersParams[0][0];
	int Height = FColorBuffersParams[0][1];

	if ( Width  == -1 ) { Width  = Env->Viewport->GetWidth(); }

	if ( Height == -1 ) { Height = Env->Viewport->GetHeight(); }

	Env->Renderer->SetViewport( 0, 0, Width, Height );

	LGL3->glReadBuffer( GL_COLOR_ATTACHMENT0 + TargetIndex );
}

LVector4 clGLFrameBuffer::GetViewport() const
{
	int Width  = FColorBuffersParams[0][0];
	int Height = FColorBuffersParams[0][1];

	if ( Width  == -1 ) { Width  = Env->Viewport->GetWidth(); }

	if ( Height == -1 ) { Height = Env->Viewport->GetHeight(); }

	return LVector4( 0, 0, Width, Height );
}

void clGLFrameBuffer::UnBind() const
{
	LGL3->glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	Env->Renderer->RestoreViewport();

	// generate mipmaps
	for ( size_t i = 0; i != FColorBuffers.size(); i++ )
	{
		FColorBuffers[i]->UpdateMipmaps();
	}

	if ( FDepthBuffer )
	{
		FDepthBuffer->UpdateMipmaps();
	}
}

iTexture* clGLFrameBuffer::GetColorTexture( int MRTIndex ) const
{
	return FColorBuffers[MRTIndex];
}

iTexture* clGLFrameBuffer::GetDepthTexture() const
{
	return FDepthBuffer;
}
/*
void clGLFrameBuffer::ReadFramebuffer()
{
   // OpenGL 3.2 deprecated
   glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, FWidth, FHeight, 0 );
}
*/
void clGLFrameBuffer::UpdateWithProgram( iShaderProgram* Program )
{
	Bind( 0 );

	int Depth = FColorBuffersParams[0][2];

	Program->BindUniforms();

	for ( int z = 0; z != Depth; z++ )
	{
		FColorBuffers[0]->AttachToCurrentFB( GL_COLOR_ATTACHMENT0, z );

		Program->SetUniformNameFloat( "in_ZSlice", ( ( float )z + 0.5f ) / ( float )Depth );

		Env->Renderer->GetCanvas()->FullscreenRectSP( Program );
	}

	UnBind();
}

void clGLFrameBuffer::UpdateWithRenderState( clRenderState* Shader )
{
	Bind( 0 );

	int Depth = FColorBuffersParams[0][2];

	Shader->GetShaderProgram()->BindUniforms();

	for ( int z = 0; z != Depth; z++ )
	{
		FColorBuffers[0]->AttachToCurrentFB( GL_COLOR_ATTACHMENT0, z );

		Shader->GetShaderProgram()->SetUniformNameFloat( "in_ZSlice", ( ( float )z + 0.5f ) / ( float )Depth );

		Env->Renderer->GetCanvas()->FullscreenRect( Shader );
	}

	UnBind();
}


/*
 * 16/02/2010
     Support for 3D rendertargets
 * 25/05/2009
     MRT support
 * 20/11/2008
     Catalyst 8.11 workaround
 * 12/06/2007
     TargetIndex parameter
 * 26/07/2007
     FloatingPoint parameter deprecated
 * 06/07/2006
     GL_DEPTH_COMPONENT24 changed to GL_DEPTH_COMPONENT16
 * 26/12/2005
     Viewport setup depending on Width & Height of the framebuffer
 * 04/12/2005
     Depth buffer as texture
 * 03/12/2005
     First working version
 * 29/11/2005
     BindAsTexture()
     UnBindAsTexture()
 * 27/11/2005
     Initialization of a framebuffer with color and depth render buffers
 * 22/11/2005
     It's here
*/
