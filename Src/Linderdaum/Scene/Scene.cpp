/**
 * \file Scene.cpp
 * \brief Scene
 * \version 0.6.00 ($Revision: 3291 $)
 * \date 15/04/2011
 * \author Sergey Kosarevsky, 2010-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "Scene.h"
#include "iUpdater.h"

#include "Core/CVars.h"
#include "Core/Logger.h"
#include "Core/Linker.h"
#include "Core/Console.h"
#include "Geometry/Geom.h"
#include "Geometry/Mesh.h"
#include "Material.h"
#include "Renderer/Canvas.h"
#include "Renderer/iTexture.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iShaderProgram.h"
#include "Renderer/iVertexArray.h"
#include "Resources/ResourcesManager.h"
#include "Utils/Viewport.h"

#include "Math/LSort.h"
#include "Math/LPlane.h"
#include "Math/LProjection.h"

#include "LColors.h"

#include <algorithm>

static const Luint HASHING_VALUE = 557;

#pragma pack(push, 1)
union uColorHasher
{
public:
	inline LVector4 IntToColor( Luint Num )
	{
		FNum = Num * HASHING_VALUE;

		LVector4 Color( static_cast<float>( R ), static_cast<float>( G ), static_cast<float>( B ), 0.0f );

		return Color / 255.0f;
	}
	inline Luint InvHashColor( Luint Num )
	{
		FNum = Num;

		return FNum / HASHING_VALUE;
	}
private:
	struct GCC_PACK( 1 )
	{
		Lubyte R, G, B, Pad;
	};
	Luint FNum;
};
#pragma pack(pop)

inline Luint64 Makekey( clRenderState* Shader, float Z )
{
	//   --------------------------------------------------------------------
	//   |  1-bit blending  |  16-bit SP  |  16-bit tex  |  31-bit Z-value  |
	//   --------------------------------------------------------------------
#pragma pack(push, 1)
	union uKey
	{
		struct
		{
			unsigned int         Z: 31;
			unsigned short     Tex: 16;
			unsigned short      SP: 16;
			bool           Blended: 1;
		};
		Luint64 Key;
	} KeyPacker;
#pragma pack(pop)

	KeyPacker.Key = 0;
	/*
	   Lushort SPHandle  = Shader->GetShaderProgram()->GetResourceID() & 0xFFFF;
	   Luint   TexHandle = ( reinterpret_cast<size_t>( Shader->GetTextureForTextureUnit( 0 ) ) >> 2 ) & 0xFFFF;

	   KeyPacker.SP  = SPHandle;
	   KeyPacker.Tex = TexHandle;

	   // reset sign bit
	   KeyPacker.Z   = *reinterpret_cast<unsigned int*>( &Z ) & 0x7FFFFFFF;

	   KeyPacker.Blended = Shader->FBlended;
	*/

	KeyPacker.Key |= ( Shader->FBlended ) ? 0xFFFFFFFFFFFFFFFFULL : 0x8000000000000000ULL;

	Lushort SPHandle  = ( reinterpret_cast<size_t>( Shader->GetShaderProgram()          ) >> 8 ) & 0xFFFF;
	Lushort TexHandle = ( reinterpret_cast<size_t>( Shader->GetTextureForTextureUnit( 0 ) ) >> 8 ) & 0xFFFF;

	KeyPacker.Key |= Luint64( SPHandle ) << 47;
	KeyPacker.Key |= Luint64( TexHandle ) << 31;
	// reset sign bit
	KeyPacker.Key |= *reinterpret_cast<Luint64*>( &Z ) & 0x7FFFFFFF;

	return KeyPacker.Key;
}
/*
bool Lesser( const clSceneIndex& Idx1, const clSceneIndex& Idx2 )
{
   clRenderState* Shader1 = Idx1.FShader;
   clRenderState* Shader2 = Idx2.FShader;

   FIXME( "" )

   // sort by rendering order
   //if ( Shader1->FRenderingOrder < Shader2->FRenderingOrder ) return true;
   //if ( Shader1->FRenderingOrder > Shader2->FRenderingOrder ) return false;

   // sort by transparency
   if ( Shader1->FBlended ) return false;
   if ( Shader2->FBlended ) return true;

   // sort by first texture
   iTexture* Tex0_1 = dynamic_cast<iTexture*>( Shader1->GetTextureForTextureUnit(0) );
   iTexture* Tex0_2 = dynamic_cast<iTexture*>( Shader2->GetTextureForTextureUnit(0) );

   if ( Tex0_1 < Tex0_2 ) return false;
   if ( Tex0_1 > Tex0_2 ) return true;

   // sort by second texture
   iTexture* Tex1_1 = dynamic_cast<iTexture*>( Shader1->GetTextureForTextureUnit(1) );
   iTexture* Tex1_2 = dynamic_cast<iTexture*>( Shader2->GetTextureForTextureUnit(1) );

   if ( Tex1_1 < Tex1_2 ) return false;
   if ( Tex1_1 > Tex1_2 ) return true;

   // sort by pointer to a VA
   if ( Idx1.FVertexArray < Idx2.FVertexArray ) return false;
   if ( Idx1.FVertexArray > Idx2.FVertexArray ) return true;

   return Shader1 < Shader2;
};
*/
clGeomInstance::clGeomInstance( int Idx, size_t Start, clGeom* Geom )
	: FGeomInstance( Idx ),
	  FStartsOfRigidsBlock( Start ),
	  FGeomNumRigids( Geom->GetCurrentMesh()->GetRigidsCount() + 2 ),
	  FHasPrivateMaterial( false ),
	  FParent( -1 )
{

}

clScene::clScene(): FGeomsCollection( 0 ),
	FGeomInstances( 0 ),
	FRigids( 0 ),
	FGlobalTransformsDirtyFlag( true ),
	FMaterials( 0 ),
	FMaterialMasked( 0 ),
	FShadersNormal( 0 ),
	FShadersShadow( 0 ),
	FShadersReflection( 0 ),
	FRenderBuffer( NULL ),
	FManualOffscreenBuffer( false ),
	FUseOffscreenBuffer( true ),
	FUseFrustumCulling( false ),
	FRenderOperations( 0 ),
	FSortedSceneDirty( true ),
	FSceneNeedsRebuild( false ),
	FIOD( 0.05f ),
	FFocalLength( 10.0f ),
	FStereoType( STEREO_NONE ),
	FLeftBuffer( NULL ),
	FRightBuffer( NULL ),
	FClearColor( true ),
	FClearDepth( true ),
	FClearStencil( true ),
	FDepthBasedShadows( true ),
	FDeferredBuffer( NULL ),
	FDeferred_Shader( NULL ),
	FPreAddBufferCallback( &EmptyPreAddBufferCallback )
{
#if defined(OS_ANDROID)
	// To speed everything up we just disable shadowmapping
	FDepthBasedShadows = false;

	// However, if there's a Mali GL ES chip, shadowmapping can be enabled later
#endif
}

clScene::~clScene()
{
	// just in case, even if we didn't allocate these buffers
	DeallocateDeferredBuffers();

	Env->DisconnectObject( this );

	Utils::DeallocateAll( FUpdatersList );

	FUpdatersList.clear();

	if ( !FManualOffscreenBuffer )
	{
		delete( FRenderBuffer );
	}

	delete( FLeftBuffer );
	delete( FRightBuffer );

	ClearLights();

	for ( size_t i = 0; i != FReflectionBuffers.size(); i++ )
	{
		delete( FReflectionBuffers[i].FRT );
	}
}

void clScene::AfterConstruction()
{
	guard();

	// pos, spot_dir, color, att, spotparams
//	AddSpotLight( vec3(-10, -4, -18), vec3(1,0,0), LC_White, vec3(1,0,0), vec3(1,1,0) );

	// create default material
	PushNewMaterial( &FDefaultMaterial );

	Env->Connect( L_EVENT_TIMER, BIND( &clScene::Event_TIMER ) );

	FRenderingTime_T   = Env->Console->GetVar( "Stats.RenderingTime_T" );
	FPickingTime_T     = Env->Console->GetVar( "Stats.PickingTime_T" );
	FDisableRendering  = Env->Console->GetVar( "DisableRendering" );
	FRenderBoxes       = Env->Console->GetVar( "Scene.RenderBoxes" );
	FRenderLights      = Env->Console->GetVar( "Scene.RenderLights" );
	FRenderReflections = Env->Console->GetVar( "Scene.RenderReflections" );
	FRenderShadowMaps  = Env->Console->GetVar( "Scene.RenderShadowMaps" );
	FEnableShadowPass     = Env->Console->GetVarDefault( "Scene.EnableShadowPass", "True" );
	FEnableReflectionPass = Env->Console->GetVarDefault( "Scene.EnableReflectionPass", "True" );
	FShowNormals       = Env->Console->GetVar( "Scene.ShowNormals" );

	CheckRendererError( "" );

	FRenderBuffer       = NULL;
	FUseOffscreenBuffer = false;

	CheckRendererError( "" );

	unguard();
}

void clScene::SetPreAddBufferCallback( LPreAddBufferCallback PreAddBufferCallback )
{
	FPreAddBufferCallback = PreAddBufferCallback ? PreAddBufferCallback : &EmptyPreAddBufferCallback;
}

void clScene::SetUseOffscreenBuffer( bool UseOffscreen, bool CreatePickingBuffer )
{
	guard();

	FUseOffscreenBuffer = UseOffscreen;

	if ( FUseOffscreenBuffer & !FRenderBuffer )
	{
		// create main buffer and picking buffer
		const int BufferWidth  = iRenderTarget::FULLSCREEN;
		const int BufferHeight = iRenderTarget::FULLSCREEN;

		LArray<LVector4i> RTParams;

		RTParams.push_back( LVector4i( BufferWidth, BufferHeight, 0, 16 ) );

		if ( CreatePickingBuffer )
		{
			RTParams.push_back( LVector4i( BufferWidth, BufferHeight, 0, 8  ) );
		}

		FRenderBuffer = Env->Renderer->CreateRenderTargetV( RTParams, true );
	}

	unguard();
}

