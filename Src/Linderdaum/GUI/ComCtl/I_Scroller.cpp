/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_Scroller.h"

#include "Engine.h"
#include "Environment.h"

void clGUIScroller::Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState )
{
	clGUIPanel::Event_MouseLeft( Source, Pnt, KeyState );

	if ( !KeyState ) { return; }

	if ( iGUIView* View = GetParentView() )
	{
		View->Event_Scroll( Source, FDirection );
	}
}

void clGUIScroller::Event_LDoubleClick( iGUIResponder* Source, const LVector2& Pnt )
{
	clGUIPanel::Event_LDoubleClick( Source, Pnt );

	Event_MouseLeft( Source, Pnt, true );
}

/*
 * 22/04/2005
     Implemented
 * 19/04/2005
     It's here
*/
