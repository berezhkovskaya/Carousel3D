/*
   (Part of Linderdaum Engine)
   Version 0.6.00
   (17/01/2011)
   (C) Kosarevsky Sergey, 2008-2011
   (C) Latypov Viktor, 2010
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _LProjection_
#define _LProjection_

#include "LMatrix.h"

enum LProjectionType
{
	PROJECTION_ORTHOGRAPHIC = 0,
	PROJECTION_PERSPECTIVE  = 1,
	PROJECTION_ERROR = 2
};

namespace Linderdaum
{
	namespace Math
	{
		scriptexport( Math )  LMatrix4    Ortho2D( float L, float R, float B, float T );
		scriptexport( Math )  LMatrix4    Ortho( float L, float R, float B, float T, float N, float F );
		scriptexport( Math )  LMatrix4    Perspective( float FOV, float Aspect, float NearCP, float FarCP );
		/// true - the left eye, false - the right eye
		scriptexport( Math )  LMatrix4    PerspectiveStereo( float FOV, float Aspect, float NearCP, float FarCP, float IOD, float FocalLength, bool WhichEye );
		scriptexport( Math )  LMatrix4    Frustum( float L, float R, float B, float T, float N, float F );
		scriptexport( Math )  LMatrix4    LookAt( LVector3 Eye, LVector3 Center, LVector3 Up );
		/// true - the left eye, false - the right eye
		scriptexport( Math )  LMatrix4    LookAtStereo( LVector3 Eye, LVector3 Center, LVector3 Up, float IOD, bool WhichEye );
		scriptexport( Math )  LMatrix4    LookAtStereoMatrix( const LMatrix4& View, float IOD, bool WhichEye );
		scriptexport( Math )  LVector3    GetViewingDirection( const LMatrix4& View );
		/// from left to right
		scriptexport( Math )  LVector3    GetSideDirection( const LMatrix4& View );
		scriptexport( Math )  LMatrix4    ProjectReflectionTexture( const LMatrix4& Projection, const LMatrix4& ModelView );
		scriptexport( Math )  LVector3    ProjectPoint( LVector3 Point, const LMatrix4& Projection, const LMatrix4& ModelView );
		scriptexport( Math )  LVector3    ProjectPointNDC( const LVector3& Point, const LMatrix4& Projection, const LMatrix4& ModelView );
		scriptexport( Math )  LVector3    UnProjectPoint( LVector3 Point, const LMatrix4& Projection, const LMatrix4& ModelView );
		scriptexport( Math )  LVector3    UnProjectPointNDC( const LVector3& Point, const LMatrix4& Projection, const LMatrix4& ModelView );
		scriptexport( Math )  LMatrix4    ObliqueReflectionProjection( const LMatrix4& Projection, const LVector4& ClipPlane );

		/// look at the center of a AABB from such a distance (closest) that the box is entirely visible
		scriptexport( Math )  LMatrix4    GetClosestViewForAABB( const LVector3& MinV, const LVector3& MaxV, const LMatrix4& Proj, const LVector3& Eye, const LVector3& Up );

		scriptexport( Math )  void        FrustumToParams( const LMatrix4& M, float& L, float& R, float& B, float& T, float& N, float& F );
		scriptexport( Math )  void        PerspectiveToParams( const LMatrix4& M, float& FOV, float& Aspect, float& NearCP, float& FarCP );
		scriptexport( Math )  void        OrthoToParams( const LMatrix4& M, float& L, float& R, float& B, float& T, float& N, float& F );

		scriptexport( Math )  bool        IsOrthographicProjection( const LMatrix4& M );
		scriptexport( Math )  bool        IsPerspectiveProjection( const LMatrix4& M );

		scriptexport( Math )  void TransformRayToCoordinates( const LVector3& P, const LVector3& A, const LMatrix4& Transform, LVector3& TransP, LVector3& TransA );

		LProjectionType DetermineProjectionType( const LMatrix4& Projection );
	};
};

#endif

/*
 * 17/01/2011
     LookAtStereo()
     PerspectiveStereo()
 * 28/02/2008
     It's here
*/
