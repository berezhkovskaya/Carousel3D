/**
 * \file GLAPIContainer.h
 * \brief GL API container
 * \version 0.6.08
 * \date 30/01/2012
 * \author Sergey Kosarevsky, 2011-2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _GLAPIContainer_
#define _GLAPIContainer_

#include "Platform.h"
#include "Renderer/iGPUBuffer.h"

class sEnvironment;
class clGLRenderContext;
struct sLGLAPI;

class clGLAPIContainer
{
public:
	clGLAPIContainer(): LGL3( NULL ) {};
	explicit clGLAPIContainer( sLGLAPI* OtherLGL3 ): LGL3( OtherLGL3 ), FLocalRenderer( NULL ) {};
	//
	// clGLAPIContainer
	//
	virtual void    GetAPI( sEnvironment* Env );
	virtual void    SetLocalRenderer( clGLRenderContext* Renderer ) { FLocalRenderer = Renderer; };
public:
	sLGLAPI*           LGL3;
protected:
	clGLRenderContext* FLocalRenderer;
};


#endif

/*
 * 30/01/2012
     Local renderer
 * 12/09/2011
     It's here
*/
