/**
 * \file SysEnv.cpp
 * \brief Low-level environment implementation
 * \version 0.5.91 ($Revision: 468 $)
 * \date 02/07/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */


#include "SysEnv.h"

#include <iostream>

#include "Platform.h"
#include "Utils/Utils.h"

#include "Core/Script.h"
#include "Core/Logger.h"
#include "Core/Linker.h"
#include "Core/VFS/FileSystem.h"
#include "Utils/Library.h"

#include "Generated/Exports/Exports_Core.h"
#include "Generated/Globals/ColorRegistration.h"
#include "Generated/Globals/KeySymbolsRegistration.h"

#include "Environment.h"

#ifdef OS_WINDOWS
#include <conio.h>
#include <windows.h>
#undef min
#undef max
#endif

#ifdef L_USE_LIBUUID
extern "C"
{
	const char* UUID_Start();
	const char* UUID_Stop();
}
#endif

#ifdef OS_LINUX
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include <termios.h>

#include <assert.h>

/* Helpful conversion constants. */
const unsigned usec_per_sec = 1000000;
const unsigned usec_per_msec = 1000;

/* These functions are written to match the win32
   signatures and behavior as closely as possible.
*/
bool QueryPerformanceFrequency( int64_t* frequency )
{
	/* Sanity check. */
	assert( frequency != NULL );

	/* gettimeofday reports to microsecond accuracy. */
	*frequency = usec_per_sec;

	return true;
}

bool QueryPerformanceCounter( int64_t* performance_count )
{
	struct timeval Time;

	/* Grab the current time. */
	gettimeofday( &Time, NULL );
	*performance_count = Time.tv_usec + /* Microseconds. */
	                     Time.tv_sec * usec_per_sec; /* Seconds. */

	return true;
}


// Keyboard routines - linux specific

int kbhit( void )
{
	struct timeval tv;
	fd_set read_fd;

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_ZERO( &read_fd );
	FD_SET( 0, &read_fd );

	if ( select( 1, &read_fd, NULL, NULL, &tv ) == -1 )
	{
		return 0;
	}

	if ( FD_ISSET( 0, &read_fd ) )
	{
		return 1;
	}

	return 0;
}

#if !defined(OS_ANDROID)
int getch( )
{
	struct termios oldt, newt;
	int ch;

	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );

	ch = getchar();

	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );

	return ch;
}
#endif // OS_ANDROID

#endif // OS_LINUX

#ifdef L_USE_LIBUUID
#  include "Core/UUID.h"
#endif // L_USE_LIBUUID

double sSysEnv::GetCycles() const
{
#ifdef OS_WINDOWS
	LARGE_INTEGER T1;
#endif // OS_WINDOWS
#ifdef OS_LINUX
	int64_t T1;
#endif // OS_LINUX

	QueryPerformanceCounter( &T1 );

#ifdef OS_LINUX
	return static_cast<double>( T1 );
#endif
#ifdef OS_WINDOWS
	return static_cast<double>( T1.QuadPart );
#endif
}

double sSysEnv::GetSeconds() const
{
	return ( GetCycles() * FRecipCyclesPerSecond );
}

sSysEnv::sSysEnv(): Linker( NULL ),
	Logger( NULL ),
	FileSystem( NULL )
{
#ifdef L_USE_LIBUUID
	UUID_Start();
#endif
	Env = reinterpret_cast<sEnvironment*> ( this );
}

sSysEnv::~sSysEnv()
{
#ifdef L_USE_LIBUUID
	UUID_Stop();
#endif
	Env = NULL;
}

void sSysEnv::InitializeLowLevel()
{
	/// Initialize timing
#ifdef OS_WINDOWS
	LARGE_INTEGER Freq;
#endif

#ifdef __linux__
	int64_t Freq;
#endif

	QueryPerformanceFrequency( &Freq );

#ifdef OS_WINDOWS
	double CyclesPerSecond = static_cast<double>( Freq.QuadPart );
#else
	double CyclesPerSecond = static_cast<double>( Freq );
#endif

	FRecipCyclesPerSecond = 1.0 / CyclesPerSecond;

	Logger = new clLogger;
	Logger->Env = Env;
	Logger->SetObjectID( "Logger" );

	Linker = new clLinker;
	Linker->Env = Env;
	Linker->SetObjectID( "Linker" );

	RegisterEnvironmentSymbols();

	clExecutionThread::ClearExecutionStats();

	FileSystem = Linker->Instantiate( "clFileSystem" );
	FileSystem->SetObjectID( "FileSystem" );

	// here was the Mount() call with CommonMedia
}

void sSysEnv::RegisterEnvironmentSymbols()
{
	// 1. Metaclasses
	RegisterPackageLinderdaumCoreTunnellers( Env );

	// 2. Symbols

	// A) enviroment

	FIXME( "seems this is unsafe" )
	Linker->RegisterPODSymbol( "Env", reinterpret_cast<void*>( &this->Linker ) );

	// B) color library
	RegisterColorSymbols( Linker );

	// C) key symbols
	RegisterKeySymbols( Linker );

	// patch low-level subsystems
	Logger->SetStaticClass( Linker->FindStaticClass( "clLogger" ) );
	Linker->SetStaticClass( Linker->FindStaticClass( "clLinker" ) );

	// finally, we can do this
	this->SetStaticClass( Linker->FindStaticClass( "sEnvironment" ) );
}

void sSysEnv::DeinitializeLowLevel()
{
	Utils::PostClean( FileSystem );

	Utils::PostClean( Linker );


	clExecutionThread::LogExecutionStats( Env );

	if ( Logger )
	{
		Logger->Log( L_LOG, "clDeployer::ShutdownEnvironment()->Env removed" );
	}

	Utils::PostClean( Logger );
}

clLibrary* sSysEnv::LoadDynamicLibrary( const LString& LibName ) const
{
	guard( "%s", LibName.c_str() );

	clLibrary* Lib = new clLibrary();
	Lib->Load( LibName );

	return Lib;

	unguard();
}

iObject* sSysEnv::FindObject( const LString& ObjID )
{
	for ( clObjectsGraph::iterator i = ObjectsGraph.begin() ; i != ObjectsGraph.end() ; ++i )
	{
		if ( ( *i )->GetObjectID() == ObjID ) { return ( *i ); }
	}

	return NULL;
}

/*
 * 02/07/2010
     Extracted from sEnvironment
 * 01/07/2010
     It's here
*/
