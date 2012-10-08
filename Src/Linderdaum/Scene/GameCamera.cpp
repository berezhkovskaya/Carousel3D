/**
 * \file GameCamera.cpp
 * \brief Game camera
 * \version 0.6.02
 * \date 16/09/2011
 * \author Viktor Latypov, 2009
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"
#include "GameCamera.h"
#include "LKeys.h"
#include "Core/Console.h"
#include "Utils/Viewport.h"
#include "Math/Trackball.h"
#include "Math/LProjection.h"

using namespace std;
using namespace Linderdaum::Math;

void LCamera::SetPosition( const LVector3& Position )
{
	FPosition = Position;

	if ( FCameraInfo ) { FCameraInfo->SetPosition( Position ); }
}

void LCamera::AddPosition( const LVector3& DeltaPosition )
{
	FPosition += DeltaPosition;
}

void LCamera::SetAngles( const LVector3& Angles )
{
	FPitch = Angles.Z;
	FPan   = Angles.X;
	FRoll  = Angles.Y;

	if ( FCameraInfo ) { FCameraInfo->SetAngles( Angles.X, Angles.Y, Angles.Z ); }
}

void LCamera::SetAnglesA( float Pitch, float Pan, float Roll )
{
	FPitch = Pitch;
	FPan   = Pan;
	FRoll  = Roll;
}

LVector3 LCamera::GetViewingDirection() const
{
	return Linderdaum::Math::GetViewingDirection( GetModelViewMatrix() );
}

LVector3 LCamera::GetSideDirection() const
{
	return Linderdaum::Math::GetSideDirection( GetModelViewMatrix() );
}

void LCamera::Update( float DeltaTime )
{
	if ( !FCameraInfo ) { return; }

	FCameraInfo->Update( DeltaTime );

	// Choose new angles
	float NewPan   = FCameraInfo->GetDesiredXAngle();
	float NewRoll  = FCameraInfo->GetDesiredYAngle();
	float NewPitch = FCameraInfo->GetDesiredZAngle();

	// Choose new position
	LVector3 NewPosition = FCameraInfo->GetDesiredPosition();

	// 1. Update FPosition
	if ( FCameraInfo->UseLinearDamping() )
	{
		float MultFactor = FCameraInfo->GetLinearDamping() * DeltaTime;

		FPosition += ( NewPosition - FPosition ) * MultFactor;
	}
	else
	{
		// Directly set position
		FPosition = NewPosition;
	}

	// 2. Update FAngles
	if ( FCameraInfo->UseAngularDamping() )
	{
		// The normalization is required to avoid "spinning" around the object 2pi times

		// Normalize old angles
		FPitch     =  ClipAngleTo0_360( FPitch );
		FPan       =  ClipAngleTo0_360( FPan );
		FRoll      =  ClipAngleTo0_360( FRoll );

		// Normalize new angles
		NewPitch   =  ClipAngleTo0_360( NewPitch );
		NewPan     =  ClipAngleTo0_360( NewPan );
		NewRoll    =  ClipAngleTo0_360( NewRoll );

		// Get damping factors
		float PitchFactor = FCameraInfo->GetPitchDamping() * DeltaTime;
		float PanFactor   = FCameraInfo->GetPanDamping()   * DeltaTime;
		float RollFactor  = FCameraInfo->GetRollDamping()  * DeltaTime;

		// Find angular deltas
		float PitchDelta = FindAngleDelta( FPitch, NewPitch ) * PitchFactor;
		float PanDelta   = FindAngleDelta( FPan, NewPan )     * PanFactor;
		float RollDelta  = FindAngleDelta( FRoll, NewRoll )   * RollFactor;

		// Finally update angles
		FPitch -= PitchDelta;
		FPan   -= PanDelta;
		FRoll  -= RollDelta;

		// Normalize new angles
		FPitch     =  ClipAngleTo0_360( FPitch );
		FPan       =  ClipAngleTo0_360( FPan );
		FRoll      =  ClipAngleTo0_360( FRoll );
	}
	else
	{
		// Directly set new values
		FPitch = NewPitch;
		FPan   = NewPan;
		FRoll  = NewRoll;
	}

	// 3. If we use direct orientation, then get the matrix
}

LMatrix4 LCamera::GetModelViewMatrix() const
{
	if ( FCameraInfo && FCameraInfo->UseMatrixOrientation() ) { return FCameraInfo->GetTransformationFromPositionAndOrientation(); }

	LMatrix4 RotationMatrix;
	RotationMatrix.FromPitchPanRoll( FPitch, FPan, FRoll );

	return LMatrix4::GetTranslateMatrix( -FPosition ) * RotationMatrix;
}

LCameraInfo::LCameraInfo()
{
	FPitchDamping     = 1.0;
	FPanDamping       = 1.0;
	FRollDamping      = 1.0;
	FLinearDamping    = 1.0;

	FOrientation.IdentityMatrix();
}

LMatrix4 LCameraInfo::GetTransformationFromPositionAndOrientation() const
{
	// Result = Orientation * PositionMatrix;
	return FOrientation * LMatrix4::GetTranslateMatrix( -Position );
}

class iEnvContainer
{
public:
	explicit iEnvContainer( sEnvironment* E ): Env( E ) {};
	sEnvironment* Env;
};

class LCameraInfo_Rail : public LCameraInfo, private iEnvContainer
{
public:
	explicit LCameraInfo_Rail( sEnvironment* E )
		: iEnvContainer( E ),
		  FWasPressed( false ),
		  FStartX( 0.0f )
	{
		SetPosition( LVector3( 0, 0, 0 ) );
		SetAngles( -45.0f, 0, 0 );

		SetLinearDamping( 10.0f );
		SetPitchDamping( 5.0f );
		SetPanDamping( 5.0f );
		SetRollDamping( 5.0f );
	}

	virtual bool UseLinearDamping() const { return true; }
	virtual bool UseAngularDamping() const { return true; }

	virtual void Update( float DeltaSeconds );
private:
	bool     FWasPressed;
	float    FStartX;
};

void LCameraInfo_Rail::Update( float DeltaSeconds )
{
	float Speed = 5.0f;

	clConsole* C = Env->Console;

	if ( C->IsKeyPressed( LK_SHIFT ) ) { Speed *= 5.0f; }

	bool MouseRotateMode = false;

	if ( MouseRotateMode )
	{
		const sMouseCursorInfo MCI = Env->Viewport->GetMouseCursorInfo();

		if ( C->IsKeyPressed( LK_RBUTTON ) )
		{
			if ( FWasPressed )
			{
				Pitch += 720.0f * ( MCI.FMouseX - FStartX ) * DeltaSeconds;
			}
			else
			{
				FStartX = MCI.FMouseX;
				FWasPressed = true;
			}
		}
		else
		{
			FWasPressed = false;
		}
	}

	if ( C->IsKeyPressed( LK_LEFT  ) ) { Pitch += -90.0f * DeltaSeconds; }

	if ( C->IsKeyPressed( LK_RIGHT ) ) { Pitch += +90.0f * DeltaSeconds; }

	if ( C->IsKeyPressed( LK_E ) ) { Pan += -90.0f * DeltaSeconds; }

	if ( C->IsKeyPressed( LK_C ) ) { Pan += +90.0f * DeltaSeconds; }

	Pitch = Math::ClipAngleTo0_360( Pitch );
	Pan   = Math::ClipAngleTo0_360( Pan );

	LMatrix4 PitchMtx;
	PitchMtx.IdentityMatrix();
	PitchMtx.RotateMatrixAxis( Linderdaum::Math::DegToRad( Pitch ), LVector3( 0, 0, 1 ) );

	LVector3 SideDir = LVector3( PitchMtx[0][0], PitchMtx[1][0], PitchMtx[2][0] );
	LVector3 ViewDir = LVector3( PitchMtx[0][1], PitchMtx[1][1], PitchMtx[2][1]  );

	if ( C->IsKeyPressed( LK_Q ) ) { Position.Z += DeltaSeconds * Speed; }

	if ( C->IsKeyPressed( LK_Z ) ) { Position.Z -= DeltaSeconds * Speed; }

	if ( C->IsKeyPressed( LK_1 ) ) { Position.Z += DeltaSeconds * Speed; }

	if ( C->IsKeyPressed( LK_2 ) ) { Position.Z -= DeltaSeconds * Speed; }

	if ( C->IsKeyPressed( LK_W ) || C->IsKeyPressed( LK_UP   ) ) { Position += +ViewDir * DeltaSeconds * Speed; }

	if ( C->IsKeyPressed( LK_S ) || C->IsKeyPressed( LK_DOWN ) ) { Position += -ViewDir * DeltaSeconds * Speed; }

	if ( C->IsKeyPressed( LK_A ) ) { Position += -SideDir * DeltaSeconds * Speed; }

	if ( C->IsKeyPressed( LK_D ) ) { Position +=  SideDir * DeltaSeconds * Speed; }
}


class LCameraInfo_Default : public LCameraInfo, private iEnvContainer
{
public:
	LCameraInfo_Default( sEnvironment* E )
		: iEnvContainer( E )
	{
		SetPosition( LVector3( 0, 0, 0 ) );
		SetAngles( 0, 0, 0 );
	}

	virtual ~LCameraInfo_Default() {}

	virtual bool     UseLinearDamping() const { return true; }
	virtual bool     UseAngularDamping() const { return true; }

	virtual void     Update( float DeltaSeconds )
	{
		float Speed = 5.0f;

		clConsole* C = Env->Console;

		if ( C->IsKeyPressed( LK_SHIFT ) ) { Speed *= 5.0f; }

		if ( C->IsKeyPressed( LK_A ) ) { Pan += -90.0f * DeltaSeconds; }

		if ( C->IsKeyPressed( LK_Z ) ) { Pan += +90.0f * DeltaSeconds; }

		if ( C->IsKeyPressed( LK_1 ) ) { Position.Z += DeltaSeconds * Speed; }

		if ( C->IsKeyPressed( LK_2 ) ) { Position.Z -= DeltaSeconds * Speed; }

		if ( C->IsKeyPressed( LK_LEFT  ) ) { Pitch += -90.0f * DeltaSeconds; }

		if ( C->IsKeyPressed( LK_RIGHT ) ) { Pitch += +90.0f * DeltaSeconds; }

		LVector3 ViewDir = GetCamera()->GetViewingDirection();

		if ( C->IsKeyPressed( LK_UP   ) ) { Position +=  ViewDir * DeltaSeconds * Speed; }

		if ( C->IsKeyPressed( LK_DOWN ) ) { Position += -ViewDir * DeltaSeconds * Speed; }
	}
};
/*
void clGameCamera::Control_Default( float DeltaSeconds )
{
   LVector3 Position  = FCamera.GetPosition();
   float    Pan       = FCamera.GetPan();
   float    Pitch     = FCamera.GetPitch();

   float Speed = 5.0f;

   clConsole* C = Env->Console;

   if ( C->IsKeyPressed( LK_SHIFT ) ) { Speed *= 5.0f; }
   if ( C->IsKeyPressed( LK_A ) ) { Pan += -90.0f * DeltaSeconds; }
   if ( C->IsKeyPressed( LK_Z ) ) { Pan += +90.0f * DeltaSeconds; }
   if ( C->IsKeyPressed( LK_1 ) ) { Position.Z += DeltaSeconds * Speed; }
   if ( C->IsKeyPressed( LK_2 ) ) { Position.Z -= DeltaSeconds * Speed; }
   if ( C->IsKeyPressed( LK_LEFT  ) ) { Pitch += -90.0f * DeltaSeconds; }
   if ( C->IsKeyPressed( LK_RIGHT ) ) { Pitch += +90.0f * DeltaSeconds; }

   FCamera.SetPitch( Pitch );
   FCamera.SetPan( Pan );

   LVector3 ViewDir = FCamera.GetViewingDirection();

   if ( C->IsKeyPressed( LK_UP   ) ) { Position +=  ViewDir * DeltaSeconds * Speed; }
   if ( C->IsKeyPressed( LK_DOWN ) ) { Position += -ViewDir * DeltaSeconds * Speed; }

   FCamera.SetPosition( Position );
}

void clGameCamera::Control_WSAD( float DeltaSeconds )
{
   LVector3 Position = FCamera.GetPosition();

   LVector3 ViewDir = FCamera.GetViewingDirection();

   clConsole* C = Env->Console;

   if ( C->IsKeyPressed( LK_W ) ) { Position +=  ViewDir * DeltaSeconds * 5.0f; }
   if ( C->IsKeyPressed( LK_S ) ) { Position += -ViewDir * DeltaSeconds * 5.0f; }

   FCamera.SetPosition( Position );

   TODO( "WSAD - forward, backward, left, right, pressed r-button - mouse look" )
}
*/
void clGameCamera::Event_TIMER( LEvent Event, const LEventArgs& Args )
{
	// Finally, call the Camera::Update() method with current tracking mode
	if ( FCurrentCameraInfo )
	{
		FCamera.SetCameraInfo( FCurrentCameraInfo );
		FCamera.Update( Args.FFloatArg );
	}
}

clGameCamera::clGameCamera(): FCurrentCameraInfo( NULL )
{
}

void clGameCamera::AfterConstruction()
{
	FCurrentCameraInfo = new LCameraInfo_Rail( Env );
	FCurrentCameraInfo->SetCamera( &FCamera );
	FCamera.SetCameraInfo( FCurrentCameraInfo );
}

/*
 * 10/04/2009
     Initial implementation
*/
