/**
 * \file GLDebug.cpp
 * \brief Advanced OpenGL debugging and error reporting based on GL_ARB_debug_output
 * \version 0.5.91
 * \date 29/07/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "GLDebug.h"
#include "Environment.h"
#include "Core/Logger.h"

#include "Generated/LGL/LGL.h"

#if defined( OS_WINDOWS )

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	void APIENTRY MessageCallback( Lenum source,
	                               Lenum type,
	                               Luint id,
	                               Lenum severity,
	                               Lsizei length,
	                               const char* message,
	                               void* userParam )
	{
		clGLDebugger* Debugger = dynamic_cast<clGLDebugger*>( ( iObject* )userParam );

		if ( Debugger )
		{
			Debugger->DebugMessageCallback( source, type, id, severity, LString( message, length ) );
		}
	}
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // OS_WINDOWS

clGLDebugger::clGLDebugger(): FMutex(),
	FPrevMessage()
{
}

clGLDebugger::~clGLDebugger()
{
}

void clGLDebugger::AfterConstruction()
{
	iObject::AfterConstruction();

	GetAPI( Env );
}

void clGLDebugger::SetEnable( bool Enable )
{
#if defined( OS_WINDOWS )

	if ( LGL3->glDebugMessageCallbackARB )
	{
		LGL3->glDebugMessageCallbackARB( Enable ? &MessageCallback : NULL, this );
		LGL3->glDebugMessageControlARB( GL_DONT_CARE, // source
		                                GL_DONT_CARE, // type
		                                GL_DONT_CARE, // severity
		                                0, NULL, true );
	}

#endif
}

void clGLDebugger::ProcessLog()
{
#if defined( OS_WINDOWS )

	if ( !LGL3->glGetDebugMessageLogARB )
	{
		return;
	}

	const GLuint  NUM_MESSAGES   = 1;

	const GLsizei MAX_LOG_LENGTH = 1024;

	GLenum Sources[NUM_MESSAGES];

	GLenum Types[NUM_MESSAGES];

	GLenum IDs[NUM_MESSAGES];

	GLenum Severities[NUM_MESSAGES];

	GLsizei Lengths[NUM_MESSAGES];

	char Log[MAX_LOG_LENGTH];

	while ( LGL3->glGetDebugMessageLogARB( NUM_MESSAGES, MAX_LOG_LENGTH, &Sources[0], &Types[0], &IDs[0], &Severities[0], &Lengths[0], &Log[0] ) )
	{
		DebugMessageCallback( Sources[0], Types[0], IDs[0], Severities[0], LString( &Log[0] ) );
	}

#endif
}

void clGLDebugger::DebugMessageCallback( Lenum Source,
                                         Lenum Type,
                                         Luint ID,
                                         Lenum Severity,
                                         const LString& Message )
{
#if defined( OS_WINDOWS )
	// DebugMessageCallback is asynchronous
	FMutex.Lock();

	LString Msg( "" );

	switch ( Type )
	{
		case GL_DEBUG_TYPE_ERROR_ARB:
			Msg = "DEBUG_TYPE_ERROR_ARB";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
			Msg = "DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
			Msg = "DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE_ARB:
			Msg = "DEBUG_TYPE_PERFORMANCE_ARB";
			break;
		case GL_DEBUG_TYPE_PORTABILITY_ARB:
			Msg = "DEBUG_TYPE_PORTABILITY_ARB";
			break;
		case GL_DEBUG_TYPE_OTHER_ARB:
			Msg = "DEBUG_TYPE_OTHER_ARB";
			break;
	}

	LString MsgSev( "" );

	switch ( Severity )
	{
		case GL_DEBUG_SEVERITY_HIGH_ARB:
			MsgSev = "DEBUG_SEVERITY_HIGH_ARB";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM_ARB:
			MsgSev = "DEBUG_SEVERITY_MEDIUM_ARB";
			break;
		case GL_DEBUG_SEVERITY_LOW_ARB:
			MsgSev = "DEBUG_SEVERITY_LOW_ARB";
			break;
	}

	LString FullMessage = Msg + "\n" + MsgSev + "\n" + Message;

	if ( FPrevMessage != FullMessage )
	{
		Env->Logger->Log( L_NOTICE, FullMessage );

		FPrevMessage = FullMessage;
	}

	FMutex.Unlock();
#endif
}

/*
 * 29/07/2010
     It's here
*/
