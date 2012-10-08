/**
 * \file LCM.h
 * \brief Linderdaum Cached Mesh loader
 * \version 0.6.02
 * \date 04/09/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LCMLoader_
#define _LCMLoader_

#include "Geometry/Mesh.h"

/// Loader of Linderdaum Cached Meshes
class scriptfinal clLCMLoader: public iMeshLoader
{
public:
	clLCMLoader();
	//
	// iMeshLoader interface
	//
	virtual bool         LoadMesh( iIStream* IStream, clMesh* Resource );
};


#endif

/*
 * 04/09/2011
     It's here
*/
