/**
 * \file Scene.h
 * \brief Scene
 * \version 0.5.99c
 * \date 15/04/2011
 * \author Sergey Kosarevsky, 2010-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clScene_
#define _clScene_

#include "Core/iObject.h"
#include "Math/LMatrix.h"
#include "Math/LAABB.h"
#include "Math/LPlane.h"
#include "Math/LFrustum.h"
#include "Math/LKeyframer.h"
#include "Geometry/VertexAttribs.h"
#include "Renderer/iRenderContext.h"
#include "Material.h"
#include "Utils/LArray.h"

class clCamera;
class clGeom;
class clLight;
class clMaterial;
class iTexture;
class iVertexArray;
class iShaderProgram;
class clRenderState;
class iRenderTarget;
class clCVar;
class iUpdater;

struct sMaterialDesc;
class clRigidInstance;

enum LLightType
{
	// with falloff and spot direction
	LLIGHT_SPOT = 0,
	// aka point
	LLIGHT_OMNI = 1,
	// directional
	LLIGHT_DIR  = 2
//	LLIGHT_VOLUME
};

enum LAnaglyphStereoType
{
	STEREO_NONE      = 0,
	STEREO_GRAY      = 1,
	STEREO_TRUE      = 2,
	STEREO_HALFCOLOR = 3,
	STEREO_FULLCOLOR = 4,
	STEREO_OPTIMIZED = 5,
	STEREO_DUBOIS    = 6,
	STEREO_SIDE_BY_SIDE = 7
};

/// a primitive render operation used to sort render command
struct clRenderOperation
{
	Luint64            FSortKey;
	int                FRigid;
	size_t             FMask;
	iTexture*          FPackedSkeleton;
	iTexture*          FPlanarReflection;
	iVertexArray*      FVertexArray;
	bool               FSkinned;
};

class clPlanarReflection
{
public:
	clPlanarReflection( const LVector3& ReflPlane,
	                    sMaterialDesc* Mat ): FReflPlane( ReflPlane ),
		FMaterialRef( Mat ) {};
	LVector3          FReflPlane;
	sMaterialDesc*    FMaterialRef;
};

class clReflectionBuffer
{
public:
	clReflectionBuffer( const LPlane& ReflPlane, iRenderTarget* RT ): FReflPlane( ReflPlane ), FRT( RT ), FRendered( false ) {};

	LPlane         FReflPlane;
	iRenderTarget* FRT;
	bool           FRendered;
};

struct sRigidID
{
	sRigidID( int GeomIdx,
	          int RigidIdx ): FGeomIdx( GeomIdx ),
		FRigidIdx( RigidIdx ) {};
	sRigidID(): FGeomIdx( -1 ),
		FRigidIdx( -1 ) {};

	/// Index of geom this rigid came from
	int    FGeomIdx;
	/// Index of rigid within the geom
	int    FRigidIdx;

	inline bool operator == ( const sRigidID& Other ) const
	{
		return ( FGeomIdx == Other.FGeomIdx ) && ( FRigidIdx == Other.FRigidIdx );
	}
};

class clGeomInstance
{
public:
	clGeomInstance(): FGeomInstance( -1 ),
		FStartsOfRigidsBlock( 0 ),
		FGeomNumRigids( 0 ),
		FHasPrivateMaterial( false ),
		FParent( -1 ) {};
	clGeomInstance( int Idx, size_t Start, clGeom* Geom );
public:
	/// handle of geom in geoms collection
	int       FGeomInstance;
	size_t    FStartsOfRigidsBlock;
	size_t    FGeomNumRigids;
	bool      FHasPrivateMaterial;
	//
	int       FParent;
};

class clRigidInstance
{
public:
	clRigidInstance(): FRigid( sRigidID( -1, -1 ) ),
		FParentRef( -1 ),
		FMaterialRef( -1 ),
		FLocalTransform( LMatrix4::Identity() ),
		FGlobalTransform( LMatrix4::Identity() ),
		FKeyframer(),
		//FKeyframes( LVector4( 0 ) ),
		//FAnimations( LVector4( 0 ) ),
		FOwner( NULL ),
		FVisible( true ),
		FReceiveShadow( true ),
		FCastShadow( true ) {};
	clRigidInstance( sRigidID ID,
	                 int ParentRef,
	                 int MaterialRef ): FRigid( ID ),
		FParentRef( ParentRef ),
		FMaterialRef( MaterialRef ),
		FLocalTransform( LMatrix4::Identity() ),
		FGlobalTransform( LMatrix4::Identity() ),
		FKeyframer(),
		//FKeyframes( LVector4( 0 ) ),
		//FAnimations( LVector4( 0 ) ),
		FOwner( NULL ),
		FVisible( true ),
		FReceiveShadow( true ),
		FCastShadow( true ),
		FTag( -1 )
	{
	};
public:
	/// unpacked and unsorted rigids from all geoms
	sRigidID    FRigid;
	/// parent index for every rigid or -1 if none
	int         FParentRef;
	/// material index for every rigid, no multimaterials are possible here
	int         FMaterialRef;
	/// local transformation for this rigid relative to its parent
	LMatrix4    FLocalTransform;
	/// precached global transformation
	LMatrix4    FGlobalTransform;
	/// used internally for rendering ( Keyframe, NextKeyrame, LerpCoef, unused, FirstFrame, LastFrame, Loopframes, FPS )
	LKeyframer  FKeyframer;
	/// used for framebuffer picking
	iObject*    FOwner;
	/// used to temporarly hide object
	bool        FVisible;
	/// used to toggle shadow receiving (False will override any values in materials and geoms)
	bool        FReceiveShadow;
	/// user to toggle shadow casting (False will override any values in materials and geoms)
	bool        FCastShadow;
	/// user-defined tag (used in callbacks)
	int         FTag;
};

// contains potentially visible objects
class scriptfinal netexportable clScene: public iObject
{
public:
	typedef LArray<clRenderState*> clStatesList;
	typedef void ( *LPreAddBufferCallback )( clRenderState* Shader, clRigidInstance* RigidInstance );
public:
	clScene();
	virtual ~clScene();

	//
	// iObject interface
	//
	virtual void    AfterConstruction();

#pragma region Projection and modelview
	scriptmethod void       SetCameraProjection( const LMatrix4& Proj ) { FMatrices.SetProjectionMatrix( Proj ); };
	scriptmethod LMatrix4   GetCameraProjection() const { return FMatrices.GetProjectionMatrix(); };
	scriptmethod void       SetCameraTransform( const LMatrix4& ModelView ) { FMatrices.SetViewMatrix( ModelView ); };
	scriptmethod LMatrix4   GetCameraTransform() const { return FMatrices.GetViewMatrix(); };
	scriptmethod sMatrices* GetMatricesPtr() { return &FMatrices; };
#pragma endregion

#pragma region Add and remove first-class items from the scene

	/// Allocate space in each array of the light
	scriptmethod int AddNewLight( int LightType, bool Enabled, const LVector3& Pos, const LVector4& Color, const LVector3& Attenuation );

	/// Remove specific light
	scriptmethod void RemoveLight( int idx );

	scriptmethod int AddSpotLight( const LVector3& Pos, const LVector3& SpotDir, const LVector4& Color, const LVector3& Attenuation, const LVector3& SpotParams );

	scriptmethod int AddPointLight( const LVector3& Pos, const LVector4& Color, const LVector3& Attenuation, const LVector3& PointParams );

	scriptmethod int AddDirLight ( const LVector3& Dir, const LVector4& Color, const LVector3& Attenuation );

	/// Calculate the total number of lights
	scriptmethod size_t GetNumLights() const { return FLightEnabled.size(); }

	/// Remove all lights
	scriptmethod void ClearLights();

	/// Add new renderable item (possibly with attached materials)
	scriptmethod int     AddGeom( clGeom* Geom );

	/// Add new renderable item, all materials are assigned to MtlIdx
	scriptmethod int     AddGeomMtl( clGeom* Geom, int MtlIdx );

#pragma region Hierarchical attachments

	/// add and link to parent
	scriptmethod int     AddGeomToParent( clGeom* Geom, int Parent );

	/// attach child to parent of any kind
	scriptmethod void    Attach( int Child, int Parent );

	/// detach child from parent
	scriptmethod void    Detach( int Child, int Parent );

	/// Attach some Geom to the specific bone at a given offset (ion Bone's reference frame)
	scriptmethod void    AttachToBone( int ChildID, int ParentGeomID,  int BoneID, const vec3& LocalOffset );

	/// Detach some Geom from previously specified parent/bone
	scriptmethod void    DetachFromBone( int ChildID, int ParentGeomID );

	/// Remove some visual item from scene. Lights are managed separately
	scriptmethod void    RemoveItem( int Idx );

	/// Set item visibility. Scene is resorted only on visibility change
	scriptmethod void    SetVisible( int Idx, bool Visible );

	/// Set if the item will receive shadows
	scriptmethod void    SetReceiveShadow( int Idx, bool ReceiveShadow );

	/// Set if the item will receive shadows
	scriptmethod void    SetCastShadow( int Idx, bool CastShadow );

#pragma endregion

#pragma region Data access and iteration
	/// get the number of geoms instances (i.e. every AddGeom() generates an instances an returns it's ID)
	scriptmethod int     GetGeomsCount() const { return static_cast<int>( FGeomInstances.size() ); }
	scriptmethod clGeom* GetGeom( int Idx ) const;

	/// get the number of unique geoms in the scene (different geoms occur only once in this list)
	scriptmethod size_t  GetUniqueGeomsCount() const { return FGeomsCollection.size(); }
	scriptmethod clGeom* GetUniqueGeom( size_t Idx ) const { return FGeomsCollection[Idx]; }
#pragma endregion

#pragma region Transformations
	scriptmethod void     SetOwner( int Idx, iObject* Owner );
	scriptmethod void     SetLocalTransform( int Idx, const LMatrix4& Transform );
	scriptmethod const LMatrix4& GetLocalTransform( int Idx ) const;
#pragma endregion

#pragma region Scaling control (only with coefs, does not modify any geometry)
	/// get LABB for zero's keyframe
	scriptmethod LAABoundingBox GetBoundingBox( int Idx ) const;
	/// get interpolated LABB for current keyframe
	scriptmethod LAABoundingBox GetBoundingBoxInterpolated( int Idx ) const;
	/// CenterMesh is used to force centering mesh in the center of its AABB
	scriptmethod void           Resize( int Idx, float DimZ, bool CenterMesh );
	scriptmethod void           Scale( int Idx, float Coef, bool CenterMesh );
#pragma endregion

#pragma region Materials
	/// Manually add the material
	scriptmethod int            AddMtl( clMaterial* Mtl );
	/// apply material MaterialIdx to all rigids of the geom instance Idx
	scriptmethod void           SetMtlIndex( int Idx, int MaterialIdx );
	/// apply material Material to all rigids of the geom instance Idx
	scriptmethod void           SetMtl( int Idx, clMaterial* Material );
	/// create material from shaders and apply it to all rigids of the geom instance Idx, the last 2 shaders can be NULL - in that case they will be replaces with ShaderNormal (scene will not deallocate your shaders)
	scriptmethod void           SetMtlFromShader( int Idx, clRenderState* ShaderNormal, clRenderState* ShaderShadow, clRenderState* ShaderReflection );
	/// get the access to the internal material, the changes will affect all geoms that use this material
	virtual sMaterialDesc&      GetMaterialDesc( int MatID ) { return FMaterials[MatID]; };
	/// this will create a new material for geom by duplicating the material from the first rigid and implement copy-on-write policy
	virtual sMaterialDesc&      GetGeomMaterialDesc( int Idx );
	/// get the shader from the rendering Pass for the specific MatID - this is quite low-level, use with caution
	scriptmethod clRenderState* GetShader( int MatID, LMatSysPass Pass );
	/// set the shader of the rendering Pass for the specific MatID - this is quite low-level, use with caution (scene will not deallocate your shaders)
	scriptmethod void           SetShader( int MatID, LMatSysPass Pass, clRenderState* Shader );
#pragma endregion

#pragma region Animation
	// Keyframe, NextKeyrame, LerpCoef, CurrentNumberOfIterations
	scriptmethod LVector4 GetKeyframe( int Idx );
	scriptmethod void     SetKeyframe( int Idx, int KeyframeNum, int NextKeyframeNum, float LerpCoef );
	scriptmethod void     SetAnimation( int Idx, const LString& AnimName );
	scriptmethod void     SetAnimationSpeed( int Idx, float FPS );
	scriptmethod void     BlendAnimation( int Idx, const LString& AnimName );
	scriptmethod bool     UpdateAnimation( int Idx, float Time );
#pragma endregion

#pragma region Updaters
	/// Attach new updater to the geom handle Idx
	scriptmethod void     AddUpdater( int Idx, iUpdater* Updater );
	/// Deallocates specified updater. If 'Updater' is NULL all updaters are removed
	scriptmethod void     RemoveUpdater( int Idx, iUpdater* Updater );
#pragma endregion

	/// Override internal buffer allocation and set the render buffer manually
	scriptmethod void    SetRenderBuffer( iRenderTarget* Tgt ) { FUseOffscreenBuffer = ( Tgt != NULL ); FManualOffscreenBuffer = ( Tgt != NULL ); FRenderBuffer = Tgt; }

	/// first call with UseOffscreen=true will allocate offscreen buffers (with CreatePickingBuffer flag)
	scriptmethod void    SetUseOffscreenBuffer( bool UseOffscreen, bool CreatePickingBuffer );
	scriptmethod bool    GetUseOffscreenBuffer() const;

	scriptmethod void    SetClearColorMode( bool ToClear ) { FClearColor = ToClear; }
	scriptmethod void    SetClearDepthMode( bool ToClear ) { FClearDepth = ToClear; }
	scriptmethod void    SetClearStencilMode( bool ToClear ) { FClearStencil = ToClear; }

	scriptmethod bool    GetClearColorMode() const { return FClearColor; }
	scriptmethod bool    GetClearDepthMode() const { return FClearDepth; }
	scriptmethod bool    GetClearStencilMode() const { return FClearStencil; }

	/// Depth-based shadowing requires recreation of the shadow maps, so this must be called before complete scene reinitialization
	scriptmethod void    SetDepthBasedShadows( bool DepthShadows ) { FDepthBasedShadows = DepthShadows; }

	/// Get depth-based shadowmapping flag
	scriptmethod bool    GetDepthBasedShadows() const { return FDepthBasedShadows; }

	scriptmethod void    SetFrustumCulling( bool Enabled );
	scriptmethod void    SetAnaglyphStereoRendering( LAnaglyphStereoType StereoType );
	scriptmethod void    SetFocalLength( float FocalLength ) { FFocalLength = FocalLength; };
	scriptmethod float   GetFocalLength() const { return FFocalLength; };
	scriptmethod void    SetInterocularDistance( float IOD ) { FIOD = IOD; };
	scriptmethod float   GetInterocularDistance() const { return FIOD; };
	scriptmethod void    RenderForward();
	/// primitive render pass (this is quite low-level)
	void    RenderSortedScene( sAttribStream<clRenderOperation>* SortedScene,
	                           const LVector4* ClipPlane,
	                           clStatesList* Shaders,
	                           LMatSysPass Pass,
	                           bool ShowNormals );
	scriptmethod iRenderTarget*    GetRenderTarget() const { return FRenderBuffer;};
	scriptmethod iObject*          PickObject( const LVector2& Pnt );
	scriptmethod clVertexAttribs*  PickVA( const LVector2& Pnt );

#pragma region Callbacks
	scriptmethod void    SetPreAddBufferCallback( LPreAddBufferCallback PreAddBufferCallback );
#pragma endregion

	// events
	FWD_EVENT_HANDLER( Event_TIMER );

#pragma region Lighting parameters

	/// Single shadow map for each light. I know, must be different for CSMs
	LArray<iRenderTarget*> FLightShadowMaps;

	/// LightTransform * LightProjection for each light
	LArray<LMatrix4>  FLightShadowMatrices;

	LArray<LMatrix4>  FLightProjections;

	LArray<float>     FLightEnabled;
	LArray<float>     FLightTypes;
	LArray<LMatrix4>  FLightTransforms;

	LArray<LVector3>  FLightAttenuations;
	LArray<LVector3>  FLightSpotParams;
	LArray<LVector3>  FLightSpotDirections;

	LArray<LVector4>  FLightAmbientColors;
	LArray<LVector4>  FLightSpecularColors;
	LArray<LVector4>  FLightDiffuseColors;

#pragma endregion

private:
	int     AddGeomIntoSlot( clGeom* Geom, int Slot, int MtlIdx );
	void    RenderForward_Internal( iRenderTarget* ToBuffer );
	int     FindGeomInstance( clGeom* Geom ) const;
	std::vector<int> FindAllGeomInstances( clGeom* Geom ) const;
	int     AddMaterial( const sMaterialDesc* Material );
	int     PushNewMaterial( const sMaterialDesc* Material );
	void    RecalculateGlobalTransforms();
	void    SetScaleTransform( int Idx, const LMatrix4& Transform );
	void    SortScene();
	void    UpdateLights();
	void    UpdateReflections( sAttribStream<clRenderOperation>* RenderOps );
	void    UpdateShadows( sAttribStream<clRenderOperation>* RenderOps );
	void    UpdateShaders( const LVector4* ClipPlane, clStatesList* Shaders, bool ShowNormals );
	int     AddReflectionPlane( const LPlane& Plane );
	int     PickPair( const LVector2& Pnt );
	int     PushRigid( int RigidIdx, int ParentRef, int MtlRef, int GeomHandle );
	void    PushRigidInstances( clGeom* Geom, int ThisGeomHandle, int MtlIdx );
	clVertexAttribs* GetRigid( const clRigidInstance& Rigid ) const;
	void    RebuildSceneRigids();
	void    RebuildRenderOperations();
	void    AddRenderOperation( const clRigidInstance* Rigid, int RigidIdx );
	int     FindUpdater( int Idx, iUpdater* Updater ) const;
	int     FindUpdaterByClass( int Idx, const LString& UpdaterClassName ) const;
	size_t  GetDefaultMask() const;
	void    RenderOneReflectionMap( const LMatrix4& NewViewMatrix, sAttribStream<clRenderOperation>* RenderOps, iRenderTarget* RT, int ThisMaterialIdx, const LVector4* ClipPlane );
#pragma region Internal debugging and profiling
	void    DebugRenderBoxes( sAttribStream<clRenderOperation>* SortedScene ) const;
	void    DebugRenderLights() const;
	void    DebugRenderReflections() const;
	void    DebugRenderShadowMaps() const;
	void    DebugLogSortedScene( sAttribStream<clRenderOperation>* SortedScene, clStatesList* Shaders ) const;
#pragma endregion

private:
	/// event from geoms
	FWD_EVENT_HANDLER( Event_CHANGED );

private:
	/// collection of geoms bound to this scene (unique entries, no duplicates)
	LArray<clGeom*>    FGeomsCollection;

	/// lightweight pointer to a geom in GeomsCollection indexed by a Geom handle
	sAttribStream<clGeomInstance>   FGeomInstances;

#pragma region Properties indexed by a Rigid ID
	sAttribStream<clRigidInstance>    FRigids;
	/// set if any of local transforms was changed
	bool                              FGlobalTransformsDirtyFlag;
#pragma endregion

#pragma region Properties indexed by a Material ID
	/// default material
	sMaterialDesc                  FDefaultMaterial;
	/// collection of scene materials
	sAttribStream<sMaterialDesc>   FMaterials;
	sAttribStream<size_t>          FMaterialMasked;
	/// used internally for rendering, one shader per material for now
	clStatesList     FShadersNormal;
	clStatesList     FShadersShadow;
	clStatesList     FShadersReflection;
#pragma endregion

	/// updaters are responsible for updating of scene objects (i.e. setup of animation)
	LArray<iUpdater*>         FUpdatersList;

	/// global view, projection matrices and different shortcuts (inverses, MVP, etc.)
	sMatrices         FMatrices;

	/// reflections
	LArray<clReflectionBuffer>    FReflectionBuffers;

	/// offscreen framebuffer
	iRenderTarget*    FRenderBuffer;
	bool              FUseOffscreenBuffer;

	/// Shadows mode (some tile-based devices do not allow depth FBO)
	bool FDepthBasedShadows;

	/// Clear mode : call ClearRenderTarget(true, true, true) for each component of the buffer
	bool              FClearColor;
	bool              FClearDepth;
	bool              FClearStencil;

	/// If the SetRenderBuffer was called this is true
	bool              FManualOffscreenBuffer;

	/// anaglyphic stereo mode
	float               FIOD;
	float               FFocalLength;
	LAnaglyphStereoType FStereoType;
	iRenderTarget*      FLeftBuffer;
	iRenderTarget*      FRightBuffer;
	clRenderState*      FAnaglyphMixer;

	/// enable frustum culling
	bool              FUseFrustumCulling;

	/// sorted scene with all empty items deleted
	sAttribStream<clRenderOperation> FRenderOperations;
	bool                             FSortedSceneDirty;
	bool                             FSceneNeedsRebuild;

	/// statistics
	clCVar*           FRenderingTime_T;
	clCVar*           FPickingTime_T;
	clCVar*           FDisableRendering;
	clCVar*           FRenderBoxes;
	clCVar*           FRenderLights;
	clCVar*           FRenderReflections;
	clCVar*           FRenderShadowMaps;
	clCVar*           FEnableShadowPass;
	clCVar*           FEnableReflectionPass;
	clCVar*           FShowNormals;

	/**
	   Deferred shading is optional at the time,
	   so it must be enabled separately.
	*/

