/**
 * \file CollisionIntervals.h
 * \brief Collision intervals
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __collisionintervals__h__included__
#define __collisionintervals__h__included__

#include "Core/iObject.h"

class LCollisionShape;

enum LCollisionTestingState
{
	CT_UNDEFINED = 0,  /// not yet checked
	CT_PARTIAL   = 1,  /// partial collision, continue checking
	CT_OVERLAP   = 2   /// Yes, they are colliding
};

/**
   Simple broad-phase collision detection: sweep-and-prune testing
*/

/// A single interval used in sweep-and-prune CD
class scriptfinal LInterval : public iObject
{
public:
	LInterval() { FOwner = NULL; FOwnerID = ""; }
	virtual ~LInterval() {}

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	/* Property(Description="ID of shape represented by this interval", Name=ShapeID, FieldName=FShape, Type=int) */
	/* Property(Description="Min projection value", Name=Min, Type=float, FieldName=FMin) */
	/* Property(Description="Max projection value", Name=Max, Type=float, FieldName=FMax) */
public:
	/// Minimum projection value (precached from FShape)
	float FMin;
	/// Maximum projection value
	float FMax;

	/// Reference to the colliding object (also it is the UID of the interval in AxisList)
	int FShape;

	/// Collision state flag to avoid redundant checkings for each axis
	LCollisionTestingState FCollisionState;

	/// "boolean" flag indicating a collision (if < 0 - then no collision, otherwise - the ID of CollisionShape)
	int CollidesTo;

	/// Name of the owner
	LString FOwnerID;

	/// Owner of this interval
	iObject* FOwner;
};

/// list of spans (boxes projected to axii)
class scriptfinal LAxisList : public iObject
{
public:
	LAxisList();
	virtual ~LAxisList();

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

public:
	/// Calculate Shape's projection and add the corresponding interval to the list
	void Insert( int ShapeIndex, LCollisionShape* Shape );

	/// Remove the colliding entity from processing
	void Remove( int ShapeIndex );

	/// Clear lists
	void Clear();

	/// Check if the interval collides with something other than itself
	bool CollidesWithSomeone( LInterval* Int );

	/// Iterate the intervals and check if they collide with any other ( only O(n) checks, because the checking is strictly sequential )
	void CheckCollisions();

	/*
	   /// Mark intervals intersecting with rays
	   bool RayIntersects();
	*/

	/// Calculate interval for the object
	LInterval* ObjectToInterval( LCollisionShape* Shape );

	scriptmethod LInterval*  GetInterval( int i ) const { return FIntervals[i]; }
	scriptmethod void        SetInterval( int i, LInterval* Intvl ) { FIntervals[i] = Intvl; }
public:
	/* Property(Name=Axis, Type=vec3, FieldName=FAxis) */
	/* Property(Name=Intervals, IndexType=int, FieldName=FIntervals, Type=LInterval, NetIndexedGetter=GetInterval, NetIndexedSetter=SetInterval) */

	/// Which axis are we projecting to
	LVector3 FAxis;

	/// At the time we just use a simple vector-based implementation. Maybe this should be a list or some kind of a tree ?
	std::vector<LInterval*> FIntervals;
};


#endif

/*
 * 09/12/2010
     Log section added
*/
