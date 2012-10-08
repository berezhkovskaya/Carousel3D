/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_Window.h"

#include "Engine.h"
#include "Environment.h"
#include "GUI/GUIManager.h"
#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"
#include "Resources/ResourcesManager.h"

void clGUIWindow::Event_Registered( iGUIResponder* Source )
{
	clGUIDialogWindow::Event_Registered( Source );
	//
	FResizeCornerShader = Env->Resources->LoadShader( "interface/shaders/resizecorner.shader" );
	FResizeCornerHighlightShader = Env->Resources->LoadShader( "interface/shaders/resizecorner_highlight.shader" );
	FResizeCornerGeometry = Env->GUI->GetDefaultTriangle();
	//
	FCloseButtonShader = Env->Resources->LoadShader( "interface/shaders/close.shader" );
	FCloseButtonHighlightShader = Env->Resources->LoadShader( "interface/shaders/close_highlight.shader" );
	FCloseButtonCaptionHighlightedShader = Env->Resources->LoadShader( "interface/shaders/close_caption_highlight.shader" );
	FCloseButtonGeometry = Env->GUI->GetDefaultRect();
}

clGUIWindow::~clGUIWindow()
{
}

bool clGUIWindow::ContainsResizeCorner( const LVector2& Pnt ) const
{
	LVector2 LocalPnt = Screen2Local( Pnt );

	return ( LocalPnt.X >= GetWidth() - Env->GUI->GetDefaultResizeCornerWidth() ) &&
	       ( LocalPnt.Y >= GetHeight() - Env->GUI->GetDefaultResizeCornerHeight() ) &&
	       ( LocalPnt.X <= GetWidth() ) &&
	       ( LocalPnt.Y <= GetHeight() );
}

void clGUIWindow::Event_Timer( iGUIResponder* Source, float DeltaTime )
{
	if ( FRequestingClose )
	{
		if ( IsMinimized() )
		{
			SetRegionState( REGION_SHRINKING );
		}

		if ( IsMaximized() )
		{
			Minimize();
		}
	}

	clGUIDialogWindow::Event_Timer( Source, DeltaTime );
}

bool clGUIWindow::ContainsCaption( const LVector2& Pnt ) const
{
	return clGUIDialogWindow::ContainsCaption( Pnt ) && ( !FCloseButtonHighlighted );
}

bool clGUIWindow::ContainsCloseButton( const LVector2& Pnt ) const
{
	LVector2 LocalPnt = Screen2Local( Pnt );

	return ( LocalPnt.X >= GetWidth() - Env->GUI->GetDefaultCloseButtonWidth() ) &&
	       ( LocalPnt.Y >= 0 ) &&
	       ( LocalPnt.X <= GetWidth() ) &&
	       ( LocalPnt.Y <= Env->GUI->GetDefaultCaptionHeight() );
}

void clGUIWindow::PreRender()
{
	clGUIDialogWindow::PreRender();

	static const float ResizeCornerW = Env->GUI->GetDefaultResizeCornerWidth();
	static const float ResizeCornerH = Env->GUI->GetDefaultResizeCornerHeight();

	UpdateUniformsV( FResizeCornerShader, LVector4( GetX2(), GetY2(), GetX2() - ResizeCornerW, GetY2() + ResizeCornerH ) );
	UpdateUniformsV( FResizeCornerHighlightShader, LVector4( GetX2(), GetY2(), GetX2() - ResizeCornerW, GetY2() + ResizeCornerH ) );

	FResizeCornerHighlighted ?
	Env->Renderer->AddBuffer( FResizeCornerGeometry, FResizeCornerHighlightShader, 1, false ) :
	Env->Renderer->AddBuffer( FResizeCornerGeometry, FResizeCornerShader, 1, false );

	static const float CloseButtonW    = Env->GUI->GetDefaultCloseButtonWidth();
	static const float CloseButtonH_D1 = Env->GUI->GetDefaultCloseButtonHeight_Delta1();
	static const float CloseButtonH_D2 = Env->GUI->GetDefaultCloseButtonHeight_Delta2();

	if ( FCloseButtonHighlighted )
	{
		UpdateUniformsV( FCloseButtonHighlightShader, LVector4( GetX2() - CloseButtonW, GetY1() + CloseButtonH_D1, GetX2(), GetY1() + CloseButtonH_D2 ) );
		Env->Renderer->AddBuffer( FCloseButtonGeometry, FCloseButtonHighlightShader, 1, false );
	}
	else
	{
		UpdateUniformsV( FCloseButtonShader, LVector4( GetX2() - CloseButtonW, GetY1() + CloseButtonH_D1, GetX2(), GetY1() + CloseButtonH_D2 ) );
		UpdateUniformsV( FCloseButtonCaptionHighlightedShader, LVector4( GetX2() - CloseButtonW, GetY1() + CloseButtonH_D1, GetX2(), GetY1() + CloseButtonH_D2 ) );

		ContainsCaption( Local2Screen( GetMousePos() ) ) ?
		Env->Renderer->AddBuffer( FCloseButtonGeometry, FCloseButtonCaptionHighlightedShader, 1, false ) :
		Env->Renderer->AddBuffer( FCloseButtonGeometry, FCloseButtonShader, 1, false );
	}
}

void clGUIWindow::Event_MouseInside( iGUIResponder* Source, const LVector2& Pnt )
{
	clGUIDialogWindow::Event_MouseInside( Source, Pnt );

	FCloseButtonHighlighted = ContainsCloseButton( Pnt );
	FResizeCornerHighlighted = IsBeenSized() || ContainsResizeCorner( Pnt );

	if ( FResizeCornerHighlighted )
	{
		Env->GUI->SetCursorForm( CFORM_SIZENWSE );
	}
}

void clGUIWindow::Event_MouseOutside( iGUIResponder* Source, const LVector2& Pnt )
{
	clGUIDialogWindow::Event_MouseOutside( Source, Pnt );

	FCloseButtonHighlighted = false;
	FResizeCornerHighlighted = IsBeenSized();
}

void clGUIWindow::Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState )
{
	clGUIDialogWindow::Event_MouseLeft( Source, Pnt, KeyState );

	if ( KeyState && ContainsCloseButton( Pnt ) )
	{
		FRequestingClose = true;
	}
}

/*
 * 06/04/2005
     Refactored
 * 10/02/2005
     Nice window borders
 * 06/02/2005
     Development...
 * 05/02/2005
     It's here
*/
