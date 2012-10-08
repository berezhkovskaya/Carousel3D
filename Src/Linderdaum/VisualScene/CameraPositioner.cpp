#include "CameraPositioner.h"

#include "VisualScene/Trajectory.h"

#include "GUI/GUIManager.h"
#include "GUI/ComCtl/I_Slider.h"
#include "GUI/ComCtl/I_CheckBox.h"

#include "Core/Console.h"
#include "Utils/Viewport.h"
#include "Core/Linker.h"

#include "LKeys.h"
#include "Environment.h"

#include "Math/LTransform.h"
#include "Math/LProjection.h"
#include "Math/LMath.h"

void clTrajectoryPositioner::AddControlUI()
{
	FUpdateCheck = Env->GUI->AddPropertyCheck ( vec2( 0.02f, 0.85f ), vec2( 0.1f, 0.05f ), "Update time", this, "UpdateTime" );
	FTrackCheck = Env->GUI->AddPropertyCheck ( vec2( 0.3f, 0.85f ), vec2( 0.1f, 0.05f ), "Track target", this, "TrackTarget" );
	FSpeedSlider = Env->GUI->AddPropertySlider ( vec2( 0.02f, 0.90f ), vec2( 0.3f, 0.05f ), -128.0f, 128.0f, "Speed", this, "SpeedFactor" );
	FTimeSlider = Env->GUI->AddPropertySlider ( vec2( 0.02f, 0.95f ), vec2( 0.3f, 0.05f ), GetMinTime(), GetMaxTime(), "Time", this, "LocalTime" );
}

void clTrajectoryPositioner::RemoveControlUI()
{
	if ( !FUpdateCheck ) { return; }

	Env->GUI->GetDesktop()->RemoveView( FUpdateCheck );
	Env->GUI->GetDesktop()->RemoveView( FTrackCheck );
	Env->GUI->GetDesktop()->RemoveView( FTimeSlider );
	Env->GUI->GetDesktop()->RemoveView( FSpeedSlider );

	FUpdateCheck->DisposeObject();
	FTrackCheck->DisposeObject();
	FTimeSlider->DisposeObject();
	FSpeedSlider->DisposeObject();

	FUpdateCheck = FTrackCheck = NULL;
	FTimeSlider = FSpeedSlider = NULL;
}

void iCameraPositioner::Event_TIMER( LEvent Event, const LEventArgs& Args )
{
	/// Redirect to update
	Update( Args.FFloatArg );
}

void iCameraPositioner::Event_KEY( LEvent Event, const LEventArgs& Args )
{
	HandleKey( ( int )Args.FFloatArg, Args.FBoolArg );
}

void iCameraPositioner::Register()
{
	FTimerHandler = Utils::Bind( &iCameraPositioner::Event_TIMER, this );
	Env->Connect( L_EVENT_TIMER,  FTimerHandler );
	FKeyHandler = Utils::Bind( &iCameraPositioner::Event_KEY, this );
	Env->Connect( L_EVENT_KEY,  FKeyHandler );

	FWheelTicks = 0;
}

void iCameraPositioner::Unregister()
{
	Env->Disconnect( L_EVENT_TIMER, FTimerHandler );
	Env->Disconnect( L_EVENT_KEY,   FKeyHandler );
}

void clSelectionPositioner::SetMode( int i )
{
	if ( i < 0 || i > static_cast<int>( FPositioners.size() ) ) { return; }

	FCurrentMode = i;
}

iCameraPositioner* clSelectionPositioner::FindPositioner( const LString& Name )
{
	for ( size_t i = 0 ; i < FPositioners.size() ; i++ )
	{
		if ( FPositioners[i]->GetObjectID() == Name ) { return FPositioners[i]; }
	}

	return NULL;
}

void clSelectionPositioner::ClearPositioners( bool Delete )
{
	if ( Delete )
	{
		for ( size_t i = 0 ; i < FPositioners.size() ; i++ )
		{
			FPositioners[i]->DisposeObject();
		}
	}

	FPositioners.clear();
}

