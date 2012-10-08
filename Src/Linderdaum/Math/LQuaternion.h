/*
   (Part of Linderdaum Engine)
   Version 0.5.72
   (12/03/2007)
   (C) Sergey Kosarevsky, 2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _LQuaternion_
#define _LQuaternion_

#include "LMatrix.h"

class LQuaternion;

inline LQuaternion operator*( const LQuaternion& Q1, const LQuaternion& Q2 );
inline LQuaternion operator+( const LQuaternion& Q1, const LQuaternion& Q2 );
inline LQuaternion operator*( const float A, const LQuaternion& B );

/// Quaternion
class LQuaternion
{
public:
	LVector3    FVec;
	float       FW;
public:
	LQuaternion(): FVec( 0.0f ),
		FW( 1.0f ) {};

	LQuaternion( const LQuaternion& q ): FVec( q.FVec.X, q.FVec.Y, q.FVec.Z ), FW( q.FW ) {};

	LQuaternion( float x, float y, float z, float w ): FVec( x, y, z ), FW( w ) {};

	LQuaternion( const LVector3& Vec, float w ): FVec( Vec ),
		FW( w ) {};
	explicit LQuaternion( const LVector4& Vec ): FVec( Vec.ToVector3() ), FW( Vec.W ) {};

	explicit LQuaternion( const LMatrix3& Mtx ) { FromMatrix3( Mtx ); }
	explicit LQuaternion( const LMatrix4& Mtx ) { FromMatrix3( Mtx.ExtractMatrix3() ); }

	//
	// LQuaternion
	//
	inline LQuaternion& Conjugate()
	{
		FVec = -FVec;

		return *this;
	}

	inline LQuaternion&    operator =( const LQuaternion& Q )
	{
		FVec = Q.FVec;
		FW   = Q.FW;

		return *this;
	}

	inline LQuaternion&    operator+=( const LQuaternion& Q )
	{
		FVec += Q.FVec;
		FW   += Q.FW;

		return *this;
	}
	inline LQuaternion&    operator-=( const LQuaternion& Q )
	{
		FVec -= Q.FVec;
		FW   -= Q.FW;

		return *this;
	}
	inline LQuaternion&    operator*=( const LQuaternion& Q )
	{
		LQuaternion Q1 = *this;
		LQuaternion Q2 = Q;

		*this = LQuaternion(
		           Q1.FW * Q2.FVec.X + Q1.FVec.X * Q2.FW     + Q1.FVec.Y * Q2.FVec.Z - Q1.FVec.Z * Q2.FVec.Y,
		           Q1.FW * Q2.FVec.Y + Q1.FVec.Y * Q2.FW     + Q1.FVec.Z * Q2.FVec.X - Q1.FVec.X * Q2.FVec.Z,
		           Q1.FW * Q2.FVec.Z + Q1.FVec.Z * Q2.FW     + Q1.FVec.X * Q2.FVec.Y - Q1.FVec.Y * Q2.FVec.X,
		           Q1.FW * Q2.FW     - Q1.FVec.X * Q2.FVec.X - Q1.FVec.Y * Q2.FVec.Y - Q1.FVec.Z * Q2.FVec.Z );

		return *this;
		/*
		     LVector3 Vec( Q.FVec * FW + Q.FW * FVec + Q.FVec.Cross( FVec ) );

		     FW   = Q.FW * FW - Q.FVec*FVec;
		     FVec = Vec;

		     return *this;
		*/
	}
	inline LQuaternion&    operator*=( const float F )
	{
		FVec *= F;
		FW   *= F;

		return *this;
	}
	inline LQuaternion&    operator/=( const float F )
	{
		const float InvF = 1.0f / F;

		FVec *= InvF;
		FW   *= InvF;

		return *this;
	}

	void        Normalize()
	{
		LVector4 Vec( FVec, FW );

		Vec.Normalize();

		FVec = Vec.ToVector3();
		FW   = Vec.W;
	}

	void        ReNormalizeW()
	{
		float Wr = 1.0f - ( FVec.X * FVec.X ) - ( FVec.Y * FVec.Y ) - ( FVec.Z * FVec.Z );

		FW = ( Wr < 0.0f ) ? 0.0f : -sqrt( Wr );
	}

	LVector3 RotateVector( const LVector3& Vector ) const
	{
		LQuaternion p     ( Vector, 0.0f );
		LQuaternion qConj ( -FVec, FW );

		p  = *this * p * qConj;

		return p.FVec;
	}

	void        IdentityQuaternion()
	{
		FVec = LVector3( 0.0f );
		FW = 1.0f;
	}

	void        FromMatrix3( const LMatrix3& mtx )
	{
		float s0, s1, s2;
		int k0, k1, k2, k3;

		float m00 = mtx[0][0];
		float m11 = mtx[1][1];
		float m22 = mtx[2][2];

		if ( m00 + m11 + m22 > 0.0f )
		{
			k0 = 3;
			k1 = 2;
			k2 = 1;
			k3 = 0;
			s0 = s1 = s2 = 1.0f;
		}
		else if ( m00 > m11 && m00 > m22 )
		{
			k0 = 0;
			k1 = 1;
			k2 = 2;
			k3 = 3;
			s0 = 1.0f;
			s1 = -1.0f;
			s2 = -1.0f;
		}
		else if ( m11 > m22 )
		{
			k0 = 1;
			k1 = 0;
			k2 = 3;
			k3 = 2;
			s0 = -1.0f;
			s1 = 1.0f;
			s2 = -1.0f;
		}
		else
		{
			k0 = 2;
			k1 = 3;
			k2 = 0;
			k3 = 1;
			s0 = -1.0f;
			s1 = -1.0f;
			s2 = 1.0f;
		}

		float t = s0 * m00 + s1 * m11 + s2 * m22 + 1.0f;

		float s = /*ReciprocalSqrt( t )*/ 0.5f / sqrt( t );

		LVector4 vv;
		vv[k0] = s * t;
		vv[k1] = ( mtx[0][1] - s2 * mtx[1][0] ) * s;
		vv[k2] = ( mtx[2][0] - s1 * mtx[0][2] ) * s;
		vv[k3] = ( mtx[1][2] - s0 * mtx[2][1] ) * s;

		FVec.X = vv[0];
		FVec.Y = vv[1];
		FVec.Z = vv[2];
		FW = -vv[3]; // seems to be erroneous...
	}

	LMatrix3    ToMatrix3() const
	{
		LMatrix3 M;

		float wx, wy, wz;
		float xx, yy, yz;
		float xy, xz, zz;
		float x2, y2, z2;

		x2 = FVec.X + FVec.X;
		y2 = FVec.Y + FVec.Y;
		z2 = FVec.Z + FVec.Z;

		xx = FVec.X * x2;
		xy = FVec.X * y2;
		xz = FVec.X * z2;

		yy = FVec.Y * y2;
		yz = FVec.Y * z2;
		zz = FVec.Z * z2;

		wx = FW * x2;
		wy = FW * y2;
		wz = FW * z2;

		M[ 0 ][ 0 ] = 1.0f - ( yy + zz );
		M[ 0 ][ 1 ] = xy - wz;
		M[ 0 ][ 2 ] = xz + wy;

		M[ 1 ][ 0 ] = xy + wz;
		M[ 1 ][ 1 ] = 1.0f - ( xx + zz );
		M[ 1 ][ 2 ] = yz - wx;

		M[ 2 ][ 0 ] = xz - wy;
		M[ 2 ][ 1 ] = yz + wx;
		M[ 2 ][ 2 ] = 1.0f - ( xx + yy );

		return M;
	}

	LVector4    ToVector4() const { return LVector4( FVec, FW ); }

	void        FromAxisAngle( const LVector3& Axis, const float Angle )
	{
		const float HalfAngle = Angle / 2.0f;

		FVec = Axis * sinf( HalfAngle );
		FW = cosf( HalfAngle );
	}

	void        ToAxisAngle( LVector3& Axis, float& Angle ) const
	{
		Angle = 2.0f * acosf( FW );
		Axis = ( FVec.SqrLength() > ::Linderdaum::Math::EPSILON ) ? FVec.GetNormalized() :
		       LVector3( 1.0f, 0.0f, 0.0f );
	}

	/**
	   \brief Spherical linear interpolation

	   Code from http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/index.htm
	*/
	void SLERP( const LQuaternion& qa, const LQuaternion& q2, float t )
	{
		LQuaternion qb = q2;
		// quaternion to return
		LQuaternion qm;

		// Calculate angle between them.
		double cosHalfTheta = qa.FVec.Dot( qb.FVec ) + qa.FW * qb.FW;

		// if qa=qb or qa=-qb then theta = 0 and we can return qa
		if ( fabs( cosHalfTheta ) >= 1.0 )
		{
			*this = qa;
			return;
		}

		if ( cosHalfTheta < 0 )
		{
			qb.FVec = LVector3( -qb.FVec.X, -qb.FVec.Y, -qb.FVec.Z );
			qb.FW = -qb.FW;
			cosHalfTheta = -cosHalfTheta;
		}

		// Calculate temporary values.
		double halfTheta = acos( cosHalfTheta );
		double sinHalfTheta = sqrt( 1.0 - cosHalfTheta * cosHalfTheta );

		// if theta = 180 degrees then result is not fully defined
		// we could rotate around any axis normal to qa or qb
		if ( fabs( sinHalfTheta ) < 0.001 )
		{
			*this = 0.5f * ( qa + qb );

			return;
		}

		float ratioA = static_cast<float>( sin( ( 1 - t ) * halfTheta ) / sinHalfTheta );
		float ratioB = static_cast<float>( sin( t * halfTheta ) / sinHalfTheta );
		//calculate Quaternion.

		*this = ( ratioA * qa + ratioB * qb );
	}
};

