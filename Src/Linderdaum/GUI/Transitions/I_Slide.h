/**
 * \file I_Slide.h
 * \brief Slide-away transition effect
 * \version 0.6.04
 * \date 21/01/2012
 * \author Sergey Kosarevsky, 2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clGUICrossFadePage_
#define _clGUICrossFadePage_

#include "GUI/ComCtl/I_TransitionPage.h"

/// Gradual opacity-based cross fade transition effect
class scriptfinal clGUISlidePage: public clGUITransitionPage
{
public:
	clGUISlidePage()
	 : FPageToVec(0.0f),
		FPageFromVec(0.0f),
		FToDelta(0.0f),
		FFromDelta(0.0f) {}

	//
	// clGUITransitionPage interface
	//

	virtual void FinishAnimation();
	virtual void SetTransitionPercent( float PP );
public:
	PROPERTY( Name=PageToVec,   Type=vec4, FieldName=FPageToVec   )
	PROPERTY( Name=PageFromVec, Type=vec4, FieldName=FPageFromVec )
public:
	LVector4    FPageToVec;
	LVector4    FPageFromVec;
	LVector4    FToDelta;
	LVector4    FFromDelta;
};

#endif

/*
 * 21/01/2012
     It's here
*/
