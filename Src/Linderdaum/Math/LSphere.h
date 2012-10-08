/*
   (Part of Linderdaum Engine)
   Version 0.5.93
   (09/10/2010)
   (C) Kosarevsky Sergey, 2007-2010
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _LSphere_
#define _LSphere_

#include "LVector.h"

class LBox;

namespace Linderdaum
{
	namespace Math
	{
		class LPolyList;
	};
};

/// 3D sphere representation
class LSphere
{
public:
	LSphere(): FOrigin(),
		FRadius( 0 ) {};
	explicit LSphere( float Radius ): FOrigin(),
		FRadius( Radius ) {};

	/// Create a sphere from the point cloud
	explicit LSphere( int NumPoints, const LVector3* Points ) { FromPoints( NumPoints, Points ); }

	/// Create a sphere with specifed parameters
	LSphere( const LVector3& Origin, float Radius ): FOrigin( Origin ), FRadius( Radius ) {};

	//
	// LSphere
	//

	inline void               SetOrigin( const LVector3& Origin ) { FOrigin = Origin; }
	inline void               SetRadius( float Radius ) { FRadius = Radius; }
	inline const LVector3&    GetOrigin() const { return FOrigin; }
	inline float              GetRadius() const { return FRadius; }

	bool                      IntersectLine( const LVector3& Point1,
	                                         const LVector3& Point2,
	                                         float* Intersection1,
	                                         float* Intersection2 ) const;

	/// Create circumscribed sphere
	LBox CircumscribedBox() const;

	/// Create inscribed sphere
	LBox InscribedBox() const;

	/// Calculate sphere from points
	void FromPoints( int NumPoints, const LVector3* Points );

	/// Get random vector inside the sphere
	LVector3                  RandomVectorInside() const;

	/// Get random vector on the sphere's surface
	LVector3                  RandomVectorOnSphere() const;

	/// Create polyhedral representation of a sphere
	void Facet( Linderdaum::Math::LPolyList* Out ) const;

private:
	LVector3    FOrigin;
	float       FRadius;
};

class LCircle
{
public:
	LCircle(): FOrigin(),
		FRadius( 0 ) {};
	explicit LCircle( float Radius ): FOrigin(),
		FRadius( Radius ) {};
	LCircle( const LVector2& Origin, float Radius ): FOrigin( Origin ),
		FRadius( Radius ) {};
	//
	// LCircle
	//
	inline void               SetOrigin( const LVector2& Origin ) { FOrigin = Origin; };
	inline void               SetRadius( float Radius ) { FRadius = Radius; };
	inline const LVector2&    GetOrigin() const { return FOrigin; };
	inline float              GetRadius() const { return FRadius; };
private:
	LVector2    FOrigin;
	float       FRadius;
};

#endif

/*
 * 09/10/2010
     LCircle
 * 08/04/2007
     IntersectLine()
 * 24/01/2007
     It's here
*/
