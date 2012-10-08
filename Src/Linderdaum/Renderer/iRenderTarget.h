/**
 * \file iRenderTarget.h
 * \brief Abstract render target representation
 * \version 0.5.89a
 * \date 27/03/2009
 * \author Sergey Kosarevsky, 2004-2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iRenderTarget_
#define _iRenderTarget_

#include "Platform.h"
#include "Core/iObject.h"
#include "Math/LVector.h"
#include "Utils/LArray.h"

class iTexture;
class iShaderProgram;
class clRenderState;

/// Render target, can be binded to texture unit
class scriptfinal netexportable iRenderTarget: public iObject
{
public:
	static const int FULLSCREEN = -1;
public:
	iRenderTarget() {};
	virtual ~iRenderTarget() {};

	//
	// iRenderTarget
	//

	/// Initialization
	virtual void InitRenderTarget( const int  Width,
	                               const int  Height,
	                               const int  Depth,
	                               const int  BitsPerChannel,
	                               const bool HasDepthBuffer,
	                               const int  MRTCount ) = 0;

	virtual void InitRenderTargetV( const LArray<LVector4i>& WidthHeightDepthBitsPerChannel,
	                                const bool HasDepthBuffer ) = 0;
	/**
	  Return color texture object for specified index of MRT render target
	**/
	virtual iTexture*    GetColorTexture( int MRTIndex ) const = 0;
	/**
	  Return depth texture object for this render target
	**/
	virtual iTexture*    GetDepthTexture() const = 0;
	/**
	  Bind as the current render target
	**/
	virtual void         Bind( int TargetIndex ) const = 0;
	/**
	  Bind color buffer TargetIndex as the current render target to read pixels
	**/
	virtual void         BindReadFrom( int TargetIndex ) const = 0;
	/**
	  Unbind render target
	**/
	virtual void         UnBind() const = 0;
	/**
	  Viewport size for this texture in pixels
	**/
	virtual LVector4     GetViewport() const = 0;

	/// Run shader program for every slice of the framebuffer color texture
	virtual void         UpdateWithProgram( iShaderProgram* Program ) = 0;

	/// Run shader for every slice of the framebuffer color texture
	virtual void         UpdateWithRenderState( clRenderState* Shader ) = 0;
};

#endif

/*
 * 27/03/2009
     ReadFramebuffer()
 * 23/03/2005
     GetPixel()
 * 13/12/2004
     It's here
*/
