#include "I_SceneView.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/Logger.h"
#include "Core/Linker.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "GUI/GUIManager.h"
#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"
#include "Resources/ResourcesManager.h"
#include "Utils/Viewport.h"
#include "Scene/Scene.h"
#include "Scene/Postprocess/RenderingTechnique.h"
#include "VisualScene/VisualScene.h"
#include "VisualScene/VisualProjector.h"

#include "LKeys.h"

clGUISceneView::clGUISceneView(): FTechnique( NULL ), FOutRT( NULL )
{
	FViewType = SCENE_VIEW_STD;
}

clGUISceneView::~clGUISceneView()
{
	delete FOutRT;
	delete FProcessedRT;
}

void clGUISceneView::AfterConstruction()
{
	clGUIPanel::AfterConstruction();

	FShader = Env->Resources->CreateCustomShader( "Interface/Programs/GUI.sp", "" );
	FShader->FDepthTest = false;

	FOutRT = Env->Renderer->CreateRenderTarget( iRenderTarget::FULLSCREEN, iRenderTarget::FULLSCREEN, 0, 8, true, 1 );

	// no z-buffer
	FProcessedRT = Env->Renderer->CreateRenderTarget( iRenderTarget::FULLSCREEN, iRenderTarget::FULLSCREEN, 0, 8, false, 1 );
}

void clGUISceneView::PreRender()
{
	FScene->SetRenderBuffer( FOutRT );

//		Env->Renderer->ClearRenderTarget(true, true, true);

	// TODO: switch scene rendering mode
	if ( FViewType == SCENE_VIEW_WIREFRAME )
	{
		Env->Renderer->SetPolygonFillMode( false );
	}

//		FScene->SetW

	if ( FViewType == SCENE_VIEW_WORLD_NORMALS )
	{
		Env->Console->GetVar( "Scene.ShowNormals" )->SetBool( true );
	}

//		FScene->SetViewType( FViewType);

	FScene->SetCameraTransform( FView );
	FScene->SetCameraProjection( FProjection );
	FScene->RenderForward();

	if ( FViewType == SCENE_VIEW_WIREFRAME )
	{
		Env->Renderer->SetPolygonFillMode( true );
	}

	if ( FViewType == SCENE_VIEW_WORLD_NORMALS )
	{
		Env->Console->GetVar( "Scene.ShowNormals" )->SetBool( false );
	}

//		TODO: Restore default scene rendering type
	// FScene->RestoreView(); ?

	FScene->SetRenderBuffer( NULL );

	if ( FTechnique )
	{
		FTechnique->ProcessBuffer( FOutRT, FProcessedRT );

		FShader->SetTexture( 0, FProcessedRT->GetColorTexture( 0 ), false );
	}
	else
	{
		FShader->SetTexture( 0, FOutRT->GetColorTexture( 0 ), false );
	}

	clGUIPanel::PreRender();
}

void clGUIVisualSceneView::UpdateView()
{
	bool OldUpdated = FUpdated;
	FUpdated = true;

	PreRender();

	FUpdated = OldUpdated;
}

void clGUIVisualSceneView::PreRender()
{
	if ( !FUpdated ) { return; }

	if ( !FVisualScene ) { return; }

	if ( !FProjector ) { return; }

	/// fetch transform and projection

	FProjection = FProjector->GetProjection();
	FView = FProjector->GetTransform();

	clGUISceneView::PreRender();
}

void clGUIVisualSceneView::SetProjector( clVisualProjector* Proj )
{
	FProjector = Proj;
}

bool clGUIVisualSceneView::Event_Key( iGUIResponder* Source, const int Key, const bool KeyState )
{
	if ( dynamic_cast<clGUIEditorSceneView*>( GetParentView() ) )
	{
		return GetParentView()->Event_Key( Source, Key, KeyState );
	}

	return false;
}

void clGUIVisualSceneView::SetScene( clVisualScene* Sc )
{
	FVisualScene = Sc;

	if ( !Sc ) { return; }

	// link to clScene field
	FScene = Sc->GetScene();
}

clGUIEditorSceneView::~clGUIEditorSceneView()
{
}

void clGUIEditorSceneView::SetLayout( eViewLayout LL )
{
	FLayout = LL;

	/// Handle default modes
	if ( LL == VIEW_LAYOUT_TILE ) { Tile(); }
}

void clGUIEditorSceneView::AfterConstruction()
{
	clGUIPanel::AfterConstruction();
	FShader = Env->Resources->CreateCustomShader( "Interface/Programs/GUI.sp", "" );

	for ( int i = 0 ; i < 4 ; i++ )
	{
		clGUIVisualSceneView* SubView = Env->Linker->Instantiate( "clGUIVisualSceneView" );

		AddView( SubView );
	}

	// Tile the views by default
	SetLayout( VIEW_LAYOUT_TILE );
}

