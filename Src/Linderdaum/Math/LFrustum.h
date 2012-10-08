/*
   (Part of Linderdaum Engine)
   Version 0.5.72
   (16/02/2007)
   (C) Sergey Kosarevsky, 2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _LFrustum_
#define _LFrustum_

#include "Platform.h"
#include "Math/LMath.h"
#include "Math/LAABB.h"
#include "Math/LSphere.h"
#include "Math/LMatrix.h"

enum LFrustumPlane
{
	PLANE_LEFT   = 0,
	PLANE_RIGHT  = 1,
	PLANE_TOP    = 2,
	PLANE_BOTTOM = 3,
	PLANE_NEAR   = 4,
	PLANE_FAR    = 5
};

enum LFrustumCorner
{
	FRUSTUM_ntl = 0,
	FRUSTUM_ntr = 1,
	FRUSTUM_nbl = 2,
	FRUSTUM_nbr = 3,
	FRUSTUM_ftl = 4,
	FRUSTUM_ftr = 5,
	FRUSTUM_fbl = 6,
	FRUSTUM_fbr = 7
};

/// Viewing frustum
class LFrustum
{
public:
	LFrustum();
	LFrustum( const LMatrix4& Projection,
	          const LMatrix4& ModelView )
	{
		CreateFrustum( Projection, ModelView );
	};
	//
	// LFrustum
	//
	void    CreateFrustum( const LMatrix4& Projection,
	                       const LMatrix4& ModelView );
	bool    IsPointInFrustum( const LVector3& Point ) const;
	bool    IsSphereInFrustum( const LSphere& Sphere ) const;
	bool    IsAABBInFrustum( const LAABoundingBox& Box ) const;

	TODO( "split frustum to parts for multiple shadow maps" )

	// SplitFromCSM()

	/// Get one of the frustum planes
	LVector4    GetPlane( int i ) const { return FPlanes[i]; }

public:
	// corner points for debug rendering
	LVector3    FCornerPoints[8];

private:
	TODO( "replace by LPlane" )
	LVector4    FPlanes[6];

	float FFarClipPlane;
	float FNearClipPlane;
private:
	void CalcOrthographicFrustumPoints( float L, float R, float B, float T, float N, float F, const LVector3& Pos, const LVector3& To, const LVector3& Up );
	void CalcPerspectiveFrustumPoints( float Angle, float Ratio, float NearCP, float FarCP, const LVector3& Pos, const LVector3& To, const LVector3& Up );
	void CalcFrustumInBasis( const LVector3& Pos, const LVector3& To, const LVector3& Up, float fw, float fh, float nw, float nh );
};

#endif

/*
 * 29/03/2010
     Recalculation of frustum corner points
     Plane/Point constants
 * 16/02/2007
     Development...
 * 24/01/2007
     It's here
*/
