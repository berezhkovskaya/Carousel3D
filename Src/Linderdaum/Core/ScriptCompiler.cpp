/**
 * \file ScriptCompiler.cxx
 * \brief Interface to the script compiler
 * \version 0.5.82
 * \date 08/12/2007
 * \author Viktor Latypov, 2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "ScriptCompiler.h"

clScriptCompiler::clScriptCompiler()
{
	FIsInitialized = false;
}

clScriptCompiler::~clScriptCompiler()
{
}

void clScriptCompiler::ClearSettings()
{
}

void clScriptCompiler::Reinitialize()
{
}

bool clScriptCompiler::ProcessSourceDirectory( const LString& DirName, bool recurse )
{
	return false;
}

bool clScriptCompiler::ProcessSourceFile( const LString& FileName )
{
	return false;
}

void clScriptCompiler::AddExportDirectory( const LString& DirName )
{
}

void clScriptCompiler::AddLibDirectory( const LString& DirName )
{
}

void clScriptCompiler::SetOutputDirectory( const LString& OutDirName )
{
}

LString clScriptCompiler::GetOutputDirectory()
{
	return "";
}

void clScriptCompiler::RecompileClass( const LString& AClassName )
{
}

void clScriptCompiler::RecompileClasses()
{
}

void clScriptCompiler::RecompileMethod( const LString& MethodName )
{
}

void clScriptCompiler::RecompileMethods()
{
}

LString clScriptCompiler::CompileStatement( const LString& stmt )
{
	return "";
}

/*
 * 25/09/2011
     * Reimplementation pending
*/
