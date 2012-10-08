/**
 * \file I_Bubbles.h
 * \brief Bubbles 2D particle system
 * \version 0.5.71
 * \date 25/01/2007
 * \author Sergey Kosarevsky, 2005-2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clGUIBubbles_
#define _clGUIBubbles_

#include "GUI/ComCtl/I_Panel.h"

class scriptfinal clGUIBubbles: public clGUIPanel
{
public:
	clGUIBubbles() {};
	virtual ~clGUIBubbles();
public:
	virtual void    Event_Registered( iGUIResponder* Source );
	virtual void    Event_Timer( iGUIResponder* Source, float DeltaTime );
private:
	LVector3    RandomVelocity();
private:
	static const int MAX_BUBBLES = 700;
	LVector3      FBubbles[MAX_BUBBLES];
	LVector3      FVelocities[MAX_BUBBLES];
};

#endif

/*
 * 25/01/2007
     Moved to ComCtls
 * 17/04/2005
     It's here
*/
