/**
 * \file CoreSerialization_Defs.h
 * \brief Macro definitions to compile autogenerated serialization
 * \version 0.5.99c
 * \date 20/04/2011
 * \author Sergey Kosarevsky, 2005-2011
 * \author Viktor Latypov, 2007-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "VFS/FileSystem.h"
#include "Core/Logger.h"

// macro to convert local file names to global
#define FILE_NAME_CONV(LocalFileName) \
   LStringBuffer( Env->FileSystem->FindFile( LStr::GetToken(LocalFileName, 1), GetFileName()) )

/*
 * 07/10/2010
     Log section added
*/
