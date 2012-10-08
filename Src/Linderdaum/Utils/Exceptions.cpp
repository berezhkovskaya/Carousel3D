/**
 * \file Exceptions.cpp
 * \brief Base facilities for exceptions handling
 * \version 0.6.00
 * \date 10/01/2011
 * \author Sergey Kosarevsky, 2005-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include <stdarg.h>

#include "Exceptions.h"
#include "Engine.h"
#include "Core/Logger.h"

static const int GUARD_BUFFER = 255;

static const char NameParams[]   = "%s(%s)->\0";
#if defined(PLATFORM_GCC)
static const char NameNoParams[] = "%s->\0";
#else
static const char NameNoParams[] = "%s()->\0";
#endif

clGuardian::clGuardian( clLogger* Logger, const char* FuncName, const char* Pattern, ... ): FLogger( Logger )
{
	va_list p;
	va_start( p, Pattern );

	char buf1[GUARD_BUFFER];
	char buf2[GUARD_BUFFER];

	Lvsnprintf( buf1, GUARD_BUFFER - 1, Pattern, p );
	Lsnprintf( buf2, GUARD_BUFFER - 1, NameParams, FuncName, buf1 );

	FLogger->PushProc( buf2 );

	va_end( p );
}

::Linderdaum::clGuardian::clGuardian( clLogger* Logger, const char* FuncName ): FLogger( Logger )
{
	char buf[GUARD_BUFFER];

	Lsnprintf( buf, GUARD_BUFFER - 1, NameNoParams, FuncName, buf );

	FLogger->PushProc( buf );
};


::Linderdaum::clGuardian::~clGuardian()
{
	FLogger->PopProc();
};


::Linderdaum::clException::clException( clLogger* Logger )
	: FLogger( Logger ),
	  FReason( "Unknown error" ),
	  FContext( "Unknown context" ),
	  FFile( "Unknown file" ),
	  FLine( 0 )
{
	FContext = FLogger->GetCurrentProcsNestingSeparated();
}

void ::Linderdaum::clException::SetFileAndLine( const char* File, int Line )
{
	FFile = File;
	FLine = Line;
}

void ::Linderdaum::clException::FatalException()
{
	FContext = FLogger->GetCurrentProcsNestingSeparated();

	FLogger->FatalException( this );
}

/*
 * 10/01/2011
     Fixed NameNoParams[] for GCC
 * 27/12/2010
     Significant speed boost in guard()/unguard() mechanism
 * 29/10/2005
     SetFileAndLine()
 * 25/10/2005
     It's here
*/
