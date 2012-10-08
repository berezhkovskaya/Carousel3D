/*
   (Part of Linderdaum Engine)
   Version 0.5.74
   (29/05/2007)
   (C) Sergey Kosarevsky, 2006-2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUITabControl_
#define _clGUITabControl_

#include "LString.h"
#include "GUI/ComCtl/I_BorderPanel.h"

/// Tab
class scriptfinal clGUITab: public clGUIBorderPanel
{
public:
	clGUITab() {};
};

/// Tab control
class scriptfinal clGUITabControl: public clGUIBorderPanel
{
public:
	clGUITabControl() {};

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS()
	//
	// iObject interface
	//
	virtual bool    EndLoad();
	//
	// iGUIView interface
	//
	virtual void    AddView( iGUIView* View );
	virtual void    AddViews( const clViewsList& Views );
	virtual void    RemoveView( iGUIView* View );
	virtual void    PreRender();
	//
	// clGUITabControl
	//
	virtual void    TabActivated( clGUITab* Tab );
	virtual void    Select( const LString& Name );
	virtual LString GetSelected() const { return FActiveTab ? FActiveTab->GetObjectID() : ""; };
	/* Property( Name="Select",     Type=string,  Setter=Select,  Getter=GetSelected ) */
public:
	virtual void   Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState );
private:
	void    RegenerateTabsList();
private:
	/// Internal representation of a tab
	struct sTab
	{
		sTab(): FX( 0 ),
			FWidth( 0 ),
			FTab( NULL ) {};
		//
		float     FX;
		float     FWidth;
		clGUITab* FTab;
	};
	LString               FSelectedID;
	std::vector<sTab>     FTabsList;
	LStr::clStringsVector FAttachedTabs;
	clGUITab*             FActiveTab;
	clGUITab*             FSelectedTab;
};

#endif

/*
 * 29/05/2007
     Merged with I_Tab.h
 * 20/11/2006
     It's here
*/
