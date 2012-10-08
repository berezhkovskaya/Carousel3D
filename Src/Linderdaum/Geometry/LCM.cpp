/**
 * \file LCM.cpp
 * \brief Linderdaum Cached Mesh loader
 * \version 0.6.02
 * \date 04/09/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "LCM.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/VFS/FileSystem.h"
#include "Core/Logger.h"

clLCMLoader::clLCMLoader()
{
}

bool clLCMLoader::LoadMesh( iIStream* IStream, clMesh* Resource )
{
	guard( "%s", IStream->GetFileName().c_str() );

	Resource->LoadFromStream( IStream );

	return true;

	unguard();
}

/*
 * 04/09/2011
     It's here
*/
