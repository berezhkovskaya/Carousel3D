/**
 * \file ASSAO.h
 * \brief Accumulative Screen Space Ambient Occlusion
 * \version 0.6.90
 * \date 18/02/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _ASSAOPostprocessor_
#define _ASSAOPostprocessor_

#include "iPostprocessor.h"

class clRenderState;
class iRenderTarget;

/// High dynamic range postprocessor
class scriptfinal clASSAOPostprocessor: public iPostprocessor
{
public:
	clASSAOPostprocessor();
	virtual ~clASSAOPostprocessor();

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
};

#endif

/*
 * 18/02/2011
     It's here
*/