#pragma region Callbacks
	LPreAddBufferCallback FPreAddBufferCallback;

	static void EmptyPreAddBufferCallback( clRenderState* Shader, clRigidInstance* RigidInstance ) {};
#pragma endregion

#pragma region Deferred rendering buffers and methods
public:
	/**
	   \brief Target 1 basic properties
	   --- buffer1
	   /// Ambient (RGB, 24bit)
	   --- buffer2
	   /// Albedo a.k.a. Diffuse – RGB ( vec3, 24bit)
	   --- buffer3
	   /// Normal (any space – world, eye)  - vec3, packed to 8:8:8
	   --- buffer4
	   /// Specular Power (scalar, 8 bit) – log_2(SpecPower) / 10.5
	   /// Specular Intensity (scalar, 8 bit)
	   /// 2D motion vector  (vec2, 16 bit)
	   --- Depth
	   /// Depth buffer (platform-dependant, 16-24-32) - depth
	*/
	iRenderTarget* FDeferredBuffer;

	/// Final light shader
	clRenderState* FDeferred_Shader;

	clStatesList FShadersNormal_Deferred;

	/// Allocate all render targets for deferred rendering
	scriptmethod void AllocateDeferredBuffers();

	/// Free buffers
	scriptmethod void DeallocateDeferredBuffers();

	/// Render single frame using deferred technique
	scriptmethod void RenderDeferred();

	/**
	   \brief Render (with possible recursion due to reflections/refractions)

	   If 'Reflect' is false, but the ClipPlane is not null, the refaction buffer is rendered
	*/
	void RenderDeferred_Internal( iRenderTarget* ToBuffer );

#pragma endregion
};

#endif

/*
 * 15/04/2011
     SetMtlFromShader() now accepts 3 shaders for every pass - one for each pass
     Added SetShader()
 * 29/09/2010
     clRigidInstance::FCastShadow
     clRigidInstance::FReceiveShadow
 * 25/09/2010
     clLight interface extended
 * 23/09/2010
     Shadow maps moved to clLight
 * 11/09/2010
     FShowNormals
 * 04/07/2010
     AddUpdater()
     RemoveUpdater()
 * 26/06/2010
     Rewritten
 * 17/06/2010
     GetCameraTransform()
     GetCameraProjection()
 * 23/04/2010
     UpdateShadows()
 * 31/03/2010
     Removed redundant streams
 * 30/03/2010
     Geoms collection
 * 25/03/2010
     First concept of clSceneObject
 * 18/03/2010
     PushRigid()
 * 04/03/2010
     It is here
*/
