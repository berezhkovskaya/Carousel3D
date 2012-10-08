/**
 * \file LAABB.h
 * \brief Axis-aligned bounding box
 * \version 0.5.91
 * \date 11/09/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LAABB_
#define _LAABB_

#include "LPlane.h"

class LBox;
class LSphere;

namespace Linderdaum
{
	namespace Math
	{
		class LPolyList;
	};
};

/// Axis-aligned bounding box
class LAABoundingBox
{
public:
	LVector3    FMin;
	LVector3    FMax;
public:
	LAABoundingBox(): FMin( +::Linderdaum::Math::INFINITY ),
		FMax( -::Linderdaum::Math::INFINITY ) {};
	LAABoundingBox( const LVector3& Min,
	                const LVector3& Max )
	{
		FMin.X = Linderdaum::Math::LMin( Min.X, Max.X );
		FMin.Y = Linderdaum::Math::LMin( Min.Y, Max.Y );
		FMin.Z = Linderdaum::Math::LMin( Min.Z, Max.Z );
		FMax.X = Linderdaum::Math::LMax( Min.X, Max.X );
		FMax.Y = Linderdaum::Math::LMax( Min.Y, Max.Y );
		FMax.Z = Linderdaum::Math::LMax( Min.Z, Max.Z );
	};
	//
	// LAABoundingBox
	//
	void        Reset()
	{
		FMin = LVector3( +::Linderdaum::Math::INFINITY );
		FMax = LVector3( -::Linderdaum::Math::INFINITY );
	};
	void        Grow( float DeltaSize );
	void        Combine( const LAABoundingBox& Box );
	void        CombinePoint( const LVector3& Point );
	bool        Intersect( const LAABoundingBox& Box ) const;
	LClassify   IntersectPlane( const LPlane& Plane ) const;
	bool        IntersectRay( const LVector3& Origin,
	                          const LVector3& Ray,
	                          LVector3& Point1,
	                          LVector3& Point2 ) const;
	bool        IsPointInside( const LVector3& Point ) const;
	void        Transform( const LMatrix4& Transform );
	LVector3    GetCornerPoint( int N ) const;
	float       MinDistanceToCorner( const LVector3& Point ) const;
	float       MaxDistanceToCorner( const LVector3& Point ) const;
	inline bool     IsEmpty() const
	{
		return FMin.X > FMax.X || FMin.Y > FMax.Y || FMin.Z > FMax.Z;
	};
	inline float    GetSizeX() const
	{
		return FMax.X - FMin.X;
	};
	inline float    GetSizeY() const
	{
		return FMax.Y - FMin.Y;
	};
	inline float    GetSizeZ() const
	{
		return FMax.Z - FMin.Z;
	};
	inline LVector3 GetSize() const
	{
		return LVector3( GetSizeX(), GetSizeY(), GetSizeZ() );
	};
	inline LVector3 GetCenter() const
	{
		return 0.5f * ( FMin + FMax );
	};
	inline int      GetPointsNum() const
	{
		return 8;
	};
	inline void     Lerp( const LAABoundingBox& Box1, const LAABoundingBox& Box2, float Factor )
	{
		FMin.Lerp( Box1.FMin, Box2.FMin, Factor );
		FMax.Lerp( Box1.FMax, Box2.FMax, Factor );
	}

	/// Create circumscribed sphere
	LSphere CircumscribedSphere() const;

	/// Create inscribed sphere
	LSphere InscribedSphere() const;

	/// Clamp/wrap vector coordinates
	LVector3 ClampVectorToBox_Periodic( const LVector3& InVec ) const;

	/// Clamp vector coordinates
	LVector3 ClampVectorToBox( const LVector3& InVec ) const;

	/// Generate a random vector inside this box
	LVector3    RandomVector() const;

	/// Convert to OBB
	LBox ToBox() const;

	/// Build AABB from points
	void FromPoints( int NumPoints, const LVector3* Points );

	/// Create a polygon list for this primitive
	void Facet( Linderdaum::Math::LPolyList* Out ) const;
};

#endif

/*
 * 11/09/2010
     Grow()
     Reset()
     GetSize()
     GetCenter()
     IsEmpty()
     IntersectPlane()
 * 29/05/2010
     IsPointInside()
 * 10/03/2010
     MinDistanceToCorner()
     MaxDistanceToCorner()
 * 12/03/2007
     Intersect()
     GetCornerPoint()
     Transform()
 * 13/02/2007
     IntersectRay()
 * 23/01/2007
     FEmpty removed
 * 07/03/2005
     It's here
*/