void clSelectionPositioner::HandleKey( int Key, bool State )
{
	if ( FCurrentMode < 0 || FCurrentMode >= static_cast<int>( FPositioners.size() ) ) { return; }

	FPositioners[FCurrentMode]->HandleKey( Key, State );
}

void clSelectionPositioner::Update( float dt )
{
	if ( FCurrentMode < 0 || FCurrentMode >= static_cast<int>( FPositioners.size() ) ) { return; }

	FPositioners[FCurrentMode]->Update( dt );

	FCurrentTransform = FPositioners[FCurrentMode]->GetCurrentTransform();
}

clTrajectoryPositioner::clTrajectoryPositioner():
	FTrajectory( NULL ),
	FTrackTarget( false ),
	FUpdateTime( true ),
	FSpeedFactor( 1.0f ),
	FStaticTarget( 0, 0, 0 ),
	FUpVector( 0, 1, 0 ),
	FUpdateCheck( NULL ),
	FTrackCheck( NULL ),
	FTimeSlider( NULL ),
	FSpeedSlider( NULL )
{
}

clTrajectoryPositioner::~clTrajectoryPositioner()
{
	if ( FTrajectory )
	{
		FTrajectory->DisposeObject();
	}
}

void clTrajectoryPositioner::AfterConstruction()
{
	FTrajectory = Env->Linker->Instantiate( "clRigidBodyTrajectory" );
}

void clTrajectoryPositioner::ToggleTimeLine( bool Visible )
{
	//
}

void clTrajectoryPositioner::GenerateOrbit( float Elevation, float OrbitRadius, const LVector3& Target )
{
	FTrajectory->CreateOrbitingAroundObject( Elevation, OrbitRadius, Target, FUpVector );
}

void clTrajectoryPositioner::Update( float dt )
{
	/// Update time counter if required
	if ( FUpdateTime ) { FLocalTime += FSpeedFactor * dt; }

	if ( !FTrajectory ) { return; }

	float MaxT = GetMaxTime();
	float MinT = GetMinTime();

	if ( FTrajectory->FLoop )
	{
		while ( FLocalTime > MaxT )
		{
			FLocalTime -= ( MaxT - MinT );
		}

		while ( FLocalTime < MinT )
		{
			FLocalTime += ( MaxT - MinT );
		}
	}
	else
	{
		FLocalTime = Math::Clamp( FLocalTime, MinT, MaxT );
	}

	LVector3 Pos = FTrajectory->GetPositionForTime( FLocalTime );

	LQuaternion Q = FTrackTarget ?
	                LQuaternion( Math::LookAt( Pos, FStaticTarget, FUpVector ).ExtractMatrix3() ) :
	                FTrajectory->GetOrientationForTime( FLocalTime );

	FCurrentTransform = ComposeCameraTransformation( Pos, LMatrix4( Q.ToMatrix3() ) );
}

float clTrajectoryPositioner::GetMinTime() const
{
	if ( !FTrajectory || ( FTrajectory->GetNodeCount() <= 0 ) ) { return 0; }

	return FTrajectory->GetNode( 0 )->FTimestamp;
}

float clTrajectoryPositioner::GetMaxTime() const
{
	if ( !FTrajectory || ( FTrajectory->GetNodeCount() <= 0 ) ) { return 0; }

	return FTrajectory->GetNode( FTrajectory->GetNodeCount() - 1 )->FTimestamp;
}

clPanOrbitPositioner::clPanOrbitPositioner():
	FMinDistance( 1.0f ),
	FMinPanningVelocity( 100.0f ),
	FMinZoomingVelocity( 100.0f ),
	FDefaultOrbitingVelocity( 40000.0f ),
	FDefaultPanningVelocity( 200.0f ),
	FDefaultZoomVelocity( 500.0f ),
	FUpVector( 0, 0, 1 ),
	FTarget( 0, 0, 0 )
{
	FWheelTicks = 0;
}

void iCameraPositioner::HandleKey( int Key, bool State )
{
	if ( ( Key == LK_WHEELDOWN ) && State )
	{
		FWheelTicks--;
	}

	if ( ( Key == LK_WHEELUP ) && State )
	{
		FWheelTicks++;
	}
}

