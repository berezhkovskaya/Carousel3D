/**
 * \file Platform.h
 * \brief Platform (OS, compiler) specific definitions
 * \version 0.5.97
 * \date 15/11/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _Platform_
#define _Platform_

#define L_TRACK_ALLOCATION_CONTEXT			1

#define L_ENABLE_OPENGL_TRACING				0

#define L_AUDIO_USE_OPENAL						1

#define L_AUDIO_USE_FMOD						0

#define L_AUDIO_USE_OGG							1

#define L_USE_FREEIMAGE							1

#undef OS_WINDOWS
#undef OS_LINUX
#undef OS_MACOSX
#undef OS_QNX
#undef OS_POSIX

#undef OS_32BIT
#undef OS_64BIT
#undef PLATFORM_MSVC
#undef PLATFORM_GCC

#if defined(_WIN32) || defined(_WIN64)
#  define OS_WINDOWS
#endif // _WIN32 || _WIN64

#if defined(__linux__)
#  define OS_LINUX
#endif // __linux__

#if defined(ANDROID)
#  define OS_ANDROID
#endif // ANDROID

#if defined(__APPLE__) && defined(__MACH__)
#  define OS_MACOSX
// No iOS detection, must be done using #include <AvailabilityMacros.h> and <TargetConditionals.h>
#endif

#if defined(__QNX__) || defined(__QNXNTO__)
#  define OS_QNX
#endif

#if defined(OS_MACOSX) || defined(OS_LINUX) || defined(OS_QNX)
#  define OS_POSIX
#endif

#if defined(_LP64) || defined(__x86_64__) || defined(__LP64__) || defined(_WIN64) || defined(_M_X64) || defined(__ppc64__)
#  define OS_64BIT
#else
#  define OS_32BIT
#endif

#ifdef OS_WINDOWS
#include <tchar.h>
// we use COM only in a procedural style, so these macros must be
// defined before including <windows.h>
// #define CINTERFACE
// #define COBJMACROS
#endif // OS_WINDOWS

//#define TODO_AND_FIXME_OUTPUT

#ifdef TODO_AND_FIXME_OUTPUT
#  define __STR2__(x) #x
#  define __STR1__(x) __STR2__(x)
#  define __LOC2__ __FILE__ "("__STR1__(__LINE__)") : "
#  define TODO(text)  COMPILER_MESSAGE("TODO", text);
#  define FIXME(text) COMPILER_MESSAGE("FIXME", text);
#else
#  define TODO(text)
#  define FIXME(text)
#endif // TODO_AND_FIXME_OUTPUT

// GCC
#ifdef __GNUC__
#  define PLATFORM_GCC
#  include "Utils/PlatformGCC.h"
#endif

// Microsoft Visual C++
#ifdef _MSC_VER
#  define PLATFORM_MSVC
#  include "Utils/PlatformMSVC.h"
#endif

APPLICATION_SHUTDOWN;

#undef  L_USE_NETWORK

// by default FMOD is off (?)

#undef L_VIDEO_USE_VFW

// for testing
#ifdef OS_ANDROID
//#   define DISABLE_GUARD_UNGUARD_MECHANISM
#endif


#if !defined(__linux__) && defined(__GNUC__)
// FMOD does not provide C++ interface for win32 gcc (namely, mingw and cygwin)
#  undef L_AUDIO_USE_FMOD
#endif

#ifdef OS_POSIX
#  undef L_VIDEO_USE_VFW
#  undef L_USE_DX11
#endif // OS_LINUX

// OpenGL
#if defined( OS_ANDROID )
// OpenGL ES 2 in Android
#  define USE_OPENGL_ES_2
#elif defined( OS_LINUX )
// OpenGL 3 on any desktop Linux
#  define USE_OPENGL_3
#elif defined( OS_WINDOWS )
// OpenGL 3 on Windows
#  define USE_OPENGL_3
#elif defined( OS_MACOSX )
// OpenGL 2 on older Macs, to be implemented
#else
#  error Define OpenGL version to use!
#endif

// we are not using PugiXML right now, so it is switched off
#define L_VFS_USE_PUGIXML

typedef unsigned int     Luint;
typedef int              Lint;
typedef unsigned char    Lubyte;
typedef signed char      Lbyte;
typedef unsigned int     Lenum;
typedef unsigned short   Lushort;
typedef short            Lshort;
typedef int              Lsizei;
typedef void*            Lhandle;

/// Specify the .NET-exported class
#define netexportable

/// Declare the non-scriptexported class
#define scriptfinal

/// Define the method exported to script
#ifndef scriptmethod
#  define scriptmethod
#endif

/// Specify a class where the static exported method will reside
#ifndef scriptexport
#define scriptexport(DotNETStorageClassName)
#endif

/// Make a method nonexportable to script/.NET
#define noexport

/// Declare exported native field
#define nativefield

#ifndef NULL
#  define NULL 0
#endif

#define PROPERTY(...)

#endif

/*
 * 15/11/2010
     Merged with LTypes.h
 * 14/04/2009
     property(PropName)
 * 30/01/2005
     It's here
*/
