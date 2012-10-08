/*
   (Part of Linderdaum Engine)
   Version 0.5.74
   (08/04/2007)
   (C) Kosarevsky Sergey, 2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "LSphere.h"

#include "Math/LRandom.h"
#include "Math/LAABB.h"
#include "Math/LBox.h"

/// Calculate sphere from points
void LSphere::FromPoints( int NumPoints, const LVector3* Points )
{
	// find min and max
	LAABoundingBox Box;
	Box.FromPoints( NumPoints, Points );
	*this = Box.CircumscribedSphere();
}

/// Create polyhedral representation of a sphere
void LSphere::Facet( Linderdaum::Math::LPolyList* Out ) const
{
	// TODO
	if ( Out )
	{
	}
}


bool LSphere::IntersectLine( const LVector3& Point1,
                             const LVector3& Point2,
                             float* Intersection1,
                             float* Intersection2 ) const
{
	LVector3 Line = Point2 - Point1;

	float A = Line.SqrLength();

	if ( A < ::Linderdaum::Math::EPSILON )
	{
		return false;
	}

	float B = 2.0f * Line.Dot( Point1 - FOrigin );

	float C = FOrigin.SqrLength() + Point1.SqrLength() - 2.0f * ( FOrigin.Dot( Point1 ) ) - FRadius * FRadius;

	float Determinant = B * B - 4.0f * A * C;

	if ( Determinant < 0 )
	{
		return false;
	}

	float SqrtDeterminant = sqrt( Determinant );

	*Intersection1 = ( -B + SqrtDeterminant ) / ( 2.0f * A );
	*Intersection2 = ( -B - SqrtDeterminant ) / ( 2.0f * A );

	return true;
}

LVector3 RandomVectorOnUnitSphere()
{
	float Phi = Linderdaum::Math::RandomInRange( 0.0f, 2.0f * Linderdaum::Math::PI );
	float Psi = Linderdaum::Math::RandomInRange( -Linderdaum::Math::PI, Linderdaum::Math::PI );

	float sp = sin( Psi );
	float cp = cos( Psi );

	float cf = cos( Phi );
	float sf = sin( Phi );

	return vec3( cf * sp, sf * sp, cp );
}

/// Get random vector inside the sphere
LVector3 LSphere::RandomVectorInside() const
{
	return Linderdaum::Math::RandomInRange( 0.0f, FRadius ) * RandomVectorOnUnitSphere();
}

/// Get random vector on the sphere's surface
LVector3 LSphere::RandomVectorOnSphere() const
{
	return FRadius * RandomVectorOnUnitSphere();
}

LBox LSphere::InscribedBox() const
{
	LBox Box;

	Box.FCenter = FOrigin;
	Box.FExtent = LVector3( FRadius / static_cast<float>( sqrt( 3.0f ) ) );
	Box.FAxis = LMatrix3::Identity();

	return Box;
}

LBox LSphere::CircumscribedBox() const
{
	LBox Box;

	Box.FCenter = FOrigin;
	Box.FAxis = LMatrix3::Identity();
	Box.FExtent = LVector3( FRadius );

	return Box;
}

/*
 * 08/04/2007
     IntersectLine()
 * 24/01/2007
     It's here
*/
