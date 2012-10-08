#pragma once

#include "Math/LAABB.h"
#include "Scene/Heightmaps/LHeightMap.h"

#include "UnitTests/Tests.h"

class scriptfinal clTestHeightDataProvider: public iHeightDataProvider
{
public:
	clTestHeightDataProvider() {};
	//
	// iHeightDataProvider interface
	//
	virtual int    GetSizeX() const
	{
		return 1;
	}
	virtual int    GetSizeY() const
	{
		return 1;
	}
	virtual int    GetHeight( int X, int Y ) const
	{
		if ( X == 0 && Y == 0 )
		{
			return 0;
		}

		if ( X == 0 && Y == 1 )
		{
			return 1;
		}

		if ( X == 1 && Y == 0 )
		{
			return 1;
		}

		if ( X == 1 && Y == 1 )
		{
			return 1;
		}

		return 0;
	}
	virtual void   SetHeight( int X, int Y, int Height )
	{
	}
	virtual int    GetMaximalHeight( int X1, int Y1, int X2, int Y2 ) const
	{
		return 1;
	}
	virtual int    GetMinimalHeight( int X1, int Y1, int X2, int Y2 ) const
	{
		return 0;
	}
};


/*
 * 23/11/2005
     It's here
*/
