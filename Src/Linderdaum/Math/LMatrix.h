/*
   (Part of Linderdaum Engine)
   Version 0.5.91
   (17/03/2010)
   (C) Kosarevsky Sergey, 2005-2010
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _LMatrix_
#define _LMatrix_

#include "LMath.h"

//
// LMatrix3
//

/// 3x3 matrix
class LMatrix3
{
private:
	LVector3 FMatrix[3];
public:
	inline LMatrix3() {};
	inline explicit LMatrix3( const float A )
	{
		FMatrix[0] = LVector3( A );
		FMatrix[1] = LVector3( A );
		FMatrix[2] = LVector3( A );
	};
	inline explicit LMatrix3( const LVector3& X,
	                          const LVector3& Y,
	                          const LVector3& Z )
	{
		FMatrix[0] = X;
		FMatrix[1] = Y;
		FMatrix[2] = Z;
	};
	//
	// LMatrix3
	//
	inline LVector3&       operator[]( const int Index )
	{
		return FMatrix[Index];
	};
	inline const LVector3& operator[]( const int Index ) const
	{
		return FMatrix[Index];
	};
	inline LMatrix3        operator*( const LMatrix3& Matrix ) const;
	inline LMatrix3        operator+( const LMatrix3& Matrix ) const;
	inline LVector3        operator*( const LVector3& Vector ) const;
	inline void            ZeroMatrix()
	{
		for ( int i = 0; i <= 2; ++i )
		{
			FMatrix[i] = LVector3( 0 );
		}
	};
	inline void            IdentityMatrix()
	{
		ZeroMatrix();

		for ( int i = 0; i <= 2; ++i )
		{
			FMatrix[i][i] = 1.0f;
		}
	};
	inline void            RotateMatrixAxis( const float Angle, const LVector3& Axis );
	inline void            RotateMatrix( const LVector3& V1, const LVector3& V2 );
	void                   Inverse();
	LMatrix3               GetInversed() const;
	LMatrix3               GetTransposed() const;
	inline const float*    ToFloatPtr() const
	{
		return FMatrix[0].ToFloatPtr();
	};
	inline float*          ToFloatPtr()
	{
		return FMatrix[0].ToFloatPtr();
	};
	static const LMatrix3& Identity();

	/// Make an orthogonal matrix out of the given one
	void                   Orthonormalize();
	void                   CalculateEigenVectors( LVector3* V, float* D ) const;
};

LMatrix3 LMatrix3::operator+( const LMatrix3& Matrix ) const
{
	LMatrix3 Result;

	for ( int i = 0; i != 3; ++i )
	{
		for ( int j = 0; j != 3; ++j )
		{
			Result[i][j] = FMatrix[i][j] + Matrix[i][j];
		}
	}

	return Result;
}

LMatrix3 LMatrix3::operator*( const LMatrix3& Matrix ) const
{
	LMatrix3 Result;

	const float* M1Ptr = ToFloatPtr();
	const float* M2Ptr = Matrix.ToFloatPtr();
	float* RPtr        = Result.ToFloatPtr();

	for ( int i = 0; i != 3; ++i )
	{
		for ( int j = 0; j != 3; ++j )
		{
			*RPtr = M1Ptr[0] * M2Ptr[ 0 * 3 + j ] +
			        M1Ptr[1] * M2Ptr[ 1 * 3 + j ] +
			        M1Ptr[2] * M2Ptr[ 2 * 3 + j ];
			RPtr++;
		}

		M1Ptr += 3;
	}

	return Result;
}

LVector3 LMatrix3::operator*( const LVector3& Vector ) const
{
	return LVector3(
	          FMatrix[ 0 ].X * Vector.X + FMatrix[ 1 ].X * Vector.Y + FMatrix[ 2 ].X * Vector.Z,
	          FMatrix[ 0 ].Y * Vector.X + FMatrix[ 1 ].Y * Vector.Y + FMatrix[ 2 ].Y * Vector.Z,
	          FMatrix[ 0 ].Z * Vector.X + FMatrix[ 1 ].Z * Vector.Y + FMatrix[ 2 ].Z * Vector.Z
	       );
}

void LMatrix3::RotateMatrixAxis( const float Angle, const LVector3& Axis )
{
	float CosA = cos( Angle );
	float SinA = sin( Angle );

	LVector3 NAxis = Axis.GetNormalized();

	float Ax = NAxis.X;
	float Ay = NAxis.Y;
	float Az = NAxis.Z;

	float AxAx = Ax * Ax;
	float AxAy = Ax * Ay;
	float AxAz = Ax * Az;

	float AyAx = AxAy;
	float AyAy = Ay * Ay;
	float AyAz = Ay * Az;

	float AzAx = AxAz;
	float AzAy = AyAz;
	float AzAz = Az * Az;

	FMatrix[0][0] = AxAx + ( 1.0f - AxAx ) * CosA;
	FMatrix[0][1] = AxAy * ( 1.0f - CosA ) + Az * SinA;
	FMatrix[0][2] = AxAz * ( 1.0f - CosA ) - Ay * SinA;

	FMatrix[1][0] = AyAx * ( 1.0f - CosA ) - Az * SinA;
	FMatrix[1][1] = AyAy + ( 1.0f - AyAy ) * CosA;
	FMatrix[1][2] = AyAz * ( 1.0f - CosA ) + Ax * SinA;

	FMatrix[2][0] = AzAx * ( 1.0f - CosA ) + Ay * SinA;
	FMatrix[2][1] = AzAy * ( 1.0f - CosA ) - Ax * SinA;
	FMatrix[2][2] = AzAz + ( 1.0f - AzAz ) * CosA;
}

void LMatrix3::RotateMatrix( const LVector3& V1, const LVector3& V2 )
{
	LVector3 Vec1 = V1.GetNormalized();
	LVector3 Vec2 = V2.GetNormalized();

	LVector3 Axis = Vec1.Cross( Vec2 );

	float CosAngle, Angle;

	if ( Axis.Length() == 0 )
	{
		Axis = LVector3( 0.0f, 0.0f, 1.0f );
	}

	CosAngle = Vec1.Dot( Vec2 );

	Angle = acos( CosAngle );

	RotateMatrixAxis( Angle, Axis );
}

//
// LMatrix4
//

/// 4x4 matrix
class LMatrix4
{
private:
	LVector4 FMatrix[4];
public:
	inline LMatrix4() {};

	inline explicit LMatrix4( const float A )
	{
		FMatrix[0] = LVector4( A );
		FMatrix[1] = LVector4( A );
		FMatrix[2] = LVector4( A );
		FMatrix[3] = LVector4( A );
	};

	inline explicit LMatrix4( const LVector4& X,
	                          const LVector4& Y,
	                          const LVector4& Z,
	                          const LVector4& W )
	{
		FMatrix[0] = X;
		FMatrix[1] = Y;
		FMatrix[2] = Z;
		FMatrix[3] = W;
	};

	inline explicit LMatrix4( const LMatrix3& mtx3 )
	{
		for ( int i = 0 ; i < 3 ; i++ ) for ( int j = 0 ; j < 3 ; j++ )
			{
				FMatrix[i][j] = mtx3[i][j];
			}

		FMatrix[0][3] = FMatrix[1][3] = FMatrix[2][3] = 0;
		FMatrix[3][0] = FMatrix[3][1] = FMatrix[3][2] = 0;
		FMatrix[3][3] = 1;
	}
	//
	// LMatrix4
	//
	inline LVector4&       operator[]( const int Index )
	{
		return FMatrix[Index];
	};
	inline const LVector4& operator[]( const int Index ) const
	{
		return FMatrix[Index];
	};
	inline LMatrix4        operator*( const LMatrix4& Matrix ) const;
	inline LVector4        operator*( const LVector4& Vector ) const;
	inline LVector3        operator*( const LVector3& Vector ) const;
	inline void            ZeroMatrix()
	{
		for ( int i = 0; i <= 3; ++i )
		{
			FMatrix[i] = LVector4( 0 );
		}
	};
	inline void            IdentityMatrix()
	{
		ZeroMatrix();

		for ( int i = 0; i <= 3; ++i )
		{
			FMatrix[i][i] = 1.0f;
		}
	};
	inline void            TranslateMatrix( const LVector3& Vector );
	inline void            ScaleMatrix( const LVector3& Vector );
	inline void            RotateMatrixAxis( const float Angle, const LVector3& Axis );
	inline void            RotateMatrix( const LVector3& V1, const LVector3& V2 );
	bool                   IsIdentityMatrix() const;
	void                   Inverse();
	LMatrix4               GetInversed() const;
	inline void            Transpose();
	inline LMatrix4        GetTransposed() const;
	LMatrix3               ExtractMatrix3() const;
	void                   SetSubMatrix( const LMatrix3& Mtx );
	float                  Det() const;
	inline const float*    ToFloatPtr() const
	{
		return FMatrix[0].ToFloatPtr();
	};
	inline float*          ToFloatPtr()
	{
		return FMatrix[0].ToFloatPtr();
	};

	/// Extract 3x3 part
	LMatrix3               ToMatrix3() const;

	/// Obsolete. Use LTransform::euler_angles instead
	LMatrix4               FromPitchPanRoll( float Pitch, float Pan, float Roll );

#pragma region Static constructors for typical matrices

	/// Identity matrix (don't use for global variables initialization)
	static const LMatrix4& Identity();

	/// Identity matrix
	static LMatrix4        IdentityStatic();

	/// Translation matrix
	static LMatrix4        GetTranslateMatrix( const LVector3& Vector );

	/// Scaling matrix
	static LMatrix4        GetScaleMatrix( const LVector3& Vector );

	/// Rotation matrix for (axis,angle) pair
	static LMatrix4        GetRotateMatrixAxis( const float Angle, const LVector3& Axis );

	/// Diag * Identity() matrix
	static LMatrix4        GetDiagonalMatrix( float Diag );

	/// Form diagonal matrix from vector components
	static LMatrix4        GetDiagonalMatrixV( const LVector4& Diag );

	static LMatrix4        GetFromPitchPanRoll( float Pitch, float Pan, float Roll );

#pragma endregion
};

inline bool operator== ( const LMatrix4& M1, const LMatrix4& M2 )
{
	const float* M1Ptr = M1.ToFloatPtr();
	const float* M2Ptr = M2.ToFloatPtr();

	for ( int i = 0; i != 16; ++i )
	{
		if ( M1Ptr[i] != M2Ptr[i] )
		{
			return false;
		}
	}

	return true;
}

inline bool operator!= ( const LMatrix4& M1, const LMatrix4& M2 )
{
	const float* M1Ptr = M1.ToFloatPtr();
	const float* M2Ptr = M2.ToFloatPtr();

	for ( int i = 0; i != 16; ++i )
	{
		if ( M1Ptr[i] != M2Ptr[i] )
		{
			return true;
		}
	}

	return false;
}

LMatrix4 LMatrix4::operator*( const LMatrix4& Matrix ) const
{
	LMatrix4 Result;

	const float* M1Ptr = ToFloatPtr();
	const float* M2Ptr = Matrix.ToFloatPtr();
	float* RPtr        = Result.ToFloatPtr();

//#pragma omp parallel for shared(RPtr)
	for ( int i = 0; i < 4; ++i )
	{
		for ( int j = 0; j < 4; ++j )
		{
			*RPtr = M1Ptr[0] * M2Ptr[ 0 * 4 + j ] +
			        M1Ptr[1] * M2Ptr[ 1 * 4 + j ] +
			        M1Ptr[2] * M2Ptr[ 2 * 4 + j ] +
			        M1Ptr[3] * M2Ptr[ 3 * 4 + j ];
			RPtr++;
		}

		M1Ptr += 4;
	}

	return Result;
}

LVector4 LMatrix4::operator*( const LVector4& Vector ) const
{
	return LVector4(
	          FMatrix[ 0 ].X * Vector.X + FMatrix[ 1 ].X * Vector.Y + FMatrix[ 2 ].X * Vector.Z + FMatrix[ 3 ].X * Vector.W,
	          FMatrix[ 0 ].Y * Vector.X + FMatrix[ 1 ].Y * Vector.Y + FMatrix[ 2 ].Y * Vector.Z + FMatrix[ 3 ].Y * Vector.W,
	          FMatrix[ 0 ].Z * Vector.X + FMatrix[ 1 ].Z * Vector.Y + FMatrix[ 2 ].Z * Vector.Z + FMatrix[ 3 ].Z * Vector.W,
	          FMatrix[ 0 ].W * Vector.X + FMatrix[ 1 ].W * Vector.Y + FMatrix[ 2 ].W * Vector.Z + FMatrix[ 3 ].W * Vector.W
	       );
}

LVector3 LMatrix4::operator*( const LVector3& Vector ) const
{
	return LVector3(
	          FMatrix[ 0 ].X * Vector.X + FMatrix[ 1 ].X * Vector.Y + FMatrix[ 2 ].X * Vector.Z + FMatrix[ 3 ].X,
	          FMatrix[ 0 ].Y * Vector.X + FMatrix[ 1 ].Y * Vector.Y + FMatrix[ 2 ].Y * Vector.Z + FMatrix[ 3 ].Y,
	          FMatrix[ 0 ].Z * Vector.X + FMatrix[ 1 ].Z * Vector.Y + FMatrix[ 2 ].Z * Vector.Z + FMatrix[ 3 ].Z
	       );
}

void LMatrix4::TranslateMatrix( const LVector3& Vector )
{
	IdentityMatrix();

	FMatrix[3] = LVector4( Vector );
	FMatrix[3][3] = 1.0f;
}

void LMatrix4::ScaleMatrix( const LVector3& Vector )
{
	ZeroMatrix();

	FMatrix[0][0] = Vector.X;
	FMatrix[1][1] = Vector.Y;
	FMatrix[2][2] = Vector.Z;
	FMatrix[3][3] = 1.0f;
}

void LMatrix4::RotateMatrixAxis( const float Angle, const LVector3& Axis )
{
	float CosA = cos( Angle );
	float SinA = sin( Angle );

	LVector3 NAxis = Axis.GetNormalized();

	float Ax = NAxis.X;
	float Ay = NAxis.Y;
	float Az = NAxis.Z;

	float AxAx = Ax * Ax;
	float AxAy = Ax * Ay;
	float AxAz = Ax * Az;

	float AyAx = AxAy;
	float AyAy = Ay * Ay;
	float AyAz = Ay * Az;

	float AzAx = AxAz;
	float AzAy = AyAz;
	float AzAz = Az * Az;

	FMatrix[0][0] = AxAx + ( 1.0f - AxAx ) * CosA;
	FMatrix[0][1] = AxAy * ( 1.0f - CosA ) + Az * SinA;
	FMatrix[0][2] = AxAz * ( 1.0f - CosA ) - Ay * SinA;
	FMatrix[0][3] = 0;

	FMatrix[1][0] = AyAx * ( 1.0f - CosA ) - Az * SinA;
	FMatrix[1][1] = AyAy + ( 1.0f - AyAy ) * CosA;
	FMatrix[1][2] = AyAz * ( 1.0f - CosA ) + Ax * SinA;
	FMatrix[1][3] = 0;

	FMatrix[2][0] = AzAx * ( 1.0f - CosA ) + Ay * SinA;
	FMatrix[2][1] = AzAy * ( 1.0f - CosA ) - Ax * SinA;
	FMatrix[2][2] = AzAz + ( 1.0f - AzAz ) * CosA;
	FMatrix[2][3] = 0;

	FMatrix[3][0] = 0;
	FMatrix[3][1] = 0;
	FMatrix[3][2] = 0;
	FMatrix[3][3] = 1.0f;
}

void LMatrix4::RotateMatrix( const LVector3& V1, const LVector3& V2 )
{
	LVector3 Vec1 = V1.GetNormalized();
	LVector3 Vec2 = V2.GetNormalized();

	LVector3 Axis = Vec1.Cross( Vec2 );

	float CosAngle, Angle;

	if ( Axis.Length() == 0 )
	{
		Axis = LVector3( 0.0f, 0.0f, 1.0f );
	}

	CosAngle = Vec1.Dot( Vec2 );

	Angle = acos( CosAngle );

	RotateMatrixAxis( Angle, Axis );
}

/// Associate a skew-symmetric matrix to the vector V
inline LMatrix3 VectorStar( const LVector3& V )
{
	LMatrix3 M;

	M[0][0] = 0.0f;
	M[1][0] = V.X;
	M[2][0] =  V.Z;
	M[0][1] = -V.X;
	M[1][1] = 0.0f;
	M[2][1] = -V.Y;
	M[0][2] = -V.Z;
	M[1][2] = V.Y;
	M[2][2] = 0.0f;

	return M;
}

/// M[i][j] = V1[i] * V2[j]
inline LMatrix3 TensorProduct_VecByVec( const LVector3& V1, const LVector3& V2 )
{
	LMatrix3 M;

	for ( int i = 0 ; i < 3 ; i++ )
	{
		for ( int j = 0 ; j < 3 ; j++ )
		{
			M[i][j] = V1[i] * V2[j];
		}
	}

	return M;
}

inline void  LMatrix4::Transpose()
{
	float t;
	int i, j;

	for ( i = 0 ; i < 4 ; i++ )
		for ( j = 0 ; j < 4 ; j++ )
			{ t = FMatrix[i][j]; FMatrix[i][j] = FMatrix[j][i]; FMatrix[j][i] = t; }
}

inline LMatrix4 LMatrix4::GetTransposed() const
{
	LMatrix4 Res;
	int i, j;

	for ( i = 0 ; i < 4 ; i++ )
		for ( j = 0 ; j < 4 ; j++ )
		{
			Res[i][j] = FMatrix[j][i];
		}

	return Res;
}

typedef LMatrix4 mtx4;
typedef LMatrix3 mtx3;

namespace Linderdaum
{
	namespace Math
	{

		scriptexport( Math ) inline LMatrix4 IdentityMatrix4()
		{
			static LMatrix4 IdMatrix;

			IdMatrix.IdentityMatrix();

			return IdMatrix;
		}

	} // namespace Math
} // namespace Linderdaum

#endif

/*
 * 17/03/2010
     GetTranslateMatrix()
 * 07/04/2007
     ToString()
 * 12/03/2007
     FromPitchPanRoll()
     GetPPRMatrix()
 * 04/02/2007
     LMatrix3
 * 23/01/2007
     Bug fixed in Mtx4 * Vec3 multiplication
 * 05/06/2006
     RotateMatrix()
     RotateMatrixAxis()
 * 01/06/2006
     ScaleMatrix()
     Non-const version of ToFloatPtr()
 * 03/03/2005
     TranslateMatrix()
 * 15/12/2004
     It's here
*/
