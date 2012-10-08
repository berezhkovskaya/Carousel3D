/*
   Compiler debug logging interface

   RealScript Compiler SDK
   (C) Viktor Latypov, 2007
*/

#ifndef __compiler__log__interface__h__included__
#define __compiler__log__interface__h__included__

#include "interface_defs.h"

struct PACKED_STRUCTURE RS_CompilerLog_Handle
{
	void* LogObject;
	void* Reserved;
};

typedef RS_CompilerLog_Handle * ( *RS_CompilerLog_create_func )( void* data );
typedef int ( *RS_CompilerLog_destroy_func )( RS_CompilerLog_Handle* handle );

typedef int  ( * RS_Compiler_report_warning_func )( RS_CompilerLog_Handle* handle, RS_Source_location* loc, const char* msg );
typedef int  ( * RS_Compiler_report_error_func )( RS_CompilerLog_Handle* handle, RS_Source_location* loc, const char* msg );
typedef void ( * RS_Compiler_log_func )( RS_CompilerLog_Handle* handle, const char* msg );

// RS_Compiler_log_interface structure
struct PACKED_STRUCTURE RS_Compiler_log_interface
{
	// Compiler logging
	// and compile-time error reporting
	RS_Compiler_log_func             log;
	RS_Compiler_report_error_func    error;
	RS_Compiler_report_warning_func  warning;

	// create/destroy
	RS_CompilerLog_create_func  create_log;
	RS_CompilerLog_destroy_func destroy_log;
};

#endif
