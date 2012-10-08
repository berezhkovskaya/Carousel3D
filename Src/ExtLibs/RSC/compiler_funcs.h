/*
   Compiler function headers

   RealScript Compiler SDK
   (C) Viktor Latypov, 2007
*/

#ifndef __compiler__funcs__h__included__
#define __compiler__funcs__h__included__

#include "compiler_types.h"

#include "compiler_log_interface.h"
#include "sys_interface.h"

typedef RS_Compiler_Handle *( *RS_CreateCompiler_func )( RS_Compiler_log_interface* log, RS_Sys_interface* sys, void* data );
typedef int ( *RS_DestroyCompiler_func )( RS_Compiler_Handle* compiler );

typedef int ( *RS_Compiler_GetOperatorCount_func ) ( RS_Compiler_Handle* compiler );
typedef int ( *RS_Compiler_GetTypeCount_func )     ( RS_Compiler_Handle* compiler );
typedef int ( *RS_Compiler_GetGlobalCount_func )   ( RS_Compiler_Handle* compiler );
typedef int ( *RS_Compiler_GetClassCount_func )    ( RS_Compiler_Handle* compiler );
typedef int ( *RS_Compiler_GetMethodCount_func )   ( RS_Compiler_Handle* compiler, RS_Class* _class, RS_State* _state );
typedef int ( *RS_Compiler_GetFieldCount_func )    ( RS_Compiler_Handle* compiler, RS_Class* _class );
typedef int ( *RS_Compiler_GetStateCount_func )    ( RS_Compiler_Handle* compiler, RS_Class* _class );

typedef RS_Operator ( *RS_Compiler_GetOperator_func )        ( RS_Compiler_Handle* compiler, int idx );
typedef RS_Type     ( *RS_Compiler_GetType_func )            ( RS_Compiler_Handle* compiler, int idx );
typedef RS_Symbol   ( *RS_Compiler_GetGlobal_func )          ( RS_Compiler_Handle* compiler, int idx );
typedef RS_Class    ( *RS_Compiler_GetClass_func )           ( RS_Compiler_Handle* compiler, int idx );
typedef RS_Symbol   ( *RS_Compiler_GetField_func )           ( RS_Compiler_Handle* compiler, int idx );
typedef RS_State    ( *RS_Compiler_GetState_func )           ( RS_Compiler_Handle* compiler, RS_Class* _class, int idx );
typedef RS_Symbol   ( *RS_Compiler_GetMethodParam_func )     ( RS_Compiler_Handle* compiler, RS_Method* method, int idx );
typedef RS_Method   ( *RS_Compiler_GetMethod_func )          ( RS_Compiler_Handle* compiler, RS_Class* _class, RS_State* _state, int idx );
typedef const char *( *RS_Compiler_GetMethodSignature_func ) ( RS_Compiler_Handle* compiler, RS_Class* _class, RS_State* _state, RS_Method* _method );

typedef const char *( *RS_Compiler_GetNameByID_func )( RS_Compiler_Handle* compiler, int id );
typedef int ( *RS_Compiler_GetIDByName_func )( RS_Compiler_Handle* compiler, const char* _name );

typedef int ( *RS_Compiler_Validate_func )         ( RS_Compiler_Handle* compiler, const char* fname );
typedef int ( *RS_Compiler_Compile_func )          ( RS_Compiler_Handle* compiler, const char* fname, const char* dfname ); // dest file name for exprs/statments etc.
typedef int ( *RS_Compiler_ProcessDirectory_func ) ( RS_Compiler_Handle* compiler, const char* dname );

typedef void ( *RS_Compiler_SetOption_func ) ( RS_Compiler_Handle* compiler, const char* oname, const char* oval );

typedef int ( *RS_Compiler_RecompileClasses_func ) ( RS_Compiler_Handle* compiler );
typedef int ( *RS_Compiler_RecompileClass_func )   ( RS_Compiler_Handle* compiler, const char* cname );
typedef int ( *RS_Compiler_RecompileMethods_func ) ( RS_Compiler_Handle* compiler );
typedef int ( *RS_Compiler_RecompileMethod_func )  ( RS_Compiler_Handle* compiler, const char* mname );

typedef int ( *RS_Compiler_CompileStatementBuffer_func ) ( RS_Compiler_Handle* compiler, const char* buf, int len );
typedef const char *( *RS_Compiler_GetCodeBuffer_func )  ( RS_Compiler_Handle* compiler );

typedef int ( *RS_Compiler_Initialize_func )( RS_Compiler_Handle* compiler );

typedef void ( *RS_Compiler_ClearSettings_func )( RS_Compiler_Handle* compiler );

typedef void ( *RS_Compiler_SetOutputDirectory_func )( RS_Compiler_Handle* compiler, const char* dname );
typedef const char *( *RS_Compiler_GetOutputDirectory_func )( RS_Compiler_Handle* compiler );

typedef void ( *RS_Compiler_AddLibDirectory_func )( RS_Compiler_Handle* compiler, const char* dname );
typedef void ( *RS_Compiler_AddExportDirectory_func )( RS_Compiler_Handle* compiler, const char* dname );

typedef void ( *RS_Compiler_ClearBreakpoints_func )( RS_Compiler_Handle* compiler );
typedef void ( *RS_Compiler_AddBreakpoint_func )( RS_Compiler_Handle* compiler, RS_Source_location* loc );

typedef const char *( *RS_Compiler_GetLastErrorCode_func )( RS_Compiler_Handle* compiler );
typedef const char *( *RS_Compiler_GetLastErrorFile_func )( RS_Compiler_Handle* compiler );
typedef int         ( *RS_Compiler_GetLastErrorLine_func )( RS_Compiler_Handle* compiler );
typedef int         ( *RS_Compiler_GetLastErrorPos_func )( RS_Compiler_Handle* compiler );

#endif