bool clScene::GetUseOffscreenBuffer() const
{
	return FUseOffscreenBuffer;
}

void clScene::SetAnaglyphStereoRendering( LAnaglyphStereoType StereoType )
{
	if ( StereoType == STEREO_SIDE_BY_SIDE )
	{
		FAnaglyphMixer = Env->Resources->LoadShader( "Shaders/stereo_side_by_side.shader" );
	}
	else
	{
		FAnaglyphMixer = Env->Resources->LoadShader( "Shaders/stereo_anaglyph.shader" );
	}

	if ( StereoType != STEREO_NONE && ( !FLeftBuffer || !FRightBuffer ) )
	{
		// create buffers for left and right eye
		const int BufferWidth  = iRenderTarget::FULLSCREEN;
		const int BufferHeight = iRenderTarget::FULLSCREEN;

		LArray<LVector4i> RTParams;

		RTParams.push_back( LVector4i( BufferWidth, BufferHeight, 0, 16 ) );

		FLeftBuffer  = Env->Renderer->CreateRenderTargetV( RTParams, true );
		FRightBuffer = Env->Renderer->CreateRenderTargetV( RTParams, true );
	}

	FStereoType = StereoType;

	LMatrix4 LeftCoefs;
	LMatrix4 RightCoefs;

	// update mixing coefs
	// refer to http://3dtv.at/Knowhow/AnaglyphComparison_en.aspx for values
	switch ( FStereoType )
	{
		case STEREO_NONE:
			// nothing to do here
			break;
		case STEREO_GRAY:
			LeftCoefs[0] = LVector4( 0.3f, 0.59f, 0.11f, 0.0f );
			LeftCoefs[1] = LVector4( 0.0f, 0.0f,  0.0f,  0.0f  );
			LeftCoefs[2] = LVector4( 0.0f, 0.0f,  0.0f,  0.0f  );

			RightCoefs[0] = LVector4( 0.0f,  0.0f,  0.0f,  0.0f );
			RightCoefs[1] = LVector4( 0.3f,  0.59f, 0.11f, 0.0f );
			RightCoefs[2] = LVector4( 0.3f,  0.59f, 0.11f, 0.0f );
			break;
		case STEREO_TRUE:
			LeftCoefs[0] = LVector4( 0.299f, 0.587f, 0.114f, 0.0f );
			LeftCoefs[1] = LVector4( 0.0f,   0.0f,   0.0f,   0.0f );
			LeftCoefs[2] = LVector4( 0.0f,   0.0f,   0.0f,   0.0f );

			RightCoefs[0] = LVector4( 0.0f,   0.0f,   0.0f,   0.0f );
			RightCoefs[1] = LVector4( 0.0f,   0.0f,   0.0f,   0.0f );
			RightCoefs[2] = LVector4( 0.299f, 0.587f, 0.114f, 0.0f );
			break;
		case STEREO_HALFCOLOR:
			LeftCoefs[0] = LVector4( 0.299f, 0.587f, 0.114f, 0.0f );
			LeftCoefs[1] = LVector4( 0.0f,   0.0f,   0.0f,   0.0f );
			LeftCoefs[2] = LVector4( 0.0f,   0.0f,   0.0f,   0.0f );

			RightCoefs[0] = LVector4( 0.0f,  0.0f,  0.0f, 0.0f );
			RightCoefs[1] = LVector4( 0.0f,  1.0f,  0.0f, 0.0f );
			RightCoefs[2] = LVector4( 0.0f,  0.0f,  1.0f, 0.0f );
			break;
		case STEREO_FULLCOLOR:
			LeftCoefs[0] = LVector4( 1.0f, 0.0f, 0.0f, 0.0f );
			LeftCoefs[1] = LVector4( 0.0f, 0.0f, 0.0f, 0.0f );
			LeftCoefs[2] = LVector4( 0.0f, 0.0f, 0.0f, 0.0f );

			RightCoefs[0] = LVector4( 0.0f,  0.0f,  0.0f, 0.0f );
			RightCoefs[1] = LVector4( 0.0f,  1.0f,  0.0f, 0.0f );
			RightCoefs[2] = LVector4( 0.0f,  0.0f,  1.0f, 0.0f );
			break;
		case STEREO_OPTIMIZED:
			LeftCoefs[0] = LVector4( 0.0f, 0.7f, 0.3f, 0.0f );
			LeftCoefs[1] = LVector4( 0.0f, 0.0f, 0.0f, 0.0f );
			LeftCoefs[2] = LVector4( 0.0f, 0.0f, 0.0f, 0.0f );

			RightCoefs[0] = LVector4( 0.0f,  0.0f,  0.0f, 0.0f );
			RightCoefs[1] = LVector4( 0.0f,  1.0f,  0.0f, 0.0f );
			RightCoefs[2] = LVector4( 0.0f,  0.0f,  1.0f, 0.0f );
			break;
			// Dubois algorithm
			// http://www.dvbviewer.tv/forum/topic/40858-3d-anaglyph-shader/
		case STEREO_DUBOIS:
			LeftCoefs[0] = LVector4( 0.4561000f,  0.5004840f,  0.17638100f, 0.0f );
			LeftCoefs[1] = LVector4( -0.0400822f, -0.0378246f, -0.01575890f, 0.0f );
			LeftCoefs[2] = LVector4( -0.0152161f, -0.0205971f, -0.00546856f, 0.0f );

			RightCoefs[0] = LVector4( -0.0434706f, -0.0879388f, -0.00155529f, 0.0f );
			RightCoefs[1] = LVector4( 0.3784760f,  0.7336400f, -0.01845030f, 0.0f );
			RightCoefs[2] = LVector4( -0.0721527f, -0.1129610f,  1.22640000f, 0.0f );
			break;
		case STEREO_SIDE_BY_SIDE:
			// no coefficients here
			break;
		default:
			ERROR_MSG( "Unknown stereo mode" );
	};

	iShaderProgram* SP = FAnaglyphMixer->GetShaderProgram();

	SP->BindUniforms();

	SP->SetUniformNameMat4Array( "LeftEyeCoefs",  1, LeftCoefs );

	SP->SetUniformNameMat4Array( "RightEyeCoefs", 1, RightCoefs );
}

void clScene::SetFrustumCulling( bool Enabled )
{
	FUseFrustumCulling = Enabled;
}

void clScene::Resize( int Idx, float DimZ, bool CenterMesh )
{
	if ( DimZ < 0.0f )
	{
		return;
	}

	// first, we have to find a bounding box of this subtree
	LAABoundingBox BBox = GetBoundingBox( Idx );

	// now, find the true scaling coef
	float OldDimZ = fabsf( BBox.FMax.Z - BBox.FMin.Z );

	if ( OldDimZ < ::Linderdaum::Math::EPSILON )
	{
		return;
	}

	if ( DimZ    < ::Linderdaum::Math::EPSILON )
	{
		return;
	}

	float Coef = DimZ / OldDimZ;

	Scale( Idx, Coef, CenterMesh );
}

void clScene::Scale( int Idx, float Coef, bool CenterMesh )
{
	LAABoundingBox BBox = GetBoundingBox( Idx );

	// center root
	LVector3 Translation = -BBox.GetCenter();

	Translation.Z = -BBox.FMin.Z;

	LMatrix4 TM = CenterMesh ? LMatrix4::GetTranslateMatrix( Coef * Translation ) : LMatrix4::Identity();

	// scale
	TM[0][0] = Coef;
	TM[1][1] = Coef;
	TM[2][2] = Coef;

	SetScaleTransform( Idx, TM );
}

LAABoundingBox clScene::GetBoundingBox( int Idx ) const
{
	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock;
	size_t RigidEnd = RigidIdx + FGeomInstances[ Idx ].FGeomNumRigids;

	LAABoundingBox AABB = LAABoundingBox();

	for ( size_t i = RigidIdx; i != RigidEnd; i++ )
	{
		if ( FRigids[i].FRigid.FRigidIdx == -1 ) { continue; }

		LAABoundingBox Box = GetRigid( FRigids[i] )->CalculateBoundingBox();

		Box.Transform( FRigids[i].FLocalTransform );

		AABB.Combine( Box );
	}

	AABB.Transform( FRigids[RigidIdx].FLocalTransform );

	return AABB;
}

LAABoundingBox clScene::GetBoundingBoxInterpolated( int Idx ) const
{
	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock;
	size_t RigidEnd = RigidIdx + FGeomInstances[ Idx ].FGeomNumRigids;

	LAABoundingBox AABB = LAABoundingBox();

	for ( size_t i = RigidIdx; i != RigidEnd; i++ )
	{
		if ( i >= FRigids.size() ) { break; }

		if ( FRigids[i].FRigid.FRigidIdx == -1 ) { continue; }

		const clRigidInstance& Rigid = FRigids[ i ];

		clVertexAttribs* VA = GetRigid( Rigid );

		LAABoundingBox Box = VA->GetBoundingBoxInterpolated( static_cast<size_t>( Rigid.FKeyframer.GetKeyframe() ),
		                                                     static_cast<size_t>( Rigid.FKeyframer.GetNextKeyframe() ),
		                                                     Rigid.FKeyframer.GetKeyframeLerp() );

		Box.Transform( Rigid.FGlobalTransform );

		AABB.Combine( Box );
	}

	return AABB;

}

void clScene::SetAnimation( int Idx, const LString& AnimName )
{
	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock;
	size_t RigidEnd = RigidIdx + FGeomInstances[ Idx ].FGeomNumRigids;

	clGeom* Geom = FGeomsCollection[ FGeomInstances[Idx].FGeomInstance ];

	sAnimSequence Anim = Geom->GetCurrentMesh()->GetAnimSequenceByName( AnimName );

	for ( size_t i = RigidIdx; i != RigidEnd; i++ )
	{
		FRigids[i].FKeyframer = LKeyframer( Anim.FFirstFrame,
		                                    Anim.FFirstFrame + Anim.FFrames,
		                                    Anim.FLoopingFrames,
		                                    Anim.FFPS );
	}

	// create animation updater
	int Updater = FindUpdaterByClass( Idx, "clAnimationUpdater" );

	if ( Updater == -1 )
	{
		AddUpdater( Idx, Env->Linker->Instantiate( "clAnimationUpdater" ) );
	}
}

