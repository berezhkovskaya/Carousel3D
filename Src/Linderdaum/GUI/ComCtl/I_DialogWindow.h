/*
   (Part of Linderdaum Engine)
   Version 0.5.73
   (05/04/2007)
   (C) Sergey Kosarevsky, 2005-2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUIDialogWindow_
#define _clGUIDialogWindow_

#include "GUI/ComCtl/I_Panel.h"

class iRenderTarget;

class scriptfinal clGUIDialogWindow: public clGUIPanel
{
public:
	clGUIDialogWindow(): FCaptionShader( NULL ),
		FCaptionHighlightShader( NULL ),
		FCaptionGeometry( NULL ),
		FCaptionHighlighted( false ),
		// borders
		FLeftBorderGeometry( NULL ),
		FLeftBorderShader( NULL ),
		FRightBorderGeometry( NULL ),
		FRightBorderShader( NULL ),
		FBottomBorderGeometry( NULL ),
		FBottomBorderShader( NULL ),
		// offscreen buffer and window composer
		FWindowComposerShader( NULL ),
		FWindowBuffer( NULL ) {};
	virtual ~clGUIDialogWindow();

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS();

	//
	// iGUIRegion interface
	//
	virtual bool    ContainsResizeCorner( const LVector2& Pnt ) const;
	//
	// iGUIView interface
	//
	virtual void    PreRender();
	virtual void    PostRender();
	virtual bool    IsViewDockable() const;
	virtual bool    IsViewDraggable() const;
	virtual bool    IsViewSizeable() const;
	//
	// clGUIDialogWindow
	//
	virtual void    SetWindowComposerShaderName( const LString& FileName );
	virtual LString GetWindowComposerShaderName() const { return FWindowComposerShader ? FWindowComposerShader->GetFileName() : ""; };
	/* Property( Name="ComposerShader",     Type=string,  Setter=SetWindowComposerShaderName, Getter=GetWindowComposerShaderName ) */
public:
	virtual void    Event_Registered( iGUIResponder* Source );
	virtual void    Event_MouseInside( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_MouseOutside( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_LDoubleClick( iGUIResponder* Source, const LVector2& Pnt );
private:
	clRenderState*   FCaptionShader;
	clRenderState*   FCaptionHighlightShader;
	iVertexArray*    FCaptionGeometry;
	bool             FCaptionHighlighted;

	// borders
	iVertexArray*    FLeftBorderGeometry;
	clRenderState*   FLeftBorderShader;

	iVertexArray*    FRightBorderGeometry;
	clRenderState*   FRightBorderShader;

	iVertexArray*    FBottomBorderGeometry;
	clRenderState*   FBottomBorderShader;

	// window composer
	clRenderState*   FWindowComposerShader;
	iRenderTarget*   FWindowBuffer;
};

#endif

/*
 * 05/04/2007
     SaveToXLMLStream()
 * 13/07/2006
     Initial data fields for window composer
 * 06/04/2005
     It's here
*/
