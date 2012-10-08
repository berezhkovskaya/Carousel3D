/**
 * \file I_Page.h
 * \brief GUI page - similar to GUI window but always occupies the whole screen and only one page can be active at a time
 * \version 0.6.02
 * \date 08/08/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clGUIPage_
#define _clGUIPage_

#include "GUI/iGUIView.h"

/// GUI page - similar to GUI window but always occupies the whole screen and only one page can be active at a time
class scriptfinal clGUIPage: public iGUIView
{
public:
	clGUIPage():
		FTapOnSwitch( true ),
		FFallbackPage( "" ),
		FFallbackTransition( "" ) {}

	virtual ~clGUIPage();

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS()
	//
	// iObject interface
	//
	virtual void    AfterConstruction();
	//
	// iGUIRegion interface
	//
	virtual bool    ContainsCaption( const LVector2& Pnt ) const { return false; };
	virtual bool    ContainsCloseButton( const LVector2& Pnt ) const { return false; };
	virtual bool    ContainsResizeCorner( const LVector2& Pnt ) const { return false; };
	//
	// iGUIView interface
	//
	virtual bool    IsPage() const { return true; };
	virtual bool    IsViewDockable() const { return false; };
	virtual bool    IsViewDraggable() const { return false; };
	virtual bool    IsViewSizeable() const { return false; };
	virtual bool    IsPageActive() const;
	virtual void    RenderHierarchy();

	/// Should we continue moving to the Fallback page ?
	virtual bool    HasFallback() { return !FFallbackPage.empty(); }

public:
	// GUI events (standart Esc press behaviour)
	virtual bool    Event_Key( iGUIResponder* Source, const int Key, const bool KeyState );
public:
	// Properties
	PROPERTY( Name = FallbackPage, Type = string, FieldName = FFallbackPage )
	PROPERTY( Name = FallbackTransition, Type = string, FieldName = FFallbackTransition )
	PROPERTY( Name = TapOnSwitch, Type = bool, FieldName = FTapOnSwitch )

	/// Haptic feedback occurs if this is true
	bool FTapOnSwitch;

	/// The page we return to when the Esc press occurs
	LString FFallbackPage;

	/// The page we use as the transition animation (for New API)
	LString FFallbackTransition;
};

#endif

/*
 * 08/08/2011
     It's here
*/
