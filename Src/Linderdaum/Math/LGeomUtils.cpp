#include "Platform.h"

#include "LGeomUtils.h"

#include "LTransform.h"

LVector3 Project( const LMatrix4& _proj, const LMatrix4& _view, const LVector3& _vec )
{
	LVector4 v4( _vec.X, _vec.Y, _vec.Z, 1.0f );

	LVector4 p = _proj * ( _view * v4 );

	LVector3 res( p.X, p.Y, p.Z );

	if ( fabs( p.W ) > Linderdaum::Math::EPSILON ) { res /= p.W; }

	return res;
}

LVector3 Unproject( const LMatrix4& _proj, const LMatrix4& _view, const LVector3& _vec )
{
	LVector4 v4( _vec.X, _vec.Y, _vec.Z, 1.0f );

	LVector4 p = _view.GetInversed() * ( _proj.GetInversed() * v4 );

	LVector3 res( p.X, p.Y, p.Z );

	if ( fabs( p.W ) > Linderdaum::Math::EPSILON ) { res /= p.W; }

	return res;
}

void MouseCoordsToWorldPointAndRay( const LMatrix4& _proj, const LMatrix4& _view, float mx, float my, LVector3& srcPoint, LVector3& dir )
{
	LVector3 screenPoint ( 2.0f * mx - 1.0f, -2.0f * my + 1.0f, 0.0f );
	LVector3 worldPoint = Unproject( _proj, _view, screenPoint );

	LMatrix4 CamRotation;
	DecomposeCameraTransformation( _view, srcPoint, CamRotation );

	dir = worldPoint - srcPoint;
}

// P - source point, A - direction
// C - sphere center, R - radius
bool IntersectRayToSphere( const LVector3& P, const LVector3& A, const LVector3& C, float R )
{
	float t = -( P.Dot( A ) - A.Dot( C ) ) / A.Dot( A );
	LVector3 v = P + A * t - C;

	return ( v.Dot( v ) <= R * R );
}

bool IntersectRayToPlane( const LVector3& P, const LVector3& A, const LVector3& N, float D, LVector3& isect )
{
	float denom = A.Dot( N );

	if ( fabs( denom ) < 0.0001f )
	{
		return false;
	}

	float t = -( D + P.Dot( N ) ) / denom;

	isect = P + A * t;

	return true;
}

void TriangularBarycentricCoords( const LVector3& V1, const LVector3& V2, const LVector3& V3, const LVector3& P, LVector3& Lambda )
{
	float detT = ( V1.X - V3.X ) * ( V2.Y - V3.Y ) - ( V2.X - V3.X ) * ( V1.Y - V3.Y );

	if ( fabs( detT ) < Linderdaum::Math::EPSILON )
	{
		Lambda.X = Lambda.Y = Lambda.Z = 0.0f;
		return;
	}

	Lambda.X = ( ( V2.Y - V3.Y ) * ( P.X - V3.X ) - ( V2.X - V3.X ) * ( P.Y - V3.Y ) ) / detT;
	Lambda.Y = ( ( V3.Y - V1.Y ) * ( P.X - V3.X ) + ( V1.X - V3.X ) * ( P.Y - V3.Y ) ) / detT;
	Lambda.Z = 1.0f - Lambda.X - Lambda.Y;
}

void ProjectToPlane( const LVector3& pt, const LVector3& Origin, const LVector3& Base1, const LVector3& Base2, LVector3& Proj )
{
	LVector3 d = ( pt - Origin );
	Proj = Origin + ( d * Base1 ) * Base1 + ( d * Base2 ) * Base2;
}

bool IntersectRayToTriangle( const LVector3& P, const LVector3& A, const LVector3& V1, const LVector3& V2, const LVector3& V3, LVector3& isect )
{
	LVector3 N = ( V3 - V2 ).Cross( V2 - V1 );

	if ( !IntersectRayToPlane( P, A, N , -N.Dot( V1 ), isect ) )
	{
		return false;
	}

	// try cross products
	LVector3 l1 = V1 - isect;
	LVector3 l2 = V2 - isect;
	LVector3 l3 = V3 - isect;

	float lam1 = ( l1 ).Cross( l2 ).Dot( N );
	float lam2 = ( l2 ).Cross( l3 ).Dot( N );
	float lam3 = ( l3 ).Cross( l1 ).Dot( N );

	// unnormalized barycentric coordinates must be of the same sign (the sign depends on N's direction and does not matter)
	return ( ( lam1 < 0 && lam2 < 0 && lam3 < 0 ) || ( lam1 > 0 && lam2 > 0 && lam3 > 0 ) );
}