void clScene::SetAnimationSpeed( int Idx, float FPS )
{
	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock;
	size_t RigidEnd = RigidIdx + FGeomInstances[ Idx ].FGeomNumRigids;

	for ( size_t i = RigidIdx; i != RigidEnd; i++ )
	{
		FRigids[i].FKeyframer.SetFPS( FPS );
	}
}

void clScene::BlendAnimation( int Idx, const LString& AnimName )
{
	FIXME( "implement blending" )

	SetAnimation( Idx, AnimName );
}

bool clScene::UpdateAnimation( int Idx, float Time )
{
	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock;
	size_t RigidEnd = RigidIdx + FGeomInstances[ Idx ].FGeomNumRigids;

	bool Result = false;

	for ( size_t i = RigidIdx; i != RigidEnd; i++ )
	{
		if ( FRigids[i].FKeyframer.Update( Time ) ) { Result = true; }

		// To track some quirks in Keyframer
//		Env->Logger->LogP(L_LOG, "Frame(%f,%f)", FRigids[i].FKeyframer.FKeyframe[0], FRigids[i].FKeyframer.FKeyframe[1]);
	}

	return Result;
}

int clScene::FindGeomInstance( clGeom* Geom ) const
{
	for ( size_t i = 0; i != FGeomsCollection.size(); i++ )
	{
		if ( FGeomsCollection[i] == Geom )
		{
			return static_cast<int>( i );
		}
	}

	return -1;
}

std::vector<int> clScene::FindAllGeomInstances( clGeom* Geom ) const
{
	std::vector<int> Instances;

	for ( int i = 0; i != static_cast<int>( FGeomInstances.size() ); i++ )
	{
		int GeomIdx = FGeomInstances[i].FGeomInstance;

		if ( FGeomsCollection[ GeomIdx ] == Geom )
		{
			Instances.push_back( i );
		}
	}

	return Instances;
}

int clScene::AddGeom( clGeom* Geom )
{
	return AddGeomIntoSlot( Geom, -1, -1 );
}

int clScene::AddGeomMtl( clGeom* Geom, int MtlIdx )
{
	return AddGeomIntoSlot( Geom, -1, MtlIdx );
}

int clScene::AddGeomIntoSlot( clGeom* Geom, int Slot, int MtlIdx )
{
	guard();

	if ( !Geom )
	{
		Env->Console->DisplayError( "Cannot add NULL geom to the scene" );
		return -1;
	}

	// TODO: wait until geom is completely loaded before proceeding
	Geom->WaitLoad();

	FSortedSceneDirty = true;

	// create geom instance
	int GeomInstance = FindGeomInstance( Geom );

	if ( GeomInstance == -1 )
	{
		// save a pointer to this Geom
		GeomInstance = static_cast<int>( FGeomsCollection.size() );

		FGeomsCollection.push_back( Geom );

		// geom's update notification
		Geom->Connect( L_EVENT_CHANGED, BIND( &clScene::Event_CHANGED ) );
	}

	//
	// create handle to this geom
	//
	int ThisGeomHandle = -1;

	clGeomInstance Instance( GeomInstance, FRigids.size(), Geom );

	if ( Slot >= 0 )
	{
		ThisGeomHandle = Slot;
		FGeomInstances[ ThisGeomHandle ] = Instance;
	}
	else
	{
		// look for a free slot
		for ( size_t i = 0; i != FGeomInstances.size(); i++ )
		{
			if ( FGeomInstances[i].FGeomInstance == -1 )
			{
				ThisGeomHandle = static_cast<int>( i );

				FGeomInstances[ i ] = Instance;

				break;
			}
		}
	}

	// ... or create a new slot
	if ( ThisGeomHandle == -1 )
	{
		ThisGeomHandle = static_cast<int>( FGeomInstances.size() );

		FGeomInstances.FStream.push_back( Instance );
	}

	PushRigidInstances( Geom, ThisGeomHandle, MtlIdx );

	// return handle
	return ThisGeomHandle;

	unguard();
}

clGeom* clScene::GetGeom( int Idx ) const
{
	return FGeomsCollection[ FGeomInstances[ Idx ].FGeomInstance ];
}

int clScene::PushRigid( int RigidIdx, int ParentRef, int MtlRef, int GeomHandle )
{
	FSortedSceneDirty = true;

	FRigids.FStream.push_back( clRigidInstance( sRigidID( FGeomInstances[GeomHandle].FGeomInstance, RigidIdx ), ParentRef, MtlRef ) );

	return static_cast<int>( FRigids.size() ) - 1;
}

void clScene::PushRigidInstances( clGeom* Geom, int ThisGeomHandle, int MtlIdx )
{
	clMesh* Mesh = Geom->GetCurrentMesh();

	// geom transform
	int RootGeomTransformID = PushRigid( -1, -1, -1, ThisGeomHandle );

	// geom scale
	int ScaleGeomTransformID = PushRigid( -1, RootGeomTransformID, -1, ThisGeomHandle );

	// copy all rigids
	for ( size_t i = 0; i != Mesh->GetRigidsCount(); i++ )
	{
		// save parent ref with offset or -1 if none
		size_t ParentRef = ( Mesh->GetParentRef( static_cast<int>( i ) ) >= 0 ) ? FGeomInstances[ThisGeomHandle].FStartsOfRigidsBlock + Mesh->GetParentRef( static_cast<int>( i ) ) + 2 : ScaleGeomTransformID;

		int MtlRef = ( MtlIdx != -1 ) ? MtlIdx : AddMaterial( Mesh->GetMaterial( static_cast<int>( i ) ) );

		clRigidInstance Rigid( sRigidID( FGeomInstances[ThisGeomHandle].FGeomInstance, static_cast<int>( i ) ), static_cast<int>( ParentRef ), MtlRef );

		// we will recalculate global transforms from local later
		Rigid.FLocalTransform =  Mesh->GetLocalTransform( static_cast<int>( i ) );

		FRigids.FStream.push_back( Rigid );

		AddRenderOperation( &FRigids.FStream.back(), static_cast<int>( FRigids.size() ) - 1 );
	}
}

void clScene::RemoveItem( int Idx )
{
	if ( Idx < 0 ) { return; }

	FSortedSceneDirty = true;
	FSceneNeedsRebuild = true;

	// deallocate updaters
	RemoveUpdater( Idx, NULL );

	// clear parents
	for ( size_t i = 0; i != FGeomInstances.size(); i++ )
	{
		if ( FGeomInstances[i].FParent == Idx )
		{
			FGeomInstances[i].FParent = -1;
		}
	}

	size_t NumRigids = FGeomInstances[ Idx ].FGeomNumRigids;
	size_t RigidIdx  = FGeomInstances[ Idx ].FStartsOfRigidsBlock;
	size_t RigidEnd  = RigidIdx + NumRigids;
	size_t NewSize   = FRigids.size() - NumRigids;

	// clear slot
	FGeomInstances[ Idx ] = clGeomInstance();

	size_t To   = RigidIdx;

	for ( size_t Rigid = RigidEnd; Rigid != FRigids.size(); Rigid++ )
	{
		FRigids[ To ] = FRigids[ Rigid ];

		if ( FRigids[ To ].FParentRef > static_cast<int>( RigidIdx ) )
		{
			FRigids[ To ].FParentRef -= static_cast<int>( NumRigids );
		}

		To++;
	}

	// update instances
	for ( size_t i = 0; i != FGeomInstances.size(); i++ )
	{
		if ( FGeomInstances[i].FStartsOfRigidsBlock > RigidIdx )
		{
			FGeomInstances[i].FStartsOfRigidsBlock -= NumRigids;
		}
	}

	FRigids.resize( NewSize );
}

void clScene::RebuildSceneRigids()
{
	if ( !FSceneNeedsRebuild )
	{
		return;
	}

	FSceneNeedsRebuild = false;

	FRigids.FStream.clear();
	FRenderOperations.FStream.clear();

	for ( size_t i = 0; i != FGeomInstances.size(); i++ )
	{
		if ( FGeomInstances[i].FGeomInstance == -1 )
		{
			continue;
		}

		FGeomInstances[i].FStartsOfRigidsBlock = FRigids.size();

		PushRigidInstances( FGeomsCollection[ FGeomInstances[i].FGeomInstance ], static_cast<int>( i ), -1 );

		// reattach to parent
		if ( FGeomInstances[i].FParent >= 0 )
		{
			Attach( static_cast<int>( i ), FGeomInstances[i].FParent );
		}
	}
}

int clScene::PushNewMaterial( const sMaterialDesc* Material )
{
	guard();

	// add material
	FMaterials.FStream.push_back( *Material );
	FMaterialMasked.FStream.push_back( GetDefaultMask() );

	FShadersNormal.push_back( NULL );
	FShadersShadow.push_back( NULL );
	FShadersReflection.push_back( NULL );
	FShadersNormal_Deferred.push_back( NULL );

	FShadersNormal.back() = Material->GetRenderStateForPass( Env, *Material, L_MATSYS_PASS_NORMAL );
	FShadersShadow.back() = FDepthBasedShadows ? Material->GetRenderStateForPass( Env, *Material, L_MATSYS_PASS_SHADOW ) : FShadersNormal.back();
	FShadersReflection.back() = Material->GetRenderStateForPass( Env, *Material, L_MATSYS_PASS_REFL );
#ifndef OS_ANDROID
	FShadersNormal_Deferred.back() = Material->GetRenderStateForPass( Env, *Material, L_MATSYS_PASS_DEFERRED_NORMAL );
#endif

	// reset mask for shadow pass
	if ( !Material->FCastShadow )
	{
		FMaterialMasked.FStream.back() &= ~static_cast<size_t>( L_MATSYS_PASS_SHADOW );
	}

	return static_cast<int>( FMaterials.size() ) - 1;

	unguard();
}

