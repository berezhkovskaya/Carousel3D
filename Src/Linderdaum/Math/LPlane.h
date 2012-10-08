/**
 * \file LPlane.h
 * \brief Plane
 * \version 0.5.91
 * \date 15/09/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LPlane_
#define _LPlane_

#include "LMatrix.h"

enum LClassify
{
	IN_FRONT = 0,
	IN_BACK  = 1,
	IN_BOTH  = 2,
	IN_PLANE = 3
};

/// 3D plane representation
class LPlane
{
public:
	LPlane() : FPlaneEquation( 1.0f, 0.0f, 0.0f, 0.0f )
	{
		FPlaneEquationDbl[0] = 0.0;
		FPlaneEquationDbl[1] = 0.0;
		FPlaneEquationDbl[2] = 0.0;
		FPlaneEquationDbl[3] = 0.0;
	}

	/// construct a plane from a plane equation
	LPlane( float A, float B, float C, float D ) : FPlaneEquation( LVector4( A, B, C, D ) )
	{
		FPlaneEquationDbl[0] = A;
		FPlaneEquationDbl[1] = B;
		FPlaneEquationDbl[2] = C;
		FPlaneEquationDbl[3] = D;
	};

	/// From normal and parameter
	explicit LPlane( const LVector3& Normal, float D )
	{
		FPlaneEquation.X = Normal.X;
		FPlaneEquation.Y = Normal.Y;
		FPlaneEquation.Z = Normal.Z;
		FPlaneEquation.W = D;
	}

	/// From point and normal
	explicit LPlane( const LVector3& Point, const LVector3& Normal )
	{
		FPlaneEquation.X = Normal.X;
		FPlaneEquation.Y = Normal.Y;
		FPlaneEquation.Z = Normal.Z;
		FPlaneEquation.W = -( Point.Dot( Normal ) );
	}

	/// construct a plane from a plane equation
	explicit LPlane( const LVector4& Vector ) : FPlaneEquation( Vector )
	{
		FPlaneEquationDbl[0] = FPlaneEquation.X;
		FPlaneEquationDbl[1] = FPlaneEquation.Y;
		FPlaneEquationDbl[2] = FPlaneEquation.Z;
		FPlaneEquationDbl[3] = FPlaneEquation.W;
	};

	/// construct a plane from 3 points
	LPlane( const LVector3& Pt1, const LVector3& Pt2, const LVector3& Pt3 )
	{
		LVector3 Normal = ( Pt2 - Pt1 ).Cross( Pt3 - Pt1 );

		Normal.Normalize();

		FPlaneEquation = LVector4( Normal.X, Normal.Y, Normal.Z, -Normal.Dot( Pt1 ) );

		FPlaneEquationDbl[0] = FPlaneEquation.X;
		FPlaneEquationDbl[1] = FPlaneEquation.Y;
		FPlaneEquationDbl[2] = FPlaneEquation.Z;
		FPlaneEquationDbl[3] = FPlaneEquation.W;
	};
	//
	// LPlane
	//
	inline const LVector4&    GetEquation() const
	{
		return FPlaneEquation;
	};
	inline LVector4&          GetEquation() { return FPlaneEquation; }

	inline const float*       ToFloatPtr() const { return FPlaneEquation.ToFloatPtr(); }
	inline float*             ToFloatPtr() { return FPlaneEquation.ToFloatPtr(); }
	inline const LVector4*    ToVec4Ptr() const { return &FPlaneEquation; }
	inline LVector4*          ToVec4Ptr() { return &FPlaneEquation; }
	inline const double*      ToDoublePtr() const { return FPlaneEquationDbl; }
	inline double*            ToDoublePtr() { return FPlaneEquationDbl; }

	inline bool  IsEqual( const LPlane& Other ) const;
	inline bool  IsEqualDbl( const LPlane& Other, double Delta ) const;
	inline bool  IsEqualEps( const LPlane& Other, float Delta ) const;

	inline float GetSignedDistanceToPoint( const LVector3& P ) const { return FPlaneEquation.ToVector3().Dot( P ) + FPlaneEquation.W; }
	inline float GetDistanceToPoint( const LVector3& P ) const { return fabs( FPlaneEquation.ToVector3().Dot( P ) + FPlaneEquation.W ); }

	/// Construct the reflection matrix for this plane
	inline LMatrix4 Mirror() const
	{
		LVector4 p = FPlaneEquation;

		LMatrix4 T;

		T[0] = LVector4( -2.0f * p.X * p.X + 1.0f, -2.0f * p.Y * p.X,        -2.0f * p.Z * p.X,        0.0f );
		T[1] = LVector4( -2.0f * p.X * p.Y,        -2.0f * p.Y * p.Y + 1.0f, -2.0f * p.Z * p.Y,        0.0f );
		T[2] = LVector4( -2.0f * p.X * p.Z,        -2.0f * p.Y * p.Z,        -2.0f * p.Z * p.Z + 1.0f, 0.0f );
		T[3] = LVector4( -2.0f * p.X * p.W,        -2.0f * p.Y * p.W,        -2.0f * p.Z * p.W,        1.0f );

		return T;
	}

	inline void BuildBasis( LVector3* V1, LVector3* V2 ) const
	{
		*V1 = GetNormal().OrthogonalVector();
		*V2 = GetNormal().Cross( *V1 ).GetNormalized();
	}

	inline LVector3 GetNormal() const { return LVector3( FPlaneEquation.X, FPlaneEquation.Y, FPlaneEquation.Z ); }
	inline float    GetD() const { return FPlaneEquation.W; }

	/// the intersection of N-ray with this plane
	inline LVector3 GetPointOnPlane() const
	{
		LVector3 N = GetNormal();
		return GetD() * N / N.Dot( N );
	}

	/// Project vector's ends to plane's normal
	inline LVector3    ProjectToNormal( const LVector3& V ) const { return ( V * GetNormal() ) * GetNormal(); }

	/// Project vector's ends to plane
	inline LVector3    ProjectPoint( const LVector3& P ) const { return P - ProjectToNormal( P )  + GetD() * GetNormal(); }

	/// Classify the location of point respectively to this plane (front, back, in-plane)
	LClassify          ClassifyPoint( const LVector3& P ) const
	{
		float D = GetSignedDistanceToPoint( P );

		if ( D > Linderdaum::Math::EPSILON ) { return IN_FRONT; }

		if ( D < Linderdaum::Math::EPSILON ) { return IN_BACK; }

		return IN_PLANE;
	}

	/** Compute in what subspace N is pointing to:
	        Nx Ny NZ
	     0  +  +  +
	     1  -  +  +
	     2  +  -  +
	     3  -  -  +
	     4  +  +  -
	     5  -  +  -
	     6  +  -  -
	     7  -  -  -
	*/
	inline static int    ComputeNearPointMask( const LVector3& N )
	{
		// ternary cascade :)
		return ( N.X > 0.0f ) ? ( ( N.Y > 0.0f ) ? ( ( N.Z > 0.0f ) ? 0 : 4 ) :
			                          ( ( N.Z > 0.0f ) ? 2 : 6 ) ) :
			       ( ( N.Y > 0.0f ) ? ( ( N.Z > 0.0f ) ? 1 : 5 ) :
			         ( ( N.Z > 0.0f ) ? 3 : 7 ) );
	}
