/**
 * \file 2DUniformGrid.h
 * \brief 2D uniform grid structure
 * \version 0.5.74
 * \date 24/07/2007
 * \author Sergey Kosarevsky, 2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _cl2DUniformGrid_
#define _cl2DUniformGrid_

#include "Platform.h"
#include "Math/LVector.h"
#include "Utils/LArray.h"

/*
 RECOMMENDATION: consider GranX and GranY equal twice the diameter ( 4 * Radius )
                 of the smallest object to get near optimal performance
*/

template <class T> class cl2DUniformGrid
{
public:
	cl2DUniformGrid( float SizeX,
	                 float SizeY,
	                 float GranX,
	                 float GranY ): FGranX( GranX ),
		FGranY( GranY )
	{
		FCellsX = ( int )ceil( SizeX / FGranX );
		FCellsY = ( int )ceil( SizeY / FGranY );

		FGridCells.resize( FCellsX * FCellsY );
	}
	~cl2DUniformGrid() {};
	//
	// cl2DUniformGrid
	//
	void    AddToGrid( const LVector2& Pos,
	                   T* Object,
	                   float Radius );
	void    RemoveFromGrid( const LVector2& Pos,
	                        T* Object,
	                        float Radius );
	void    CreateClosestObjectsList( const LVector2& Pos,
	                                  float Radius );
	T*      GetClosestObject( int Index ) const
	{
		return FClosestObjects[Index];
	}
	int     GetClosestObjectsCount() const
	{
		return static_cast<int>( FClosestObjects.size() );
	}
private:
	struct sGridCell
	{
		sGridCell(): FObjects() {};
		//
		// sGridCell
		//
		LArray<T*>    FObjects;
	};
private:
	LArray<T*>           FClosestObjects;
	LArray<sGridCell>    FGridCells;
	int                  FCellsX;
	int                  FCellsY;
	float                FGranX;
	float                FGranY;
	// cache
	int                  FPrevCenterCellIndex;
	int                  FPrevDx;
	int                  FPrevDy;
};

#define CALCULATE_METRICS() int Dx = (int)ceil( Radius / FGranX );  \
                            int Dy = (int)ceil( Radius / FGranY );  \
                                                                    \
                            int CellX = int( Pos.X / FGranX );      \
                            int CellY = int( Pos.Y / FGranY );      \
                                                                    \
                            int X1 = CellX - Dx;                    \
                            int Y1 = CellY - Dy;                    \
                                                                    \
                            int X2 = CellX + Dx + 1;                \
                            int Y2 = CellY + Dy + 1;                \
                                                                    \
                            if ( X1 < 0 ) X1 = 0;                   \
                            if ( Y1 < 0 ) Y1 = 0;                   \
                            if ( X2 > FCellsX ) X2 = FCellsX;       \
                            if ( Y2 > FCellsY ) Y2 = FCellsY;

template <class T> void cl2DUniformGrid<T>::AddToGrid( const LVector2& Pos,
                                                       T* Object,
                                                       float Radius )
{
	CALCULATE_METRICS();

	for ( int j = Y1; j != Y2; ++j )
	{
		for ( int i = X1; i != X2; ++i )
		{
			FGridCells[ j * FCellsX + i ].FObjects.push_back( Object );
		}
	}
}

template <class T> void cl2DUniformGrid<T>::RemoveFromGrid( const LVector2& Pos,
                                                            T* Object,
                                                            float Radius )
{
	CALCULATE_METRICS();

	for ( int j = Y1; j != Y2; ++j )
	{
		for ( int i = X1; i != X2; ++i )
		{
			int Index = j * FCellsX + i;

			LArray<T*>* Objects = &FGridCells[ Index ].FObjects;

			for ( size_t k = 0; k != Objects->size(); ++k )
			{
				if ( ( *Objects )[ k ] == Object )
				{
					( *Objects )[ k ] = ( *Objects )[ Objects->size() - 1 ];

					Objects->pop_back();

					break;
				}
			}
		}
	}
}

template <class T> void cl2DUniformGrid<T>::CreateClosestObjectsList( const LVector2& Pos,
                                                                      float Radius )
{
	CALCULATE_METRICS();

	// if we have already constructed the list of closets objects with these
	// conditions - do nothing
	if ( FPrevCenterCellIndex == CellY * FCellsX + CellX &&
	     FPrevDx == Dx &&
	     FPrevDy == Dy )
	{
		return;
	}

	FPrevDx              = Dx;
	FPrevDy              = Dy;
	FPrevCenterCellIndex = CellY * FCellsX + CellX;

	FClosestObjects.clear();

	for ( int j = Y1; j != Y2; ++j )
	{
		for ( int i = X1; i != X2; ++i )
		{
			int Index = j * FCellsX + i;

			LArray<T*>* Objects = &FGridCells[ Index ].FObjects;

			for ( size_t k = 0; k != Objects->size(); ++k )
			{
				FClosestObjects.push_back( ( *Objects )[ k ] );
			}
		}
	}
}

#endif

/*
 * 24/07/2007
     It's here
*/
