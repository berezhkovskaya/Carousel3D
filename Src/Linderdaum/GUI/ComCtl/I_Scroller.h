/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUIScroller_
#define _clGUIScroller_

#include "GUI/ComCtl/I_Panel.h"

#include "Renderer/iVertexArray.h"
#include "Renderer/RenderState.h"

/// Scroller
class scriptfinal clGUIScroller: public clGUIPanel
{
public:
	clGUIScroller(): FDirection( GUIDIR_LEFT ) {};
	virtual ~clGUIScroller() {};

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS()
	//
	// clGUIScroller
	//
	virtual void    SetScrollingDirection( const LGUIDirection Direction ) { FDirection = Direction; };
public:
	virtual void    Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState );
	virtual void    Event_LDoubleClick( iGUIResponder* Source, const LVector2& Pnt );
private:
	LGUIDirection FDirection;
};

#endif

/*
 * 22/04/2005
     Implemented
 * 19/04/2005
     It's here
*/
