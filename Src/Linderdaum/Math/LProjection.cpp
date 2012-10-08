/*
   (Part of Linderdaum Engine)
   Version 0.6.00
   (17/01/2011)
   (C) Kosarevsky Sergey, 2008-2011
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "LProjection.h"

namespace Linderdaum
{
	namespace Math
	{

		LProjectionType DetermineProjectionType( const LMatrix4& Projection )
		{
			if ( IsOrthographicProjection( Projection ) )
			{
				return PROJECTION_ORTHOGRAPHIC;
			}

			if ( IsPerspectiveProjection( Projection ) )
			{
				return PROJECTION_PERSPECTIVE;
			}

			return PROJECTION_ERROR;
		}

		bool IsOrthographicProjection( const LMatrix4& M )
		{
			return ( fabs( M[3][3] - 1.0f ) < EPSILON );
		}

		bool IsPerspectiveProjection( const LMatrix4& M )
		{
			return ( fabs( M[3][3] ) < EPSILON );
		}

		LMatrix4 Ortho2D( float L, float R, float B, float T )
		{
			return Ortho( L, R, B, T, -1, 1 );
		}

		LMatrix4 Ortho( float L, float R, float B, float T, float N, float F )
		{
			LMatrix4 Matrix;

			Matrix[0][0] = 2 / ( R - L );
			Matrix[1][0] = 0;
			Matrix[2][0] = 0;
			Matrix[3][0] = -( R + L ) / ( R - L );

			Matrix[0][1] = 0;
			Matrix[1][1] = 2 / ( T - B );
			Matrix[2][1] = 0;
			Matrix[3][1] = -( T + B ) / ( T - B );

			Matrix[0][2] = 0;
			Matrix[1][2] = 0;
			Matrix[2][2] = -2 / ( F - N );
			Matrix[3][2] = -( F + N ) / ( F - N );

			Matrix[0][3] = 0;
			Matrix[1][3] = 0;
			Matrix[2][3] = 0;
			Matrix[3][3] = 1;

			return Matrix;
		}

		LMatrix4 Perspective( float FOV, float Aspect, float NearCP, float FarCP )
		{
			float YMax = NearCP * tan( FOV * ::Linderdaum::Math::DTOR * 0.5f );
			float YMin = -YMax;
			float XMin = YMin * Aspect;
			float XMax = YMax * Aspect;

			return Frustum( XMin, XMax, YMin, YMax, NearCP, FarCP );
		}

		LMatrix4 PerspectiveStereo( float FOV, float Aspect, float NearCP, float FarCP, float IOD, float FocalLength, bool WhichEye )
		{
			float D      = NearCP * tan( FOV * 0.5f );
			float Offset = ( WhichEye ? -0.5f : 0.5f ) * IOD * NearCP / FocalLength;

			float Left   = - Aspect * D;
			float Right  =   Aspect * D;
			float Bottom = - D + Offset;
			float Top    =   D + Offset;

			return Frustum( Bottom, Top, Left, Right, NearCP, FarCP );
		}

		LMatrix4 Frustum( float L, float R, float B, float T, float N, float F )
		{
			LMatrix4 Matrix;

			Matrix[0][0] =  2 * N / ( R - L );
			Matrix[1][0] =  0;
			Matrix[2][0] =  ( R + L ) / ( R - L );
			Matrix[3][0] =  0;

			Matrix[0][1] =  0;
			Matrix[1][1] =  2 * N / ( T - B );
			Matrix[2][1] =  ( T + B ) / ( T - B );
			Matrix[3][1] =  0;

			Matrix[0][2] =  0;
			Matrix[1][2] =  0;
			Matrix[2][2] = -( F + N ) / ( F - N );
			Matrix[3][2] = -2 * F * N / ( F - N );

			Matrix[0][3] =  0;
			Matrix[1][3] =  0;
			Matrix[2][3] = -1;
			Matrix[3][3] =  0;

			return Matrix;
		}

		void OrthoToParams( const LMatrix4& M, float& L, float& R, float& B, float& T, float& N, float& F )
		{
			float m00 = M[0][0];
			float m30 = M[3][0];
			float m11 = M[1][1];
			float m31 = M[3][1];
			float m22 = M[2][2];
			float m32 = M[3][2];

			N = ( 1.0f - m32 ) / m22;
			F = -( m32 + 1.0f ) / m22;
			B = ( 1.0f - m31 ) / m11;
			T = -( m32 + 1.0f ) / m22;
			L = ( 1.0f - m30 ) / m00;
			R = -( m30 + 1.0f ) / m00;
		}

		void FrustumToParams( const LMatrix4& M, float& L, float& R, float& B, float& T, float& N, float& F )
		{
			float m00 = M[0][0];
			float m20 = M[2][0];
			float m11 = M[1][1];
			float m21 = M[2][1];
			float m22 = M[2][2];
			float m32 = M[3][2];

			N = +m32 / ( 1.0f - m22 );
			F = -m32 / ( 1.0f + m22 );

			L = N * ( m20 - 1.0f ) / m00;
			R = N * ( m20 + 1.0f ) / m00;

			B = N * ( m21 - 1.0f ) / m11;
			T = N * ( m21 + 1.0f ) / m11;
		}

		void PerspectiveToParams( const LMatrix4& M, float& FOV, float& Aspect, float& NearCP, float& FarCP )
		{
			float XMin, XMax, YMin, YMax;

			FrustumToParams( M, XMin, XMax, YMin, YMax, NearCP, FarCP );

			FOV = 2.0f * atan( YMax / NearCP ) / ( ::Linderdaum::Math::DTOR );

			if ( fabs( YMin ) < 0.0f )
			{
				Aspect = XMax / YMax;
			}
			else
			{
				Aspect = XMin / YMin;
			}
		}

		LMatrix4 LookAt( LVector3 Eye, LVector3 Center, LVector3 Up )
		{
			LMatrix4 Matrix;

			LVector3 X, Y, Z;
			// make rotation matrix

			// Z vector
			Z = Eye - Center;
			Z.Normalize();

			// Y vector
			Y = Up;

			// X vector = Y cross Z
			X = Y.Cross( Z );

			// Recompute Y = Z cross X
			Y = Z.Cross( X );

			// cross product gives area of parallelogram, which is < 1.0 for
			// non-perpendicular unit-length vectors; so normalize x, y here
			X.Normalize();
			Y.Normalize();

			Matrix[0][0] = X.X;
			Matrix[1][0] = X.Y;
			Matrix[2][0] = X.Z;
			Matrix[3][0] = -X.Dot( Eye );
			Matrix[0][1] = Y.X;
			Matrix[1][1] = Y.Y;
			Matrix[2][1] = Y.Z;
			Matrix[3][1] = -Y.Dot( Eye );
			Matrix[0][2] = Z.X;
			Matrix[1][2] = Z.Y;
			Matrix[2][2] = Z.Z;
			Matrix[3][2] = -Z.Dot( Eye );
			Matrix[0][3] = 0;
			Matrix[1][3] = 0;
			Matrix[2][3] = 0;
			Matrix[3][3] = 1.0f;

			return Matrix;
		}

		LMatrix4 LookAtStereo( LVector3 Eye, LVector3 Center, LVector3 Up, float IOD, bool WhichEye )
		{
			const LVector3 ViewDirection = ( Center - Eye ).GetNormalized();
			const LVector3 Right         = ( ViewDirection.Cross( Up ) ).GetNormalized();

			LVector3 StereoOffset = ( WhichEye ? 0.5f : -0.5f ) * Right * IOD;
			LVector3 NewEye = Eye + StereoOffset;

			return LookAt( NewEye, NewEye + ViewDirection, Up );
		}

		LMatrix4 LookAtStereoMatrix( const LMatrix4& View, float IOD, bool WhichEye )
		{
//			const LVector3 ViewDirection = ( Center - Eye ).GetNormalized();
//			const LVector3 Right         = ( ViewDirection.Cross(Up) ).GetNormalized();
//			LVector3 StereoOffset = ( WhichEye ? 0.5f : -0.5f ) * Right * IOD;
//			LVector3 NewEye = Eye + StereoOffset;
//			return LookAt( NewEye, NewEye + ViewDirection, Up );

			const LVector3 Right         = GetSideDirection( View );

			LVector3 StereoOffset = ( WhichEye ? 0.5f : -0.5f ) * Right * IOD;

			return LMatrix4::GetTranslateMatrix( StereoOffset ) * View;
		}

		LVector3 GetViewingDirection( const LMatrix4& View )
		{
			return LVector3( -View[1][0],
			                 View[0][0],
			                 View[2][0] );
		}

		LVector3 GetSideDirection( const LMatrix4& View )
		{
			return LVector3( -View[1][2],
			                 View[0][2],
			                 View[2][2] );
		}

		LMatrix4 ProjectReflectionTexture( const LMatrix4& Projection, const LMatrix4& ModelView )
		{
			LMatrix4 Scale;
			LMatrix4 Translate = LMatrix4::GetTranslateMatrix( LVector3( 0.5f, 0.5f, 0.5f ) );
			/*
			    glLoadIdentity;
			    glTranslatef(0.5, 0.5, 0);
			    glScalef(0.5, 0.5, 0);
			    // Important: aspect ratio must be that of the main window, not that of the texture.
			    gluPerspective(60, ClientWidth/ClientHeight, 0.01, 100000);
			    glMultMatrixf(@cM);
			    glScalef(1, -1, 1);
			*/
			Scale.ScaleMatrix( LVector3( 0.5f, 0.5f, 0.5f ) );

			LMatrix4 TexProjectionMatrix = ModelView * ( Projection * ( Scale * Translate ) );

			return TexProjectionMatrix;
		}

		LVector3 ProjectPoint( LVector3 Point, const LMatrix4& Projection, const LMatrix4& ModelView )
		{
			LVector4 R( Point, 1.0f );

			// eye coordinates
			R = ModelView * R;
			// clip coordinates
			R = Projection * R;
			// NDC
// R /= R.W;
			// screen coordinates
			R.Y = -R.Y;
			R.X = ( R.X + 1.0f ) * 0.5f;
			R.Y = ( R.Y + 1.0f ) * 0.5f;

			return R.ToVector3();
		}

		LVector3 UnProjectPoint( LVector3 Point, const LMatrix4& Projection, const LMatrix4& ModelView )
		{
			LVector4 R( Point, 1.0f );

			R.X = 2.0f * R.X - 1.0f;
			R.Y = 2.0f * R.Y - 1.0f;
			R.Y = -R.Y;

			R.Z = 1.0f;

			R = Projection.GetInversed() * R;

			R = ModelView.GetInversed() * R;

			return R.ToVector3();
		}

		LVector3 ProjectPointNDC( const LVector3& Point, const LMatrix4& Projection, const LMatrix4& ModelView )
		{
			LVector4 R( Point, 1.0f );

			// eye coordinates
			R = ModelView * R;
			// clip coordinates
			R = Projection * R;

			R /= R.W;

			return R.ToVector3();
		}

		LVector3 UnProjectPointNDC( const LVector3& Point, const LMatrix4& Projection, const LMatrix4& ModelView )
		{
			LVector4 R( Point, 1.0f );

			R = Projection.GetInversed() * R;

			R = ModelView.GetInversed() * R;

			R /= R.W;

			return R.ToVector3();
		}

		LMatrix4 ObliqueReflectionProjection( const LMatrix4& Projection, const LVector4& ClipPlane )
		{
			LMatrix4 Out = Projection;

			LVector3 PlaneNormal = ClipPlane.ToVector3();
			LVector3 SNormal( 0.0f, 0.0f, 0.0f );
			/*
			   if (PlaneNormal.X < 0.0f) SNormal.X = -1.0f;
			   if (PlaneNormal.Y < 0.0f) SNormal.Y = -1.0f;
			   if (PlaneNormal.Z < 0.0f) SNormal.Z = -1.0f;

			   if (PlaneNormal.X > 0.0f) SNormal.X = 1.0f;
			   if (PlaneNormal.Y > 0.0f) SNormal.Y = 1.0f;
			   if (PlaneNormal.Z > 0.0f) SNormal.Z = 1.0f;
			*/
//   LVector4 Q = LVector4( SNormal, 1.0f );
//   Q = Projection.GetInversed() * Q;
			LVector4 Q;

			Q.X = ( fsign( ClipPlane.X ) + Projection.ToFloatPtr()[8] ) / Projection.ToFloatPtr()[0];
			Q.Y = ( fsign( ClipPlane.Y ) + Projection.ToFloatPtr()[9] ) / Projection.ToFloatPtr()[5];

			Q.Z = -1.0f;
			Q.W = ( 1.0f + Projection.ToFloatPtr()[10] ) / Projection.ToFloatPtr()[14];

//   float C = 2.0f / ( PlaneNormal * LVector3(Q.X, Q.Y, Q.Z) );
//   LVector4 CP = LVector4(PlaneNormal, ClipPlane.W + Math::EPSILON ) * C;

			LVector4 CP = ClipPlane * ( 2.0f / ClipPlane.Dot( Q ) );

			Out.ToFloatPtr()[2] = CP.X;
			Out.ToFloatPtr()[6] = CP.Y;
			Out.ToFloatPtr()[10] = CP.Z + 1.0f;
			Out.ToFloatPtr()[14] = CP.W;

			return Out;
		}

		void TransformRayToCoordinates( const LVector3& P, const LVector3& A, const LMatrix4& Transform, LVector3& TransP, LVector3& TransA )
		{
			// transform P to TransP
			TransP = Transform * P;

			// rotate A to TransA
			TransA = Transform.ExtractMatrix3() * A;
		}

		LMatrix4    GetClosestViewForAABB( const LVector3& MinV, const LVector3& MaxV, const LMatrix4& Proj, const LVector3& Eye, const LVector3& Up )
		{
			( void )Proj;
			LVector3 Center = 0.5f * ( MinV + MaxV );

			return LookAt( Eye, Center, Up );
		}

	} // namespace Math
} // namespace Linderdaum


/*
 * 17/01/2011
     LookAtStereo()
     PerspectiveStereo()
 * 14/01/2010
     ProjectPointNDC()
     UnProjectPointNDC()
 * 28/02/2008
     It's here
*/