private:
	LVector4    FPlaneEquation;
	double      FPlaneEquationDbl[4];
};

bool LPlane::IsEqual( const LPlane& Other ) const
{
	return FPlaneEquation == Other.FPlaneEquation;
}

bool LPlane::IsEqualEps( const LPlane& Other, float Eps ) const
{
	return ( fabs( FPlaneEquation[0] - Other.FPlaneEquation[0] ) < Eps ) &&
	       ( fabs( FPlaneEquation[1] - Other.FPlaneEquation[1] ) < Eps ) &&
	       ( fabs( FPlaneEquation[2] - Other.FPlaneEquation[2] ) < Eps ) &&
	       ( fabs( FPlaneEquation[3] - Other.FPlaneEquation[3] ) < Eps );
}

bool LPlane::IsEqualDbl( const LPlane& Other, double Eps ) const
{
	return ( fabs( FPlaneEquationDbl[0] - Other.FPlaneEquationDbl[0] ) < Eps ) &&
	       ( fabs( FPlaneEquationDbl[1] - Other.FPlaneEquationDbl[1] ) < Eps ) &&
	       ( fabs( FPlaneEquationDbl[2] - Other.FPlaneEquationDbl[2] ) < Eps ) &&
	       ( fabs( FPlaneEquationDbl[3] - Other.FPlaneEquationDbl[3] ) < Eps );
}

#endif

/*
 * 15/09/2010
     IsEqualDbl()
     ComputeNearPointMask()
     BuildBasis() accepts LVector3*
 * 11/09/2010
     ClassifyPoint()
     GetDistanceToPoint()
     GetSignedDistanceToPoint()
 * 05/06/2006
     It's here
*/
