/**
 * \file Gestures.h
 * \brief Gestures handling helpers
 * \version 0.6.01
 * \date 22/07/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __Gestures__
#define __Gestures__

#include "Core/iObject.h"
#include "Math/LMatrix.h"
#include "GUI/iGUIResponder.h"

enum LFlingStatus
{
   FLING_MOVING,
   FLING_IDLE
};

class LMotionData
{
public:
	LMotionData(): FTouchPoints() {};
	scriptmethod void               Clear() { FTouchPoints.clear(); };

	scriptmethod size_t             GetNumTouchPoints() const { return FTouchPoints.size(); }
	scriptmethod const sTouchPoint& GetTouchPoint( size_t Idx )    const { return FTouchPoints[Idx]; }
	scriptmethod const LVector2&    GetTouchPointPos( size_t Idx ) const { return FTouchPoints[Idx].FPoint; }
	scriptmethod int                GetTouchPointID( size_t Idx )  const { return FTouchPoints[Idx].FID; }
	scriptmethod const std::vector<sTouchPoint>& GetTouchPoints() const { return FTouchPoints; }

	scriptmethod void               AddTouchPoint( const sTouchPoint& TouchPoint )
	{
		for ( size_t i = 0; i != FTouchPoints.size(); i++ )
		{
			if ( FTouchPoints[i].FID == TouchPoint.FID )
			{
				FTouchPoints[i] = TouchPoint;
				return;
			}
		}

		FTouchPoints.push_back( TouchPoint );
	}

	scriptmethod void               AddTouchPointID( int ID, const LVector2& Point, double TimeStamp ) { AddTouchPoint( sTouchPoint( ID, Point, TimeStamp ) ); }

	scriptmethod void               RemoveTouchPointID( int ID )
	{
		for ( size_t i = 0; i < FTouchPoints.size(); )
		{
			if ( FTouchPoints[i].FID == ID )
			{
				FTouchPoints[i] = FTouchPoints.back();
				FTouchPoints.pop_back();
				continue;
			}

			i++;
		}
	}

private:
	std::vector<sTouchPoint> FTouchPoints;
};

/// Generic gesture handler, useful if you want to access multitouches manually
class clGestureHandler: public iObject
{
public:
	clGestureHandler():
		FMotionData(),
		FMotionDataValid( false ),
		FMoving( false ),
		FInitialPoint( 0, LVector2(), 0.0 ),
		FCurrentPoint( 0, LVector2(), 0.0 ) {}

	//
	// iObject interface
	//
	virtual void    AfterConstruction();

	//
	// clGestureHandler
	//

	/// return true if MotionData contains meaningful info
	virtual bool    IsGestureValid() const { return FMotionDataValid; }

	/// return true if dragging valid
	virtual bool    IsDraggingValid() const;

	/// access to the actual motion data, no changes allowed
	virtual const LMotionData* GetMotionData() const { return &FMotionData; }

	/// get the vector between the initial and the current position
	virtual LVector2   GetPositionDelta() const;

	/// get translation matrix for moving with a click
	virtual LMatrix4   GetTranslationMatrix() const;

	/// get the position of the first touch point in the current gesture, immutable within a gesture
	virtual const sTouchPoint&    GetInitialTouchPoint() const { return FInitialPoint; }

	/// get the position of the current touch point in the current gesture, that means the current position of the last point touched
	virtual const sTouchPoint&    GetCurrentTouchPoint() const { return FCurrentPoint; }

	#pragma region Gesture listeners - responders
	virtual void               AddListener( iGestureResponder* Listener ) { FListeners.push_back( Listener ); }
	virtual size_t             GetNumListeners() const { return FListeners.size(); };
	virtual iGestureResponder* GetListener( size_t i ) const { return FListeners[i]; };
	#pragma endregion

	FWD_EVENT_HANDLER( Event_MOTION );
protected:
	virtual void    UpdateGesture() {};

protected:
	LMotionData FMotionData;
private:
	std::vector<iGestureResponder*> FListeners;

	bool                FMotionDataValid;
	mutable bool        FMoving;
	mutable sTouchPoint FInitialPoint;
	mutable sTouchPoint FCurrentPoint;
};

/**
   \brief Fling gesture handler

   Fling autoregisters itself on creation, but starts inactive

   Each time an update occurs
*/
class clFlingHandler: public clGestureHandler
{
public:
	clFlingHandler(): FActive( false ), FSensitivity( 1.0f ) { Reset(); }
	virtual ~clFlingHandler();

