/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_Memo.h"

#include "Engine.h"
#include "Environment.h"
#include "GUI/GUIManager.h"

#include "LColors.h"

void clGUIMemo::AddLine( const LString& Line )
{
	push_back( Line, GetDefaultColor() );
}

void clGUIMemo::push_back( const LString& Line, const LVector4& Color )
{
	sMemoLine ML;

	ML.FLine  = Line;
	ML.FColor = Color;

	FLines.push_back( ML );
}

void clGUIMemo::push_front( const LString& Line, const LVector4& Color )
{
	sMemoLine ML;

	ML.FLine  = Line;
	ML.FColor = Color;

	FLines.push_front( ML );
}

void clGUIMemo::PreRender()
{
	clGUIBorderPanel::PreRender();

	int Offset = 0;

	for ( std::list<sMemoLine>::const_iterator i = FLines.begin(); i != FLines.end(); ++i )
	{
		OutTextXY( 0.03f, 0.0f + Env->GUI->GetStringHeight() * Offset++, ( *i ).FLine, ( *i ).FColor );
	}
}

/*
 * 06/05/2005
     Colored lines
 * 24/04/2005
     It's here
*/
