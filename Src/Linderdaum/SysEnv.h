/**
 * \file SysEnv.h
 * \brief Engine low-level environment
 *
 * Logger, Linker and File system
 *
 * \version 0.5.91
 * \date 02/07/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LSysEnvironment_
#define _LSysEnvironment_

#include "Platform.h"
#include "Core/iObject.h"

#include <list>

class clLinker;
class clLogger;
class clFileSystem;
class clLibrary;

/// Engine low-level environment (logger, linker and file system)
class scriptfinal netexportable sSysEnv : public iObject
{
public:
	sSysEnv();
	virtual ~sSysEnv();

	void InitializeLowLevel();
	void DeinitializeLowLevel();

	NET_EXPORTABLE()

	//
	// sSysEnv
	//

	/// Get seconds count from engine's start
	scriptmethod double       GetSeconds() const;

	/// Find object in global list
	scriptmethod iObject*     FindObject( const LString& ObjID );

	/// Internal utility to load dynamic libraries
	scriptmethod clLibrary*   LoadDynamicLibrary( const LString& LibName ) const;

private:
	/// Register standart runtime environment
	void RegisterEnvironmentSymbols();

	/// wrapper for win32 QueryPerformaceCounter
	double    GetCycles() const;

	/// Inverse number of CyclesPerSecond
	double    FRecipCyclesPerSecond;

public:
	typedef std::list<iObject*>      clObjectsGraph;
public:
	/// global list of allocated objects
	clObjectsGraph                   ObjectsGraph;


#pragma region Properties
	/* Property(Description="Global class factory", Category="Core", Name=Linker,     Type=clLinker,     FieldName=Linker)     */
	/* Property(Description="Logger",               Category="Core", Name=Logger,     Type=clLogger,     FieldName=Logger)     */
	/* Property(Description="Virtual file system",  Category="Core", Name=FileSystem, Type=clFileSystem, FieldName=FileSystem) */
#pragma endregion

public:
	/// metaclasses repository
	nativefield    clLinker*            Linker;

	/// logs messages to console and file
	nativefield    clLogger*            Logger;

	/// system independent IO
	nativefield    clFileSystem*    FileSystem;
};

#endif

/*
 * 02/07/2010
     Extracted from sEnvironment
 * 01/07/2010
     It's here
*/
