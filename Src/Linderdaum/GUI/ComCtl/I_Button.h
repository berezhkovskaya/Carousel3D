/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _iGUIButton_
#define _iGUIButton_

#include "GUI/ComCtl/I_Panel.h"

/// Base class for different buttons
class scriptfinal iGUIButton: public clGUIPanel
{
public:
	iGUIButton() {};
	virtual ~iGUIButton() {};

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS()
	//
	// iGUIRegion interface
	//
	virtual bool         ContainsCaption( const LVector2& Pnt ) const      { return false; }
	virtual bool         ContainsResizeCorner( const LVector2& Pnt ) const { return false; }
	virtual bool         ContainsCloseButton( const LVector2& Pnt ) const  { return false; }

public:
	virtual void         Event_Registered( iGUIResponder* Source );
	virtual void         Event_LDoubleClick( iGUIResponder* Source, const LVector2& Pnt );
};

#endif

/*
 * 30/01/2005
     It's here
*/
