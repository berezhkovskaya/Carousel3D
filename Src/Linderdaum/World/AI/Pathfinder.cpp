/**
 * \file Pathfinder.cpp
 * \brief Pathfinder
 * \version 0.5.74
 * \date 03/08/2007
 * \author Sergey Kosarevsky, 2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Pathfinder.h"

struct sIntVec2
{
	sIntVec2( int X, int Y ): dX( X ), dY( Y ) {};
	int dX;
	int dY;
};

void clPath::Clear()
{
	FWaypoints.clear();
}

void clPath::AddWaypoint( s2DWaypoint Waypoint )
{
	FWaypoints.push_back( Waypoint );
}

int clPath::GetTotalWaypoints() const
{
	return static_cast<int>( FWaypoints.size() );
}

s2DWaypoint clPath::GetWaypoint( int Index ) const
{
	return FWaypoints[Index];
}

/*
const int MAX_HEADING = 8;

sIntVec2 sHeading[MAX_HEADING] = {
                                  sIntVec2(-1,-1),
                                  sIntVec2( 0,-1),
                                  sIntVec2(+1,-1),
                                  sIntVec2(-1, 0),
                                  sIntVec2(+1, 0),
                                  sIntVec2(-1,+1),
                                  sIntVec2( 0,+1),
                                  sIntVec2(+1,+1),
                                 };
*/
const int MAX_HEADING = 4;

const int sHeadingX[MAX_HEADING] = {  0, -1, +1,  0 };
const int sHeadingY[MAX_HEADING] = { -1,  0,  0, +1 };


void clPathfinder::MarkPoint( int X, int Y, int Step )
{
	if ( X < 0 || Y < 0 || X >= FSizeX || Y >= FSizeY )
	{
		return;
	}

	if ( FAccessMap.GetPoint( X, Y ) <= Step )
	{
		return;
	}

	FAccessMap.SetPoint( X, Y, Step );

	if ( X == FDstX && Y == FDstY )
	{
		return;
	}

	for ( int i = 0; i != MAX_HEADING; ++i )
	{
		MarkPoint( X + sHeadingX[i],
		           Y + sHeadingY[i],
		           Step + 1 );
	}
}

clPath* clPathfinder::FindPath2DGrid( int SizeX, int SizeY,
                                      int XSrc , int YSrc,
                                      int XDest, int YDest,
                                      cl2DPassabilityMap* Map )
{
	FPath.Clear();

	FSizeX = SizeX;
	FSizeY = SizeY;

	FDstX = XDest;
	FDstY = YDest;

	const int UNREACHABLE = 0xFFFFFF;
	const int UNPASSABLE  = -1;

	// run the wave
	FAccessMap.SetSize( SizeX, SizeY );
	FAccessMap.Fill( UNREACHABLE );

	for ( int i = 0; i != SizeX; ++i )
	{
		for ( int j = 0; j != SizeY; ++j )
		{
			if ( !Map->IsPassable( i, j ) )
			{
				FAccessMap.SetPoint( i, j, UNPASSABLE );
			}
		}
	}

	if ( FAccessMap.GetPoint( XDest, YDest ) == UNPASSABLE )
	{
		return &FPath;
	}

	MarkPoint( XSrc, YSrc, 0 );

	// create path
	int X = XDest;
	int Y = YDest;
	int Step = FAccessMap.GetPoint( XDest, YDest );

	if ( Step == UNREACHABLE  )
	{
		return &FPath;
	}

	for ( int i = Step; i >= 0; --i )
	{
		FPath.AddWaypoint( s2DWaypoint( X, Y ) );

		for ( int j = 0; j != MAX_HEADING; ++j )
		{
			int aX = X + sHeadingX[j];
			int aY = Y + sHeadingY[j];

			if ( aX < 0 || aY < 0 || aX >= FSizeX || aY >= FSizeY )
			{
				continue;
			}

			if ( FAccessMap.GetPoint( aX, aY ) == Step - 1 )
			{
				X = aX;
				Y = aY;

				Step--;

				break;
			}
		}
	}

	return &FPath;
}

/*
 * 03/08/2007
     It's here
*/
