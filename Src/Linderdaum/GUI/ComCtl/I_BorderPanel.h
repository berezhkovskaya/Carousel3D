/**
 * \file I_BorderPanel.h
 * \brief Panel with border
 * \version 0.5.60
 * \date 17/12/2005
 * \author Sergey Kosarevsky, 2005
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clGUIBorderPanel_
#define _clGUIBorderPanel_

#include "GUI/ComCtl/I_Panel.h"

class scriptfinal clGUIBorderPanel: public clGUIPanel
{
public:
	clGUIBorderPanel() {};

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS()
public:
	virtual void    Event_Registered( iGUIResponder* Source );
};

#endif

/*
 * 12/02/2005
     It's here
*/
