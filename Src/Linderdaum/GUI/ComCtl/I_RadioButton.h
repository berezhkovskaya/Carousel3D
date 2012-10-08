/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUIRadioButton_
#define _clGUIRadioButton_

#include "GUI/ComCtl/I_Checkable.h"

/// Radio button
class scriptfinal clGUIRadioButton: public iGUICheckable
{
public:
	clGUIRadioButton() {};

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS()
	//
	// iGUICheckable interface
	//
	virtual void    Check();
public:
	virtual void    Event_Registered( iGUIResponder* Source );
	virtual void    Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState );
};

#endif

/*
 * 08/04/2005
     It's here
*/
