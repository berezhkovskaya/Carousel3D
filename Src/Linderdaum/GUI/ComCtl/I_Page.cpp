/**
 * \file I_Page.cpp
 * \brief GUI page - similar to window but always occupies the whole screen and only one page can be active at a time
 * \version 0.6.02
 * \date 08/08/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "I_Page.h"

#include "Engine.h"
#include "LKeys.h"
#include "Environment.h"
#include "Input/Input.h"
#include "GUI/GUIManager.h"
#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"
#include "Resources/ResourcesManager.h"

void clGUIPage::AfterConstruction()
{
	iGUIView::AfterConstruction();

	// make a full-screen page
	SetCoords( 0.0f, 0.0f, 1.0f, 1.0f );
	SetOpacity( 1.0f );
}

clGUIPage::~clGUIPage()
{
}

void clGUIPage::RenderHierarchy()
{
	if ( !IsPageActive() ) { return; }

	iGUIView::RenderHierarchy();
}

bool clGUIPage::IsPageActive() const
{
	return Env->GUI->IsPageActive( this );
}

bool clGUIPage::Event_Key( iGUIResponder* Source, const int Key, const bool KeyState )
{
	if ( KeyState && Key == LK_ESCAPE )
	{
		/// Call custom handler to allow some actions like haptic feedback
		if ( HasFallback() )
		{
			if ( FTapOnSwitch ) { Env->Input->HapticFeedback_Tap(); }

			if ( FFallbackTransition.empty() )
			{
				Env->GUI->SetActivePageID( FFallbackPage );
			}
			else
			{
				Env->GUI->SetActivePageTransitionID( FFallbackTransition, FFallbackPage );
			}

			return true;
		}
	}

	return false;
}

/*
 * 08/08/2011
     It's here
*/
