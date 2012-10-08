/**
 * \file ASE.cpp
 * \brief Probably one of the most complete .ASE parsers on the Net
 * \version 0.5.93
 * \date 17/11/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "ASE.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "Core/Logger.h"
#include "Resources/ResourcesManager.h"
#include "Geometry/VertexAttribs.h"
#include "Utils/Exceptions.h"

#include <stdio.h>

#undef ASE_HEAVY_DEBUG

TODO( "make this faster by removing stupid GetToken()s" )

static const LString ASE_Signature             =  "*3DSMAX_ASCIIEXPORT";
static const LString ASE_Scene                 =  "*SCENE";
static const LString ASE_FrameSpeed            =  "*SCENE_FRAMESPEED";
static const LString ASE_TicksPerFrame         =  "*SCENE_TICKSPERFRAME";
static const LString ASE_GeomObject            =  "*GEOMOBJECT";
static const LString ASE_ShapeObject           =  "*SHAPEOBJECT";
static const LString ASE_Group                 =  "*GROUP";
static const LString ASE_NodeTM                =  "*NODE_TM";
static const LString ASE_TMRow0                =  "*TM_ROW0";
static const LString ASE_TMRow1                =  "*TM_ROW1";
static const LString ASE_TMRow2                =  "*TM_ROW2";
static const LString ASE_TMRow3                =  "*TM_ROW3";
static const LString ASE_TMAnimation           =  "*TM_ANIMATION";
static const LString ASE_ControlPosTrack       =  "*CONTROL_POS_TRACK";
static const LString ASE_ControlRotTrack       =  "*CONTROL_ROT_TRACK";
static const LString ASE_ControlScaleTrack     =  "*CONTROL_SCALE_TRACK";
static const LString ASE_Comment               =  "*COMMENT";
static const LString ASE_MaterialList          =  "*MATERIAL_LIST";
static const LString ASE_MaterialCount         =  "*MATERIAL_COUNT";
static const LString ASE_Material              =  "*MATERIAL";
static const LString ASE_SubMaterial           =  "*SUBMATERIAL";
static const LString ASE_SubMaterialCount      =  "*NUMSUBMTLS";
static const LString ASE_MaterialName          =  "*MATERIAL_NAME";
static const LString ASE_MaterialClass         =  "*MATERIAL_CLASS";
static const LString ASE_MaterialAmbient       =  "*MATERIAL_AMBIENT";
static const LString ASE_MaterialDiffuse       =  "*MATERIAL_DIFFUSE";
static const LString ASE_MaterialSpecular      =  "*MATERIAL_SPECULAR";
static const LString ASE_MaterialShine         =  "*MATERIAL_SHINE";
static const LString ASE_MaterialShineStrength =  "*MATERIAL_SHINESTRENGTH";
static const LString ASE_MaterialTransparency  =  "*MATERIAL_TRANSPARENCY";
static const LString ASE_MaterialWireSize      =  "*MATERIAL_WIRESIZE";
static const LString ASE_MaterialShading       =  "*MATERIAL_SHADING";
static const LString ASE_MaterialXPFalloff     =  "*MATERIAL_XP_FALLOFF";
static const LString ASE_MaterialSelfIllum     =  "*MATERIAL_SELFILLUM";
static const LString ASE_MaterialFalloff       =  "*MATERIAL_FALLOFF";
static const LString ASE_MaterialXPType        =  "*MATERIAL_XP_TYPE";
static const LString ASE_MaterialMapAmbient    =  "*MAP_AMBIENT";
static const LString ASE_MaterialMapDiffuse    =  "*MAP_DIFFUSE";
static const LString ASE_MaterialMapSpecular   =  "*MAP_SPECULAR";
static const LString ASE_MaterialMapBump       =  "*MAP_BUMP";
static const LString ASE_MaterialMapName       =  "*MAP_NAME";
static const LString ASE_MaterialMapClass      =  "*MAP_CLASS";
static const LString ASE_MaterialMapSubNo      =  "*MAP_SUBNO";
static const LString ASE_MaterialMapAmount     =  "*MAP_AMOUNT";
static const LString ASE_MaterialMapBitmap     =  "*BITMAP ";
static const LString ASE_MaterialMapType       =  "*MAP_TYPE";
static const LString ASE_MaterialMapUOffset    =  "*UVW_U_OFFSET";
static const LString ASE_MaterialMapVOffset    =  "*UVW_V_OFFSET";
static const LString ASE_MaterialMapUTiling    =  "*UVW_U_TILING";
static const LString ASE_MaterialMapVTiling    =  "*UVW_V_TILING";
static const LString ASE_MaterialMapAngle      =  "*UVW_ANGLE";
static const LString ASE_MaterialMapBlur       =  "*UVW_BLUR ";
static const LString ASE_MaterialMapBlurOffset =  "*UVW_BLUR_OFFSET";
static const LString ASE_MaterialMapNoiseAmt1  =  "*UVW_NOUSE_AMT";  // missprint in 3DMax ASE exporter
static const LString ASE_MaterialMapNoiseAmt2  =  "*UVW_NOISE_AMT";
static const LString ASE_MaterialMapNoiseSize  =  "*UVW_NOISE_SIZE";
static const LString ASE_MaterialMapNoiseLevel =  "*UVW_NOISE_LEVEL";
static const LString ASE_MaterialMapNoisePhase =  "*UVW_NOISE_PHASE";
static const LString ASE_MaterialMapBitmapFilter = "*BITMAP_FILTER";
static const LString ASE_MaterialRef      =  "*MATERIAL_REF";
static const LString ASE_MotionBlur       =  "*PROP_MOTIONBLUR";
static const LString ASE_CastShadow       =  "*PROP_CASTSHADOW";
static const LString ASE_ReceiveShadow    =  "*PROP_RECVSHADOW";
static const LString ASE_NodeName         =  "*NODE_NAME";
static const LString ASE_NodeParent       =  "*NODE_PARENT";
static const LString ASE_Mesh             =  "*MESH";
static const LString ASE_MeshVertexList   =  "*MESH_VERTEX_LIST";
static const LString ASE_MeshVertex       =  "*MESH_VERTEX";
static const LString ASE_MeshTVertexList  =  "*MESH_TVERTLIST";
static const LString ASE_MeshTVertex      =  "*MESH_TVERT";
static const LString ASE_MeshFaceList     =  "*MESH_FACE_LIST";
static const LString ASE_MeshFace         =  "*MESH_FACE";
static const LString ASE_MeshTFaceList    =  "*MESH_TFACELIST";
static const LString ASE_MeshTFace        =  "*MESH_TFACE";
static const LString ASE_MeshNormals      =  "*MESH_NORMALS";
static const LString ASE_MeshFaceNormal   =  "*MESH_FACENORMAL";
static const LString ASE_MeshVertexNormal =  "*MESH_VERTEXNORMAL";
static const LString ASE_MeshMaterialID   =  "*MESH_MTLID";
static const LString ASE_MeshSmoothing    =  "*MESH_SMOOTHING";

static const LString ASE_MeshNumVertices  =  "*MESH_NUMVERTEX";
static const LString ASE_MeshNumFaces     =  "*MESH_NUMFACES";

static const LString ASE_MeshNumTVertices  =  "*MESH_NUMTVERTEX";
static const LString ASE_MeshNumTFaces     =  "*MESH_NUMTVFACES";

static const LString DEFAULT_MATERIAL_FALLBACK = "MaterialSystem/default.material"; // "MaterialSystem/default.shader";

void clASELoader::ASE_ReadMap( iIStream* FStream, sASEMap* ExtMap )
{
	guard();

	sASEMap Map;

	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapName ) )
		{
			Map.FName = LStr::GetToken( Line, 2 );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapClass ) )
		{
			Map.FClass = LStr::GetToken( Line, 2 );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapSubNo ) )
		{
			Map.FSubNo = LStr::ToInt( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapAmount ) )
		{
			Map.FAmount = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapBitmap ) )
		{
			Map.FBitmap = LStr::GetToken( Line, 2 );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapType ) )
		{
			Map.FMapType = LStr::GetToken( Line, 2 );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapUOffset ) )
		{
			Map.FUOffset = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapVOffset ) )
		{
			Map.FVOffset = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapUTiling ) )
		{
			Map.FUTiling = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapVTiling ) )
		{
			Map.FVTiling = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapAngle ) )
		{
			Map.FUVWAngle = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapBlur ) )
		{
			Map.FUVWBlur = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapBlurOffset ) )
		{
			Map.FUVWBlurOffset = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapNoiseAmt1 ) ||
		          LStr::StartsWith( Line, ASE_MaterialMapNoiseAmt2 ) )
		{
			Map.FUVWNoiseAmt = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapNoiseSize ) )
		{
			Map.FUVWNoiseSize = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapNoiseLevel ) )
		{
			Map.FUVWNoiseLevel = LStr::ToInt( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapNoisePhase ) )
		{
			Map.FUVWNoisePhase = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapBitmapFilter ) )
		{
			Map.FBitmapFilter = LStr::GetToken( Line, 2 );
		}
		else
		{
			ASE_SkipBlock( FStream, Line );
		}
	}

	*ExtMap = Map;

	unguard();
}

bool clASELoader::LoadMesh( iIStream* FStream, clMesh* Resource )
{
	guard( "%s", FStream->GetFileName().c_str() );

	FMesh = Resource;
	this->FStream = FStream;

	LString Signature = FStream->ReadLineTrimLeadSpaces();

	FATAL( !LStr::StartsWith( Signature, ASE_Signature ), "Invalid signature in .ASE file" );

	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::StartsWith( Line, ASE_Scene ) )
		{
			ASE_ReadScene( FStream );
		}
		else if ( LStr::StartsWith( Line, ASE_GeomObject ) ||
		          LStr::StartsWith( Line, ASE_Group      ) )
		{
			ASE_ReadGeomObject( FStream, 0, -1 );
		}
		else if ( LStr::StartsWith( Line, ASE_ShapeObject ) )
		{
			ASE_ReadShapeObject( FStream );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialList ) )
		{
			ASE_ReadMaterialList( FStream );
		}
		else if ( LStr::StartsWith( Line, ASE_Comment ) )
		{
			// just bypass the comment
		}
		else
		{
			ASE_SkipBlock( FStream, Line );
		}
	}

	FMesh->RecalculateLocalFromGlobal();
	FMesh->PrecalculateBoundingBoxes();

	return true;

	unguard();
}

void clASELoader::ASE_ReadScene( iIStream* FStream )
{
	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else if ( LStr::StartsWith( Line, ASE_FrameSpeed ) )
		{
			FSceneFrameSpeed = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_TicksPerFrame ) )
		{
			FSceneTicksPerFrame = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_Comment ) )
		{
			// just bypass the comment
		}
		else
		{
			ASE_SkipBlock( FStream, Line );
		}
	}
}

void clASELoader::ASE_SkipBlock( iIStream* FStream, const LString& Block )
{
	if ( !LStr::ContainsSubStr( Block, "{" ) )
	{
#ifdef ASE_HEAVY_DEBUG
		Env->Logger->Log( L_NOTICE, "Skipping ASE line: " + LStr::GetToken( Block, 1 ) );
#endif
		return;
	}

#ifdef ASE_HEAVY_DEBUG
	Env->Logger->Log( L_NOTICE, "Skipping ASE block: " + LStr::GetToken( Block, 1 ) );
#endif
	int BracketsCount = 1;

	while ( BracketsCount )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "{" ) )
		{
			BracketsCount++;
		}
		else if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			BracketsCount--;
		}
	}
}

LMatrix4 clASELoader::ASE_ReadNodeTM( iIStream* FStream )
{
	LMatrix4 TM;

	TM.IdentityMatrix();

	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else if ( LStr::StartsWith( Line, ASE_TMRow0 ) )
		{
			TM[0] = LVector4( LStr::ToFloat( LStr::GetToken( Line, 2 ) ),
			                  LStr::ToFloat( LStr::GetToken( Line, 3 ) ),
			                  LStr::ToFloat( LStr::GetToken( Line, 4 ) ), 0.0 );
		}
		else if ( LStr::StartsWith( Line, ASE_TMRow1 ) )
		{
			TM[1] = LVector4( LStr::ToFloat( LStr::GetToken( Line, 2 ) ),
			                  LStr::ToFloat( LStr::GetToken( Line, 3 ) ),
			                  LStr::ToFloat( LStr::GetToken( Line, 4 ) ), 0.0 );
		}
		else if ( LStr::StartsWith( Line, ASE_TMRow2 ) )
		{
			TM[2] = LVector4( LStr::ToFloat( LStr::GetToken( Line, 2 ) ),
			                  LStr::ToFloat( LStr::GetToken( Line, 3 ) ),
			                  LStr::ToFloat( LStr::GetToken( Line, 4 ) ), 0.0 );
		}
		else if ( LStr::StartsWith( Line, ASE_TMRow3 ) )
		{
			TM[3] = LVector4( LStr::ToFloat( LStr::GetToken( Line, 2 ) ),
			                  LStr::ToFloat( LStr::GetToken( Line, 3 ) ),
			                  LStr::ToFloat( LStr::GetToken( Line, 4 ) ), 1.0 );
		}
		else
		{
			ASE_SkipBlock( FStream, Line );
		}
	}

	return TM;
}

void clASELoader::ASE_ReadTMAnimation( iIStream* FStream )
{
	guard();

	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else if ( LStr::StartsWith( Line, ASE_ControlPosTrack ) )
		{
			ASE_ReadControlPosTrack( FStream );
		}
		else if ( LStr::StartsWith( Line, ASE_ControlRotTrack ) )
		{
			ASE_ReadControlRotTrack( FStream );
		}
		else if ( LStr::StartsWith( Line, ASE_ControlScaleTrack ) )
		{
			ASE_ReadControlScaleTrack( FStream );
		}
		else
		{
			ASE_SkipBlock( FStream, Line );
		}
	}

	unguard();
}

void clASELoader::ASE_ReadControlPosTrack( iIStream* FStream )
{
	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else
		{
			ASE_SkipBlock( FStream, Line );
		}
	}
}

void clASELoader::ASE_ReadControlRotTrack( iIStream* FStream )
{
	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else
		{
			ASE_SkipBlock( FStream, Line );
		}
	}
}

void clASELoader::ASE_ReadControlScaleTrack( iIStream* FStream )
{
	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else
		{
			ASE_SkipBlock( FStream, Line );
		}
	}
}

void clASELoader::ASE_ReadShapeObject( iIStream* FStream )
{
	guard();

	LMatrix4 NodeTM;

	NodeTM.IdentityMatrix();

	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else if ( LStr::StartsWith( Line, ASE_NodeTM ) )
		{
			NodeTM = ASE_ReadNodeTM( FStream );
		}
		else if ( LStr::StartsWith( Line, ASE_TMAnimation ) )
		{
			ASE_ReadTMAnimation( FStream );
		}
		else
		{
			ASE_SkipBlock( FStream, Line );
		}
	}

	unguard();
}

void clASELoader::ASE_ReadGeomObject( iIStream* FStream, int Depth, int ParentIdx )
{
	guard();

	int RealParentIdx = ParentIdx;

	LString GeomObjectName( "" );

	clVAMender* Mesh = NULL;

	LMatrix4 NodeTM;

	NodeTM.IdentityMatrix();

	int MaterialIndex  = -1;

	bool MotionBlur    = false;
	bool CastShadow    = true;
	bool ReceiveShadow = true;

	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else if ( LStr::StartsWith( Line, ASE_NodeTM ) )
		{
			NodeTM = ASE_ReadNodeTM( FStream );
		}
		else if ( LStr::StartsWith( Line, ASE_TMAnimation ) )
		{
			ASE_ReadTMAnimation( FStream );
		}
		else if ( LStr::StartsWith( Line, ASE_NodeName ) )
		{
			GeomObjectName = LStr::GetToken( Line, 2 );

#ifdef ASE_HEAVY_DEBUG
			Env->Logger->Log( L_DEBUG, "Reading geom object: " + GeomObjectName );
#endif
		}
		else if ( LStr::StartsWith( Line, ASE_NodeParent ) )
		{
			LString ParentName = LStr::GetToken( Line, 2 );

			int Parent = FMesh->FindRigidByName( ParentName );

			if ( Parent != -1 )
			{
				RealParentIdx = Parent;
			}
		}
		else if ( LStr::StartsWith( Line, ASE_GeomObject ) ||
		          LStr::StartsWith( Line, ASE_Group      ) )
		{
			// recursive structure
			ASE_ReadGeomObject( FStream, Depth + 1, RealParentIdx );
		}
		else if ( LStr::StartsWith( Line, ASE_ShapeObject ) )
		{
			// recursive structure
			ASE_ReadShapeObject( FStream );
		}
		else if ( LStr::StartsWith( Line, ASE_Mesh ) )
		{
			FATAL( Mesh != NULL, "Mesh was already defined. Check " + ASE_Mesh + " in ASE file" );

			Mesh = ASE_ReadMesh( FStream );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialRef ) )
		{
			MaterialIndex = LStr::ToInt( LStr::GetToken( Line, 2 ) );

			FATAL( MaterialIndex >= static_cast<int>( FMaterialList.size() ),
			       "Invalid " + ASE_MaterialRef + " value in ASE file: " + LStr::ToStr( MaterialIndex ) + " (current size: " + LStr::ToStr( FMaterialList.size() ) + ")" );

			FATAL( !Mesh, "Mesh should be defined prior to " + ASE_MaterialRef );
		}
		else if ( LStr::StartsWith( Line, ASE_MotionBlur ) )
		{
			MotionBlur = LStr::ToInt( LStr::GetToken( Line, 2 ) ) > 0;
		}
		else if ( LStr::StartsWith( Line, ASE_CastShadow ) )
		{
			CastShadow = LStr::ToInt( LStr::GetToken( Line, 2 ) ) > 0;
		}
		else if ( LStr::StartsWith( Line, ASE_ReceiveShadow ) )
		{
			ReceiveShadow = LStr::ToInt( LStr::GetToken( Line, 2 ) ) > 0;
		}
		else
		{
			ASE_SkipBlock( FStream, Line );
		}
	}

	if ( FMaterialList.size() == 0 )
	{
		// no materials were found in ASE file - use basic default
		sASEMaterial Material;

		Material.FName = DEFAULT_MATERIAL_FALLBACK;

		FMaterialList.resize( 1 );
		FMaterialList[ 0 ] = Material;

		MaterialIndex = 0;
	}

	if ( MaterialIndex == -1 )
	{
		MaterialIndex = 0;
	}


	if ( !Mesh )
	{
		return;
	}

	if ( FMaterialList[ MaterialIndex ].FSubMaterials.size() == 0 )
	{
		Mesh->ReplaceAllSubMaterials( -1 );
	}

	// create geometry with default material
#ifdef ASE_HEAVY_DEBUG
	Env->Logger->Log( L_DEBUG, "Building vertex arrays..." );
#endif

	clVertexAttribs* VA = Mesh->CreateVertexAttribs( -1 );

	if ( VA )
	{
		VA->SetMotionBlur( MotionBlur );
		VA->SetCastShadow( CastShadow );
		VA->SetReceiveShadow( ReceiveShadow );

		AddNode( RealParentIdx, VA, FMaterialList[ MaterialIndex ], GeomObjectName, NodeTM );
	}

	// create geometry with submaterials
	for ( size_t i = 0; i != FMaterialList[ MaterialIndex ].FSubMaterials.size(); ++i )
	{
		clVertexAttribs* VA = Mesh->CreateVertexAttribs( static_cast<int>( i ) );

		if ( VA )
		{
			VA->SetMotionBlur( MotionBlur );
			VA->SetCastShadow( CastShadow );
			VA->SetReceiveShadow( ReceiveShadow );

#ifdef ASE_HEAVY_DEBUG
			Env->Logger->Log( L_DEBUG, "Extracting submaterial: " + FMaterialList[ MaterialIndex ].FSubMaterials[i].FName );
#endif

			AddNode( RealParentIdx, VA, FMaterialList[ MaterialIndex ].FSubMaterials[i], GeomObjectName, NodeTM );
		}
		else
		{
//         Mesh->GetLastError() contains textual error description
		}
	}

	delete Mesh;

	unguard();
}

sMaterialDesc clASELoader::CreateMaterialFromASE( const sASEMaterial& ASEMaterial )
{
	if ( ASEMaterial.FName == "internal" )
	{
		sMaterialDesc Mtl;

		// covert to engine's material - also check local dir for textures
		Mtl.FAmbientMap  = LStringBuffer( Env->FileSystem->FindFile( ASEMaterial.FAmbientMap.FBitmap, FStream->GetVirtualFileName() ) );
		Mtl.FDiffuseMap  = LStringBuffer( Env->FileSystem->FindFile( ASEMaterial.FDiffuseMap.FBitmap, FStream->GetVirtualFileName() ) );
		Mtl.FSpecularMap = LStringBuffer( Env->FileSystem->FindFile( ASEMaterial.FSpecularMap.FBitmap, FStream->GetVirtualFileName() ) );
		Mtl.FBumpMap     = LStringBuffer( Env->FileSystem->FindFile( ASEMaterial.FBumpMap.FBitmap, FStream->GetVirtualFileName() ) );

		// convert colors
		Mtl.FProperties.FAmbientColor  = LVector4( ASEMaterial.FAmbient,  1.0f );
		Mtl.FProperties.FDiffuseColor  = LVector4( ASEMaterial.FDiffuse,  1.0f );
		Mtl.FProperties.FSpecularColor = LVector4( ASEMaterial.FSpecular, 1.0f );

		Mtl.FProperties.FTransparency  = ASEMaterial.FTransparency;

		return Mtl;
	}

	// check if we have material file with the same name as this ASE material

	// check local dir for material
	LString MatName = Env->FileSystem->FindFile( ASEMaterial.FName, FStream->GetVirtualFileName() );

	LString TheFileName( Env->FileSystem->FileExists( MatName ) ? MatName : "MaterialSystem/default.material" );

	// load material if it exists
	return Env->Resources->LoadMaterial( TheFileName )->GetMaterialDesc();

}

void clASELoader::AddNode( int ParentIdx,
                           clVertexAttribs* VA,
                           const sASEMaterial& Material,
                           const LString& ObjName,
                           const LMatrix4& GlobalTransform )
{
	guard();

	sMaterialDesc Mat = CreateMaterialFromASE( Material );

	int MaterialIdx = FMesh->AddMaterial( Mat );

	FMesh->AddVertexAttribs( ObjName, VA, GlobalTransform, ParentIdx, MaterialIdx );

	// multiply all vertices by inverse GlobalTransform
	LVector3* Vertices = VA->FVertices.GetPtr();

	LMatrix4 InvGlobalTM = GlobalTransform.GetInversed();

	for ( size_t i = 0; i != VA->FVertices.size(); i++ )
	{
		Vertices[i] = InvGlobalTM * Vertices[i];
	}

	//

	unguard();
}

clVAMender* clASELoader::ASE_ReadMesh( iIStream* FStream )
{
	guard();

#ifdef ASE_HEAVY_DEBUG
	Env->Logger->Log( L_DEBUG, "Reading mesh..." );
#endif

	clVAMender* Mender = new clVAMender(); //Env->Renderer->CreateVAMender();

	int SavedNumVNormals = -1;
	int SavedNumFNormals = -1;

	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else if ( LStr::StartsWith( Line, ASE_MeshNumFaces ) )
		{
			int NumFaces = LStr::ToInt( LStr::GetToken( Line, 2 ) );
			SavedNumFNormals = NumFaces;

			if ( NumFaces < 0 )
			{
				NumFaces = 0;
			}

			Mender->ReserveFaces( NumFaces );
		}
		else if ( LStr::StartsWith( Line, ASE_MeshNumTFaces ) )
		{
			int NumTFaces = LStr::ToInt( LStr::GetToken( Line, 2 ) );

			if ( NumTFaces < 0 )
			{
				NumTFaces = 0;
			}

			Mender->ReserveTFaces( NumTFaces );
		}
		else if ( LStr::StartsWith( Line, ASE_MeshNumVertices ) )
		{
			int NumVtx = LStr::ToInt( LStr::GetToken( Line, 2 ) );
			SavedNumVNormals = NumVtx;

			if ( NumVtx < 0 )
			{
				NumVtx = 0;
			}

			Mender->ReserveVertices( NumVtx );
		}
		else if ( LStr::StartsWith( Line, ASE_MeshNumTVertices ) )
		{
			int NumTVtx = LStr::ToInt( LStr::GetToken( Line, 2 ) );

			if ( NumTVtx < 0 )
			{
				NumTVtx = 0;
			}

			Mender->ReserveTVertices( NumTVtx );
		}
		else if ( LStr::StartsWith( Line, ASE_MeshVertexList ) )
		{
			ASE_ReadVertexList( FStream, Mender );
		}
		else if ( LStr::StartsWith( Line, ASE_MeshTVertexList ) )
		{
			ASE_ReadTVertexList( FStream, Mender );
		}
		else if ( LStr::StartsWith( Line, ASE_MeshFaceList ) )
		{
			ASE_ReadFaceList( FStream, Mender );
		}
		else if ( LStr::StartsWith( Line, ASE_MeshTFaceList ) )
		{
			ASE_ReadTFaceList( FStream, Mender );
		}
		else if ( LStr::StartsWith( Line, ASE_MeshNormals ) )
		{
			if ( SavedNumVNormals > 0 )
			{
				Mender->ReserveNormals( SavedNumVNormals );
			}

			if ( SavedNumFNormals > 0 )
			{
				Mender->ReserveFNormals( SavedNumFNormals );
			}

			ASE_ReadNormals( FStream, Mender );
		}
		else
		{
			ASE_SkipBlock( FStream, Line );
		}
	}

	return Mender;

	unguard();
}

void clASELoader::ASE_ReadVertexList( iIStream* FStream, clVAMender* Mender )
{
	guard();

#ifdef ASE_HEAVY_DEBUG
	Env->Logger->Log( L_DEBUG, "Reading vertex list..." );
#endif

	char Keyword[32];

	int Index;
	float X, Y, Z;

	while ( !FStream->Eof() )
	{
		LString Line( FStream->ReadLineTrimLeadSpaces() );

		if ( LStr::StartsWith( Line, ASE_MeshVertex ) )
		{
			sscanf( Line.c_str(), "%s %d %f %f %f", Keyword, &Index, &X, &Y, &Z );

			Mender->EmitVertex( Index, LVector3( X, Y, Z ), -1, -1 );
		}
		else if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else
		{
			FATAL_MSG( "Unexpected token in " + ASE_MeshVertexList + " : " + Line );
		}
	}

	unguard();
}

void clASELoader::ASE_ReadTVertexList( iIStream* FStream, clVAMender* Mender )
{
	guard();

#ifdef ASE_HEAVY_DEBUG
	Env->Logger->Log( L_DEBUG, "Reading Tvertex list..." );
#endif

	char Keyword[32];

	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else if ( LStr::StartsWith( Line, ASE_MeshTVertex ) )
		{
			int Index;
			float U, V, W;

			sscanf( Line.c_str(), "%s %d %f %f %f", Keyword, &Index, &U, &V, &W );

			Mender->EmitTextureVertex( Index, LVector3( U, 1.0f - V, W ) );
		}
		else
		{
			FATAL_MSG( "Unexpected token in " + ASE_MeshTVertexList + " : " + Line );
		}
	}

	unguard();
}

void clASELoader::ASE_ReadFaceList( iIStream* FStream, clVAMender* Mender )
{
	guard();

#ifdef ASE_HEAVY_DEBUG
	Env->Logger->Log( L_DEBUG, "Reading face list..." );
#endif

	LStr::clStringsVector Tokens( 19 );

	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else if ( LStr::StartsWith( Line, ASE_MeshFace ) )
		{
			LStr::FastSplitLine( 1, 18, Line, Tokens, true );

			int Index  = LStr::ToInt( Tokens[2].substr( 0, Tokens[2].length() - 1 ) );
			int A      = LStr::ToInt( Tokens[4] );
			int B      = LStr::ToInt( Tokens[6] );
			int C      = LStr::ToInt( Tokens[8] );

			LString Token = Tokens[16];

			int SubMTLTokenIndex = ( Token == ASE_MeshMaterialID ) ? 17 : 18;

			LString SubMTL_ID = Tokens[SubMTLTokenIndex];

			int SubMTL = SubMTL_ID.empty() ? -1 : LStr::ToInt( SubMTL_ID );

			int SmoothingGroup = LStr::ToInt( Tokens[SubMTLTokenIndex-2] );
			/*
			         LString IndexS = LStr::GetToken(Line, 2);

			         int Index  = LStr::ToInt( IndexS.substr(0, IndexS.length()-1) );
			         int A      = LStr::ToInt( LStr::GetToken(Line, 4) );
			         int B      = LStr::ToInt( LStr::GetToken(Line, 6) );
			         int C      = LStr::ToInt( LStr::GetToken(Line, 8) );

			         LString Token = LStr::GetToken(Line, 16);

			         int SubMTLTokenIndex = ( Token == ASE_MeshMaterialID ) ? 17 : 18;

			         LString SubMTL_ID = LStr::GetToken( Line, SubMTLTokenIndex );

			         int SubMTL = SubMTL_ID.empty() ? -1 : LStr::ToInt( SubMTL_ID );

			         int SmoothingGroup = LStr::ToInt( LStr::GetToken( Line, SubMTLTokenIndex-2 ) );
			*/

			/*
			         char Keyword[32];

			         int A, B, C;
			         int Index;
			         int AB, BC, CA;
			         int SmoothingGroup;
			         int SubMTL;

			         sscanf( Line.c_str(), "%s %d: A: %d  B: %d  C: %d AB: %d BC: %d CA: %d *MESH_SMOOTHING %d",
			                 Keyword, &Index, &A, &B, &C, &AB, &BC, &CA, &SmoothingGroup);

			         char* pBuf = strrchr(Keyword, '*');
			         sscanf(pBuf,"*MESH_MTLID %d", &SubMTL);
			*/