void clGUIEditorSceneView::SetVisualScene( clVisualScene* Sc )
{
	FScene = Sc;

	for ( int i = 0 ; i < MAX_VIEW_RT ; i++ ) { dynamic_cast<clGUIVisualSceneView*>( GetSubView( i ) )->SetScene( Sc ); }
}

/// Equal rectangles for each of the views:
//   0 ->   1
//   ^      |
//   |     \ /
//   3  <-  2
void clGUIEditorSceneView::Tile()
{
	float SubW = GetWidth() / 2.0f;
	float SubH = GetHeight() / 2.0f;

	LVector2 Pos[] = { vec2( 0, 0 ), vec2( SubW, 0.0f ), vec2( SubW, SubH ), vec2( 0.0f, SubH ) };

	for ( int i = 0 ; i < 4 ; i++ )
	{
		iGUIView* V = GetSubView( i );
		V->SetWidth( SubW );
		V->SetHeight( SubH );
		V->MoveTo( Pos[i] );
	}

	ShowAll( true );
	SetUpdateFlagForAll( false );

	FActiveView = 2;
}

void clGUIEditorSceneView::SplitScreen( bool Horiz, int View1, int View2 )
{
	FActiveView = View1;

	float NewW = Horiz ? GetWidth()      : GetWidth() / 2.0f;
	float NewH = Horiz ? GetHeight() / 2.0f : GetHeight();

	float NewX2 = Horiz ? 0 : GetWidth() / 2.0f;
	float NewY2 = Horiz ? GetHeight() / 2.0f : 0;

	iGUIView* V1 = GetSubView( View1 );
	iGUIView* V2 = GetSubView( View2 );

	ShowAll( false );

	V1->SetVisible( true );
	V2->SetVisible( true );

	V1->SetWidth( NewW );
	V1->SetHeight( NewH );
	V2->SetWidth( NewW );
	V2->SetHeight( NewH );

	V1->MoveTo( vec2( 0, 0 ) );
	V2->MoveTo( vec2( NewX2, NewY2 ) );
}

void clGUIEditorSceneView::SetProjectorForPart( int ViewID, clVisualProjector* Proj )
{
	dynamic_cast<clGUIVisualSceneView*>( GetSubView( ViewID ) )->SetProjector( Proj );
}

void clGUIEditorSceneView::SetTechniqueForPart( int ViewID, clRenderingTechnique* RT )
{
	dynamic_cast<clGUIVisualSceneView*>( GetSubView( ViewID ) )->SetRenderTechnique( RT );
}

void clGUIEditorSceneView::UpdateView()
{
	for ( int i = 0 ; i != MAX_VIEW_RT ; i++ ) { dynamic_cast<clGUIVisualSceneView*>( GetSubView( i ) )->UpdateView(); }
}

void clGUIEditorSceneView::SetUpdateFlagForAll( bool Upd )
{
	for ( int i = 0 ; i < MAX_VIEW_RT ; i++ ) { dynamic_cast<clGUIVisualSceneView*>( GetSubView( i ) )->FUpdated = Upd; }
}

void clGUIEditorSceneView::MaximizePart( int ViewID, bool MaximizedFlag )
{
	FActiveView = ViewID;
	FViewMaximized = MaximizedFlag;

	if ( MaximizedFlag )
	{
		ShowAll( false );

		iGUIView* V = GetSubView( ViewID );
		V->SetVisible( true );
		V->SetWidth( GetWidth() );
		V->SetHeight( GetHeight() );
	}
	else
	{
		// restore old view, tile everthing by now
		Tile();
	}
}

void clGUIEditorSceneView::ShowAll( bool Visible )
{
	for ( int i = 0 ; i < MAX_VIEW_RT ; i++ ) { GetSubView( i )->SetVisible( Visible ); }
}

bool clGUIEditorSceneView::Event_Key( iGUIResponder* Source, const int Key, const bool KeyState )
{
	/// handle maximization keystroke (Alt+W)
	if ( KeyState && ( Key == LK_W ) )
	{
		if ( Env->Console->IsKeyPressed( LK_MENU ) )
		{
			MaximizePart( FActiveView, !FViewMaximized );
			return false;
		}
	}

	return true;
}

void clGUIEditorSceneView::Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState )
{
	// first switch the active view, then transform mouse coords and pass the key to scene
}

void clGUIEditorSceneView::Event_MouseRight( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState )
{
}
