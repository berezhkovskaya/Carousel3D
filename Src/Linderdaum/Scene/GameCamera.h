/**
 * \file GameCamera.h
 * \brief Game camera interface
 * \version 0.6.02
 * \date 16/09/2011
 * \author Viktor Latypov, 2009
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __LGameCamera__h__included__
#define __LGameCamera__h__included__

#include "Core/iObject.h"

#include "Math/LMatrix.h"

class clCVar;

class LCameraInfo;

TODO ( "quaternion transformations to avoid problems with Euler angles" )

/**
   Light-weight camera class : returns the ModelView matrix
   Can track objects or provide specific movement modes using LCameraInfo/Update()

   The easiest way to use this camera class :

      // declare the variable
      LCamera myCamera;

      // somewhere in user-interaction handling code
      MyCamera.SetPosition(...);
      MyCamera.SetAngles(...);

      // somewhere in the rendering code
      LMatrix4 Transform = MyCamera.GetModelViewMatrix();

   Camera has minimal state : Position and Angles

   If someone needs dynamic smoothly-moving camera, then
   one should call the Update() method, supplying a LCameraInfo object,
   which tells the desired position.
**/
class LCamera
{
public:
	LCamera(): FPosition( 0, 0, 0 ), FPitch( 0.0 ), FPan( 0.0 ), FRoll( 0.0 ), FCameraInfo( NULL )
	{
	}

	/// Get current camera position
	LVector3 GetPosition() const { return FPosition; }

	/// Get current camera angles (x - pan, y - roll, z - pitch)
	LVector3 GetAngles() const { return LVector3( FPan, FRoll, FPitch ); }

	/// Set new camera position
	void SetPosition( const LVector3& Position );

	/// Add offset to camera position
	void AddPosition( const LVector3& DeltaPosition );

	/// Set new camera angles (x - pan, y - roll, z - pitch)
	void SetAngles( const LVector3& Angles );

	/// Set individual angle values
	void SetAnglesA( float Pitch, float Pan, float Roll );

	/// Set Pitch value
	void SetPitch( float Pitch ) { FPitch = Pitch; }

	/// Set Pan value
	void SetPan( float Pan ) { FPan = Pan; }

	/// Set Roll value
	void SetRoll( float Roll ) { FRoll = Roll; }

	/// Get Pitch value
	float GetPitch() const { return FPitch; }

	/// Get Pan value
	float GetPan() const { return FPan; }

	/// Get Roll value
	float GetRoll() const { return FRoll; }

	/// Get current transformation
	LMatrix4 GetModelViewMatrix() const;

	/// Get current viewing direction
	LVector3 GetViewingDirection() const;

	/// Get current side direction
	LVector3 GetSideDirection() const;

	/**
	   Advanced method : used to provide smooth camera movement
	   Updates camera position and angles if it is dynamically tracking something
	   CameraInfo is used to retrieve the desired position/angles
	**/
	void Update( float DeltaTime );

	LCameraInfo* GetCameraInfo() const { return FCameraInfo; }

	void SetCameraInfo( LCameraInfo* CamInfo ) { FCameraInfo = CamInfo; }
protected:
	LCameraInfo* FCameraInfo;
private:
	LVector3 FPosition;
	float    FPan;
	float    FPitch;
	float    FRoll;
};

/**
   Something which returns desired camera position/angles
   Also stores DampingParameters and DampingFlags

   For the basic usage of LCamera this class is not (!) required
**/
class LCameraInfo
{
public:
	LCameraInfo();
	virtual ~LCameraInfo() {};

	/**
	   Returns the desired position for camera
	**/
	virtual LVector3 GetDesiredPosition() const { return Position; }

	/**
	   Returns the desired Pan angle for camera
	**/
	virtual float   GetDesiredXAngle() const { return Pan; }

	/**
	   Returns the desired Roll angle for camera
	**/
	virtual float   GetDesiredYAngle() const { return Roll; }

	/**
	   Returns the desired Pitch angle for camera
	**/
	virtual float   GetDesiredZAngle() const { return Pitch; }

	/**
	   Update the state of camera info
	**/
	virtual void     Update( float DeltaSeconds ) {}

	/**
	   Returns flag to enable LinearDamping
	**/
	virtual bool     UseLinearDamping() const { return false; }

	/**
	   Returns flag to enable AngularDamping
	**/
	virtual bool     UseAngularDamping() const { return false; }

	/**
	   Usually we use angles, but sometimes it is necessary to specify the rotation matrix directly
	**/
	virtual bool     UseMatrixOrientation() const { return false; }
	virtual void     SetOrientation( const LMatrix4& Orientation ) { FOrientation = Orientation; }

	virtual LMatrix4 GetTransformationFromPositionAndOrientation() const;

	void SetPosition( const LVector3& ThePos )
	{
		Position = ThePos;
	}
	void SetAngles( float x, float y, float z )
	{
		Pan = x;
		Roll = y;
		Pitch = z;
	}

	virtual float   GetLinearDamping() const { return FLinearDamping; }
	virtual float   GetPitchDamping() const { return FPitchDamping; }
	virtual float   GetPanDamping() const { return FPanDamping; }
	virtual float   GetRollDamping() const { return FRollDamping; }

	virtual void     SetLinearDamping( float LinDamp ) { FLinearDamping = LinDamp; }
	virtual void     SetPitchDamping( float PitchDamp ) { FPitchDamping  = PitchDamp; }
	virtual void     SetPanDamping( float PanDamp ) { FPanDamping    = PanDamp; }
	virtual void     SetRollDamping( float RollDamp ) { FRollDamping   = RollDamp; }

	void     SetCamera( LCamera* Cam ) { FCamera = Cam; }
	LCamera* GetCamera() const { return FCamera; }
private:
	LCamera* FCamera;

	float FLinearDamping;
	float FPanDamping;
	float FPitchDamping;
	float FRollDamping;
protected:
	// for the direct manipulation in subclasses
	LVector3 Position;
	float Pitch;
	float Pan;
	float Roll;
	LMatrix4 FOrientation;
};

/**
   Handles movement of the LCamera object
   and updates transformation in specified scenenode (?)

   If clGameCamera is not attached to SceneNode, then
   the user can call GetModelViewMatrix() to get current rendering transformation
**/
class scriptfinal clGameCamera: public iObject
{
public:
	clGameCamera();

	//
	// iObject interface
	//
	virtual void    AfterConstruction();

	// Get access to actual camera parameters
	virtual LCamera& GetCamera() { return FCamera; }

	FWD_EVENT_HANDLER( Event_TIMER );
private:
	/// Current position and angles holder
	LCamera      FCamera;

	/// Camera mode
	LCameraInfo* FCurrentCameraInfo;
};

#endif

/*
 * 23/04/2009
     SetCameraControlMode()
 * 10/04/2009
     Initial implementation
*/
