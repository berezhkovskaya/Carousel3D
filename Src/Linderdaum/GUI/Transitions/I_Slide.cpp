/**
 * \file I_Slide.cpp
 * \brief Slide-away transition effect
 * \version 0.6.04
 * \date 21/01/2012
 * \author Sergey Kosarevsky, 2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "I_Slide.h"

void clGUISlidePage::FinishAnimation()
{
	clGUITransitionPage::FinishAnimation();

	if ( FPageFrom )
	{
		FPageFrom->MoveTo( LVector2(0.0f) );
	}

	if ( FPageTo )
	{
		FPageTo->MoveTo( LVector2(0.0f) );
	}
}

void clGUISlidePage::SetTransitionPercent( float PP )
{
	clGUITransitionPage::SetTransitionPercent( PP );

	LVector4 ToVec   = (1.0f-PP) * FPageToVec;
	LVector4 FromVec = PP * FPageFromVec;

	if ( FPageFrom )
	{
		FPageFrom->MoveTo( FromVec.XY() );
	}

	if ( FPageTo )
	{
		FPageTo->MoveTo( ToVec.XY() );
	}
}

/*
 * 21/01/2012
     It's here
*/
