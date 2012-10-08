/**
 * \file LTransform.cpp
 * \brief Transformation
 * \version 0.5.91
 * \date 01/03/2010
 * \author Viktor Latypov, 2009-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "LTransform.h"
#include "LProjection.h"

void LTransform::SetPosition( const LVector3& P )
{
	FMatrix[3][0] = P.X;
	FMatrix[3][1] = P.Y;
	FMatrix[3][2] = P.Z;
}

LVector3 LTransform::GetPosition() const
{
	return FMatrix[3].ToVector3();
}

LMatrix4 ComposeTransformation( const LVector3& Pos, const LMatrix4& Rot )
{
	return LMatrix4::GetTranslateMatrix( Pos ) * Rot;
}

LMatrix4 ComposeCameraTransformation( const LVector3& Pos, const LMatrix4& Rot )
{
	return LMatrix4::GetTranslateMatrix( -Pos ) * Rot;
}

// Decompose T to rotation and translation
void DecomposeTransformation( const LMatrix4& T, LVector3& Pos, LMatrix4& Rot )
{
	LMatrix4 InvT = T.GetInversed();

	for ( int i = 0 ; i < 3 ; i++ )
	{
		for ( int j = 0 ; j < 3 ; j++ )
		{
			Rot[i][j] = InvT[j][i];
		}
	}

	Pos = Rot.GetInversed() * vec3( -InvT[3].X, -InvT[3].Y, -InvT[3].Z );
}

// Decompose camera T to translation and rotation
void DecomposeCameraTransformation( const LMatrix4& T, LVector3& Pos, LMatrix4& Rot )
{
	LMatrix4 InvT = T;

	Rot.IdentityMatrix();
	LMatrix4 RotInv;
	RotInv.IdentityMatrix();

	for ( int i = 0 ; i < 3 ; i++ )
	{
		for ( int j = 0 ; j < 3 ; j++ )
		{
			Rot[i][j] = InvT[i][j];
			RotInv[i][j] = InvT[j][i];
		}
	}

// vec3 Pos1 = RotInv * vec3(-InvT[3].X, -InvT[3].Y, -InvT[3].Z);
// vec3 Pos2 = RotInv * vec3(InvT[3].X, InvT[3].Y, InvT[3].Z);
// vec3 Pos3 = Rot * vec3(-InvT[3].X, -InvT[3].Y, -InvT[3].Z);
// vec3 Pos4 = Rot * vec3(InvT[3].X, InvT[3].Y, InvT[3].Z);

	Pos = RotInv * vec3( -InvT[3].X, -InvT[3].Y, -InvT[3].Z );
}


LTransform::LTransform()
{
	FMatrix = LMatrix4::Identity();
	FAngleSystem = Euler_zxys;
}

void LTransform::SetAngles( const LVector3& Angles )
{
	SetAngleTriple( Angles.X, Angles.Y, Angles.Z );
}

void LTransform::SetAngleTriple( float T1, float T2, float T3 )
{
	LMatrix4 M;
	AnglesToMatrix( FAngleSystem, M, T1, T2, T3 );

	for ( int i = 0 ; i < 3 ; i++ )
		for ( int j = 0 ; j < 3 ; j++ )
		{
			FMatrix[i][j] = M[i][j];
		}
}

LVector3 LTransform::GetAngles() const
{
	float T1, T2, T3;

	GetAngleTriple( T1, T2, T3 );

	return LVector3( T1, T2, T3 );
}

void LTransform::GetAngleTriple( float& T1, float& T2, float& T3 ) const
{
	MatrixToAngles( FAngleSystem, FMatrix.ExtractMatrix3(), T1, T2, T3 );
}

void LTransform::SetPositionAndAngles( const LVector3& Pos, float AngleX, float AngleY, float AngleZ )
{
	FMatrix = LMatrix4::GetFromPitchPanRoll( AngleZ, AngleX, AngleY ) * LMatrix4::GetTranslateMatrix( Pos );
}

void LTransform::SetPositionAndAngles( const LVector3& Pos, const LVector3& Angles )
{
	SetPositionAndAngles( Pos, Angles.X, Angles.Y, Angles.Z );
}

void LTransform::SetPositionAndOrientation( const LVector3& pos, const LQuaternion& quat )
{
	FMatrix = LMatrix4( quat.ToMatrix3() ) * LMatrix4::GetTranslateMatrix( pos );
}

void LTransform::SetPositionAndAxisAngle( const LVector3& Pos, const LVector3& Axis, float Angle )
{
	FMatrix = LMatrix4::GetRotateMatrixAxis( Angle, Axis ) * LMatrix4::GetTranslateMatrix( Pos );
}

void LTransform::SetPosMatrixAndAxisAngle( const LMatrix4& Pos, const LVector3& Axis, float Angle )
{
	FMatrix = LMatrix4::GetRotateMatrixAxis( Angle, Axis ) * Pos;
}

LTransform::LTransform( const LVector3& pos, const LQuaternion& quat )
{
	SetPositionAndOrientation( pos, quat );
}

LTransform::LTransform( const LMatrix4& mtx4 )
{
	SetMatrix4( mtx4 );
}

void LTransform::GetPositionAndOrientation( LVector3& Pos, LQuaternion& Q ) const
{
	LMatrix4 Mtx;

	DecomposeTransformation( FMatrix, Pos, Mtx );
	Q.FromMatrix3( Mtx.ExtractMatrix3() );
}

void LTransform::LookAt( const LVector3& From, const LVector3& To, const LVector3& Up )
{
	FMatrix = ::Linderdaum::Math::LookAt( From, To, Up );
}

// Lerp + SLerp between O1 and O2 for t in [0,1]
void LTransform::Interpolate( const LTransform& O1, const LTransform& O2, float t )
{
	LMatrix4 Mtx1, Mtx2;
	LQuaternion Q1, Q2;
	LVector3 Pos1, Pos2;

	DecomposeTransformation( O1.FMatrix, Pos1, Mtx1 );
	DecomposeTransformation( O2.FMatrix, Pos2, Mtx2 );
	Q1.FromMatrix3( Mtx1.ExtractMatrix3() );
	Q2.FromMatrix3( Mtx2.ExtractMatrix3() );

	LQuaternion Q;

	Q.SLERP( Q1, Q2, t );

	LVector3 Pos = ( 1 - t ) * Pos1 + t * Pos2;

	//
	SetPositionAndOrientation( Pos, Q );
}

using namespace Linderdaum::Math;

/// indices of the axes fora given coordinate system
int EulerAxes[12][3] =
{
	{0, 2, 0}, {0, 2, 1}, {0, 1, 0}, {0, 1, 2},
	{1, 0, 1}, {1, 0, 2}, {1, 2, 1}, {1, 2, 0},
	{2, 1, 2}, {2, 1, 0}, {2, 0, 2}, {2, 0, 1}
};

LVector3 EuclideanBasis[3] = { vec3( 1, 0, 0 ), vec3( 0, 1, 0 ), vec3( 0, 0, 1 ) };

/**
   Find a planar rotation (c=cos, s=sin) which maps (a,b) vector to (r,0)
*/
void GivensRotation( float a, float b, float& c, float& s, float& r )
{
	/**
	http://www.cgafaq.info/wiki/Euler_angles_from_matrix

	The following method, choosing either t=cot(theta) or t=tan(theta) as route,
	is a variation of that recommended by Golub and Van Loan, made continuous as suggested by Edward Anderson
	*/

	if ( fabs( b ) < Linderdaum::Math::EPSILON )
	{
		c = Sign( a );
		s = 0.0f;
		r = fabs( a );

		return;
	}

	if ( fabs( a ) < Linderdaum::Math::EPSILON )
	{
		c = 0.0f;
		s = Sign( b );
		r = fabs( b );

		return;
	}

	float t, u;

	if ( fabs( b ) > fabs( a ) )
	{
		t = a / b;
		u = Sign( b ) * sqrt( 1.0f + t * t );
		s = 1.0f / u;
		c = s * t;
		r = b * u;
	}
	else
	{
		t = b / a;
		u = Sign( a ) * sqrt( 1.0f + t * t );
		c = 1.0f / u;
		s = c * t;
		r = a * u;
	}
}