bool IntersectRayToBox( const LVector3& P, const LVector3& A, const LVector3& Pos, LVector3* V, const LVector3& Size, LVector3& isect )
{
	float Sign[] = { -1, -1, -1, +1, +1, +1 };

	// distance to source point;
	float _dist = -1.0f;

	bool Intersection = false;

	// check six faces
	for ( int i = 0 ; i < 6 ; i++ )
	{
		LVector3 Pt = Sign[i] * V[i % 3] * Size[i % 3] * 0.5f;
		LVector3 N = Pt.GetNormalized();

		Pt = P + Pt;

		float D = N.Dot( Pt );

		LVector3 LocalISect;

		if ( IntersectRayToPlane( P, A, N, D, LocalISect ) )
		{
			// plane vectors
			LVector3 V1 = V[( i+1 )%3];
			LVector3 V2 = V[( i+2 )%3];

			float szX = Size[( i+1 )%3];
			float szY = Size[( i+2 )%3];

			// project to plane
			float x = ( LocalISect - Pt ).Dot( V1 );
			float y = ( LocalISect - Pt ).Dot( V2 );

			// check if the point is inside the rectangle
			if ( x > -szX / 2 && x < szX / 2 && y > -szY / 2 && y < szY / 2 )
			{
				Intersection = true;
				// it is, compare its distance to P - to select the closest one
				float _newDist = ( isect - P ).Length();

				if ( _dist < 0.0f || _newDist < _dist )
				{
					isect = LocalISect;
					_dist = _newDist;
				}
			}
		}
	}

	return Intersection;
}

// build the basis of complementary 2-dimensional space
void BuildComplementaryBasis( const LVector3& N, LVector3& V1, LVector3& V2 )
{
	V1 = N.Cross( vec3( 1, 0, 0 ) );

	if ( V1.SqrLength() <= Linderdaum::Math::EPSILON )
	{
		V1 = N.Cross( vec3( 0, 1, 0 ) );

		if ( V1.SqrLength() <= Linderdaum::Math::EPSILON )
		{
			V1 = N.Cross( vec3( 0, 0, 1 ) );
		}
	}

	V2 = N.Cross( V1 );

	V1.Normalize();
	V2.Normalize();
}

// Vertices - source vertex array, Center - center of the box, Axes - box axes, A - box extents
void BuildBoundingBox( int NumPts, const LVector3* Vertices, LVector3& Center, LVector3* Axes, float* A )
{
	if ( NumPts <= 0 ) { return; }

	int i, j, p; // indices

	Center = LVector3( 0, 0, 0 );

	// 1. determine geometric center

	float Denom = 1.0f / static_cast<float>( NumPts );

	for ( p = 0 ; p < NumPts ; p++ ) { Center += Vertices[p] * Denom; }

	// 2. calculate covariance matrix

	LMatrix3 Cov;

	// variations
	for ( p = 0 ; p < NumPts ; p++ )
	{
		LVector3 D = Vertices[p] - Center;

		for ( i = 0 ; i < 3 ; i++ )
			for ( j = 0 ; j < 3 ; j++ )
			{
				Cov[i][j] += ( D[i] * D[j] ) * Denom;
			}
	}

	// substract squared expectation
	for ( i = 0 ; i < 3 ; i++ )
		for ( j = 0 ; j < 3 ; j++ )
		{
			Cov[i][j] -= Center[i] * Center[j];
		}

	// 3. eigenvectors of the covariance matrix are the axes of the bbox
	Cov.CalculateEigenVectors( Axes, A );

	// 4. calculate box extents along each axis

	for ( i = 0 ; i < 3 ; i++ ) { A[i] = -1.0f; }

	for ( p = 0 ; p < NumPts ; p++ )
		for ( i = 0 ; i < 3 ; i++ )
		{
			float l = fabs( ( Vertices[p] - Center ).Dot( Axes[i] ) );

			if ( l > A[i] ) { A[i] = l; }
		}
}

// old Pascal source ...
bool ClosestPointsOnLines( const LVector3& Pt1, const LVector3& Dir1, const LVector3& Pt2, const LVector3& Dir2, float& t1, float& t2 )
{
	float C0, C1;
	float a11, a12, a21, a22;

	float det1, detA, detB;

	C0  = ( Pt2 - Pt1 ).Dot( Dir1 );
	C1  = ( Pt2 - Pt1 ).Dot( Dir2 );

	a11 = +Dir1.Dot( Dir1 );
	a12 = -Dir1.Dot( Dir2 );
	a21 = -a12;
	a22 = -Dir2.Dot( Dir2 );

	det1 = a11 * a22 - a21 * a12;

	FIXME( "perform approximate IsZero() check" )

	if ( det1 != 0.0f )
	{
		detA = C0 * a22 - C1 * a12;
		detB = a11 * C1 - a21 * C0;

		t1 = detA / det1;
		t2 = detB / det1;

		return true;
	}

	// no single point , but we just give a required pair
	t1 = C0 / a11;
	t2 = 0;

	return false;
}

/*
 * 22/02/2010
     It's here
*/
