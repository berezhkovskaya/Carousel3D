/**
 * \file Exceptions.h
 * \brief Base facilities for exceptions handling
 * \version 0.6.00
 * \date 27/12/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _Exceptions_h_
#define _Exceptions_h_

#include "LString.h"

#define EXCEPTIONABLE class clExcept: public ::Linderdaum::clException { public: explicit clExcept(clLogger* Logger):clException(Logger) {}; }
#define EXCEPTION(ex_class, ex_message) class ex_class: public clExcept { public: explicit ex_class(clLogger* Logger):clExcept(Logger) { FReason=ex_message; } }

#define FATAL_MSG(message) FATAL_MSG_ENV(Env, message)

#define FATAL_MSG_ENV(env, message) { \
                            ::Linderdaum::clException E(env->Logger); E.SetFileAndLine( __FILE__, __LINE__ ); \
                            E.FReason.assign( message ); \
                            E.FatalException(); \
                           }

#define FATAL(condition, message) if ( condition ) FATAL_MSG(message)

#define FATAL_ENV(env, condition, message) if ( condition ) FATAL_MSG_ENV(env, message)

#define CHECK(condition) if ( condition ) FATAL_MSG("CHECK() condition failed")

#define ERROR_MSG(message) { \
                            Env->Logger->Error( message, __FILE__, LStr::ToStr(__LINE__), Env->Logger->GetCurrentProcsNestingSeparated() ); \
                           }

#define CHECK_ERROR(condition, message) if ( condition ) ERROR_MSG(message)

#define RAISE(class) { \
                       class E(Env->Logger); E.SetFileAndLine( __FILE__, __LINE__ ); \
                       E.FatalException(); \
                     }

#define RAISE_MSG(class, addmessage) { \
                                        class E(Env->Logger); E.SetFileAndLine( __FILE__, __LINE__ ); \
                                        E.FReason += LString(": ") + addmessage; \
                                        E.FatalException(); \
                                     }
#define RAISE_MSG_IF(condition, class, addmessage) if ( condition ) RAISE_MSG(class, addmessage)

class sEnvironment;

class clLogger;

class clError
{
public:
	int     FErrorCode;
	LString FErrorName;
};

#define CHECK_RET(condition, ErrorCode, ErrorName) if ( condition ) { FErrorName = ErrorName; FErrorCode = ErrorCode; return FErrorCode; };

namespace Linderdaum
{
	/// Handles guard() macro
	class clGuardian
	{
	public:
		clGuardian( clLogger* Logger, const char* FuncName );
		clGuardian( clLogger* Logger, const char* FuncName, const char* Pattern, ... );
		~clGuardian();
	private:
		clLogger* FLogger;
	};

	/// Parent class for all engine's exceptions
	class clException
	{
	public:
		explicit clException( clLogger* Logger );
		virtual ~clException() {};
		void SetFileAndLine( const char* File, int Line );
	public:
		clLogger*        FLogger;
		LString          FReason;
		LString          FContext;
		const char*      FFile;
		int              FLine;
		void    FatalException();
	};
}

#endif

/*
 * 27/12/2010
     Significant speed boost in guard()/unguard() mechanism
 * 01/11/2010
     Used 'const char*' and 'int' in SetFileAndLine()
 * 07/04/2009
     Shorter macros (no code duplication)
 * 06/04/2009
     clGuardian() accepts const LString& as parameter...
 * 30/10/2005
     clGuardian
 * 29/10/2005
     A lot of RAISE_* macros
 * 25/10/2005
     It's here
*/
