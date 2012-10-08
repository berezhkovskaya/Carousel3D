#include "BodyControllers.h"

#include "Input/Input.h"

#include "Physics/CollisionShape.h"
#include "Physics/SimplePhysics.h"

#include "Environment.h"
#include "Core/Console.h"
#include "LKeys.h"

#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"

#include "LColors.h"

#include "Utils/Viewport.h"

void RigidBodyController::ApplyToBody()
{
	FBody->FPosition    = FPosition;
	FBody->FOrientation = FOrientation;

	FBody->FVelocity        = FVelocity;
	FBody->FAngularVelocity = FAngularVelocity;

	FBody->FMtxOrientation = LMatrix4( FOrientation.ToMatrix3() );
	FBody->FTransform      = ComposeTransformation( FPosition, FBody->FMtxOrientation );
}

void ModifyVector( LVector3& Value, const LVector3& DesiredValue, const LVector3& Damping, float dt )
{
	LVector3 PosDelta = DesiredValue - Value;

	float Len = PosDelta.Length();

	if ( Len < Math::EPSILON ) { return; }

	LVector3 Amount( Damping.X * PosDelta.X, Damping.Y * PosDelta.Y, Damping.Z * PosDelta.Z );
	LVector3 ActualDelta( Amount * dt );

	LVector3 AbsDelta( Math::LAbs( PosDelta.X ), Math::LAbs( PosDelta.Y ), Math::LAbs( PosDelta.Z ) );

	( ActualDelta.X > AbsDelta.X ) ? Value.X = DesiredValue.X : Value.X += ActualDelta.X;
	( ActualDelta.Y > AbsDelta.Y ) ? Value.Y = DesiredValue.Y : Value.Y += ActualDelta.Y;
	( ActualDelta.Z > AbsDelta.Z ) ? Value.Z = DesiredValue.Z : Value.Z += ActualDelta.Z;
}

void RigidBodyController_MoveTo::Update( float dt )
{
	ModifyVector( FPosition, FDesiredPosition, FLinearDamping, dt );
	ModifyVector( FAngles, FDesiredAngles, FAngularDamping, dt );

	LMatrix4 TheOrientation;
	AnglesToMatrix( Euler_xyzs, TheOrientation, FAngles.X, FAngles.Y, FAngles.Z );

	FOrientation.FromMatrix3( TheOrientation.ExtractMatrix3() );

	if ( FDevice != NULL )
	{
		// fetch axis values
		LVector3 Speed( FDevice->GetAxisValue( FXAxis ), FDevice->GetAxisValue( FYAxis ), FDevice->GetAxisValue( FZAxis ) );

		LVector3 Delta = Speed * dt / 10000.0f;

		if ( FDevice->IsPressed( FRotationSwitchButton ) )
		{
			FDesiredAngles += Delta;
		}
		else
		{
			FDesiredPosition += vec3( Delta.X, -Delta.Y, Delta.Z );
		}
	}

	ReadKeys( dt );
}

void RigidBodyController_MoveTo::ApplyToBody()
{
	FBody->FPosition = FPosition;
	FBody->FOrientation = FOrientation;
	FBody->FMtxOrientation = LMatrix4( FOrientation.ToMatrix3() );
	FBody->FTransform = FBody->FMtxOrientation * LMatrix4::GetTranslateMatrix( FPosition );

	FBody->FVelocity = FVelocity;
	FBody->FAngularVelocity = FAngularVelocity;

	if ( FBody->FCollisionShape )
	{
		// update collision info
		FBody->FCollisionShape->GetBodyParameters( FBody );
	}
}

/// FIXME : use CVar binding or (better) OnKey event from InputSystem
void RigidBodyController_MoveTo::ReadKeys( float dt )
{
	LVector3 Delta( 0, 0, 0 );

	/// TODO: move to class properties
	const float FAngleSpeed  = 5.0f;
	const float FLinearSpeed = 5.0f;

	if ( Env->Console->IsKeyPressed( LK_LEFT ) )  { Delta.X = -1.0f; }

	if ( Env->Console->IsKeyPressed( LK_RIGHT ) ) { Delta.X = +1.0f; }

	if ( Env->Console->IsKeyPressed( LK_UP ) )    { Delta.Y = +1.0f; }

	if ( Env->Console->IsKeyPressed( LK_DOWN ) )  { Delta.Y = -1.0f; }

	float FMult = Env->Console->IsKeyPressed( LK_SHIFT ) ? 5.0f : 1.0f;

	if ( Env->Console->IsKeyPressed( LK_CONTROL ) )
	{
		FDesiredAngles += FMult * FAngleSpeed * Delta * dt;
	}
	else
	{
		FDesiredPosition += FMult * FLinearSpeed * Delta * dt;
	}
}

RigidBodyController_Walk::RigidBodyController_Walk():
	FMoveSpeed( 5.0f ), FJumpIntensity( 125.0f ),
	FUp( 0, 1, 0 ), FView( 0 ), FDir( 0 ),
	FLookPoint( 0, 0, 0 ), FLookDist( 2.0f ),
	FMass( 1.0f ),
	FTurnRate( 2.0f ), FLookRate( 1.0f ),
	FStandingOn( NULL )
{
	Reset();
}

