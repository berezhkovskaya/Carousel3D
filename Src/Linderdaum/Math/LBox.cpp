#include "Math/LBox.h"
#include "Math/LSphere.h"
#include "Math/LPoly.h"

LSphere LBox::CircumscribedSphere() const
{
	return LSphere( FCenter, ( FExtent[0] * FAxis[0] + FExtent[1] * FAxis[1] + FExtent[2] * FAxis[2] ).Length() );
}

LSphere LBox::InscribedSphere() const
{
	return LSphere( FCenter, FExtent[FExtent.GetMinimumComponentIndex()] );
}

void LBox::Facet( Linderdaum::Math::LPolyList* Out ) const
{
	// Corner points
	LVector3 Pts[] =
	{
		// upper
		( FExtent[0] * FAxis[0] + FExtent[1] * FAxis[1] + FExtent[2] * FAxis[2] ), // 0
		( -FExtent[0] * FAxis[0] + FExtent[1] * FAxis[1] + FExtent[2] * FAxis[2] ), // 1
		( -FExtent[0] * FAxis[0] - FExtent[1] * FAxis[1] + FExtent[2] * FAxis[2] ), // 2
		( FExtent[0] * FAxis[0] - FExtent[1] * FAxis[1] + FExtent[2] * FAxis[2] ), // 3
		// lower
		( FExtent[0] * FAxis[0] + FExtent[1] * FAxis[1] - FExtent[2] * FAxis[2] ), // 4
		( -FExtent[0] * FAxis[0] + FExtent[1] * FAxis[1] - FExtent[2] * FAxis[2] ), // 5
		( -FExtent[0] * FAxis[0] - FExtent[1] * FAxis[1] - FExtent[2] * FAxis[2] ), // 6
		( FExtent[0] * FAxis[0] - FExtent[1] * FAxis[1] - FExtent[2] * FAxis[2] ) // 7
	};

	Out->FPolys.resize( 6 );

	Out->FPolys[0].FTexV1 = Out->FPolys[1].FTexV1 = FAxis[0];
	Out->FPolys[0].FTexV1 = Out->FPolys[1].FTexV2 = FAxis[1];

	// Top
	Out->FPolys[0].MakeQuad( Pts[0], Pts[1], Pts[2], Pts[3] );
	// Bottom
	Out->FPolys[1].MakeQuad( Pts[4], Pts[5], Pts[6], Pts[7] );

	// Sides
	Out->FPolys[2].MakeQuad( Pts[0], Pts[1], Pts[5], Pts[4] );
	Out->FPolys[3].MakeQuad( Pts[1], Pts[2], Pts[6], Pts[5] );
	Out->FPolys[4].MakeQuad( Pts[3], Pts[2], Pts[6], Pts[7] );
	Out->FPolys[5].MakeQuad( Pts[0], Pts[3], Pts[7], Pts[4] );
}

/*
 * 08/01/2011
     It's here
*/
