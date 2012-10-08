/**
 * \file GLDebug.h
 * \brief Advanced OpenGL debugging and error reporting based on GL_ARB_debug_output
 * \version 0.5.91
 * \date 29/07/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clGLDebug_
#define _clGLDebug_

#include "Core/iObject.h"
#include "Utils/Mutex.h"

#include "GLAPIContainer.h"

/// Advanced OpenGL debugging and error reporting based on GL_ARB_debug_output
class scriptfinal clGLDebugger: public iObject, public clGLAPIContainer
{
public:
	clGLDebugger();
	virtual ~clGLDebugger();
	//
	// iObject interface
	//
	virtual void    AfterConstruction();
	//
	// clGLDebugger
	//
	virtual void    SetEnable( bool Enable );
	virtual void    DebugMessageCallback( Lenum Source,
	                                      Lenum Type,
	                                      Luint ID,
	                                      Lenum Severity,
	                                      const LString& Message );
	virtual void    ProcessLog();
private:
	clMutex   FMutex;
	LString   FPrevMessage;
};

#endif

/*
 * 29/07/2010
     It's here
*/
