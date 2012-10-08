/**
 * \file Collision.cpp
 * \brief Basic collision detection
 * \version 0.5.93
 * \date 09/10/2010
 * \author Viktor Latypov, 2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Math/Collision.h"

#include "Math/LMath.h"
#include "Math/LPoly.h"
#include "Math/LHull.h"
#include "Math/LGeomUtils.h"

using namespace Linderdaum;

bool Linderdaum::Math::Collision_SphereToPlane( const LSphere& S, const LPlane& P, LVector3* ISect, LVector3* Normal, float* Depth )
{
	LVector3 PlaneNormal = P.GetNormal();
	float D = P.GetEquation().W;

	float dist = fabs( S.GetOrigin().Dot( PlaneNormal ) + D ) - S.GetRadius();

	if ( dist < 0 )
	{
		if ( ISect ) { *ISect = P.ProjectPoint( S.GetOrigin() ); }

		if ( Normal ) { *Normal = PlaneNormal; }

		if ( Depth ) { *Depth = fabs( dist ); }

		return true;
	}

	return false;
}

bool Linderdaum::Math::Collision_SphereToSphere( const LSphere& S1, const LSphere& S2, LVector3* ISect, LVector3* Normal, float* Depth )
{
	LVector3 d = S1.GetOrigin() - S2.GetOrigin();

	float dist = d.Length();

	float diff = dist - S1.GetRadius() - S2.GetRadius();

	if ( diff < 0 )
	{
		d.Normalize();

		if ( ISect ) { *ISect = ( ( S1.GetOrigin() - S1.GetRadius() * d ) + ( S2.GetOrigin() - S2.GetRadius() * d ) ) / 2.0f; }

		if ( Normal ) { *Normal = d; }

		if ( Depth ) { *Depth = fabs( diff ) / 2.0f; }

		return true;
	}

	return false;
}

bool Linderdaum::Math::Collision_CircleToCircle( const LCircle& S1, const LCircle& S2, LVector2* ISect, LVector2* Normal, float* Depth )
{
	LVector2 d = S1.GetOrigin() - S2.GetOrigin();

	float dist = d.Length();

	float diff = dist - S1.GetRadius() - S2.GetRadius();

	if ( diff < 0 )
	{
		d.Normalize();

		if ( ISect ) { *ISect = ( ( S1.GetOrigin() - S1.GetRadius() * d ) + ( S2.GetOrigin() - S2.GetRadius() * d ) ) / 2.0f; }

		if ( Normal ) { *Normal = d; }

		if ( Depth ) { *Depth = fabs( diff ) / 2.0f; }

		return true;
	}

	return false;
}

bool Linderdaum::Math::Collision_SphereToBox( const LSphere& Sphere, const LBox& Box, Linderdaum::Math::sCollisionInfo* Info )
{
	/// Now consider all 27 cases of possible intersection

	/**
	   Here V = (X,Y,Z) = sphere center in box reference frame

	   for 26 cases (excluding the one, when a sphere center is inside the box)

	   a) the collision point on the box is
	          BoxPt = BCenter + Sign(V*Axis1)*Min(Abs(X),A1)*Axis1+
	                            Sign(V*Axis2)*Min(Abs(Y),A2)*Axis2+
	                            Sign(V*Axis3)*Min(Abs(Z),A3)*Axis3

	   b) the box normal at collision point
	          BoxNormal = Normalize(w_1*Sign(V*Axis1)*Axis1+
	                                w_2*Sign(V*Axis2)*Axis2+
	                              w_3*Sign(V*Axis3)*Axis3)

	                   where w_1 = 1 if Abs(X) > A1
	                               0 otherwise
	                         w_2 = 1 if Abs(Y) > A2
	                               0 otherwise
	                         w_3 = 1 if Abs(Z) > A3
	                               0 otherwise
	      and on the sphere

	   c) the point      SphPt = SphCenter+R*(BoxPt-SphCenter)/Len(BoxPt-SC);
	   d)     normal     SphNormal = (BoxPt-SphCenter)/Len(BoxPt-SC);

	   e) finally , the intersection test is : Dist(PointOnBox,SphereCenter) <= R

	   The code is really straightforward and compact ,
	   but of course, the  unrolling of all vector operations
	   would make it faster, but significantly longer

	   // Case  1. X >  A1,       Y > A2       , Z >  A3         // sphere-to-vertex
	   // Case  2. X >  A1,       Y > A2       , Z < -A3         // sphere-to-vertex
	   // Case  3. X >  A1,       Y > A2       , Z in [-A3,A3]   // sphere-to-edge
	   // Case  4. X >  A1,       Y < -A2      , Z >  A3         // sphere-to-vertex
	   // Case  5. X >  A1,       Y < -A2      , Z < -A3         // sphere-to-vertex
	   // Case  6. X >  A1,       Y < -A2      , Z in [-A3,A3]   // sphere-to-edge
	   // Case  7. X >  A1,       Y in [-A2,A2], Z >  A3         // sphere-to-edge
	   // Case  8. X >  A1,       Y in [-A2,A2], Z < -A3         // sphere-to-edge
	   // Case  9. X >  A1,       Y in [-A2,A2], Z in [-A3,A3]   // sphere-to-face
	   // Case 10. X < -A1,       Y > A2       , Z >  A3         // sphere-to-vertex
	   // Case 11. X < -A1,       Y > A2       , Z < -A3         // sphere-to-vertex
	   // Case 12. X < -A1,       Y > A2       , Z in [-A3,A3]   // sphere-to-edge
	   // Case 13. X < -A1,       Y < -A2      , Z >  A3         // sphere-to-vertex
	   // Case 14. X < -A1,       Y < -A2      , Z < -A3         // sphere-to-vertex
	   // Case 15. X < -A1,       Y < -A2      , Z in [-A3,A3]   // sphere-to-edge
	   // Case 16. X < -A1,       Y in [-A2,A2], Z >  A3         // sphere-to-edge
	   // Case 17. X < -A1,       Y in [-A2,A2], Z < -A3         // sphere-to-edge
	   // Case 18. X < -A1,       Y in [-A2,A2], Z in [-A3,A3]   // sphere-to-face
	   // Case 19. X in [-A1,A1], Y > A2       , Z >  A3         // sphere-to-edge
	   // Case 20. X in [-A1,A1], Y > A2       , Z < -A3         // sphere-to-edge
	   // Case 21. X in [-A1,A1], Y > A2       , Z in [-A3,A3]   // sphere-to-face
	   // Case 22. X in [-A1,A1], Y < -A2      , Z >  A3         // sphere-to-edge
	   // Case 23. X in [-A1,A1], Y < -A2      , Z < -A3         // sphere-to-edge
	   // Case 24. X in [-A1,A1], Y < -A2      , Z in [-A3,A3]   // sphere-to-face
	   // Case 25. X in [-A1,A1], Y in [-A2,A2], Z >  A3         // sphere-to-face
	   // Case 26. X in [-A1,A1], Y in [-A2,A2], Z < -A3         // sphere-to-face
	   // Case 27. X in [-A1,A1], Y in [-A2,A2], Z in [-A3,A3]   // sphere center is inside the box
	*/

	/// Vector from box center to sphere center
	LVector3 D = Sphere.GetOrigin() - Box.FCenter;

	/// sphere center in box reference frame
	LVector3 V( D.Dot( Box.FAxis[0] ), D.Dot( Box.FAxis[1] ), D.Dot( Box.FAxis[2] ) );

	/// Auxiliary values
	LVector3 S, W;

	for ( int i = 0 ; i < 3 ; i++ )
	{
		S[i] = ( V[i] > 0 ) ? 1.0f : -1.0f;
		W[i] = ( Math::LAbs( V[i] ) > Box.FExtent[i] ) ? 1.0f : 0.0f;
	}

	/// 1) Collision point on box
	LVector3 T( S[0] * Math::LMin( fabs( V[0] ), Box.FExtent[0] ), S[1] * Math::LMin( fabs( V[1] ), Box.FExtent[1] ), S[2] * Math::LMin( fabs( V[2] ), Box.FExtent[2] ) );

	LVector3 CollisionPointOnBox = Box.FCenter + T[0] * Box.FAxis[0] + T[1] * Box.FAxis[1] + T[2] * Box.FAxis[2];

	/// test for intersection
	float Dist = ( CollisionPointOnBox - Sphere.GetOrigin() ).Length();

	if ( Dist > Sphere.GetRadius() ) { return false; } /// no intersection

	/// We have a collision

	/// if ( W_1 + W_2 + W_3 > 0) then the sphere's center is strictly outside the box

	/// 2) Box normal at collision point
	/**
	   at the very degenerate case (when the sphere's center coincides with the box' center)
	   we assign a zero normal

	   I think it is normal , since such situation is not possible in principle (when the overall simulation goes fine)
	*/

	LVector3 CollisionNormalOnBox;

	if ( W[0] + W[1] + W[2] > 0 )
	{
		CollisionNormalOnBox = W[0] * S[0] * Box.FAxis[0] + W[1] * S[1] * Box.FAxis[1] + W[2] * S[2] * Box.FAxis[2];

		CollisionNormalOnBox.Normalize();
	}
	else
	{
		/// push the box "down"
		CollisionNormalOnBox = vec3( 0, 0, -1 );
	}

	LVector3 CollisionNormalOnSphere;

	/// 4) normalize the sphere's normal
	if ( Dist > Math::EPSILON )
	{
		CollisionNormalOnSphere = ( CollisionPointOnBox - Sphere.GetOrigin() ) / Dist;
	}
	else
	{
		/// push the sphere "up"
		CollisionNormalOnSphere = vec3( 0, 0, 1 );
	}

	/// 5) collision point on box
	LVector3 CollisionPointOnSphere = Sphere.GetOrigin() + Sphere.GetRadius() * CollisionNormalOnSphere;

	Info->FNumPoints = 1;

	/// first the sphere
	Info->FPoints1[0] = CollisionPointOnSphere;
	Info->N1 = CollisionNormalOnSphere;

	/// then the box
	Info->FPoints2[0] = CollisionPointOnBox;
	Info->N2 = CollisionNormalOnBox;

	Info->FPoints[0] = ( Info->FPoints1[0] + Info->FPoints2[0] ) * 0.5f;

	/// penetration
	Info->FPenetration[0] = ( CollisionPointOnBox - CollisionPointOnSphere ).Length();

	return true;
}