int clScene::AddMaterial( const sMaterialDesc* Material )
{
	if ( !Material )
	{
		// return default material index in FMaterials
		return 0;
	}

	// check if we already have the same material
	for ( size_t i = 0; i != FMaterials.size(); i++ )
	{
		if ( FMaterials.FStream[i] == *Material )
		{
			return static_cast<int>( i );
		}
	}

	return PushNewMaterial( Material );
}

int clScene::AddMtl( clMaterial* Mtl )
{
	sMaterialDesc Mat = Mtl->GetMaterialDesc();

	return this->AddMaterial( &Mat );
}

size_t clScene::GetDefaultMask() const
{
	// subject is visible in all passed
	return static_cast<size_t>( L_MATSYS_PASS_NORMAL ) |
	       static_cast<size_t>( L_MATSYS_PASS_SHADOW ) |
	       static_cast<size_t>( L_MATSYS_PASS_REFL   ) |
	       static_cast<size_t>( L_MATSYS_PASS_DEFERRED_NORMAL ) |
	       static_cast<size_t>( L_MATSYS_PASS_DEFERRED_SHADOW );
}

void clScene::RecalculateGlobalTransforms()
{
	if ( !FGlobalTransformsDirtyFlag )
	{
		return;
	}

	for ( size_t Rigid = 0; Rigid != FRigids.size(); Rigid++ )
	{
		clRigidInstance& R = FRigids[ Rigid ];

		int Parent = R.FParentRef;

		LMatrix4 ParentTransform = LMatrix4::Identity();

		// collect local transformations of all parents
		// NOTE: parents are leftwards of the child inside the array, so maybe we can just use FGlobalTransforms[Parent] ?
		while ( Parent != -1 )
		{
			ParentTransform = ParentTransform * FRigids[ Parent ].FLocalTransform;
			Parent = FRigids[Parent].FParentRef;
		}

		R.FGlobalTransform = R.FLocalTransform * ParentTransform;
	}

	FGlobalTransformsDirtyFlag = false;
}

int clScene::AddGeomToParent( clGeom* Geom, int Parent )
{
	guard( "%s, %i", Geom->GetFileName().c_str(), Parent );

	int GeomIdx = AddGeom( Geom );

	Attach( GeomIdx, Parent );

	return GeomIdx;

	unguard();
}

void clScene::SetMtlIndex( int Idx, int MaterialIdx )
{
	FSortedSceneDirty = true;

	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock;
	size_t RigidEnd = RigidIdx + FGeomInstances[ Idx ].FGeomNumRigids;

	// update all rigids of this geom
	for ( size_t Rigid = RigidIdx; Rigid != RigidEnd; Rigid++ )
	{
		FRigids[ Rigid ].FMaterialRef = MaterialIdx;
	}
}

void clScene::SetMtl( int Idx, clMaterial* Material )
{
	sMaterialDesc Mat = Material->GetMaterialDesc();

	int MatIdx = AddMaterial( &Mat );

	SetMtlIndex( Idx, MatIdx );
}

sMaterialDesc& clScene::GetGeomMaterialDesc( int Idx )
{
	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock;

	int MaterialIdx = FRigids[ RigidIdx ].FMaterialRef;

	if ( !FGeomInstances[ Idx ].FHasPrivateMaterial )
	{
		// implement copy-on-write policy
		MaterialIdx = PushNewMaterial( ( MaterialIdx < 0 ) ? &FDefaultMaterial : &FMaterials[MaterialIdx] );

		this->SetMtlIndex( Idx, MaterialIdx );

		FGeomInstances[ Idx ].FHasPrivateMaterial = true;
	}

	return GetMaterialDesc( FRigids[ RigidIdx ].FMaterialRef );
}

clRenderState* clScene::GetShader( int MatID, LMatSysPass Pass )
{
	if ( Pass == L_MATSYS_PASS_SHADOW ) { return FShadersShadow[ MatID ]; }

	if ( Pass == L_MATSYS_PASS_REFL   ) { return FShadersReflection[ MatID ]; }

	return FShadersNormal[ MatID ];
}

void clScene::SetShader( int MatID, LMatSysPass Pass, clRenderState* Shader )
{
	if ( Pass == L_MATSYS_PASS_SHADOW ) { FShadersShadow[ MatID ] = Shader; }

	if ( Pass == L_MATSYS_PASS_REFL   ) { FShadersReflection[ MatID ] = Shader; }

	FShadersNormal[ MatID ] = Shader;
}

LVector4 clScene::GetKeyframe( int Idx )
{
	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock;

	return FRigids[ RigidIdx ].FKeyframer.GetKeyframeVec();
}

void clScene::SetKeyframe( int Idx, int KeyframeNum, int NextKeyframeNum, float LerpCoef )
{
	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock;
	size_t RigidEnd = RigidIdx + FGeomInstances[ Idx ].FGeomNumRigids;

	float Frame     = static_cast<float>( KeyframeNum );
	float NextFrame = static_cast<float>( NextKeyframeNum );

	for ( size_t i = RigidIdx; i != RigidEnd; i++ )
	{
		FRigids[i].FKeyframer.SetKeyframe( Frame, NextFrame, LerpCoef );
		/*
		      // software skinning
		      if ( FRigids[i] )
		      {
		         FRigids[i]->SetKeyframe( KeyframeNum, NextKeyframeNum, LerpCoef );
		         if ( FVertexArrays[i] ) FVertexArrays[i]->CommitChanges();
		      }
		*/
	}
}

void clScene::SetVisible( int Idx, bool Visible )
{
	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock;
	size_t RigidEnd = RigidIdx + FGeomInstances[ Idx ].FGeomNumRigids;

	for ( size_t i = RigidIdx; i != RigidEnd; i++ )
	{
		if ( FRigids[i].FVisible != Visible )
		{
			FSortedSceneDirty  = true;
			FSceneNeedsRebuild = true;
		}

		FRigids[i].FVisible = Visible;
	}
}

void clScene::SetReceiveShadow( int Idx, bool ReceiveShadow )
{
	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock;
	size_t RigidEnd = RigidIdx + FGeomInstances[ Idx ].FGeomNumRigids;

	for ( size_t i = RigidIdx; i != RigidEnd; i++ )
	{
		if ( FRigids[i].FReceiveShadow != ReceiveShadow )
		{
			FSortedSceneDirty = true;
		}

		FRigids[i].FReceiveShadow = ReceiveShadow;
	}
}

void clScene::SetCastShadow( int Idx, bool CastShadow )
{
	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock;
	size_t RigidEnd = RigidIdx + FGeomInstances[ Idx ].FGeomNumRigids;

	for ( size_t i = RigidIdx; i != RigidEnd; i++ )
	{
		if ( FRigids[i].FCastShadow != CastShadow )
		{
			FSortedSceneDirty = true;
		}

		FRigids[i].FCastShadow = CastShadow;
	}
}

void clScene::SetMtlFromShader( int Idx, clRenderState* ShaderNormal, clRenderState* ShaderShadow, clRenderState* ShaderReflection )
{
	clRenderState* Shadow     = ShaderShadow     ? ShaderShadow     : ShaderNormal;
	clRenderState* Reflection = ShaderReflection ? ShaderReflection : ShaderNormal;

	FSortedSceneDirty = true;

	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock;
	size_t RigidEnd = RigidIdx + FGeomInstances[ Idx ].FGeomNumRigids;

	// check if we already have the same shader
	for ( size_t i = 0; i != FMaterials.size(); i++ )
	{
		if ( FShadersNormal[i] == ShaderNormal &&
		     FShadersShadow[i] == Shadow &&
		     FShadersReflection[i] == Reflection )
		{
			// update all rigids of this geom
			for ( size_t Rigid = RigidIdx; Rigid != RigidEnd; Rigid++ )
			{
				FRigids[ Rigid ].FMaterialRef = static_cast<int>( i );
			}

			return;
		}
	}

	FMaterials.FStream.push_back( sMaterialDesc() );
	FMaterialMasked.FStream.push_back( GetDefaultMask() );
	FShadersNormal.push_back( ShaderNormal );
	FShadersShadow.push_back( Shadow );
	FShadersReflection.push_back( Reflection );
	// FIXME: hack just not to crash
#ifndef OS_ANDROID
	FShadersNormal_Deferred.push_back( ShaderNormal );
#endif

	int MaterialRef = static_cast<int>( FMaterials.size() ) - 1;

	// update all rigids of this geom
	for ( size_t Rigid = RigidIdx; Rigid != RigidEnd; Rigid++ )
	{
		FRigids[ Rigid ].FMaterialRef = MaterialRef;
	}
}

void clScene::SetOwner( int Idx, iObject* Owner )
{
	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock;
	size_t RigidEnd = RigidIdx + FGeomInstances[ Idx ].FGeomNumRigids;

	for ( size_t Rigid = RigidIdx; Rigid != RigidEnd; Rigid++ )
	{
		FRigids[ Rigid ].FOwner = Owner;
	}
}

void clScene::Attach( int Child, int Parent )
{
	FGlobalTransformsDirtyFlag = true;

	FGeomInstances[ Child ].FParent = Parent;

	// update all root rigids of this geom (they have -1 as ParentRef)
	size_t RigidIdx = FGeomInstances[ Child ].FStartsOfRigidsBlock;
	size_t RigidEnd = RigidIdx + FGeomInstances[ Child ].FGeomNumRigids;

	size_t ParentRigidIdx = FGeomInstances[ Parent ].FStartsOfRigidsBlock;

	for ( size_t Rigid = RigidIdx; Rigid != RigidEnd; Rigid++ )
	{
		if ( FRigids[ Rigid ].FParentRef == -1 )
		{
			FRigids[ Rigid ].FParentRef = static_cast<int>( ParentRigidIdx );
		}
	}
}

