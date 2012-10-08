/**
 * \file PlatformGCC.h
 * \brief GCC compiler specific definitions
 * \version 0.5.99c
 * \date 01/04/2011
 * \author Sergey Kosarevsky, 2005-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _PlatformGCC_
#define _PlatformGCC_

#include <stddef.h>

#define L_USE_LIBUUID

#ifdef OS_ANDROID
#  define APPLICATION_ENTRY_POINT sEnvironment* android_main(int argc, char* argv[])
#  define APPLICATION_EXIT_POINT(_Env) return _Env;
#  define NO_CONSOLE_WINDOW
#else
#  if defined(OS_POSIX)
#    define APPLICATION_ENTRY_POINT int main(int argc, char* argv[])
#  else
#    define APPLICATION_ENTRY_POINT int main(int argc, _TCHAR* argv[])
#  endif
#  define APPLICATION_EXIT_POINT(_Env) LEngineApplicationShutdown(); _Env->ShutdownEnvironment(); delete(_Env); return 0;
#endif

#define APPLICATION_SHUTDOWN void LEngineApplicationShutdown()

#define __COMPILER_VER__ "GCC"

#define GCC_CONVERT_TYPEINFO(Name) \
{ \
   char  ResultBuffer[256]; \
   char* Buff     = &ResultBuffer[0]; \
   size_t Digit   = 0; \
   const char* ch = Name.c_str(); \
   \
   while ( *ch ) \
   { \
      if ( (*ch >= '0') && (*ch <= '9') ) \
      { \
         if ( (!Digit) && (Buff > &ResultBuffer[0]+1) ) { *Buff++ = ':'; *Buff++ = ':'; }; \
         Digit = Digit * 10 + (*ch-'0'); \
         ch++; \
      } \
      else \
      { \
         if ( Digit ) do { *Buff++ = *ch++; } while ( --Digit && *ch ); else ch++;\
      } \
   } \
   *Buff = 0; \
   Name.assign( ResultBuffer ); \
}

#define GCC_PACK(n) __attribute__((packed,aligned(n)))
#define CACHELINE_SIZE        16
#define CACHELINE_ALIGNED     __attribute__((packed,aligned(CACHELINE_SIZE)))

#define COMPILER_MESSAGE( Prefix, Message ) __pragma(message(__LOC2__ Prefix ": " Message))

#define FUNC_NAME __PRETTY_FUNCTION__

#include <typeinfo>
#include <stdint.h>
typedef int64_t       Lint64;
typedef uint64_t      Luint64;
typedef int32_t       Lint32;
typedef uint32_t      Luint32;
const uint32_t Endianness = 0xDEADBEEF;

#define LASSERT( a )

#endif

/*
 * 01/04/2011
     Special APPLICATION_ENTRY_POINT for Android
 * 10/09/2010
     GCC_CONVERT_TYPEINFO reimplemented in C-style
 * 09/11/2007
     Fixes for PACKED_STRUCT()
 * 18/03/2007
     Included stddef.h
 * 11/01/2006
     PACKED_STRUCT()
     PACKED_STRUCT_END()
 * 19/11/2005
     Fixed bug in GCC_CONVERT_TYPEINFO()
 * 08/11/2005
     STACK_FRAME_PTR deprecated
 * 29/10/2005
     GCC_CONVERT_TYPEINFO() made more sophisticated
 * 30/01/2005
     It's here
*/
