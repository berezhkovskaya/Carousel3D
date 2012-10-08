/**
 * \file iPostprocessor.h
 * \brief Generic interface for 2D image postprocessor
 * \version 0.5.89
 * \date 12/01/2009
 * \author Sergey Kosarevsky, 2007-2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iPostprocessor_
#define _iPostprocessor_

#include "Core/iObject.h"

class clCVar;
class iRenderTarget;

/// Generic interface for 2D image postprocessor
class iPostprocessor: public iObject
{
public:
	iPostprocessor();

	NET_EXPORTABLE();
	SERIALIZABLE_CLASS();

	//
	// iPostprocessor
	//
	virtual bool             Render( iRenderTarget* RenderTarget, iRenderTarget* Input, iRenderTarget* Output ) = 0;
	virtual bool             IsEnabled() const;
	virtual void             SetEnableVarName( const LString& EnableVarName );
	virtual LString          GetEnableVarName() const;
	virtual clCVar*          GetEnableVar() const { return FEnableVar; }
	/* Property(Name=EnableVarName, Type=string, Getter=GetEnableVarName, Setter=SetEnableVarName) */
private:
	clCVar*            FEnableVar;
};

#endif

/*
 * 12/01/2009
     GetInputRenderTargetName()
     GetOutputRenderTargetName()
 * 20/04/2007
     It's here
*/
