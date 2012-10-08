#ifndef __GeomUtils__h__included__
#define __GeomUtils__h__included__

#include "LMatrix.h"

LVector3 Unproject( const LMatrix4& _proj, const LMatrix4& _view, const LVector3& _vec );
LVector3 Project( const LMatrix4& _proj, const LMatrix4& _view, const LVector3& _vec );

// build the basis of complementary 2-dimensional space
void BuildComplementaryBasis( const LVector3& N, LVector3& V1, LVector3& V2 );

void MouseCoordsToWorldPointAndRay( const LMatrix4& _proj, const LMatrix4& _view, float mx, float my, LVector3& srcPoint, LVector3& dir );

// P - source point, A - direction
// C - sphere center, R - radius
bool IntersectRayToSphere( const LVector3& P, const LVector3& A, const LVector3& C, float R );

bool IntersectRayToPlane( const LVector3& P, const LVector3& A, const LVector3& N, float D, LVector3& isect );

bool IntersectRayToTriangle( const LVector3& P, const LVector3& A, const LVector3& V1, const LVector3& V2, const LVector3& V3, LVector3& isect );

bool IntersectRayToBox( const LVector3& P, const LVector3& A, const LVector3& Pos, LVector3* V, const LVector3& Size, LVector3& isect );

// Vertices - source vertex array, Center - center of the box, Axes - box axes, A - box extents
void BuildBoundingBox( int NumPts, const LVector3* Vertices, LVector3& Center, LVector3* Axes, float* A );

/// Find closest points on two lines (if these lines are not collinear)
bool ClosestPointsOnLines( const LVector3& Pt1, const LVector3& Dir1, const LVector3& Pt2, const LVector3& Dir2, float& t1, float& t2 );

/// Find absolute distance to plane
inline float DistanceToPlane( const LVector3& From, const LVector3& PlanePoint, const LVector3& PlaneNormal )
{
	return fabs( ( From - PlanePoint ).Dot( PlaneNormal ) );
}

#endif

/*
 * 22/02/2010
     It's here
*/
