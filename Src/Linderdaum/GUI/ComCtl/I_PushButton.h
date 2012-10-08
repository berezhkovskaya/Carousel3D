/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUIPushButton_
#define _clGUIPushButton_

#include "GUI/ComCtl/I_Button.h"

#include "Renderer/iVertexArray.h"
#include "Renderer/RenderState.h"

/// Push button
class scriptfinal clGUIPushButton: public iGUIButton
{
public:
	clGUIPushButton();
	virtual ~clGUIPushButton() {};

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS()
	//
	// iGUIView interface
	//
	virtual void    PreRender();
public:
	/* Property( Name="Disabled",   Type=bool,  FieldName=FButtonDisabled ) */
	bool              FButtonDisabled;
public:
	virtual void    Event_Registered( iGUIResponder* Source );
	virtual void    Event_CaptureChanged( iGUIResponder* Source );
	virtual void    Event_MouseInside( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_MouseOutside( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState );
private:
	clRenderState*    FNormalShader;
	clRenderState*    FPushedShader;
	clRenderState*    FHighlightShader;
	clRenderState*    FDisabledShader;
	bool              FButtonPushed;
};

#endif

/*
 * 24/02/2007
     FButtonDisabled
     FDisabledShader
 * 30/01/2005
     It's here
*/
