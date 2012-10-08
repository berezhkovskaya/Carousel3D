/**
 * \file VisualScene.h
 * \brief Visual scene
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __VisualScene__h__included__
#define __VisualScene__h__included__

#include "Core/iObject.h"

#include "Math/LMatrix.h"

class clVisualObject;

class clVisualCamera;
class clVisualModifier;
class clVisualLight;
class clVisualSpotLight;
class clVisualPointLight;
class clVisualDirectionalLight;
class clVisualRenderable;
class clVisualGeom;
class clVisualVolume;
class clVisualParticles;

class clScene;
class clMaterial;
class clGeom;
class clRenderingTechnique;

class clTranslateGizmo;
class clRotateGizmo;

class iCameraPositioner;

/// Basic editable scene
class netexportable scriptfinal clVisualScene : public iObject
{
public:
	clVisualScene();
	virtual ~clVisualScene();

	virtual void AfterConstruction();

	FWD_EVENT_HANDLER( Event_TIMER );
	FWD_EVENT_HANDLER( Event_KEY );

	/// Register Timer/Click/Key events for this scene
	scriptmethod void Register();

	/// Unregister Timer/Click/Key handlers
	scriptmethod void Unregister();

	virtual bool BeginLoad();
	virtual bool EndLoad();

	/// Clear the scene by removing every entity
	virtual void Clear();

	scriptmethod LString GetLastError() const { return FLastError; }

	/// And kind of debugging information
	scriptmethod void RenderDebug( const LMatrix4& Proj, const LMatrix4& View );

	/// And kind of offscreen helpers
	scriptmethod void RenderAuxBuffers( const LMatrix4& Proj, const LMatrix4& View );

	/**
	   The rendering (by now) consists of some stages:
	     1) Render the volumes to separate buffers
	     2) Render the "RenderScene" to its internal buffer
	     3) Render the "DebugScene" to its internal buffer
	     4) Compose each render target into the final buffer
	     5) Render final buffer as a fullscreen rect with no depthtest and no z-writes
	     6) Render the debug information, usually done with Canvas/TextOut
	*/
	scriptmethod void Render();

	/// Pick object using current mouse coordinates
	scriptmethod clVisualObject* PickObject( float x, float y );

	scriptmethod clVisualObject* FindObject( const LString& _ID );

	/// Select active camera
	scriptmethod void SetActiveCamera( int Idx ) { FActiveCamera = Idx; }

	/// Get active camera index
	scriptmethod int  GetActiveCamera() const { return FActiveCamera; }

	/// Get current viewing transfomation or identity if no camera is selected
	scriptmethod LMatrix4 GetCurrentView();

	/// Get current projection or identity if no camera is selected
	scriptmethod LMatrix4 GetCurrentProjection();

#pragma region Access to scene elements
	/* Property(Name=Cameras,   Type=clVisualCamera,     IndexType=int, FieldName=FCameras,   NetIndexedGetter=GetCamera,     NetIndexedSetter=SetCamera)     */
	/* Property(Name=Lights,    Type=clVisualLight,      IndexType=int, FieldName=FLights,    NetIndexedGetter=GetLight,      NetIndexedSetter=SetLight)      */
	/* Property(Name=Visuals,   Type=clVisualRenderable, IndexType=int, FieldName=FVisuals,   NetIndexedGetter=GetRenderable, NetIndexedSetter=SetRenderable) */
	/* Property(Name=Volumes,   Type=clVisualVolume,     IndexType=int, FieldName=FVolumes,   NetIndexedGetter=GetVolume,     NetIndexedSetter=SetVolume)     */
	/* Property(Name=Modifiers, Type=clVisualModifier,   IndexType=int, FieldName=FModifiers, NetIndexedGetter=GetModifier,   NetIndexedSetter=SetModifier)   */
	/* Property(Name=Particles, Type=clVisualParticles,  IndexType=int, FieldName=FParticles, NetIndexedGetter=GetParticles,  NetIndexedSetter=SetParticles)  */

	scriptmethod int               GetNumVolumes()      const { return static_cast<int>( FVolumes.size() );   }
	scriptmethod int               GetNumParticles()    const { return static_cast<int>( FParticles.size() ); }
	scriptmethod int               GetNumCameras()      const { return static_cast<int>( FCameras.size() );   }
	scriptmethod int               GetNumLights()       const { return static_cast<int>( FLights.size() );    }
	scriptmethod int               GetNumRenderables()  const { return static_cast<int>( FVisuals.size() );   }
	scriptmethod int               GetNumModifiers()    const { return static_cast<int>( FModifiers.size() ); }

	scriptmethod clVisualVolume*     GetVolume( int i )     const { return FVolumes[i];   }
	scriptmethod clVisualParticles*  GetParticles( int i )  const { return FParticles[i]; }
	scriptmethod clVisualRenderable* GetRenderable( int i ) const { return FVisuals[i];   }
	scriptmethod clVisualLight*      GetLight( int i )      const { return FLights[i];    }
	scriptmethod clVisualCamera*     GetCamera( int i )     const { return FCameras[i];   }
	scriptmethod clVisualModifier*   GetModifier( int i )   const { return FModifiers[i]; }

	scriptmethod void              SetVolume( int i, clVisualVolume* Vol )         {}
	scriptmethod void              SetParticles( int i, clVisualParticles* Part )  {}
	scriptmethod void              SetRenderable( int i, clVisualRenderable* Rnd ) {}
	scriptmethod void              SetCamera( int i, clVisualCamera* Cam )         {}
	scriptmethod void              SetLight( int i, clVisualLight* L )             {}
	scriptmethod void              SetModifier( int i, clVisualModifier* Mod )     {}

	scriptmethod int               GetObjectIndex( clVisualObject* Obj ) const;
	scriptmethod int               GetCameraIndex( clVisualCamera* Cam ) const;
	scriptmethod int               GetLightIndex( clVisualLight* L ) const;
	scriptmethod int               GetRenderableIndex( clVisualRenderable* R ) const;
	scriptmethod int               GetModifierIndex( clVisualModifier* M ) const;