#ifdef ASE_HEAVY_DEBUG
			Env->Logger->Log( L_DEBUG, "Smoothing group:" + LStr::ToStr( SmoothingGroup ) );
#endif

			Mender->EmitFace( Index, A, B, C, SubMTL, SmoothingGroup );
		}
		else
		{
			FATAL_MSG( "Unexpected token in " + ASE_MeshFaceList + " : " + Line );
		}
	}

	unguard();
}

void clASELoader::ASE_ReadTFaceList( iIStream* FStream, clVAMender* Mender )
{
	guard();

#ifdef ASE_HEAVY_DEBUG
	Env->Logger->Log( L_DEBUG, "Reading Tface list..." );
#endif

	LStr::clStringsVector Tokens( 6 );

	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else if ( LStr::StartsWith( Line, ASE_MeshTFace ) )
		{
			LStr::FastSplitLine( 1, 5, Line, Tokens, true );

			int Index  = LStr::ToInt( Tokens[2] );
			int A      = LStr::ToInt( Tokens[3] );
			int B      = LStr::ToInt( Tokens[4] );
			int C      = LStr::ToInt( Tokens[5] );

			Mender->EmitTextureFace( Index, A, B, C );
		}
		else
		{
			FATAL_MSG( "Unexpected token in " + ASE_MeshTFaceList + " : " + Line );
		}
	}

	unguard();
}

