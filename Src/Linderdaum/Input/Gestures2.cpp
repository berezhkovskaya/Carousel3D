/**
 * \file Gestures.cpp
 * \brief Gestures handling helpers implementation
 * \version 0.6.02
 * \date 27/12/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

//
// WORK IN PROGRESS. Will replace classes from Gestures.cpp in the near future.
//

#include "Environment.h"
#include "Gestures2.h"
#include "Utils/Viewport.h"
#include "GUI/iGUIResponder.h"

void clFlingHandler2::UpdateGesture()
{
	clGestureHandler::UpdateGesture();

	if ( this->IsDraggingValid() )
	{
		// react on single point dragging
		if ( this->GetPositionDelta().Length() > FlingThresholdSensitivity )
		{
			size_t NumListeners = GetNumListeners();
			const sTouchPoint& Pt1 = GetInitialTouchPoint();
			const sTouchPoint& Pt2 = GetCurrentTouchPoint();

			for ( size_t i = 0; i != NumListeners; i++ )
			{
				GetListener( i )->Event_Drag( this->GetPositionDelta() );
				GetListener( i )->Event_Drag2( Pt1, Pt2 );
			}

			FFlingWasValid = true;
		}
	}
	else
	{
		// finish gesture
		if ( FFlingWasValid )
		{
			size_t NumListeners = GetNumListeners();
			const sTouchPoint& Pt1 = GetInitialTouchPoint();
			const sTouchPoint& Pt2 = GetCurrentTouchPoint();

			if ( this->GetPositionDelta().Length() > FlingStartSensitivity )
			{
				for ( size_t i = 0; i != NumListeners; i++ )
				{
					// will be only 1 event
					GetListener( i )->Event_Fling( this->GetPositionDelta() );
					GetListener( i )->Event_Fling2( Pt1, Pt2 );
				}
			}
			else
			{
				for ( size_t i = 0; i != NumListeners; i++ )
				{
					GetListener( i )->Event_Drag( LVector2( 0.0f ) );
					GetListener( i )->Event_Drag2( Pt1, Pt2 );
				}
			}

			FFlingWasValid = false;
		}
	}
}

/*
 * 22/07/2011
     clPinchZoomHandler: all transforms are relative to the gesture center point
 * 21/07/2011
     It's here
*/
