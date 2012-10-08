#ifndef __Scene_view_h__included__
#define __Scene_view_h__included__

#include "I_Panel.h"

class clScene;
class clEditor;
class clVisualScene;
class clVisualProjector;
class iRenderTarget;
class clRenderingTechnique;

/// Type of scene rendering
enum eSceneView
{
	/// World coordinates
	SCENE_VIEW_WORLD         = 0,

	/// World-space normals
	SCENE_VIEW_WORLD_NORMALS = 1,

	/// Object-space normals
	SCENE_VIEW_OBJ_NORMALS   = 2,

	/// Standart rendering (using default technique)
	SCENE_VIEW_STD           = 3,

	/// Wireframe view of the scene
	SCENE_VIEW_WIREFRAME     = 4,

	/// Depth buffer
	SCENE_VIEW_DEPTH         = 5,

	/// Linearized depth buffer
	SCENE_VIEW_NORM_DEPTH    = 6
};

/// Scene viewer
/* netexportable */
class scriptfinal clGUISceneView: public clGUIPanel
{
public:
	clGUISceneView();
	virtual ~clGUISceneView();

	virtual void AfterConstruction();

	virtual void    PreRender();

	virtual void    SetShaderName( const LString& FileName ) { } // no shader overrides

	scriptmethod void SetRenderTechnique( clRenderingTechnique* RT ) { FTechnique = RT; }

	/** Property(Description="View matrix", Name=View, Type=mtx4, FieldName=FView) */
	/** Property(Description="Projection matrix", Name=Projection, Type=mtx4, FieldName=FProjection) */
public:
	/// Define how the actual view looks like
	eSceneView FViewType;

	/// Camera transform
	LMatrix4 FView;

	/// Projection matrix
	LMatrix4 FProjection;

	/// Link to rendered scene
	clScene* FScene;

	/// Current rendering technique for this view
	clRenderingTechnique* FTechnique;

protected:
	/// For the intermediate output we use RT
	iRenderTarget* FOutRT;

	iRenderTarget* FProcessedRT;
};

/// Debug output of light's or camera's properties (uses VisualScene)
/* netexportable */
class scriptfinal clGUIVisualSceneView: public clGUISceneView
{
public:
	clGUIVisualSceneView(): FVisualScene( NULL ), FProjector( NULL ), FUpdated( true ), FRenderDebugScene( false ) {}
	virtual ~clGUIVisualSceneView() {};

	virtual bool Event_Key( iGUIResponder* Source, const int Key, const bool KeyState );

	virtual void    PreRender();

	/// Force content repaint
	scriptmethod void UpdateView();

	/// Set used projector
	scriptmethod void SetProjector( clVisualProjector* Proj );

	/// Set link to visual scene
	scriptmethod void SetScene( clVisualScene* Sc );

public:
	/// Which part of VisualScene to render - normal or debug
	bool FRenderDebugScene;

	/// Is it re-rendered each frame
	bool FUpdated;

	/// Cached link to the scene
	clVisualScene* FVisualScene;

	/// Light or camera for the display (View/Proj container). The link to VisualScene is inside
	clVisualProjector* FProjector;
};

/// 4 by default
const int MAX_VIEW_RT = 4;

/// Layout of the EditorSceneView
enum eViewLayout
{
	/// Tiled (equal rectangles)
	VIEW_LAYOUT_TILE    = 0,

	/// Split-screen (vertical)
	VIEW_LAYOUT_SPLIT_V = 1,

	/// Split-screen (horizontal)
	VIEW_LAYOUT_SPLIT_H = 2,

	/// Non-standart, user adjusted
	VIEW_LAYOUT_CUSTOM  = 3
};

/**
   \brief Multi-view on-screen gui control

   Redirects events to the VisualScene or attached editor.

   Maybe we should implement as a container of multiple SceneView controls,
   but right now we just make the straight-forward implementation with a fixed
   number of views.

*/
/* netexportable */
class scriptfinal clGUIEditorSceneView : public clGUIPanel
{
public:
	clGUIEditorSceneView(): FLayout( VIEW_LAYOUT_CUSTOM ), FActiveView( 2 ) {}
	virtual ~clGUIEditorSceneView();

	virtual void AfterConstruction();

	scriptmethod void SetProjectorForPart( int ViewID, clVisualProjector* Proj );

	scriptmethod void SetTechniqueForPart( int ViewID, clRenderingTechnique* RT );

	scriptmethod void SetEditor( clEditor* EE ) { FEditor = EE; }

	scriptmethod void SetVisualScene( clVisualScene* Sc );

	/// Get current view layout
	scriptmethod eViewLayout GetLayout() const { return FLayout; }

	/// Set current layout
	scriptmethod void SetLayout( eViewLayout LL );

	/// Force repaint for each views
	scriptmethod void UpdateView();

	/// Toggle update for each view
	scriptmethod void SetUpdateFlagForAll( bool Upd );

	/// Maximize/normalize one of the viewports
	scriptmethod void MaximizePart( int ViewID, bool MaximizedFlag );

#pragma region Event handling

	/**
	   Handle internal events or redirect events to one of the viewports (scene)

	   One important remark is that the cooridnates of the mouse clicks
	   are renormalized to fit size of each of the views.

	   Only active view receives events.
	*/

	virtual bool    Event_Key( iGUIResponder* Source, const int Key, const bool KeyState );
	virtual void    Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState );
	virtual void    Event_MouseRight( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState );

#pragma endregion

public:
	/// Show or hide each view
	scriptmethod void ShowAll( bool Visible );

	/// Two views (split horizontaly or verticaly)
	scriptmethod void SplitScreen( bool Horiz, int View1, int View2 );

	/// Equal rectangles for each of the views
	scriptmethod void Tile();

	/// Subviews
	clGUIVisualSceneView* FViews[MAX_VIEW_RT];

	/// Cached link to visual scene
	clVisualScene* FScene;

	/// Index of the active view
	int FActiveView;

	/// Alt+W toggle (maximized one of the views)
	bool FViewMaximized;

	/// Current control layout
	eViewLayout FLayout;

	/// Direct link to current active editor instance. NULL can be set if no editor needed
	clEditor* FEditor;
};

#endif
