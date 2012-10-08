/**
 * \file VisualScene.cpp
 * \brief Visual scene
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

/**
   PROBLEMS :

      - no debug scene (AddToScenes/RemoveFromScenes use FScene only, not FDebugScene !)

      - no picking tests

      - no correct blending of Debug/Real scenes

      - no volumes/particles

      - AddSubObject and HierarchyCreator somehow adds itself to the list
*/

#include "VisualScene.h"
#include "VisualObject.h"
#include "VisualCamera.h"
#include "VisualRenderables.h"
#include "VisualModifier.h"
#include "VisualProjector.h"

#include "CameraPositioner.h"

#include "Math/LGeomUtils.h"
#include "Scene/Scene.h"

#include "Core/VFS/FileSystem.h"
#include "Core/Linker.h"
#include "Core/Logger.h"
#include "Environment.h"

#include "Scene/Postprocess/RenderingTechnique.h"

#include "Gizmos.h"

clVisualScene::clVisualScene() : FLastError( "" )
{
	FActiveCamera = -1;
}

clVisualScene::~clVisualScene()
{
	Clear();

	FScene->DisposeObject();
	FDebugScene->DisposeObject();

	FTechnique->DisposeObject();
}

void clVisualScene::Register()
{
	FTimerHandler = Utils::Bind( &clVisualScene::Event_TIMER, this );
	Env->Connect( L_EVENT_TIMER,  FTimerHandler );
	FKeyHandler = Utils::Bind( &clVisualScene::Event_KEY, this );
	Env->Connect( L_EVENT_KEY,  FKeyHandler );
}

void clVisualScene::Unregister()
{
	Env->Disconnect( L_EVENT_TIMER, FTimerHandler );
	Env->Disconnect( L_EVENT_KEY,   FKeyHandler );
}

void clVisualScene::Event_TIMER( LEvent Event, const LEventArgs& Args )
{
	/// Redirect to update
	Update( Args.FFloatArg );
}

void clVisualScene::Event_KEY( LEvent Event, const LEventArgs& Args )
{
	HandleKey( ( int )Args.FFloatArg, Args.FBoolArg );
}

clVisualObject* clVisualScene::FindObject( const LString& _ID )
{
	int Sz = GetTotalSubObjects();

	for ( int j = 0 ; j < Sz ; j++ )
	{
		clVisualObject* O = GetObject( j );

		if ( O->GetObjectID() == _ID )
		{
			return O;
		}
	}

	return NULL;
}

int clVisualScene::GetObjectIndex( clVisualObject* Obj ) const
{
	int Sz = GetTotalSubObjects();

	for ( int j = 0 ; j < Sz ; j++ )
	{
		if ( GetObjectConst( j ) == Obj ) { return j; }
	}

	return -1;
}

int clVisualScene::GetCameraIndex( clVisualCamera* Cam ) const
{
	for ( size_t j = 0 ; j < FCameras.size() ; j++ )
		if ( FCameras[j] == Cam )
		{
			return ( int )j;
		}

	return -1;
}

int clVisualScene::GetLightIndex( clVisualLight* L ) const
{
	for ( size_t j = 0 ; j < FLights.size() ; j++ )
		if ( FLights[j] == L )
		{
			return ( int )j;
		}

	return -1;
}

int clVisualScene::GetModifierIndex( clVisualModifier* M ) const
{
	for ( size_t j = 0 ; j < FModifiers.size() ; j++ )
		if ( FModifiers[j] == M )
		{
			return ( int )j;
		}

	return -1;
}

int clVisualScene::GetRenderableIndex( clVisualRenderable* R ) const
{
	for ( size_t j = 0 ; j < FVisuals.size() ; j++ )
		if ( FVisuals[j] == R )
		{
			return ( int )j;
		}

	return -1;
}

/// Pick object using current mouse coordinates
clVisualObject* clVisualScene::PickObject( float x, float y )
{
	LMatrix4 Proj = GetCurrentProjection();
	LMatrix4 View = GetCurrentView();

	LVector3 srcPoint;
	LVector3 dir;
	MouseCoordsToWorldPointAndRay( Proj, View, x, y, srcPoint, dir );

	LVector3 isect;

	clVisualObject* Obj = NULL;

	for ( int i = 0 ; i != GetTotalSubObjects() ; i++ )
	{
		Obj = GetObject( i );

		if ( Obj->IntersectsWithRay( Proj, View, srcPoint, dir, isect ) )
		{
			return Obj;
		}
	}

	return NULL;
}

