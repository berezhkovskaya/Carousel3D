/**
 * \file Terrain.h
 * \brief Terrain
 * \version 0.5.89a
 * \date 10/04/2009
 * \author Sergey Kosarevsky, 2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clTerrain_
#define _clTerrain_

#include "Resources/iResource.h"
#include "Scene/Heightmaps/LHeightMap.h"

class iVertexArray;
class iShaderProgram;

class scriptfinal netexportable clTerrain: public iObject
{
public:
	clTerrain()://FOptimizer(NULL),
		//FSceneNode(NULL),
		FBoundingBoxValid( false ) {};
	virtual ~clTerrain();
	//
	// clTerrain
	//
	virtual void               LoadHeightMap( const LString& FileName,
	                                          int SizeX, int SizeY, int BitsPerPixel,
	                                          const LAABoundingBox& InGameDimensions );
	virtual LAABoundingBox     GetBoundingBox();
	virtual void               RenderDirect( iShaderProgram* SPOverride );
protected:
	iVertexArray*              FLandscapeVertices;
	bool                       FBoundingBoxValid;
	LAABoundingBox             FBoundingBox;
	LHeightMap                 FHeightMap;
};

#endif

/*
 * 10/04/2009
     It's here
*/
