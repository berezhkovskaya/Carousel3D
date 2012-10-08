/*
   (Part of Linderdaum Engine)
   Version 0.5.74
   (13/08/2007)
   (C) Sergey Kosarevsky, 2005-2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUIStaticWidget_
#define _clGUIStaticWidget_

#include "GUI/iGUIView.h"

/// Container for GUI elements that will not receive any messages
class scriptfinal clGUIStaticWidget: public iGUIView
{
public:
	clGUIStaticWidget();

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()
	//
	// iGUIRegion interface
	//
	virtual bool    Contains( const LVector2& Pnt ) const             { return false; }
	virtual bool    ContainsCaption( const LVector2& Pnt ) const      { return false; }
	virtual bool    ContainsResizeCorner( const LVector2& Pnt ) const { return false; }
	virtual bool    ContainsCloseButton( const LVector2& Pnt ) const  { return false; }
	//
	// iGUIView interface
	//
	virtual void    PreRender();
	virtual bool    CanFocus() const { return false; };
};

#endif

/*
 * 13/08/2007
     Color
 * 11/02/2005
     It's here
*/
