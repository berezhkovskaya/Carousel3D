/**
 * \file CollisionShape.h
 * \brief Collision shape
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __CollisionShape__h__included__
#define __CollisionShape__h__included__

#include "Core/iObject.h"

#include "Math/LMath.h"
#include "Math/LBox.h"

class clGeom;
class clMaterial;
class clRigidBody;

/**
   After some thoughts a hard decision was made : to implement collision shapes in some hierarchy, not as a hard-coded switch-case based list
*/
class scriptfinal LCollisionShape  : public iObject
{
public:
	LCollisionShape();
	virtual ~LCollisionShape();

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	/// Class ID for collision filtering [Class can collide only with some other classes]
	int FCollisionClass;

	/// Get physical extents in the Axis direction
	virtual void CalculateExtents( const LVector3& Axis, float& Min, float& Max ) = 0;

	/// Infinite objects are handled in a different way
	virtual bool IsInfinite() { return false; }

	/// Create some geometrical representation of this colliding entity (TODO : what to do after property change ?)
//	vir_tual clGeom*   CreateDebugGeometry( clMaterial* Material ) const = 0;

	/// Debug-only rendering using canvas
//	vir_tual void      DebugRender( const LMatrix4& Proj, const LMatrix4& View ) = 0;

	/// Fetch current dynamic parameters from rigid body
	virtual void      GetBodyParameters( clRigidBody* Body ) {}

	scriptmethod   void     SetOwnerID( const LString& AnID ) { FOwner = NULL; FOwnerID = AnID; }
	scriptmethod   LString  GetOwnerID() const { return FOwnerID; }

	scriptmethod   iObject* GetOwner();
	scriptmethod   void     SetOwner( iObject* Owner ) { FOwner = Owner; FOwnerID = Owner->GetObjectID(); }

	/** Property(Name=OwnerID, Type=string, Getter=GetOwnerID, Setter=SetOwnerID) */
private:
	/// GUID of the colliding object
	LString     FOwnerID;

	/// Actual object being collided (might be null, the ID is cached)
	iObject*    FOwner;
};

class scriptfinal LCollisionShape_Sphere : public LCollisionShape
{
public:
	LCollisionShape_Sphere() {}
	virtual ~LCollisionShape_Sphere() {}

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	virtual void CalculateExtents( const LVector3& Axis, float& Min, float& Max );

	/// Fetch current dynamic parameters from rigid body
	virtual void GetBodyParameters( clRigidBody* Body );

	/* Property(Name=Position, Type=vec3, FieldName=FPosition) */
	/* Property(Name=Radius, Type=float, FieldName=FRadius) */
public:
	LVector3   FPosition;
	float      FRadius;
};

class scriptfinal LCollisionShape_Plane : public LCollisionShape
{
public:
	LCollisionShape_Plane() {}
	virtual ~LCollisionShape_Plane() {}

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	virtual bool IsInfinite() { return true; }

	virtual void CalculateExtents( const LVector3& Axis, float& Min, float& Max ) { Min = -Linderdaum::Math::INFINITY; Max = +Linderdaum::Math::INFINITY; }

	/* Property(Name=Equation, Type=LVector4, FieldName=FEquation) */
public:
	LVector4   FEquation;
};

class scriptfinal LCollisionShape_Box : public LCollisionShape
{
public:
	LCollisionShape_Box() {}
	virtual ~LCollisionShape_Box() {}

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	virtual void CalculateExtents( const LVector3& Axis, float& Min, float& Max );

	virtual void      GetBodyParameters( clRigidBody* Body );

public:
	LBox FBox;
};

#endif

/*
 * 09/12/2010
     Log section added
*/
