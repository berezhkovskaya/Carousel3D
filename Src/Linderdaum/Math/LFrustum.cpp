/*
   (Part of Linderdaum Engine)
   Version 0.5.71
   (24/01/2007)
   (C) Sergey Kosarevsky, 2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "LFrustum.h"

#include "Math/LSphere.h"

LFrustum::LFrustum(): FPlanes()
{
}

void LFrustum::CreateFrustum( const LMatrix4& Projection,
                              const LMatrix4& ModelView )
{
	LMatrix4 ProjModelView = ModelView * Projection;

	for ( int i = 0; i != 4; ++i )
	{
		FPlanes[0][i] = ProjModelView[i][3] + ProjModelView[i][0]; // left
		FPlanes[1][i] = ProjModelView[i][3] - ProjModelView[i][0]; // right
		FPlanes[2][i] = ProjModelView[i][3] - ProjModelView[i][1]; // top
		FPlanes[3][i] = ProjModelView[i][3] + ProjModelView[i][1]; // bottom
		FPlanes[4][i] = ProjModelView[i][3] + ProjModelView[i][2]; // near
		FPlanes[5][i] = ProjModelView[i][3] - ProjModelView[i][2]; // far
	}

	for ( int i = 0; i != 6; ++i )
	{
		LVector3 Vec = FPlanes[i].ToVector3();

		float Len = Vec.Length();

		if ( Len > Linderdaum::Math::EPSILON ) { FPlanes[i] /= Len; }
	}
}

// don't check far & near planes
static const int CheckPlanes = 6;

bool LFrustum::IsPointInFrustum( const LVector3& Point ) const
{
	LVector4 Point4 = LVector4( Point, 1.0f );

	for ( int i = 0; i != CheckPlanes; ++i )
	{
		if ( FPlanes[i].Dot( Point4 ) <= 0 )
		{
			return false;
		}
	}

	return true;
}

bool LFrustum::IsSphereInFrustum( const LSphere& Sphere ) const
{
	if ( Sphere.GetOrigin().SqrLength() < Linderdaum::Math::EPSILON /*IsZeroVector()*/ )
	{
		return true;
	}

	LVector4 Point4 = LVector4( Sphere.GetOrigin(), 1.0f );

//	int Planes = 0;

	for ( int i = 0; i != CheckPlanes; ++i )
	{
		float Distance = FPlanes[i].Dot( Point4 );

		// completely outside
		if ( Distance < -Sphere.GetRadius() ) { return false; }

		// partially inside
		if ( fabs( Distance ) < Sphere.GetRadius() ) { return true; }

//		if ( Distance > Sphere.GetRadius() ) Planes++;
	}

	// NOTE:  Planes == CheckPlanes  - full inside
	//        Planes != CheckPlanes  - partial inside

	return true;
}

bool LFrustum::IsAABBInFrustum( const LAABoundingBox& Box ) const
{
	LVector3 bMax = Box.FMax;
	LVector3 bMin = Box.FMin;

	LVector3 Near, N;

	const LVector4* Plane = &FPlanes[0];

	/// Find extreme point for each plane
	for ( int i = 0 ; i < 6 ; i++ )
	{
		N = Plane->ToVector3();

		Near.x = ( N.x > 0.0f ) ? bMin.x : bMax.x;
		Near.y = ( N.y > 0.0f ) ? bMin.y : bMax.y;
		Near.z = ( N.z > 0.0f ) ? bMin.z : bMax.z;

		/// If the near extreme point is outside, then the AABB is totally outside the frustum
		if ( N.Dot( Near ) + Plane->W > 0.0f ) { return false; }

		Plane++;
	}

	return true;
}

// indices of the plane-defining corner points
LVector3 FrustumPlanePoints[] =
{
	// PLANE_LEFT
	LVector3( FRUSTUM_ntl, FRUSTUM_nbl, FRUSTUM_fbl ),
	// PLANE_RIGHT
	LVector3( FRUSTUM_nbr, FRUSTUM_ntr, FRUSTUM_fbr ),
	// PLANE_TOP
	LVector3( FRUSTUM_ntr, FRUSTUM_ntl, FRUSTUM_ftl ),
	// PLANE_BOTTOM
	LVector3( FRUSTUM_nbl, FRUSTUM_nbr, FRUSTUM_fbr ),
	// PLANE_NEAR
	LVector3( FRUSTUM_ntl, FRUSTUM_ntr, FRUSTUM_nbr ),
	// PLANE_FAR
	LVector3( FRUSTUM_ftr, FRUSTUM_ftl, FRUSTUM_fbl )
};

void LFrustum::CalcFrustumInBasis( const LVector3& Pos, const LVector3& To, const LVector3& Up, float fw, float fh, float nw, float nh )
{
	LVector3 Z = Pos - To;
	Z.Normalize();

	LVector3 X = Up.Cross( Z );
	X.Normalize();

	LVector3 Y = Z.Cross( X );

	LVector3 nc = Pos - Z * FNearClipPlane;
	LVector3 fc = Pos - Z * FFarClipPlane;

	FCornerPoints[FRUSTUM_ntl] = nc + Y * nh - X * nw;
	FCornerPoints[FRUSTUM_ntr] = nc + Y * nh + X * nw;
	FCornerPoints[FRUSTUM_nbl] = nc - Y * nh - X * nw;
	FCornerPoints[FRUSTUM_nbr] = nc - Y * nh + X * nw;

	FCornerPoints[FRUSTUM_ftl] = fc + Y * fh - X * fw;
	FCornerPoints[FRUSTUM_ftr] = fc + Y * fh + X * fw;
	FCornerPoints[FRUSTUM_fbl] = fc - Y * fh - X * fw;
	FCornerPoints[FRUSTUM_fbr] = fc - Y * fh + X * fw;

	for ( int i = PLANE_LEFT ; i < PLANE_FAR + 1 ; i++ )
	{
		LVector3 P1 = FCornerPoints[ static_cast<int>( FrustumPlanePoints[i].X ) ];
		LVector3 P2 = FCornerPoints[ static_cast<int>( FrustumPlanePoints[i].Y ) ];
		LVector3 P3 = FCornerPoints[ static_cast<int>( FrustumPlanePoints[i].Z ) ];

		TODO( "store planes as LPlane, not as Vector4 !" )

//    FPlanes[i].From3Points(P1, P2, P3);
	}
}

void LFrustum::CalcOrthographicFrustumPoints( float L, float R, float B, float T, float N, float F, const LVector3& Pos, const LVector3& To, const LVector3& Up )
{
	float nh = T - B;
	float nw = R - L;
	float fh = nh;
	float fw = nw;

	CalcFrustumInBasis( Pos, To, Up, fw, fh, nw, nh );
}

void LFrustum::CalcPerspectiveFrustumPoints( float Angle, float Ratio, float NearCP, float FarCP, const LVector3& Pos, const LVector3& To, const LVector3& Up )
{
	float Tang = ( float )tan( Linderdaum::Math::DegToRad( Angle ) );

	float nh = NearCP * Tang;
	float nw = nh * Ratio;
	float fh = FarCP  * Tang;
	float fw = fh * Ratio;

	CalcFrustumInBasis( Pos, To, Up, fw, fh, nw, nh );
}

/*
 * 16/02/2007
     Development...
 * 24/01/2007
     It's here
*/
