/**
 * \file DX11RenderContext.cpp
 * \brief DirectX 11 rendering context
 * \version 0.5.89a
 * \date 11/08/2009
 * \author Sergey Kosarevsky, 2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "DX11RenderContext.h"

#include "Environment.h"

#include "Utils/Utils.h"
#include "Core/Linker.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "Core/Logger.h"
#include "Utils/Screen.h"
#include "Utils/Viewport.h"
#include "Core/VFS/FileSystem.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iShaderProgram.h"
#include "Math/LProjection.h"
#include "LColors.h"

#include "Images/FI_Utils.h"

#include <string.h>

//
// A lot of Microsoft specific code goes here
//

#ifdef _MSC_VER
//   #pragma comment( lib, "d3d11.lib")
//   #pragma comment( lib, "d3dx11d.lib" )
#endif

clDX11RenderContext::clDX11RenderContext()
#ifdef USE_DX_11
	: FD3DDevice( NULL ),
	  FSwapChain( NULL ),
	  FRenderTargetView( NULL )
#endif
{
	FClassName_RenderTarget  = "clDX11FrameBuffer";
	FClassName_ShaderProgram = "clDX11SLShaderProgram";
	FClassName_Texture       = "clDX11Texture";
	FClassName_VertexArray   = "clDX11VertexArray";
	FClassName_Query         = "clDX11Query";
	FClassName_Buffer        = "clDX11Buffer";
}

bool clDX11RenderContext::InitContext( clViewport* Viewport,
                                       int BitsPerPixel,
                                       int zBufferBits,
                                       int StencilBits,
                                       int Multisample,
                                       bool TakeoverContext )
{
	guard();

#ifdef USE_DX_11

	iRenderContext::InitContext( Viewport, BitsPerPixel, zBufferBits, StencilBits, Multisample, TakeoverContext );

	HWND g_hWnd = ( HWND )Viewport->GetWindow();

	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect( g_hWnd, &rc );
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;

	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

	DXGI_SWAP_CHAIN_DESC sd;

	ZeroMemory( &sd, sizeof( sd ) );

	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	hr = D3D11CreateDeviceAndSwapChain( NULL, D3D11_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
	                                    D3D11_SDK_VERSION, &sd, &FSwapChain, &FD3DDevice );

	if ( FAILED( hr ) )
	{
		return false;
	}

	// Create a render target view
	ID3D11Texture2D* pBackBuffer;
	hr = FSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );

	if ( FAILED( hr ) )
	{
		return false;
	}

	hr = FD3DDevice->CreateRenderTargetView( pBackBuffer, NULL, &FRenderTargetView );
	pBackBuffer->Release();

	if ( FAILED( hr ) )
	{
		return false;
	}

	FD3DDevice->OMSetRenderTargets( 1, &FRenderTargetView, NULL );

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	FD3DDevice->RSSetViewports( 1, &vp );

#endif

	return true;

	unguard();
}

clDX11RenderContext::~clDX11RenderContext()
{
}

void clDX11RenderContext::SetPolygonFillMode( bool Fill )
{
}

void clDX11RenderContext::SetViewport( int X, int Y, int Width, int Height )
{
}

void clDX11RenderContext::SetViewportV( const LVector4& ViewportSize )
{
//   FCurrentViewportWidth = static_cast<int>( ViewportSize[2] );
//   FCurrentViewportHeight = static_cast<int>( ViewportSize[3] );

//   glViewport( static_cast<int>( ViewportSize[0] ),
//               static_cast<int>( ViewportSize[1] ),
//               static_cast<int>( ViewportSize[2] ),
//               static_cast<int>( ViewportSize[3] ) );
}

void clDX11RenderContext::RestoreViewport()
{
//   SetViewport( FViewport->GetLeft(), FViewport->GetTop(),
//                FViewport->GetWidth(), FViewport->GetHeight() );
}

void clDX11RenderContext::CheckError( const LString& File, const LString& Line, const LString& Info ) const
{
}

int clDX11RenderContext::GetScreenshotSize() const
{
//   return FViewport->GetWidth() * FViewport->GetHeight() * 3;
	return 0;
}

void clDX11RenderContext::GetScreenshot( void* Ptr ) const
{
//   int Width = FViewport->GetWidth();
//   int Height = FViewport->GetHeight();

//   glReadPixels( 0, 0, Width, Height, GL_BGR, GL_UNSIGNED_BYTE, Ptr );
//   glFinish();
}

Luint clDX11RenderContext::GetPixel( const LVector2& Pnt ) const
{
	return 0;
}

void clDX11RenderContext::ClearRenderTarget( bool Color, bool Depth, bool Stencil )
{
}

void clDX11RenderContext::SetClearColor4v( const LVector4& Color ) const
{
}

sVideoSystemInfo clDX11RenderContext::GetVideoSystemInfo() const
{
	sVideoSystemInfo Info;

//   Info.FVersion    = reinterpret_cast<const char*>( glGetString(GL_VERSION   ) );
//   Info.FVendor     = reinterpret_cast<const char*>( glGetString(GL_VENDOR    ) );
//   Info.FRenderer   = reinterpret_cast<const char*>( glGetString(GL_RENDERER  ) );
//   Info.FExtensions = reinterpret_cast<const char*>( glGetString(GL_EXTENSIONS) );
//   Info.FShadingLangVersion = "not supported";

	return Info;
}

Lint64 clDX11RenderContext::GetTotalVideoMemory() const
{
	return 0;
}

Lint64 clDX11RenderContext::GetFreeVideoMemory() const
{
	return 0;
}

/*
 * 11/08/2009
     It's here
*/
