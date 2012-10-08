/**
 * \file iGUIRegion.cpp
 * \brief GUI region
 * \version 0.5.90
 * \date 09/02/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "iGUIRegion.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/Logger.h"
#include "GUI/GUIManager.h"
#include "Utils/Exceptions.h"
#include "Utils/Localizer.h"
#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iShaderProgram.h"

const float GUI_PARENT_CENTERED = -128;

void iGUIRegion::ParseCoords( const LString& Coords )
{
	FSavedCoords = Coords;

	float CTL_X, CTL_Y, CTL_W, CTL_H;

	static const LString GUI_PARENT_CENTERED_STR = "GUI_CENTERED";
	static const LString GUI_TITLE_WIDTH         = "GUI_TITLE_WIDTH";
	static const LString GUI_TITLE_HEIGHT        = "GUI_TITLE_HEIGHT";

	// check parameters list
	for ( int i = 1; i != 5; ++i )
	{
		// X and Y could be GUI_PARENT_CENTERED
		if ( i == 1 || i == 2 )
		{
			if ( LStr::GetToken( Coords, i ) == GUI_PARENT_CENTERED_STR ) { continue; }
		}

		if ( i == 3 ) if ( LStr::GetToken( Coords, i ) == GUI_TITLE_WIDTH ) { continue; }

		if ( i == 4 ) if ( LStr::GetToken( Coords, i ) == GUI_TITLE_HEIGHT ) { continue; }

		CHECK_ERROR( !LStr::IsCorrectDouble( LStr::GetToken( Coords, i ) ), "Invalid coord at position " + LStr::ToStr( i ) + " in " + Coords );
	}

	// parse parameters: "X Y W H"
	CTL_X = LStr::ToFloat( LStr::GetToken( Coords, 1 ) );
	CTL_Y = LStr::ToFloat( LStr::GetToken( Coords, 2 ) );
	CTL_W = LStr::ToFloat( LStr::GetToken( Coords, 3 ) );
	CTL_H = LStr::ToFloat( LStr::GetToken( Coords, 4 ) );

	if ( LStr::GetToken( Coords, 3 ) == GUI_TITLE_WIDTH )
	{
		CHECK_ERROR( FRegionTitle.empty(), "Specify non-empty title before setting coords for: " + LString( ClassName() ) );

		CTL_W = Env->GUI->GetStringWidth( FRegionTitle, NULL );
	}

	if ( LStr::GetToken( Coords, 4 ) == GUI_TITLE_HEIGHT ) { CTL_H = Env->GUI->GetStringHeight(); }

	SetCoords( CTL_X, CTL_Y, CTL_W, CTL_H );

	if ( LStr::GetToken( Coords, 1 ) == GUI_PARENT_CENTERED_STR ) { FNeedToCenterWidth  = true; }

	if ( LStr::GetToken( Coords, 2 ) == GUI_PARENT_CENTERED_STR ) { FNeedToCenterHeight = true; }
}

void iGUIRegion::SetCoords( float X, float Y, float W, float H )
{
	FRect.X1() = X;
	FRect.Y1() = Y;

	SetWidth( W );
	SetHeight( H );

	FMaximizedHeight = H;
}

void iGUIRegion::OutTextXY( const float X, const float Y, const LString& Text, const LVector4& Color )
{
	iFontProperties* Font = GetFont();

	if ( Font && !Font->IsFixedFont() )
	{
		const LRect Rect( FTextOffsets.X + FRect.X1(), FTextOffsets.Y + FRect.Y1(), FTextOffsets.Z + FRect.X2(), FTextOffsets.W + FRect.Y2() );

		clFreeTypeFontProperties* FTFont( dynamic_cast<clFreeTypeFontProperties*>( Font ) );

		Env->Renderer->GetCanvas()->TextStrFreeType( Rect, LOCALIZE( Text ), FTFont->FFontSize, LVector4( Color.ToVector3(), GetOpacity() ), FTFont->FFontFace, FTFont->FAlignment );
	}
	else
	{
		Env->Renderer->GetCanvas()->TextStr( X + FRect.X1(), Y + FRect.Y1(), Text, LVector4( Color.ToVector3(), GetOpacity() ), dynamic_cast<clFixedFontProperties*>( Font ) );
	}
}

LVector2 iGUIRegion::Screen2Local( const LVector2& ScreenCoords ) const
{
	return LVector2( ScreenCoords.X - FRect.X1(),
	                 ScreenCoords.Y - FRect.Y1() );
}

LVector2 iGUIRegion::Local2Screen( const LVector2& LocalCoords ) const
{
	return LVector2( LocalCoords.X + FRect.X1(),
	                 LocalCoords.Y + FRect.Y1() );
}

bool iGUIRegion::Contains( const LVector2& Pnt ) const
{
	LVector2 LocalPnt = Screen2Local( Pnt );

	return ( LocalPnt.X >= 0 ) &&
	       ( LocalPnt.Y >= 0 ) &&
	       ( LocalPnt.X <= FRect.GetWidth() ) &&
	       ( LocalPnt.Y <= FRect.GetHeight() );
}

bool iGUIRegion::ContainsCaption( const LVector2& Pnt ) const
{
	LVector2 LocalPnt = Screen2Local( Pnt );

	return ( LocalPnt.X >= 0 ) &&
	       ( LocalPnt.Y >= 0 ) &&
	       ( LocalPnt.X <= FRect.GetWidth() ) &&
	       ( LocalPnt.Y <= Env->GUI->GetDefaultCaptionHeight() );
}

bool iGUIRegion::ContainsResizeCorner( const LVector2& Pnt ) const
{
	// by default regions doesn't have Resize corner
	return false;
}

bool iGUIRegion::ContainsCloseButton( const LVector2& Pnt ) const
{
	// by default regions doesn't have Close button
	return false;
}

void iGUIRegion::MoveTo( const LVector2& Pnt )
{
	MoveRel( Pnt - LVector2( FRect.X1(), FRect.Y1() ) );

	FInLocalCoords = false;
}

void iGUIRegion::MoveRel( const LVector2& Delta )
{
	FRect.MoveRel( Delta );
}

void iGUIRegion::SetSize( const float Width, const float Height )
{
	SetWidth( Width );
	SetHeight( Height );

	FMaximizedHeight = Height;
	FRegionState = REGION_MAXIMIZED;
}

void iGUIRegion::Precenter( iGUIRegion* Parent )
{
	float NewX = FRect.X1();
	float NewY = FRect.Y1();

	if ( IsInLocalCoords() )
	{
		if ( FNeedToCenterWidth  ) { NewX = ( Parent->GetWidth()  - GetWidth()       ) / 2.0f; }

		if ( FNeedToCenterHeight ) { NewY = ( Parent->GetHeight() - FMaximizedHeight ) / 2.0f; }
	}

	// reset flags
	FNeedToCenterWidth  = false;
	FNeedToCenterHeight = false;

	MoveTo( LVector2( NewX, NewY ) );
}

float iGUIRegion::CenterTextHorizontal( const LString& Str )
{
	return 0.5f * ( GetWidth() - Env->GUI->GetStringWidth( Str, GetFont() ) );
}

float iGUIRegion::CenterTextVertical()
{
	return 0.5f * ( GetHeight() - Env->GUI->GetStringHeight() );
}

void iGUIRegion::DockCoordsToThisRegion( float* X, float* Y, const float W, const float H )
{
	FRect.DockCoordsToThisRect( X, Y, W, H, Env->GUI->GetDefaultDockingDistance() );
}

void iGUIRegion::DockSizeToThisRegion( const float X, const float Y, float* W, float* H )
{
	FRect.DockSizeToThisRect( X, Y, W, H, Env->GUI->GetDefaultDockingDistance() );
}

void iGUIRegion::UpdateUniformsV( clRenderState* Shader, const LVector4& Values ) const
{
	if ( !Shader ) { return; }

	if ( iShaderProgram* SP = Shader->GetShaderProgram() )
	{
		SP->SetGUIUniforms( Values, GetOpacity(), GetLifeTime() );
	}
}

void iGUIRegion::UpdateUniforms( clRenderState* Shader ) const
{
	if ( !Shader ) { return; }

	if ( iShaderProgram* SP = Shader->GetShaderProgram() )
	{
		SP->SetGUIUniforms( GetUniformsVec(), GetOpacity(), GetLifeTime() );
	}
}

void iGUIRegion::Event_Timer( iGUIResponder* Source, float DeltaTime )
{
	FLifeTime += DeltaTime;

	iGUIResponder::Event_Timer( Source, DeltaTime );
}

/*
 * 09/02/2010
     UpdateUniforms()
 * 06/05/2005
     ContainsResizeCorner() returns false
 * 03/02/2005
     OutTextXY()
 * 31/01/2005
     It's here
*/
