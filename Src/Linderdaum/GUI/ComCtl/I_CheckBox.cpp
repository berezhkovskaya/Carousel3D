/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_CheckBox.h"

#include "Engine.h"
#include "Environment.h"
#include "GUI/GUIManager.h"
#include "Renderer/iRenderContext.h"
#include "Resources/ResourcesManager.h"

void clGUICheckBox::AfterConstruction()
{
	iGUICheckable::AfterConstruction();

	SetUnCheckedShader( Env->Resources->LoadShader( "interface/shaders/checkbox.shader" ) );
	SetCheckedShader( Env->Resources->LoadShader( "interface/shaders/checkbox_checked.shader" ) );
}

void clGUICheckBox::Event_Registered( iGUIResponder* Source )
{
	iGUICheckable::Event_Registered( Source );


	FGeometry = Env->GUI->GetDefaultRect();
}

void clGUICheckBox::Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState )
{
	iGUICheckable::Event_MouseLeft( Source, Pnt, KeyState );

	if ( !KeyState && Contains( Pnt ) )
	{
		Toggle();
		ExecuteCommandsStack();
	}
}

/*
 * 09/04/2005
     Event_MouseLeft()
 * 12/02/2005
     It's here
*/
