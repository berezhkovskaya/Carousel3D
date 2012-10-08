/*
   (Part of Linderdaum Engine)
   Version 0.5.72
   (02/03/2007)
   (C) Sergey Kosarevsky, 2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _iGUIInputLine_
#define _iGUIInputLine_

#include "GUI/ComCtl/I_Button.h"

class clValidator;

/// Text input line
class scriptfinal clGUIInputLine: public iGUIButton
{
public:
	clGUIInputLine();
	virtual ~clGUIInputLine();

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS()
	//
	// iObject interface
	//
	virtual void    AfterConstruction();
	//
	// iGUIView interface
	//
	virtual void    PreRender();
	virtual void    PostRender();
	//
	// clGUIInputLine
	//
	virtual void    SetValidator( clValidator* Validator );
public:
	virtual bool    Event_Key( iGUIResponder* Source, const int Key, const bool KeyState );
	virtual void    Event_Registered( iGUIResponder* Source );
	virtual void    Event_Timer( iGUIResponder* Source, float DeltaTime );
private:
	size_t         FCursorPos;
	bool           FEditable;
	float          FCursorBlinking;
	clValidator*   FValidator;
};

#endif

/*
 * 02/03/2007
     It's here
*/