void clScene::Detach( int Child, int Parent )
{
	TODO( "implement" )
}

void clScene::SetLocalTransform( int Idx, const LMatrix4& Transform )
{
	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock;

	FRigids[ RigidIdx ].FLocalTransform  = Transform;

	FGlobalTransformsDirtyFlag = true;
}

const LMatrix4& clScene::GetLocalTransform( int Idx ) const
{
	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock;

	return FRigids[ RigidIdx ].FLocalTransform;
}

void clScene::SetScaleTransform( int Idx, const LMatrix4& Transform )
{
	size_t RigidIdx = FGeomInstances[ Idx ].FStartsOfRigidsBlock + 1;

	FRigids[ RigidIdx ].FLocalTransform  = Transform;

	FGlobalTransformsDirtyFlag = true;
}

clVertexAttribs* clScene::GetRigid( const clRigidInstance& Rigid ) const
{
	if ( Rigid.FRigid.FRigidIdx == -1 )
	{
		return NULL;
	}

	clGeom* Geom = FGeomsCollection[ Rigid.FRigid.FGeomIdx ];
	clMesh* Mesh = Geom->GetCurrentMesh();

	return Mesh->GetRigid( Rigid.FRigid.FRigidIdx );
}

iObject* clScene::PickObject( const LVector2& Pnt )
{
	int Idx = PickPair( Pnt );

	return ( Idx > -1 ) ? FRigids[ Idx ].FOwner : NULL;
}

clVertexAttribs* clScene::PickVA( const LVector2& Pnt )
{
	int Idx = PickPair( Pnt );

	return ( Idx > -1 ) ? GetRigid( FRigids[ Idx ] ) : NULL;
}

int clScene::PickPair( const LVector2& Pnt )
{
	if ( !FUseOffscreenBuffer )
	{
		return -1;
	}

	double StartTime = Env->GetSeconds();

	// read from ATTACHMENT1 of the render buffer
	FRenderBuffer->BindReadFrom( 1 );

	Luint C = Env->Renderer->GetPixel( Pnt );

	FRenderBuffer->UnBind();

	// decode pixel
	uColorHasher Color;

	// mask 3 lower bytes
	Luint NodeNum = Color.InvHashColor( C & 0xFFFFFF ) - 1;

	int Rigid = static_cast<int>( NodeNum ) - 1;

	// check for validity
	//
	// NOTE: seems that if NodeNum >= FNodes.size() we've got a logic error
	if ( NodeNum >= FRigids.size() )
	{
		Rigid = -1;
	}

	FPickingTime_T->SetDouble( FPickingTime_T->GetDouble() + Env->GetSeconds() - StartTime );

	return Rigid;
}

void clScene::AddRenderOperation( const clRigidInstance* Rigid, int RigidIdx )
{
	int MtlIdx = Rigid->FMaterialRef;

	if ( MtlIdx == -1 ) { return; }

	if ( Rigid->FRigid.FRigidIdx == -1 ) { return; }

	if ( !Rigid->FVisible ) { return; }

	clGeom* Geom = FGeomsCollection[ Rigid->FRigid.FGeomIdx ];
	clMesh* Mesh = Geom->GetCurrentMesh();

	clRenderOperation RenderOp;

	// create render operation
	RenderOp.FSortKey  = Makekey( FShadersNormal[ MtlIdx ], 0.0f );
	RenderOp.FRigid    = RigidIdx;
	RenderOp.FMask     = GetDefaultMask();
	RenderOp.FPlanarReflection = NULL;
	RenderOp.FVertexArray = Mesh->GetVertexArray( Rigid->FRigid.FRigidIdx );
#if defined( USE_OPENGL_3 )
	RenderOp.FPackedSkeleton = Mesh->GetPackedSkeleton( Rigid->FRigid.FRigidIdx );
#else
	RenderOp.FPackedSkeleton = NULL;
#endif
	RenderOp.FSkinned = ( GetRigid( *Rigid )->GetSkeletonFramesCount() > 0 );

	// don't cast shadow
	if ( !( GetRigid( *Rigid )->GetCastShadow() && Rigid->FCastShadow ) )
	{
		RenderOp.FMask &= ~static_cast<size_t>( L_MATSYS_PASS_SHADOW );
	}

	// add render operation
	FRenderOperations.FStream.push_back( RenderOp );
}

void clScene::RebuildRenderOperations()
{
	if ( !FSceneNeedsRebuild ) { return; }

	double Time = Env->GetSeconds();

	FRenderOperations.FStream.clear();

	// compress scene (remove empty slots)
	for ( int i = 0; i != static_cast<int>( FRigids.size() ); i++ )
	{
		AddRenderOperation( &FRigids[i], i );
	}

	Env->Logger->Log( L_NOTICE, "Scene renderops rebuilt in " + LStr::ToStr( Env->GetSeconds() - Time ) + " seconds" );
}

void clScene::SortScene()
{
	if ( !FSortedSceneDirty ) { return; }

	guard();

	RebuildRenderOperations();

	double Time = Env->GetSeconds();

	size_t Size = FRenderOperations.size();

	clRenderOperation* RenderOp = &FRenderOperations[0];

	// update sort keys
	for ( size_t i = 0; i != Size; i++, RenderOp++ )
	{
		RenderOp->FSortKey = Makekey( FShadersNormal[ FRigids[ RenderOp->FRigid ].FMaterialRef ], 0.0f );

//      clRigidInstance& Rigid = FRigids[ RenderOp->FRigid ];
//      int MtlIdx = Rigid.FMaterialRef;
//      iShaderProgram* Shader = FShadersNormal[ MtlIdx ]->GetShaderProgram();
//      Env->Logger->Log( L_NOTICE, LStr::ToStrHex( RenderOp->FSortKey ) + " " + Shader->GetFileName() );
	}

	// sort scene's render operations
	Math::RadixSort( FRenderOperations.FStream );

	Env->Logger->Log( L_NOTICE, "Scene sorted in " + LStr::ToStr( Env->GetSeconds() - Time, 5 ) + " seconds" );

	RenderOp = &FRenderOperations[0];
	/*
	      for ( size_t i = 0; i != Size; i++, RenderOp++ )
	      {
	         clRigidInstance& Rigid = FRigids[ RenderOp->FRigid ];

	         int MtlIdx = Rigid.FMaterialRef;

	         iShaderProgram* Shader = FShadersNormal[ MtlIdx ]->GetShaderProgram();

	         Env->Logger->Log( L_NOTICE, LStr::ToStrHex( RenderOp->FSortKey ) + " " + Shader->GetFileName() );
	      }
	*/
	FSortedSceneDirty = false;

	unguard();
}

void clScene::RenderForward()
{
	double StartTime = Env->GetSeconds();

	if ( FStereoType != STEREO_NONE )
	{
		// Asymmetric Frustum Parallel Axis Projection Stereo
		// as described at http://www.orthostereo.com/geometryopengl.html
		float FOV;
		float Aspect;
		float NearCP;
		float FarCP;

		LMatrix4 Proj   = GetCameraProjection();
		LMatrix4 Transf = GetCameraTransform();

		Math::PerspectiveToParams( Proj, FOV, Aspect, NearCP, FarCP );

		// TODO: what the heck with "-" and Aspect?
		LMatrix4 LeftProj  = Math::PerspectiveStereo( FOV, 1.0f / Aspect, -NearCP, -FarCP, FIOD, FFocalLength, true  );
		LMatrix4 RightProj = Math::PerspectiveStereo( FOV, 1.0f / Aspect, -NearCP, -FarCP, FIOD, FFocalLength, false );

		LMatrix4 LeftTransform  = Math::LookAtStereoMatrix( Transf, FIOD, true  );
		LMatrix4 RightTransform = Math::LookAtStereoMatrix( Transf, FIOD, false );

		SetCameraProjection( LeftProj );
		SetCameraTransform( LeftTransform );
		RenderForward_Internal( FLeftBuffer );

		SetCameraProjection( RightProj );
		SetCameraTransform( RightTransform );
		RenderForward_Internal( FRightBuffer );

		SetCameraProjection( Proj );
		SetCameraTransform( Transf );

		FAnaglyphMixer->SetTexture( 0, FLeftBuffer->GetColorTexture( 0 ), false );
		FAnaglyphMixer->SetTexture( 1, FRightBuffer->GetColorTexture( 0 ), false );

		if ( FUseOffscreenBuffer ) { FRenderBuffer->Bind( 0 ); }

		Env->Renderer->GetCanvas()->FullscreenRect( FAnaglyphMixer );

		if ( FUseOffscreenBuffer ) { FRenderBuffer->UnBind(); }
	}
	else
	{
		RenderForward_Internal( FUseOffscreenBuffer ? FRenderBuffer : NULL );
	}

	FRenderingTime_T->SetDouble( FRenderingTime_T->GetDouble() + Env->GetSeconds() - StartTime );
}

void clScene::RenderForward_Internal( iRenderTarget* ToBuffer )
{
	RecalculateGlobalTransforms();

	SortScene();

	UpdateLights();

	if ( FEnableShadowPass->GetBool()     ) { UpdateShadows( &FRenderOperations ); }

	if ( FEnableReflectionPass->GetBool() ) { UpdateReflections( &FRenderOperations ); }

	if ( ToBuffer ) { ToBuffer->Bind( 0 ); }

	Env->Renderer->ClearRenderTarget( FClearColor, FClearDepth, FClearStencil );

	SendSync( L_EVENT_SCENE_PRERENDER, LEventArgs( this ), false );

	RenderSortedScene( &FRenderOperations, NULL, &FShadersNormal, L_MATSYS_PASS_NORMAL, FShowNormals->GetBool() );

#if !defined( OS_ANDROID )
	if ( FRenderBoxes->GetBool()       ) { DebugRenderBoxes( &FRenderOperations ); }

//	if ( FRenderLights->GetBool()      ) { DebugRenderLights(); }
	if ( FRenderReflections->GetBool() ) { DebugRenderReflections(); }

	if ( FRenderShadowMaps->GetBool()  ) { DebugRenderShadowMaps(); }
#endif

	if ( ToBuffer ) { ToBuffer->UnBind(); }

	SendSync( L_EVENT_SCENE_POSTRENDER, LEventArgs( this ), false );
}

