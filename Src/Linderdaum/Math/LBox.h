/**
 * \file LBox.h
 * \brief Oriented bounding box
 * \version 0.5.91
 * \date 16/06/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __LBox__h__included__
#define __LBox__h__included__

#include "Math/LGeomUtils.h"

class LSphere;

namespace Linderdaum
{
	namespace Math
	{
		class LPolyList;
	};
};

/**
   \brief Oriented bounding box used in collision detection and static geometry processing

   The box is defined by three axes, centerr and extents for each axis

   Can be built from an array of 3D points
   Can be transformed by the matrix
*/
class LBox
{
public:

	/// Init the box
	LBox() : FCenter( 0, 0, 0 ), FExtent( 0, 0, 0 )
	{
		FAxis.IdentityMatrix();
	}

	/// Make OBB for the point array
	LBox( int NumPts, const LVector3* Vtxs ) { FromPoints( NumPts, Vtxs ); }

	/// Copy box
	LBox( const LBox& B )
	{
		AssignBox( B );
	}

	/// Assign box
	LBox& operator = ( const LBox& B )
	{
		AssignBox( B );
		return *this;
	}

	// multiply by matrix
	void MultiplyByMatrix( const LMatrix4& Mtx )
	{
		LMatrix3 Rotation = Mtx.ExtractMatrix3();

		FAxis = Rotation * FAxis;

		FCenter = Mtx * FCenter;
	}

	/// Transform the box by some matrix
	LBox operator * ( const LMatrix4& Mtx ) const
	{
		LBox New( *this );

		New.MultiplyByMatrix( Mtx );

		return New;
	}

	/// Create circumscribed sphere
	LSphere CircumscribedSphere() const;

	/// Create inscribed sphere
	LSphere InscribedSphere() const;

	/// Build an OBB from the point cloud (TODO: Convex hull building before OBB fitting)
	inline void FromPoints( int NumPts, const LVector3* Vertices ) { BuildBoundingBox( NumPts, Vertices, FCenter, ( LVector3* )&FAxis, FExtent.ToFloatPtr() ); }

	/// Create polyhedral representation of a box
	void Facet( Linderdaum::Math::LPolyList* Out ) const;

public:
	LVector3 FCenter;
	LMatrix3 FAxis;
	LVector3 FExtent;

private:
	void AssignBox( const LBox& B )
	{
		FCenter = B.FCenter;
		FAxis = B.FAxis;
		FExtent = B.FExtent;
	}
};

#endif

/*
 * 06/01/2011
     BuildFromPoints
 * 16/06/2010
     It's here
*/
