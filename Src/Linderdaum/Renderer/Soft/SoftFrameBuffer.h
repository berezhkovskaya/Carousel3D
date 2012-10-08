/**
 * \file SoftFrameBuffer.h
 * \brief MRTs via software rendering pipeline
 * \version 0.5.96
 * \date 10/06/2010
 * \author Viktor Latypov, 2010
 * \author viktor@linderdaum.com http://www.linderdaum.com
 */

#ifndef _SoftFrameBuffer_
#define _SoftFrameBuffer_

#include "Renderer/iRenderTarget.h"

class iTexture;
class iShaderProgram;

class scriptfinal clSoftFrameBuffer: public iRenderTarget
{
public:
	clSoftFrameBuffer(): FColorBuffers(),
		FDepthBuffer( NULL ) {};
	virtual ~clSoftFrameBuffer();
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
	virtual iTexture*    GetColorTexture( int MRTIndex ) const
	{
		return FColorBuffers[MRTIndex];
	}
	virtual iTexture*    GetDepthTexture() const
	{
		return FDepthBuffer;
	}

	virtual void         Bind( int TargetIndex ) const;
	virtual void         BindReadFrom( int TargetIndex ) const;
	virtual void         UnBind() const;

	virtual LVector4     GetViewport() const
	{
		return LVector4( 0, 0, FColorBuffersParams[0][0], FColorBuffersParams[0][1] );
	}

	// No shader programs yet for software rendering
	virtual void         UpdateWithProgram( iShaderProgram* Program ) {}
	virtual void         UpdateWithRenderState( clRenderState* Shader ) {}
private:
	LArray<iTexture*>   FColorBuffers;
	iTexture*           FDepthBuffer;

	/// (Width, Height, Depth, BitsPerChannel) for every attached color buffer
	LArray<LVector4i>   FColorBuffersParams;
};

#endif

/*
 * 10/06/2010
     Initial release
*/
