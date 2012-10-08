/**
 * \file I_TransitionPage.h
 * \brief Auxillary transition GUI page. Animates the transition between two pages.
 * \version 0.6.04
 * \date 19/01/2012
 * \author Sergey Kosarevsky, 2012
 * \author Viktor Latypov, 2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clGUITransitionPage_
#define _clGUITransitionPage_

#include "GUI/ComCtl/I_Page.h"

/**
   \brief Auxillary transition GUI page

   Animates the transition between From and To pages.

   From and To can be NULL (from from/to "nowhere")

   Problems with interrupted transitions must be handled in GUIManager
*/
class clGUITransitionPage: public clGUIPage
{
public:
	clGUITransitionPage():
		FPageFrom( NULL ),
		FPageTo( NULL ),
		FPageFromID(),
		FPageToID(),
		FTransitionPercent( 0.0f ),
		FLocalTime( 0.0f ),
		FAnimationTime( 0.5f ) {}
	//
	// iGUIView interface
	//
	virtual void PreRender();

	//
	// clGUITransitionPage
	//

	virtual void StartAnimation();
	virtual void FinishAnimation();

	/// The actual animation stuff
	virtual void SetTransitionPercent( float PP ) { FTransitionPercent = PP; }

	/// Animation info
	virtual float GetTransitionPercent() const { return FTransitionPercent; }

public:
	virtual void    Event_PageActive( iGUIResponder* Source, clGUIPage* PrevPage );
	virtual void    Event_PageInactive( iGUIResponder* Source, clGUIPage* NextPage );
	virtual void    Event_Timer( iGUIResponder* Source, float DeltaTime );

public:
	PROPERTY( Name=TransitionPercent, Type=float, Getter=GetTransitionPercent, Setter=SetTransitionPercent )
	PROPERTY( Name=AnimationTime,     Type=float, FieldName=FAnimationTime )
	PROPERTY( Name=PageFromID,        Type=string, FieldName=FPageFromID   )
	PROPERTY( Name=PageToID,          Type=string, FieldName=FPageToID     )

	/// 0.0f - first page, 1.0f - second page
	float FTransitionPercent;

	/// How much time the transition takes
	float FAnimationTime;

	LString    FPageFromID;
	LString    FPageToID;
protected:
	clGUIPage* FPageFrom;
	clGUIPage* FPageTo;
private:
	/// Local time counter
	float FLocalTime;
};

#endif

/*
 * 19/01/2012
     It's here
*/
