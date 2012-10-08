/**
 * \file LAABB.cpp
 * \brief Axis-aligned bounding box
 * \version 0.5.91
 * \date 11/09/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Math/LAABB.h"
#include "Math/LSphere.h"
#include "Math/LPoly.h"
#include "Math/LBox.h"

#include "Math/LRandom.h"

using namespace Linderdaum;

void LAABoundingBox::Facet( Linderdaum::Math::LPolyList* Out ) const
{
	ToBox().Facet( Out );
}

LSphere LAABoundingBox::CircumscribedSphere() const
{
	return LSphere ( ( FMax + FMin ) * 0.5f, ( FMax - FMin ).Length() );
}

LSphere LAABoundingBox::InscribedSphere() const
{
	LVector3 D = FMax - FMin;
	return LSphere ( ( FMax + FMin ) * 0.5f, D[D.GetMinimumComponentIndex()] );
}

LBox LAABoundingBox::ToBox() const
{
	LBox Box;

	Box.FCenter = ( FMin + FMax ) * 0.5f;
	Box.FAxis = LMatrix3::Identity();
	Box.FExtent = ( FMax - FMin ) * 0.5f;

	return Box;
}

void LAABoundingBox::FromPoints( int NumPoints, const LVector3* Points )
{
	FMin = LVector3( 0, 0, 0 );
	FMax = LVector3( 0, 0, 0 );

	const LVector3* Pt = Points;

	for ( int i = 0 ; i < NumPoints ; i++ )
	{
		this->CombinePoint( *Pt++ );
	}
}

LVector3 LAABoundingBox::RandomVector() const
{
	return Linderdaum::Math::RandomVector3InRange( FMin, FMax );
}

void LAABoundingBox::Grow( float DeltaSize )
{
	FMin -= LVector3( DeltaSize );
	FMax += LVector3( DeltaSize );
}

LVector3 LAABoundingBox::ClampVectorToBox_Periodic( const LVector3& InVec ) const
{
	LVector3 Res = InVec;
	LVector3 Delta = FMax - FMin;

	while ( Res.X < FMin.X )
	{
		Res.X += Delta.X;
	}

	while ( Res.Y < FMin.Y )
	{
		Res.Y += Delta.Y;
	}

	while ( Res.Z < FMin.Z )
	{
		Res.Z += Delta.Z;
	}

	while ( Res.X > FMax.X )
	{
		Res.X -= Delta.X;
	}

	while ( Res.Y > FMax.Y )
	{
		Res.Y -= Delta.Y;
	}

	while ( Res.Z > FMax.Z )
	{
		Res.Z -= Delta.Z;
	}

	return Res;
}

LVector3 LAABoundingBox::ClampVectorToBox( const LVector3& InVec ) const
{
	LVector3 Res = InVec;
	LVector3 Delta = FMax - FMin;

	if ( Res.X < FMin.X ) { Res.X = FMin.X; }

	if ( Res.Y < FMin.Y ) { Res.Y = FMin.Y; }

	if ( Res.Z < FMin.Z ) { Res.Z = FMin.Z; }

	if ( Res.X > FMax.X ) { Res.X = FMax.X; }

	if ( Res.Y > FMax.Y ) { Res.Y = FMax.Y; }

	if ( Res.Z > FMax.Z ) { Res.Z = FMax.Z; }

	return Res;
}

void LAABoundingBox::Combine( const LAABoundingBox& Box )
{
	FMin.X = Math::LMin( FMin.X, Box.FMin.X );
	FMin.Y = Math::LMin( FMin.Y, Box.FMin.Y );
	FMin.Z = Math::LMin( FMin.Z, Box.FMin.Z );

	FMin.X = Math::LMin( FMin.X, Box.FMax.X );
	FMin.Y = Math::LMin( FMin.Y, Box.FMax.Y );
	FMin.Z = Math::LMin( FMin.Z, Box.FMax.Z );

	FMax.X = Math::LMax( FMax.X, Box.FMax.X );
	FMax.Y = Math::LMax( FMax.Y, Box.FMax.Y );
	FMax.Z = Math::LMax( FMax.Z, Box.FMax.Z );

	FMax.X = Math::LMax( FMax.X, Box.FMin.X );
	FMax.Y = Math::LMax( FMax.Y, Box.FMin.Y );
	FMax.Z = Math::LMax( FMax.Z, Box.FMin.Z );
}

void LAABoundingBox::CombinePoint( const LVector3& Point )
{
	FMin.X = Math::LMin( FMin.X, Point.X );
	FMin.Y = Math::LMin( FMin.Y, Point.Y );
	FMin.Z = Math::LMin( FMin.Z, Point.Z );

	FMax.X = Math::LMax( FMax.X, Point.X );
	FMax.Y = Math::LMax( FMax.Y, Point.Y );
	FMax.Z = Math::LMax( FMax.Z, Point.Z );
}

bool LAABoundingBox::IntersectRay( const LVector3& Origin,
                                   const LVector3& Ray,
                                   LVector3& Point1,
                                   LVector3& Point2 ) const
{
	float MinX, MaxX, MinY, MaxY, MinZ, MaxZ;
	float Flag = 1.0;

	// check X
	if ( fabs( Ray.X ) < Math::EPSILON )
	{
		MinX = Math::INFINITY;
		MaxX = Math::INFINITY;
	}
	else if ( Ray.X > 0 )
	{
		MinX = ( FMin.X - Origin.X ) / Ray.X;
		MaxX = ( FMax.X - Origin.X ) / Ray.X;
	}
	else
	{
		MinX = ( FMax.X - Origin.X ) / Ray.X;
		MaxX = ( FMin.X - Origin.X ) / Ray.X;
	}

	// check Y
	if ( fabs( Ray.Y ) < ::Linderdaum::Math::EPSILON )
	{
		MinY = ::Linderdaum::Math::INFINITY;
		MaxY = ::Linderdaum::Math::INFINITY;
	}
	else if ( Ray.Y > 0 )
	{
		MinY = ( FMin.Y - Origin.Y ) / Ray.Y;
		MaxY = ( FMax.Y - Origin.Y ) / Ray.Y;
	}
	else
	{
		MinY = ( FMax.Y - Origin.Y ) / Ray.Y;
		MaxY = ( FMin.Y - Origin.Y ) / Ray.Y;
	}

	if ( ( MinX > MaxY ) ||
	     ( MinY > MaxX ) )
	{
		Flag = -1.0;
	}

	if ( MinY > MinX )
	{
		MinX = MinY;
	}

	if ( MaxY < MaxX )
	{
		MaxX = MaxY;
	}

	// check Z
	if ( fabs( Ray.Z ) < ::Linderdaum::Math::EPSILON )
	{
		MinZ = ::Linderdaum::Math::INFINITY;
		MaxZ = ::Linderdaum::Math::INFINITY;
	}
	else if ( Ray.Z > 0 )
	{
		MinZ = ( FMin.Z - Origin.Z ) / Ray.Z;
		MaxZ = ( FMax.Z - Origin.Z ) / Ray.Z;
	}
	else
	{
		MinZ = ( FMax.Z - Origin.Z ) / Ray.Z;
		MaxZ = ( FMin.Z - Origin.Z ) / Ray.Z;
	}

	if ( ( MinX > MaxZ ) ||
	     ( MinZ > MaxX ) )
	{
		Flag = -1.0;
	}

	if ( MinZ > MinX )
	{
		MinX = MinZ;
	}

	if ( MaxZ < MaxX )
	{
		MaxX = MaxZ;
	}

	LVector3 Direction = Ray.GetNormalized();

	Point1 = Origin + Direction * MinX;
	Point2 = Origin + Direction * MaxX;

	return ( Flag > 0 );
}

bool LAABoundingBox::Intersect( const LAABoundingBox& Box ) const
{
	if ( ( FMax.X < Box.FMin.X ) || ( FMin.X > Box.FMax.X ) ||
	     ( FMax.Y < Box.FMin.Y ) || ( FMin.Y > Box.FMax.Y ) ||
	     ( FMax.Z < Box.FMin.Z ) || ( FMin.Z > Box.FMax.Z ) )
	{
		return false;
	}

	return true;
}

LClassify LAABoundingBox::IntersectPlane( const LPlane& Plane ) const
{
	int Side[ IN_PLANE + 1];

	for ( int i = 0; i != GetPointsNum(); i++ )
	{
		LVector3 Pt = GetCornerPoint( i );

		Side[ Plane.ClassifyPoint( Pt ) ]++;
	}

	if ( Side[ IN_FRONT ] && Side[ IN_BACK ] )
	{
		return IN_BOTH;
	}

	if ( Side[ IN_BACK  ] )
	{
		return IN_BACK;
	}

	if ( Side[ IN_FRONT ] )
	{
		return IN_FRONT;
	}

	return IN_BOTH;
}

void LAABoundingBox::Transform( const LMatrix4& Transform )
{
	LAABoundingBox NewBox;

	for ( int i = 0; i != 8; i++ )
	{
		NewBox.CombinePoint( Transform * GetCornerPoint( i ) );
	}

	*this = NewBox;
//   FMin = Transform * FMin;
//   FMax = Transform * FMax;
}

LVector3 LAABoundingBox::GetCornerPoint( int N ) const
{
	switch ( N )
	{
		case 0:
			return FMin;
		case 1:
			return LVector3( FMin.X, FMax.Y, FMin.Z );
		case 2:
			return LVector3( FMax.X, FMax.Y, FMin.Z );
		case 3:
			return LVector3( FMax.X, FMin.Y, FMin.Z );
		case 4:
			return FMax;
		case 5:
			return LVector3( FMin.X, FMax.Y, FMax.Z );
		case 6:
			return LVector3( FMin.X, FMin.Y, FMax.Z );
		case 7:
			return LVector3( FMax.X, FMin.Y, FMax.Z );
		default:
			return LVector3( 0, 0, 0 );
	}
}

float LAABoundingBox::MinDistanceToCorner( const LVector3& Point ) const
{
	float MinD = ( Point - FMin ).SqrLength();

	for ( int i = 1; i != GetPointsNum(); i++ )
	{
		float D = ( Point - GetCornerPoint( i ) ).SqrLength();

		if ( D < MinD )
		{
			MinD = D;
		}
	}

	return sqrt( MinD );
}

float LAABoundingBox::MaxDistanceToCorner( const LVector3& Point ) const
{
	float MaxD = ( Point - FMin ).SqrLength();

	for ( int i = 1; i != GetPointsNum(); i++ )
	{
		float D = ( Point - GetCornerPoint( i ) ).SqrLength();

		if ( D > MaxD )
		{
			MaxD = D;
		}
	}

	return sqrt( MaxD );
}

bool LAABoundingBox::IsPointInside( const LVector3& Point ) const
{
	bool PtMin = ( Point.X > FMin.X ) && ( Point.Y > FMin.Y ) && ( Point.Z > FMin.Z );
	bool PtMax = ( Point.X < FMax.X ) && ( Point.Y < FMax.Y ) && ( Point.Z < FMax.Z );

	return PtMin && PtMax;
}

/*
 * 29/05/2010
     IsPointInside()
 * 10/03/2010
     MinDistanceToCorner()
     MaxDistanceToCorner()
 * 12/03/2007
     Intersect()
     GetCornerPoint()
     Transform()
 * 13/02/2007
     IntersectRay()
 * 07/03/2005
     It's here
*/
