/*
   (Part of Linderdaum Engine)
   Version 0.5.73
   (05/04/2007)
   (C) Sergey Kosarevsky, 2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUISidebar_
#define _clGUISidebar_

#include "GUI/ComCtl/I_DialogWindow.h"

/// Sidebar
class scriptfinal clGUISidebar: public clGUIDialogWindow
{
public:
	clGUISidebar();
	virtual ~clGUISidebar();

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS()
	//
	// iGUIView interface
	//
	virtual void    PreRender();
	virtual void    PostRender();
public:
	/* Property( Name="HiddenX",     Type=float,  FieldName=FHiddenX ) */
	/* Property( Name="HiddenY",     Type=float,  FieldName=FHiddenY ) */
	float    FHiddenX;
	float    FHiddenY;
public:
	virtual void    Event_Registered( iGUIResponder* Source );
	virtual void    Event_MouseInside( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_MouseOutside( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState );
	virtual void    Event_Timer( iGUIResponder* Source, float DeltaTime );
private:
	float    FTargetX;
	float    FTargetY;
};

#endif

/*
 * 05/04/2007
     SaveToXLMLStream()
 * 18/03/2007
     It's here
*/