void clPanOrbitPositioner::Update( float dt )
{
	/// 1. Read input

	/// TODO: Read joysticks

	// use some keys to zoom later
	FZoomOut = 0.0f;
	FZoomIn  = 0.0f;

	FOrbiting = FPanning = false;

	if ( Env->Console->IsKeyPressed( LK_MBUTTON ) )
	{
		if ( Env->Console->IsKeyPressed( LK_MENU ) )
		{
			FOrbiting = true;
		}
		else
		{
			FPanning = true;
		}
	}

//	FOrbiting = Env->Console->IsKeyPressed( LK_RBUTTON );
//	FPanning  = Env->Console->IsKeyPressed( LK_MBUTTON );

	FZooming = false; //Env->Console->IsKeyPressed( LK_CONTROL );

	FZoomingIn  = false; //Env->Console->IsKeyPressed(LK_WHEELUP);
	FZoomingOut = false; //Env->Console->IsKeyPressed(LK_WHEELDOWN);

	if ( FWheelTicks != 0 )
	{
		FZoomIn  = Math::LMax( ( ( float )FWheelTicks ) * 0.03f, 0.0f );
		FZoomOut = Math::LMin( ( ( float )FWheelTicks ) * 0.03f, 0.0f );
		FWheelTicks = 0;
	}

//	if (Env->Console->IsKeyPressed(LK_WHEELUP)) FZoomIn = 1.0f;
//	if (Env->Console->IsKeyPressed(LK_WHEELDOWN)) FZoomOut = 1.0f;

	FLastMouse = FMouse;

	LVector2 P = Env->Viewport->GetMousePosition();
	FMouse = vec3( P.X, P.Y, 0.0f );

	/// Last mouse change
	FMouseDelta = FMouse - FLastMouse;

	/// Last panning value (might be taken from joystick axis)
	FPanDelta = vec3( 0, 0, 0 );

	/// Last orbiting value
	FOrbitDelta = vec3( 0, 0, 0 );

	/// 2. Read keyboard

	/// 3. Update state
	MakeStep( dt );
}

float clStdCameraPositioner::GetSpeed( float dt )
{
	// TODO : move to properties
	const float Speed = 50.0f;
	const float SpeedMultiplier = 5.0f;

	float ThisSpeed = Speed;

	if ( Env->Console->IsKeyPressed( LK_SHIFT ) ) { ThisSpeed *= SpeedMultiplier; }

	return ThisSpeed * dt;
}

void clFightingChessPositioner::Update( float dt )
{
	float S = GetSpeed( dt );

	if ( Env->Console->IsKeyPressed( LK_A ) ) { FPan -= 90.0f * dt; }

	if ( Env->Console->IsKeyPressed( LK_Z ) ) { FPan += 90.0f * dt; }

	if ( Env->Console->IsKeyPressed( LK_1 ) ) { FPosition.Z += S; }

	if ( Env->Console->IsKeyPressed( LK_2 ) ) { FPosition.Z -= S; }

	if ( Env->Console->IsKeyPressed( LK_LEFT  ) ) { FPitch -= 90.0f * dt; }

	if ( Env->Console->IsKeyPressed( LK_RIGHT ) ) { FPitch += 90.0f * dt; }

	LMatrix4 PitchMtx;
	PitchMtx.IdentityMatrix();
	PitchMtx.RotateMatrixAxis( Linderdaum::Math::DegToRad( FPitch ), LVector3( 0, 0, 1 ) );

	LVector3 ViewDir = LVector3( PitchMtx[0][1], PitchMtx[1][1], PitchMtx[2][1]  );

	if ( Env->Console->IsKeyPressed( LK_UP   ) ) { FPosition +=  ViewDir * S; }

	if ( Env->Console->IsKeyPressed( LK_DOWN ) ) { FPosition += -ViewDir * S; }

	clStdCameraPositioner::Update( dt );
}

clRailPositioner::clRailPositioner(): FUpVector( 0, 1, 0 ), FDirVector( 0, 0, 1 )
{
}

