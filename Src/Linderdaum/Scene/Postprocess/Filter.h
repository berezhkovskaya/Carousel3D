/**
 * \file Filter.h
 * \brief Postprocessing convolution filter interface
 * \version 0.5.89
 * \date 12/01/2009
 * \author Sergey Kosarevsky, 2007-2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _FilterPostprocessor_
#define _FilterPostprocessor_

#include "iPostprocessor.h"

class clRenderState;
class iRenderTarget;

/// Postprocessing convolution filter interface
class clFilterPostprocessor: public iPostprocessor
{
public:
	clFilterPostprocessor();
	virtual ~clFilterPostprocessor();

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
	//
	// clFilterPostprocessor
	//
	virtual void             SetFilterName( const LString& FilterName );
	virtual LString          GetFilterName() const;

	/* Property(Name=FilterName, Type=string, Getter=GetFilterName, Setter=SetFilterName) */
private:
	clRenderState*     FFilterShader;
};

#endif

/*
 * 12/01/2009
     GetInputRenderTargetName()
     GetOutputRenderTargetName()
 * 06/06/2007
     IsEnabled()
 * 28/04/2007
     It's here
*/
