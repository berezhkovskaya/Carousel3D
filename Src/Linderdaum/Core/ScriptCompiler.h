/**
 * \file ScriptCompiler.h
 * \brief Interface to the script compiler
 * \version 0.5.89a
 * \date 30/04/2009
 * \author Viktor Latypov, 2007-2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __ScriptCompiler__h__included__
#define __ScriptCompiler__h__included__

#include "Core/iObject.h"

/**
   \brief Interface to external scripts compiler

   To be implemented using the new C++ compiler
**/
class scriptfinal clScriptCompiler : public iObject
{
public:
	clScriptCompiler();
	virtual ~clScriptCompiler();

	// iScriptCompiler interface
	virtual void ClearSettings();
	virtual void Reinitialize();
	virtual bool ProcessSourceDirectory( const LString& DirName, bool recurse );
	virtual bool ProcessSourceFile( const LString& FileName );

	virtual void AddExportDirectory( const LString& DirName );
	virtual void AddLibDirectory( const LString& DirName );

	// set class output directory
	virtual void SetOutputDirectory( const LString& OutDirName );
	virtual LString GetOutputDirectory();

	virtual void RecompileClass( const LString& AClassName );
	virtual void RecompileClasses();

	virtual void RecompileMethod( const LString& MethodName );
	virtual void RecompileMethods();

	virtual LString CompileStatement( const LString& stmt );

	virtual bool IsInitialized() const { return FIsInitialized; }

private:
	bool FIsInitialized;
};

#endif

/*
 * 30/04/2009
     * Merged with iScriptCompiler
 * 08/12/2007
     * Delayed initialization
 * 17/08/2007
     * Log section added
*/
