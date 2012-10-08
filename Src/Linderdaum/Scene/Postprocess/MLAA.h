#ifndef __clMLAAPostprocessor__
#define __clMLAAPostprocessor__

class clRenderState;
class iRenderTarget;

#include "iPostprocessor.h"

/// MLAA postprocessor
class netexportable scriptfinal clMLAAPostprocessor: public iPostprocessor
{
public:
	clMLAAPostprocessor() {}
	virtual ~clMLAAPostprocessor() { Destroy(); }

	//
	// iObject interface
	//
	virtual void    AfterConstruction();
	//
	// iPostprocessor interface
	//
	virtual bool             Render( iRenderTarget* RenderTarget, iRenderTarget* Input, iRenderTarget* Output );
	//
	// clMLAAPostprocessor
	//
	virtual float  GetDiscontinuity() const;
	virtual void   SetDiscontinuity( float D );

	virtual void   SetImageSize( int W, int H );

	/** Property(Name=Discontinuity, Type=float, Getter=GetDiscontinuity, Setter=SetDiscontinuity) */
	/** Property(Name=UsePrepass,    Type=bool,  FieldName=FUsePrepass) */

	bool FUsePrepass;

private:
	virtual void   Init( int W, int H );
	virtual void   Destroy();
private:
	clRenderState* FPrepassShader;
	clRenderState* FCalcAlphaShader;
	clRenderState* FBlendShader;
	clRenderState* FLineColShader;
	clRenderState* FDistanceCalcShader;

	iRenderTarget* FTmpAlphaRT;
	iRenderTarget* FTmpLinesRT[2];
	iRenderTarget* FTmpLineColRT;
	iRenderTarget* FPrepassRT;

	Lint32 FThreshold_Uniform;

	float  FDiscontinuity;

	// used for output
	iRenderTarget*     FRenderTarget;
};

#endif

/*
 * 24/02/2011
     Implementing...
*/
