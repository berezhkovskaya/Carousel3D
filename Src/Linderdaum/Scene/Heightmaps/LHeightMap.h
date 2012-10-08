/**
 * \file LHeightMap.h
 * \brief Heighmap utils
 * \version 0.5.74
 * \date 13/04/2007
 * \author Sergey Kosarevsky, 2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LHeightMap_
#define _LHeightMap_

#include "Core/iObject.h"
#include "Math/LAABB.h"
#include "Math/LVector.h"

/**
   Data source for heightmap data. Represents heightmap as a 2D grid with 'int'
   elements. Map is indexted in range 0,0 - SizeX, SizeY. 8, 16 and 32 bit
   data sources are supported.
**/
class scriptfinal iHeightDataProvider: public iObject
{
public:
	iHeightDataProvider() {};
	//
	// iHeightDataProvider
	//
	virtual int    GetSizeX() const
	{
		return 0;
	};
	virtual int    GetSizeY() const
	{
		return 0;
	};
	virtual int    GetHeight( int X, int Y ) const
	{
		return 0;
	};
	virtual void   SetHeight( int X, int Y, int Height ) {};
	virtual int    GetMaximalHeight( int X1, int Y1, int X2, int Y2 ) const
	{
		return 0;
	};
	virtual int    GetMinimalHeight( int X1, int Y1, int X2, int Y2 ) const
	{
		return 0;
	};
};

/// Generic 2D convolution kernel for heightmap
class iKernel2D: public iObject
{
public:
	iKernel2D() {};
	//
	// LKernel2D
	//
	virtual float    GetValue( float OfsX, float OfsY ) const
	{
		return 1;
	};
};

/**
   Universal representation of heightmap. Provides high level interface
   (height interpolation, normals calculation) to access heightmap
   from a game logic. Maps world coordinates to integer heightmap coordinates
   and back.

   Three coordinate systems are employed:

   - World
        Game coordinates, containing game objects inside the game world
   - Local
        Integer local coordinates of this heightmap patch.
        Are within range from 0,0 to (FDataX2-FDataX1, FDataY2-FDataY1)
   - Data
        Integer coordinates relative to iHeightDataProvider.
        Are within range from 0,0 to (SizeX, SizeY)

   All generic methods accept world coordinates and return heights and normals
   in world coordinates.
**/
class LHeightMap
{
public:
	LHeightMap();
	//
	// LHeightMap
	//
	void                       SetHeightDataProvider( const clPtr<iHeightDataProvider>& Provider );
	clPtr<iHeightDataProvider> GetHeightDataProvider() const
	{
		return FHeightDataProvider;
	};
	/**
	   This method sets the location and size of this heightmap patch in
	   a game world
	**/
	void                       SetWorldAABB( LAABoundingBox Box );
	void                       SetDataBoundry( int X1, int Y1, int X2, int Y2 );
	void                       SetGranularity( const LVector3& Granularity );
	LVector3                   GetGranularity() const;
	// requests
	/**
	   This method defines a window in 2D heightmap of currently
	   attached iHeightDataProvider to read from
	**/
	void                       GetDataBoundry( int& X1, int& Y1, int& X2, int& Y2 ) const;
	int                        GetDataSizeX() const;
	int                        GetDataSizeY() const;
	LAABoundingBox             GetBoundingBox() const;
	float                      GetHeight( float X, float Y ) const;
	LVector3                   GetNormal( float X, float Y ) const;
	// terraforming
	void                       Level( float X, float Y,
	                                  float DesiredHeight,
	                                  float BrushRadius );
	void                       Deform( float X, float Y,
	                                   float DeltaHeight,
	                                   float BrushSize,
	                                   iKernel2D* Kernel2D );
	// coordsys transforms
	void                       WorldToLocal( float WorldX, float WorldY, float& X, float& Y ) const;
	void                       WorldToData( float WorldX, float WorldY, int& I, int& J, float& FracI, float& FracJ ) const;
	void                       LocalToData( float LocalX, float LocalY, int& I, int& J, float& FracI, float& FracJ ) const;
	void                       LocalToWorld( float LocalX, float LocalY, float& X, float& Y ) const;
	LVector3                   DataToWorld( int I, int J ) const;
	bool                       IntersectRay( const LVector3& Origin,
	                                         const LVector3& Ray,
	                                         LVector3& Intersection ) const;
private:
	void                       UpdateBoundingBox();
	LVector3                   GetDataNormal( int I, int J ) const;
private:
	clPtr<iHeightDataProvider>    FHeightDataProvider;
	int                           FDataX1;
	int                           FDataY1;
	int                           FDataX2;
	int                           FDataY2;
	LVector3                      FOrigin;
	LAABoundingBox                FBoundingBox;
	LVector3                      FScales;
	LVector3                      FGranularity;
};

#endif

/*
 * 13/04/2007
     Level()
     Deform()
 * 24/01/2007
     It's here
*/
