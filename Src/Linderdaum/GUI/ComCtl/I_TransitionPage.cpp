/**
 * \file I_TransitionPage.h
 * \brief Auxillary transition GUI page. Animates the transition between two pages.
 * \version 0.6.04
 * \date 19/01/2012
 * \author Sergey Kosarevsky, 2012
 * \author Viktor Latypov, 2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "I_TransitionPage.h"

#include "Engine.h"
#include "Core/Linker.h"
#include "LKeys.h"
#include "Environment.h"
#include "Input/Input.h"
#include "GUI/GUIManager.h"
#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"
#include "Resources/ResourcesManager.h"

void clGUITransitionPage::PreRender()
{
	if ( FPageFrom )
	{
		FPageFrom->RenderHierarchy();
		FPageFrom->RenderTopmost();
	}

	if ( FPageTo )
	{
		FPageTo->RenderHierarchy();
		FPageTo->RenderTopmost();
	}
}

void clGUITransitionPage::StartAnimation()
{
	SetTransitionPercent( 0.0f );
}

void clGUITransitionPage::FinishAnimation()
{
	SetTransitionPercent( 1.0f );
}

void clGUITransitionPage::Event_PageActive( iGUIResponder* Source, clGUIPage* PrevPage )
{
	clGUIPage::Event_PageActive( Source, PrevPage );

	FPageFrom = Env->GUI->FindView<clGUIPage>( FPageFromID );
	FPageTo   = Env->GUI->FindView<clGUIPage>( FPageToID   );

	if ( !FPageFrom ) FPageFrom = PrevPage;

	if ( FPageFrom ) FPageFrom->Event_PageAnimationActive( this );
	if ( FPageTo   ) FPageTo->Event_PageAnimationActive( this );

	/// Reload animation
	FLocalTime = 0.0f;

	StartAnimation();
}

void clGUITransitionPage::Event_PageInactive( iGUIResponder* Source, clGUIPage* NextPage )
{
	if ( FPageFrom ) FPageFrom->Event_PageAnimationInactive( this );
	if ( FPageTo   ) FPageTo->Event_PageAnimationInactive( this );
}

void clGUITransitionPage::Event_Timer( iGUIResponder* Source, float DeltaTime )
{
	clGUIPage::Event_Timer( Source, DeltaTime );

	FLocalTime += DeltaTime;

	// zero-length animations are not interesting
	if ( FAnimationTime < Math::EPSILON ) return;

	float Percent = std::min( FLocalTime / FAnimationTime, 1.0f );

	SetTransitionPercent( Percent );

	if ( FLocalTime >= FAnimationTime )
	{
		Env->GUI->SetActivePage( FPageTo );

		FinishAnimation();

		/// Self-terminate
		//Env->GUI->RequestClose( this );
	}
}

/*
 * 19/01/2012
     It's here
*/
