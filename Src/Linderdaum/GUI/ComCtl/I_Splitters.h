/*
   (Part of Linderdaum Engine)
   Version 0.05.74
   (29/05/2007)
   (C) Sergey Kosarevsky, 2005-2007
   (C) Viktor Latypov, 2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUISplitters_
#define _clGUISplitters_

#include "GUI/ComCtl/I_Panel.h"

/// Base class for different splitters
class scriptfinal clGUISplitter: public clGUIPanel
{
public:
	clGUISplitter(): FSplitterShaderName() {};
	virtual ~clGUISplitter() {};

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS()

	//
	// iGUIView
	//
	virtual void    Event_Registered( iGUIResponder* Source );
protected:
	LString FSplitterShaderName;
};

/// Horizontal splitter
class scriptfinal clGUIHorizontalSplitter: public clGUISplitter
{
public:
	clGUIHorizontalSplitter()
	{
		FSplitterShaderName = "interface\\shaders\\splitter_h.shader";
	}
};

/// Vertical splitter
class scriptfinal clGUIVerticalSplitter: public clGUISplitter
{
public:
	clGUIVerticalSplitter()
	{
		FSplitterShaderName = "interface\\shaders\\splitter_v.shader";
	}
};

#endif

/*
 * 29/05/2007
     Merged Vert/Horiz splitters
 * 12/02/2005
     It's here
*/
