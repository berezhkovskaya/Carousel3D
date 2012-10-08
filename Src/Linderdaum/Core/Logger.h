/**
 * \file Logger.h
 * \brief Global debug logging
 * \version 0.6.00
 * \date 04/05/2011
 * \author Sergey Kosarevsky, 2004-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LoggingDevice_
#define _LoggingDevice_

#include "Core/iObject.h"
#include "Utils/LArray.h"

enum LLogLevel
{
	L_PARANOID = 0,
	L_DEBUG    = 1,
	L_NOTICE   = 2,
	L_LOG      = 3,
	L_WARNING  = 4
};

LString LogLevelToString(LLogLevel L);
LLogLevel StringToLogLevel(const LString& S);

namespace Linderdaum
{
	class clException;
};

class clMutex;

/**
   \brief Global debug logging

   Provides multi-level logging with current execution context tracking.
   This class is implemented using FILE* to ensure maximum portability and simplicity.
**/
class scriptfinal clLogger: public iObject
{
private:
	static const size_t DefaultThreadsEstimate = 8;
public:
	clLogger(): FLogFile( NULL ), FThreads( DefaultThreadsEstimate ), FCurrentLogLevel( L_DEBUG ), FLastTimeStamp( 0.0 ) {};
	virtual ~clLogger()
	{
		ShutdownLogger();
	};

	NET_EXPORTABLE()

	scriptmethod void    SetCurrentThreadName( const LString& Name );
	scriptmethod void    SetThreadName( size_t ThreadID, const LString& Name );

	/**
	   Write a message to the LogFile
	      \param LogLevel - type of the message : Notice, Warning or Error
	      \param Str      - debug message
	**/
	scriptmethod void    Log( LLogLevel LogLevel, const LString& Str ) const;
	void                 LogP( LLogLevel LogLevel, const char* Pattern, ... ) const;
	/**
	   Write timing information to the log since the last call to MarkTime(), 
      time will be placed as %f in Pattern
	**/
	scriptmethod void    MarkTime( LLogLevel LogLevel, const char* Pattern ) const;
	/**
	   Only logs with the specified LogLevel and higher will be saved to file
	**/
	scriptmethod void      SetLogLevel( LLogLevel LogLevel ) { FCurrentLogLevel = LogLevel; };
	scriptmethod LLogLevel GetLogLevel() const { return FCurrentLogLevel; };
	/**
	   Initialize logging
	   Called at statup
	**/
	scriptmethod void    EnableFileLogging( const LString& LogFileName );
	/**
	   Form a string with detailed call stack information
	**/
	scriptmethod LString GetCurrentProcsNesting() const;
	/**
	   Form a multiline string with detailed call stack information
	**/
	scriptmethod LString GetCurrentProcsNestingSeparated() const;
	/**
	   Service function which pushes the routine name to internal stack.
	   Called from guard() macro at the beginning of every routine being debugged.
	**/
	scriptmethod void    PushProc( const char* Str );
	/**
	   Service function which pops the routine name from internal stack.
	   Called from unguard() macro at the end of every routine being debugged.
	**/
	scriptmethod void    PopProc();
	/**
	   Terminate Engine execution on some fatal exception
	   Called from FATAL/FATAL_MSG macros
	**/
	scriptmethod void    Fatal( const LString& ErrorMessage, const LString& File, const LString& Line, const LString& Context );

	///   Terminate Engine excution and log the information about exception
	scriptmethod void    FatalException( const ::Linderdaum::clException* E );

	/**
	   Generate non-fatal error message
	**/
	scriptmethod void    Error( const LString& ErrorMessage, const LString& File, const LString& Line, const LString& Context );
private:
	static const int MAX_PROCS_NESTING = 32;
private:
	struct sThreadLogContext
	{
		sThreadLogContext(): FThreadID( 0 ), FProcsNestingLevel( 0 ), FLogsOnThisLevel(), FThreadName() {};
		sThreadLogContext( size_t ThreadID ): FThreadID( ThreadID ), FProcsNestingLevel( 0 ), FLogsOnThisLevel(), FThreadName() {};

		size_t           FThreadID;
		/// Current call stack
		LStringBuffer    FProcsNesting[MAX_PROCS_NESTING];
		int              FProcsNestingLevel;
		/// Boolean flags indicating presence of any log messages on current execution level
		LArray<bool>     FLogsOnThisLevel;
		/// User-defined name of the thread
		LString          FThreadName;
	};
	sThreadLogContext* GetThreadLogContext() const;
	void      ShutdownLogger();
	LString   GetTimeStamp() const;
	LString   PrepareErrorMessage( const LString& ErrorMessage, const LString& File, const LString& Line, const LString& Context );
	void      WriteLineToLog( const char* Line, sThreadLogContext* Ctx ) const;
private:
	/// Actual handle for the file with debug log
	FILE*                   FLogFile;

	mutable std::vector<sThreadLogContext> FThreads;
	mutable clMutex                        FMutex;

	LLogLevel FCurrentLogLevel;

	mutable double FLastTimeStamp;
};

#endif

/*
 * 04/05/2011
     Added L_PARANOID
 * 18/02/2011
     LogP()
 * 20/01/2011
     Thread-safety and many threading related code
 * 13/01/2011
     WriteLineToLog()
 * 27/02/2008
     Merged with iLoggingDevice
 * 21/01/2005
     Pass parameters as const strings by reference
 * 26/11/2004
     It's here
*/
