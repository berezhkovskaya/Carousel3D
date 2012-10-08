/**
 * \file RigidBody.h
 * \brief Physical rigid body
 * \version 0.5.91
 * \date 12/06/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __RigidBody__h__included__
#define __RigidBody__h__included__

#include "Core/iObject.h"
#include "Math/LProjection.h"
#include "Math/LQuaternion.h"

#include "Math/LPlane.h"

#include "Math/LTransform.h"

class LCollisionShape;

/// might be called as a Frame, but I prefer rigid body - the source of transformation
class netexportable scriptfinal clCoordinateFrame : public iObject
{
public:
	clCoordinateFrame();
	virtual ~clCoordinateFrame();

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	virtual LMatrix3     GetMtxOrientation3() const;
	virtual LMatrix4     GetMtxOrientation() const;
	virtual LQuaternion  GetQuatOrientation() const;

	virtual void         SetMtxOrientation( const LMatrix3& Rot );

	virtual LMatrix4     GetTransformation() const;
	virtual void         SetTransformation( const LMatrix4& Mtx );

	virtual void         SetEulerAnglesV( LEulerAngleSystem Sys, const LVector3& Angles );
	virtual void         SetEulerAngles( LEulerAngleSystem Sys, float T1, float T2, float T3 );

	virtual LVector3     GetEulerAnglesV( LEulerAngleSystem Sys ) const;
	virtual void         GetEulerAngles( LEulerAngleSystem Sys, float& T1, float& T2, float& T3 ) const;

	virtual void         SetPosition( const LVector3& Pos );
	virtual LVector3     GetPosition() const;

	/* Property(Name=Orientation, Type=LMatrix3, Getter=GetMtxOrientation3, Setter=SetMtxOrientation ) */
	/* Property(Name=Position,    Type=vec3, Getter=GetPosition, Setter=SetPosition) */
public:
	LMatrix4 FTransform;

	LVector3 FPosition;
	LQuaternion FOrientation;
	LMatrix4 FMtxOrientation;
};

LMatrix3 InertiaTensor_SolidSphere ( float Mass, float Radius );
LMatrix3 InertiaTensor_HollowSphere( float Mass, float Radius );

LMatrix3 InertiaTensor_Cuboid( float Mass, const LVector3& Extents );

/// Z is the axis of symmetry, Radius is the XY radius
LMatrix3 InertiaTensor_CylinderXY( float Mass, float Radius, float Height );

/**
   \brief Rigid body description

   Rigid body is defined by the moving coordinate frame and angular/linear velocity.

   Acceleration/Torque accumulators are also here

   Negative mass determines static entities
*/
class netexportable scriptfinal clRigidBody : public clCoordinateFrame
{
public:
	clRigidBody();
	virtual ~clRigidBody();

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	virtual clRigidBody* Clone();

	scriptmethod LCollisionShape* GetCollisionShape() const { return FCollisionShape; }
	scriptmethod void             SetCollisionShape( LCollisionShape* Sh ) { FCollisionShape = Sh; }

#pragma region fixed mass params
	LMatrix3    FInertiaTensor;
	float       FMass;

	scriptmethod void SetInertiaTensor( const LMatrix3& Inertia );
	scriptmethod LMatrix3 GetInertiaTensor() const { return FInertiaTensor; }

	scriptmethod void  SetMass( float M ) { FMass = M; }
	scriptmethod float GetMass() const  { return FMass; }

	scriptmethod void MakeZeroParams();

#pragma endregion

#pragma region kinematic params

	LVector3    FVelocity;
	LVector3    FAngularVelocity;

	/// Get the velocity at point P using Euler's formula
	scriptmethod LVector3 GetVelocityAtPoint( const LVector3& p ) const;

	/// Get the acceleration using (W_t, W_n) decomposition
	scriptmethod LVector3 GetAccelerationAtPoint( const LVector3& p ) const;

	/// Calculate instant rotation axis from Angular/Linear velocities
	scriptmethod LVector3 GetInstantRotationAxis() const;

	/// Update kinematic paramters for this body
	scriptmethod void Update( float dt );

	/// Make a single Euler dynamics step
	scriptmethod void MakeEulerStep( float dt );

#pragma endregion

#pragma region dynamic params

	LVector3    FForceAccumulator;
	LVector3    FTorqueAccumulator;

	/// Clear force accumulators
	virtual void ClearAccumulators();

	/// Change net torque
	virtual void ApplyPureTorque( const LVector3& T );

	/// Change net force
	virtual void ApplyPureForce( const LVector3& F );

	/// Apply force F to the body at point P (this create some torque if P is not a mass center)
	virtual void ApplyForce( const LVector3& F, const LVector3& P );

#pragma endregion

#pragma region Simulation

	/// Apply an impulse caused by an impact with some static body
	virtual void ResolveImpact_Single( const LVector3& Cp1, const LVector3& N2 );

	/// Apply impulses to the bodies involved in a fast non-resting contact
	static void ResolveImpact_Double( const LVector3& Cp1, const LVector3& Cp2, const LVector3& N1, const LVector3& N2, clRigidBody* Body1, clRigidBody* Body2 );

	/// Project everything (positions, velocities, forces) to the plane P
	virtual void RestrictDynamicsToPlane( const LPlane& P );

#pragma endregion

	/* Property(Category="Kinematics", Description="Angular velocity",   Name=AngularVelocity, Type=vec3,  FieldName=FAngularVelocity) */
	/* Property(Category="Kinematics", Description="Velocity",           Name=Velocity,        Type=vec3,  FieldName=FVelocity) */

	/* Property(Category="Mass properties", Description="Mass",          Name=Mass,            Type=float, FieldName=FMass) */
	/* Property(Category="Mass properties", Description="InertiaTensor", Name=Inertia,         Type=mtx3,  Getter=GetInertiaTensor, Setter=SetInertiaTensor) */

	/* Property(Category="Dynamics", Description="Total force",          Name=Force,           Type=vec3,  FieldName=FForceAccumulator)  */
	/* Property(Category="Dynamics", Description="Total torque",         Name=Torque,          Type=vec3,  FieldName=FTorqueAccumulator) */

	// FIXME: NULL-valued object properties are not yet supported ! And also getter/setter object-valued properties are no supported : we need dynamic cast in Setter OBJECT_PROPERTY_LOAD_SETTER macro
	/* Property(Category="Collision", Description="Collision shape",     Name=CollisionShape,  Type=LCollisionShape, FieldName=FCollisionShape) */
public:
#pragma region Temporary state

	/// Inverse inertia tensor
	LMatrix3 FInvInertiaTensor;

	/// Inertia tensor in world reference frame
	LMatrix3 FWorldInertia;

#pragma endregion

#pragma region Geometric/Collision parameters

	/// Current collision shape
	LCollisionShape* FCollisionShape;

#pragma endregion
};

#endif

/*
 * 12/06/2010
     Log section added
*/