///Possible axes for box-box separation
enum eBoxSeparatingAxis
{
	S_AXIS_NONE = -1,
	S_AXIS_A0   =  0,
	S_AXIS_A1   =  1,
	S_AXIS_A2   =  2,
	S_AXIS_B0   =  3,
	S_AXIS_B1   =  4,
	S_AXIS_B2   =  5,
	S_AXIS_A0B0 =  6,
	S_AXIS_A0B1 =  7,
	S_AXIS_A0B2 =  8,
	S_AXIS_A1B0 =  9,
	S_AXIS_A1B1 = 10,
	S_AXIS_A1B2 = 11,
	S_AXIS_A2B0 = 12,
	S_AXIS_A2B1 = 13,
	S_AXIS_A2B2 = 14,
};

bool Linderdaum::Math::Collision_BoxToBox( const LBox& ABox, const LBox& BBox, Linderdaum::Math::sCollisionInfo* Info )
{
	float R, R0, R1;

	/// Maximum detected penetration
	float MaxDepth = -10000.0f;

	eBoxSeparatingAxis SeparatingAxis = S_AXIS_NONE;

	float AxisLen, TmpDepth;

	/// Relative distance
	LVector3 D = BBox.FCenter - ABox.FCenter;

	/// No contact detected yet
	Info->FNumPoints = 0;

	SeparatingAxis = S_AXIS_NONE;

	LVector3 Normal, RelNormal, TempAxis;
	bool InvertNormal = false;

	/// Test each separating plane with Axes A0..A2, B0..B2 (six cases)

	/// The normal is not relative in this case
	bool IsRelative = false;

#define TEST_SEP_PLANE( PARAM_AxisName, PARAM_RelativeVal, PARAM_R0, PARAM_R1, PARAM_Normal ) \
   R  = fabs(PARAM_RelativeVal);       \
   R0 = PARAM_R0;             \
   R1 = PARAM_R1;             \
   /* If (R>R0+R1) Then there is no intersection */\
   TmpDepth = R0 + R1 - R;          \
   if (TmpDepth < 0) return false;         \
                     \
   if (MaxDepth > TmpDepth) {        \
      MaxDepth = TmpDepth;       \
      SeparatingAxis = PARAM_AxisName; \
      Normal = PARAM_Normal;        \
      InvertNormal = (PARAM_RelativeVal < 0);   \
   }

	float a0 = ABox.FExtent[0];
	float a1 = ABox.FExtent[1];
	float a2 = ABox.FExtent[2];
	float b0 = BBox.FExtent[0];
	float b1 = BBox.FExtent[1];
	float b2 = BBox.FExtent[2];

	/// 1. A0
	float A0D = ABox.FAxis[0].Dot( D );

	float c00 = ABox.FAxis[0].Dot( BBox.FAxis[0] );
	float c01 = ABox.FAxis[0].Dot( BBox.FAxis[1] );
	float c02 = ABox.FAxis[0].Dot( BBox.FAxis[2] );

	TEST_SEP_PLANE( S_AXIS_A0, A0D, a0, b0 * fabs( c00 ) + b1 * fabs( c01 ) + b2 * fabs( c02 ), ABox.FAxis[0] )

	/// 2. A1
	float A1D = D.Dot( ABox.FAxis[1] );

	float c10 = ABox.FAxis[1].Dot( BBox.FAxis[0] );
	float c11 = ABox.FAxis[1].Dot( BBox.FAxis[1] );
	float c12 = ABox.FAxis[1].Dot( BBox.FAxis[2] );

	TEST_SEP_PLANE( S_AXIS_A1, A1D, a1, b0 * fabs( c10 ) + b1 * fabs( c11 ) + b2 * fabs( c12 ), ABox.FAxis[1] )

	/// 3. A2
	float A2D = ABox.FAxis[2].Dot( D );

	float c20 = ABox.FAxis[2].Dot( BBox.FAxis[0] );
	float c21 = ABox.FAxis[2].Dot( BBox.FAxis[1] );
	float c22 = ABox.FAxis[2].Dot( BBox.FAxis[2] );

	TEST_SEP_PLANE( S_AXIS_A2, A2D, a2, b0 * fabs( c20 ) + b1 * fabs( c21 ) + b2 * fabs( c22 ), ABox.FAxis[2] )

	/// 4. B0
	float B0D = BBox.FAxis[0].Dot( D );

	TEST_SEP_PLANE( S_AXIS_B0, B0D, a0 * fabs( c00 ) + a1 * fabs( c01 ) + a2 * fabs( c02 ), b0, BBox.FAxis[0] )

	/// 5. B1
	float B1D = BBox.FAxis[1].Dot( D );

	TEST_SEP_PLANE( S_AXIS_B1, B1D, a0 * fabs( c10 ) + a1 * fabs( c11 ) + a2 * fabs( c12 ), b1, BBox.FAxis[1] )

	/// 6. B2
	float B2D = BBox.FAxis[2].Dot( D );

	TEST_SEP_PLANE( S_AXIS_B2, B2D, a0 * fabs( c20 ) + a1 * fabs( c21 ) + a2 * fabs( c22 ), b2, BBox.FAxis[2] )

#undef TEST_SEP_PLANE

	/// Now we test the cross-product axes

	LVector3 DirA, DirB;

	IsRelative = true;

#define TEST_SEP_AXIS( PARAM_AxisName, PARAM_DirA, PARAM_DirB, PARAM_RelativeVal, PARAM_R0, PARAM_R1) \
   TempAxis = PARAM_DirA .Cross( PARAM_DirB );     \
   AxisLen  = TempAxis.SqrLength();       \
                        \
   if ( AxisLen > Linderdaum::Math::EPSILON)           \
   {                    \
      R  = PARAM_RelativeVal;          \
      R0 = PARAM_R0;             \
      R1 = PARAM_R1;             \
                        \
      TmpDepth = R0 + R1 - fabs(R);       \
      if (TmpDepth < 0) return false;         \
                        \
      if (MaxDepth * AxisLen > TmpDepth )     \
      {                 \
         MaxDepth = TmpDepth / AxisLen;      \
         SeparatingAxis = PARAM_AxisName; \
                        \
         DirA = PARAM_DirA;         \
         DirB = PARAM_DirB;         \
                        \
         RelNormal = TempAxis;         \
         InvertNormal = (PARAM_RelativeVal < 0); \
      }                 \
   }

	///  7.-15.       Name        DirA            DirB                RelVal                      R0                           R1
	TEST_SEP_AXIS( S_AXIS_A0B0, ABox.FAxis[0], BBox.FAxis[0] , c10 * A2D - c20 * A1D,  a1 * fabs( c20 ) + a2 * fabs( c10 ), b1 * fabs( c02 ) + b2 * fabs( c01 ) )
	TEST_SEP_AXIS( S_AXIS_A0B1, ABox.FAxis[0], BBox.FAxis[1] , c11 * A2D - c21 * A1D,  a1 * fabs( c21 ) + a2 * fabs( c11 ), b0 * fabs( c02 ) + b2 * fabs( c00 ) )
	TEST_SEP_AXIS( S_AXIS_A0B2, ABox.FAxis[0], BBox.FAxis[2] , c12 * A2D - c22 * A1D,  a1 * fabs( c22 ) + a2 * fabs( c12 ), b0 * fabs( c01 ) + b1 * fabs( c00 ) )
	TEST_SEP_AXIS( S_AXIS_A1B0, ABox.FAxis[1], BBox.FAxis[0] , c20 * A0D - c00 * A2D,  a0 * fabs( c20 ) + a2 * fabs( c00 ), b1 * fabs( c12 ) + b2 * fabs( c11 ) )
	TEST_SEP_AXIS( S_AXIS_A1B1, ABox.FAxis[1], BBox.FAxis[1] , c21 * A0D - c01 * A2D,  a0 * fabs( c21 ) + a2 * fabs( c01 ), b0 * fabs( c12 ) + b2 * fabs( c10 ) )
	TEST_SEP_AXIS( S_AXIS_A1B2, ABox.FAxis[1], BBox.FAxis[2] , c22 * A0D - c02 * A2D,  a0 * fabs( c22 ) + a2 * fabs( c02 ), b0 * fabs( c11 ) + b1 * fabs( c10 ) )
	TEST_SEP_AXIS( S_AXIS_A2B0, ABox.FAxis[2], BBox.FAxis[0] , c00 * A1D - c10 * A0D,  a0 * fabs( c10 ) + a1 * fabs( c00 ), b1 * fabs( c22 ) + b2 * fabs( c21 ) )
	TEST_SEP_AXIS( S_AXIS_A2B1, ABox.FAxis[2], BBox.FAxis[1] , c01 * A1D - c11 * A0D,  a0 * fabs( c11 ) + a1 * fabs( c01 ), b0 * fabs( c22 ) + b2 * fabs( c20 ) )
	TEST_SEP_AXIS( S_AXIS_A2B2, ABox.FAxis[2], BBox.FAxis[2] , c02 * A1D - c12 * A0D,  a0 * fabs( c12 ) + a1 * fabs( c02 ), b0 * fabs( c21 ) + b1 * fabs( c20 ) )

	if ( SeparatingAxis == S_AXIS_NONE ) { return false; }

#undef TEST_SEP_AXIS

	/// Now we will determine the canonical set of contact points that the caller will find the in the global array

	/// 1. Find contact normal

	if ( IsRelative )
	{
		/// Normal to the first box in global coords
		Normal = RelNormal.Dot( ABox.FAxis[0] ) * ABox.FAxis[0] + RelNormal.Dot( ABox.FAxis[1] ) * ABox.FAxis[1] + RelNormal.Dot( ABox.FAxis[2] ) * ABox.FAxis[2];
	}

	if ( InvertNormal ) { Normal = -Normal; }

	Normal.Normalize();

	/// 2. Consider all possible cases of SeparatingAxis

	if ( SeparatingAxis > S_AXIS_B2 )
	{
		/// edge-to-edge contact

		/// a) Find points on the closest edges

		float vv1 = Math::Sign( Normal.Dot( ABox.FAxis[0] ) ) * a0;
		float vv2 = Math::Sign( Normal.Dot( ABox.FAxis[1] ) ) * a1;
		float vv3 = Math::Sign( Normal.Dot( ABox.FAxis[2] ) ) * a2;

		LVector3 pA = ABox.FCenter + vv1 * ABox.FAxis[0] + vv2 * ABox.FAxis[1] + vv3 * ABox.FAxis[2];

		vv1 = Math::Sign( Normal.Dot( BBox.FAxis[0] ) ) * b0;
		vv2 = Math::Sign( Normal.Dot( BBox.FAxis[1] ) ) * b1;
		vv3 = Math::Sign( Normal.Dot( BBox.FAxis[2] ) ) * b2;

		LVector3 pB = BBox.FCenter + vv1 * BBox.FAxis[0] + vv2 * BBox.FAxis[1] + vv3 * BBox.FAxis[2];

		/// b) Find directions for the edges

		/// DirA, DirB are already determined in non-intersection testing above

		/// c) Now find two closest points on the edge

		float t1, t2;
		ClosestPointsOnLines( pA, DirA, pB, DirB, t1, t2 );

		pA = pA + t1 * DirA;
		pB = pB + t2 * DirB;

		/// d) Fill the contact information structure

		Info->N1 =  Normal;
		Info->N2 = -Normal;

		/// Single contact point
		Info->FNumPoints = 1;

		Info->FPoints1[0] = pA;
		Info->FPoints2[0] = pB;

		Info->FPoints[0] = ( pA + pB ) * 0.5f;

		/// Distance as a penetration
		Info->FPenetration[0] = ( pA - pB ).Length();
	}
	else
	{
		/// face-to-something contact
		float ReferenceExtent0 = 0.0f;
		LVector3  ReferenceCenter, ReferenceNormal;

		/// a) Find normals to the colliding boxes and build the collision polygon to be clipped

		LPoly ReferencePoly;

		/// Build a quad corresponding to the face, wich is parallel to the separating plane
		ReferencePoly.FNumVertices = 4;

		/// "Height" is the size in normal direction
#define BUILD_REFERENCE_POLY(PARAM_AxisName, PARAM_Height, PARAM_Extent0, PARAM_Extent1, PARAM_Axis0, PARAM_Axis1) \
   case PARAM_AxisName : \
   {\
      ReferenceExtent0 = PARAM_Height;                         \
      ReferencePoly.FVertices[0] = - PARAM_Extent0 * PARAM_Axis0 - PARAM_Extent1 * PARAM_Axis1;    \
      ReferencePoly.FVertices[1] = + PARAM_Extent0 * PARAM_Axis0 - PARAM_Extent1 * PARAM_Axis1;    \
      ReferencePoly.FVertices[2] = + PARAM_Extent0 * PARAM_Axis0 + PARAM_Extent1 * PARAM_Axis1;    \
      ReferencePoly.FVertices[3] = - PARAM_Extent0 * PARAM_Axis0 + PARAM_Extent1 * PARAM_Axis1;    \
      break; \
   }

		switch ( SeparatingAxis )
		{
				BUILD_REFERENCE_POLY( S_AXIS_A0, a0, a1, a2, ABox.FAxis[1], ABox.FAxis[2] )
				BUILD_REFERENCE_POLY( S_AXIS_A1, a1, a0, a2, ABox.FAxis[0], ABox.FAxis[2] )
				BUILD_REFERENCE_POLY( S_AXIS_A2, a2, a0, a1, ABox.FAxis[0], ABox.FAxis[1] )
				BUILD_REFERENCE_POLY( S_AXIS_B0, b0, b1, b2, BBox.FAxis[1], BBox.FAxis[2] )
				BUILD_REFERENCE_POLY( S_AXIS_B1, b1, b0, b2, BBox.FAxis[0], BBox.FAxis[2] )
				BUILD_REFERENCE_POLY( S_AXIS_B2, b2, b0, b1, BBox.FAxis[0], BBox.FAxis[1] )
		}

#undef BUILD_REFERENCE_POLY

		LVector3  IncidentCenter, IncidentNormal;
		LVector3  IncidentAxis1,  IncidentAxis2;
		float IncidentSize1 = 0.0f,  IncidentSize2 = 0.0f;

		/// b) Correct normal's direction

		float Dist = 1000000.0f;
		float Dist1;

		/// Now we search the incident face: the center of incident face is closest to the reference face center

#define CHECK_INCIDENT_AXIS(PARAM_ICenter, PARAM_Normal, PARAM_V1, PARAM_V2, PARAM_Size1, PARAM_Size2) \
      TmpVec = PARAM_ICenter;                   \
      Dist1 = DistanceToPlane( TmpVec, ReferenceCenter, ReferenceNormal);  \
                                 \
      if ( Dist > Dist1 )                     \
      {                          \
         IncidentCenter = TmpVec;               \
         IncidentNormal = PARAM_Normal;               \
         IncidentAxis1  = PARAM_V1;             \
         IncidentAxis2  = PARAM_V2;             \
         IncidentSize1  = PARAM_Size1;             \
         IncidentSize2  = PARAM_Size2;             \
         Dist = Dist1;                    \
      }

		LVector3 TmpVec;

		if ( SeparatingAxis < S_AXIS_B0 )
		{
			/// A0..A2

			ReferenceNormal = ( ( Normal.Dot( D ) > 0 ) ? +1.0f : -1.0f ) * Normal;

			ReferenceCenter = ReferenceNormal * ReferenceExtent0 + ABox.FCenter;

			CHECK_INCIDENT_AXIS( BBox.FCenter + b0 * BBox.FAxis[0],  BBox.FAxis[0], BBox.FAxis[1], BBox.FAxis[2], b1, b2 )
			CHECK_INCIDENT_AXIS( BBox.FCenter - b0 * BBox.FAxis[0], -BBox.FAxis[0], BBox.FAxis[1], BBox.FAxis[2], b1, b2 )
			CHECK_INCIDENT_AXIS( BBox.FCenter + b1 * BBox.FAxis[1],  BBox.FAxis[1], BBox.FAxis[0], BBox.FAxis[2], b0, b2 )
			CHECK_INCIDENT_AXIS( BBox.FCenter - b1 * BBox.FAxis[1], -BBox.FAxis[1], BBox.FAxis[0], BBox.FAxis[2], b0, b2 )
			CHECK_INCIDENT_AXIS( BBox.FCenter + b2 * BBox.FAxis[2],  BBox.FAxis[2], BBox.FAxis[0], BBox.FAxis[1], b0, b1 )
			CHECK_INCIDENT_AXIS( BBox.FCenter - b2 * BBox.FAxis[2], -BBox.FAxis[2], BBox.FAxis[0], BBox.FAxis[1], b0, b1 )

			Info->N1 = ReferenceNormal;
			Info->N2 = IncidentNormal;
		}
		else
		{
			/// B0..B2
			ReferenceNormal = ( ( Normal.Dot( D ) > 0 ) ? -1.0f : 1.0f ) * Normal;

			ReferenceCenter = ReferenceNormal * ReferenceExtent0 + BBox.FCenter;

			CHECK_INCIDENT_AXIS( ABox.FCenter + a0 * ABox.FAxis[0],  ABox.FAxis[0], ABox.FAxis[1], ABox.FAxis[2], a1, a2 )
			CHECK_INCIDENT_AXIS( ABox.FCenter - a0 * ABox.FAxis[0], -ABox.FAxis[0], ABox.FAxis[1], ABox.FAxis[2], a1, a2 )
			CHECK_INCIDENT_AXIS( ABox.FCenter + a1 * ABox.FAxis[1],  ABox.FAxis[1], ABox.FAxis[0], ABox.FAxis[2], a0, a2 )
			CHECK_INCIDENT_AXIS( ABox.FCenter - a1 * ABox.FAxis[1], -ABox.FAxis[1], ABox.FAxis[0], ABox.FAxis[2], a0, a2 )
			CHECK_INCIDENT_AXIS( ABox.FCenter + a2 * ABox.FAxis[2],  ABox.FAxis[2], ABox.FAxis[0], ABox.FAxis[1], a0, a1 )
			CHECK_INCIDENT_AXIS( ABox.FCenter - a2 * ABox.FAxis[2], -ABox.FAxis[2], ABox.FAxis[0], ABox.FAxis[1], a0, a1 )

			Info->N1 = IncidentNormal;
			Info->N2 = ReferenceNormal;
		}

#undef CHECK_INCIDENT_AXIS

		/// Shift every vertex of the reference poly to make it coincide with the reference plane
		ReferencePoly.Shift( ReferenceCenter );

		/// c) Here we have normals to the boxes and now we will just find the contact points between to quads

		/// Construct the clipping hull for the incident face
		LPlaneHull IncidentHull;
		IncidentHull.HullForQuad( IncidentCenter, IncidentNormal, IncidentAxis1, IncidentAxis2, IncidentSize1, IncidentSize2 );

		/// Clip the reference polygon by IncidentHull
		IncidentHull.ClipPolygon( ReferencePoly );

		/// Copy remaining polygon to the contact point list
		Info->FNumPoints = ReferencePoly.FNumVertices;

		for ( int i = 0 ; i < Info->FNumPoints ; i++ )
		{
			Info->FPoints[i] = Info->FPoints1[i] = Info->FPoints2[i] = ReferencePoly.FVertices[i];
//			Info->FPenetration[i] = 0.0f; // TODO: estimate penetration depth
		}
	}

	// we are done
	return true;
}

bool Linderdaum::Math::Collision_BoxToPlane( const LBox& B, const LPlane& P, sCollisionInfo* Info )
{
	return false;
}

/*
 * 07/01/2011
     Collision_SphereToBox
     Initial translation of Collision_BoxToBox
 * 09/10/2010
     Collision_CircleToCircle()
 * 16/06/2010
     It's here
*/
