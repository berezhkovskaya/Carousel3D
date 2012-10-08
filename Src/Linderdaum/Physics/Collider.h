/**
 * \file Collider.h
 * \brief Collider
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __Collider_h__included__
#define __Collider_h__included__

#include "Core/iObject.h"

#include "Math/LMatrix.h"

#define MAX_COLLISION_CLASSES (100)

class LCollisionShape;
class LCollisionShape_Sphere;
class LCollisionShape_Box;
class LCollisionShape_Plane;

class LAxisList;

#include "Generated/Functors.h"

// passes both collision shapes whose projections intersect
typedef ::Linderdaum::Utils::clFunctor<void, TYPELIST_2( LCollisionShape*, LCollisionShape* ) > LBroadPhaseDualCollisionCallback;

// this also needs both bodies
typedef ::Linderdaum::Utils::clFunctor<void, TYPELIST_2( LCollisionShape*, LCollisionShape* ) > LBroadPhaseSingleCollisionCallback;

/**
   Event-driven broadphase collision detector
   Detects potential static collisions between objects using collision filters (class-to-class ally matrix) and

   For simple (slow)bullet-to-object collisions this can be enough

   Exact collision determination can be done in some other manager, like physics.
*/
class netexportable scriptfinal LCollider : public iObject
{
public:
	LCollider();
	virtual ~LCollider();

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	/// Add object for collision handling
	virtual void AddObject( LCollisionShape* Shape );

	/// Remove an object from processing
	virtual void RemoveObject( LCollisionShape* Shape );

	/// Find every potential collision pairs and call handlers for them
	virtual void DetectBroadPhaseCollisions();

	/// N*N testing of intersection (for debugging purposes)
	virtual void BruteForceIntersect();

	/// Set collision callbacks to NULL
	virtual void ClearCallbacks();

	/// Set the function which is called after every collision for each colliding pair of shapes
	virtual void SetDualCollisionCallback( int Class1, int Class2 , LBroadPhaseDualCollisionCallback* Callback );

	/// Set the function which is called after every collision for each shape involved
	virtual void SetSingleCollisionCallback( int Class1, int Class2 , LBroadPhaseSingleCollisionCallback* Callback );

#pragma region Some factory methods

	/// Create sphere with a specified radius
	scriptmethod   LCollisionShape_Sphere*  CreateCollisionSphere( const LVector3& Pos, float Rad ) const;

	/// Create box with dimensions 'Extents' and initial transformation 'Transform'
	scriptmethod   LCollisionShape_Box*     CreateCollisionBox   ( const LMatrix4& Transform, const LVector3& Extents ) const;

	/// Create plane with specified equation
	scriptmethod   LCollisionShape_Plane*   CreateCollisionPlane ( const LVector4& Eq ) const;

#pragma endregion

	/* We serialize collision lists for debugging purposes */

	/* Property(Name=AxisLists, Type=LAxisList,       IndexType=int, FieldName=FIntervalList, NetIndexedGetter=GetAxisList, NetIndexedSetter=SetAxisList) */
	/* Property(Name=Shapes,    Type=LCollisionShape, IndexType=int, FieldName=FShapes,       NetIndexedGetter=GetShape,    NetIndexedSetter=SetShape) */

	scriptmethod void               SetAxisList( int i, LAxisList* AList ) { FIntervalList[i] = AList; }
	scriptmethod LAxisList*         GetAxisList( int i ) const             { return FIntervalList[i];  }

	scriptmethod void               SetShape( int i, LCollisionShape* Shape ) { FShapes[i] = Shape; }
	scriptmethod LCollisionShape*   GetShape( int i ) const { return FShapes[i]; }

public:
	LBroadPhaseSingleCollisionCallback*   FSingleCallback [MAX_COLLISION_CLASSES][MAX_COLLISION_CLASSES];

	LBroadPhaseDualCollisionCallback*     FDualCallback   [MAX_COLLISION_CLASSES][MAX_COLLISION_CLASSES];

	/**
	   Projections to three axii are maintained.

	   At each timestep we just check if there are any ovelaps in AxisLists
	*/

	/// Intervals (projections) of objects to three basic axii
	std::vector<LAxisList*> FIntervalList;

	/// List of all colliding entities
	std::vector<LCollisionShape*> FShapes;


	/// Clear all collision state flags to CT_UNDEFINED (used for debugging)
	void ClearCollisionFlags();
};

#endif

/*
 * 09/12/2010
     Log section added
*/
