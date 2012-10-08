#pragma once

#ifndef __control_h__included__
#define __control_h__included__

#include "Math/LTransform.h"
#include "Core/iObject.h"
#include "Utils/LArray.h"

/**
   Helper classes for controller implementation

   Given a desired and current state vectors (or quaternions)
   these classes incrementally correct the state until it reaches the goal
*/

/// Some feedback controller
class netexportable scriptfinal LFeedbackLoop: public iObject
{
public:
	LFeedbackLoop(): FLocalTime( 0.0f ), FStepSize( 0.1f ), FMaxStepSize( 0.5f ), FGoalReached( false ) {}
	virtual ~LFeedbackLoop() {}

	/// Check if the goal is reached
	scriptmethod bool  GoalReached() const { return FGoalReached; }

	/// Internal state update routine
	virtual void Update( float dt ) = 0;

public:
	/// Local time counter
	float FLocalTime;

	/** Property(Name=StepSize,    Type=float, FieldName=FStepSize)    */
	/** Property(Name=MaxStepSize, Type=float, FieldName=FMaxStepSize) */

	/// Maximum allowed step size
	float FMaxStepSize;

	/// Current step size
	float FStepSize;

protected:
	/// Internal flag to indicate current progress
	bool  FGoalReached;
};

/// Proportional linear/angular controller which drives a list of values/angles to specified values with a given speed
class netexportable scriptfinal LProportionalFeedbackLoop : public LFeedbackLoop
{
public:
	LProportionalFeedbackLoop() {}
	virtual ~LProportionalFeedbackLoop() {}

	/// Get number of state variables
	scriptmethod int   GetNumGoals() const  { return static_cast<int>( FGoal.size() ); }

	/// Set the gain
	scriptmethod void  SetGain( int i, float g )         { FGain[i] = g;      }

	/// Get the gain
	scriptmethod float GetGain( int i ) const     { return FGain[i];   }

	/// Set one of the goals
	scriptmethod void  SetGoal( int i, float G )  { FGoal[i] = G; FGoalReached = false;  }

	/// Get one of the goals
	scriptmethod float GetGoal( int i ) const     { return FGoal[i];   }

	/// Set one of the current vars
	scriptmethod void  SetState( int i, float S ) { FState[i] = S; FGoalReached = false; }

	/// Get one of the current angles
	scriptmethod float GetState( int i ) const    { return FState[i];  }

	/// Mark the variable as an angle
	scriptmethod void  SetAngle( int i, bool B )  { FAngle[i] = B;     }

	/// Get angular value flag
	scriptmethod bool  GetAngle( int i ) const    { return FAngle[i];  }

	/// Update the object
	virtual      void  Update( float dt );

	/** Property(Name=Goal,  Type=float, IndexType=int, FieldName=FGoal,  NetIndexedGetter=GetGoal,  NetIndexedSetter=SetGoal)  */
	/** Property(Name=Gain,  Type=float, IndexType=int, FieldName=FGain,  NetIndexedGetter=GetGain,  NetIndexedSetter=SetGain)  */
	/** Property(Name=State, Type=float, IndexType=int, FieldName=FState, NetIndexedGetter=GetState, NetIndexedSetter=SetState) */
	/** Property(Name=Angle, Type=bool,  IndexType=int, FieldName=FAngle, NetIndexedGetter=GetAngle, NetIndexedSetter=SetAngle) */

public:
	/// Proportional gain for each channel
	LArray<float>  FGain;

	/// Goal for each channel
	LArray<float>  FGoal;

	/// List of current values
	LArray<float>  FState;

	/// Flag to denote angular values
	LArray<bool>   FAngle;
};

/// Feedback loop
class netexportable scriptfinal LQuaternionFeedbackLoop: public LFeedbackLoop
{
public:
	LQuaternionFeedbackLoop(): FGain( 0.0f ), FGoal(),  FState() {}
	virtual ~LQuaternionFeedbackLoop() {}

	virtual      void Update( float dt );

	/// Set current goal
	scriptmethod void SetGoal( const LQuaternion& G ) { FGoal = G; FGoalReached = false; }

	/// Get current goal
	scriptmethod LQuaternion GetGoal() const { return FGoal; }

	scriptmethod void  SetGain( float G ) { FGain = G; }

	scriptmethod float GetGain() const  { return FGain; }

	scriptmethod void  SetState( const LQuaternion& S ) { FState = S; }

	scriptmethod LQuaternion  GetState() const { return FState; }

	/** Property(Name=Goal,  Type=quat,  FieldName=FGoal)  */
	/** Property(Name=Gain,  Type=float, FieldName=FGain)  */
	/** Property(Name=State, Type=quat,  FieldName=FState) */

public:
	/// Gain coefficient
	float  FGain;

	/// Current goal
	LQuaternion   FGoal;

	/// Current orientation
	LQuaternion   FState;
};

/// Drive a specified rigid body (a.k.a. transform) to the given position and orientation
class netexportable scriptfinal LTransformFeedbackLoop: public LFeedbackLoop
{
public:
	LTransformFeedbackLoop() {}
	virtual ~LTransformFeedbackLoop() {}

	/// Update the object
	virtual      void       Update( float dt );

	/// Set new goal
	scriptmethod void       SetGoal( const LTransform& G );

	/// Get current goal
	scriptmethod LTransform GetGoal();

	/// Set current state
	scriptmethod void       SetState( const LTransform& G );

	/// Get current state
	scriptmethod LTransform GetState();

	/// Set controller parameter for orientation
	scriptmethod void  SetOrientationGain( float G ) { FQuatFeedback.SetGain( G ); }

	/// Set controller parameter for position
	scriptmethod void  SetPositionGain( float G )    { for ( int i = 0 ; i < 3 ; i++ ) { FPosFeedback.SetGain( i, G ); } }

	/// Get controller parameter for orientation
	scriptmethod float GetOrientationGain() const  { return FQuatFeedback.GetGain(); }

	/// Get controller parameter for position
	scriptmethod float GetPositionGain() const     { return FPosFeedback.GetGain( 0 ); }

private:
	/// Auxillary orientation interpolator
	LQuaternionFeedbackLoop  FQuatFeedback;

	/// Auxillary position interpolator
	LProportionalFeedbackLoop FPosFeedback;
};

#endif // #if included

/*
 * 08/01/2011
     It's here
*/
