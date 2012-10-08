/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_Button.h"

#include "Engine.h"
#include "Environment.h"
#include "GUI/GUIManager.h"

void iGUIButton::Event_Registered( iGUIResponder* Source )
{
	clGUIPanel::Event_Registered( Source );

	SetBevelRadius( Env->GUI->GetDefaultButtonBevelRadius() );
};

void iGUIButton::Event_LDoubleClick( iGUIResponder* Source, const LVector2& Pnt )
{
	Event_MouseLeft( Source, Pnt, true );
}


/*
 * 10/04/2005
     Double click behaves the same way as single click
 * 30/01/2005
     It's here
*/