void clASELoader::ASE_ReadNormals( iIStream* FStream, clVAMender* Mender )
{
	guard();

#ifdef ASE_HEAVY_DEBUG
	Env->Logger->Log( L_DEBUG, "Reading normals..." );
#endif

	int CurrentFace = -1;
	int VertexIndex = 0;
	sFaceNormal FaceNormal;

	LStr::clStringsVector Tokens( 6 );

	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else if ( LStr::StartsWith( Line, ASE_MeshFaceNormal ) )
		{
			if ( CurrentFace > -1 )
			{
				Mender->EmitFaceNormal( CurrentFace, FaceNormal );
			}

			LStr::FastSplitLine( 1, 5, Line, Tokens, true );

			CurrentFace = LStr::ToInt( Tokens[2] );
			float X     = LStr::ToFloat( Tokens[3] );
			float Y     = LStr::ToFloat( Tokens[4] );
			float Z     = LStr::ToFloat( Tokens[5] );

			VertexIndex = 0;

			FaceNormal.FFaceNormal = LVector3( X, Y, Z );
		}
		else if ( LStr::StartsWith( Line, ASE_MeshVertexNormal ) )
		{
			LStr::FastSplitLine( 1, 5, Line, Tokens, true );

			int   Index = LStr::ToInt( Tokens[2] );
			float X     = LStr::ToFloat( Tokens[3] );
			float Y     = LStr::ToFloat( Tokens[4] );
			float Z     = LStr::ToFloat( Tokens[5] );

			LVector3 Vec( X, Y, Z );

			Mender->EmitNormal( Index, Vec );

			FaceNormal.FVertexNormal[ VertexIndex++ ] = Vec;
		}
		else
		{
			FATAL_MSG( "Unexpected token in " + ASE_MeshNormals + " : " + Line );
		}
	}

	if ( CurrentFace > -1 )
	{
		Mender->EmitFaceNormal( CurrentFace, FaceNormal );
	}

	unguard();
}

