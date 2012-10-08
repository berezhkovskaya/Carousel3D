#pragma once

#ifndef __CameraPositioner__included__h__
#define __CameraPositioner__included__h__

#include "Core/iObject.h"
#include "Math/LTransform.h"
#include "Utils/Event.h"
#include "Utils/LArray.h"

class clRigidBodyTrajectory;

class clGUISlider;
class clGUICheckBox;

/**
   \brief Abstract interface for camera transform calculation

   It is not very good, but here two functions are mixed:
      - transform calculation
      - input system polling

   I.e., given some input devices the positioner reads this device
*/
class netexportable iCameraPositioner : public iObject
{
public:
	iCameraPositioner() {}
	virtual ~iCameraPositioner() {}

	NET_EXPORTABLE()

	/// Install timer event handler for this camera positioner
	scriptmethod  void Register();

	/// Remove timer event handler for this camera positioner
	scriptmethod  void Unregister();

	/// Update internal camera positioner state
	virtual void Update( float dt ) = 0;

	virtual void HandleKey( int Key, bool State );

	/// Get current calculated transformation for the camera
	scriptmethod const LMatrix4& GetCurrentTransform() const { return FCurrentTransform; }

	/// Event_TIMER event handler (simple redirection to )
	FWD_EVENT_HANDLER( Event_TIMER );

	FWD_EVENT_HANDLER( Event_KEY );

protected:
	/// Current transformation for the camera
	LMatrix4     FCurrentTransform;

	/// Number of wheel ticks - increased on WheelUp and decreased on WheelDown
	int FWheelTicks;

private:
	/// Local timer/key event handlers
	LEventSubscriber::LEventHandler FTimerHandler, FKeyHandler;
};

/**
   \brief A switch between different camera modes

   Given a list of positioners, this camera positioner selects one of them in runtime
   and updates this selected positioner
*/
class scriptfinal netexportable clSelectionPositioner : public iCameraPositioner
{
public:
	clSelectionPositioner(): FCurrentMode( -1 ) {}
	virtual ~clSelectionPositioner() { ClearPositioners( true ); }

	/// Set current mode
	scriptmethod void SetMode( int i );

	/// Get current mode
	scriptmethod int  GetMode() const { return FCurrentMode; }

	/// Get one of the positioners
	scriptmethod iCameraPositioner* GetPositioner( int i ) const { return FPositioners[i]; }

	/// Change one of the positioners
	scriptmethod void               SetPositioner( int i, iCameraPositioner* P ) {}

	/// find positioner by name (object ID)
	scriptmethod iCameraPositioner*  FindPositioner( const LString& Name );

	/// Clear and delete positioners
	scriptmethod void ClearPositioners( bool Delete );

	/// Get the number of positioners
	scriptmethod int  GetNumPositioners() { return static_cast<int>( FPositioners.size() ); }

	/// Update current mode
	virtual void Update( float dt );

	virtual void HandleKey( int Key, bool State );

	/** Property(Name=Positioners, Type=iCameraPositioner, IndexType=int, FieldName=FPositioners, NetIndexedGetter=GetPositioner, NetIndexedSetter=SetPositioner) */

	/** Property(Name=CurrentMode, Type=int, Getter=GetMode, Setter=SetMode) */
public:
	/// Current camera mode
	int FCurrentMode;

	/// List of available positioners
	LArray<iCameraPositioner*>  FPositioners;
};

/**
   \brief Trajectory tracker for camera positioning

   Calculate an interpolated position and orientation on a trajectory
*/
class scriptfinal netexportable clTrajectoryPositioner : public iCameraPositioner
{
public:
	clTrajectoryPositioner();
	virtual ~clTrajectoryPositioner();

	virtual void AfterConstruction();

	/// Add/Remove timeline attached to FLocalTime
	scriptmethod void ToggleTimeLine( bool Visible );

	/// Circular orbit around static target
	scriptmethod void GenerateOrbit( float Elevation, float OrbitRadius, const LVector3& Target );

	NET_EXPORTABLE()

	scriptmethod float GetMinTime() const;
	scriptmethod float GetMaxTime() const;

	/// Set the camera to initial position
	scriptmethod void  ResetTime() { FLocalTime = GetMinTime(); }

	/// Read current time variable
	virtual void Update( float dt );

	/// Add on-screen tweaking GUI
	scriptmethod void AddControlUI();

	/// Remove on-screen tweaking GUI
	scriptmethod void RemoveControlUI();

	/** Property(Name=UpdateTime,   Type=bool,   FieldName=FUpdateTime)   */
	/** Property(Name=SpeedFactor,  Type=float,  FieldName=FSpeedFactor)  */
	/** Property(Name=TrackTarget,  Type=bool,   FieldName=FTrackTarget)  */
	/** Property(Name=StaticTarget, Type=vec3,   FieldName=FStaticTarget) */
	/** Property(Name=LocalTime,    Type=float,  FieldName=FLocalTime)    */

