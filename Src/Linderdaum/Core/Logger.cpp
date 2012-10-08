/**
 * \file Logger.cpp
 * \brief Global debug logging
 * \version 0.6.00
 * \date 18/02/2011
 * \author Sergey Kosarevsky, 2004-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Platform.h"

#ifdef OS_WINDOWS
#include <windows.h>
#include <conio.h>
#endif // OS_WINDOWS

#ifdef OS_POSIX
#include <sys/time.h>
#include <stdarg.h>
#endif

#ifdef OS_ANDROID
#include <android/log.h>
#endif // OS_ANDROID

#include <time.h>
#include <stdio.h>
#include <numeric>

#include "Engine.h"
#include "Environment.h"
#include "Core/VFS/FileSystem.h"
#include "Core/Console.h"
#include "Utils/Screen.h"
#include "Utils/Mutex.h"
#include "Utils/Thread.h"
#include "Logger.h"

LString LogLevelToString(LLogLevel L)
{
	switch(L)
	{
		case L_PARANOID: return "PARANOID";
		case L_DEBUG: return "DEBUG";
		case L_NOTICE: return "NOTICE";
		case L_LOG: return "LOG";
		case L_WARNING: return "WARNING";
	}

	return "UNDEFINED";
}

LLogLevel StringToLogLevel(const LString& S)
{
	LString U(LStr::GetUpper(S));

	if(U == "PARANOID") { return L_PARANOID; }
	if(U == "DEBUG") { return L_DEBUG; }
	if(U == "NOTICE") { return L_NOTICE; }
	if(U == "LOG") { return L_LOG; }
	if(U == "WARNING") { return L_WARNING; }

	return L_LOG;
}

int GetCurrentMilliseconds()
{
#if defined( OS_WINDOWS )
	SYSTEMTIME st;

	GetSystemTime( &st );

	return st.wMilliseconds;
#else
	struct timeval TimeVal;

	gettimeofday( &TimeVal, NULL );

	return TimeVal.tv_usec / 1000;
#endif
}

void clLogger::FatalException( const ::Linderdaum::clException* E )
{
	LString ExceptionName = LString( typeid( *E ).name() );

	GCC_CONVERT_TYPEINFO( ExceptionName );

	Fatal( E->FReason + "\n\n" + LString( "Exception: " ) + ExceptionName,
	       LString( E->FFile ),
	       LStr::ToStr( E->FLine ),
	       E->FContext );
}

void clLogger::EnableFileLogging( const LString& LogFileName )
{
	// if no log file was specified - leave the things as they are
	if ( LogFileName.empty() ) { return; }

	LString Dir;

	clFileSystem::SplitPath( LogFileName, NULL, &Dir, NULL, NULL );

	clFileSystem::CreateDirsPhys( Dir );

	// close if there was an old log file
	if ( FLogFile )
	{
		fflush( FLogFile );
		fclose( FLogFile );
	}

	FLogFile = fopen( LogFileName.c_str() , "w" );

	// display a nice error message
	if ( FLogFile == 0 )
	{
		// just comment this out and logging will be disabled
		Fatal( LString( "Unable to open log file: \"" ) + LogFileName + "\"", __FILE__, LStr::ToStr( __LINE__ ), "clLogger::InitLogger()" );
	}

	SetCurrentThreadName( "Main" );

	LogP( L_LOG, "clLogger::InitLogger()->Initializing..." );
	LogP( L_LOG, "clLogger::InitLogger()->Log file: %s", LogFileName.c_str() );
}

void clLogger::ShutdownLogger()
{
	Log( L_LOG, "clLogger::ShutdownLogger()->Shutting down..." );

	if ( Env->ObjectsGraph.size() > 0 )
	{
		Log( L_WARNING, "--- WARNING: Memory leaks detected --- Objects: " + LStr::ToStr( ( int )Env->ObjectsGraph.size() ) );

		for ( sEnvironment::clObjectsGraph::const_iterator i = Env->ObjectsGraph.begin(); i != Env->ObjectsGraph.end(); ++i )
		{
			Log( L_WARNING, ( *i )->NativeClassName() + "(" + ( *i )->GetObjectID() + ") in context: " + ( *i )->GetAllocationContext() );
		}

		Log( L_WARNING, "------------------------------------------------" );
	}

	Log( L_NOTICE, "clLogger::ShutdownLogger()->Closing log file..." );

	if ( FLogFile )
	{
		fflush( FLogFile );
		fclose( FLogFile );
	}
}

void clLogger::MarkTime( LLogLevel LogLevel, const char* Pattern ) const
{
	double ThisTime =Env->GetSeconds();

	float DeltaTime = static_cast<float>( ThisTime - FLastTimeStamp );

	FLastTimeStamp = ThisTime;

	LogP( LogLevel, Pattern, DeltaTime );
}

void clLogger::Log( LLogLevel LogLevel, const LString& Str ) const
{
	LogP( LogLevel, "%s", Str.c_str() );
}

void clLogger::LogP( LLogLevel LogLevel, const char* Pattern, ... ) const
{
	if ( LogLevel < FCurrentLogLevel ) { return; }

	va_list p;
	va_start( p, Pattern );

	const int LogMessageBufferSize = 8192;

	char LogMessageBuffer[ LogMessageBufferSize ];

	Lvsnprintf( LogMessageBuffer, LogMessageBufferSize - 1, Pattern, p );

	va_end( p );

	sThreadLogContext* Ctx = GetThreadLogContext();

	if ( Ctx->FLogsOnThisLevel.size() > 0 )
	{
		Ctx->FLogsOnThisLevel.back() = true;
	}

	LString Str       = LString( LogMessageBuffer );
	LString LogString = GetTimeStamp() + GetCurrentProcsNesting() + Str;

	SendAsync( L_EVENT_LOG, LEventArgs( &LogString, static_cast<float>( LogLevel ), ( iObject* )&Str, false ), false );

	LMutex Lock( &FMutex );

	if ( Env->Console )
	{
		if ( LogLevel == L_LOG )
		{
			Env->Console->Display( Str );
		}
		else if ( LogLevel >= L_WARNING )
		{
			Env->Console->DisplayError( Str );
		}
	}

	WriteLineToLog( LogString.c_str(), Ctx );

	// NOTE: text console logging (slow)
	if ( LogLevel == L_WARNING || LogLevel == L_LOG )
	{
#ifdef OS_WINDOWS
		WORD Attr = 15;

		switch ( LogLevel )
		{
			case L_DEBUG:
				Attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
				break;
			case L_NOTICE:
				Attr = FOREGROUND_GREEN;
				break;
			case L_LOG:
				Attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
				fflush( FLogFile );
				break;
			case L_WARNING:
				Attr = FOREGROUND_RED | FOREGROUND_INTENSITY;
				fflush( FLogFile );
				break;
		}

		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), Attr );
#endif

		if ( !Ctx->FThreadName.empty() )
		{
			printf( "(%s):%s\n", Ctx->FThreadName.c_str(), LogString.c_str() );
		}
		else
		{
//			FIXME(%u pattern for size_t gives a reasonable warning on 64-bit posix systems)
			printf( "(%u):%s\n", iThread::GetCurrentThread(), LogString.c_str() );
		}

#ifdef OS_WINDOWS
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 15 );
#endif
	}
}

void clLogger::WriteLineToLog( const char* Line, sThreadLogContext* Ctx ) const
{
#if defined(OS_ANDROID)
	__android_log_print( ANDROID_LOG_INFO, "LEngine", "(%s):%s", Ctx->FThreadName.c_str(), Line );
#endif // OS_ANDROID

	if ( FLogFile == 0 ) { return; }

	if ( !Ctx->FThreadName.empty() )
	{
		fprintf( FLogFile, "(%s):%s\n", Ctx->FThreadName.c_str(), Line );
	}
	else
	{
		fprintf( FLogFile, "(%u):%s\n", iThread::GetCurrentThread(), Line );
	}

#if !defined( NDEBUG )
	fflush( FLogFile );
#endif // NDEBUG
}

void clLogger::SetCurrentThreadName( const LString& Name )
{
	SetThreadName( iThread::GetCurrentThread(), Name );
}

#if defined( PLATFORM_MSVC )
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType;     // Must be 0x1000.
	LPCSTR szName;    // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
#endif  // PLATFORM_MSVC

void clLogger::SetThreadName( size_t ThreadID, const LString& Name )
{
	sThreadLogContext* Ctx = GetThreadLogContext();

	Ctx->FThreadName = Name;

#if defined( PLATFORM_MSVC )

	if ( !IsDebuggerPresent() ) { return; }

	const DWORD MS_VC_EXCEPTION = 0x406D1388;

	THREADNAME_INFO info;

	info.dwType = 0x1000;

	info.szName = Name.c_str();

	info.dwThreadID = ( DWORD )ThreadID;

	info.dwFlags = 0;

	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof( info ) / sizeof( ULONG_PTR ), ( ULONG_PTR* )&info );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
#endif // PLATFORM_MSVC

#if defined( OS_POSIX ) && !defined( OS_ANDROID )
	pthread_setname_np( ThreadID, Name.c_str() );
#endif // OS_POSIX

#if defined( OS_ANDROID )
	Log( L_DEBUG, "Thread "+LStr::ToStr(ThreadID)+" is named '"+Name.c_str()+"'" );
#endif // OS_ANDROID
}

void clLogger::PushProc( const char* Str )
{
	sThreadLogContext* Ctx = GetThreadLogContext();

	new ( &( Ctx->FProcsNesting[ Ctx->FProcsNestingLevel ] ) ) LStringBuffer( Str );
	Ctx->FProcsNestingLevel++;
	Ctx->FLogsOnThisLevel.push_back( false );
}

void clLogger::PopProc()
{
	sThreadLogContext* Ctx = GetThreadLogContext();

	if ( Ctx->FLogsOnThisLevel.back() )
	{
		Log( L_DEBUG, "<-" );
	}

	Ctx->FLogsOnThisLevel.pop_back();
	Ctx->FProcsNestingLevel--;
}

LString clLogger::GetTimeStamp() const
{
	time_t TempTime;
	time( &TempTime );

	::tm Time = *localtime( &TempTime );

	LString TimeStamp = LStr::GetFormatted( "%02d:%02d:%02d.%03d   ", Time.tm_hour, Time.tm_min, Time.tm_sec, GetCurrentMilliseconds() );

	return TimeStamp;
}

LString clLogger::GetCurrentProcsNesting() const
{
	sThreadLogContext* Ctx = GetThreadLogContext();

	LString Str;

	for ( int i = 0; i != Ctx->FProcsNestingLevel; i++ ) { Str += Ctx->FProcsNesting[i].ToString(); }

	return Str;
}

LString clLogger::GetCurrentProcsNestingSeparated() const
{
	sThreadLogContext* Ctx = GetThreadLogContext();

	LString Nesting;

	for ( int i = 0; i != Ctx->FProcsNestingLevel; i++ )
	{
		Nesting += LString( "    " ) + Ctx->FProcsNesting[i].ToString() + "\n";
	}

	return Nesting;
}

LString clLogger::PrepareErrorMessage( const LString& ErrorMessage, const LString& File, const LString& Line, const LString& Context )
{
	sThreadLogContext* Ctx = GetThreadLogContext();

	LString Error( ErrorMessage );

	Error += "\n\n Debug info: ";

	Error += "\n\n    Source file: " + File;
	Error +=  "\n    Source line: " + Line;
	Error +=  "\n    Thread: " + Ctx->FThreadName;

	Error += "\n\n History: \n\n";

	if ( Context.empty() )
	{
		// create accuratly looking history
		Error += GetCurrentProcsNestingSeparated();
	}
	else
	{
		Error += Context + "\n";
	}

	Log( L_WARNING, "\n\n Fatal error: \n\n" + Error );

	for ( size_t i = 0; i != Error.length(); ++i )
	{
		if ( Error[i] < ' ' && Error[i] != '\n' )
		{
			Error[i] = '#';
		}
	}

	return Error;
}

void clLogger::Fatal( const LString& ErrorMessage,
                      const LString& File,
                      const LString& Line,
                      const LString& Context )
{
	LString Error = PrepareErrorMessage( ErrorMessage, File, Line, Context );

	SendSync( L_EVENT_FATAL, LEventArgs( &Error, 0, this, false ), false );

	clScreen::ShowMessageBox( "Linderdaum Engine. Fatal error:", Error, L_MB_OK, true );

#ifdef _MSC_VER
	_ASSERT( 0 );
#endif

	exit( 255 );
}

void clLogger::Error( const LString& ErrorMessage, const LString& File, const LString& Line, const LString& Context )
{
	LString Error = PrepareErrorMessage( ErrorMessage, File, Line, Context );

	Env->SendSync( L_EVENT_ERROR, LEventArgs( &Error, 0, this, false ), false );
}

clLogger::sThreadLogContext* clLogger::GetThreadLogContext() const
{
	size_t Thread = iThread::GetCurrentThread();

	for ( size_t i = 0; i != FThreads.size(); i++ )
	{
		if ( FThreads[i].FThreadID == Thread ) { return &FThreads[i]; }
	}

	LMutex Lock( &FMutex );

	FThreads.push_back( sThreadLogContext( Thread ) );

	return &FThreads.back();
}

/*
 * 18/02/2011
     LogP()
 * 16/02/2011
     Thread info on error
 * 20/01/2011
     Thread-safety and many threading related code
 * 13/01/2011
     WriteLineToLog()
 * 06/08/2010
     Colored output is for any OS_WINDOWS target
 * 26/05/2009
     Changed fatal message formatting
 * 04/04/2009
     Colored output is only for _WIN32 target
 * 30/03/2009
     Colored console output
 * 05/04/2008
     Added more params for L_EVENT_LOG
 * 29/05/2007
     (LV) ANSI-style timestamp retrieval
 * 14/04/2007
     Memory leaks info dumping to log on shutdown
 * 16/11/2005
     LogFileName == 0 changed to LogFileName.empty()
 * 03/11/2005
     EnableFileLogging()
 * 12/04/2005
     Source file name & line number tracking in log
 * 27/01/2005
     Cleaner output to log in Fatal()
 * 03/12/2004
     Fatal()
 * 02/12/2004
     First working version
     GetTimeStamp()
 * 26/11/2004
     It's here
*/
