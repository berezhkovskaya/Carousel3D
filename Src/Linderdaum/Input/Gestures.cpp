/**
 * \file Gestures.cpp
 * \brief Gestures handling helpers implementation
 * \version 0.6.01
 * \date 21/07/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"
#include "Gestures.h"
#include "Utils/Viewport.h"
#include "GUI/iGUIResponder.h"

void clGestureHandler::AfterConstruction()
{
	iObject::AfterConstruction();

	CONNECTOBJ( L_EVENT_MOTION, &clGestureHandler::Event_MOTION, this );
}

void clGestureHandler::Event_MOTION( LEvent Event, const LEventArgs& Args )
{
	// unpack the motion data
	int ContactID = ( int )Args.FPtrArg;
	LVector2 Pos( Args.FFloatArg, Args.FFloatArg1 );
	bool Pressed = Args.FBoolArg;

	LMotionFlag Flag = ( LMotionFlag )( ( int )Args.FObjArg );

	// reset the current gesture
	if ( ContactID == L_MOTION_START )
	{
		FMotionDataValid = false;
		FMotionData.Clear();

		return;
	}
	else
	{
		// complete the current gesture
		if ( ContactID == L_MOTION_END )
		{
			FMotionDataValid = true;

			Env->Logger->LogP( L_DEBUG, "MotionDataValid: %i", ( int )FMotionData.GetNumTouchPoints() );

			UpdateGesture();
		}
		// add a point to the current gesture
		else
		{
			if ( Pressed )
			{
				FMotionData.AddTouchPoint( sTouchPoint( ContactID, Pos, Env->GetSeconds() ) );
			}

			size_t NumListeners = this->GetNumListeners();

			switch ( Flag )
			{
				case L_MOTION_MOVE:

					for ( size_t i = 0; i != NumListeners; i++ )
					{
						GetListener( i )->Event_PointerMoved( ContactID, Pos );
					}

					break;

				case L_MOTION_UP:
				case L_MOTION_DOWN:

					for ( size_t i = 0; i != NumListeners; i++ )
					{
						GetListener( i )->Event_PointerChanged( ContactID, Pos, Flag == L_MOTION_DOWN );
					}

					break;
			}
		}
	}

	Env->Logger->LogP( L_PARANOID, "Event_MOTION: id: %i x: %f, y: %f, pressed: %i", ContactID, Pos.x, Pos.y, Pressed );
}

bool clGestureHandler::IsDraggingValid() const
{
	// single-point dragging
	if ( IsGestureValid() && FMotionData.GetNumTouchPoints() == 1 && FMotionData.GetTouchPointID( 0 ) == 0 )
	{
		if ( FMoving )
		{
			FCurrentPoint = FMotionData.GetTouchPoint( 0 );
		}
		else
		{
			FMoving       = true;
			FInitialPoint = FMotionData.GetTouchPoint( 0 );
			// to prevent glitch, since no current point is set
			return false;
		}
	}
	else
	{
		FMoving = false;
	}

	return FMoving;
}

LVector2 clGestureHandler::GetPositionDelta() const
{
	return FCurrentPoint.FPoint - FInitialPoint.FPoint;
}

LMatrix4 clGestureHandler::GetTranslationMatrix() const
{
	return LMatrix4::GetTranslateMatrix( LVector3( GetPositionDelta(), 0.0f ) );
}

void clFlingHandler::AfterConstruction()
{
	clGestureHandler::AfterConstruction();

	CONNECTOBJ( L_EVENT_TIMER, &clFlingHandler::Event_TIMER, this );
}

clFlingHandler::~clFlingHandler()
{
	Env->Disconnect( L_EVENT_TIMER, Utils::Bind( &clFlingHandler::Event_TIMER, this ) );
}

void clFlingHandler::SetValue( const vec2& NewValue )
{
	FValue = NewValue;

	FStepsLeft = LVector2i( 0 );

	// recalculate Step ?
}

void clFlingHandler::Event_TIMER( LEvent Event, const LEventArgs& Args )
{
	( void )Event;

	if ( FActive ) { Update( Args.FFloatArg ); }
}

bool clPinchZoomHandler::IsPinchZoomValid() const
{
	if ( IsGestureValid() && FMotionData.GetNumTouchPoints() == 2 )
	{
		const LVector2& Pos1( FMotionData.GetTouchPointPos( 0 ) );
		const LVector2& Pos2( FMotionData.GetTouchPointPos( 1 ) );

		float NewDistance = ( Pos1 - Pos2 ).Length();

		if ( FPinchZoomValid )
		{
			// do pinch zoom
			FZoomFactor    = NewDistance / FInitialDistance;
			FCurrentPoint1 = Pos1;
			FCurrentPoint2 = Pos2;
			FCurrentCenter = ( Pos1 + Pos2 ) * 0.5f;
		}
		else
		{
			// start new pinch zoom
			FInitialDistance = NewDistance;
			FPinchZoomValid  = true;
			FZoomFactor      = 1.0f;
			FInitialPoint1   = Pos1;
			FInitialPoint2   = Pos2;
			FInitialCenter = ( Pos1 + Pos2 ) * 0.5f;
			// to prevent glitch, since no current point is set
			return false;
		}
	}
	else
	{
		// stop pinch zoom
		FPinchZoomValid = false;
		FZoomFactor     = 1.0f;
	}

	return FPinchZoomValid;
}

LMatrix4 clPinchZoomHandler::GetPinchZoomMatrix() const
{
	// construct XY scale matrix
	LMatrix4 T1Matrix    = LMatrix4::GetTranslateMatrix( LVector3( -FCurrentCenter, 0.0f ) );
	LMatrix4 T2Matrix    = LMatrix4::GetTranslateMatrix( LVector3(  FCurrentCenter, 0.0f ) );

	LMatrix4 ScaleMatrix = LMatrix4::GetScaleMatrix( LVector3( FZoomFactor, FZoomFactor, 1.0f ) );

	return T1Matrix * ScaleMatrix * T2Matrix;
}

LMatrix4 clPinchZoomHandler::GetPinchRotationMatrix() const
{
	LMatrix4 M( LMatrix4::Identity() );

	M.RotateMatrix( -LVector3( FInitialPoint2 - FInitialPoint1, 0.0f ), -LVector3( FCurrentPoint2 - FCurrentPoint1, 0.0f ) );

	LMatrix4 T1Matrix = LMatrix4::GetTranslateMatrix( LVector3( -FCurrentCenter, 0.0f ) );
	LMatrix4 T2Matrix = LMatrix4::GetTranslateMatrix( LVector3(  FCurrentCenter, 0.0f ) );

	return T1Matrix * M * T2Matrix;
}

LMatrix4 clPinchZoomHandler::GetPinchTranslationMatrix() const
{
	return LMatrix4::GetTranslateMatrix( LVector3( FCurrentCenter - FInitialCenter, 0.0f ) );
}

LMatrix4 clPinchZoomHandler::GetPinchCombinedMatrix() const
{
	return GetPinchZoomMatrix() * GetPinchTranslationMatrix() * GetPinchRotationMatrix();
}


/*
 * 22/07/2011
     clPinchZoomHandler: all transforms are relative to the gesture center point
 * 21/07/2011
     It's here
*/
