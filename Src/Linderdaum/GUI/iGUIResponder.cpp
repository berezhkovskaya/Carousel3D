/**
 * \file iGUIResponder.cpp
 * \brief GUI responder
 * \version 0.5.93
 * \date 25/10/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "iGUIResponder.h"

void iGUIResponder::Event_PointerChanged( int PointerID, const LVector2& Pnt, bool Pressed )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_PointerChanged( PointerID, Pnt, Pressed );
	}
}

void iGUIResponder::Event_PointerMoved( int PointerID, const LVector2& Pnt )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_PointerMoved( PointerID, Pnt );
	}
}

void iGUIResponder::Event_Fling( LVector2 Velocity )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_Fling( Velocity );
	}
}

void iGUIResponder::Event_Drag( LVector2 Delta )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_Drag( Delta );
	}
}

bool iGUIResponder::Event_Key( iGUIResponder* Source, const int Key, const bool KeyState )
{
	if ( FGUIResponder )
	{
		return FGUIResponder->Event_Key( Source, Key, KeyState );
	}

	return true;
}

void iGUIResponder::Event_ViewportDetached( iGUIResponder* Source, clViewport* Viewport )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_ViewportDetached( Source, Viewport );
	}
}

void iGUIResponder::Event_Registered( iGUIResponder* Source )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_Registered( Source );
	}
}

void iGUIResponder::Event_UnRegistered( iGUIResponder* Source )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_UnRegistered( Source );
	}
}

void iGUIResponder::Event_Timer( iGUIResponder* Source, float DeltaTime )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_Timer( Source, DeltaTime );
	}
}

void iGUIResponder::Event_Minimized( iGUIResponder* Source )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_Minimized( Source );
	}
}

void iGUIResponder::Event_Maximized( iGUIResponder* Source )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_Maximized( Source );
	}
}

void iGUIResponder::Event_Shrinked( iGUIResponder* Source )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_Shrinked( Source );
	}
}

void iGUIResponder::Event_Dissolved( iGUIResponder* Source )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_Dissolved( Source );
	}
}

void iGUIResponder::Event_CaptureChanged( iGUIResponder* Source )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_CaptureChanged( Source );
	}
}

void iGUIResponder::Event_MouseInside( iGUIResponder* Source, const LVector2& Pnt )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_MouseInside( Source, Pnt );
	}
}

void iGUIResponder::Event_MouseOutside( iGUIResponder* Source, const LVector2& Pnt )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_MouseOutside( Source, Pnt );
	}
}

void iGUIResponder::Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_MouseLeft( Source, Pnt, KeyState );
	}
}

void iGUIResponder::Event_MouseRight( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_MouseRight( Source, Pnt, KeyState );
	}
}

void iGUIResponder::Event_LDoubleClick( iGUIResponder* Source, const LVector2& Pnt )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_LDoubleClick( Source, Pnt );
	}
}

void iGUIResponder::Event_RDoubleClick( iGUIResponder* Source, const LVector2& Pnt )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_RDoubleClick( Source, Pnt );
	}
}

void iGUIResponder::Event_Scroll( iGUIResponder* Source, const LGUIDirection Direction )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_Scroll( Source, Direction );
	}
}

void iGUIResponder::Event_StartDragging( iGUIResponder* Source, const LVector2& Pnt )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_StartDragging( Source, Pnt );
	}
}

void iGUIResponder::Event_StopDragging( iGUIResponder* Source, const LVector2& Pnt )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_StopDragging( Source, Pnt );
	}
}

void iGUIResponder::Event_StartResize( iGUIResponder* Source, const LVector2& Pnt )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_StartResize( Source, Pnt );
	}
}

void iGUIResponder::Event_StopResize( iGUIResponder* Source, const LVector2& Pnt )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_StopResize( Source, Pnt );
	}
}

void iGUIResponder::Event_EnterTopmost( iGUIResponder* Source )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_EnterTopmost( Source );
	}
}

void iGUIResponder::Event_LeaveTopmost( iGUIResponder* Source )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_LeaveTopmost( Source );
	}
}

void iGUIResponder::Event_PageActive( iGUIResponder* Source, clGUIPage* PrevPage )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_PageActive( Source, PrevPage );
	}
}

void iGUIResponder::Event_PageInactive( iGUIResponder* Source, clGUIPage* NextPage )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_PageInactive( Source, NextPage );
	}
}

void iGUIResponder::Event_PageAnimationActive( iGUIResponder* Source )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_PageAnimationActive( Source );
	}
}

void iGUIResponder::Event_PageAnimationInactive( iGUIResponder* Source )
{
	if ( FGUIResponder )
	{
		FGUIResponder->Event_PageAnimationInactive( Source );
	}
}

/////////////

void iGUIViewRenderer::PreRender()
{
	if ( FGUIViewRenderer ) FGUIViewRenderer->PreRender();
}

void iGUIViewRenderer::PostRender()
{
	if ( FGUIViewRenderer ) FGUIViewRenderer->PostRender();
}

void iGUIViewRenderer::SetRenderer( iGUIViewRenderer* Renderer )
{
	FGUIViewRenderer = Renderer;
}

/*
 * 25/10/2010           8
     It's here (splitted from iGUIRegion.cpp)
*/
