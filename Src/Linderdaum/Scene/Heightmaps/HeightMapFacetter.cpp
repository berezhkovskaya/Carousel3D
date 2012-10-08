/**
 * \file HeightMapFacetter.cpp
 * \brief Heightmap facetter implementation
 * \version 0.5.71
 * \date 29/01/2007
 * \author Sergey Kosarevsky, 2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "Engine.h"
#include "Geometry/VertexAttribs.h"

#include "HeightMapFacetter.h"

clHeightMapFacetter::clHeightMapFacetter()
{
}

clHeightMapFacetter::~clHeightMapFacetter()
{
}

void clHeightMapFacetter::UpdateFacettedHeightMap( clVertexAttribs* VA, const LHeightMap& HeightMap )
{
	int DimX = HeightMap.GetDataSizeX();
	int DimY = HeightMap.GetDataSizeY();

	int VIndex = 0;

	LVector3* Vertices = VA->FVertices.GetPtr();

	for ( int j = 0; j != DimY; ++j )
	{
		for ( int i = 0; i != DimX; ++i )
		{
			Vertices[ VIndex ] = HeightMap.DataToWorld( i, j );

			VIndex++;
		}
	}
}

clVertexAttribs* clHeightMapFacetter::FacetHeightMap( const LHeightMap& HeightMap )
{
	FHeightMap = HeightMap;

	int DimX = HeightMap.GetDataSizeX();
	int DimY = HeightMap.GetDataSizeY();

	// generate raw vertices
	clVertexAttribs* Vertices = clVertexAttribs::Create( DimX * DimY, L_TEXCOORDS_BIT );

	Vertices->FPrimitiveType = L_PT_TRIANGLE_STRIP;

	int VIndex = 0;

	for ( int j = 0; j != DimY; ++j )
	{
		for ( int i = 0; i != DimX; ++i )
		{
			Vertices->FVertices.GetPtr()[ VIndex ] = HeightMap.DataToWorld( i, j );

			float dX = static_cast<float>( i ) / HeightMap.GetDataSizeX();
			float dY = static_cast<float>( j ) / HeightMap.GetDataSizeY();

			Vertices->FTexCoords.GetPtr()[ VIndex ] = LVector4( dX, dY, 128.0f * dX, 128.0f * dY );

			VIndex++;
		}
	}

	// generate indices

	FIXME( "evaluate this accuratly" )
	int EstVertexCount = ( 2 * DimX + 2 ) * ( DimY - 1 );

	Luint* IndicesArray = new Luint[ EstVertexCount ];

	int VertexCount = Tesselate( IndicesArray, 1, 0, 0, 0, 0 );

	Vertices->SetIndices32( VertexCount, IndicesArray );

	return Vertices;
}

int clHeightMapFacetter::VertexIndex( int X, int Y ) const
{
	return X + Y * FHeightMap.GetDataSizeY();
}

int AdjustToStep( int X, int Step )
{
	return X - ( X % Step );
}

int clHeightMapFacetter::Tesselate( Luint* Indices, int MipLevel,
                                    int A, int B, int C, int D ) const
{
	guard();

	using namespace Linderdaum::Math;

	int MipStep = 1;

	int J = 0;

	int Offset = -2;

	while ( J < FHeightMap.GetDataSizeY() - 1 )
	{
		int I = 0;

		while ( I <= FHeightMap.GetDataSizeX() - 1 )
		{
			Offset += 2;

			int V1 = VertexIndex( I, J );
			int V2 = VertexIndex( I, J + MipStep );

			Indices[ Offset+0 ] = V1;
			Indices[ Offset+1 ] = V2;

			I += MipStep;
		}

		if ( J + MipStep >= FHeightMap.GetDataSizeY() )
		{
			break;
		}

		/*
		      FATAL( Offset+3 >= IndicesCount,
		             "Indices overrun: got "+LString(Offset+3)+" should be "+LString(IndicesCount) );
		*/
		// create degenerate triangle
		int V1 = Indices [ Offset+1 ];
		int V2 = VertexIndex( 0, AdjustToStep( J + MipStep, IntPow( 2, B ) * MipStep ) );

		Indices[ Offset+2 ] = V1;
		Indices[ Offset+3 ] = V2;

		Offset += 2;

		J += MipStep;
	}

	return Offset;

	unguard();
}


/*
 * 29/01/2007
     It's here
*/
