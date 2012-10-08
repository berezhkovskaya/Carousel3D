/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_RadioButton.h"
#include "I_RadioGroup.h"

#include "Engine.h"
#include "Environment.h"
#include "GUI/GUIManager.h"
#include "Renderer/iRenderContext.h"
#include "Resources/ResourcesManager.h"

void clGUIRadioButton::Event_Registered( iGUIResponder* Source )
{
	iGUICheckable::Event_Registered( Source );

	FGeometry = Env->GUI->GetDefaultRect();

	SetUnCheckedShader( Env->Resources->LoadShader( "interface/shaders/RadioButton.shader" ) );
	SetCheckedShader( Env->Resources->LoadShader( "interface/shaders/RadioButton_checked.shader" ) );
};

void clGUIRadioButton::Check()
{
	iGUICheckable::Check();

	if ( iGUIView* View = GetParentView() )
	{
		if ( clGUIRadioGroup* RG = dynamic_cast<clGUIRadioGroup*>( View ) )
		{
			RG->RadioButtonActivated( this );
		}
	}
}

void clGUIRadioButton::Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState )
{
	iGUICheckable::Event_MouseLeft( Source, Pnt, KeyState );

	if ( !KeyState && Contains( Pnt ) )
	{
		Check();
		ExecuteCommandsStack();
	}
}


/*
 * 09/04/2005
     Event_MouseLeft()
 * 08/04/2005
     It's here
*/
