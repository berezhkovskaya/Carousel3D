/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUIWindow_
#define _clGUIWindow_

#include "GUI/ComCtl/I_DialogWindow.h"

/// GUI window (moveable/sizeable)
class clGUIWindow: public clGUIDialogWindow
{
public:
	clGUIWindow(): FRequestingClose( false ),
		FResizeCornerShader( NULL ),
		FResizeCornerHighlightShader( NULL ),
		FResizeCornerGeometry( NULL ),
		FResizeCornerHighlighted( false ),
		FCloseButtonShader( NULL ),
		FCloseButtonHighlightShader( NULL ),
		FCloseButtonCaptionHighlightedShader( NULL ),
		FCloseButtonGeometry( NULL ),
		FCloseButtonHighlighted( false ) {};
	virtual ~clGUIWindow();

//	NET_NOT_EXPORTABLE()
	SERIALIZABLE_CLASS()
	//
	// iGUIRegion interface
	//
	virtual bool    ContainsCaption( const LVector2& Pnt ) const;
	virtual bool    ContainsCloseButton( const LVector2& Pnt ) const;
	virtual bool    ContainsResizeCorner( const LVector2& Pnt ) const;
	//
	// iGUIView interface
	//
	virtual void    PreRender();
public:
	virtual void    Event_Registered( iGUIResponder* Source );
	virtual void    Event_MouseInside( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_MouseOutside( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState );
	virtual void    Event_Timer( iGUIResponder* Source, float DeltaTime );
private:
	bool            FRequestingClose;

	clRenderState*    FResizeCornerShader;
	clRenderState*    FResizeCornerHighlightShader;
	iVertexArray*     FResizeCornerGeometry;
	bool              FResizeCornerHighlighted;

	clRenderState*    FCloseButtonShader;
	clRenderState*    FCloseButtonHighlightShader;
	clRenderState*    FCloseButtonCaptionHighlightedShader;
	iVertexArray*     FCloseButtonGeometry;
	bool              FCloseButtonHighlighted;
};

#endif

/*
 * 06/04/2005
     Inherited from clGUIDialogWindow
 * 05/02/2005
     It's here
*/
