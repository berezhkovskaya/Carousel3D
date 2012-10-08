/**
 * \file VolumeRenderer.cpp
 * \brief Linderdaum Volume Rendering Library - object-oriented implementation
 * \version 0.5.91
 * \date 30/05/2010
 * \author Sergey Kosarevsky, 2009-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "VolumeRenderer.h"

#include "Core/Logger.h"
#include "Math/LMath.h"
#include "Utils/Viewport.h"
#include "Geometry/Geom.h"
#include "Geometry/GeomServ.h"
#include "Geometry/VertexAttribs.h"
#include "Scene/Scene.h"
#include "Renderer/Canvas.h"
#include "Renderer/iTexture.h"
#include "Renderer/iVertexArray.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iShaderProgram.h"
#include "Images/Image.h"
#include "Resources/ResourcesManager.h"

#pragma region Recommended render target parameters

/// Render target reduction divisor. 1 - size is unchanged, 2 - half size, 3 - one third, etc.
const int RT_Reduction = 2;

/// Minimal render target width
const int RT_MinWidth  = 512;

/// Minimal render target height
const int RT_MinHeight = 512;

/// Maximal render target width
const int RT_MaxWidth  = 1600;

/// Maximal render target height
const int RT_MaxHeight = 1200;

#pragma endregion

/// Names of texture units for low-level interoperation with raycasting shader
enum LVolumeTextureSlot
{
	L_SLOT_VOLUME     = 0,
	L_SLOT_TF         = 1,
	L_SLOT_NOISE      = 2,
	L_SLOT_GRADIENT   = 3,
	L_SLOT_2DMAP      = 4,
	L_SLOT_ESL        = 5
};

/// Main raycaster shader program name
const LString VolumeProg_Name    = "VolumeLib\\raycaster.sp";

/// Volume texture with 3D noise to render procedural materials
const LString NoiseTexture_Name  = "Textures\\noisevolume.dds";

/// Shader program name for offscreen buffer compositing
const LString BlenderProg_Name   = "VolumeLib\\blender.sp";

clRenderState* clLVLibVolume::CreateShader( iShaderProgram* Prog )
{
	clRenderState* Shader = Env->Resources->CreateShader();

	Shader->SetShaderProgram( Prog );
	Shader->FDepthTest = true;
	Shader->FDepthWrites = true;

	return Shader;
}

clLVLibVolume::clLVLibVolume(): FMatrices(),
	FVolumeLoaded( false ),
	FNeedRelink( false ),
	FRenderOffscreen( false ),
	FCameraInsideVolume( false ),
	FScattering( false ),
	FUse2DMap( false ),
	FUseESL( false ),
	FOffscreenBufferInternal( NULL ),
	FOffscreenBuffer( NULL ),
	FOffscreenBufferFinal( NULL ),
	FProxyGeometry( NULL ),
	FVolumeShader( NULL ),
	FVolumeProg( NULL ),
	FSizeX( 1.0f ),
	FSizeY( 1.0f ),
	FSizeZ( 1.0f ),
	FRenderMode( L_VRMODE_ISOSURFACE ),
	FRenderMaterial( L_VRMATERIAL_DEFAULT ),
	FGradientsMode( L_VRGRADIENTS_FORWARDDIFF ),
	FTransferFunctionMode( L_TFMODE_NONE ),
	FClipPlane( false ),
	FMVPChanged( true ),
	FViewportNeedResize( false ),
	FUpdateModel( true )
{
	FPrecompiledPrograms.clear();

	FOldProjection.IdentityMatrix();
	FOldModelView.IdentityMatrix();

	FL_MAT_COLOR = LVector4( 1.0f, 0.842717f, 0.625152f, 1.0f );
	FL_LIGHT_POS = LVector3( 100.0f, -100.0f, 0.0f );

	FL_SATURATION    = 0.2f;

	FL_CUT_OFF_LOW   = 0.11f;
	FL_ISO_VALUE     = 0.3f;

	FL_TRACING_STEPS = 128.0f;

	FL_CUT_PLANE   = LVector4( 0.0f, 0.0f, 0.0f, -1.0f );
	FL_CUT_PLANE_THICKNESS = 2.0f;
	FL_CUT_PLANE_COLOR = LVector4( 1.0f, 0.0f, 0.0f, 1.0f );

	FL_SCALE_2DMAP = LVector3( 1.0f, 1.0f, 1.0f );
}

clLVLibVolume::~clLVLibVolume()
{
	Env->DisconnectObject( this );

	delete( FProxyGeometry );

	delete( FOffscreenBufferInternal );
	delete( FOffscreenBufferFinal );

	delete( FVolumeShader );
	delete( FBlenderShader );
}

void clLVLibVolume::SetOffscreenRenderTarget( iRenderTarget* RT )
{
	FOffscreenBuffer = RT;
}

LString clLVLibVolume::GetPropertiesString()
{
	LString S = "";

	sVideoSystemInfo Info = Env->Renderer->GetVideoSystemInfo();

	if ( LStr::ContainsSubStr( Info.FVendor, "NVIDIA" ) )
	{
		S += "NVIDIA ";
	}

	if ( LStr::ContainsSubStr( Info.FVendor, "ATI" ) ||
	     LStr::ContainsSubStr( Info.FVendor, "AMD" ) )
	{
		S += "AMD ";
	}

	if ( FRenderMode == L_VRMODE_ISOSURFACE )
	{
		S += "MODE_ISOSURFACE ";
	}

	if ( FRenderMode == L_VRMODE_OPACITY )
	{
		S += "MODE_OPACITY ";
	}

	if ( FRenderMode == L_VRMODE_MIP )
	{
		S += "MODE_MIP ";
	}

	if ( FRenderMaterial == L_VRMATERIAL_DEFAULT )
	{
		S += "MATERIAL_IVORY ";
	}

	if ( FRenderMaterial == L_VRMATERIAL_MARBLE )
	{
		S += "MATERIAL_MARBLE ";
	}

	if ( FRenderMaterial == L_VRMATERIAL_WOOD )
	{
		S += "MATERIAL_WOOD ";
	}

	if ( FRenderMaterial == L_VRMATERIAL_IVORY )
	{
		S += "MATERIAL_IVORY ";
	}

	if ( FScattering )
	{
		S += "MATERIAL_SCATTERED ";
	}

	if ( FUse2DMap   )
	{
		S += "USE_2D_TEXTURE_MAP ";
	}

	if ( FUseESL     )
	{
		S += "USE_ESL_MAP ";
	}

	if ( FGradientsMode == L_VRGRADIENTS_CENDERDIFF  )
	{
		S += "NORMAL_CENTERDIFF ";
	}

	if ( FGradientsMode == L_VRGRADIENTS_FORWARDDIFF )
	{
		S += "NORMAL_FORWARDDIFF ";
	}

	if ( FGradientsMode == L_VRGRADIENTS_PRECALCSOBEL )
	{
		S += "NORMAL_PRECOMPUTED_RGBA ";
	}

	if ( FGradientsMode == L_VRGRADIENTS_SOBEL       )
	{
		S += "NORMAL_DYN_FILTERED ";
	}

	if ( FTransferFunctionMode == L_TFMODE_NONE     )
	{
		S += "TF_NONE ";
	}

	if ( FTransferFunctionMode == L_TFMODE_1D       )
	{
		S += "TF_1D ";
	}

	if ( FTransferFunctionMode == L_TFMODE_1DPREINT )
	{
		S += "TF_1D_PREINTEGRATION ";
	}

	if ( FTransferFunctionMode == L_TFMODE_2D       )
	{
		S += "TF_2D ";
	}

	if ( FClipPlane )
	{
		S += "ENABLE_CLIPPLANE ";
	}

	return S;
}

void clLVLibVolume::EVENT_HANDLER( Event_VIEWPORTRESIZED )
{
	FViewportNeedResize = true;
}

void clLVLibVolume::EVENT_HANDLER( Event_CHANGED )
{
	guard();

	FMVPChanged = true;
	FUpdateModel = true;

	Env->Logger->Log( L_DEBUG, ClassName() + " received Event_CHANGED" );

	unguard();
}

void clLVLibVolume::EVENT_HANDLER( Event_CHANGED_VOLUME )
{
	guard();

	FMVPChanged = true;
	FUpdateModel = true;
	FVolumeLoaded = true;

	Env->Logger->Log( L_DEBUG, ClassName() + " received Event_CHANGED_VOLUME" );

	unguard();
}

void clLVLibVolume::AfterConstruction()
{
	Env->Connect( L_EVENT_VIEWPORTRESIZED, BIND( &clLVLibVolume::Event_VIEWPORTRESIZED ) );

	// 1. Create render targets
//   int WidthFull  = 1600;
//   int HeightFull = 1200;
	int WidthFull  = 1024;
	int HeightFull = 768;

	if ( WidthFull  > RT_MaxWidth  )
	{
		WidthFull  = RT_MaxWidth;
	}

	if ( HeightFull > RT_MaxHeight )
	{
		HeightFull = RT_MaxHeight;
	}

	int Width  = Env->Viewport->GetWidth() / RT_Reduction;
	int Height = Env->Viewport->GetHeight() / RT_Reduction;

	if ( Width  < RT_MinWidth  )
	{
		Width  = RT_MinWidth;
	}

	if ( Height < RT_MinHeight )
	{
		Height = RT_MinHeight;
	}

	FOffscreenBuffer      = Env->Renderer->CreateRenderTarget( Width, Height, 0, 8, true, 1 );
	FOffscreenBufferFinal = Env->Renderer->CreateRenderTarget( WidthFull, HeightFull, 0, 8, true, 1 );
	FOffscreenBufferInternal = FOffscreenBuffer;

	// 2. Create proxy geometry
	FProxyGeometry = Env->Renderer->AllocateEmptyVA();

	const float EdgeOffset = 1.0f - ( 1.0f / 256.0f );

	clVertexAttribs* Box = clGeomServ::CreateAxisAlignedBox( EdgeOffset * LVector3( -1, -1, -1 ),
	                                                         EdgeOffset * LVector3(  1,  1,  1 ), true );

	FProxyGeometry->SetVertexAttribs( Box );

	// 3. Create volume shader
	FVolumeProg = Env->Resources->LoadSP( VolumeProg_Name, GetPropertiesString() );
	BindParameters( FVolumeProg );

	FVolumeShader = CreateShader( FVolumeProg );
	FVolumeShader->SetBlending( L_SRC_ALPHA, L_ONE_MINUS_SRC_ALPHA );
	FVolumeShader->FBackfaceCulling = true;
	FVolumeShader->FBackFacing = false;
//   FVolumeShader->FDepthWrites = true;
//   FVolumeShader->FDepthTest   = true;

	FVolumeShader->SetTexture( L_SLOT_VOLUME, NULL, false );
	FVolumeShader->SetTexture( L_SLOT_TF, NULL, false );
	FVolumeShader->SetTexture( L_SLOT_NOISE, Env->Resources->LoadImg( NoiseTexture_Name, L_TEXTURE_3D )->GetTexture(), false );
	FVolumeShader->SetTexture( L_SLOT_GRADIENT, NULL, false );
	FVolumeShader->SetTexture( L_SLOT_2DMAP, NULL, false );
	FVolumeShader->SetTexture( L_SLOT_ESL, NULL, false );

	FBlenderShader = CreateShader( Env->Resources->LoadSP( BlenderProg_Name, "" ) );
	FBlenderShader->SetTexture( 0, FOffscreenBuffer->GetColorTexture( 0 ), false );
	FBlenderShader->SetBlending( L_SRC_ALPHA, L_ONE_MINUS_SRC_ALPHA );
}

void clLVLibVolume::BindParameters( iShaderProgram* Prog )
{
	Prog->BindUniforms();

	Prog->SetUniformNameVec4Array( "MAT_COLOR", 1, FL_MAT_COLOR );
	Prog->SetUniformNameVec3Array( "LIGHT_POS", 1, FL_LIGHT_POS );

	Prog->SetUniformNameFloat( "SATURATION", FL_SATURATION );

	Prog->SetUniformNameFloat( "CUT_OFF_LOW", FL_CUT_OFF_LOW );
	Prog->SetUniformNameFloat( "ISO_VALUE",   FL_ISO_VALUE );

	Prog->SetUniformNameFloat( "TRACING_STEPS", FL_TRACING_STEPS );

	Prog->SetUniformNameVec4Array( "CUT_PLANE", 1, FL_CUT_PLANE );
	Prog->SetUniformNameFloat( "CUT_PLANE_THICKNESS", FL_CUT_PLANE_THICKNESS );
	Prog->SetUniformNameVec4Array( "CUT_PLANE_COLOR", 1, FL_CUT_PLANE_COLOR );
	Prog->SetUniformNameVec3Array( "SCALE_2DMAP", 1, FL_SCALE_2DMAP );
	Prog->SetUniformNameVec4Array( "VOLUME_SIZE", 1, LVector4( FSizeX, FSizeY, FSizeZ, 1.0f ) );
}

void clLVLibVolume::SetVolumeSize_Internal( iShaderProgram* Prog,
                                            float SizeX, float SizeY, float SizeZ )
{
	if ( !Prog )
	{
		return;
	}

	Prog->BindUniforms();
	Prog->SetUniformNameVec4Array( "VOLUME_SIZE", 1, LVector4( FSizeX, FSizeY, FSizeZ, 1.0f ) );
}

void clLVLibVolume::SetRenderOffscreen( bool Offscreen )
{
	if ( FRenderOffscreen != Offscreen )
	{
		FUpdateModel = true;
	}

	FRenderOffscreen = Offscreen;
};

void clLVLibVolume::Render()
{

	if ( !FVolumeLoaded )
	{
		return;
	}

	if ( FNeedRelink )
	{
		FNeedRelink = false;
		RelinkShader();
	}

	if ( FMVPChanged )
	{
		FMVPChanged = false;
		FUpdateModel = true;
	}

	if ( FUpdateModel && !FRenderOffscreen )
	{
		FVolumeShader->GetShaderProgram()->SetTransformationUniforms( FMatrices );

		BindParameters( FVolumeProg );
		FOffscreenBufferFinal->Bind( 0 );
		Env->Renderer->ClearRenderTarget( true, true, true );
		Env->Renderer->AddBuffer( FProxyGeometry, FVolumeShader, 1, false );
		FOffscreenBufferFinal->UnBind();

		FUpdateModel = false;
	}

	FBlenderShader->SetTexture( 0, FOffscreenBufferFinal->GetColorTexture( 0 ), false );
	FBlenderShader->SetTexture( 1, FOffscreenBufferFinal->GetDepthTexture(), false );

	// 3. Render volume
	if ( FRenderOffscreen )
	{
		BindParameters( FVolumeProg );

		FOffscreenBuffer->Bind( 0 );
		Env->Renderer->ClearRenderTarget( true, true, true );

//   }
		FVolumeShader->GetShaderProgram()->SetTransformationUniforms( FMatrices );
		Env->Renderer->AddBuffer( FProxyGeometry, FVolumeShader, 1, false );
//   if ( FRenderOffscreen )
//   {

		FOffscreenBuffer->UnBind();

		FBlenderShader->SetTexture( 0, FOffscreenBuffer->GetColorTexture( 0 ), false );
		FBlenderShader->SetTexture( 1, FOffscreenBuffer->GetDepthTexture(), false );
	}

	// 4. Render on screen
//   if ( FRenderOffscreen )
	if ( FOffscreenBufferInternal == FOffscreenBuffer )
	{
		Env->Renderer->GetCanvas()->FullscreenRect( FBlenderShader );
	}
}

void clLVLibVolume::RelinkShader()
{
	FMVPChanged = true;
	FUpdateModel = true;

	LString Props = GetPropertiesString();

	// look in cache first
	std::map<LString, iShaderProgram*>::const_iterator Iter = FPrecompiledPrograms.find( Props );

	if ( Iter == FPrecompiledPrograms.end() )
	{
		// load new shader program
		FVolumeProg = Env->Resources->LoadSP( VolumeProg_Name, Props );
		FPrecompiledPrograms[ Props ] = FVolumeProg;
	}
	else
	{
		// apply cached shader program
		FVolumeProg = Iter->second;
	}

	SetVolumeSize( FSizeX, FSizeX, FSizeZ );

	// apply current rendering parameters to the shader
	BindParameters( FVolumeProg );

	FVolumeShader->SetShaderProgram( FVolumeProg );
}

void clLVLibVolume::SetProjection( const LMatrix4& Projection )
{
	if ( FMatrices.GetProjectionMatrix() != Projection )
	{
		FMVPChanged = true;
	}

	FOldProjection = FMatrices.GetProjectionMatrix();

	FMatrices.SetProjectionMatrix( Projection );
}

void clLVLibVolume::SetModelView( const LMatrix4& ModelView )
{
	if ( FMatrices.GetModelMatrix() != ModelView )
	{
		FMVPChanged = true;
	}

	FOldModelView = FMatrices.GetModelMatrix();

	FMatrices.SetModelMatrix( ModelView );

	LAABoundingBox Box = FProxyGeometry->GetVertexAttribs()->CalculateBoundingBox();

	LVector3 Camera = ( ModelView.GetInversed() * LVector4( 0, 0, 0, 1 ) ).ToVector3();

	FCameraInsideVolume = Box.IsPointInside( Camera );
}

clImage* clLVLibVolume::GetVolume()
{
	return FVolumeShader->GetTextureForTextureUnit( L_SLOT_VOLUME )->GetImage();
//   return
}

void clLVLibVolume::SetVolume( clImage* Volume )
{
	Volume->WaitLoad();
	SetVolumeTexture( Volume->GetTexture() );
	Volume->Connect( L_EVENT_CHANGED, BIND( &clLVLibVolume::Event_CHANGED ) );
}

void clLVLibVolume::ChangeVolumeTexture( iTexture* Volume )
{
	FVolumeShader->SetTexture( L_SLOT_VOLUME, Volume, false );
	Volume->Connect( L_EVENT_CHANGED, BIND( &clLVLibVolume::Event_CHANGED ) );
}

void clLVLibVolume::SetVolumeTexture( iTexture* Volume )
{
	FVolumeShader->SetTexture( L_SLOT_VOLUME, Volume, false );

	SetVolumeSize( FSizeX, FSizeY, FSizeZ );

	if ( Volume )
	{
		clImage* Image = Volume->GetImage();

		FVolumeLoaded = true;

		if ( Image )
		{
			Image->Connect( L_EVENT_CHANGED, BIND( &clLVLibVolume::Event_CHANGED_VOLUME ) );
			FVolumeLoaded = Image->IsAsyncLoadComplete();
		}
	}
}

void clLVLibVolume::SetVolumeSize( float SizeX, float SizeY, float SizeZ )
{
	FSizeX = SizeX;
	FSizeY = SizeY;
	FSizeZ = SizeZ;

	// update all shader programs
	SetVolumeSize_Internal( FVolumeProg,    SizeX, SizeY, SizeZ );

	FUpdateModel = true;
}

void clLVLibVolume::SetGradients( clImage* Gradients )
{
	Gradients->WaitLoad();
	FVolumeShader->SetTexture( L_SLOT_GRADIENT, Gradients->GetTexture(), false );

	FGradientsMode = L_VRGRADIENTS_PRECALCSOBEL;

	FNeedRelink = true;

	if ( Gradients ) { Gradients->Connect( L_EVENT_CHANGED, BIND( &clLVLibVolume::Event_CHANGED ) ); }
}

void clLVLibVolume::SetESLMap( clImage* ESLMap )
{
	ESLMap->WaitLoad();
	FVolumeShader->SetTexture( L_SLOT_ESL, ESLMap->GetTexture(), false );

	FUseESL = ( ESLMap != NULL );

	FNeedRelink = true;

	if ( ESLMap ) { ESLMap->Connect( L_EVENT_CHANGED, BIND( &clLVLibVolume::Event_CHANGED ) ); }
}

void clLVLibVolume::SetScattering( bool Scattering )
{
	FScattering = Scattering;

	FNeedRelink = true;
}

void clLVLibVolume::SetGradientsMode( LGradientsMode Mode )
{
	FGradientsMode = Mode;

	FNeedRelink = true;
}

void clLVLibVolume::SetIsosurfaceValue( const float IsoValue )
{
	FL_ISO_VALUE = IsoValue;

	FUpdateModel = true;
}

void clLVLibVolume::SetIsosurfaceColor( const LVector4& RGBColor )
{
	FL_MAT_COLOR = RGBColor;

	FUpdateModel = true;
}

void clLVLibVolume::SetLightPosition( const LVector3& LightPos )
{
	FUpdateModel = ( ( FL_LIGHT_POS - LightPos ).Length() > Math::EPSILON );

	FL_LIGHT_POS = LightPos;
}

void clLVLibVolume::EnableClipPlane( const LVector4& ClippingPlane, const LVector4& ClipColor, float Thickness )
{
	SetClipPlaneParams( ClippingPlane, ClipColor, Thickness );

	FClipPlane = true;
	FNeedRelink = true;
}

void clLVLibVolume::SetClipPlaneParams( const LVector4& ClippingPlane, const LVector4& ClipColor, float Thickness )
{
	LVector3 Normal = ClippingPlane.ToVector3().GetNormalized();

	FL_CUT_PLANE = LVector4( Normal, ClippingPlane.W );
	FL_CUT_PLANE_THICKNESS = Thickness;

	FL_CUT_PLANE_COLOR = ClipColor;
	FL_CUT_PLANE_COLOR.W = 1.0f;
}

void clLVLibVolume::DisableClipPlane()
{
	FClipPlane = false;
	FNeedRelink = true;
}

void clLVLibVolume::SetIsosurfaceMaterial( const LVolumeRenderMaterial Material )
{
	FRenderMaterial = Material;

	FNeedRelink = true;
}

void clLVLibVolume::SetLowCutoff( const float Value )
{
	FL_CUT_OFF_LOW = Value;

	FUpdateModel = true;
}

void clLVLibVolume::SetOpacitySaturation( const float Value )
{
	FL_SATURATION = Value;

	FUpdateModel = true;
}

void clLVLibVolume::SetIterations( const int Iterations )
{
	FL_TRACING_STEPS = static_cast<float>( Iterations );

	FUpdateModel = true;
}

void clLVLibVolume::SetRenderMode( const LVolumeRenderMode Mode )
{
	FRenderMode = Mode;

	FNeedRelink = true;
}

void clLVLibVolume::SetTransferFunction( clImage* TF, LTransferFunctionMode TFMode )
{
	TF->WaitLoad();
	FVolumeShader->SetTexture( L_SLOT_TF, TF ? TF->GetTexture() : NULL, false );

	FTransferFunctionMode = TFMode;

	if ( !TF )
	{
		FTransferFunctionMode = L_TFMODE_NONE;
	}

	FNeedRelink = true;
}

void clLVLibVolume::Set2DTextureMap( clImage* Map, const LVector2& Scale )
{
	FL_SCALE_2DMAP = LVector3( Scale, 1.0f );

	FVolumeShader->SetTexture( L_SLOT_2DMAP, Map ? Map->GetTexture() : NULL, false );

	FUse2DMap = ( Map != NULL );

	FNeedRelink = true;
}

bool clLVLibVolume::AutoLoadVolumeAndGradients( const LString& FileName,
                                                int SizeX,
                                                int SizeY,
                                                int SizeZ,
                                                int BitsPerPixel,
                                                bool Scale,
                                                bool PrecomputeGradient,
                                                bool CreateESLMap )
{
	clImage* Volume = Env->Resources->LoadImgHints( FileName, L_TEXTURE_3D, SizeX, SizeY, SizeZ, false, false );

	SetVolume( Volume );

	if ( PrecomputeGradient )
	{
		clImage* Grad = Env->Resources->LoadImgHints( FileName, L_TEXTURE_3D, SizeX, SizeY, SizeZ, true, false );

		SetGradients( Grad );
	}

	//if ( Scale )
	{
		using std::max;
		int MaxDim = max( SizeX, max( SizeY, SizeZ ) );

		float CoefX = static_cast<float>( SizeX ) / static_cast<float>( MaxDim );
		float CoefY = static_cast<float>( SizeY ) / static_cast<float>( MaxDim );
		float CoefZ = static_cast<float>( SizeZ ) / static_cast<float>( MaxDim );

		SetVolumeSize( CoefX, CoefY, CoefZ );
	}

	if ( CreateESLMap )
	{
		clImage* ESL = Env->Resources->LoadImgHints( FileName, L_TEXTURE_3D, SizeX, SizeY, SizeZ, false, true );

		SetESLMap( ESL );
	}

	return true;
}

clGeom* clLVLibVolume::CreateVolumeGeom() const
{
	clGeom* Geom = Env->Resources->CreateGeom();

	const float EdgeOffset = 1.0f - ( 1.0f / 256.0f );

	clVertexAttribs* ProxyBox = clGeomServ::CreateAxisAlignedBox( EdgeOffset * LVector3( -1, -1, -1 ),
	                                                              EdgeOffset * LVector3(  1,  1,  1 ), true );

	Geom->GetCurrentMesh()->AddVertexAttribs( "Volume", ProxyBox, LMatrix4::Identity(), -1, -1 );

	return Geom;
}

int clLVLibVolume::AddToScene( clScene* Scene ) const
{
	int Idx = Scene->AddGeom( CreateVolumeGeom() );

	Scene->SetMtlFromShader( Idx, FVolumeShader, NULL, NULL );


	return Idx;
}

/*
 * 30/05/2010
     Initial support for depth blending
 * 29/05/2010
     Backface culling bug fixed
 * 16/02/2010
     Single pass rendering
 * 14/02/2010
     Fixes for OpenGL 3.2 core compatibility
 * 13/05/2009
     L_SLOT_ESL texture slot reserved
 * 01/05/2009
     Empty space leaping
 * 05/04/2009
     LVLib interface fully implemented
 * 03/04/2009
     Automatic transfer function mode selection
 * 02/04/2009
     Precaching of raycasting shader programs for fast rendering mode switching
 * 23/03/2009
     It's here
*/
