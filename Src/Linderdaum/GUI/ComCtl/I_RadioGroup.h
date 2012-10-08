/*
   (Part of Linderdaum Engine)
   Version 0.5.73
   (05/04/2007)
   (C) Sergey Kosarevsky, 2005-2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUIRadioGroup_
#define _clGUIRadioGroup_

#include "GUI/ComCtl/I_BorderPanel.h"

class clGUIRadioButton;

/// Radio group
class scriptfinal clGUIRadioGroup: public clGUIBorderPanel
{
public:
	clGUIRadioGroup(): FCheckedButton( NULL ),
		FInitualCheckedButton() {};

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS()
	//
	// iGUIView interface
	//
	virtual void    PreRender();
	//
	// clGUIRadioGroup
	//
	virtual void                 RadioButtonActivated( clGUIRadioButton* RadioButton );
	virtual void                 UnCheckAll();
	virtual void                 CheckButton( const LString& ButtonID );
	virtual clGUIRadioButton*    GetCheckedButton();
public:
	virtual void    Event_Timer( iGUIResponder* Source, float DeltaTime );
	virtual void    Event_Registered( iGUIResponder* Source );
public:
	clGUIRadioButton*    FCheckedButton;
	/* Property( Name="Check",     Type=string,  FieldName=FInitualCheckedButton ) */
	LString              FInitualCheckedButton;
};

#endif

/*
 * 05/04/2007
     SaveToXLMLStream()
 * 08/04/2005
     It's here
*/
