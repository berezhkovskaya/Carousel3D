/*
   (Part of Linderdaum Engine)
   Version 0.05.69
   (20/11/2006)
   (C) Sergey Kosarevsky, 2006
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_TabControl.h"

#include "Environment.h"
#include "GUI/GUIManager.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/Canvas.h"

#include "LColors.h"

void clGUITabControl::Select( const LString& Name )
{
	FSelectedID = Name;

	clGUITab* Tab = dynamic_cast<clGUITab*>( FindSubViewByID( Name ) );

	if ( Tab ) { TabActivated( Tab ); }
}

bool clGUITabControl::EndLoad()
{
	bool Result = clGUIBorderPanel::EndLoad();

	RegenerateTabsList();

	Select( FSelectedID );

	return Result;
}

void clGUITabControl::RegenerateTabsList()
{
	FTabsList.clear();

	float X = 0.02f;

	for ( size_t i = 0; i != GetTotalSubViews(); ++i )
	{
		clGUITab* Tab = dynamic_cast<clGUITab*>( GetSubView( i ) );

		if ( Tab )
		{
			sTab NewTab;

			NewTab.FX     = X;
			NewTab.FWidth = Env->GUI->GetStringWidth( Tab->GetTitle() + "  ", NULL );
			NewTab.FTab   = Tab;

			FTabsList.push_back( NewTab );

			X += NewTab.FWidth;

			TabActivated( Tab );
		}
	}

}

void clGUITabControl::AddView( iGUIView* View )
{
	clGUIBorderPanel::AddView( View );

	clGUITab* Tab = dynamic_cast<clGUITab*>( View );

	if ( Tab )
	{
		TabActivated( Tab );

		RegenerateTabsList();
	}
}

void clGUITabControl::AddViews( const clViewsList& Views )
{
	clGUIBorderPanel::AddViews( Views );

	for ( size_t i = 0; i != Views.size(); i++ )
	{
		clGUITab* Tab = dynamic_cast<clGUITab*>( Views[i] );

		if ( Tab ) { TabActivated( Tab ); }
	}

	RegenerateTabsList();
}

void clGUITabControl::RemoveView( iGUIView* View )
{
	clGUIBorderPanel::RemoveView( View );

	RegenerateTabsList();
}

void clGUITabControl::PreRender()
{
	clGUIBorderPanel::PreRender();

	LVector2 Pos = GetMousePos();

//   OutTextXY( 0.0f, -0.05f, Pos.ToString(','), LC_White );

	FSelectedTab = NULL;

	const float TextOffset = -0.025f;

	for ( size_t i = 0; i != FTabsList.size(); ++i )
	{
		LVector4 Color = LC_Red;

		if ( FActiveTab )
		{
			float Y = FActiveTab->GetY1();

			if ( FTabsList[i].FTab == FActiveTab )
			{
				Env->Renderer->GetCanvas()->Rect( GetX1() + FTabsList[i].FX,
				                                  GetY1() + 0.005f,
				                                  GetX1() + FTabsList[i].FX + FTabsList[i].FWidth,
				                                  //GetY1() + Env->GUI->GetStringHeight() + 0.005f,
				                                  Y,
				                                  LC_Blue );
				Color = LC_Green;
			}
		}

		if ( IsMouseOver() )
		{
			if ( Pos.X > FTabsList[i].FX &&
			     Pos.Y > 0 &&
			     Pos.X < FTabsList[i].FX + FTabsList[i].FWidth &&
			     Pos.Y < FTabsList[i].FTab->GetY1() /*0.02f*/ )
			{
				FSelectedTab = FTabsList[i].FTab;

				Color = LC_Yellow;
			}
		}

		OutTextXY( FTabsList[i].FX, TextOffset, FTabsList[i].FTab->GetTitle(), Color );
	}
}

void clGUITabControl::Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState )
{
	if ( FSelectedTab && KeyState )
	{
		TabActivated( FSelectedTab );
	}
}

void clGUITabControl::TabActivated( clGUITab* Tab )
{
	for ( size_t i = 0; i != GetTotalSubViews(); ++i )
	{
		clGUITab* SubTab = dynamic_cast<clGUITab*>( GetSubView( i ) );

		if ( SubTab ) { SubTab->SetVisible( false ); }
	}

	if ( Tab ) { Tab->SetVisible( true ); }

	FActiveTab = Tab;
}

/*
 * 20/11/2006
     It's here
*/
