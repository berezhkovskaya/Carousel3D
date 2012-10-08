/*
   (Part of Linderdaum Engine)
   Version 0.5.74
   (13/08/2007)
   (C) Sergey Kosarevsky, 2005-2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_StaticWidget.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/CVars.h"
#include "GUI/GUIManager.h"

#include "LColors.h"

clGUIStaticWidget::clGUIStaticWidget()
{
	SetDefaultColor( LC_Flesh );
}

void clGUIStaticWidget::PreRender()
{
	iGUIView::PreRender();

	LVector4 Color( GetDefaultColor() );
	Color.W = GetViewOpacity();

	if ( GetConsoleVariable() )
	{
		OutTextXY( 0, 0, GetConsoleVariable()->GetString(), Color );
	}
	else if ( !GetTitle().empty() )
	{
		OutTextXY( 0, 0, GetTitle(), Color );
	}
}

/*
 * 13/08/2007
     Color
 * 11/02/2005
     It's here
*/
