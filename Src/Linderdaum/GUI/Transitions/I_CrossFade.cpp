/**
 * \file I_CrossFade.cpp
 * \brief Gradual opacity-based cross fade transition effect
 * \version 0.6.04
 * \date 19/01/2012
 * \author Sergey Kosarevsky, 2012
 * \author Viktor Latypov, 2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "I_CrossFade.h"

void clGUICrossFadePage::SetTransitionPercent( float PP )
{
	clGUITransitionPage::SetTransitionPercent( PP );

	if ( FPageFrom ) { FPageFrom->SetOpacity( 1.0f - FTransitionPercent ); }

	if ( FPageTo )   { FPageTo->SetOpacity( FTransitionPercent ); }
}

/*
 * 19/01/2012
     It's here
*/