// cast a ray in direction V to find th closest object
clRigidBody* FindSomething( SimplePhysics* P, const LVector3& From, const LVector3& Dir )
{
	// trace the ray
	for ( int i = 0 ; i < P->GetNumBodies() ; i++ )
	{
		clRigidBody* B = P->GetBody( i );

		LCollisionShape* Sh = B->FCollisionShape;

		if ( !Sh ) { continue; }

		LCollisionShape_Box* Box = dynamic_cast<LCollisionShape_Box*>( Sh );

		if ( Box )
		{
			LVector3 Pt;

			if ( IntersectRayToBox( From, Dir, Box->FBox.FCenter, ( LVector3* )( Box->FBox.FAxis.ToFloatPtr() ), Box->FBox.FExtent * 2.0f, Pt ) )
			{
				return B;
			}
		}
	}

	return NULL;
}

/// Build a plane with (0,1,0) normal using a StandOn body
LPlane BuildSupportingPlane( clRigidBody* SupportBody, const LVector3& Pos, float Radius )
{
	LVector3 Pt( 0, 0, 0 );

	return LPlane( vec3( 0, 1, 0 ), Pt );
}

void RigidBodyController_Walk::Update( float dt )
{
	if ( !FBody ) { return; }

	ReadKeys( dt );

	// forces

	// integrate
//	FBody->Update(dt);

	if ( FStandingOn )
	{
		// do "collision" response

		// yeah, I know. we should just make the -CollisionNormal impulse
		// but here we handle the resting/standing contacts

		if ( FBody->FVelocity.Y /*Dot(Info.N1)*/ < 0.0f )
		{
			// we are falling and trying to stand
			if ( Info.FNumPoints > 0 )
			{
				FPosition = Info.FPoints1[0] + FRadius * vec3( 0, 1, 0 );
				FBody->FVelocity.Y = 0.0f;
//				FPosition = Info.FPoints1[0] + FRadius * Info.N1 /*vec3(0,1,0)*/;
//				FBody->FVelocity = FBody->FVelocity - FBody->FVelocity.Dot(Info.N1) * Info.N1;
			}
		}
	}

	FPosition += FBody->FVelocity * dt;

	FBody->FVelocity += ( FForce / FMass ) * dt;

	LMatrix4 DirMtx = LMatrix4::GetRotateMatrixAxis( FDir, FUp );

	FOrientation = DirMtx * LMatrix4::GetRotateMatrixAxis( FView, DirMtx * LVector3( 1, 0, 0 ) );

	// false collisions
	if ( FPosition.Y < 0.0f )
	{
		FPosition.Y = 0.0f;
		FBody->FVelocity.Y = 0.0f;
	}

	FBody->FTransform      = FOrientation * LMatrix4::GetTranslateMatrix( FPosition );
}

void RigidBodyController_Walk::ApplyToBody()
{
//	Rigi
}

void RigidBodyController_Walk::DebugRender( const LMatrix4& Proj, const LMatrix4& View )
{
	if ( FStandingOn )
	{
		clCanvas* C = Env->Renderer->GetCanvas();

		C->RigidBody( Proj, View, FStandingOn, false, false, false );
		C->ContactInfo( Proj, View, &Info, 0.1f, 1.0f, LC_ForestGreen, LC_DarkOliveGreen, LC_White, LC_LightBlue );
	}
}

void RigidBodyController_Walk::ReadKeys( float dt )
{
	float Move = 0.0f;
	float Turn = 0.0f;
	float Look = 0.0f;
	float Jump = 0.0f;

	// cast a ray down and find out what we are standing on
	if ( FBody->FVelocity.Length() > Math::EPSILON )
	{
		FStandingOn = FindSomething( FPhysics, FPosition, FBody->FVelocity.GetNormalized() );
	}

	if ( FStandingOn )
	{
		float Rad = dynamic_cast<LCollisionShape_Sphere*>( FBody->FCollisionShape )->FRadius;

		LBox Box = dynamic_cast<LCollisionShape_Box*>( FStandingOn->FCollisionShape )->FBox;

		Info.FNumPoints = 0;

		Math::Collision_SphereToBox( LSphere( FBody->GetPosition(), Rad ), Box, &Info );
	}

	if ( Env->Console->IsKeyPressed( LK_LEFT ) )  { Turn = -1.0f; }

	if ( Env->Console->IsKeyPressed( LK_RIGHT ) ) { Turn = +1.0f; }

	if ( Env->Console->IsKeyPressed( LK_Q ) )  { Look = -1.0f; }

	if ( Env->Console->IsKeyPressed( LK_Z ) )  { Look = +1.0f; }

	if ( Env->Console->IsKeyPressed( LK_UP ) )    { Move = -1.0f; }

	if ( Env->Console->IsKeyPressed( LK_DOWN ) )  { Move = +1.0f; }

	if ( Env->Console->IsKeyPressed( LK_SPACE ) ) { Jump = +1.0f; }

	LVector3 DirVector = LMatrix4::GetRotateMatrixAxis( FDir, FUp ) * LVector3( 0, 0, 1 );

	FDir += Turn * FTurnRate * dt;
	FView += Look * FLookRate * dt;

	const float FDamping = 0.7f;

	FBody->FVelocity.X *= FDamping;
	FBody->FVelocity.Z *= FDamping;

	FBody->FVelocity += Move * FMoveSpeed * DirVector; // * dt;

	FOldMouse = FMouse;
	FMouse = Env->Viewport->GetMousePosition();

	if ( FBody->FVelocity.Y <= Math::EPSILON )
	{
		if ( Jump > 0 )
		{
			FBody->FVelocity.Y += Jump * FJumpIntensity * dt;
		}
	}

	FForce = vec3( 0.0f, -9.8f, 0.0f );
}

/*
 * 08/01/2011
     It's here
*/
