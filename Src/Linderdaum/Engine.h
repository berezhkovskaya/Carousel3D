/**
 * \file Engine.h
 * \brief Engine information constants
 * \version 0.6.08
 * \date 26/02/2012
 * \author Sergey Kosarevsky, 2004-2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _Engine_
#define _Engine_

#include "Platform.h"
#include "LString.h"

///////////////////

#if !defined(NDEBUG) || L_ENABLE_OPENGL_TRACING
#  define CheckRendererError(Info) Env->Renderer->CheckError(__FILE__, LStr::ToStr(__LINE__), (Info) )
#else
#  define CheckRendererError(Info)
#endif

#if defined( DISABLE_GUARD_UNGUARD_MECHANISM ) && !L_ENABLE_OPENGL_TRACING
#  define guardEnv(Env, Pattern, ...) {
#  define guard(...) {
#  define unguard() }
#else
#  define guardEnv(Env, Pattern, ...) { ::Linderdaum::clGuardian Guardian( Env->Logger, FUNC_NAME, Pattern, __VA_ARGS__ );
#  define guard(...) { ::Linderdaum::clGuardian Guardian( Env->Logger, FUNC_NAME, ## __VA_ARGS__ );
#  define unguard() }
#endif

#define LOCALIZE(Str) Env->Localizer->LocalizeString( Str )

///////////////////

#if defined( OS_ANDROID )
#  define EXTRACT_COMMAND_LINE(Str) { Str.clear(); Str[0] = sEnvironment::FCommandLine; }
#else
#  define EXTRACT_COMMAND_LINE(Str) { Str.clear(); for (int i = 1; i < argc; i++) Str.push_back( LString( argv[i] ) ); }
#endif

#if defined( OS_WINDOWS )
const char PATH_SEPARATOR = '\\';
#else
const char PATH_SEPARATOR = '/';
#endif // OS_WINDOWS

enum LEndianness
{
	BIG = 0,
	LITTLE = 1
};

const LEndianness ENDIANNESS = *( const char* )&Endianness == 0xEF ? LITTLE : BIG;

/// Generic namespace for Linderdaum Engine
namespace Linderdaum
{
	const LString ENGINE_VERSION        = LString( "0.6.08 ($Build: Id $)" );
	const LString ENGINE_NAME           = LString( "Linderdaum Engine" );
	const LString ENGINE_COPYRIGHT1     = LString( "(C) Sergey Kosarevsky, 2004-2012" );
	const LString ENGINE_COPYRIGHT2     = LString( "(C) Viktor Latypov, 2007-2012" );

#ifndef NDEBUG
	const LString ENGINE_BUILD_MODE = "debug build";
#else
	const LString ENGINE_BUILD_MODE = "release build";
#endif // NDEBUG

	const LString ENGINE_BUILD_OS =
#if defined(_WIN64)
	"Win64";
#elif defined(OS_WINDOWS)
	"Win32";
#elif defined(OS_ANDROID)
	"Android";
#elif defined(OS_LINUX)
	"Linux";
#elif defined(OS_MACOS)
	"MacOS X";
#else
	"Unknown";
#endif

	const LString ENGINE_BUILD_TARGET =
#if defined(__ppc64__)
	"ppc64";
#elif defined(__ppc__) || defined(_M_PPC)
	"ppc32";
#elif defined(_M_ARM) || defined(__arm__)
	"arm";
#elif defined(_M_X64) || defined(__x86_64__)
	"amd64";
#else
	"x86";
#endif

	const LString ENGINE_TARGET = "(" + ENGINE_BUILD_MODE + ", " + ENGINE_BUILD_OS + ", " + ENGINE_BUILD_TARGET + ")";

	const LString ENGINE_COMPILE_TIME   = LString( __DATE__ ) + "  " + LString( __TIME__ ) + " via " + __COMPILER_VER__ + " " + ENGINE_TARGET;

	const LString DEFAULT_ENGINE_LOG_FILE  = LString( "Data/Engine.log" );
	const LString DEFAULT_ENGINE_INI_FILE  = LString( "StdConfig/System.cfg" );
	const LString DEFAULT_ENGINE_ROOT_DIR  = LString( "Data" );
	const LString DEFAULT_CONSOLE_AUTOEXEC = LString( "StdConfig/console.cfg" );
	////////////////////////////////
}

using namespace ::Linderdaum;

#endif

/*
 * 26/02/2012
     0.6.08
 * 28/01/2012
     0.6.06
 * 01/01/2012
     0.6.04
 * 19/11/2011
     0.6.02
 * 21/07/2011
     0.6.01
 * 12/07/2011
     0.6.00
 * 08/05/2011
     0.5.99d
 * 08/04/2011
     0.5.99c
 * 03/12/2010
     0.5.99b
 * 30/11/2010
     0.5.98
 * 15/11/2010
     0.5.96
 * 17/10/2010
     0.5.94
 * 18/09/2010
     0.5.92
 * 17/01/2010
     0.5.90
 * 23/02/2009
     0.5.89
 * 28/05/2008
     0.5.88
 * 24/02/2008
     0.5.86
 * 23/12/2007
     0.5.84
 * 19/11/2007
     0.5.82
 * 28/09/2007
     0.5.80
 * 03/09/2007
     Scene manager
 * 27/07/2007
     iWorldsManager deprecated (iWorld is now a first person citizen)
 * 11/06/2007
     iGameManager deprecated
 * 29/05/2007
     iTimer deprecated
     iMainLoop deprecated
     iCVarsManager deprecated
 * 10/03/2007
     0.5.72
 * 30/12/2006
     0.5.70
 * 24/11/2006
     0.5.68d
 * 23/10/2006
     0.5.68c
 * 15/08/2006
     0.5.68b
 * 15/07/2006
     0.5.68a
 * 01/07/2006
     0.5.68
 * 28/03/2006
     0.5.66
 * 23/01/2006
     0.5.64
 * 31/12/2005
     0.5.62
 * 11/12/2005
     0.5.60
 * 27/11/2005
     0.5.58
 * 20/11/2005
     0.5.56
 * 13/11/2005
     0.5.54
 * 03/11/2005
     sEnvironment
     sEnvironment::MainLoop deprecated
 * 02/11/2005
     Linderdaum::OS deprecated
     Linderdaum::Output deprecated
 * 01/11/2005
     0.5.52
 * 30/10/2005
     clGuardian moved to Exceptions.h
 * 28/10/2005
     A lot of "#include" directives moved out of here
 * 27/06/2005
     0.5.50
 * 15/06/2005
     0.5.48
 * 06/06/2005
     0.5.46
 * 30/05/2005
     0.5.44
 * 15/05/2005
     0.5.42
 * 10/05/2005
     0.5.40
 * 06/05/2005
     0.5.38
 * 30/04/2005
     0.5.36
 * 24/04/2005
     0.5.34
 * 17/04/2005
     0.5.32
 * 10/04/2005
     0.5.30
 * 04/04/2005
     0.5.28
 * 31/03/2005
     0.5.26
 * 27/03/2005
     0.5.24
 * 24/03/2005
     0.5.22
 * 20/03/2005
     0.5.20
 * 13/03/2005
     0.5.18
 * 08/03/2005
     0.5.16
 * 03/03/2005
     0.5.14
 * 23/02/2005
     0.5.12
 * 19/02/2005
     0.5.10
 * 14/02/2005
     0.5.08
 * 05/02/2005
     0.5.06
 * 01/02/2005
     0.5.04
 * 27/01/2005
     0.5.02
 * 23/01/2005
     0.5.00
 * 15/01/2005
     Consts instead of defines
 * 17/12/2004
     SubSystemsManager
 * 08/12/2004
     OS
 * 03/12/2004
     Output
     Timer
 * 01/12/2004
     LString.h
 * 26/11/2004
     It's here
*/
