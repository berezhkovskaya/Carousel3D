/**
 * \file I_StaticRect.h
 * \brief Container do draw a colored rect
 * \version 0.6.06
 * \date 21/01/2012
 * \author Sergey Kosarevsky, 2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clGUIStaticRect_
#define _clGUIStaticRect_

#include "GUI/iGUIView.h"

/// Container do draw a colored rect
class scriptfinal clGUIStaticRect: public iGUIView
{
public:
	clGUIStaticRect();

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
 * 21/01/2012
     It's here
*/
