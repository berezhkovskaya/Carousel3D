/**
 * \file PlatformMSVC.h
 * \brief MS VisualC++ compiler specific definitions
 * \version 0.5.84
 * \date 09/11/2007
 * \author Sergey Kosarevsky, 2005-2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _PlatformMSVC_
#define _PlatformMSVC_

#undef L_USE_LIBUUID

#ifndef _CONSOLE
#  define APPLICATION_ENTRY_POINT int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#  define NO_CONSOLE_WINDOW
#else
#  define APPLICATION_ENTRY_POINT int _tmain(int argc, _TCHAR* argv[])
#endif

#define APPLICATION_EXIT_POINT(_Env) LEngineApplicationShutdown(); _Env->ShutdownEnvironment(); delete(_Env); return 0;
#define APPLICATION_SHUTDOWN void LEngineApplicationShutdown()

#define __COMPILER_VER__ "Microsoft Visual C++"

#define GCC_CONVERT_TYPEINFO(Name) Name = Name.substr(6, Name.length()-6);

#define GCC_PACK(n) __declspec(align(n))
#define CACHELINE_SIZE        16
#define CACHELINE_ALIGNED     __declspec(align(CACHELINE_SIZE))

#define COMPILER_MESSAGE( Prefix, Message ) __pragma(message(__LOC2__ Prefix ": " Message))

#define FUNC_NAME __FUNCTION__

// C++ exception handling must be enabled
#ifndef _CPPUNWIND
#error "Bad MSVC option: C++ exception handling must be enabled"
#endif

// RTTI must be enabled
#ifndef _CPPRTTI
#error "Bad MSVC option: RTTI must be enabled"
#endif


// disable some of level 4 warnings
#pragma warning(disable : 4100) // unreferenced formal parameter
#pragma warning(disable : 4127) //
#pragma warning(disable : 4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable : 4311) // 'type cast' : pointer truncation from 'FARPROC' to 'unsigned int'
#pragma warning(disable : 4408) // anonymous union did not declare any data members
#pragma warning(disable : 4511) // copy constructor could not be generated
#pragma warning(disable : 4512) // assignment operator could not be generated
#pragma warning(disable : 4530) // C++ exception handler used, but unwind semantics are not enabled
#pragma warning(disable : 4800) // forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable : 4996) // was declared deprecated
#pragma warning(disable : 6011) // dereferencing NULL pointer
#pragma warning(disable : 6211) // leaking memory due to an exception
#pragma warning(disable : 6308) // 'realloc' might return null pointer

typedef __int64          Lint64;
typedef unsigned __int64 Luint64;
typedef __int32          Lint32;
typedef unsigned __int32 Luint32;
const unsigned __int32 Endianness = 0xDEADBEEF;

#define LASSERT( a ) _ASSERT( a );

#endif

/*
 * 02/01/2012
     Disabled warnings 6011, 6211 and 6308
 * 09/11/2007
     Fixes for PACKED_STRUCT()
 * 04/06/2006
     GCC_PACK() fixed
 * 11/01/2006
     PACKED_STRUCT()
     PACKED_STRUCT_END()
 * 21/12/2005
     Linking with ogg_static.lib
     Linking with vorbisfile_static.lib
 * 08/11/2005
     STACK_FRAME_PTR deprecated
 * 30/01/2005
     It's here
*/