void MatrixToAngles( LEulerAngleSystem Sys, const LMatrix3& M, float& T1, float& T2, float& T3 )
{
	int i, j, k, h;
	int neg, alt, rev;

	// convert integer code I to N,A,R and (i,j,k,h) indices
	int S = static_cast<int>( Sys );

	i = S >> 3;

	neg = ( S >> 2 ) & 0x1;
	alt = ( S >> 1 ) & 0x1;
	rev =  S       & 0x1;

	static int EulerNext[4] = {1, 2, 3, 1};

	j = EulerNext[i + neg];
	k = ( 0 + 1 + 2 )/*(x+y+z)*/ - i - j;
	h = EulerNext[k + ( 1^neg^alt )];

	// now convert the matrix to angles

	LVector3 v = M[i];

	// [a, b] := [v_h, v_k]
	float a = v[h];
	float b = v[k];

	float c, s;

	GivensRotation( a, b, c, s, v[h] );

	float s1 = c * M[j][k] - s * M[j][h];
	float c1 = c * M[k][k] - s * M[k][h];

	T1 = atan2( s1, c1 );
	T2 = atan2( v[j], v[i] );
	T3 = atan2( s, c );

	if ( alt )
	{
		T3 = -T3;
	}

	if ( neg )
	{
		T1 = -T1;
		T2 = -T2;
		T3 = -T3;
	}

	if ( rev )
	{
		/// [T1,T2,T3] -> [T3,T2,T1]
		float TT = T3;
		T3 = T1;
		T1 = TT;
	}
}

void MakeRotationMatrix( LMatrix4& R, int axis, float angle )
{
	int i1 = 1, i2 = 2;

	if ( axis == 0 )
	{
		i1 = 1;
		i2 = 2;
	}

	if ( axis == 1 )
	{
		i1 = 0;
		i2 = 2;
	}

	if ( axis == 2 )
	{
		i1 = 0;
		i2 = 1;
	}

	float s = sin( angle );
	float c = cos( angle );

	R.IdentityMatrix();

	R[i1][i1] = +c;
	R[i1][i2] = -s;
	R[i2][i1] = +s;
	R[i2][i2] = +c;
}

void AnglesToMatrix( LEulerAngleSystem Sys, LMatrix4& M, float T1, float T2, float T3 )
{
	int S = static_cast<int>( Sys );

	int Idx = ( S > 12 ) ? ( S - 12 ) : S;

	float T[3] = {T1, T2, T3};

	LMatrix4 Mt[3];

	for ( int i = 0 ; i < 3 ; i++ )
	{
		MakeRotationMatrix( Mt[i], EulerAxes[Idx][i], T[i] );
	}

	// reverse or direct
	M = ( S > 12 ) ? ( Mt[2] * Mt[1] * Mt[0] ) : ( Mt[0] * Mt[1] * Mt[2] );
}

/*
 * 01/03/2010
     Log section added
*/
