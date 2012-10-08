/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUICheckBox_
#define _clGUICheckBox_

#include "GUI/ComCtl/I_Checkable.h"

/// Check box
class scriptfinal clGUICheckBox: public iGUICheckable
{
public:
	clGUICheckBox() {};

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS()

	//
	// iObject interface
	//
	virtual void    AfterConstruction();

	/* Property( Name="CheckedShader",     Type=string,  Setter=SetCheckedShaderName,   Getter=GetCheckedShaderName   ) */
	/* Property( Name="UnCheckedShader",   Type=string,  Setter=SetUnCheckedShaderName, Getter=GetUnCheckedShaderName ) */
public:
	virtual void    Event_Registered( iGUIResponder* Source );
	virtual void    Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState );
};

#endif

/*
 * 11/02/2005
     It's here
*/
