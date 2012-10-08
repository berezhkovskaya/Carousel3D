/*
   (Part of Linderdaum Engine)
   Version 0.5.72
   (12/03/2007)
   (C) Kosarevsky Sergey, 2005-2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "LMatrix.h"
#include "LMath.h"

//
// LMatrix3
//

const LMatrix3 Mat3( LVector3( 1, 0, 0 ),
                     LVector3( 0, 1, 0 ),
                     LVector3( 0, 0, 1 ) );

const LMatrix3& LMatrix3::Identity()
{
	return Mat3;
}

void LMatrix3::Inverse()
{
	LMatrix3 Inverse;

	Inverse[0][0] = FMatrix[1][1] * FMatrix[2][2] - FMatrix[1][2] * FMatrix[2][1];
	Inverse[1][0] = FMatrix[1][2] * FMatrix[2][0] - FMatrix[1][0] * FMatrix[2][2];
	Inverse[2][0] = FMatrix[1][0] * FMatrix[2][1] - FMatrix[1][1] * FMatrix[2][0];

	float Determinant = FMatrix[0][0] * Inverse[0][0] + FMatrix[0][1] * Inverse[1][0] + FMatrix[0][2] * Inverse[2][0];

	if ( fabsf( Determinant ) < ::Linderdaum::Math::EPSILON )
	{
		return;
	}

	float InvDeterminant = 1.0f / Determinant;

	Inverse[0][1] = FMatrix[0][2] * FMatrix[2][1] - FMatrix[0][1] * FMatrix[2][2];
	Inverse[0][2] = FMatrix[0][1] * FMatrix[1][2] - FMatrix[0][2] * FMatrix[1][1];
	Inverse[1][1] = FMatrix[0][0] * FMatrix[2][2] - FMatrix[0][2] * FMatrix[2][0];
	Inverse[1][2] = FMatrix[0][2] * FMatrix[1][0] - FMatrix[0][0] * FMatrix[1][2];
	Inverse[2][1] = FMatrix[0][1] * FMatrix[2][0] - FMatrix[0][0] * FMatrix[2][1];
	Inverse[2][2] = FMatrix[0][0] * FMatrix[1][1] - FMatrix[0][1] * FMatrix[1][0];

	FMatrix[0][0] = Inverse[0][0] * InvDeterminant;
	FMatrix[0][1] = Inverse[0][1] * InvDeterminant;
	FMatrix[0][2] = Inverse[0][2] * InvDeterminant;

	FMatrix[1][0] = Inverse[1][0] * InvDeterminant;
	FMatrix[1][1] = Inverse[1][1] * InvDeterminant;
	FMatrix[1][2] = Inverse[1][2] * InvDeterminant;

	FMatrix[2][0] = Inverse[2][0] * InvDeterminant;
	FMatrix[2][1] = Inverse[2][1] * InvDeterminant;
	FMatrix[2][2] = Inverse[2][2] * InvDeterminant;
}

void LMatrix3::Orthonormalize()
{
	FMatrix[0].Normalize();
	FMatrix[2] = FMatrix[0].Cross( FMatrix[1] );
	FMatrix[2].Normalize();
	FMatrix[1] = FMatrix[2].Cross( FMatrix[0] );
	FMatrix[1].Normalize();
}

LMatrix3 LMatrix3::GetInversed() const
{
	LMatrix3 Mat = ( *this );

	Mat.Inverse();

	return Mat;
}

LMatrix3 LMatrix3::GetTransposed() const
{
	LMatrix3 Mat;

	for ( int i = 0; i != 3; i++ )
	{
		for ( int j = 0; j != 3; j++ )
		{
			Mat[ i ][ j ] = FMatrix[ j ][ i ];
		}
	}

	return Mat;
}

//
// LMatrix4
//

const LMatrix4 Mat4( LVector4( 1, 0, 0, 0 ),
                     LVector4( 0, 1, 0, 0 ),
                     LVector4( 0, 0, 1, 0 ),
                     LVector4( 0, 0, 0, 1 ) );

const LMatrix4& LMatrix4::Identity()
{
	return Mat4;
}

LMatrix4 LMatrix4::IdentityStatic()
{
	return LMatrix4( LVector4( 1, 0, 0, 0 ),
	                 LVector4( 0, 1, 0, 0 ),
	                 LVector4( 0, 0, 1, 0 ),
	                 LVector4( 0, 0, 0, 1 ) );
}

LMatrix4 LMatrix4::GetDiagonalMatrix( float Diag )
{
	return LMatrix4( vec4( Diag, 0.0f, 0.0f, 0.0f ), vec4( 0.0f, Diag, 0.0f, 0.0f ), vec4( 0.0f, 0.0f, Diag, 0.0f ), vec4( 0.0f, 0.0f, 0.0f, Diag ) );
}

LMatrix4 LMatrix4::GetDiagonalMatrixV( const LVector4& Diag )
{
	return LMatrix4( vec4( Diag.X, 0.0f, 0.0f, 0.0f ), vec4( 0.0f, Diag.Y, 0.0f, 0.0f ), vec4( 0.0f, 0.0f, Diag.Z, 0.0f ), vec4( 0.0f, 0.0f, 0.0f, Diag.W ) );
}

LMatrix4 LMatrix4::GetTranslateMatrix( const LVector3& Vector )
{
	LMatrix4 Mat;

	Mat.TranslateMatrix( Vector );

	return Mat;
}

LMatrix4 LMatrix4::GetScaleMatrix( const LVector3& Vector )
{
	LMatrix4 Mat;

	Mat.ScaleMatrix( Vector );

	return Mat;
}

LMatrix4 LMatrix4::GetFromPitchPanRoll( float Pitch, float Pan, float Roll )
{
	LMatrix4 Mat;

	Mat.FromPitchPanRoll( Pitch, Pan, Roll );

	return Mat;
}

LMatrix4 LMatrix4::GetRotateMatrixAxis( const float Angle, const LVector3& Axis )
{
	LMatrix4 Mat;

	Mat.RotateMatrixAxis( Angle, Axis );

	return Mat;
}

bool LMatrix4::IsIdentityMatrix() const
{
	const float* M1Ptr = ToFloatPtr();
	const float* M2Ptr = ::Linderdaum::Math::IdentityMatrix4().ToFloatPtr();

	for ( int i = 0; i != 16; ++i )
	{
		if ( M1Ptr[i] != M2Ptr[i] )
		{
			return false;
		}
	}

	return true;
}

LMatrix4 LMatrix4::GetInversed() const
{
	LMatrix4 Mat = ( *this );

	Mat.Inverse();

	return Mat;
}

LMatrix3 LMatrix4::ToMatrix3() const
{
	LMatrix3 Mat;

	Mat[0] = FMatrix[0].ToVector3();
	Mat[1] = FMatrix[1].ToVector3();
	Mat[2] = FMatrix[2].ToVector3();

	return Mat;
}

void LMatrix4::Inverse()
{
	// 2x2 sub-determinants required to calculate 4x4 Determinant
	float Det2_01_01 = FMatrix[0][0] * FMatrix[1][1] - FMatrix[0][1] * FMatrix[1][0];
	float Det2_01_02 = FMatrix[0][0] * FMatrix[1][2] - FMatrix[0][2] * FMatrix[1][0];
	float Det2_01_03 = FMatrix[0][0] * FMatrix[1][3] - FMatrix[0][3] * FMatrix[1][0];
	float Det2_01_12 = FMatrix[0][1] * FMatrix[1][2] - FMatrix[0][2] * FMatrix[1][1];
	float Det2_01_13 = FMatrix[0][1] * FMatrix[1][3] - FMatrix[0][3] * FMatrix[1][1];
	float Det2_01_23 = FMatrix[0][2] * FMatrix[1][3] - FMatrix[0][3] * FMatrix[1][2];

	// 3x3 sub-determinants required to calculate 4x4 Determinant
	float Det3_201_012 = FMatrix[2][0] * Det2_01_12 - FMatrix[2][1] * Det2_01_02 + FMatrix[2][2] * Det2_01_01;
	float Det3_201_013 = FMatrix[2][0] * Det2_01_13 - FMatrix[2][1] * Det2_01_03 + FMatrix[2][3] * Det2_01_01;
	float Det3_201_023 = FMatrix[2][0] * Det2_01_23 - FMatrix[2][2] * Det2_01_03 + FMatrix[2][3] * Det2_01_02;
	float Det3_201_123 = FMatrix[2][1] * Det2_01_23 - FMatrix[2][2] * Det2_01_13 + FMatrix[2][3] * Det2_01_12;

	float Determinant = ( - Det3_201_123 * FMatrix[3][0] + Det3_201_023 * FMatrix[3][1] - Det3_201_013 * FMatrix[3][2] + Det3_201_012 * FMatrix[3][3] );

	if ( fabsf( Determinant ) < ::Linderdaum::Math::EPSILON )
	{
		return;
	}

	float InvDeterminant = 1.0f / Determinant;

	// remaining 2x2 sub-determinants
	float Det2_03_01 = FMatrix[0][0] * FMatrix[3][1] - FMatrix[0][1] * FMatrix[3][0];
	float Det2_03_02 = FMatrix[0][0] * FMatrix[3][2] - FMatrix[0][2] * FMatrix[3][0];
	float Det2_03_03 = FMatrix[0][0] * FMatrix[3][3] - FMatrix[0][3] * FMatrix[3][0];
	float Det2_03_12 = FMatrix[0][1] * FMatrix[3][2] - FMatrix[0][2] * FMatrix[3][1];
	float Det2_03_13 = FMatrix[0][1] * FMatrix[3][3] - FMatrix[0][3] * FMatrix[3][1];
	float Det2_03_23 = FMatrix[0][2] * FMatrix[3][3] - FMatrix[0][3] * FMatrix[3][2];

	float Det2_13_01 = FMatrix[1][0] * FMatrix[3][1] - FMatrix[1][1] * FMatrix[3][0];
	float Det2_13_02 = FMatrix[1][0] * FMatrix[3][2] - FMatrix[1][2] * FMatrix[3][0];
	float Det2_13_03 = FMatrix[1][0] * FMatrix[3][3] - FMatrix[1][3] * FMatrix[3][0];
	float Det2_13_12 = FMatrix[1][1] * FMatrix[3][2] - FMatrix[1][2] * FMatrix[3][1];
	float Det2_13_13 = FMatrix[1][1] * FMatrix[3][3] - FMatrix[1][3] * FMatrix[3][1];
	float Det2_13_23 = FMatrix[1][2] * FMatrix[3][3] - FMatrix[1][3] * FMatrix[3][2];

	// remaining 3x3 sub-determinants
	float Det3_203_012 = FMatrix[2][0] * Det2_03_12 - FMatrix[2][1] * Det2_03_02 + FMatrix[2][2] * Det2_03_01;
	float Det3_203_013 = FMatrix[2][0] * Det2_03_13 - FMatrix[2][1] * Det2_03_03 + FMatrix[2][3] * Det2_03_01;
	float Det3_203_023 = FMatrix[2][0] * Det2_03_23 - FMatrix[2][2] * Det2_03_03 + FMatrix[2][3] * Det2_03_02;
	float Det3_203_123 = FMatrix[2][1] * Det2_03_23 - FMatrix[2][2] * Det2_03_13 + FMatrix[2][3] * Det2_03_12;

	float Det3_213_012 = FMatrix[2][0] * Det2_13_12 - FMatrix[2][1] * Det2_13_02 + FMatrix[2][2] * Det2_13_01;
	float Det3_213_013 = FMatrix[2][0] * Det2_13_13 - FMatrix[2][1] * Det2_13_03 + FMatrix[2][3] * Det2_13_01;
	float Det3_213_023 = FMatrix[2][0] * Det2_13_23 - FMatrix[2][2] * Det2_13_03 + FMatrix[2][3] * Det2_13_02;
	float Det3_213_123 = FMatrix[2][1] * Det2_13_23 - FMatrix[2][2] * Det2_13_13 + FMatrix[2][3] * Det2_13_12;

	float Det3_301_012 = FMatrix[3][0] * Det2_01_12 - FMatrix[3][1] * Det2_01_02 + FMatrix[3][2] * Det2_01_01;
	float Det3_301_013 = FMatrix[3][0] * Det2_01_13 - FMatrix[3][1] * Det2_01_03 + FMatrix[3][3] * Det2_01_01;
	float Det3_301_023 = FMatrix[3][0] * Det2_01_23 - FMatrix[3][2] * Det2_01_03 + FMatrix[3][3] * Det2_01_02;
	float Det3_301_123 = FMatrix[3][1] * Det2_01_23 - FMatrix[3][2] * Det2_01_13 + FMatrix[3][3] * Det2_01_12;

	FMatrix[0][0] = - Det3_213_123 * InvDeterminant;
	FMatrix[1][0] = + Det3_213_023 * InvDeterminant;
	FMatrix[2][0] = - Det3_213_013 * InvDeterminant;
	FMatrix[3][0] = + Det3_213_012 * InvDeterminant;

	FMatrix[0][1] = + Det3_203_123 * InvDeterminant;
	FMatrix[1][1] = - Det3_203_023 * InvDeterminant;
	FMatrix[2][1] = + Det3_203_013 * InvDeterminant;
	FMatrix[3][1] = - Det3_203_012 * InvDeterminant;

	FMatrix[0][2] = + Det3_301_123 * InvDeterminant;
	FMatrix[1][2] = - Det3_301_023 * InvDeterminant;
	FMatrix[2][2] = + Det3_301_013 * InvDeterminant;
	FMatrix[3][2] = - Det3_301_012 * InvDeterminant;

	FMatrix[0][3] = - Det3_201_123 * InvDeterminant;
	FMatrix[1][3] = + Det3_201_023 * InvDeterminant;
	FMatrix[2][3] = - Det3_201_013 * InvDeterminant;
	FMatrix[3][3] = + Det3_201_012 * InvDeterminant;
}

LMatrix3 LMatrix4::ExtractMatrix3() const
{
	LMatrix3 Res;

	for ( int i = 0 ; i < 3 ; i++ )
	{
		for ( int j = 0; j < 3 ; j++ )
		{
			Res[i][j] = FMatrix[i][j];
		}
	}

	return Res;
}

void LMatrix4::SetSubMatrix( const LMatrix3& Mtx )
{
	for ( int i = 0 ; i < 3 ; i++ )
	{
		for ( int j = 0; j < 3 ; j++ )
		{
			FMatrix[i][j] = Mtx[i][j];
		}
	}
}

LMatrix4 LMatrix4::FromPitchPanRoll( float Pitch, float Pan, float Roll )
{
	LMatrix4 RollMtx;

	RollMtx.IdentityMatrix();
	RollMtx.RotateMatrixAxis( Roll * ::Linderdaum::Math::DTOR, LVector3( 0, 1, 0 ) );

	LMatrix4 PanMtx;

	PanMtx.IdentityMatrix();
	PanMtx.RotateMatrixAxis( Pan * ::Linderdaum::Math::DTOR, LVector3( 1, 0, 0 ) );

	LMatrix4 PitchMtx;

	PitchMtx.IdentityMatrix();
	PitchMtx.RotateMatrixAxis( Pitch * ::Linderdaum::Math::DTOR, LVector3( 0, 0, 1 ) );

	*this = PitchMtx * PanMtx * RollMtx;

	return *this;
}

template<class T> T det33( T a11, T a12, T a13,
                           T a21, T a22, T a23,
                           T a31, T a32, T a33 )
{
	return ( a11 * ( a22 * a33 - a23 * a32 ) - a12 * ( a21 * a33 - a31 * a23 ) + a13 * ( a21 * a32 - a22 * a31 ) );
}

float LMatrix4::Det() const
{
	float d1, d2, d3, d4;

	d1 = det33( FMatrix[1][1], FMatrix[1][2], FMatrix[1][3],
	            FMatrix[2][1], FMatrix[2][2], FMatrix[2][3],
	            FMatrix[3][1], FMatrix[3][2], FMatrix[3][3] );

	d2 = det33( FMatrix[1][0], FMatrix[1][2], FMatrix[0][3],
	            FMatrix[2][0], FMatrix[2][2], FMatrix[2][3],
	            FMatrix[3][0], FMatrix[3][2], FMatrix[3][3] );

	d3 = det33( FMatrix[1][0], FMatrix[1][1], FMatrix[0][3],
	            FMatrix[2][0], FMatrix[2][1], FMatrix[2][3],
	            FMatrix[3][0], FMatrix[3][1], FMatrix[3][3] );

	d4 = det33( FMatrix[1][0], FMatrix[1][1], FMatrix[0][2],
	            FMatrix[2][0], FMatrix[2][1], FMatrix[2][2],
	            FMatrix[3][0], FMatrix[3][1], FMatrix[3][2] );

	return ( FMatrix[0][0] * d1 - FMatrix[0][1] * d2 + FMatrix[0][2] * d3 - FMatrix[0][3] * d4 );
}


// Jacobi rotations for the 3x3 matrix. From Numerical Recipes.
// returns the number of iterations
int JacobiRotations3( LMatrix3& A, LVector3* V, float* D )
{
	int nrot = 0;

	int i, j, iq, ip;
	float tresh, theta, tau, t, sm, s, h, g, c;
	float b[3], z[3];

	for ( ip = 0; ip < 3 ; ip++ )
	{
		for ( iq = 0; iq < 3 ; iq++ )
		{
			V[ip][iq] = 0.0f;
		}

		V[ip][ip] = 1.0f;
	}

	for ( ip = 0; ip < 3; ip++ )
	{
		D[ip] = b[ip] = A[ip][ip];

		z[ip] = 0.0;
	}

	nrot = 0;

	for ( i = 0 ; i < 50 ; i++ )
	{
		// sum absolute values of the off-diagonal terms
		sm = fabs( A[0][1] ) + fabs( A[0][2] ) + fabs( A[1][2] );

		if ( sm == 0.0 )
		{
			break;
		}

		if ( i < 4 )
		{
			tresh = 0.2f * sm / 9.0f;
		}
		else
		{
			tresh = 0.0f;
		}

		for ( ip = 0 ; ip < 2 ; ip++ )
		{
			for ( iq = ip + 1 ; iq < 3 ; iq++ )
			{
				g = 100.0f * fabs( A[ip][iq] );

				if ( ( i > 4 ) && ( ( fabs( D[ip] ) + g ) == fabs( D[ip] ) )
				     && ( ( fabs( D[iq] ) + g ) == fabs( D[iq] ) ) )
				{
					A[ip][iq] = 0.0f;
				}

				if ( fabs( A[ip][iq] ) > tresh )
				{
					h = D[iq] - D[ip];

					if ( ( fabs( h ) + g ) == fabs( h ) )
					{
						t = A[ip][iq] / h;
					}
					else
					{
						theta = 0.5f * h / A[ip][iq];
						t = 1.0f / ( fabs( theta ) + sqrt( 1.0f + theta * theta ) );

						if ( theta < 0.0f )
						{
							t = -t;
						}
					}

					c = 1.0f / sqrt( 1 + t * t );
					s = t * c;
					tau = s / ( 1.0f + c );
					h = t * A[ip][iq];
					z[ip] = z[ip] - h;
					z[iq] = z[iq] + h;

					D[ip] = D[ip] - h;
					D[iq] = D[iq] + h;

					A[ip][iq] = 0.0f;

					for ( j = 0 ; j < ip ; j++ )
					{
						g = A[j][ip];
						h = A[j][iq];
						A[j][ip] = g - s * ( h + g * tau );
						A[j][iq] = h + s * ( g - h * tau );
					}

					for ( j = ip + 1 ; j < iq ; j++ )
					{
						g = A[ip][j];
						h = A[j][iq];

						A[ip][j] = g - s * ( h + g * tau );
						A[j][iq] = h + s * ( g - h * tau );
					}

					for ( j = iq + 1 ; j < 3 ; j++ )
					{
						g = A[ip][j];
						h = A[iq][j];

						A[ip][j] = g - s * ( h + g * tau );
						A[iq][j] = h + s * ( g - h * tau );
					}

					for ( j = 0 ; j < 3 ; j++ )
					{
						g = V[j][ip];
						h = V[j][iq];

						V[j][ip] = g - s * ( h + g * tau );
						V[j][iq] = h + s * ( g - h * tau );
					}

					++nrot;
				}
			}
		}
	}

	for ( ip = 0 ; ip < 3 ; ip++ )
	{
		b[ip] += z[ip];
		D[ip]  = b[ip];
		z[ip]  = 0.0f;
	}

	return nrot;
}

void LMatrix3::CalculateEigenVectors( LVector3* V, float* D ) const
{
	LMatrix3 A = *this;

	int n = JacobiRotations3( A, V, D );

	if ( n > 49 )
	{
		// no convergence
		V[1] = vec3( 1, 0, 0 );
		V[2] = vec3( 0, 1, 0 );
		V[3] = vec3( 0, 0, 1 );
	}
}

/*
 * 07/03/2010
    JacobiRotations, CalcEigenVectors
 * 12/03/2007
     FromPitchPanRoll()
     GetPPRMatrix()
 * 05/06/2006
     Inverse()
     GetInversed()
 * 30/04/2006
     Cleaned
 * 02/03/2005
     LMatrix4 multiplication
 * 16/12/2004
     Perspective()
     Frustum()
     LookAt()
 * 15/12/2004
     It's here
*/