inline LQuaternion operator+( const LQuaternion& Q1,
                              const LQuaternion& Q2 )
{
	return LQuaternion( Q1.FVec + Q2.FVec,
	                    Q1.FW + Q2.FW );
}

inline LQuaternion operator-( const LQuaternion& Q1,
                              const LQuaternion& Q2 )
{
	return LQuaternion( Q1.FVec - Q2.FVec,
	                    Q1.FW - Q2.FW );
}

inline LQuaternion operator-( const LQuaternion& Q )
{
	return LQuaternion( -Q.FVec, -Q.FW );
}

inline LQuaternion operator*( const float A, const LQuaternion& B )
{
	return LQuaternion( A * B.FVec, A * B.FW );
}

inline LQuaternion operator*( const LQuaternion& Q1,
                              const LQuaternion& Q2 )
{
	/*
	   return LQuaternion( Q1.FVec * Q2.FW + Q1.FW * Q2.FVec + Q1.FVec.Cross( Q2.FVec ),
	                       Q1.FW * Q2.FW - Q1.FVec*Q2.FVec );
	*/
	return LQuaternion(
	          Q1.FW * Q2.FVec.X + Q1.FVec.X * Q2.FW     + Q1.FVec.Y * Q2.FVec.Z - Q1.FVec.Z * Q2.FVec.Y,
	          Q1.FW * Q2.FVec.Y + Q1.FVec.Y * Q2.FW     + Q1.FVec.Z * Q2.FVec.X - Q1.FVec.X * Q2.FVec.Z,
	          Q1.FW * Q2.FVec.Z + Q1.FVec.Z * Q2.FW     + Q1.FVec.X * Q2.FVec.Y - Q1.FVec.Y * Q2.FVec.X,
	          Q1.FW * Q2.FW     - Q1.FVec.X * Q2.FVec.X - Q1.FVec.Y * Q2.FVec.Y - Q1.FVec.Z * Q2.FVec.Z );

}

typedef LQuaternion quat;

#endif

/*
 * 12/03/2007
     It's here
*/