LMatrix4 clVisualScene::GetCurrentView()
{
	if ( FActiveCamera == -1 ) { return LMatrix4::Identity(); }

	return FCameras[FActiveCamera]->GetTransform();
}

LMatrix4 clVisualScene::GetCurrentProjection()
{
	if ( FActiveCamera == -1 ) { return LMatrix4::Identity(); }

	return FCameras[FActiveCamera]->GetProjection();
}

void clVisualScene::RenderAuxBuffers( const LMatrix4& Proj, const LMatrix4& View )
{
	for ( int i = 0 ; i != GetTotalSubObjects() ; i++ )
	{
		GetObject( i )->RenderAuxBuffers( Proj, View );
	}
}

void clVisualScene::Update( float DT )
{
	for ( int i = 0 ; i != GetTotalSubObjects() ; i++ )
	{
		GetObject( i )->Update( DT );
	}
}

void clVisualScene::HandleKey( int Key, bool State )
{
	// handle keys if modifiers are present
}

/**
   FIX IT!!!
*/
void clVisualScene::Render()
{
	if ( FActiveCamera == -1 ) { return; }

	clVisualCamera* Cam = FCameras[FActiveCamera];

	if ( Cam->FPositioner ) { Cam->SetTransform( Cam->FPositioner->GetCurrentTransform() ); }

	LMatrix4 Proj = Cam->GetProjection();
	LMatrix4 View = Cam->GetTransform();

	// 1. Stage one : render volumes

	// 2. Stage two : render auxillary buffers (like noise from particles)
	RenderAuxBuffers( Proj, View );

	// 3. Stage three : render base scene

	FScene->SetCameraProjection( Proj );
	FScene->SetCameraTransform( View );
	FScene->SetUseOffscreenBuffer( /*false*/ true, false );

	if ( FTechnique )
	{
		FTechnique->Render( FScene );
	}
	else
	{
		FScene->RenderForward();
	}

	// 4. Stage four : render debug scene

	// 5. Stage five : render debug information
//	RenderDebug(Proj, View);
}

void clVisualScene::RenderDebug( const LMatrix4& Proj, const LMatrix4& View )
{
	for ( int i = 0 ; i != GetTotalSubObjects() ; i++ )
	{
		GetObject( i )->RenderDebug( Proj, View );
	}
}

bool clVisualScene::BeginLoad()
{
	// 1. Clear the scene
	Clear();

	return true;
}

bool clVisualScene::EndLoad()
{
	// combine the objects in a single list
	for ( int i = 0 ; i != GetNumCameras() ; i++ ) { AddSubObject( GetCamera( i ) ); }

	for ( int i = 0 ; i != GetNumLights() ; i++ ) { clVisualLight* L = GetLight( i ); L->UpdateProjection(); AddSubObject( L ); }

	for ( int i = 0 ; i != GetNumRenderables() ; i++ ) { AddSubObject( GetRenderable( i ) ); }

	for ( int i = 0 ; i != GetNumModifiers() ; i++ ) { AddSubObject( GetModifier( i ) ); }

	for ( int i = 0 ; i != GetNumVolumes() ; i++ ) { AddSubObject( GetVolume( i ) ); }

	for ( int i = 0 ; i != GetNumParticles() ; i++ ) { AddSubObject( GetParticles( i ) ); }

	// Add every object to scenes
	AddToScenes();

	// ping every object about finished loading
	for ( int i = 0 ; i < GetTotalSubObjects() ; i++ )
	{
		if ( !GetObject( i )->EndLoad() ) { return false; }
	}

	return true;
}

void clVisualScene::AfterConstruction()
{
	// create two scenes
	FDebugScene = Env->Linker->Instantiate( "clScene" );
	FScene = Env->Linker->Instantiate( "clScene" );

#if !defined(OS_ANDROID)
	// disable postprocessing - too much shaders there
	FTechnique = Env->Renderer->CreatePostprocessChain();
#endif
}

void clVisualScene::RemoveFromScenes()
{
	for ( int i = 0 ; i != GetTotalSubObjects() ; i++ )
	{
		GetObject( i )->RemoveFromScene( this );
	}
}

void clVisualScene::AddToScenes()
{
	for ( int i = 0 ; i != GetTotalSubObjects() ; i++ )
	{
		GetObject( i )->AddToScene( this );
	}
}

