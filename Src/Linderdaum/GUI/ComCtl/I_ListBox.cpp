/*
   (Part of Linderdaum Engine)
   Version 0.5.74
   (02/05/2007)
   (C) Sergey Kosarevsky, 2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_ListBox.h"

#include "Engine.h"
#include "Environment.h"
#include "GUI/GUIManager.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/Canvas.h"

#include "LKeys.h"
#include "LColors.h"

void clGUIListBox::ClearList()
{
	FItems.clear();

	UpdateCurrentItem();
}

void clGUIListBox::AddItem( const LString& ItemName )
{
	FItems.push_back( ItemName );

	UpdateCurrentItem();
}

bool clGUIListBox::Event_Key( iGUIResponder* Source, const int Key, const bool KeyState )
{
	if ( Key == LK_WHEELUP   || Key == LK_UP   )
	{
		Event_Scroll( Source, GUIDIR_UP   );
	}

	if ( Key == LK_WHEELDOWN || Key == LK_DOWN )
	{
		Event_Scroll( Source, GUIDIR_DOWN );
	}

	return clGUIBorderPanel::Event_Key( Source, Key, KeyState );
}

void clGUIListBox::Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState )
{
	if ( !KeyState )
	{
		ExecuteCommandsStack();
	}

	clGUIBorderPanel::Event_MouseLeft( Source, Pnt, KeyState );
}

void clGUIListBox::Event_Scroll( iGUIResponder* Source, const LGUIDirection Direction )
{
	switch ( Direction )
	{
		case GUIDIR_UP:

			if ( IsScrollUpPossible() )
			{
				FOffset--;
			}

			break;
		case GUIDIR_DOWN:

			if ( IsScrollDownPossible() )
			{
				FOffset++;
			}

			break;
	}

	clGUIBorderPanel::Event_Scroll( Source, Direction );
}

void clGUIListBox::UpdateCurrentItem()
{
	FCurrentItem = FOffset + static_cast<int>( ( GetMousePos().Y ) / Env->GUI->GetStringHeight() ) - 1;

	if ( FCurrentItem > static_cast<int>( FItems.size() ) )
	{
		FCurrentItem = 0;
	}

	if ( FCurrentItem < 1                                 )
	{
		FCurrentItem = 0;
	}

	if ( GetConsoleVariable() && FCurrentItem > 0 )
	{
		GetConsoleVariable()->SetString( FItems[ FCurrentItem-1 ] );
	}
}

LString clGUIListBox::GetCurrentItem() const
{
	return ( FCurrentItem > 0 ) ? FItems[ FCurrentItem-1 ] : "";
}

int clGUIListBox::GetVisibleItems() const
{
	return static_cast<int>( GetHeight() / Env->GUI->GetStringHeight() ) - 3;
}

bool clGUIListBox::IsScrollUpPossible() const
{
	return ( FOffset > 0 );
}

bool clGUIListBox::IsScrollDownPossible() const
{
	return ( FOffset + GetVisibleItems() + 1 < static_cast<int>( FItems.size() ) );
}

void clGUIListBox::PreRender()
{
	clGUIBorderPanel::PreRender();

	UpdateCurrentItem();

	/*
	   OutTextXY( 0.03f,
	             -0.02f,
	              "Current item: "+LStr::ToStr(FCurrentItem),
	              LC_Green );
	*/
	if ( IsScrollUpPossible() )
	{
		OutTextXY( CenterTextHorizontal( "^" ),  -0.02f, "^", LC_Green );
	}

	if ( IsScrollDownPossible() )
	{
		OutTextXY( CenterTextHorizontal( "\\/" ),  GetHeight() - 0.02f, "\\/", LC_Green );
	}

	for ( int i = FOffset; i < static_cast<int>( FItems.size() ); ++i )
	{
		if ( i > FOffset + GetVisibleItems() )
		{
			break;
		}

		LVector4 Color = LC_White;

		float Y = 0.0f + Env->GUI->GetStringHeight() * ( i - FOffset );

		if ( i == FCurrentItem - 1 )
		{
			const float Y1Offset = 0.030f;
			const float Y2Offset = 0.031f;

			Env->Renderer->GetCanvas()->Rect( GetX1(),
			                                  GetY1() + Y + Y1Offset,
			                                  GetX2(),
			                                  GetY1() + Y + Y2Offset + Env->GUI->GetStringHeight(),
			                                  LC_Blue );

			Color = LC_Red;
		}

		OutTextXY( 0.01f, Y, FItems[i], Color );
	}

}

/*
 * 02/05/2007
     GetCurrentItem()
 * 05/04/2007
     SaveToXLMLStream()
 * 30/03/2007
     It's here
*/
