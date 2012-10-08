/**
 * \file SimplePhysics.h
 * \brief A simple pool of free-floating colliding objects. Used as a test for collider and RigidBody class, not more.
 *        For grouping, joint support and full integration to rendering pipeline PhysicsScene is needed
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __SimplePhysics__h__included__
#define __SimplePhysics__h__included__

#include "RigidBody.h"

#include "Math/Collision.h"

class LCollider;

/**
   This is bad. Really bad. No joints, no grouping, array-of-structs instead of struct-of-arrays etc.
*/
class scriptfinal netexportable SimplePhysics : public iObject
{
public:
	SimplePhysics();
	virtual ~SimplePhysics();

	virtual void AfterConstruction();

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	/**
	   \brief Update state of each body in simulation

	   At first all bodies are updates, then the collisions are checked and finally found collisions are resolved
	*/
	scriptmethod void Update( float DT );

	/// Update kinematics and do not check for collisions
	scriptmethod void UpdateOnlyBodies( float DT );

	/// Only check for collisions
	scriptmethod void CheckForCollisions();

	/// Resolve collisions
	scriptmethod void ResolveCollisions();

	/// Set user-specified callback for pair collisions (can used in simple game logic)
//	script_method void SetPairCollisionCallback();
	/// Set user-specified callback for single collisions
//	script_method void SetSingleCollisionCallback();

	/// Render some debug geometry using internal Canvas
	scriptmethod void DebugRender( const LMatrix4& Proj, const LMatrix4& View );

	/// Add one more body to simulation pool
	scriptmethod int  AddBody( clRigidBody* Body );

	/// Remove rigid body from simulation
	scriptmethod void RemoveBody( int idx );

	/// Clear the list of bodies (with optional item deletion)
	scriptmethod void Clear( bool ToDelete );

	scriptmethod clRigidBody* GetBody( int idx ) const           { return FBodies[idx]; }
	scriptmethod void         SetBody( int idx, clRigidBody* B ) { FBodies[idx] = B;    }

	scriptmethod int          GetNumBodies() const { return static_cast<int>( FBodies.size() ); }

	/// Collide two bodies and get the collision point list
	bool CollideBodies( clRigidBody* Body1, clRigidBody* Body2, Linderdaum::Math::sCollisionInfo* Info );

#pragma region Some factory methods (create body and attach required collision geometry)

	/// Create solid sphere
	scriptmethod clRigidBody* CreateSolidSphere( float Mass, float Radius );

	/// Create thin shell with a spherical shape
	scriptmethod clRigidBody* CreateHollowSphere( float Mass, float Radius );

	/// Solid zero-centered cuboid with specified extents
	scriptmethod clRigidBody* CreateSolidCuboid( float Mass, const LVector3& Extents );

	/// Solid cylinder with Z-symmetry
	scriptmethod clRigidBody* CreateCylinderXY( float Mass, float Radius, float Height );

	/// Infinite massless static plane
	scriptmethod clRigidBody* CreatePlane( const LVector3& Point, const LVector3& Normal );

	/// Create the rigid body with collision shape approximated by cuboid (cuboid extents are calculated from the mesh)
//	script_method clRigidBody* CreateBodyForMesh_Cuboid( clVertexAttribs* VA)
	/// Create the rigid body with collision shape approximated by sphere (sphere dimensions is calculated from the mesh)
//	script_method clRigidBody* CreateBodyForMesh_Sphere( clVertexAttribs* VA)

#pragma endregion

public:

	/** Prop_erty(Name=NumBodies, Type=int, Getter=GetNumBodies) */

	/** Property(Name=Bodies, Type=clRigidBody, IndexType=int, FieldName=FBodies, NetIndexedSetter=SetBody, NetIndexedGetter=GetBody) */

	/// TODO: fix LSDC to handle wrapped properties once again. It worked
	/** Prop_erty(Name=Collider, Type=LCollider, FieldName=FCollider) */

	/// Current list of bodies
	std::vector<clRigidBody*> FBodies;

	/// Broad-phase collider
	LCollider* FCollider;

private:
	/// Update kinematics and dynamics, check for collisions and add impulses
	void InternalUpdate( float DT );

	/// Broadphase pair collision callback function (just put the colliding pair to the list)
///     void Collision

	/// Temporary list of all collisions
//	std::vector<std::pair<int, int> > FCollisionList;
};

#endif

/*
 * 09/12/2010
     Log section added
*/
