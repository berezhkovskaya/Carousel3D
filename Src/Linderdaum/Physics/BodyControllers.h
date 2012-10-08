#pragma once

#ifndef __BodyControllers__h__included__
#define __BodyControllers__h__included__

#include "Physics/RigidBody.h"
#include "Math/Collision.h"

class iInputDevice;

class SimplePhysics;

/**
   \brief Generic rigid body controller

   Other uses can include pushing/pulling etc.
*/
class netexportable RigidBodyController: public iObject
{
public:
	RigidBodyController():
		FBody( NULL ), FDevice( NULL ),
		/// Default axis and key mapping
		FRotationSwitchButton( 4 ), FXAxis( 0 ), FYAxis( 1 ), FZAxis( 2 )
	{
		Reset();
	}

	virtual void Reset()
	{
		FPosition    = LVector3( 0, 0, 0 );
		FOrientation = LQuaternion( 0.0f, 0.0f, 0.0f, 1.0f );

		FAngularVelocity = LVector3( 0, 0, 0 );
		FVelocity        = LVector3( 0, 0, 0 );
	}

	virtual ~RigidBodyController() {}

	virtual void Update( float dt ) {}

	/// Copy calculated parameters to the rigid body
	virtual void ApplyToBody();

	/// Assing the input device
	scriptmethod void SetInputDevice( iInputDevice* Dev ) { FDevice = Dev; }

public:
	LVector3 FPosition;
	LQuaternion FOrientation;

	LVector3 FAngularVelocity;
	LVector3 FVelocity;

	/// Controlled rigid body
	clRigidBody* FBody;

	/// Some auxillary axial input device (i.e., Joystick or Mouse)
	iInputDevice* FDevice;

	/// Axis mapping (which input axis affects rigid body orientation)
	int FXAxis, FYAxis, FZAxis;

	/// Index of the button
	int FRotationSwitchButton;
};

/**
   \brief Simple manipulation: set desired position and orientation for body

   The position and orientation are not set instantly but with some proportional delay.

   TODO: use FeedbackLoops from Control.h here
*/
class netexportable scriptfinal RigidBodyController_MoveTo : public RigidBodyController
{
public:
	RigidBodyController_MoveTo() :
		FLinearDamping( 1.0f, 1.0f, 1.0f ),
		FAngularDamping( 1.0f, 1.0f, 1.0f ),
		FAngles( 0, 0, 0 ),
		FDesiredAngles( 0, 0, 0 )
	{
		Reset();
	}

	virtual ~RigidBodyController_MoveTo() {}

	virtual void Update( float dt );

	virtual void ApplyToBody();

public:
	LVector3 FDesiredPosition;
	LVector3 FDesiredAngles;

	LVector3 FAngles;

	LVector3 FLinearDamping;
	LVector3 FAngularDamping;

private:
	void ReadKeys( float dt );
};


/**
   Simple 3d walking controller (for platformer-style movement)
*/
class netexportable scriptfinal RigidBodyController_Walk : public RigidBodyController
{
public:
	RigidBodyController_Walk();

	virtual ~RigidBodyController_Walk() {}

	virtual void Update( float dt );

	virtual void ApplyToBody();

	virtual void DebugRender( const LMatrix4& Proj, const LMatrix4& View );

public:
	/// Link to physics engine
	SimplePhysics* FPhysics;

	Math::sCollisionInfo Info;

	/// up-vector
	LVector3 FUp;

	/// walking direction  (y-axis rotation)
	float FDir;

	/// vertical direction (x-axis rotation)
	float FView;

	/// collision radius
	float FRadius;

	float FMoveSpeed;
	float FJumpIntensity;
	float FTurnRate;
	float FLookRate;

	float FLookDist;

	LVector3 FLookPoint;

	LMatrix4 FOrientation;

	LVector3 FPosition;
	LVector3 FForce;

	float FMass;

	clRigidBody* FStandingOn;

private:
	LVector2 FOldMouse;
	LVector2 FMouse;

	void ReadKeys( float dt );
};


#endif

/*
 * 08/01/2011
     It's here
*/
