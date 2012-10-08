/**
 * \file GLAPIContainer.cpp
 * \brief GL API container
 * \version 0.6.08
 * \date 30/01/2012
 * \author Sergey Kosarevsky, 2011-2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "GLAPIContainer.h"
#include "GLRenderContext.h"

void clGLAPIContainer::GetAPI( sEnvironment* Env )
{
	FLocalRenderer = dynamic_cast<clGLRenderContext*>( Env->Renderer );

	LGL3 = FLocalRenderer->GetLGLAPI();
}

/*
 * 30/01/2012
     Local renderer
 * 12/09/2011
     It's here
*/
