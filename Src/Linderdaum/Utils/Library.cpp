/**
 * \file Library.cpp
 * \brief DLL/SO library
 * \version 0.5.67
 * \date 03/05/2006
 * \author Sergey Kosarevsky, 2005-2006
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Library.h"
#include "Environment.h"

#include "Core/VFS/FileSystem.h"

#include "Utils/Utils.h"

#ifdef __linux__
#include <dlfcn.h>
#endif

clLibrary::clLibrary()
{
}

bool clLibrary::Load( const LString& LibName )
{
#if defined( OS_WINDOWS )
	FLibHandle = ::LoadLibrary( LibName.c_str() );

	if ( !FLibHandle )
	{
		LString DLL = FindLibrary( LibName );

		FLibHandle = ::LoadLibrary( DLL.c_str() );
	}

#else
	const char* ErrStr;

	FLibHandle = dlopen( LibName.c_str(), RTLD_LAZY );

	if ( ( ErrStr = dlerror() ) != NULL )
	{
		FLibHandle = NULL;
	}

#endif

	CHECK_RET( !FLibHandle, false, "Unable to load library " + LibName );

	return true;
}

LString clLibrary::FindLibrary( const LString& LibName )
{
	// TODO
	if ( clFileSystem::FileExistsPhys( LibName ) ) { return LibName; }

	if ( clFileSystem::FileExistsPhys( AdditionalDLLPath + LibName ) ) { return AdditionalDLLPath + LibName; }

	if ( clFileSystem::FileExistsPhys( "..\\..\\" + AdditionalDLLPath + LibName ) ) { return LString( "..\\..\\" ) + AdditionalDLLPath + LibName; }

	return "";
}

clLibrary::~clLibrary()
{
#ifdef OS_WINDOWS
	FreeLibrary( FLibHandle );
#else
	dlclose( FLibHandle );
#endif
}

void* clLibrary::GetProcAddress( const LString& ProcName ) const
{
#ifdef OS_WINDOWS
	return ::Linderdaum::Utils::ultimate_cast<void*>( ::GetProcAddress( FLibHandle, ProcName.c_str() ) );
#else
	const char* err_str;

	void* ret = dlsym( FLibHandle, ProcName.c_str() );

	if ( ( err_str = dlerror() ) != NULL )
	{
		return NULL; //fprintf(stdout,"cannot link to SO : %s\n",err_str);
	}

	return ret;
#endif
}

/*
 * 03/05/2006
     Ugly typecast removed in GetProcAddress()
 * 08/12/2004
     It's here
*/