	virtual void AfterConstruction();

	/// Clear current state
	scriptmethod void Reset()
	{
		FValue = vec2( 0 );
		FStep  = vec2( 0 );

		FMinValue = vec2( -100000.0f );
		FMaxValue = vec2( +100000.0f );

		FValuePeriod = vec2( 0 );

		FStepsLeft.X = FStepsLeft.Y = 0;

		FFlingStatus = FLING_IDLE;
	}

	/// Switch to incremental rotation (half-pi rotations, 2pi-periodic)
	scriptmethod void SetIncrementalRotationMode( const vec2& ElementaryRotation, const vec2& AnimSpeed )
	{
		FValuePeriod = vec2( Linderdaum::Math::PI2 );
		FStepGranularity = AnimSpeed;

		FValueGranularity = ElementaryRotation;
	}

	/// Handle isolated free-form strokes and send OnFling events
	scriptmethod void SetStrokeMode()
	{
		FStepGranularity = vec2( 1.0f, 1.0f );
		FValueGranularity = vec2( 1.0f, 1.0f );

		FValuePeriod = vec2( 0.0f );
	}

	/// Default event handler
	FWD_EVENT_HANDLER( Event_TIMER );

	#pragma region Flinging properties

	/// Set new target value (animation will do the rest)
	scriptmethod void SetValue( const vec2& NewValue );

	/// Get current value
	scriptmethod vec2 GetValue() const { return FValue; }

	/// Activate or deactivate flinging
	scriptmethod void SetActive( bool Active ) { FActive = Active; }

	/// Check flinging activity
	scriptmethod bool IsActive() const { return FActive; }

public:
	/// Are we active (updating)
	bool FActive;

	/// Single step (1 page or, e.g., PI/2 for rotations) - animation speed
	vec2 FStepGranularity;

	/// The discrete sizeof a single motion (angle of elementary rotation or some interval between items in coverflow)
	vec2 FValueGranularity;

	/// Sensitivity a.k.a. movement speed
	vec2 FSensitivity;

	/// Period for values (2 * PI for rotations)
	vec2 FValuePeriod;

	/// Current fling step
	vec2 FStep;

	/// Minimum axis values
	vec2 FMinValue;

	/// Maximum axis values
	vec2 FMaxValue;

	/// Internal Fling state
	LFlingStatus FFlingStatus;

	/** Property(Name=Granularity, Type=vec2, FieldName=FValueGranularity) */
	/** Property(Name=Period,      Type=vec2, FieldName=FValuePeriod) */
	/** Property(Name=MinValue,    Type=vec2, FieldName=FMinValue) */
	/** Property(Name=MaxValue,    Type=vec2, FieldName=FMaxValue) */
	/** Property(Name=Step,        Type=vec2, FieldName=FStep) */

	/** Property(Name=Sensitivity, Type=vec2, FieldName=FSensitivity) */

	/** Property(Name=Active,      Type=bool, Getter=IsActive, Setter=SetActive) */

	#pragma endregion

private:

	/// Current X/Y values, altered by fling
	vec2 FValue;

	/// How much steps left to add to FValue
	LVector2i FStepsLeft;

	#pragma region Value updates

private:

	/// Function to update the Fling state. Increments values and optionally send events
	void Update( float dt )
	{
		if ( this->IsDraggingValid() && FFlingStatus == FLING_IDLE )
		{
			vec2 Pos = this->GetPositionDelta();

			if ( fabs( Pos.x ) > fabs( Pos.y ) )
			{
				SetTheValue( Pos.x > 0 ? +1.0f : -1.0f, FStepGranularity.X, &FStep.X, &FStepsLeft.X );
			}
			else
			{
				SetTheValue( Pos.y > 0 ? +1.0f : -1.0f, FStepGranularity.Y, &FStep.Y, &FStepsLeft.Y );
			}
		}

		vec2 RealStep( FStep.X, FStep.Y ); // (FStep.X * FSensitivity.X * dt, FStep.Y * FSensitivity.Y * dt);

		UpdateValue( &FValue.Y, RealStep.Y, &FStepsLeft.Y, FMinValue.Y, FMaxValue.Y );
		UpdateValue( &FValue.X, RealStep.X, &FStepsLeft.X, FMinValue.X, FMaxValue.X );

		/// Deactivate
		if ( FStepsLeft.Y == 0 && FStepsLeft.X == 0 ) { FFlingStatus = FLING_IDLE; }

		ClipValue( &FValue.X, FValueGranularity.X, FValuePeriod.X, RealStep.X );
		ClipValue( &FValue.Y, FValueGranularity.Y, FValuePeriod.Y, RealStep.Y );
	}