void clRailPositioner::Update( float dt )
{
	float S = GetSpeed( dt );

	// move to properties
	float PitchRate = 90.0f * dt;
	float PanRate = 90.0f * dt;

	PitchRate *= Env->Console->IsKeyPressed( LK_SHIFT ) ? 3.0f : 1.0f;
	PanRate *= Env->Console->IsKeyPressed( LK_SHIFT ) ? 3.0f : 1.0f;

	if ( Env->Console->IsKeyPressed( LK_LEFT  ) ) { FPitch -= PitchRate; }

	if ( Env->Console->IsKeyPressed( LK_RIGHT ) ) { FPitch += PitchRate; }

	if ( Env->Console->IsKeyPressed( LK_E ) )     { FPan -= PanRate; }

	if ( Env->Console->IsKeyPressed( LK_C ) )     { FPan += PanRate; }

	FPitch = Math::ClipAngleTo0_360( FPitch );
	FPan   = Math::ClipAngleTo0_360( FPan );

	LMatrix4 PitchMtx;
	PitchMtx.IdentityMatrix();
	PitchMtx.RotateMatrixAxis( Linderdaum::Math::DegToRad( -FPitch ), FUpVector );

	LVector3 ViewDir = -( PitchMtx * FDirVector );
	LVector3 SideDir = -( FUpVector.Cross( ViewDir ) );

	if ( Env->Console->IsKeyPressed( LK_Q ) || Env->Console->IsKeyPressed( LK_1 ) ) { FPosition += S * FUpVector; }

	if ( Env->Console->IsKeyPressed( LK_Z ) || Env->Console->IsKeyPressed( LK_2 ) ) { FPosition -= S * FUpVector; }

	if ( Env->Console->IsKeyPressed( LK_W ) || Env->Console->IsKeyPressed( LK_UP   ) ) { FPosition += -ViewDir * S; }

	if ( Env->Console->IsKeyPressed( LK_S ) || Env->Console->IsKeyPressed( LK_DOWN ) ) { FPosition += ViewDir * S; }

	if ( Env->Console->IsKeyPressed( LK_A ) ) { FPosition +=  SideDir * S; }

	if ( Env->Console->IsKeyPressed( LK_D ) ) { FPosition += -SideDir * S; }

	LMatrix4 Rotation = LMatrix4::GetRotateMatrixAxis( Math::DegToRad( FPan ), SideDir ) * LMatrix4::GetRotateMatrixAxis( Math::DegToRad( FPitch ), FUpVector );

	FCurrentTransform = LMatrix4::GetTranslateMatrix( -FPosition ) * Rotation;
}

void clPanOrbitPositioner::Reset()
{
	FWheelTicks = 0;

	FView = FViewerPosition - FTarget;

	FViewDistance = FView.Length();

	FSphericalCoords = Math::CartesianToSpherical( FView );

	FCurrentTransform = LMatrix4::Identity();

	MakeStep( 0.0f );
}

