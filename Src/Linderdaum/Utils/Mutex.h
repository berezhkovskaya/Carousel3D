/**
 * \file Mutex.h
 * \brief Mutex
 * \version 0.5.91
 * \date 24/06/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __Mutex__h__included__
#define __Mutex__h__included__

#if !defined(_WIN32) && !defined(_WIN64)
//  __linux__ or just some posix
#include <pthread.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef min
#undef max
#endif

#include <iostream>
#include "Utils/Thread.h"

class clMutex
{
public:
	clMutex()
	{
#ifdef OS_POSIX
		pthread_mutex_init( &TheMutex, NULL );
#endif
#ifdef OS_WINDOWS
		// LV: EnterCriticalSection is not enough on multicore systems
		InitializeCriticalSection( &TheCS );
#endif
	}

	/// Enter the critical section -- other threads are locked out
	void Lock() const
	{
#ifdef OS_POSIX
//		if ( !pthread_mutex_trylock( &TheMutex ) )
//		{
//			LOGI( "BLOCK: OwningThread = %i, ThisThread = %i", TheMutex.value, iThread::GetCurrentThread() );
		pthread_mutex_lock( &TheMutex );
//		}
#endif
#ifdef OS_WINDOWS

		if ( !TryEnterCriticalSection( &TheCS ) )
		{
//			std::cout << "BLOCK: OwningThread = " << TheCS.OwningThread << ", ThisThread = " << GetCurrentThread() << std::endl;
			EnterCriticalSection( &TheCS );
		}

#endif
	}

	/// Leave the critical section
	void Unlock() const
	{
#ifdef OS_POSIX
		pthread_mutex_unlock( &TheMutex );
#endif
#ifdef OS_WINDOWS
		LeaveCriticalSection( &TheCS );
#endif
	}

	~clMutex()
	{
#ifdef OS_WINDOWS
		DeleteCriticalSection( &TheCS );
#endif
#ifdef OS_POSIX
		pthread_mutex_destroy( &TheMutex );
#endif
	}

#ifdef OS_POSIX
	// POSIX threads
	mutable pthread_mutex_t TheMutex;
#endif
#ifdef OS_WINDOWS
	mutable CRITICAL_SECTION TheCS;
#endif
};

class LMutex
{
public:
	explicit LMutex( const clMutex* Mutex ) : FMutex( Mutex ) { FMutex->Lock(); };
	~LMutex() { FMutex->Unlock(); };
private:
	const clMutex* FMutex;
};

#endif

/*
 * 24/06/2010
     Log section added
*/