#pragma endregion

#pragma region Utility constructors and item managers

	scriptmethod clVisualSpotLight*        AddSpotLight();
	scriptmethod clVisualPointLight*       AddPointLight();
	scriptmethod clVisualDirectionalLight* AddDirectionalLight();

	/// Add a new camera with optional positioner
	scriptmethod clVisualCamera*           AddCamera( const LMatrix4& Projection, const LMatrix4& Transform, iCameraPositioner* Pos );

	/// Add a new geom with overridden material (can be null) and initial LocalTransform
	scriptmethod clVisualGeom*             AddGeom( clGeom* G, clMaterial* Mat, const LMatrix4& Transform );

//	scr_iptmethod VisualVolume*           AddVolume();
	scriptmethod clVisualParticles*        AddParticles();

	scriptmethod void AddModifier( clVisualModifier* Mod );

	scriptmethod void AddLight( clVisualLight* L );
	scriptmethod void RemoveLight( clVisualLight* L );

	/// If the camera coincides with the ActiveCamera, then ActiveCamera is set to -1
	scriptmethod void RemoveCamera( clVisualCamera* Cam );

	scriptmethod void RemoveRenderable( clVisualRenderable* R );
	scriptmethod void RemoveGeom( clVisualGeom* G );
	scriptmethod void RemoveVolume( clVisualVolume* V );
	scriptmethod void RemoveParticles( clVisualParticles* PP );
	scriptmethod void RemoveModifier( clVisualModifier* Mod );

#pragma region Visual modifiers. Clicking/Updates are managed by the VisualScene

	/// Link translation gizmo to some mtx property
	scriptmethod clTranslateGizmo*        AddTranslateGizmoPropMtx( const LString& ObjID, const LString& PropID );
	/// Link translation gizmo to some mtx property
	scriptmethod clTranslateGizmo*        AddTranslateGizmoPropVec( const LString& ObjID, const LString& PropID );
	scriptmethod clTranslateGizmo*        AddTranslateGizmoVarMtx( const LString& VarName );
	scriptmethod clTranslateGizmo*        AddTranslateGizmoVarVec( const LString& VarName );

	/// Link rotation gizmo to some mtx property
	scriptmethod clRotateGizmo*           AddRotateGizmoPropMtx( const LString& ObjID, const LString& PropID );
	scriptmethod clRotateGizmo*           AddRotateGizmoPropVec( const LString& ObjID, const LString& PropID );
	scriptmethod clRotateGizmo*           AddRotateGizmoPropQuat( const LString& ObjID, const LString& PropID );
	scriptmethod clRotateGizmo*           AddRotateGizmoVarMtx( const LString& VarName );
	scriptmethod clRotateGizmo*           AddRotateGizmoVarVec( const LString& VarName );
	scriptmethod clRotateGizmo*           AddRotateGizmoVarQuat( const LString& VarName );

#pragma endregion

#pragma endregion

	/// Workaround for non-working subobjects (from iObject)
	scriptmethod void          AddSubObject( clVisualObject* SubObject );
	scriptmethod void          RemoveSubObject( clVisualObject* SubObject );
	scriptmethod void          ClearSubObjects() { FObjects.clear(); }
	scriptmethod int           GetTotalSubObjects() const { return static_cast<int>( FObjects.size() ); }
	scriptmethod clVisualObject* GetObject( int i ) { return FObjects[i]; }
	scriptmethod clVisualObject* GetObjectConst( int i ) const { return FObjects[i]; }

	scriptmethod clScene*      GetDebugScene() const { return FDebugScene; }
	scriptmethod clScene*      GetScene() const { return FScene; }

	virtual void Update( float DT );

	virtual void HandleKey( int Key, bool State );

	/* Property(Name=Technique, Type=clRenderingTechnique, Getter=GetTechnique) */

	scriptmethod clRenderingTechnique* GetTechnique() const { return FTechnique; }
private:
	/// Local timer/key event handlers
	LEventSubscriber::LEventHandler FTimerHandler, FKeyHandler;

	void AddToScenes();
	void RemoveFromScenes();

	/// Debug scene
	clScene* FDebugScene;

	/// Real scene
	clScene* FScene;

	clRenderingTechnique* FTechnique;

	/// List of visual objects in this scene (resides in FSubObjects by now)
	std::vector<clVisualObject*> FObjects;

	/// Last error message
	LString FLastError;

	/// Currenttly used camera
	int FActiveCamera;
public:
	/// Auxillary cameras list
	std::vector<clVisualParticles*>  FParticles;
	std::vector<clVisualVolume*>     FVolumes;
	std::vector<clVisualCamera*>     FCameras;
	std::vector<clVisualLight*>      FLights;
	std::vector<clVisualRenderable*> FVisuals;
	std::vector<clVisualModifier*>   FModifiers;
};

#endif


/*
 * 12/06/2010
     Log section added
*/