void clPanOrbitPositioner::MakeStep( float dt )
{
	/// zoom/pan faster when we are farther away
	float PanVelocity = FDefaultPanningVelocity * FViewDistance;
	PanVelocity = std::max( FMinPanningVelocity, PanVelocity );

	float ZoomVelocity = FDefaultZoomVelocity   * FViewDistance;
	ZoomVelocity = std::max( FMinZoomingVelocity, ZoomVelocity );

	/// use supplied input values
	float PanHorz  = -FPanDelta.X;
	float PanVert  = -FPanDelta.Y;
	float LookHorz = -FOrbitDelta.X;
	float LookVert = +FOrbitDelta.Y;

	float Zoom = FZoomIn + FZoomOut;

	if ( FOrbiting )
	{
		LookHorz += FMouseDelta.X;
		LookVert += FMouseDelta.Y;
	}

	if ( FPanning )
	{
		PanHorz += FMouseDelta.X;
		PanVert -= FMouseDelta.Y;
	}

	if ( FZooming ) { Zoom += -FMouseDelta.X + FMouseDelta.Y; }

	float MoveFactorH = PanHorz * PanVelocity * dt;
	float MoveFactorV = PanVert * PanVelocity * dt;

	// TODO : use Up/ViewDir here
	LVector3 HorzMove = LVector3( FCurrentTransform[0][0], FCurrentTransform[1][0], FCurrentTransform[2][0] ) * MoveFactorH;
	LVector3 VertMove = LVector3( FCurrentTransform[0][1], FCurrentTransform[1][1], FCurrentTransform[2][1] ) * MoveFactorV;

	FTarget += HorzMove + VertMove;

//	if ( FZoomingIn ) { Zoom -= 1.0f; }
//	if ( FZoomingOut ) { Zoom += 1.0f; }

	FViewDistance += Zoom * ZoomVelocity * dt;

	/// handle orbiting
	FSphericalCoords.Z += LookVert * FDefaultOrbitingVelocity * dt;
	FSphericalCoords.Y += LookHorz * FDefaultOrbitingVelocity * dt;

	/// avoid that the camera slips past the center of interest
	if ( FViewDistance < FMinDistance )
	{
		FViewDistance = FMinDistance;
		FTarget -= vec3( FCurrentTransform[0][2], FCurrentTransform[1][2], FCurrentTransform[2][2] ) * FViewDistance;
	}

	/// get polar vector in cartesian space
//	LMatrix4 ViewTrans = LMatrix4::GetTranslateMatrix( -FUpVector * FViewDistance );
	LMatrix4 ViewTrans = LMatrix4::GetTranslateMatrix( vec3( 0.0f, 0.0f, -FViewDistance ) );

	float Angle2 = Math::DegToRad( FSphericalCoords.Z );
	float Angle1 = Math::DegToRad( FSphericalCoords.Y - 90.0f );

	LMatrix4 RotY = LMatrix4::GetRotateMatrixAxis( Angle2, vec3( 1, 0, 0 ) );
	LMatrix4 RotZ = LMatrix4::GetRotateMatrixAxis( Angle1, FUpVector );

	LMatrix4 m = RotZ * RotY * ViewTrans;

	FCurrentTransform = LMatrix4::GetTranslateMatrix( FTarget ) * m;

	// Flip the Y axis. TODO: figure out the transform
//	mtx4 Refl = LMatrix4::GetDiagonalMatrixV(LVector4(1,1,-1,1));
//	FCurrentTransform = Refl * FCurrentTransform;

	DecomposeCameraTransformation( FCurrentTransform, FViewerPosition, m );
}

void clDampingPositioner::Update( float dt )
{
	if ( !FOriginalCamera ) { return; }

	LMatrix4 NewTransform = FOriginalCamera->GetCurrentTransform();

//	if (!FUseDamping)
	{
		FCurrentTransform = NewTransform;
		return;
	}

	/// interpolate between current and desired positions

// DecomposeCameraTransformation( NewTransform, NewPos, NewRot );
}

void clDampingPositioner::LinkToCameraPositioner( iCameraPositioner* P )
{
	FOriginalCamera = P;

	FCurrentTransform = P->GetCurrentTransform();
}

clStandartPositioner::clStandartPositioner()
{
}

clStandartPositioner::~clStandartPositioner()
{
}

void clStandartPositioner::AfterConstruction()
{
	/// 1. max-like
	clPanOrbitPositioner* Cam = Env->Linker->Instantiate( "clPanOrbitPositioner" );
	Cam->FUpVector = LVector3( 0, 0, 1 );

	/// 2. orbiting
	clTrajectoryPositioner* Traj = Env->Linker->Instantiate( "clTrajectoryPositioner" );
	Traj->ResetTime();
	Traj->FSpeedFactor = 3.0f;

	Traj->FUpVector = LVector3( 0, 0, 1 );

	// 3. old "rail" positioner
	clRailPositioner* Rail = Env->Linker->Instantiate( "clRailPositioner" );
	Rail->FDirVector = LVector3( 0, 1, 0 );
	Rail->FUpVector = LVector3( 0, 0, 1 );
	Rail->FPosition = LVector3( 0, 0, 200 );
	Rail->FPan = +45.0f;

	FPositioners.push_back( Cam );
	FPositioners.push_back( Traj );
	FPositioners.push_back( Rail );

	SetMode( 0 );
}

/*
 * 08/01/2011
     It's here
*/
