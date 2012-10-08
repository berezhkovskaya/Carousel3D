/**
 * \file GUIManager.h
 * \brief Internal GUI manager
 * \version 0.5.91
 * \date 06/07/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clGUIManager_
#define _clGUIManager_

#include "Core/iObject.h"
#include "Core/Console.h"
#include "GUI/iGUIView.h"

#include "GUIDesktop.h"

#include <string.h>

enum LCursorForm
{
	CFORM_NOCURSOR = 0,
	CFORM_ARROW    = 1,
	CFORM_SIZENWSE = 2,
	CFORM_SIZEALL  = 3
};

class clCVar;
class iRenderTarget;

class clGUIPage;
class clGUITransitionPage;
class clGUICheckBox;
class clGUIInputLine;
class clGUISlider;
class clGUIStaticWidget;
class clGUIPushButton;
class clGUIColorDial;
class clGUIMtxView;
class clGUISceneView;
class clGUIFullScreenGraph;
class clGUIVisualSceneView;
class clRenderingTechnique;
class clVisualProjector;
class clVisualScene;

/// Internal GUI (Graphical User Interface) manager
class netexportable scriptfinal clGUIManager: public iObject
{
public:
	typedef std::list<iGUIView*>    clViewsList;
public:
	clGUIManager();
	virtual ~clGUIManager();
	//
	// iObject interface
	//
	virtual void           AfterConstruction();
	//
	// clGUIManager
	//
	virtual bool           IsActiveView() { return ( FActiveView != FDesktop ) && FGUIVisible; };
	virtual void           RegisterView( iGUIView* View );
	virtual void           UnRegisterView( iGUIView* View );
	virtual void           SetCursorForm( LCursorForm CursorForm );
	virtual LCursorForm    GetCursorForm() const;
	virtual void           HideGUI();
	virtual void           ShowGUI();
	virtual void           SetCapture( iGUIView* View );
	virtual void           ReleaseCapture();
	virtual void           RequestClose( iGUIView* View );
	virtual void           RecheckMouse();
	virtual void           SetTopmostView( iGUIView* View );
	virtual void           SetTopmostViewID( const LString& ID );
	virtual void           SetActivePage( clGUIPage* Page );
	virtual void           SetActivePageID( const LString& ID );
	virtual void           SetActivePageTransition( clGUITransitionPage* TransitionPage, const LString& TargetPageID );
	virtual void           SetActivePageTransitionID( const LString& TransitionPageID, const LString& TargetPageID );
	virtual bool           IsViewTopmost( const iGUIView* View ) const { return FTopmostView == View; }
	virtual bool           IsPageActive( const clGUIPage* Page ) const { return FActivePage == Page; }
	virtual void           ShowView( const LString& ID, const bool Visible ) const;
	virtual iRenderTarget* GetWindowBuffer() const;
	virtual iVertexArray*  GetDefaultRect() const;
	virtual iVertexArray*  GetDefaultBevelRect() const;
	virtual iVertexArray*  GetDefaultTriangle() const;
	virtual clViewsList*   GetDockableViewsList() { return &FDockableViewsList; };
	virtual iGUIView*      FindViewByID( const LString& ID ) const;
	template <class T> T*  FindView( const LString& ID ) const { return dynamic_cast<T*>( FindViewByID( ID ) ); };
	virtual iGUIView*     GetDesktop() const { return FDesktop; };
	virtual void          ProcessInputLineKey( const int Key, LString& TextBuffer, size_t& CursorPos, float MaxScrLength ) const;

	virtual void          AddGUITo( const LString& FileName, iGUIView* To );
	virtual void          DeleteGUIFrom( const LString& ViewID, iGUIView* From );
	//
	virtual float         GetStringWidth( const LString& Str, iFontProperties* Font ) const;
	virtual float         GetStringHeight() const;
	// look'n'feel
	virtual void          InitLookAndFeel();
	virtual float         GetDefaultCaptionHeight() const;
	virtual float         GetDefaultCaptionLedge() const;
	virtual float         GetDefaultCaptionRadius() const;
	virtual float         GetDefaultResizeCornerWidth() const;
	virtual float         GetDefaultResizeCornerHeight() const;
	virtual float         GetDefaultCloseButtonWidth() const;
	virtual float         GetDefaultCloseButtonHeight_Delta1() const;
	virtual float         GetDefaultCloseButtonHeight_Delta2() const;
	virtual float         GetDefaultVerticalTitlePosition() const;
	virtual float         GetDefaultButtonBevelRadius() const;
	virtual float         GetDefaultPanelBevelRadius() const;
	virtual float         GetDefaultDockingDistance() const;
	virtual float         GetDefaultWindowBorderWidth() const;
	virtual float         GetDefaultWindowBorderHeight() const;
	virtual float         GetDefaultButtonVerticalTitleOffset() const;
	virtual float         GetDefaultCheckBoxWidth() const;
	virtual float         GetDefaultCheckBoxHorizontalTitleOffset() const;
	virtual float         GetDefaultCheckBoxHeight_Delta1() const;
	virtual float         GetDefaultCheckBoxHeight_Delta2() const;
	virtual float         GetDefaultGaugeXOffset() const;
	virtual float         GetDefaultGaugeYOffset() const;
	virtual float         GetDefaultGaugeTextYOffset() const;
	// callbacks
	virtual void          Callback_SizeView( iGUIView* View ) { FViewBeenSized = View; };
	virtual void          Callback_DragView( iGUIView* View ) { FViewBeenDragged = View; };
	//
	FWD_EVENT_HANDLER( Event_TIMER );
	FWD_EVENT_HANDLER( Event_DRAWOVERLAY );
	FWD_EVENT_HANDLER( Event_KEY );
	FWD_EVENT_HANDLER( Event_POSTINIT );

	// Service routines. Without them it IS totally unusable

	/// Add a button which invokes the console command
	scriptmethod clGUIPushButton*  AddButtonCmd     ( const LVector2& Pos, const LVector2& Size, const LString& Title, const LString& Command );

	/// Add a button which invokes the procedure (implemented as some universal wrapper for ConsoleProc to avoid another hook in GUIView)
	scriptmethod clGUIPushButton*  AddButtonCallback( const LVector2& Pos, const LVector2& Size, const LString& Title, clConsole::clConsoleProc Callback );

	/// Add a slider and bind it to variable. Initial value is taken from variable
	scriptmethod clGUISlider*      AddSlider        ( const LVector2& Pos, const LVector2& Size, float Min, float Max, const LString& ToolTip, const LString& VarName );

	/// Add a static text with variable's string contents
	scriptmethod clGUIStaticWidget* AddStatic       ( const LVector2& Pos, const LVector2& Size, const LString& VarName );

	/// Add an editbox and bind it to variable
	scriptmethod clGUIInputLine*   AddEditBox       ( const LVector2& Pos, const LVector2& Size, const LString& VarName );

	/// Add a checkbox and bind it to variable
	scriptmethod clGUICheckBox*    AddCheckBox      ( const LVector2& Pos, const LVector2& Size, const LString& Title, const LString& VarName );

	/// Add a color dial and bind it to the vec4 variable
	scriptmethod clGUIColorDial*   AddColorDial     ( const LVector2& Pos, const LVector2& Size, const LString& OutVarName );

	/// Add a color dial and bind it to the vec4 variable
	scriptmethod clGUIMtxView*     AddMtxView       ( const LVector2& Pos, const LVector2& Size, const LString& InVarName );

	/// Add a full-screen graph attached to the variable
	scriptmethod clGUIFullScreenGraph*   AddFSGraph ( const LString& InVarName, const LString& VisibleVarName );

	/// Bind a property of some object to the slider
	scriptmethod clGUISlider*      AddPropertySlider( const LVector2& Pos, const LVector2& Size, float Min, float Max, const LString& ToolTip, iObject* Obj, const LString& PropName );

	/// Bind a property of some object to the editor
	scriptmethod clGUIInputLine*   AddPropertyEdit  ( const LVector2& Pos, const LVector2& Size, iObject* Obj, const LString& PropName );

	/// Bind a property of some object to the static text
	scriptmethod clGUIStaticWidget* AddPropertyStatic ( const LVector2& Pos, const LVector2& Size, iObject* Obj, const LString& PropName );

	/// Bind a boolean property to checkbox modifier
	scriptmethod clGUICheckBox*    AddPropertyCheck ( const LVector2& Pos, const LVector2& Size, const LString& Title, iObject* Obj, const LString& PropName );

	/// Add a color dial and bind it to the vec4 variable
	scriptmethod clGUIColorDial*   AddPropColorDial ( const LVector2& Pos, const LVector2& Size, iObject* Obj, const LString& PropName );

	/// Add a new scene view
	scriptmethod clGUISceneView*   AddSceneView( const LVector2& Pos, const LVector2& Size, const LMatrix4& Proj, const LMatrix4& View, clScene* Scene );

	/// Add a new visual scene view
	scriptmethod clGUIVisualSceneView* AddVisualSceneView( const LVector2& Pos, const LVector2& Size, clVisualScene* Sc, clVisualProjector* Prj, clRenderingTechnique* RT );

	/// Return true if we are on a smartphone with a touchscreen
	scriptmethod bool IsTouchScreen() const;

	virtual bool IsGUISoundEnabled() const;
	virtual void SetGUISoundEnabled( bool Enabled );

private:
	inline void    LoadGUIFile( const LString& FileName, iGUIView* To );
	inline void    SaveGUIFile( const LString& FileName );
	inline void    PurgeViews();
	inline void    CheckMouse();
	inline void    RenderToolTip() const;
private:
	clCVar*          FGUISoundEnabled;
	bool             FGUIVisible;
	clGUIDesktop*    FDesktop;
	clViewsList      FGlobalViewsList;
	clViewsList      FClosingViewsList;
	clViewsList      FDockableViewsList;
	clViewsList      FKeyboardSelectableViewsList;
	int              FPrevKeyboardSelectedView;
	bool             FRecheckMouse;
	LVector2         FMousePosition;
	LCursorForm      FCursorForm;
	float            FMouseIdleTime;
	float            FMouseMovementDelay;
	mutable iRenderTarget* FWindowBuffer;
	iVertexArray*    FDefaultRect;
	iVertexArray*    FDefaultBevelRect;
	iVertexArray*    FDefaultTriangle;

	inline void DropDragging( const LVector2& MousePosition )
	{
		if ( FViewBeenDragged )
		{
			// drop view
			FViewBeenDragged->Event_StopDragging( FViewBeenDragged, MousePosition );
			FViewBeenDragged->Event_MouseOutside( FViewBeenDragged, MousePosition );
			FViewBeenDragged = NULL;
		}
	}
	inline void DropResizeing( const LVector2& MousePosition )
	{
		if ( FViewBeenSized )
		{
			// drop view
			FViewBeenSized->Event_StopResize( FViewBeenSized, MousePosition );
			FViewBeenSized->Event_MouseOutside( FViewBeenSized, MousePosition );
			FViewBeenSized = NULL;
		}
	}
	inline void DropActiveViews( const LVector2& MousePosition )
	{
		if ( FActiveView ) { FActiveView->Event_MouseOutside( FActiveView, MousePosition ); }

		if ( FCapturedView ) { FCapturedView->Event_CaptureChanged( FCapturedView ); }

		if ( FViewBeenSized ) { FViewBeenSized->Event_StopResize( FViewBeenSized, MousePosition ); }

		if ( FViewBeenDragged ) { FViewBeenDragged->Event_StopDragging( FViewBeenDragged, MousePosition ); }

		FActiveView = FCapturedView = FViewBeenSized = FViewBeenDragged = FTopmostView = NULL;
	}

	iGUIView*   FActiveView;
	iGUIView*   FCapturedView;
	iGUIView*   FViewBeenSized;
	iGUIView*   FViewBeenDragged;
	iGUIView*   FTopmostView;
	clGUIPage*  FActivePage;
private:
	// commands handlers
	virtual void    AddGUIC( const LString& Param );
	virtual void    AddGUIToC( const LString& Param );
	virtual void    SaveGUIC( const LString& Param );
	virtual void    ResetGUIC( const LString& Param );
	virtual void    CloseViewC( const LString& Param );
	virtual void    ToggleGUIC( const LString& Param );
	virtual void    SetActivePageC( const LString& Param );
private:
	// look'n'feel
	clCVar* DefaultCaptionHeight;
	clCVar* DefaultCaptionLedge;
	clCVar* DefaultCaptionRadius;
	clCVar* DefaultResizeCornerWidth;
	clCVar* DefaultResizeCornerHeight;
	clCVar* DefaultCloseButtonWidth;
	clCVar* DefaultCloseButtonHeight_Delta1;
	clCVar* DefaultCloseButtonHeight_Delta2;
	clCVar* DefaultVerticalTitlePosition;
	clCVar* DefaultButtonBevelRadius;
	clCVar* DefaultPanelBevelRadius;
	clCVar* DefaultDockingDistance;
	clCVar* DefaultWindowBorderWidth;
	clCVar* DefaultWindowBorderHeight;
	clCVar* DefaultButtonVerticalTitleOffset;
	clCVar* DefaultCheckBoxWidth;
	clCVar* DefaultCheckBoxHorizontalTitleOffset;
	clCVar* DefaultCheckBoxHeight_Delta1;
	clCVar* DefaultCheckBoxHeight_Delta2;
	clCVar* DefaultGaugeXOffset;
	clCVar* DefaultGaugeYOffset;
	clCVar* DefaultGaugeTextYOffset;
};

#endif

/*
 * 05/07/2010
     ProcessInputLineKey()
 * 23/06/2007
     OpenContextMenu()
 * 22/06/2007
     ToggleGUIC()
 * 05/04/2007
     SaveGUIC()
 * 25/01/2007
     FMouseMovementDelay
 * 14/01/2007
     Activate()
     Deactivate()
 * 09/12/2005
     GetDefaultFontWidth()
 * 06/05/2005
     GetDefaultGaugeXOffset()
     GetDefaultGaugeYOffset()
 * 04/03/2005
     Callback_SizeView()
     Callback_DragView()
 * 05/02/2005
     GetCursorForm()
     SetCursorForm()
 * 03/02/2005
     clActiveViews
 * 26/01/2005
     It's here
*/