int clScene::AddReflectionPlane( const LPlane& Plane )
{
	guard();

	for ( size_t j = 0; j != FReflectionBuffers.size(); j++ )
	{
		if ( FReflectionBuffers[j].FReflPlane.IsEqualEps( Plane, Math::EPSILON ) )
		{
			return static_cast<int>( j );
		}
	}

	const int REFL_WIDTH  = 1024;

	const int REFL_HEIGHT = 1024;

	const int REFL_BITS   = 16;

	clReflectionBuffer Buf( Plane, Env->Renderer->CreateRenderTarget( REFL_WIDTH, REFL_HEIGHT, 0, REFL_BITS, true, 1 ) );

	FReflectionBuffers.push_back( Buf );

	return static_cast<int>( FReflectionBuffers.size() ) - 1;

	unguard();
}

void clScene::RenderOneReflectionMap( const LMatrix4& NewViewMatrix,
                                      sAttribStream<clRenderOperation>* RenderOps,
                                      iRenderTarget* RT,
                                      int ThisMaterialIdx,
                                      const LVector4* ClipPlane )
{
	// save old view matrix
	LMatrix4 OldViewMatrix = FMatrices.GetViewMatrix();

	FMatrices.SetViewMatrix( NewViewMatrix );

	RT->Bind( 0 );

	Env->Renderer->ClearRenderTarget( true, true, true );

	// set the pass mask - don't render this material in the mirror
	FMaterialMasked[ ThisMaterialIdx ] &= ~static_cast<size_t>( L_MATSYS_PASS_REFL );

	RenderSortedScene( RenderOps, ClipPlane, &FShadersReflection, L_MATSYS_PASS_REFL, false );

	// restore mask
	FMaterialMasked[ ThisMaterialIdx ] |= static_cast<size_t>( L_MATSYS_PASS_REFL );

	RT->UnBind();

	// restore view matrix
	FMatrices.SetViewMatrix( OldViewMatrix );
}

void clScene::UpdateReflections( sAttribStream<clRenderOperation>* RenderOps )
{
	guard();

	size_t Size = RenderOps->size();

	if ( !Size )
	{
		return;
	}

	clRenderOperation* RenderOp = &( *RenderOps )[0];

	// reset update flags
	for ( size_t i = 0; i != FReflectionBuffers.size(); i++ )
	{
		FReflectionBuffers[i].FRendered = false;
	}

	for ( size_t i = 0; i != Size; i++, RenderOp++ )
	{
		int MtlIdx = FRigids[ RenderOp->FRigid ].FMaterialRef;

		sMaterialDesc* Mat = &FMaterials[ MtlIdx ];

		RenderOp->FPlanarReflection = NULL;

		if ( !Mat->FPlanarReflection ) { continue; }

		LPlane Plane( Mat->FReflectionPlane );

		FIXME( "this is slow ( O(n^2) ): we need to add a new reflection plane only then we change scene layout" )

		int ReflPlane = AddReflectionPlane( Plane );

		iRenderTarget* RT = FReflectionBuffers[ ReflPlane ].FRT;

		// store per-rigid pointer to this reflection map
		RenderOp->FPlanarReflection = RT->GetColorTexture( 0 );

		// this RT was already updated
		if ( FReflectionBuffers[ ReflPlane ].FRendered ) { continue; }

		FReflectionBuffers[ ReflPlane ].FRendered = true;

		// render reflection into RT
		RenderOneReflectionMap( Plane.Mirror() * FMatrices.GetViewMatrix(), RenderOps, RT, MtlIdx, Plane.ToVec4Ptr() );
	}

	unguard();
}

template <class T>
void ShiftArray( LArray<T>& Arr, int From )
{
	for ( size_t i = From + 1 ; i != Arr.size() ; i++ )
	{
		Arr[i - 1] = Arr[i];
	}

	Arr.pop_back();
}

void clScene::RemoveLight( int idx )
{
	if ( idx < 0 || idx >= static_cast<int>( FLightShadowMaps.size() ) ) { return; }

	// accurate array shifts and delete() calls
	delete FLightShadowMaps[idx];

	ShiftArray( FLightTypes, idx );
	ShiftArray( FLightEnabled, idx );
	ShiftArray( FLightShadowMaps, idx );
	ShiftArray( FLightProjections, idx );
	ShiftArray( FLightTransforms, idx );
	ShiftArray( FLightAttenuations, idx );
	ShiftArray( FLightSpotDirections, idx );
	ShiftArray( FLightSpotParams, idx );
	ShiftArray( FLightShadowMatrices, idx );

	ShiftArray( FLightAmbientColors, idx );
	ShiftArray( FLightSpecularColors, idx );
	ShiftArray( FLightDiffuseColors, idx );
}

void clScene::ClearLights()
{
	for ( size_t i = 0 ; i != FLightShadowMaps.size() ; i++ )
	{
		delete( FLightShadowMaps[i] );
	}

	FLightTypes.clear();
	FLightEnabled.clear();
	FLightShadowMaps.clear();
	FLightProjections.clear();
	FLightTransforms.clear();
	FLightAttenuations.clear();
	FLightSpotDirections.clear();
	FLightSpotParams.clear();
	FLightShadowMatrices.clear();

	FLightAmbientColors.clear();
	FLightSpecularColors.clear();
	FLightDiffuseColors.clear();

	FLightTypes.reserve( 16 );
	FLightEnabled.reserve( 16 );
	FLightShadowMaps.reserve( 16 );
	FLightProjections.reserve( 16 );
	FLightTransforms.reserve( 16 );
	FLightAttenuations.reserve( 16 );
	FLightSpotDirections.reserve( 16 );
	FLightSpotParams.reserve( 16 );
	FLightShadowMatrices.reserve( 16 );
	FLightAmbientColors.reserve( 16 );
	FLightSpecularColors.reserve( 16 );
	FLightDiffuseColors.reserve( 16 );
}

int clScene::AddDirLight ( const LVector3& Dir, const LVector4& Color, const LVector3& Attenuation )
{
	int idx = AddNewLight( LLIGHT_DIR, true, Dir, Color, Attenuation );

	LVector3 V1, V2;
	BuildComplementaryBasis( Dir, V1, V2 );

	LMatrix4 Mtx;
	Mtx[0] = LVector4( Dir.GetNormalized(), 0.0f );
	Mtx[1] = LVector4( V1, 0.0f );
	Mtx[2] = LVector4( V2, 0.0f );
	Mtx[3] = LVector4( 0, 0, 0, 1 );

	mtx4& Proj = FLightProjections[idx];

	FLightTransforms[idx] = Mtx;
	FLightShadowMatrices[idx] = Mtx * Proj;

	return idx;
}

int clScene::AddSpotLight( const LVector3& Pos, const LVector3& SpotDir, const LVector4& Color, const LVector3& Attenuation, const LVector3& SpotParams )
{
	int idx = AddNewLight( LLIGHT_SPOT, true, Pos, Color, Attenuation );

	FLightSpotDirections[idx] = SpotDir;
	FLightSpotParams[idx] = SpotParams;

	return idx;
}

int clScene::AddPointLight( const LVector3& Pos, const LVector4& Color, const LVector3& Attenuation, const LVector3& PointParams )
{
	return AddNewLight( LLIGHT_OMNI, true, Pos, Color, Attenuation );
}

/// Allocate space in each array of the light
int clScene::AddNewLight( int LightType, bool Enabled, const LVector3& Pos, const LVector4& Color, const LVector3& Attenuation )
{
	guard();
	LMatrix4 Mtx( LMatrix4::GetTranslateMatrix( Pos ) );

	FLightTypes.push_back( ( float )LightType );
	FLightEnabled.push_back( Enabled ? 1.0f : 0.0f );

	FLightAttenuations.push_back( Attenuation );
	FLightTransforms.push_back( Mtx );
	FLightAmbientColors.push_back( Color );
	FLightSpecularColors.push_back( Color );
	FLightDiffuseColors.push_back( Color );

	FLightSpotDirections.push_back( vec3( 0 ) );
	FLightSpotParams.push_back( vec3( 0 ) );

	/// TODO: calculate from SpotParams (FOV = SpotParams.SpotCosCutoff)
	// construct light projection matrix
	LMatrix4 Proj = Math::Perspective( 80.0f, Env->Viewport->GetAspectRatio(), 0.4f, 1000.0f );

	FLightProjections.push_back( Proj );

	// aux shadow matrix
	FLightShadowMatrices.push_back( Mtx * Proj );

	// shadow map buffer
	iRenderTarget* AuxRT = NULL;

	// Here comes the magic:
	if ( FDepthBasedShadows )
	{
		// use full-blown depth-based *ShadowMaps
		AuxRT = Env->Renderer->CreateRenderTarget( 512, 512, 0, 8, true, 1 );
	}
	else
	{
		// work-around, use direct half-float world coords
		AuxRT = Env->Renderer->CreateRenderTarget( 512, 512, 0, 8, false, 1 );
	}

	FLightShadowMaps.push_back( AuxRT );

	return static_cast<int>( FLightTypes.size() ) - 1;

	unguard();
}

void clScene::UpdateShadows( sAttribStream<clRenderOperation>* RenderOps )
{
	guard();

	size_t NumLights = FLightTransforms.size();

	// New code: use light arrays directly
	for ( size_t i = 0; i != NumLights ; i++ )
	{
		// save old matrices
		LMatrix4 CameraProj = FMatrices.GetProjectionMatrix();
		LMatrix4 ViewMatrix = FMatrices.GetViewMatrix();

		FMatrices.SetViewMatrix( FLightTransforms[i] );
		FMatrices.SetProjectionMatrix( FLightProjections[i] );

		FLightShadowMaps[i]->Bind( 0 );

		Env->Renderer->ClearRenderTarget( true, true, true );

		RenderSortedScene( RenderOps, NULL, &FShadersShadow, L_MATSYS_PASS_SHADOW, false );

		FLightShadowMaps[i]->UnBind();

		// restore matrices
		FMatrices.SetViewMatrix( ViewMatrix );
		FMatrices.SetProjectionMatrix( CameraProj );
	}

	unguard();
}

