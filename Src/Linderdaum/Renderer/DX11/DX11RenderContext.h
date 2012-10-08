/**
 * \file DX11RenderContext.h
 * \brief DirectX 11 rendering context
 * \version 0.5.89b
 * \date 01/12/2009
 * \author Sergey Kosarevsky, 2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _DX10RenderContext_
#define _DX10RenderContext_

#include "Platform.h"

#ifdef OS_WINDOWS
#include <windows.h>
#undef min
#undef max
#endif

#include "Renderer/iRenderContext.h"

#undef USE_DX_11

#ifdef USE_DX_11
#include <d3d11.h>
#include <d3dx11.h>
#endif

class clCVar;
class clViewport;

/// DirectX 11 rendering context
class scriptfinal clDX11RenderContext: public iRenderContext
{
public:
	clDX11RenderContext();
	virtual ~clDX11RenderContext();
	//
	// iRenderContext interface
	//
	virtual bool                    InitContext( clViewport* Viewport, int BitsPerPixel, int zBufferBits, int StencilBits, int Multisample, bool TakeoverContext );
	virtual void                    CheckError( const LString& File, const LString& Line, const LString& Info ) const;
	virtual Luint                   GetPixel( const LVector2& Pnt ) const;
	virtual void                    SetViewport( int X, int Y, int Width, int Height );
	virtual void                    SetViewportV( const LVector4& ViewportSize );
	virtual void                    RestoreViewport();
	virtual void                    SetClearColor4v( const LVector4& Color ) const;
	virtual int                     GetScreenshotSize() const;
	virtual void                    GetScreenshot( void* Ptr ) const;
	virtual sVideoSystemInfo        GetVideoSystemInfo() const;
	virtual Lint64                  GetTotalVideoMemory() const;
	virtual Lint64                  GetFreeVideoMemory() const;
	virtual void    ClearRenderTarget( bool Color, bool Depth, bool Stencil );
	virtual void    SetPolygonFillMode( bool Fill );
	virtual void    Finish() {};
	virtual void    SetState( clRenderState* State ) {};
	virtual void    UpdateState( clRenderState* State ) {};
private:
#ifdef USE_DX_11
	ID3D11Device*              FD3DDevice;
	IDXGISwapChain*            FSwapChain;
	ID3D11RenderTargetView*    FRenderTargetView;
#endif
};

#endif

/*
 * 01/12/2009
     Updated
 * 11/08/2009
     It's here
*/
