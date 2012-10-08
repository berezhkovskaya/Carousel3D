/**
 * \file SoftRenderContext.cpp
 * \brief Software rendering context
 * \version 0.5.95
 * \date 10/06/2010
 * \author Viktor Latypov, 2009
 * \author viktor@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "SoftRenderContext.h"

#include "Utils/Viewport.h"
#include "Images/Bitmap.h"
#include "Geometry/VertexAttribs.h"
#include "Renderer/iVertexArray.h"
#include "Resources/ResourcesManager.h"

#include "LColors.h"

#include <string.h>


clSoftRenderContext::clSoftRenderContext()//:FViewport(NULL),
//FInsideFrame(false)
//                            ,
//                                       FCurrentViewportWidth(0),
//                                       FCurrentViewportHeight(0)
{
	FClassName_RenderTarget  = "clSoftFrameBuffer";
	FClassName_ShaderProgram = "clSoftShaderProgram";
	FClassName_Texture       = "clSoftTexture";
	FClassName_VertexArray   = "clSoftVertexArray";
	FClassName_Query         = "clSoftQuery";
	FClassName_Buffer        = "clSoftBuffer";
}

bool clSoftRenderContext::InitContext( clViewport* Viewport,
                                       int BitsPerPixel,
                                       int zBufferBits,
                                       int StencilBits,
                                       int Multisample,
                                       bool TakeoverContext )
{
	guard();

	iRenderContext::InitContext( Viewport, BitsPerPixel, zBufferBits, StencilBits, Multisample, TakeoverContext );

	int W = Viewport->GetWidth();
	int H = Viewport->GetHeight();

	FFrameBuffer = clBitmap::CreateBitmap( Env, W, H, 1, L_BITMAP_BGR8, L_TEXTURE_2D );

	FX = 0.0f;
	FY = 0.0f;
	FWidth = static_cast<float>( W );
	FHeight = static_cast<float>( H );

	FTakeoverContext = TakeoverContext;

	FViewport = Viewport;

	FCurrentState = Env->Resources->CreateShader();

	return true;

	unguard();
}

clSoftRenderContext::~clSoftRenderContext()
{
}

void clSoftRenderContext::SetPolygonFillMode( bool Fill )
{
}

void clSoftRenderContext::SetViewport( int X, int Y, int Width, int Height )
{
//   FCurrentViewportWidth = Width;
//   FCurrentViewportHeight = Height;
}

void clSoftRenderContext::SetViewportV( const LVector4& ViewportSize )
{
//   FCurrentViewportWidth = static_cast<int>( ViewportSize[2] );
//   FCurrentViewportHeight = static_cast<int>( ViewportSize[3] );
}

void clSoftRenderContext::RestoreViewport()
{
	/*
	SetViewport( FViewport->GetLeft(), FViewport->GetTop(),
	             FViewport->GetWidth(), FViewport->GetHeight() );
	         */
}

int clSoftRenderContext::GetScreenshotSize() const
{
	return static_cast<int>( FWidth ) * static_cast<int>( FHeight ) * 3;
}

void clSoftRenderContext::GetScreenshot( void* Ptr ) const
{
//   int Width = FViewport->GetWidth();
//   int Height = FViewport->GetHeight();

//   glReadPixels( 0, 0, Width, Height, GL_BGR, GL_UNSIGNED_BYTE, Ptr );
}

Luint clSoftRenderContext::GetPixel( const LVector2& Pnt ) const
{
	TODO( "read from image" )

//   Lint X = static_cast<Lint>( Pnt.X        * FCurrentViewportWidth );
//   Lint Y = static_cast<Lint>( (1.0f-Pnt.Y) * FCurrentViewportHeight );

	Luint C = 0;

//   glReadPixels(X, Y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &C);

	return C;
}

void clSoftRenderContext::ClearRenderTarget( bool Color, bool Depth, bool Stencil )
{
	if ( Color )
	{
		FFrameBuffer->Clear( FClearColor );
	}
}

LVector3 clSoftRenderContext::ToViewport( const LVector3& Pt )
{
	return LVector3( FX + ( Pt.X + 1.0f ) * FWidth / 2.0f,
	                 FY + ( Pt.Y + 1.0f ) * FHeight / 2.0f, Pt.Z );
}

void clSoftRenderContext::AddBuffer( iVertexArray* VertexArray,
                                     clRenderState* Shader,
                                     const sMatrices* Matrices,
                                     int Instances,
                                     bool Wireframe )
{

	if ( !Shader )
	{
		return;
	}

	if ( !VertexArray )
	{
		return;
	}

	LMatrix4 FProjection = Matrices->in_ProjectionMatrix;
	LMatrix4 FTransform  = Matrices->in_ModelViewMatrix;

	// we do not use any shaders yet - just render the wireframe
	clVertexAttribs* VB = VertexArray->GetVertexAttribs();

	LVector3 V[3], VV[3];
	LVector4 V4[3], PV[3];

	int x[3], y[3];

	for ( int i = 0; i < VB->GetNumTriangles(); i++ )
	{
		// 1. Get the triangle
		VB->GetTriangle( i, V[0], V[1], V[2] );

		for ( int j = 0 ; j < 3 ; j++ )
		{
			V4[j].X = V[j].X;
			V4[j].Y = V[j].Y;
			V4[j].Z = V[j].Z;
			V4[j].W = 1.0f;
		}

		// 2. Transform and project
		for ( int j = 0 ; j < 3 ; j++ )
		{
			PV[j] = FProjection * ( FTransform * V4[j] );
		}

		// 3. Clip in the post-perspective space to [-1..1]x[-1..1]x[-1..1] cube (clip against six planes)

		// 4. Perspective division
		for ( int j = 0 ; j < 3 ; j++ )
		{
			PV[j].X /= PV[j].W;
			PV[j].Y /= PV[j].W;
			PV[j].Z /= PV[j].W;
			PV[j].W = 1.0f;
		}

		// 5. Convert NDC to viewport coordinates
		for ( int j = 0 ; j < 3 ; j++ )
		{
			VV[j] = ToViewport( PV[j].ToVector3() );
		}

		// 6. render the resulting 2D triangle
		for ( int j = 0 ; j < 3 ; j++ )
		{
			x[j] = static_cast<int>( VV[j].X );
			y[j] = static_cast<int>( VV[j].Y );
		}

		FFrameBuffer->DrawLine2D( x[0], y[0], x[1], y[1], LC_White );
		FFrameBuffer->DrawLine2D( x[0], y[0], x[2], y[2], LC_White );
		FFrameBuffer->DrawLine2D( x[1], y[1], x[2], y[2], LC_White );
	}
}

void clSoftRenderContext::SetClearColor4v( const LVector4& Color ) const
{
	FClearColor = Color;
}

sVideoSystemInfo clSoftRenderContext::GetVideoSystemInfo() const
{
	sVideoSystemInfo Info;

	Info.FVersion    = ENGINE_VERSION;
	Info.FVendor     = ENGINE_NAME;
	Info.FRenderer   = "software";
	Info.FExtensions = "not supported";
	Info.FShadingLangVersion = "not supported";

	Info.FMaxTextureUnits = 16;
	Info.FMaxTextureSize = 4096;

	return Info;
}

void clSoftRenderContext::EndFrame( bool SwapBuffer )
{
	iRenderContext::EndFrame( false );

	Env->Viewport->BlitBitmap( static_cast<int>( FX ), static_cast<int>( FY ), FFrameBuffer );
}

/*
 * 10/06/2010
     First version
*/
