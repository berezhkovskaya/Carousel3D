/**
 * \file LVector.h
 * \brief 2D/3D/4D vectors
 * \version 0.6.00
 * \date 12/01/2011
 * \author Sergey Kosarevsky, 2004-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LVector_
#define _LVector_

#include <math.h>

class LVector2;
class LVector3;
class LVector4;
class LVector2i;

inline LVector2  operator*( const float A, const LVector2&  B );
inline LVector2i operator*( const int A,   const LVector2i& B );

inline LVector3 operator*( const float A, const LVector3& B );
inline LVector3 operator/( const float A, const LVector3& B );
inline LVector4 operator*( const float A, const LVector4& B );

/// 2D vector
class LVector2
{
public:
	union
	{
		float X;
		float x;
	};
	union
	{
		float Y;
		float y;
	};
public:
	LVector2(): X( 0 ), Y( 0 ) {};
	LVector2( float lx, float ly ): X( lx ), Y( ly ) {};
	LVector2( int lx, int ly ): X( static_cast<float>( lx ) ),
		Y( static_cast<float>( ly ) ) {};
	explicit LVector2( const float lx ): X( lx ), Y( lx ) {};
	explicit LVector2( const LVector2i& Vec );
	//
	// LVector2
	//
	inline float        operator[]( const int Index ) const { return ( &X )[Index]; };
	inline float&       operator[]( const int Index ) { return ( &X )[Index]; };
	inline LVector2     operator-() const { return LVector2( -X, -Y ); }

	inline LVector2     operator-( const LVector2& Vec ) const { return LVector2( X - Vec.X, Y - Vec.Y ); }
	inline LVector2     operator+( const LVector2& Vec ) const { return LVector2( X + Vec.X, Y + Vec.Y ); }

	inline LVector2     operator*( const float A ) const { return LVector2( X * A, Y * A ); }
	inline LVector2     operator/( const float A ) const { return LVector2( X / A, Y / A ); }

	inline LVector2&    operator+=( const LVector2& Vec )
	{
		X += Vec.X;
		Y += Vec.Y;

		return *this;
	}

	inline LVector2&    operator-=( const LVector2& Vec )
	{
		X -= Vec.X;
		Y -= Vec.Y;

		return *this;
	}

	inline LVector2&    operator*=( const float A )
	{
		X *= A;
		Y *= A;

		return *this;
	}

	inline bool  operator == ( const LVector2& Vec ) const { return ( Vec.X == X ) && ( Vec.Y == Y ); }
	inline bool  operator != ( const LVector2& Vec ) const { return ( Vec.X != X ) || ( Vec.Y != Y ); }

	inline const float* ToFloatPtr() const { return &X; };
	inline float*       ToFloatPtr() { return &X; };

	/// Dot product
	inline float        Dot( const LVector2& Vec ) const { return ( X * Vec.X + Y * Vec.Y ); }

	/// Interpolate vetween two vectors
	void         Lerp( const LVector2& Vec1, const LVector2& Vec2, float Factor )
	{
		if ( Factor <= 0.0f )
		{
			( *this ) = Vec1;
		}
		else if ( Factor >= 1.0f )
		{
			( *this ) = Vec2;
		}
		else
		{
			( *this ) = Vec1 + Factor * ( Vec2 - Vec1 );
		}
	}

	/// Safe 2d vector normalization
	void         Normalize();

	inline LVector2 GetNormalized() const
	{
		LVector2 Vec( *this );
		Vec.Normalize();
		return Vec;
	};

	/// Calculate reflection vector (valid only for unit vectors)
	void Reflect( const LVector2& Normal )
	{
		( *this ) -=  ( 2.0f * Dot( Normal ) ) * Normal;
	}

	/// Get reflection vector (valid only for unit vectors)
	inline LVector2 GetReflected( const LVector2& Normal ) const
	{
		LVector2 Vec( *this );
		Vec.Reflect( Normal );
		return Vec;
	}

	/// Get orthogonal vector
	inline LVector2 GetOrthogonal() const { return LVector2( -Y, X ); }

	/// Euclidean length
	inline float        Length() const { return sqrt( X * X + Y * Y ); };

	/// Squared length (for faster distance comparison)
	inline float        SqrLength() const { return X * X + Y * Y; };
};

/// 2D integer vector
class LVector2i
{
public:
	union
	{
		int X;
		int x;
	};
	union
	{
		int Y;
		int y;
	};
public:
	LVector2i(): X( 0 ), Y( 0 ) {};
	LVector2i( int lx, int ly ): X( lx ), Y( ly ) {};
	explicit LVector2i( const int lx ): X( lx ), Y( lx ) {};
	//
	// LVector2i
	//
	inline int  operator[]( const int Index ) const { return ( &X )[Index]; };
	inline int& operator[]( const int Index ) { return ( &X )[Index]; };

	inline LVector2i operator-( const LVector2i& Vec ) const { return LVector2i( X - Vec.X, Y - Vec.Y ); }
	inline LVector2i operator+( const LVector2i& Vec ) const { return LVector2i( X + Vec.X, Y + Vec.Y ); }

	inline bool operator==( const LVector2i& Vec ) const { return ( Vec.X == X ) && ( Vec.Y == Y ); }
	inline bool operator!=( const LVector2i& Vec ) const { return ( Vec.X != X ) || ( Vec.Y != Y ); }

	inline LVector2i operator*( const int A ) const { return LVector2i( X * A, Y * A ); }
	inline LVector2i operator/( const int A ) const { return LVector2i( X / A, Y / A ); }

	inline const int*    ToIntPtr() const { return &X; };
	inline int*          ToIntPtr() { return &X; };
	inline float         Length() const { return sqrt( static_cast<float>( X * X + Y * Y ) ); };
	inline int           SqrLength() const { return X * X + Y * Y; };
};

/// 3D vector
class LVector3
{
public:
	union
	{
		float X;
		float x;
	};
	union
	{
		float Y;
		float y;
	};
	union
	{
		float Z;
		float z;
	};
public:
	LVector3(): X( 0 ), Y( 0 ), Z( 0 ) {};
	LVector3( float lx, float ly, float lz ): X( lx ), Y( ly ), Z( lz ) {};
	LVector3( int lx, int ly, int lz ): X( static_cast<float>( lx ) ),
		Y( static_cast<float>( ly ) ),
		Z( static_cast<float>( lz ) ) {};
	LVector3( const LVector2& Vec, const float lz ): X( Vec.X ), Y( Vec.Y ), Z( lz ) {};
	explicit LVector3( const float lx ): X( lx ), Y( lx ), Z( lx ) {};
	//
	// LVector3
	//
	inline float  operator[]( const int Index ) const { return ( &X )[Index]; }

	inline float& operator[]( const int Index ) { return ( &X )[Index]; }

	inline LVector3 operator-() const { return LVector3( -X, -Y, -Z ); }
	inline LVector3 operator+() const { return LVector3( +X, +Y, +Z ); }

	inline LVector3 operator*( const float A ) const { return LVector3( X * A, Y * A, Z * A ); }
	inline LVector3 operator/( const float A ) const { return LVector3( X / A, Y / A, Z / A ); }

	inline LVector3 operator/( const LVector3& Vec ) const { return LVector3( X / Vec.X, Y / Vec.Y, Z / Vec.Z ); }

	inline LVector3 operator-( const LVector3& Vec ) const { return LVector3( X - Vec.X, Y - Vec.Y, Z - Vec.Z ); }
	inline LVector3 operator+( const LVector3& Vec ) const { return LVector3( X + Vec.X, Y + Vec.Y, Z + Vec.Z ); }

	LVector3& operator*=( const float A )
	{
		X *= A;
		Y *= A;
		Z *= A;

		return *this;
	}

	LVector3& operator/=( const float A )
	{
		// NO CHECKS HERE: maxumym speed.

		/// We DO check for zero manually in all the cases

//		if ( A > ::Linderdaum::Math::EPSILON )
		{
			X /= A;
			Y /= A;
			Z /= A;
		}

		return *this;
	}

	LVector3& operator+=( const LVector3& Vec )
	{
		X += Vec.X;
		Y += Vec.Y;
		Z += Vec.Z;

		return *this;
	}

	LVector3& operator-=( const LVector3& Vec )
	{
		X -= Vec.X;
		Y -= Vec.Y;
		Z -= Vec.Z;

		return *this;
	}

	inline bool operator==( const LVector3& Vec ) const { return ( Vec.X == X ) && ( Vec.Y == Y ) && ( Vec.Z == Z ); }
	inline bool operator!=( const LVector3& Vec ) const { return ( Vec.X != X ) || ( Vec.Y != Y ) || ( Vec.Z != Z ); }

	/// Per-component multiplication
	inline LVector3  operator*( const LVector3& Vec ) const { return LVector3( X * Vec.X, Y * Vec.Y, Z * Vec.Z ); }

	inline float Dot( const LVector3& Vec ) const { return ( X * Vec.X + Y * Vec.Y + Z * Vec.Z ); }
	inline LVector3 Cross( const LVector3& Vec ) const { return LVector3( Y * Vec.Z - Z * Vec.Y, Z * Vec.X - X * Vec.Z, X * Vec.Y - Y * Vec.X ); }

	LVector3      OrthogonalVector() const
	{
		LVector3 Result = *this;

		Result.Normalize();

		return Result.Cross( Result + LVector3( 1.0f, 2.0f, 3.0f ) );
	}

	inline const float* ToFloatPtr() const { return &X; }
	inline float*       ToFloatPtr()       { return &X; }

	inline LVector2     ToVector2() const  { return LVector2( X, Y ); }

	void Lerp( const LVector3& Vec1, const LVector3& Vec2, float Factor )
	{
		if ( Factor <= 0.0f )
		{
			( *this ) = Vec1;
		}
		else if ( Factor >= 1.0f )
		{
			( *this ) = Vec2;
		}
		else
		{
			( *this ) = Vec1 + Factor * ( Vec2 - Vec1 );
		}
	}

	void     Normalize();

	inline   bool IsZeroVector( float Eps ) const { return ( fabs( X ) < Eps && fabs( Y ) < Eps && fabs( Z ) < Eps ); }

	inline LVector3 GetNormalized() const
	{
		LVector3 Vec( *this );
		Vec.Normalize();
		return Vec;
	};

	inline float Length() const { return sqrt( X * X + Y * Y + Z * Z ); }
	inline float SqrLength() const { return X * X + Y * Y + Z * Z; }

	/// Get the zero-based index of this vector's maximum component
	inline int GetMaximumComponentIndex() const { return ( X > Y ) ? ( ( X > Z ) ? 0 : 2 ) : ( ( Y > Z ) ? 1 : 2 ); }

	/// Get the zero-based index of this vector's minimum component
	inline int GetMinimumComponentIndex() const { return ( X < Y ) ? ( ( X < Z ) ? 0 : 2 ) : ( ( Y < Z ) ? 1 : 2 ); }

	/// Calculate reflection vector (valid only for unit vectors)
	void Reflect( const LVector3& Normal ) { ( *this ) -=  ( 2.0f * Dot( Normal ) ) * Normal; }

	/// Get reflection vector (valid only for unit vectors)
	inline LVector3 GetReflected( const LVector3& Normal ) const
	{
		LVector3 Vec( *this );
		Vec.Reflect( Normal );
		return Vec;
	}
};

/// 3D integer vector
class LVector3i
{
public:
	union
	{
		int X;
		int x;
	};
	union
	{
		int Y;
		int y;
	};
	union
	{
		int Z;
		int z;
	};
public:
	LVector3i(): X( 0 ), Y( 0 ), Z( 0 ) {};
	LVector3i( int lx, int ly, int lz ): X( lx ), Y( ly ), Z( lz ) {};
	explicit LVector3i( const int lx ): X( lx ), Y( lx ), Z( lx ) {};
	//
	// LVector3i
	//
	inline int  operator[]( const int Index ) const { return ( &X )[Index]; }
	inline int& operator[]( const int Index )       { return ( &X )[Index]; }

	inline LVector3i operator-( const LVector3i& Vec ) const { return LVector3i( X - Vec.X, Y - Vec.Y, Z - Vec.Z ); }
	inline LVector3i operator+( const LVector3i& Vec ) const { return LVector3i( X + Vec.X, Y + Vec.Y, Z + Vec.Z ); }

	inline bool operator==( const LVector3i& Vec ) const { return ( Vec.X == X ) && ( Vec.Y == Y ) && ( Vec.Z == Z ); }
	inline bool operator!=( const LVector3i& Vec ) const { return ( Vec.X != X ) || ( Vec.Y != Y ) || ( Vec.Z != Z ); }

	inline LVector3i operator*( const int A ) const { return LVector3i( X * A, Y * A, Z * A ); }
	inline LVector3i operator/( const int A ) const { return LVector3i( X / A, Y / A, Z / A ); }

	inline const int*    ToIntPtr() const { return &X; }
	inline int*          ToIntPtr()       { return &X; }

	inline float         Length() const { return sqrt( static_cast<float>( X * X + Y * Y + Z * Z ) ); }
	inline int           SqrLength() const { return X * X + Y * Y + Z * Z; }
};

/// 4D int vector
class LVector4i
{
public:
	union
	{
		int X;
		int x;
	};
	union
	{
		int Y;
		int y;
	};
	union
	{
		int Z;
		int z;
	};
	union
	{
		int W;
		int w;
	};
public:
	LVector4i(): X( 0 ), Y( 0 ), Z( 0 ), W( 0 ) {};
	LVector4i( int lx, int ly, int lz, int lw ): X( lx ), Y( ly ), Z( lz ), W( lw ) {};
	explicit LVector4i( const int lx ): X( lx ), Y( lx ), Z( lx ), W( lx ) {};
	//
	// LVector4i
	//
	inline int  operator[]( const int Index ) const { return ( &X )[Index]; }
	inline int& operator[]( const int Index )       { return ( &X )[Index]; }

	inline bool operator==( const LVector4i& Vec ) const { return ( Vec.X == X ) && ( Vec.Y == Y ) && ( Vec.Z == Z ) && ( Vec.W == W ); }
	inline bool operator!=( const LVector4i& Vec ) const { return ( Vec.X != X ) || ( Vec.Y != Y ) || ( Vec.Z != Z ) || ( Vec.W != W ); }

	inline LVector2i    XY() const { return LVector2i( X, Y ); }
	inline LVector2i    YX() const { return LVector2i( Y, X ); }
	inline LVector3i    XYZ() const { return LVector3i( X, Y, Z ); }
	inline LVector3i    ZYX() const { return LVector3i( Z, Y, X ); }
};

/// 4D vector
class LVector4
{
public:
	union
	{
		float X;
		float x;
	};
	union
	{
		float Y;
		float y;
	};
	union
	{
		float Z;
		float z;
	};
	union
	{
		float W;
		float w;
	};
public:
	LVector4(): X( 0 ), Y( 0 ), Z( 0 ), W( 0 ) {};
	LVector4( float lx, float ly, float lz, float lw ): X( lx ), Y( ly ), Z( lz ), W( lw ) {};
	LVector4( int lx, int ly, int lz, int lw ): X( static_cast<float>( lx ) ),
		Y( static_cast<float>( ly ) ),
		Z( static_cast<float>( lz ) ),
		W( static_cast<float>( lw ) ) {};
	LVector4( const LVector3& Vec, const float lw ): X( Vec.X ), Y( Vec.Y ), Z( Vec.Z ), W( lw ) {};
	explicit LVector4( const float lx ): X( lx ), Y( lx ), Z( lx ), W( lx ) {};
	explicit LVector4( const LVector2& Vec ): X( Vec.X ), Y( Vec.Y ), Z( 0 ), W( 0 ) {};
	explicit LVector4( const LVector3& Vec ): X( Vec.X ), Y( Vec.Y ), Z( Vec.Z ), W( 0 ) {};
	//
	// LVector4
	//
	inline float     operator[]( const int Index ) const { return ( &X )[Index]; }
	inline float&    operator[]( const int Index )       { return ( &X )[Index]; }

	inline LVector4  operator-( const LVector4& Vec ) const { return LVector4( X - Vec.X, Y - Vec.Y, Z - Vec.Z, W - Vec.W ); }
	inline LVector4  operator+( const LVector4& Vec ) const { return LVector4( X + Vec.X, Y + Vec.Y, Z + Vec.Z, W + Vec.W ); }

	inline LVector4  operator*( const float A ) const { return LVector4( X * A, Y * A, Z * A, W * A ); }
	inline LVector4  operator/( const float A ) const { return LVector4( X / A, Y / A, Z / A, W / A ); }

	inline LVector4& operator*=( const float A )
	{
		X *= A;
		Y *= A;
		Z *= A;
		W *= A;

		return *this;
	}

	inline LVector4& operator/=( const float A )
	{
		// We DO check for zero explicitly. This operator is used only three times in the engine
//		if ( A > ::Linderdaum::Math::EPSILON )
		{
			X /= A;
			Y /= A;
			Z /= A;
			W /= A;
		}

		return *this;
	}

	inline LVector4& operator -= ( const LVector4& Vec )
	{
		X -= Vec.X;
		Y -= Vec.Y;
		Z -= Vec.Z;
		W -= Vec.W;

		return *this;
	}

	inline LVector4& operator += ( const LVector4& Vec )
	{
		X += Vec.X;
		Y += Vec.Y;
		Z += Vec.Z;
		W += Vec.W;

		return *this;
	}

	inline bool operator==( const LVector4& Vec ) const { return ( Vec.X == X ) && ( Vec.Y == Y ) && ( Vec.Z == Z ) && ( Vec.W == W ); }
	inline bool operator!=( const LVector4& Vec ) const { return ( Vec.X != X ) || ( Vec.Y != Y ) || ( Vec.Z != Z ) || ( Vec.W != W ); }

	/// Per-component multiplication
	inline LVector4 operator*( const LVector4& Vec ) const { return LVector4( X * Vec.X, Y * Vec.Y, Z * Vec.Z, W * Vec.W ); }

	/// Dot product
	inline float Dot( const LVector4& Vec ) const { return ( X * Vec.X + Y * Vec.Y + Z * Vec.Z + W * Vec.W ); }

	inline const  float* ToFloatPtr() const { return &X; }
	inline float*        ToFloatPtr()       { return &X; }

	inline LVector2      ToVector2() const  { return LVector2( X, Y ); }
	inline LVector3      ToVector3() const  { return LVector3( X, Y, Z ); }

	void         Lerp( const LVector4& Vec1, const LVector4& Vec2, float Factor )
	{
		if ( Factor <= 0.0f )
		{
			( *this ) = Vec1;
		}
		else if ( Factor >= 1.0f )
		{
			( *this ) = Vec2;
		}
		else
		{
			( *this ) = Vec1 + Factor * ( Vec2 - Vec1 );
		}
	}

	void         Normalize();

	inline float Length() const { return sqrt( X * X + Y * Y + Z * Z + W * W ); }
	inline float SqrLength() const { return X * X + Y * Y + Z * Z + W * W; }

	inline bool  IsZeroVector( float Eps ) const { return ( fabs( X ) < Eps && fabs( Y ) < Eps && fabs( Z ) < Eps && fabs( W ) < Eps ); }

	/// swizzlers
	inline LVector4    BGRA() const { return LVector4( Z, Y, X, W ); };

	inline LVector2    XY() const { return LVector2( X, Y ); }
	inline LVector2    YX() const { return LVector2( Y, X ); }
	inline LVector3    XYZ() const { return LVector3( X, Y, Z ); }
	inline LVector3    ZYX() const { return LVector3( Z, Y, X ); }
};

inline LVector2  operator*( const float A, const LVector2&  B ) { return LVector2 ( B.X * A, B.Y * A ); }
inline LVector2i operator*( const int A,   const LVector2i& B ) { return LVector2i( B.X * A, B.Y * A ); }

inline LVector3 operator*( const float A, const LVector3& B ) { return LVector3( B.X * A, B.Y * A, B.Z * A ); }
inline LVector3 operator/( const float A, const LVector3& B ) { return LVector3( B.X / A, B.Y / A, B.Z / A ); }
inline LVector4 operator*( const float A, const LVector4& B ) { return LVector4( B.X * A, B.Y * A, B.Z * A, B.W * A ); }

/// Utility methods

inline float LVector3_MixedProduct( const LVector3& A, const LVector3& B, const LVector3& C )
{
	return A.Dot( B.Cross( C ) );
}

inline bool LVector3_AreCollinear( const LVector3& A, const LVector3& B, const LVector3& C, float Eps )
{
	return ( B - A ).Cross( C - A ).SqrLength() < Eps /*::Linderdaum::Math::EPSILON*/;
}

inline bool LVector3_AreComplanar( const LVector3& A, const LVector3& B, const LVector3& C, const LVector3& D, float Eps )
{
	return fabs( LVector3_MixedProduct( B - A, C - A, D - A ) ) < Eps /*::Linderdaum::Math::EPSILON*/;
}

// shortcuts
typedef LVector2 vec2;
typedef LVector3 vec3;
typedef LVector4 vec4;

#endif

/*
 * 12/01/2011
     Lo-case xyzw fields
 * 03/11/2010
     LVector4i::operator==
     LVector4i::operator!=
 * 16/09/2010
     LVector3::Reflect()
     LVector3::GetReflected()
     LVector3::AreComplanar()
     LVector3::AreCollinear()
     LVector3::MixedProduct()
 * 03/04/2010
     Operator * changed to per-component multiplication
     LVector3::Dot()
     LVector4::Dot()
 * 22/03/2010
     LVector2i
 * 03/03/2010
     Additional operators added
 * 25/02/2007
     ToString()
 * 03/06/2006
     Lerp()
 * 01/06/2006
     Non-const version of ToFloatPtr()
 * 10/05/2005
     LVector2(float)
     LVector2(int, int)
     LVector3(int, int, int)
 * 15/12/2004
     It's here
*/