	/// .NET marshaller does not yet support class forwarding in properties
	/** Prop_erty(Name=Trajectory,   Type=RigidBodyTrajectory, FieldName=FTrajectory) */

	/** Property(Name=UpVector, Type=vec3,  FieldName=FUpVector) */

public:
	/// Speed of the time update
	float   FSpeedFactor;

	/// Local time tracker
	float   FLocalTime;

	/// Just use variable (if false) or directly update the time counter variable
	bool    FUpdateTime;

	/// Track static/dynamic target
	bool    FTrackTarget;

	/// Static target position
	LVector3    FStaticTarget;

	/// The Up vector
	LVector3    FUpVector;

	/// The trajectory to be followed
	clRigidBodyTrajectory* FTrajectory;

private:
	/// Usability timeline trackbar
	clGUISlider*   FTimeSlider;
	clGUISlider*   FSpeedSlider;
	clGUICheckBox* FTrackCheck;
	clGUICheckBox* FUpdateCheck;
};

/// A decorator which takes calculated position from another positioner and reaches it with some delay factor
// TODO : extract from LCamera/LCameraInfo
class scriptfinal netexportable clDampingPositioner : public iCameraPositioner
{
public:
	clDampingPositioner(): FOriginalCamera( NULL ) { FCurrentTransform = LMatrix4::Identity(); }
	virtual ~clDampingPositioner() {}

	virtual void Update( float dt );

	/// Attach this damper to another camera
	scriptmethod void LinkToCameraPositioner( iCameraPositioner* P );

public:
	/** Property(Name=UseDamping, Type=bool, FieldName=FUseDamping) */

	/** Property(Name=LinearDamping,  Type=float, FieldName=FLinearDampFactor) */
	/** Property(Name=AngularDamping, Type=float, FieldName=FAngularDampFactor) */

	/// Turn on damping
	bool  FUseDamping;

	/// Positional damping
	float FLinearDampFactor;

	/// Orientation damping
	float FAngularDampFactor;

	/// Link to source camera
	iCameraPositioner* FOriginalCamera;
};

/*

/// Track some point in space and move appropriately (using object axes or world axes)
/// This is the base class for First-person and Third-person cameras
/// TODO: move from AcesHigh
class scriptfinal netexportable LTrackingPositioner : public iCameraPositioner
{
};

*/

/**
   \brief Standart key/mouse-based controller

   GetSpeed() read the 'Shift' key and return position increments
*/
class scriptfinal netexportable clStdCameraPositioner: public iCameraPositioner
{
public:
	clStdCameraPositioner(): FPan( 0.0f ), FPitch( 0.0f ), FRoll( 0.0f ), FPosition( 0, 0, 0 ) {}
	virtual ~clStdCameraPositioner() {}

	virtual void Update( float dt )
	{
		FCurrentTransform = GetTransformationFromPositionAndOrientation();
	}

	// for the direct manipulation in subclasses
	LVector3 FPosition;

	float FPitch;
	float FPan;
	float FRoll;

protected:
	/// Get current increment (read 'Shift' key)
	float GetSpeed( float dt );


	LMatrix4 GetTransformationFromPositionAndOrientation() const
	{
		LMatrix4 RotationMatrix;
		RotationMatrix.FromPitchPanRoll( FPitch, FPan, FRoll );

		return LMatrix4::GetTranslateMatrix( -FPosition ) * RotationMatrix;
	}
};

/**
   \brief Common WASD/QZEC controller

   W/S  -  Forward/Backward
   A/D  -  Strafe left/right
   E/C  -  Look up/down
   Q/Z  -  Move up/down
*/
class scriptfinal netexportable clRailPositioner : public clStdCameraPositioner
{
public:
	clRailPositioner();
	virtual ~clRailPositioner() {}

	virtual void Update( float dt );

public:
	/// Current Up vector
	LVector3 FUpVector;

	/// Initial/Current view direction
	LVector3 FDirVector;
};

/**
   \brief Standart [mouse look] + [keyboard WSAD] controller

   W/S  -  Forward/Backward
   A/D  -  Strafe left/right
*/
/*
class netexportable WSADPositioner : public StdCameraPositioner
{
};
*/

/**
   \brief "Classic" controller

   Up/Down     -  Forward/Backward
   Left/Right  -  Turn left/right
   A/Z         -  Look up/down
   1/2         -  Move up/down
*/
class scriptfinal netexportable clFightingChessPositioner : public clStdCameraPositioner
{
public:
	clFightingChessPositioner() {}
	virtual ~clFightingChessPositioner() {}

	virtual void Update( float dt );
};

