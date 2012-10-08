
/**
 * \file Thread.cpp
 * \brief POSIX thread
 * \version 0.5.95
 * \date 16/06/2010
 * \author Viktor Latypov, 2009-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Thread.h"

#ifdef OS_WINDOWS
#	include <windows.h>
#	include <process.h>
#endif

#ifdef OS_ANDROID
#	include <unistd.h>
#endif

#ifdef OS_POSIX
#	include <sched.h>
#endif

iThread::iThread()
	: Env( NULL ),
	  FThreadHandle( 0 ),
	  FPendingExit( false )
{
}

iThread::~iThread()
{
}

#ifdef OS_WINDOWS
unsigned int __stdcall ThreadStaticEntryPoint( void* Ptr )
#endif
#ifdef OS_POSIX
void* ThreadStaticEntryPoint( void* Ptr )
#endif
{
	iThread* Thread = reinterpret_cast<iThread*>( Ptr );

	if ( Thread )
	{
		Thread->Run();
	}

#ifdef OS_WINDOWS
	_endthreadex( 0 );
	return 0;
#endif

#ifdef OS_POSIX
	pthread_exit( 0 );
	return NULL;
#endif
}

void iThread::Start( sEnvironment* E, LPriority Priority )
{
	Env = E;

	void* ThreadParam = reinterpret_cast<void*>( this );

#ifdef OS_WINDOWS
	unsigned int ThreadID = 0;
	FThreadHandle = ( uintptr_t )_beginthreadex( NULL, 0, &ThreadStaticEntryPoint, ThreadParam, 0, &ThreadID );

	int P = THREAD_PRIORITY_IDLE;

	if ( Priority == Priority_Lowest      ) { P = THREAD_PRIORITY_LOWEST; }

	if ( Priority == Priority_Low         ) { P = THREAD_PRIORITY_BELOW_NORMAL; }

	if ( Priority == Priority_Normal      ) { P = THREAD_PRIORITY_NORMAL; }

	if ( Priority == Priority_High        ) { P = THREAD_PRIORITY_ABOVE_NORMAL; }

	if ( Priority == Priority_Highest     ) { P = THREAD_PRIORITY_HIGHEST; }

	if ( Priority == Priority_TimeCritical ) { P = THREAD_PRIORITY_TIME_CRITICAL; }

	SetThreadPriority( ( HANDLE )FThreadHandle, P );
#endif

#ifdef OS_POSIX
	pthread_create( &FThreadHandle, NULL, ThreadStaticEntryPoint, ThreadParam );
	pthread_detach( FThreadHandle );

	int SchedPolicy = SCHED_OTHER;

	int MaxP = sched_get_priority_max( SchedPolicy );
	int MinP = sched_get_priority_min( SchedPolicy );

	sched_param SchedParam;
	SchedParam.sched_priority = MinP + ( MaxP - MinP ) / ( Priority_TimeCritical - Priority + 1 );

	pthread_setschedparam( FThreadHandle, SchedPolicy, &SchedParam );
#endif
}

void iThread::Exit( bool Wait )
{
	FPendingExit = true;

	if ( !Wait ) { return; }

	if ( GetCurrentThread() != FThreadHandle )
	{
#ifdef OS_WINDOWS
		WaitForSingleObject( ( HANDLE )FThreadHandle, INFINITE );
		CloseHandle( ( HANDLE )FThreadHandle );
#else
		pthread_join( FThreadHandle, NULL );
#endif
	}
}

size_t iThread::GetCurrentThread()
{
#if defined( OS_WINDOWS )
	return GetCurrentThreadId();
#elif defined( OS_ANDROID )
	return gettid();
#else
	return pthread_self();
#endif
}

/*
 * 06/04/2009
     Initial implementation
*/
