/*
   (Part of Linderdaum Engine)
   Version 0.5.74
   (02/05/2007)
   (C) Sergey Kosarevsky, 2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUIListBox_
#define _clGUIListBox_

#include "LString.h"
#include "GUI/ComCtl/I_BorderPanel.h"

/// List box
class scriptfinal clGUIListBox: public clGUIBorderPanel
{
public:
	clGUIListBox(): FCurrentItem( 0 ),
		FOffset( 0 ) {};

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS();
	//
	// iGUIView interface
	//
	virtual void    PreRender();
	//
	// clGUIListBox
	//
	virtual void       ClearList();
	virtual void       AddItem( const LString& ItemName );
	virtual LString    GetCurrentItem() const;
	/* Property( Name="Item",     Type=string,  Setter=AddItem,  Getter=GetCurrentItem ) */
public:
	virtual bool    Event_Key( iGUIResponder* Source, const int Key, const bool KeyState );
	virtual void    Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState );
	virtual void    Event_Scroll( iGUIResponder* Source, const LGUIDirection Direction );
private:
	void    UpdateCurrentItem();
	int     GetVisibleItems() const;
	bool    IsScrollUpPossible() const;
	bool    IsScrollDownPossible() const;
private:
	LStr::clStringsVector FItems;
	int                   FCurrentItem;
	int                   FOffset;
};

#endif

/*
 * 02/05/2007
     GetCurrentItem()
 * 05/04/2007
     SaveToXLMLStream()
 * 30/03/2007
     It's here
*/