	void SetTheValue( float Dir, float Granularity /* PI/2 */, float* Step, int* StepsLeft )
	{
		*Step = Dir * Granularity / 10.0f;
		*StepsLeft = int( Granularity / fabs( *Step ) );
		FFlingStatus = FLING_MOVING;
	}

	void UpdateValue( float* ValuePtr, float Step, int* StepsLeft, float MinVal, float MaxVal )
	{
		if ( *StepsLeft > 0 )
		{
			( *StepsLeft )--;
			*ValuePtr += Step;
		}

		if ( *ValuePtr < MinVal ) { *ValuePtr = MinVal; }

		if ( *ValuePtr > MaxVal ) { *ValuePtr = MaxVal; }
	}

	void ClipValue( float* ValuePtr, float Granularity /* Math::PI/2 */, float ValuePeriod /* Math::PI2 */, float Step )
	{
		if ( std::fabs( std::fmod( *ValuePtr, Granularity ) ) < std::fabs( Step ) * 0.5f )
		{
			*ValuePtr = Granularity * int( *ValuePtr / Granularity );

			if ( ValuePeriod > 0.0f ) { *ValuePtr = std::fmod( *ValuePtr, ValuePeriod ); }
		}
	}

	#pragma endregion
};

/// Pinch-zoom gesture handler
class clPinchZoomHandler: public clGestureHandler
{
public:
	clPinchZoomHandler(): FPinchZoomValid( false ), FZoomFactor( 1.0f ), FInitialDistance( 1.0f ) {}

	//
	// clPinchZoomHandler
	//

	/// return true if the belowmentioned getters return meaningful data
	virtual bool    IsPinchZoomValid() const;

	/// get the current zoom factor, initial zoom factor is always 1.0f
	virtual float   GetPinchZoomFactor() const { return FZoomFactor; }

	/// get the distance between touch point just after the pinch zoom was initiated
	virtual float   GetPinchInitialDistance() const { return FInitialDistance; }

	/// get the initial position of the first point (the one that was touched at first)
	virtual LVector2   GetPinchInitialPoint1() const { return FInitialPoint1; };

	/// get the initial position of the second point
	virtual LVector2   GetPinchInitialPoint2() const { return FInitialPoint2; };

	/// get the initial center point of 2 touches
	virtual LVector2   GetPinchInitialCenter() const { return FInitialCenter; };

	/// get the current position of the first point (the one that was touched at first)
	virtual LVector2   GetPinchCurrentPoint1() const { return FCurrentPoint1; };

	/// get the current position of the second point
	virtual LVector2   GetPinchCurrentPoint2() const { return FCurrentPoint2; };

	/// get the current center point of 2 touches
	virtual LVector2   GetPinchCurrentCenter() const { return FCurrentCenter; };

	/// get the XY scaling matrix of the current zoom, relative to the initial Point1
	virtual LMatrix4   GetPinchZoomMatrix() const;

	/// get the Z-axis rotation matrix of the current zoom, relative to the initial Point1 and Point2
	virtual LMatrix4   GetPinchRotationMatrix() const;

	/// get the translation matrix between initial and current centers
	virtual LMatrix4   GetPinchTranslationMatrix() const;

	/// get the combined transformation matrix GetPinchZoomMatrix()*GetPinchTranslationMatrix()*GetPinchRotationMatrix()
	virtual LMatrix4   GetPinchCombinedMatrix() const;
private:
	mutable bool     FPinchZoomValid;
	mutable float    FZoomFactor;
	mutable float    FInitialDistance;
	mutable LVector2 FInitialPoint1;
	mutable LVector2 FInitialPoint2;
	mutable LVector2 FCurrentPoint1;
	mutable LVector2 FCurrentPoint2;
	mutable LVector2 FInitialCenter;
	mutable LVector2 FCurrentCenter;
};


#endif

/*
 * 22/07/2011
     Added center point
 * 21/07/2011
     It's here
*/