void clVisualScene::Clear()
{
	// 1. remove geometry from scenes
	RemoveFromScenes();

	// 2. actual clearance
	for ( int i = 0 ; i != GetTotalSubObjects() ; i++ ) { GetObject( i )->DisposeObject(); }

	ClearSubObjects();

	FCameras.clear();
	FLights.clear();
	FVisuals.clear();
	FModifiers.clear();
	FVolumes.clear();
	FParticles.clear();
}

void clVisualScene::AddLight( clVisualLight* L )
{
	// 1. Add to scene
	L->AddToScene( this );
	// 2. Store in internal list
	FLights.push_back( L );
	AddSubObject( L );

	// 3. Pass parameters through SyncWithScene
}

clVisualSpotLight* clVisualScene::AddSpotLight()
{
	clVisualSpotLight* L = Env->Linker->Instantiate( "clVisualSpotLight" );
	AddLight( L );
	return L;
}

clVisualPointLight* clVisualScene::AddPointLight()
{
	clVisualPointLight* L = Env->Linker->Instantiate( "clVisualPointLight" );
	AddLight( L );
	return L;
}

clVisualDirectionalLight* clVisualScene::AddDirectionalLight()
{
	clVisualDirectionalLight* L = Env->Linker->Instantiate( "clVisualDirectionalLight" );
	AddLight( L );
	return L;
}

clVisualCamera* clVisualScene::AddCamera( const LMatrix4& Projection, const LMatrix4& Transform, iCameraPositioner* Pos )
{
	clVisualCamera* Cam = Env->Linker->Instantiate( "clVisualCamera" );

	Cam->SetProjection( Projection );
	Cam->SetPositioner( Pos );
	Cam->SetTransform( Transform );

	FCameras.push_back( Cam );

	Cam->AddToScene( this );

	if ( Pos != NULL )
	{
		Cam->SetTransform( Pos->GetCurrentTransform() );
	}

	AddSubObject( Cam );

	return Cam;
}

clVisualGeom* clVisualScene::AddGeom( clGeom* G, clMaterial* Mat, const LMatrix4& Transform )
{
	clVisualGeom* Geom = Env->Linker->Instantiate( "clVisualGeom" );

	Geom->SetGeom( G );
	Geom->SetMaterial( Mat );

	Geom->AddToScene( this );

	Geom->SetTransform( Transform );

	FVisuals.push_back( Geom );

	AddSubObject( Geom );

	return Geom;
}

void clVisualScene::AddModifier( clVisualModifier* Mod )
{
	Mod->AddToScene( this );

	FModifiers.push_back( Mod );
	AddSubObject( Mod );
}

//	scr_iptmethod VisualVolume* void VisualScene::AddVolume() { return NULL; }
clVisualParticles* clVisualScene::AddParticles()
{
	return NULL;
}

void clVisualScene::RemoveSubObject( clVisualObject* SubObject )
{
	int idx = GetObjectIndex( SubObject );

	if ( idx < 0 || idx >= static_cast<int>( FObjects.size() ) ) { return; }

	FObjects.erase( FObjects.begin() + idx );
}

void clVisualScene::RemoveLight( clVisualLight* L )
{
	// 1. shift all IDs for the lights
	for ( size_t i = 0 ; i < FLights.size() ; i++ )
		if ( FLights[i]->FInternalID > L->FInternalID )
		{
			FLights[i]->FInternalID--;
		}

	// 2. remove the light from Scene
	FScene->RemoveLight( L->FInternalID );

	int idx = GetLightIndex( L );
	FLights.erase( FLights.begin() + idx );

	RemoveSubObject( L );
}

void clVisualScene::RemoveCamera( clVisualCamera* Cam )
{
	int idx = GetCameraIndex( Cam );

	if ( idx < 0 || idx >= static_cast<int>( FCameras.size() ) ) { return; }

	if ( idx == FActiveCamera ) { FActiveCamera = -1; } // reset active camera

	Cam->RemoveFromScene( this );

	FCameras.erase( FCameras.begin() + idx );

	RemoveSubObject( Cam );

	delete Cam;
}

void clVisualScene::RemoveRenderable( clVisualRenderable* R )
{
	int idx = GetRenderableIndex( R );

	if ( idx < 0 || idx >= static_cast<int>( FVisuals.size() ) ) { return; }

	R->RemoveFromScene( this );

	FVisuals.erase( FVisuals.begin() + idx );
	RemoveSubObject( R );

	delete R;
}

