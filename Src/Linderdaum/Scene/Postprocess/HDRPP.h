/**
 * \file HDRPP.h
 * \brief High dynamic range postprocessor
 * \version 0.5.74
 * \date 20/04/2007
 * \author Sergey Kosarevsky, 2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _HDRPostprocessor_
#define _HDRPostprocessor_

#include "iPostprocessor.h"

class clRenderState;
class iRenderTarget;

static const int MAX_LUMINANCE_MIPMAPS = 4;

/// High dynamic range postprocessor
class scriptfinal clHDRPostprocessor: public iPostprocessor
{
public:
	clHDRPostprocessor();
	virtual ~clHDRPostprocessor();

	NET_EXPORTABLE();
	SERIALIZABLE_CLASS();

	//
	// iObject interface
	//
	virtual void AfterConstruction();
	//
	// iPostprocessor interface
	//
	virtual bool             Render( iRenderTarget* RenderTarget, iRenderTarget* Input, iRenderTarget* Output );
private:
	clRenderState*    FFinalPassShader;
	clRenderState*    FToLuminanceShader;
	clRenderState*    FDownScaleShader;
	clRenderState*    FBypassShader;
	clRenderState*    FBypassDepthShader;
	clRenderState*    FBloomXShader;
	clRenderState*    FBloomYShader;
	clRenderState*    FStreakShader;
	clRenderState*    FBrightPassShader;
	clRenderState*    FLightAdaptationShader;
	clRenderState*    FFillHDRWhiteShader;
	iRenderTarget*    FBrightPass;
	iRenderTarget*    FBloomX;
	iRenderTarget*    FBloomY;
	iRenderTarget*    FAvgLuminance[ MAX_LUMINANCE_MIPMAPS ];
	iRenderTarget*    FCurrentLuminance;
	iRenderTarget*    FAdaptedLuminance;
};

#endif

/*
 * 28/04/2007
     It's here
*/
