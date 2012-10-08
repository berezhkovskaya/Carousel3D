/**
 * \file iGUIResponder.h
 * \brief GUI responder
 * \version 0.5.93
 * \date 25/10/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iGUIResponder_
#define _iGUIResponder_

#include "Platform.h"

#include "Math/LVector.h"

enum LGUIDirection
{
	GUIDIR_LEFT,
	GUIDIR_RIGHT,
	GUIDIR_UP,
	GUIDIR_DOWN
};

struct sTouchPoint
{
	sTouchPoint(): FID( -1 ), FPoint( 0.0f ), FTimeStamp( 0.0 ) {};
	sTouchPoint( int TouchID, const LVector2& Point, double TimeStamp ): FID( TouchID ), FPoint( Point ), FTimeStamp( TimeStamp ) {};
	int      FID;
	LVector2 FPoint;
	double   FTimeStamp;
};

class clGUIPage;
class clViewport;

/// responding to gestures-related events
class iGestureResponder
{
public:
	/// send after pointer is up or down
	virtual void Event_PointerChanged( int PointerID, const LVector2& Pnt, bool Pressed ) {};
	/// send after pointer is moved
	virtual void Event_PointerMoved( int PointerID, const LVector2& Pnt ) {};
	/// send after pointer is up
	virtual void Event_Fling( LVector2 Velocity ) {};
	/// send after pointer is up
	virtual void Event_Fling2( const sTouchPoint& Down, const sTouchPoint& Up ) {};
	/// send while pointer is moving
	virtual void Event_Drag( LVector2 Delta ) {};
	/// send while pointer is moving
	virtual void Event_Drag2( const sTouchPoint& Down, const sTouchPoint& Current ) {};
};

/// responding to general GUI events
class iGUIResponder: public iGestureResponder
{
public:
	iGUIResponder(): FGUIResponder( NULL ) {};

#pragma region Gestures
	virtual void Event_PointerChanged( int PointerID, const LVector2& Pnt, bool Pressed );
	virtual void Event_PointerMoved( int PointerID, const LVector2& Pnt );
	virtual void Event_Fling( LVector2 Velocity );
	virtual void Event_Drag( LVector2 Delta );
#pragma endregion

#pragma region GUI events
	virtual void    SetResponder( iGUIResponder* Responder )
	{
		FGUIResponder = Responder;
	};
	virtual bool    Event_Key( iGUIResponder* Source, const int Key, const bool KeyState );
	virtual void    Event_ViewportDetached( iGUIResponder* Source, clViewport* Viewport );
	virtual void    Event_Registered( iGUIResponder* Source );
	virtual void    Event_UnRegistered( iGUIResponder* Source );
	virtual void    Event_Timer( iGUIResponder* Source, float DeltaTime );
	virtual void    Event_Minimized( iGUIResponder* Source );
	virtual void    Event_Maximized( iGUIResponder* Source );
	virtual void    Event_Shrinked( iGUIResponder* Source );
	virtual void    Event_Dissolved( iGUIResponder* Source );
	virtual void    Event_CaptureChanged( iGUIResponder* Source );
	virtual void    Event_MouseInside( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_MouseOutside( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState );
	virtual void    Event_MouseRight( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState );
	virtual void    Event_LDoubleClick( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_RDoubleClick( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_Scroll( iGUIResponder* Source, const LGUIDirection Direction );
	virtual void    Event_StartDragging( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_StopDragging( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_StartResize( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_StopResize( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_EnterTopmost( iGUIResponder* Source );
	virtual void    Event_LeaveTopmost( iGUIResponder* Source );
	/// called if a page becomes active, i.e. can accept user input
	virtual void    Event_PageActive( iGUIResponder* Source, clGUIPage* PrevPage );
	virtual void    Event_PageInactive( iGUIResponder* Source, clGUIPage* NextPage );
	/// called if a page will participate in animation, i.e. rendering will be performed
	virtual void    Event_PageAnimationActive( iGUIResponder* Source );
	virtual void    Event_PageAnimationInactive( iGUIResponder* Source );
#pragma endregion
private:
	iGUIResponder*     FGUIResponder;
};

class iGUIViewRenderer
{
public:
	iGUIViewRenderer():FGUIViewRenderer(NULL) {}
	//
	// iGUIViewRenderer
	//
	virtual void    PreRender();
	virtual void    PostRender();
	virtual void    SetRenderer( iGUIViewRenderer* Renderer );
	virtual iGUIViewRenderer* GetRenderer() const { return FGUIViewRenderer; }
protected:
	iGUIViewRenderer* FGUIViewRenderer;
};

#endif

/*
 * 25/10/2010
     It's here (splitted from iGUIRegion.h)
*/