void clVisualScene::RemoveGeom( clVisualGeom* G )
{
	RemoveRenderable( G );
}

void clVisualScene::RemoveVolume( clVisualVolume* V )
{
}

void clVisualScene::RemoveParticles( clVisualParticles* PP )
{
}

void clVisualScene::RemoveModifier( clVisualModifier* Mod )
{
	int idx = GetModifierIndex( Mod );

	if ( idx < 0 || idx >= static_cast<int>( FModifiers.size() ) ) { return; }

	Mod->RemoveFromScene( this );

	FModifiers.erase( FModifiers.begin() + idx );
	RemoveSubObject( Mod );

	// ugly, but hopefully this is just for immediate effect
	if ( clGizmo* GG = dynamic_cast<clGizmo*>( Mod ) )
	{
		GG->Unregister();
	}

	delete Mod;
}

void clVisualScene::AddSubObject( clVisualObject* SubObject )
{
	FObjects.push_back( SubObject );
}

////////////

clTranslateGizmo* clVisualScene::AddTranslateGizmoPropMtx( const LString& ObjID, const LString& PropID )
{
	clTranslateGizmo* G = Env->Linker->Instantiate( "clTranslateGizmo" );
	AddModifier( G );
	G->Register();
	G->BindToPropMatrix( Env->FindObject( ObjID ), PropID );

	return G;
}

clTranslateGizmo* clVisualScene::AddTranslateGizmoPropVec( const LString& ObjID, const LString& PropID )
{
	clTranslateGizmo* G = Env->Linker->Instantiate( "clTranslateGizmo" );
	AddModifier( G );
	G->Register();
	G->BindToPropVector( Env->FindObject( ObjID ), PropID );

	return G;
}

clTranslateGizmo* clVisualScene::AddTranslateGizmoVarMtx( const LString& VarName )
{
	clTranslateGizmo* G = Env->Linker->Instantiate( "clTranslateGizmo" );
	AddModifier( G );
	G->Register();
	G->BindToVarMatrix( VarName );

	return G;
}

clTranslateGizmo* clVisualScene::AddTranslateGizmoVarVec( const LString& VarName )
{
	clTranslateGizmo* G = Env->Linker->Instantiate( "clTranslateGizmo" );
	AddModifier( G );
	G->Register();
	G->BindToVarVector( VarName );

	return G;
}

clRotateGizmo* clVisualScene::AddRotateGizmoPropMtx( const LString& ObjID, const LString& PropID )
{
	clRotateGizmo* G = Env->Linker->Instantiate( "clRotateGizmo" );
	AddModifier( G );
	G->Register();
	G->BindToPropMatrix( Env->FindObject( ObjID ), PropID );

	return G;
}

clRotateGizmo* clVisualScene::AddRotateGizmoPropVec( const LString& ObjID, const LString& PropID )
{
	clRotateGizmo* G = Env->Linker->Instantiate( "clRotateGizmo" );
	AddModifier( G );
	G->Register();
	G->BindToPropVector( Env->FindObject( ObjID ), PropID );

	return G;
}

clRotateGizmo* clVisualScene::AddRotateGizmoVarMtx( const LString& VarName )
{
	clRotateGizmo* G = Env->Linker->Instantiate( "clRotateGizmo" );
	AddModifier( G );
	G->Register();
	G->BindToVarMatrix( VarName );

	return G;
}

clRotateGizmo* clVisualScene::AddRotateGizmoVarVec( const LString& VarName )
{
	clRotateGizmo* G = Env->Linker->Instantiate( "clRotateGizmo" );
	AddModifier( G );
	G->Register();
	G->BindToVarVector( VarName );

	return G;
}

clRotateGizmo* clVisualScene::AddRotateGizmoPropQuat( const LString& ObjID, const LString& PropID )
{
	clRotateGizmo* G = Env->Linker->Instantiate( "clRotateGizmo" );
	AddModifier( G );
	G->Register();
	G->BindToPropQuat( Env->FindObject( ObjID ), PropID );

	return G;
}

clRotateGizmo* clVisualScene::AddRotateGizmoVarQuat( const LString& VarName )
{
	clRotateGizmo* G = Env->Linker->Instantiate( "clRotateGizmo" );
	AddModifier( G );
	G->Register();
	G->BindToVarQuat( VarName );

	return G;
}

/*
 * 12/06/2010
     Log section added
*/
