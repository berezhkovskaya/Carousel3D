/**
 * \file GLFrameBuffer.h
 * \brief MRTs via GL framebuffer object
 * \version 0.5.91
 * \date 21/02/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _GLFrameBuffer_
#define _GLFrameBuffer_

#include "Renderer/iRenderTarget.h"

#include "GLAPIContainer.h"

class iTexture;
class iShaderProgram;

class scriptfinal clGLFrameBuffer: public iRenderTarget, public clGLAPIContainer
{
public:
	clGLFrameBuffer(): FColorBuffers(),
		FDepthBuffer( NULL ) {};
	virtual ~clGLFrameBuffer();
	//
	// iObject interface
	//
	virtual void    AfterConstruction();

	//
	// iRenderTarget interface
	//

	/**
	   Initialize render target, WidthHeightDepthBitsPerChannel contains
	   as many 4-vectors as MRTCount
	**/
	virtual void InitRenderTarget( const int  Width,
	                               const int  Height,
	                               const int  Depth,
	                               const int  BitsPerChannel,
	                               const bool HasDepthBuffer,
	                               const int  MRTCount );

	virtual void InitRenderTargetV( const LArray<LVector4i>& WidthHeightDepthBitsPerChannel,
	                                const bool HasDepthBuffer );
	//
	// iRenderTarget interface
	//
	virtual iTexture*    GetColorTexture( int MRTIndex ) const;
	virtual iTexture*    GetDepthTexture() const;
	virtual void         Bind( int TargetIndex ) const;
	virtual void         BindReadFrom( int TargetIndex ) const;
	virtual void         UnBind() const;
	virtual LVector4     GetViewport() const;
	virtual void         UpdateWithProgram( iShaderProgram* Program );
	virtual void         UpdateWithRenderState( clRenderState* Shader );

	FWD_EVENT_HANDLER( Event_SURFACE_DETACHED );
	FWD_EVENT_HANDLER( Event_SURFACE_ATTACHED );
protected:
	void    CheckFrameBuffer() const;
private:
	int                  FMemSize;

	Luint                FFrameBuffer;

	LArray<iTexture*>    FColorBuffers;
	iTexture*            FDepthBuffer;

	/// (Width, Height, Depth, BitsPerChannel) for every attached color buffer
	LArray<LVector4i>   FColorBuffersParams;
	bool                FHasDepthBuffer;
};

#endif

/*
 * 21/02/2010
     Buffers unified with textures
 * 25/05/2009
     Multiple color buffers
 * 22/11/2005
     It's here
*/
