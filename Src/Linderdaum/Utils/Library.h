/**
 * \file Library.h
 * \brief DLL/SO library
 * \version 0.6.02
 * \date 16/09/2011
 * \author Sergey Kosarevsky, 2005-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _Library_
#define _Library_

#include "Platform.h"
#include "Exceptions.h"

#ifdef OS_WINDOWS
#include <windows.h>
#undef min
#undef max
#endif

#include "Core/iObject.h"

#ifdef OS_64BIT
const LString AdditionalDLLPath = "../../Libs.Win64/";
#else
const LString AdditionalDLLPath = "../../Libs.Win32/";
#endif

/// Dynamic link library
class scriptfinal clLibrary: public clError
{
public:
	explicit clLibrary();
	virtual ~clLibrary();
	//
	// clLibrary
	//
	virtual bool    Load( const LString& LibName );
	virtual void*   GetProcAddress( const LString& ProcName ) const;

private:
	/// Find the .dll/.so file somewhere
	LString FindLibrary( const LString& DLLName );

private:
#ifdef OS_WINDOWS
	HMODULE FLibHandle;
#else
	void*   FLibHandle;
#endif
};

#endif

/*
 * 16/09/2011
     Error handling instead of FATAL's
 * 25/07/2010
     AdditionalDLLPath
 * 08/12/2004
     It's here
*/
