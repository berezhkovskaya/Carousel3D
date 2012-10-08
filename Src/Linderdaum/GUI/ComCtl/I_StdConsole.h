/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUIStdConsole_
#define _clGUIStdConsole_

#include "GUI/iGUIView.h"

#include "Renderer/iVertexArray.h"
#include "Renderer/RenderState.h"

/// Default console HUD
class scriptfinal clGUIStdConsole: public iGUIView
{
public:
	clGUIStdConsole(): FConsoleShader( NULL ),
		FConsoleGeometry( NULL ),
		FTextBarShader( NULL ),
		FTextBarGeometry( NULL ),
		FCurrentPosition( 0 ),
		FTextBuffer(),
		FCursorPos( 0 ),
		FCursorBlinking( 0 ) {};
	virtual ~clGUIStdConsole();

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()
	//
	// iGUIView interface
	//
	virtual void    PreRender();
	virtual void    PostRender();
public:
	virtual bool    Event_Key( iGUIResponder* Source, const int Key, const bool KeyState );
	virtual void    Event_Registered( iGUIResponder* Source );
	virtual void    Event_UnRegistered( iGUIResponder* Source );
	virtual void    Event_Timer( iGUIResponder* Source, float DeltaTime );
private:
	void    CloseConsoleC( const LString& Param );
	void    ExecuteCommand();
private:
	clRenderState*   FConsoleShader;
	iVertexArray*    FConsoleGeometry;

	clRenderState*   FTextBarShader;
	iVertexArray*    FTextBarGeometry;

	float            FCurrentPosition;

	LString          FTextBuffer;
	float            FCursorBlinking;

	size_t           FCursorPos;
};

#endif

/*
 * 31/01/2005
     It's here
*/
