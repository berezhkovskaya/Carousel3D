/**
 * \file HeightMapFacetter.h
 * \brief Heightmap facetter implementation
 * \version 0.5.89a
 * \date 17/05/2009
 * \author Sergey Kosarevsky, 2007-2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clHeightMapFacetter_
#define _clHeightMapFacetter_

#include "Platform.h"
#include "Scene/Heightmaps/LHeightMap.h"

class clVertexAttribs;

/// Does a tesselation of a heightmap using LHeightMap and attached iHeightDataProvider
class scriptfinal iHeightMapFacetter: public iObject
{
public:
	iHeightMapFacetter() {};
	//
	// iHeightMapFacetter
	//
	virtual clVertexAttribs*    FacetHeightMap( const LHeightMap& HeightMap ) = 0;
	virtual void                UpdateFacettedHeightMap( clVertexAttribs* VA, const LHeightMap& HeightMap ) = 0;
};

/// Implementation of iHeightMapFacetter
class scriptfinal clHeightMapFacetter: public iHeightMapFacetter
{
public:
	clHeightMapFacetter();
	virtual ~clHeightMapFacetter();
	//
	// iHeightMapFacetterr interface
	//
	virtual clVertexAttribs*    FacetHeightMap( const LHeightMap& HeightMap );
	virtual void                UpdateFacettedHeightMap( clVertexAttribs* VA, const LHeightMap& HeightMap );
private:
	int    VertexIndex( int X, int Y ) const;
	int    Tesselate( Luint* Indices, int MipLevel,
	                  int A, int B, int C, int D ) const;
private:
	LHeightMap    FHeightMap;
};

#endif

/*
 * 17/05/2009
     Merged with iHeightMapFacetter.h
 * 29/01/2007
     It's here
*/
