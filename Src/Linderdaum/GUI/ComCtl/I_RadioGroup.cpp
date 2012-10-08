/*
   (Part of Linderdaum Engine)
   Version 0.5.73
   (05/04/2007)
   (C) Sergey Kosarevsky, 2005-2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_RadioGroup.h"
#include "I_RadioButton.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/CVars.h"
#include "GUI/GUIManager.h"

#include "LColors.h"

void clGUIRadioGroup::PreRender()
{
	clGUIBorderPanel::PreRender();

	static const float VerticalPos = Env->GUI->GetDefaultVerticalTitlePosition() +
	                                 Env->GUI->GetDefaultWindowBorderHeight();

	OutTextXY( CenterTextHorizontal( GetTitle() ),
	           VerticalPos,
	           GetTitle(),
	           LC_Red );
}

void clGUIRadioGroup::Event_Registered( iGUIResponder* Source )
{
	clGUIBorderPanel::Event_Registered( Source );

	if ( !FInitualCheckedButton.empty() )
	{
		CheckButton( FInitualCheckedButton );
	}
}

void clGUIRadioGroup::RadioButtonActivated( clGUIRadioButton* RadioButton )
{
	for ( size_t i = 0; i != GetTotalSubViews(); ++i )
	{
		if ( clGUIRadioButton* RB = dynamic_cast<clGUIRadioButton*>(  GetSubView( i ) ) )
		{
			if ( RB != RadioButton ) { RB->UnCheck(); }
		}
	}

	FCheckedButton = RadioButton;

	if ( GetConsoleVariable() ) { GetConsoleVariable()->SetString( RadioButton->GetObjectID() ); }
}

void clGUIRadioGroup::UnCheckAll()
{
	for ( size_t i = 0; i != GetTotalSubViews(); ++i )
	{
		if ( clGUIRadioButton* RB = dynamic_cast<clGUIRadioButton*>( GetSubView( i ) ) )
		{
			RB->UnCheck();
		}
	}

	FCheckedButton = NULL;
}

clGUIRadioButton* clGUIRadioGroup::GetCheckedButton()
{
	return FCheckedButton;
}

void clGUIRadioGroup::CheckButton( const LString& ButtonID )
{
	for ( size_t i = 0; i != GetTotalSubViews(); ++i )
	{
		if ( clGUIRadioButton* RB = dynamic_cast<clGUIRadioButton*>( GetSubView( i ) ) )
		{
			if ( RB->GetObjectID() == ButtonID )
			{
				RB->Check();

				return;
			}
		}
	}
}

void clGUIRadioGroup::Event_Timer( iGUIResponder* Source, float DeltaTime )
{
	if ( GetConsoleVariable() )
	{
		if ( FCheckedButton )
		{
			if ( GetConsoleVariable()->GetString() != FCheckedButton->GetObjectID() )
			{
				CheckButton( GetConsoleVariable()->GetString() );
			}
		}
		else
		{
			CheckButton( GetConsoleVariable()->GetString() );
		}
	}
}

/*
 * 05/04/2007
     SaveToXLMLStream()
 * 01/05/2005
     UnCheckAll()
     GetCheckedButton()
 * 08/04/2005
     It's here
*/
