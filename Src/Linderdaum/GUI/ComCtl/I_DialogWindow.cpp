/*
   (Part of Linderdaum Engine)
   Version 0.5.73
   (05/04/2007)
   (C) Sergey Kosarevsky, 2005-2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_DialogWindow.h"

#include "Engine.h"
#include "Environment.h"
#include "GUI/GUIManager.h"
#include "Utils/Viewport.h"
#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iShaderProgram.h"
#include "Resources/ResourcesManager.h"
#include "LColors.h"

#include <algorithm>

using std::min;

void clGUIDialogWindow::SetWindowComposerShaderName( const LString& FileName )
{
	FWindowComposerShader = Env->Resources->LoadShader( FileName );
}

void clGUIDialogWindow::Event_Registered( iGUIResponder* Source )
{
	clGUIPanel::Event_Registered( Source );

	SetBevelRadius( Env->GUI->GetDefaultCaptionRadius() );

//   FGeometry->SetVertexAttribs( clGeomServ::CreateRect2D( 0, 0, 1, 1, 0, false, 20 ) );
	FGeometry = Env->GUI->GetDefaultBevelRect();
	//->SetVertexAttribs( clGeomServ::CreateBevelRect2D( 0, 0, 1, 1, 0, Env->GUI->GetDefaultCaptionRadius() ) );

	SetShader( Env->Resources->LoadShader( "interface/shaders/windowback.shader" ) );
	//
	FCaptionShader = Env->Resources->LoadShader( "interface/shaders/window.shader" );
	FCaptionHighlightShader = Env->Resources->LoadShader( "interface/shaders/window_high.shader" );
	//

	// borders
	FLeftBorderShader = Env->Resources->LoadShader( "interface/shaders/vborder_left.shader" );
	FRightBorderShader = Env->Resources->LoadShader( "interface/shaders/vborder_right.shader" );
	FBottomBorderShader = Env->Resources->LoadShader( "interface/shaders/bborder.shader" );

	// nice floating windows
	SetMaximizedHeight( GetHeight() );
	SetRegionState( REGION_MAXIMIZING );
	SetHeight( Env->GUI->GetDefaultCaptionHeight() );

	if ( !FWindowComposerShader )
	{
		// default composer
		FWindowComposerShader = Env->Resources->LoadShader( "interface/shaders/windowcomposer.shader" );
	}

	static const float CaptionRadius = Env->GUI->GetDefaultCaptionRadius();

	FCaptionGeometry = Env->GUI->GetDefaultBevelRect(); //SetVertexAttribs( clGeomServ::CreateBevelRect2D( 0, 0, 1, 1, 0, CaptionRadius ) );
	FLeftBorderGeometry = Env->GUI->GetDefaultRect(); //->SetVertexAttribs( clGeomServ::CreateRect2D( 0, 0, 1, 1, 0, false, 0 ) );
	FRightBorderGeometry = Env->GUI->GetDefaultRect(); //->SetVertexAttribs( clGeomServ::CreateRect2D( 0, 0, 1, 1, 0, false, 0 ) );
	FBottomBorderGeometry = Env->GUI->GetDefaultRect(); //->SetVertexAttribs( clGeomServ::CreateRect2D( 0, 0, 1, 1, 0, false, 0 ) );
}

clGUIDialogWindow::~clGUIDialogWindow()
{
}

bool clGUIDialogWindow::ContainsResizeCorner( const LVector2& Pnt ) const
{
	return false;
}

bool clGUIDialogWindow::IsViewDockable() const
{
	return true;
};

bool clGUIDialogWindow::IsViewDraggable() const
{
	return true;
};

bool clGUIDialogWindow::IsViewSizeable() const
{
	return true;
};

void clGUIDialogWindow::PreRender()
{
	// aquire buffer
	FWindowBuffer = Env->GUI->GetWindowBuffer();

	// use window composer
	FWindowBuffer->Bind( 0 );

	Env->Renderer->GetCanvas()->BeginTextPacket();

	Env->Renderer->ClearRenderTarget( true, true, false );

	clGUIPanel::PreRender();

	static const float CaptionHeight = Env->GUI->GetDefaultCaptionHeight();
	static const float CaptionLedge  = Env->GUI->GetDefaultCaptionLedge();
	static const float HalfCaptionHeight = CaptionHeight * 0.5f;
	static const float BorderWidth       = Env->GUI->GetDefaultWindowBorderWidth();
	static const float BorderHeight      = Env->GUI->GetDefaultWindowBorderHeight();

	LVector4 LeftUniforms( GetX1(), GetY1() + HalfCaptionHeight, GetX1() + BorderWidth, GetY2() );
	LVector4 RightUniforms( GetX2() - BorderWidth, GetY1() + HalfCaptionHeight, GetX2(), GetY2() );
	LVector4 BottomUniforms( GetX1(), GetY2() - BorderHeight, GetX2(), GetY2() );
	LVector4 CaptionUniforms( GetX1() - CaptionLedge, GetY1(), GetX2() + CaptionLedge, GetY1() + CaptionHeight );

	UpdateUniformsV( FLeftBorderShader,   LeftUniforms );
	Env->Renderer->AddBuffer( FLeftBorderGeometry, FLeftBorderShader, 1, false );

	UpdateUniformsV( FRightBorderShader,  RightUniforms );
	Env->Renderer->AddBuffer( FRightBorderGeometry, FRightBorderShader, 1, false );

	UpdateUniformsV( FBottomBorderShader, BottomUniforms );
	Env->Renderer->AddBuffer( FBottomBorderGeometry, FBottomBorderShader, 1, false );

	UpdateUniformsV( FCaptionShader, CaptionUniforms );
	UpdateUniformsV( FCaptionHighlightShader, CaptionUniforms );
	Env->Renderer->AddBuffer( FCaptionGeometry, FCaptionHighlighted ? FCaptionHighlightShader : FCaptionShader, 1, false );

	OutTextXY( CenterTextHorizontal( GetTitle() ),
	           Env->GUI->GetDefaultVerticalTitlePosition(),
	           GetTitle(),
	           LC_Red );
}

void clGUIDialogWindow::PostRender()
{
	clGUIPanel::PostRender();

	Env->Renderer->GetCanvas()->EndTextPacket();
	Env->Renderer->GetCanvas()->TextStrFreeType_Flush();

	// use window composer
	FWindowBuffer->UnBind();

	FWindowComposerShader->SetTexture( 0, FWindowBuffer->GetColorTexture( 0 ), false );

	UpdateUniforms( FWindowComposerShader );
	Env->Renderer->AddBuffer( FGeometry, FWindowComposerShader, 1, false );
}

void clGUIDialogWindow::Event_MouseInside( iGUIResponder* Source, const LVector2& Pnt )
{
	clGUIPanel::Event_MouseInside( Source, Pnt );

	FCaptionHighlighted = ContainsCaption( Pnt );

	FCaptionHighlighted ?
	Env->GUI->SetCursorForm( CFORM_SIZEALL ) :
	Env->GUI->SetCursorForm( CFORM_ARROW );
}

void clGUIDialogWindow::Event_MouseOutside( iGUIResponder* Source, const LVector2& Pnt )
{
	clGUIPanel::Event_MouseOutside( Source, Pnt );

	Env->GUI->SetCursorForm( CFORM_ARROW );

	FCaptionHighlighted = false;
}

void clGUIDialogWindow::Event_LDoubleClick( iGUIResponder* Source, const LVector2& Pnt )
{
	clGUIPanel::Event_LDoubleClick( Source, Pnt );

	if ( ContainsCaption( Pnt ) )
	{
		switch ( GetRegionState() )
		{
			case REGION_MAXIMIZED:
				Minimize();
				break;
			case REGION_MINIMIZED:
				Maximize();
				break;
		}
	}
}

/*
 * 05/04/2007
     SaveToXLMLStream()
 * 06/03/2007
     Composing is activated only is GLSL is supported
 * 06/08/2006
     Window opacity control via shader program
 * 27/07/2006
     New parameter "COMPOSERSHADER"
 * 13/07/2006
     Initial window composer support code
 * 06/05/2005
     ContainsResizeCorner() returns false
 * 06/04/2005
     It's here
*/
