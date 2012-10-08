/**
 * \file I_CrossFade.h
 * \brief Gradual opacity-based cross fade transition effect
 * \version 0.6.04
 * \date 19/01/2012
 * \author Sergey Kosarevsky, 2012
 * \author Viktor Latypov, 2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clGUICrossFadePage_
#define _clGUICrossFadePage_

#include "GUI/ComCtl/I_TransitionPage.h"

/// Gradual opacity-based cross fade transition effect
class scriptfinal clGUICrossFadePage: public clGUITransitionPage
{
public:
	clGUICrossFadePage() {}

	//
	// clGUITransitionPage interface
	//

	virtual void SetTransitionPercent( float PP );
};

#endif

/*
 * 08/08/2011
     It's here
*/