void clScene::DebugLogSortedScene( sAttribStream<clRenderOperation>* RenderOps, clStatesList* Shaders ) const
{
	size_t Size = RenderOps->size();

	if ( !Size ) { return; }

	// now let's process all renderops
	clRenderOperation* RenderOp = &( *RenderOps )[0];

	Env->Logger->Log( L_NOTICE, "Scene states:" );
	Env->Logger->Log( L_NOTICE, "-------------" );

	for ( size_t i = 0; i != Size; i++, RenderOp++ )
	{
		const clRigidInstance& Rigid = FRigids[ RenderOp->FRigid ];

		int MtlIdx = Rigid.FMaterialRef;

		clRenderState*  Shader = Shaders->at( MtlIdx );
		iShaderProgram* SP     = Shader->GetShaderProgram();
		LString Blended = Shader->FBlended ? "(blended)" : "";

		TODO( "Log here" );
		Env->Logger->Log( L_NOTICE, "Texture(0): " + LStr::ToStrHex( ( size_t )Shader->GetTextureForTextureUnit( 0 ) ) + " SP: " + LStr::ToStrHex( ( size_t )SP ) + Blended + " Key: " + LStr::ToStrHex( RenderOp->FSortKey ) );
	}

	Env->Logger->Log( L_NOTICE, "-------------" );
}

void clScene::UpdateShaders( const LVector4* ClipPlane, clStatesList* Shaders, bool ShowNormals )
{
	float  EnableClipPlaneF = ClipPlane ? 1.0f : 0.0f;
	size_t NumShaders       = Shaders->size();

	FMatrices.UpdateMatricesBuffer();

	for ( size_t i = 0; i != NumShaders; i++ )
	{
		clRenderState* Shader = Shaders->at( i );

		if ( !Shader ) { continue; }

		/// Assign SINGLE shadow map. TODO: for CSM we need something really flexible. I.e., we can pack 4 maps into a single texture
//		for( size_t j = 0 ; j != FLightShadowMaps.size() ; j++)
		Shader->SetTexture( 8, FLightShadowMaps.empty() ? NULL : FLightShadowMaps[0]->GetDepthTexture(), false );

		/// ... and single shadow matrix
		LMatrix4* ShadowMatrix = FLightShadowMatrices.empty() ? NULL : &FLightShadowMatrices[0];

		iShaderProgram* SP = Shader->GetShaderProgram();

		/// Set light uniforms
		SP->SetLightUniformsPerShader( this );

		/// Set common uniforms
		SP->SetSceneUniformsPerShader( ShadowMatrix != NULL, ShadowMatrix ? *ShadowMatrix : LMatrix4::Identity(), EnableClipPlaneF, ClipPlane != NULL, ClipPlane ? *ClipPlane : LVector4( 0 ), ShowNormals );
	}
}

void clScene::RenderSortedScene( sAttribStream<clRenderOperation>* RenderOps,
                                 const LVector4* ClipPlane,
                                 clStatesList* Shaders,
                                 LMatSysPass Pass,
                                 bool ShowNormals )
{
	guard();

	if ( FDisableRendering->GetBool() ) { return; }

	Env->Logger->MarkTime( L_DEBUG, "TIMER: RenderSortedScene()" );

//	DebugLogSortedScene( RenderOps, Shaders );

	size_t PassMask         = static_cast<size_t>( Pass );
	size_t Size             = RenderOps->size();

	// check the size, otherwise &(*SortedScene)[0] will fail on empty scene
	if ( !Size ) { return; }

	// update per-shader properties
	UpdateShaders( ClipPlane, Shaders, ShowNormals );

	// prepare color hasher (user for framebuffer picking to convert object ID into color)
	uColorHasher Hasher;

	// construct viewing frustum
	LFrustum Frustum( FMatrices.GetProjectionMatrix(), FMatrices.GetModelViewMatrix() );

	// now let's process all renderops
	clRenderOperation* RenderOp = &( *RenderOps )[0];

	for ( size_t i = 0; i != Size; i++, RenderOp++ )
	{
		clRigidInstance& Rigid = FRigids[ RenderOp->FRigid ];

		int MtlIdx = Rigid.FMaterialRef;

		bool Masked1 = ( FMaterialMasked[ MtlIdx ] & PassMask ) == 0;
		bool Masked2 = ( RenderOp->FMask           & PassMask ) == 0;

		if ( Masked1 || Masked2 ) { continue; }

		// 1. Frustum culling
		if ( FUseFrustumCulling )
		{
			LAABoundingBox Box = GetBoundingBoxInterpolated( Rigid.FRigid.FGeomIdx );

			// cull against entire Geom bounding box and bypass all rigids contained in this Geom
			if ( !Frustum.IsAABBInFrustum( Box ) ) { continue; }
		}

		clRenderState*  Shader = Shaders->at( MtlIdx );
		iShaderProgram* SP     = Shader->GetShaderProgram();

		sMaterialDesc& MtlDesc = FMaterials[ MtlIdx ];

		// 2. Update animation and picking color
		SP->SetSceneUniformsPerRigid( MtlDesc,
		                              Hasher.IntToColor( static_cast<Luint>( RenderOp->FRigid + 1 ) ),
		                              Rigid.FKeyframer.GetKeyframeVec(),
		                              RenderOp->FSkinned,
		                              GetRigid( Rigid )->GetReceiveShadow() && Rigid.FReceiveShadow );

#if defined( USE_OPENGL_3 )
		Shader->SetTexture( 0, RenderOp->FPackedSkeleton, false );
#else

		if ( RenderOp->FSkinned )
		{
			// do a nasty software skinning
			clVertexAttribs* VA = GetRigid( Rigid );
			const clJointsSet& Frame1 = VA->GetSkeletonFrame( Rigid.FKeyframer.GetKeyframe() );
			const clJointsSet& Frame2 = VA->GetSkeletonFrame( Rigid.FKeyframer.GetNextKeyframe() );
			VA->CalcPoints( const_cast<clJointsSet*>( &Frame1 ), const_cast<clJointsSet*>( &Frame2 ), Rigid.FKeyframer.GetKeyframeLerp() );
			RenderOp->FVertexArray->CommitChanges();
		}

#endif

#if defined( USE_OPENGL_3 )
		// 3. Bind associated reflection texture
		Shader->SetTexture( 7, RenderOp->FPlanarReflection, false );
#endif

		// 4. Render geometry
		FMatrices.SetModelMatrix( Rigid.FGlobalTransform );

		FPreAddBufferCallback( Shader, &Rigid );

		SP->SetTransformationUniforms( FMatrices );

		Env->Renderer->AddBuffer( RenderOp->FVertexArray, Shader, 1, MtlDesc.FWireframe );
	}

	Env->Logger->MarkTime( L_DEBUG, "TIMER: Loop %f" );

	unguard();
}

void clScene::UpdateLights()
{
	// Not necessary, because we directly own the arrays
	/*
	   for ( size_t i = 0; i != FLights.size(); i++ )
	   {
	      FLightsPos[i]   = FLights[i]->FPosition;
	      FLightsColor[i] = FLights[i]->FColor;
	   }
	*/
}

void clScene::DebugRenderBoxes( sAttribStream<clRenderOperation>* SortedScene ) const
{
	Env->Renderer->GetCanvas()->SetMatrices( FMatrices.GetProjectionMatrix(), FMatrices.GetViewMatrix() );

	LAABoundingBox SceneBB;

	// render LABB for every geom instance
	for ( int i = 0; i != static_cast<int>( FGeomInstances.size() ); i++ )
	{
		LAABoundingBox Box = GetBoundingBoxInterpolated( i );

		SceneBB.Combine( Box );

		Env->Renderer->GetCanvas()->BoundingBox( Box, LC_White );
	}

	Env->Renderer->GetCanvas()->BoundingBox( SceneBB, LC_Red );

	/*
	   // render LABB for every rigid in the scene (this is heavy)

	   clRenderOperation* RenderOp = &(*SortedScene)[0];

	   size_t Size = SortedScene->size();

	   for ( size_t i = 0; i != Size; i++, RenderOp++ )
	   {
	      clRigidInstance& Rigid = FRigids[ RenderOp->FRigid ];

	      clVertexAttribs* VA = GetRigid( Rigid );

	      LAABoundingBox Box = VA->GetBoundingBoxInterpolated( static_cast<size_t>( Rigid.FKeyframes[0] ),
	                                                           static_cast<size_t>( Rigid.FKeyframes[1] ),
	                                                           Rigid.FKeyframes[2] );
	      Box.Transform( Rigid.FGlobalTransform );

	      Env->Renderer->GetCanvas()->BoundingBox( FMatrices.GetProjectionMatrix(), FMatrices.GetViewMatrix(), Box, LC_White );
	   }
	*/
}

void clScene::DebugRenderLights() const
{
	Env->Renderer->GetCanvas()->SetMatrices( FMatrices.GetProjectionMatrix(), FMatrices.GetViewMatrix() );

	for ( size_t i = 0; i != FLightProjections.size(); i++ )
	{
		Env->Renderer->GetCanvas()->Sphere( LSphere( FLightTransforms[i][3].ToVector3(), 0.1f ), LC_White );
	}

	Env->Renderer->GetCanvas()->Flush();
}

void clScene::DebugRenderReflections() const
{
	if ( FReflectionBuffers.size() > 0 )
	{
		Env->Renderer->GetCanvas()->TexturedRect( 0.0f, 0.2f, 0.3f, 0.5f, FReflectionBuffers[0].FRT->GetColorTexture( 0 ), NULL, LC_White );
		Env->Renderer->GetCanvas()->Flush();
	}
}

