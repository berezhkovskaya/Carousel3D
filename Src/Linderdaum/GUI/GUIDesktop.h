/**
 * \file GUIDesktop.h
 * \brief GUI desktop - root object
 * \version 0.5.60
 * \date 17/12/2005
 * \author Sergey Kosarevsky, 2005
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iGUIDesktop_
#define _iGUIDesktop_

#include "GUI/iGUIView.h"

#include "Renderer/iVertexArray.h"
#include "Renderer/RenderState.h"

/// Root object for internal GUI
class netexportable scriptfinal clGUIDesktop: public iGUIView
{
public:
	clGUIDesktop();
	virtual ~clGUIDesktop();
	//
	// iObject interface
	//
	virtual void AfterConstruction();
	//
	// iGUIRegion interface
	//
	virtual float     GetOpacity() const { return 1.0f; };
	//
	// iGUIDesktop
	//
	virtual void RenderCursor();
private:
	iVertexArray*     FCursorGeometry;
	clRenderState*    FArrowCursorShader;
	clRenderState*    FSizeAllCursorShader;
	clRenderState*    FSizeNWSECursorShader;

	void ShowStats();

#pragma region Last matrices (to visualize debug information)
	clCVar*    FLastProjection;
	clCVar*    FLastModelView;
	clCVar*    FShowModelView;
#pragma endregion

#pragma region Stats from traversers
	clCVar*    FPickingTime_T;
	clCVar*    FRenderingTime_T;
#pragma endregion

#pragma region Rendering stats
	clCVar*    FShowFPS;
	clCVar*    FShowStats;
	clCVar*    FShowTraversers;
	clCVar*    FFPS;
	clCVar*    FUpdateTime;
	clCVar*    FUpdatesPerFrame;
	clCVar*    FDips;
	clCVar*    FDipsBlended;
	clCVar*    FStateChanges;
	clCVar*    FTextureRebinds;
	clCVar*    FProgramRebinds;
	clCVar*    FVALocksCount;
	clCVar*    FAllocatedBytesRT;
#pragma endregion
};

#endif

/*
 * 27/01/2005
     It's here
*/