void clASELoader::ASE_ReadMaterialList( iIStream* FStream )
{
	guard();

	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialCount ) )
		{
			int MaterialCount = LStr::ToInt( LStr::GetToken( Line, 2 ) );

			FMaterialList.resize( MaterialCount );
		}
		else if ( LStr::StartsWith( Line, ASE_Material ) )
		{
			int MaterialIndex = LStr::ToInt( LStr::GetToken( Line, 2 ) );

			//ASE_ReadMaterial( MaterialIndex, -1, 0 );
			FMaterialList[ MaterialIndex ] = ASE_ReadMaterial( FStream, 0 );
		}
		else
		{
			FATAL_MSG( "Unexpected token in " + ASE_MaterialList + " : " + Line );
		}
	}

	unguard();
}

//void clASELoader::ASE_ReadMaterial(int MaterialIndex, int SubMaterialIndex, int SubMaterialNesting)
sASEMaterial clASELoader::ASE_ReadMaterial( iIStream* FStream, int SubMaterialNesting )
{
	guard();

	sASEMaterial Material;

	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLineTrimLeadSpaces();

		if ( LStr::ContainsSubStr( Line, "}" ) )
		{
			break;
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialName ) )
		{
//         size_t TotalMTLSlots = FMaterialList.size();
			//FATAL( MaterialIndex >= static_cast<int>( TotalMTLSlots ),
			//"Number of reserved material slots exceeded. Check for "+ASE_MaterialCount+" value in ASE file: " + FMesh->GetFileName() + "(reserved: "+LStr::ToStr( TotalMTLSlots ) + ", requested: "+LStr::ToStr(MaterialIndex)+")"  );

			Material.FName = LStr::GetToken( Line, 2 );

#ifdef ASE_HEAVY_DEBUG
			Env->Logger->Log( L_DEBUG, "Reading material: " + Material.FName );
#endif
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialClass ) )
		{
			Material.FClass = LStr::GetToken( Line, 2 );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialAmbient ) )
		{
			Material.FAmbient = LVector3( LStr::ToFloat( LStr::GetToken( Line, 2 ) ),
			                              LStr::ToFloat( LStr::GetToken( Line, 3 ) ),
			                              LStr::ToFloat( LStr::GetToken( Line, 4 ) ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialDiffuse ) )
		{
			Material.FDiffuse = LVector3( LStr::ToFloat( LStr::GetToken( Line, 2 ) ),
			                              LStr::ToFloat( LStr::GetToken( Line, 3 ) ),
			                              LStr::ToFloat( LStr::GetToken( Line, 4 ) ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialSpecular ) )
		{
			Material.FSpecular = LVector3( LStr::ToFloat( LStr::GetToken( Line, 2 ) ),
			                               LStr::ToFloat( LStr::GetToken( Line, 3 ) ),
			                               LStr::ToFloat( LStr::GetToken( Line, 4 ) ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialShine ) )
		{
			Material.FShine = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialShineStrength ) )
		{
			Material.FShineStrength = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialTransparency ) )
		{
			Material.FTransparency = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialWireSize ) )
		{
			Material.FWireSize = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialShading ) )
		{
			Material.FShading = LStr::GetToken( Line, 2 );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialXPFalloff ) )
		{
			Material.FXPFallOff = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialSelfIllum ) )
		{
			Material.FSelfIllum = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialFalloff ) )
		{
			Material.FFallOff = LStr::GetToken( Line, 2 );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialXPType ) )
		{
			Material.FXPType = LStr::GetToken( Line, 2 );
		}
		else if ( LStr::StartsWith( Line, ASE_SubMaterialCount ) )
		{
			int SubMaterialCount = LStr::ToInt( LStr::GetToken( Line, 2 ) );

			if ( Material.FSubMaterials.size() == 0 )
			{
				// don't redefine submaterials
				Material.FSubMaterials.resize( SubMaterialCount );
			}
		}
		else if ( LStr::StartsWith( Line, ASE_SubMaterial ) )
		{
			int SubMaterialIndex = LStr::ToInt( LStr::GetToken( Line, 2 ) );

#ifdef ASE_HEAVY_DEBUG
			Env->Logger->Log( L_NOTICE, "Found submaterial:" + LStr::ToStr( SubMaterialIndex ) );
#endif

//         if ( SubMaterialNesting < 1 ) // don't read sub-submaterials...
			Material.FSubMaterials[ SubMaterialIndex ] = ASE_ReadMaterial( FStream, SubMaterialNesting + 1 );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapAmbient ) )
		{
			ASE_ReadMap( FStream, &Material.FAmbientMap );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapDiffuse ) )
		{
			ASE_ReadMap( FStream, &Material.FDiffuseMap );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapSpecular ) )
		{
			ASE_ReadMap( FStream, &Material.FSpecularMap );
		}
		else if ( LStr::StartsWith( Line, ASE_MaterialMapBump ) )
		{
			ASE_ReadMap( FStream, &Material.FBumpMap );
		}
		else
		{
			ASE_SkipBlock( FStream, Line );
		}
	}

	/*
	   // add to list
	   if ( SubMaterialIndex == -1 )
	   {
	      Env->Logger->Log( L_NOTICE, "Material:" + LString( Material.FName ) );

	      FMaterialList[ MaterialIndex ].FDefaultMeterial = Material;
	   }
	   else
	   {
	      Env->Logger->Log( L_NOTICE, "SubMaterial:" + LString( Material.FName ) );

	      size_t TotalMTLSlots = FMaterialList[ MaterialIndex ].FSubMaterials.size();

	      FATAL( SubMaterialIndex >= static_cast<int>( TotalMTLSlots ),
	             "Number of reserved submaterial slots exceeded. Check for "+ASE_SubMaterialCount+" value in ASE file. " + "(reserved: "+LStr::ToStr( TotalMTLSlots ) + ", requested: "+LStr::ToStr(SubMaterialIndex)+")" );

	      FMaterialList[ MaterialIndex ].FSubMaterials[ SubMaterialIndex ] = Material;
	   }*/
	return Material;

	unguard();
}

/*
 * 17/11/2010
     ASE_ReadScene()
     ASE_ReadShapeObject()
     ASE_ReadTMAnimation()
     ASE_ReadControlPosTrack()
     ASE_ReadControlRotTrack()
     ASE_ReadControlScaleTrack()
 * 12/02/2010
     No dependancy on scenegraph
 * 20/04/2009
     Material maps and colors loading
 * 17/04/2008
     Material parameters parsing
 * 08/11/2007
     Chenged hierarchy of the created scenegraph
 * 04/11/2007
     Submaterials support
 * 24/04/2007
     Special mode for loading meshes with invalid shaders
 * 04/03/2006
     Fixed EOF expection handling in Load()
 * 13/03/2005
     Loading of node's names
 * 12/03/2005
     Loading of materials
 * 07/03/2005
     Initial working version - still has a long way to go
 * 04/03/2005
     It's here
*/