/// 3ds/Maya-like camera
class scriptfinal netexportable clPanOrbitPositioner : public iCameraPositioner
{
public:
	clPanOrbitPositioner();
	virtual ~clPanOrbitPositioner() {}

	/// Read mouse, keyboard, joysticks and call MakeStep to update the state
	virtual void Update( float dt );

	/// Internal update and transform calculation
	void MakeStep( float dt );

	/// Calculate initial transform for a given Up/Target/ViewPos triple
	scriptmethod void Reset();

#pragma region Instant state (set externally by some actual input controller)

	/// Are we orbiting
	bool   FOrbiting;

	/// Are we panning
	bool   FPanning;

	/// Are we zooming
	bool   FZooming;

	/// Zooming in
	bool   FZoomingIn;

	/// Zooming out
	bool   FZoomingOut;

	/// Amount of zoom-in
	float  FZoomOut;

	/// Amount of zoom-out
	float  FZoomIn;

	/// Last mouse change
	LVector3   FMouseDelta;

	/// Last panning value (might be taken from joystick axis)
	LVector3   FPanDelta;

	/// Last orbiting value
	LVector3   FOrbitDelta;

#pragma endregion

#pragma region Movement properties

	/** Property(Description="Default orbiting velocity", Category="Moving properties", Name=DefaultOrbitingVelocity, Type=float, FieldName=FDefaultOrbitingVelocity) */
	/** Property(Description="Default panning velocity",  Category="Moving properties", Name=DefaultPanningVelocity,  Type=float, FieldName=FDefaultPanningVelocity)  */
	/** Property(Description="Default zooming velocity",  Category="Moving properties", Name=DefaultZoomVelocity,     Type=float, FieldName=FDefaultZoomVelocity)     */
	/** Property(Description="Minimum panning velocity",  Category="Moving properties", Name=MinPanningVelocity,      Type=float, FieldName=FMinPanningVelocity)      */
	/** Property(Description="Minimum zooming velocity",  Category="Moving properties", Name=MinZoomingVelocity,      Type=float, FieldName=FMinZoomingVelocity)      */

	float  FDefaultOrbitingVelocity; // 0.25f;
	float  FDefaultPanningVelocity;  // 0.08f;
	float  FDefaultZoomVelocity;     // 0.25f;

	/// min panning velocity
	float  FMinPanningVelocity; // 0.08f;

	/// min zoom speed
	float  FMinZoomingVelocity; // 0.50f;

#pragma endregion

#pragma region Viewing parameters

	/** Property(Description="Current viewer position",    Category="Viewing parameters", Name=ViewerPosition, Type=vec3,  FieldName=FViewerPosition) */
	/** Property(Description="Camera Up vector",           Category="Viewing parameters", Name=UpVector,       Type=vec3,  FieldName=FUpVector)       */
	/** Property(Description="Current target",             Category="Viewing parameters", Name=Target,         Type=vec3,  FieldName=FTarget)         */
	/** Property(Description="Minimum distance to target", Category="Viewing parameters", Name=MinDistance,    Type=float, FieldName=FMinDistance)    */

	/// Viewer position
	LVector3  FViewerPosition;

	/// The Up vector
	LVector3  FUpVector;

	/// The target we are looking at
	LVector3  FTarget;

	/// Minimum possible distance to the object
	float FMinDistance;

#pragma endregion

#pragma region Calculated parameters

	/// Spherical coordinates of the viewer in target-centered frame
	LVector3  FSphericalCoords;

	/// Current view distance
	float FViewDistance;

	/// View direction calculated from Target and Viewer
	LVector3  FView;
#pragma endregion

private:
	/// Previous mouse position
	LVector3 FLastMouse;

	/// Current mouse position
	LVector3 FMouse;
};

/**
   \brief Utility class with all standart cameras: rail, orbit, max-like etc.
*/
class scriptfinal netexportable clStandartPositioner: public clSelectionPositioner
{
public:
	clStandartPositioner();
	virtual ~clStandartPositioner();

	virtual void AfterConstruction();

#pragma region Shortcuts

	scriptmethod void GenerateOrbit( float Elevation, float OrbitRadius, const LVector3& OrbitCenter )
	{
		GetTrajectoryPositioner()->GenerateOrbit( Elevation, OrbitRadius, OrbitCenter );
	}

	scriptmethod clRailPositioner* GetRail() const { return dynamic_cast<clRailPositioner*>( FPositioners[2] ); }
	scriptmethod clPanOrbitPositioner* GetMaxLike() const { return dynamic_cast<clPanOrbitPositioner*>( FPositioners[0] ); }
	scriptmethod clTrajectoryPositioner* GetTrajectoryPositioner() const { return dynamic_cast<clTrajectoryPositioner*>( FPositioners[1] ); }

#pragma endregion
};

#endif // #if included

/*
 * 08/01/2011
     It's here
*/