void clScene::DebugRenderShadowMaps() const
{
	if ( FLightTransforms.size() > 0 && FLightShadowMaps.size() > 0 )
	{
		Env->Renderer->GetCanvas()->TexturedRect( 0.0f, 0.4f, 0.3f, 0.7f, FLightShadowMaps[0]->GetDepthTexture(), NULL, LC_White );
		Env->Renderer->GetCanvas()->Flush();
	}
}

int clScene::FindUpdater( int Idx, iUpdater* Updater ) const
{
	for ( size_t i = 0; i != FUpdatersList.size(); i++ )
	{
		if ( ( FUpdatersList[i] == Updater ) && FUpdatersList[i]->IsIndex( Idx ) )
		{
			return static_cast<int>( i );
		}
	}

	return -1;
}

int clScene::FindUpdaterByClass( int Idx, const LString& UpdaterClassName ) const
{
	for ( size_t i = 0; i != FUpdatersList.size(); i++ )
	{
		if ( FUpdatersList[i]->IsIndex( Idx ) && FUpdatersList[i]->ClassName() == UpdaterClassName )
		{
			return static_cast<int>( i );
		}
	}

	return -1;
}

void clScene::AddUpdater( int Idx, iUpdater* Updater )
{
	if ( Updater )
	{
		if ( FindUpdater( Idx, Updater ) != -1 )
		{
			return;
		}

		FUpdatersList.push_back( Updater );

		Updater->Attach( Idx, this );
	}
}

void clScene::RemoveUpdater( int Idx, iUpdater* Updater )
{
	if ( !Updater )
	{
		// deallocate all
		for ( size_t i = 0; i != FUpdatersList.size(); )
		{
			if ( FUpdatersList[i]->IsIndex( Idx ) )
			{
				delete( FUpdatersList[i] );

				FUpdatersList[i] = FUpdatersList.back();
				FUpdatersList.pop_back();

				continue;
			}

			i++;
		}
	}

	// deallocate specific updater
	if ( int UpdaterID = FindUpdater( Idx, Updater ) != -1 )
	{
		delete( FUpdatersList[ UpdaterID ] );

		FUpdatersList[ UpdaterID ] = FUpdatersList.back();
		FUpdatersList.pop_back();
	}
}

void clScene::Event_TIMER( LEvent Event, const LEventArgs& Args )
{
	// ping all updaters
	if ( Env->IsPaused() )
	{
		for ( size_t i = 0; i != FUpdatersList.size(); i++ )
		{
			FUpdatersList[i]->UpdateInPause( Args.FFloatArg );
		}
	}
	else
	{
		for ( size_t i = 0; i != FUpdatersList.size(); i++ )
		{
			FUpdatersList[i]->Update( Args.FFloatArg );
		}
	}
}

void clScene::Event_CHANGED( LEvent Event, const LEventArgs& Args )
{
	guard();

	clGeom* Geom = dynamic_cast<clGeom*>( Args.FObjArg );

	if ( !Geom ) { return; }

	Env->Logger->Log( L_DEBUG, ClassName() + " received Event_CHANGED for " + Geom->GetFileName() );

	// 1. Find IDs of all instances of this geom
	std::vector<int> Instances = FindAllGeomInstances( Geom );

	// 2. Collect transforms
	std::vector<LMatrix4> Mtx( Instances.size() );

	for ( int i = 0; i != static_cast<int>( Instances.size() ); i++ )
	{
		Mtx[i] = GetLocalTransform( i );
	}

	// 3. Remove all instances
	for ( int i = 0; i != static_cast<int>( Instances.size() ); i++ )
	{
		Env->Logger->Log( L_DEBUG, "Removing instance: " + LStr::ToStr( i ) );
		RemoveItem( i );
	}

	// 4. Remove garbage
	RebuildSceneRigids();

	// 5. Re-add instances
	for ( size_t i = 0; i != Instances.size(); i++ )
	{
		Env->Logger->Log( L_DEBUG, "Re-adding instance: " + LStr::ToStr( i ) );
		AddGeomIntoSlot( Geom, static_cast<int>( i ), -1 );
	}

	// 6. Restore transforms
	for ( int i = 0; i != static_cast<int>( Instances.size() ); i++ )
	{
		SetLocalTransform( i, Mtx[i] );
	}

	unguard();
}

void clScene::AttachToBone( int ChildID, int ParentGeomID,  int BoneID, const vec3& LocalOffset )
{
	TODO( "Implement" )
}

void clScene::DetachFromBone( int ChildID, int ParentGeomID )
{
	TODO( "Implement" )
}

/// Deferred rendering stuff

/// Recursion entry point
void clScene::RenderDeferred()
{
	AllocateDeferredBuffers();

	/// No reflection, zero recursion level
	RenderDeferred_Internal( FUseOffscreenBuffer ? FRenderBuffer : NULL );
}

/// Render a single frame with reflections/refractions. Possibly recursive
void clScene::RenderDeferred_Internal( iRenderTarget* ToBuffer )
{
	RecalculateGlobalTransforms();
	SortScene();

//	UpdateLights();

	SendSync( L_EVENT_SCENE_PRERENDER, LEventArgs( this ), false );

	FDeferredBuffer->Bind( 0 );
	//if ( ToBuffer ) { ToBuffer->Bind( 0 ); }
	Env->Renderer->ClearRenderTarget( FClearColor, FClearDepth, FClearStencil );
	RenderSortedScene( &FRenderOperations, NULL, &FShadersNormal_Deferred, L_MATSYS_PASS_DEFERRED_NORMAL, FShowNormals->GetBool() );
	//if ( ToBuffer ) { ToBuffer->UnBind(); }
	FDeferredBuffer->UnBind();

	// 2-nd deferred pass - apply lighting
	if ( ToBuffer ) { ToBuffer->Bind( 0 ); }

	Env->Renderer->ClearRenderTarget( FClearColor, FClearDepth, FClearStencil );
	Env->Renderer->GetCanvas()->FullscreenRect( FDeferred_Shader );

	if ( ToBuffer ) { ToBuffer->UnBind(); }

	SendSync( L_EVENT_SCENE_POSTRENDER, LEventArgs( this ), false );


//	if( RecursionDepth > MAX_DEPTH) return;

	// 1. Render the scene using ViewMatrix and fill each buffer

	// Render_Params[] - shader for each buffer fill pass
	// DeferredBuffers[i] – buffers
	/*
	   foreach i
	   DeferredBuffers[i]->Bind();
	      RenderSortedScene( List , Render_Params[i] )
	   DeferredBuffers[i]->Unbind();
	*/
	// 2. Calculate shadows - CSM/PSM, depending on the light type
	/*
	   foreach light j
	   ShaderBuffers[j]->Bind();
	      RenderSortedScene( List, Shadow_Shader[j] );  // depends on the shadow type
	   ShaderBuffers[j]->Unbind();
	*/

	// 3. Blend everything and apply lighting models
}

void clScene::AllocateDeferredBuffers()
{
	if ( FDeferred_Shader ) { return; }

	FDeferred_Shader = Env->Resources->LoadShader( "MaterialSystem/default_pass2.shader" );

	// create fullscreen buffers
	const int BufferWidth  = iRenderTarget::FULLSCREEN;
	const int BufferHeight = iRenderTarget::FULLSCREEN;

	LArray<LVector4i> RTParams;

	// HDR diffuse
	RTParams.push_back( LVector4i( BufferWidth, BufferHeight, 0, 16 ) );

	// picking
	RTParams.push_back( LVector4i( BufferWidth, BufferHeight, 0, 8  ) );

	// normals
	RTParams.push_back( LVector4i( BufferWidth, BufferHeight, 0, 32 ) );

	// world positions
	RTParams.push_back( LVector4i( BufferWidth, BufferHeight, 0, 32 ) );

//	if ( CreatePickingBuffer )
//	{
//		RTParams.push_back( LVector4i( BufferWidth, BufferHeight, 0, 8  ) );
//	}

	FDeferredBuffer = Env->Renderer->CreateRenderTargetV( RTParams, true );

	FDeferred_Shader->SetTexture( 0, FDeferredBuffer->GetColorTexture( 0 ), false );
	FDeferred_Shader->SetTexture( 1, FDeferredBuffer->GetColorTexture( 1 ), false );
	FDeferred_Shader->SetTexture( 2, FDeferredBuffer->GetColorTexture( 2 ), false );
	FDeferred_Shader->SetTexture( 3, FDeferredBuffer->GetColorTexture( 3 ), false );
}

void clScene::DeallocateDeferredBuffers()
{
	if ( FDeferredBuffer ) { FDeferredBuffer->DisposeObject(); FDeferredBuffer = NULL; }

//	if ( FAdvancedBuffer ) { FAdvancedBuffer->DisposeObject(); FAdvancedBuffer = NULL; }
}

/*
 * 15/04/2011
     SetMtlFromShader() now accepts 3 shaders for every pass
     Added SetShader()
 * 11/02/2011
     Dynamic update of changed Geoms
 * 29/09/2010
     Prevent all shadowcasting on explicit 'false' in clRigidInstance::FCastShadow and clRigidInstance::FReceiveShadow
 * 26/09/2010
     Updaters are called according to the current IsPaused() state (ticket #120)
 * 25/09/2010
     Light matrices moved to clLight
 * 29/08/2010
     Frustum culling repaired
 * 01/08/2010
     Debug rendering
 * 04/07/2010
     AddUpdater()
     RemoveUpdater()
 * 26/06/2010
     Rewritten
 * 23/04/2010
     UpdateShadows()
 * 31/03/2010
     Scene sorting improved
 * 18/03/2010
     PushRigid()
 * 13/03/2010
     GPU skinning
 * 08/03/2010
     Scene sorting
 * 07/03/2010
     First working version
 * 04/03/2010
     It's here
*/
