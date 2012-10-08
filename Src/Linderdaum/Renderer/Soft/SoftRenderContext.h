/**
 * \file SoftRenderContext.h
 * \brief Software rendering context
 * \version 0.5.95
 * \date 10/06/2010
 * \author Viktor Latypov, 2010
 * \author viktor@linderdaum.com http://www.linderdaum.com
 */

#ifndef _SoftRenderContext_
#define _SoftRenderContext_

#include "Platform.h"
#include "Core/iObject.h"
#include "Renderer/RenderState.h"
#include "Renderer/iRenderTarget.h"
#include "Renderer/iRenderContext.h"
#include "Math/LMatrix.h"

class iVertexArray;
class clViewport;
class clBitmap;

/// Abstract render context representation
class scriptfinal clSoftRenderContext: public iRenderContext
{
public:
	clSoftRenderContext();
	virtual ~clSoftRenderContext();

	virtual bool     InitContext( clViewport* Viewport, int BitsPerPixel, int zBufferBits, int StencilBits, int Multisample, bool TakeoverContext );
	virtual void     EndFrame( bool SwapBuffer );
	virtual Luint    GetPixel( const LVector2& Pnt ) const;
	virtual void     SetViewport( int X, int Y, int Width, int Height );
	virtual void     SetViewportV( const LVector4& ViewportSize );
	virtual void     RestoreViewport();
	virtual void     SetClearColor4v( const LVector4& Color ) const;
	virtual int      GetScreenshotSize() const;
	virtual void     GetScreenshot( void* Ptr ) const;

	virtual sVideoSystemInfo   GetVideoSystemInfo() const;

	/// Return total GPU memory
	virtual Lint64    GetTotalVideoMemory() const
	{
		return 0;
	};

	/// Return currently free GPU memory
	virtual Lint64    GetFreeVideoMemory() const
	{
		return 0;
	};

	/// Terminates application if error occured
	virtual void    CheckError( const LString& File, const LString& Line, const LString& Info ) const {}

	virtual void    AddBuffer( iVertexArray* VertexArray, clRenderState* Shader, const sMatrices* Matrices, int Instances, bool Wireframe );
	virtual void    ClearRenderTarget( bool Color, bool Depth, bool Stencil );
	virtual void    SetPolygonFillMode( bool Fill );
	virtual void    Finish() {};

	virtual void    SetState( clRenderState* State ) {};
	virtual void    UpdateState( clRenderState* State ) {};
private:
	LVector3    ToViewport( const LVector3& Pt );

	clViewport*        FViewport;

	float    FX;
	float    FY;
	float    FWidth;
	float    FHeight;

	TODO( "replace by SoftFrameBuffer instance" )
	clBitmap*    FFrameBuffer;

	mutable LVector4    FClearColor;
};

#endif

/*
 * 10/06/2010
     Initial version
*/
