/**
 * \file Thread.h
 * \brief POSIX thread
 * \version 0.5.95
 * \date 16/06/2010
 * \author Viktor Latypov, 2009-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __iThread__h__included__
#define __iThread__h__included__

#include "Platform.h"

#ifdef OS_POSIX
#include <pthread.h>
#endif

class sEnvironment;

/**
   Posix-based thread

   Linux implementation uses pthreads and win32/64 uses WinAPI to avoid external dependancies
*/
class iThread
{
public:
	enum LPriority
	{
		Priority_Idle         = 0,
		Priority_Lowest       = 1,
		Priority_Low          = 2,
		Priority_Normal       = 3,
		Priority_High         = 4,
		Priority_Highest      = 5,
		Priority_TimeCritical = 6
	};
public:
	iThread();
	virtual ~iThread();

	/// start a thread
	void Start( sEnvironment* E, LPriority Priority );
	void Exit( bool Wait );

	static size_t GetCurrentThread();

	/// Worker routine
	virtual void Run() = 0;

	bool IsPendingExit() const { return FPendingExit; };
private:
	volatile bool FPendingExit;

public:
	sEnvironment* Env;
private:
#ifdef OS_WINDOWS
	uintptr_t FThreadHandle;
#endif

#ifdef OS_POSIX
	pthread_t FThreadHandle;
#endif
};

#endif

/*
 * 16/06/2010
     Linux port
 * 06/04/2009
     Initial implementation
*/
