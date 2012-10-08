/**
 * \file ResourcesManager.cpp
 * \brief Resources manager
 * \version 0.5.93
 * \date 30/09/2010
 * \author Sergey Kosarevsky, 2004-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "iResource.h"
#include "ResourcesManager.h"

#include "Environment.h"
#include "Engine.h"
#include "Core/Logger.h"
#include "Core/Linker.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "Audio/Audio.h"
#include "Renderer/iShaderProgram.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/VolumeRenderer.h"
#include "Images/Image.h"
#include "Geometry/VertexAttribs.h"
#include "Geometry/GeomServ.h"
#include "Geometry/Geom.h"
#include "Geometry/Surfaces.h"
#include "Math/LMathStrings.h"
#include "Utils/LBlob.h"

void clLoaderThread::Run()
{
	Env->Logger->SetCurrentThreadName( "Loader" );

	guard();

	while ( !IsPendingExit() )
	{
		if ( FLoadOps.empty() )
		{
			Env->ReleaseTimeslice( 10 );

			continue;
		}

		// 1. Extract loading command from dequeue
		iLoadOp* Op = ExtractLoadOp();

		// 2. Load file and switch default & current object in resource
		Op->Load();

		Env->Logger->Log( L_PARANOID, "Async loaded resource: " + Op->GetResource()->GetFileName() );

		delete( Op );
	}

	Env->Logger->Log( L_NOTICE, "Loader thread exited" );

	unguard();
}

iResource* clLoaderThread::clLoadOp_Geom::GetResource() const
{
	return FResource;
}

void clLoaderThread::clLoadOp_Geom::Load()
{
	// this Env is used in guard()
	sEnvironment* Env = this->FResource->Env;

	guard( "%s", this->FResource->GetFileName().c_str() );

	LString FileName = this->FResource->GetFileName();

	bool CacheEnabled = Env->Console->GetVarDefault( "Cache.MeshesCacheEnabled", "false" )->GetBool();

	this->FResource->Load( FileName, CacheEnabled );

	this->FResource->SetAsyncLoadComplete( true );

	unguard();
}

iResource* clLoaderThread::clLoadOp_Image::GetResource() const
{
	return FResource;
}

void clLoaderThread::clLoadOp_Image::Load()
{
	// this Env is used in guard()
	sEnvironment* Env = this->FResource->Env;

	guard( "%s", this->FResource->GetFileName().c_str() );

	LString FileName = this->FResource->GetFileName();

	clBitmap* Bitmap = clBitmap::CreateEmptyBitmap( Env );
	sBitmapParams* Params = &( this->FResource->GetDefaultBitmap()->FBitmapParams );

	Env->Logger->Log( L_DEBUG, "Reallocating image data for new bitmap..." );

	Bitmap->ReallocImageData( Params );

	switch  ( Bitmap->GetTextureType() )
	{
		case L_TEXTURE_2D:
			FATAL( Bitmap->Load2DImage( Env, FileName ) == false, "Unable to load 2D texture: " + FileName );
			break;
		case L_TEXTURE_3D:

			if ( Params->FAutoGradient )
			{
				FATAL( Bitmap->Load3DImageGradients( Env, FileName ) == false, "Unable to load Gradients 3D texture: " + FileName );
			}
			else if ( Params->FAutoESL )
			{
				FATAL( Bitmap->Load3DImageESL( Env, FileName ) == false, "Unable to load ESL 3D texture: " + FileName );
			}
			else
			{
				FATAL( Bitmap->Load3DImage( Env, FileName ) == false, "Unable to load 3D texture: " + FileName );
			}

			break;
		case L_TEXTURE_CUBE:
			FATAL( Bitmap->LoadCubeImage( Env, FileName ) == false, "Unable to load CUBE texture: " + FileName );
			break;
		default:
			FATAL_MSG( "Unknown texture type" );
	}

	Env->Logger->Log( L_DEBUG, "Updating bitmap in resource..." );

	this->FResource->SetBitmap( Bitmap );
	this->FResource->SetAsyncLoadComplete( true );

	unguard();
}

void clLoaderThread::Event_STARTLOADING( LEvent Event, const LEventArgs& Args )
{
}

void clLoaderThread::Event_STOPLOADING( LEvent Event, const LEventArgs& Args )
{
}

clLoaderThread::iLoadOp* clLoaderThread::ExtractLoadOp()
{
	LMutex Lock( &FMutex );

	iLoadOp* Op = FLoadOps.front();

	FLoadOps.pop_front();

	Env->Logger->LogP( L_DEBUG, "Size of loading queue: %i", FLoadOps.size() );

	return Op;
}

void clLoaderThread::EnqueueLoading( iLoadOp* LoadOp )
{
	LMutex Lock( &FMutex );

	FLoadOps.push_back( LoadOp );
}

clResourcesManager::clResourcesManager():
	FResourcesGraph( 0 ),
	FTime_Waveforms( 0 ),
	FTime_ShaderPrograms( 0 ),
	FTime_Images( 0 ),
	FTime_Materials( 0 ),
	FTime_Meshes( 0 ),
	FTime_RenderStates( 0 ),
	FTime_Animations( 0 )
{
}

void clResourcesManager::AfterConstruction()
{
	FLockedLoading = Env->Console->GetVarDefault( "Resources.LockedLoading", "false" );

	Env->Console->RegisterCommand( "PreCache",         Utils::Bind( &clResourcesManager::PreCacheC,   this ) );
	Env->Console->RegisterCommand( "ReCache",          Utils::Bind( &clResourcesManager::ReCacheC,    this ) );
	Env->Console->RegisterCommand( "ReCacheNew",       Utils::Bind( &clResourcesManager::ReCacheNewC, this ) );

	FLoaderThread = new clLoaderThread();
	FLoaderThread->Start( Env, iThread::Priority_Low );
}

clBlob* clResourcesManager::CreateEmptyBlob() const
{
	return Env->Linker->Instantiate( "clBlob" );
}

clBlob* clResourcesManager::CreateBlob( size_t Size ) const
{
	clBlob* B = Env->Linker->Instantiate( "clBlob" );

	B->SetSize( Size );

	return B;
}

clBlob* clResourcesManager::CreateManagedBlob( void* DataBlock, size_t Size ) const
{
	clBlob* B = Env->Linker->Instantiate( "clBlob" );

	B->SetExternalData( DataBlock, Size );

	return B;
}

void clResourcesManager::StopLoaderThread()
{
	guard();

	Env->Logger->Log( L_LOG, "Stopping loader thread" );

	FLoaderThread->Exit( false );

	unguard();
}

clResourcesManager::~clResourcesManager()
{
	FLoaderThread->Exit( true );
	delete( FLoaderThread );

	Env->Logger->Log( L_LOG, "Resources total loading time:" );
	Env->Logger->Log( L_LOG, "         Waveforms: " + LStr::ToStr( FTime_Waveforms, 1 ) );
	Env->Logger->Log( L_LOG, "   Shader programs: " + LStr::ToStr( FTime_ShaderPrograms, 1 ) );
	Env->Logger->Log( L_LOG, "            Images: " + LStr::ToStr( FTime_Images, 1 ) );
	Env->Logger->Log( L_LOG, "         Materials: " + LStr::ToStr( FTime_Materials, 1 ) );
	Env->Logger->Log( L_LOG, "            Meshes: " + LStr::ToStr( FTime_Meshes, 1 ) );
	Env->Logger->Log( L_LOG, "     Render states: " + LStr::ToStr( FTime_RenderStates, 1 ) );
	Env->Logger->Log( L_LOG, "        Animations: " + LStr::ToStr( FTime_Animations, 1 ) );

	Env->Console->UnRegisterCommand( "PreCache",         Utils::Bind( &clResourcesManager::PreCacheC,   this ) );
	Env->Console->UnRegisterCommand( "ReCache",          Utils::Bind( &clResourcesManager::ReCacheC,    this ) );
	Env->Console->UnRegisterCommand( "ReCacheNew",       Utils::Bind( &clResourcesManager::ReCacheNewC, this ) );

	PurgeAll();
}

iWaveform* clResourcesManager::LoadWaveform( const LString& FileName )
{
	double StartTime = Env->GetSeconds();

	iWaveform* Resource = FindResourceFile<iWaveform>( FileName );

	if ( !Resource )
	{
		Resource = Env->Audio->CreateWaveform();
		Resource->Load( FileName, false );
	}

	FTime_Waveforms += Env->GetSeconds() - StartTime;

	return Resource;
}

iShaderProgram* clResourcesManager::LoadSP( const LString& FileName, const LString& DefinesList )
{
	guard( "%s,%s", FileName.c_str(), DefinesList.c_str() );

	if ( !Env->Renderer )
	{
		return NULL;
	}

	double StartTime = Env->GetSeconds();

	iShaderProgram* Resource = NULL;

	clResourcesGraph::const_iterator i;

	// manual iteration
	for ( i = FResourcesGraph.begin(); i != FResourcesGraph.end(); ++i )
	{
		if ( iShaderProgram* SP = dynamic_cast<iShaderProgram*>( *i ) )
		{
			bool SameName =  SP->GetFileName() == FileName;
			bool SameDefs =  SP->GetDefinesList() == DefinesList;

			if ( SameName && SameDefs )
			{
				Resource = SP;
				break;
			}
		}
	}

	if ( !Resource )
	{
		bool CacheEnabled = Env->Console->GetVarDefault( "Cache.ShaderProgramsCacheEnabled", "false" )->GetBool();

		Resource = Env->Renderer->CreateShaderProgram();

		Resource->SetDefinesList( DefinesList );
		Resource->Load( FileName, CacheEnabled );
	}

	FTime_ShaderPrograms += Env->GetSeconds() - StartTime;

	return Resource;

	unguard();
}

clImage* clResourcesManager::CreateImage() const
{
	return Env->Linker->Instantiate( "clImage" );
}

clImage* clResourcesManager::CreateImageFromBitmap( clBitmap* Bitmap ) const
{
	clImage* Res = CreateImage();

	Res->SetBitmap( Bitmap );

	return Res;
}

clImage* clResourcesManager::LoadImg( const LString& FileName, LTextureType TextureType )
{
	return LoadImgHints( FileName, TextureType, 0, 0, 0, false, false );
}

clImage* clResourcesManager::LoadImgSync( const LString& FileName, LTextureType TextureType )
{
	clImage* Image = LoadImgHints( FileName, TextureType, 0, 0, 0, false, false );

	Image->WaitLoad();

	return Image;
}

clImage* clResourcesManager::LoadImgHints( const LString& FileName,
                                           LTextureType TextureType,
                                           int HintSizeX, int HintSizeY, int HintSizeZ,
                                           bool AutoGradient,
                                           bool AutoESL )
{
	guard( "%s", FileName.c_str() );

	double StartTime = Env->GetSeconds();

	clImage* Resource = FindResourceFile<clImage>( FileName );

//   if ( Resource && Resource->IsAutoGradient() != AutoGradient ) Resource = NULL;
//   if ( Resource && Resource->IsAutoESL()      != AutoESL      ) Resource = NULL;

	if ( !Resource )
	{
//      bool CacheEnabled = Env->Console->GetVarDefault( "Cache.TexturesCacheEnabled", "false" )->GetBool();

		/// clImage::Load() -> use linker for xlml, or just simple file
//      clImage* OutImage = Env->Linker->LoadFromXLML(FileName)

		Resource = CreateImage();
		Resource->SetAsyncLoadComplete( false );
		Resource->SetFileName( FileName );

		sBitmapParams Params = sBitmapParams( Env, HintSizeX, HintSizeY, HintSizeZ, L_BITMAP_GRAYSCALE8, TextureType );
		Params.FAutoGradient = AutoGradient;
		Params.FAutoESL      = AutoESL;

		Resource->GetDefaultBitmap()->ReallocImageData( &Params );

		EnqueueLoading( new clLoaderThread::clLoadOp_Image( Resource ) );

		if ( FLockedLoading->GetBool() )
		{
			Resource->WaitLoad();
		}
	}

	FTime_Images += Env->GetSeconds() - StartTime;

	return Resource;

	unguard();
}

clMaterial* clResourcesManager::CreateMaterial() const
{
	return Env->Linker->Instantiate( "clMaterial" );
}

clMaterial* clResourcesManager::CreateColoredMaterial( const LVector4& Color, bool CastShadow, bool ReceiveShadow, float Transparency ) const
{
	clMaterial* Mat = Env->Linker->Instantiate( "clMaterial" );
	Mat->SetPropertyValue( "DiffuseColor", LStr::Vec4ToStr( Color ) );
	Mat->FMaterialDesc.FCastShadow = CastShadow;
	Mat->FMaterialDesc.FReceiveShadow = ReceiveShadow;
	Mat->FMaterialDesc.FAlphaTransparency = Transparency > 0.0f;
	Mat->SetPropertyValue( "Transparency", LStr::ToStr( Transparency ) );

	return Mat;
}

clMaterial* clResourcesManager::LoadMaterial( const LString& FileName )
{
	double StartTime = Env->GetSeconds();

	clMaterial* Resource = FindResourceFile<clMaterial>( FileName );

	if ( !Resource )
	{
		Resource = Env->Linker->Instantiate( "clMaterial" );
		Resource->Load( FileName, false );
	}

	FTime_Materials += Env->GetSeconds() - StartTime;

	return Resource;
}

clGeom* clResourcesManager::MergeGeoms( const std::vector<clGeom*>& Geoms, const std::vector<LMatrix4>& Transforms ) const
{
	clGeom* CombinedGeom = CreateGeom();

//	CombinedGeom->GetCurrentMesh()->DisposeObject();

	std::vector<clMesh*> MeshList( Geoms.size() );

	for ( size_t j = 0 ; j < Geoms.size() ; j++ )
	{
		MeshList[j] = Geoms[j]->GetCurrentMesh();
	}

	clMesh* NewMesh = MergeMeshes( MeshList, Transforms );

	CombinedGeom->SetMesh( NewMesh );

	return CombinedGeom;
}

clMesh* clResourcesManager::MergeMeshes( const std::vector<clMesh*>& Meshes, const std::vector<LMatrix4>& Transforms ) const
{
	size_t Sz = Meshes.size();

	// No match between transforms and size
	if ( Sz != Transforms.size() ) { return NULL; }

	clMesh* CombinedMesh = Env->Resources->CreateMesh();

	for ( size_t j = 0 ; j < Sz ; j++ )
	{
		clMesh* Tmp = Meshes[j]->CloneMesh();
		Tmp->Transform( Transforms[j] );
		CombinedMesh->MergeWith( Tmp );
		CombinedMesh->MergeSimilarRigids();
		delete Tmp;
	}

	/// Now take materials from the first mesh
	if ( Meshes.size() > 0 )
	{
		CombinedMesh->CopyMaterialsFrom( Meshes[0] );
	}

	return CombinedMesh;
}

clMesh* clResourcesManager::CreateMesh() const
{
	return Env->Linker->Instantiate( "clMesh" );
}

clGeom* clResourcesManager::CreateGeom() const
{
	return Env->Linker->Instantiate( "clGeom" );
}

clUVSurfaceGenerator* clResourcesManager::CreateSurfaceGenerator( const LString& GeneratorName ) const
{
	return Env->Linker->Instantiate( GeneratorName );
}

clGeom* clResourcesManager::CreateGeomForNamedVA( const LString& VAName, clVertexAttribs* VA ) const
{
	clGeom* Geom = CreateGeom();

	Geom->GetCurrentMesh()->AddVertexAttribs( VAName, VA, LMatrix4::Identity(), -1, -1 );

	return Geom;
}

clGeom* clResourcesManager::CreateSphere( float Radius, const LVector3& Center ) const
{
	// create sphere vertices
	const int TESS_LEVEL = 6;

	// add vertices to the mesh
	return CreateGeomForNamedVA( "Sphere", clGeomServ::CreateSphere( Radius, Center, TESS_LEVEL ) );
}

clGeom* clResourcesManager::CreateBox( const LVector3& Min, const LVector3& Max ) const
{
	return CreateGeomForNamedVA( "Box", clGeomServ::CreateAxisAlignedBox( Min, Max, false ) );
}

clGeom* clResourcesManager::CreateTetrahedron( float Radius, LVector3 Center ) const
{
	return CreateGeomForNamedVA( "Tetrahedron", clGeomServ::CreateTetrahedron( Radius, Center ) );
}

clGeom* clResourcesManager::CreateIcosahedron( float Radius, LVector3 Center ) const
{
	return CreateGeomForNamedVA( "Icosahedron", clGeomServ::CreateIcosahedron( Radius, Center ) );
}

clGeom* clResourcesManager::CreateGear( float BaseRadius, float RootRadius, float OuterRadius, float Width, int TotalTeeth, int InvoluteSteps ) const
{
	return CreateGeomForNamedVA( "Gear", clGeomServ::CreateGear( BaseRadius, RootRadius, OuterRadius, Width, TotalTeeth, InvoluteSteps ) );
}

clGeom* clResourcesManager::CreateISOGear( float Module, int NumberOfTeeth, float ProfileShift, float Width ) const
{
	return CreateGeomForNamedVA( "ISOGear", clGeomServ::CreateISOGear( Module, NumberOfTeeth, ProfileShift, Width ) );
}

clGeom* clResourcesManager::CreatePlane( float X1, float Y1, float X2, float Y2, float Z, int Subdiv ) const
{
	guard();

	// allocate clGeom container
	clGeom* Geom = CreateGeom();

	// add vertices to the mesh
	Geom->GetCurrentMesh()->AddVertexAttribs( "Plane", clGeomServ::CreateRect2D( X1, Y1, X2, Y2, Z, false, Subdiv ), LMatrix4::Identity(), -1, -1 );

	return Geom;

	unguard();
}

void clResourcesManager::EnqueueLoading( clLoaderThread::iLoadOp* LoadOp )
{
	FLoaderThread->EnqueueLoading( LoadOp );
}

clGeom* clResourcesManager::LoadGeom( const LString& FileName )
{
	double StartTime = Env->GetSeconds();

	clGeom* Resource = FindResourceFile<clGeom>( FileName );

	if ( !Resource )
	{
		Resource = CreateGeom();
		Resource->SetAsyncLoadComplete( false );
		Resource->SetFileName( FileName );

		EnqueueLoading( new clLoaderThread::clLoadOp_Geom( Resource ) );

		if ( FLockedLoading->GetBool() )
		{
			Resource->WaitLoad();
		}
	}

	FTime_Meshes += Env->GetSeconds() - StartTime;

	return Resource;
}

clAnimation* clResourcesManager::CreateAnimation() const
{
	return Env->Linker->Instantiate( "clAnimation" );
}

clAnimation* clResourcesManager::LoadAnimation( const LString& FileName )
{
	double StartTime = Env->GetSeconds();

	clAnimation* Resource = FindResourceFile<clAnimation>( FileName );

	if ( !Resource )
	{
		bool CacheEnabled = Env->Console->GetVarDefault( "Cache.AnimationsCacheEnabled", "false" )->GetBool();

		Resource = CreateAnimation();
		Resource->Load( FileName, CacheEnabled );
	}

	FTime_Animations += Env->GetSeconds() - StartTime;

	return Resource;
}

clRenderState* clResourcesManager::CreateShader() const
{
	return Env->Linker->Instantiate( "clRenderState" );
}

clRenderState* clResourcesManager::CreateCustomShader( const LString& ShaderProgramName,
                                                       const LString& DefinesList ) const
{
	clRenderState* Shader = CreateShader();

	Shader->FDepthTest = false;
	Shader->FDepthWrites = false;

	Shader->SetShaderProgram( Env->Resources->LoadSP( ShaderProgramName, DefinesList ) );

	return Shader;
}

clRenderState* clResourcesManager::LoadShader( const LString& FileName )
{
	double StartTime = Env->GetSeconds();

	clRenderState* Resource = FindResourceFile<clRenderState>( FileName );

	if ( !Resource )
	{
		bool CacheEnabled = Env->Console->GetVarDefault( "Cache.ShadersCacheEnabled", "false" )->GetBool();

		Resource = CreateShader();
		Resource->Load( FileName, CacheEnabled );
	}

	FTime_RenderStates += Env->GetSeconds() - StartTime;

	return Resource;
}

clLVLibVolume* clResourcesManager::CreateVolume() const
{
	return Env->Linker->Instantiate( "clLVLibVolume" );
}

void clResourcesManager::PurgeAll()
{
	guard();

	//Env->Logger->Log( L_NOTICE, "Purging resources..." );

	clResourcesGraph LocalGraph( FResourcesGraph );

	for ( clResourcesGraph::iterator i = LocalGraph.begin(); i != LocalGraph.end(); ++i )
	{
		iResource* Res = *i;

		Env->Logger->Log( L_PARANOID, "Purging resource: " + Res->NativeClassName() );

		delete( Res );
	}

	if ( !FResourcesGraph.empty() )
	{
		Env->Logger->Log( L_WARNING, "Resources graph is not empty!" );
	}

	/*
	   while ( !FResourcesGraph.empty() )
	   {
	      iResource* Res = FResourcesGraph.back();

	      Env->Logger->Log( L_NOTICE, "Purging resource: " + Res->NativeClassName() );

	      delete( Res );
	   }
	*/
	unguard();
}

iResource* clResourcesManager::FindResourceInGraph( clResourcesGraph& Graph,
                                                    const LString& FileName )
{
	LString ConvFileName = LStr::GetUpper( FileName );

	for ( clResourcesGraph::iterator i = Graph.begin(); i != Graph.end(); ++i )
	{
		if ( LStr::GetUpper( ( *i )->GetFileName() ) == ConvFileName )
		{
			// simple LRU cache
			std::swap( *Graph.begin(), *i );

			return *Graph.begin();
		}
	}

	return NULL;
}

/**
   This will convert requested filename with full path into a single file name
    e.g.  c:\\data\\textures\\texture1.tga
          c~_data_textures_texture1.tga
**/
LString clResourcesManager::ConvertName( const LString& FileName )
{
	LString ConvName;

	for ( size_t i = 0; i < FileName.length(); ++i )
	{
		switch ( FileName[i] )
		{
			case '\\':
			case '/':
			{
				ConvName.push_back( '_' );
				continue;
			}
			case ':':
			{
				ConvName.push_back( '~' );
				continue;
			}
			case ' ':
			{
				ConvName.push_back( '#' );
				continue;
			}
		}

		ConvName.push_back( FileName[i] );
	}

	return ConvName;
}

void clResourcesManager::PreCacheC( const LString& Param )
{
	if ( Param.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: PRECACHE <shader or texture file name>" );
		return;
	}

	Env->Console->Display( "Precaching shader... " + Param );

	Env->Resources->LoadShader( Param );
}

void clResourcesManager::ReCacheC( const LString& Param )
{
	guard( "%s", Param.c_str() );

	if ( Param.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: RECACHE <file name>" );
		return;
	}

	iResource* Resource = Env->Resources->FindResourceInGraph( FResourcesGraph, Param );

	if ( !Resource )
	{
		Env->Console->DisplayError( "Unable to find resource: " + Param );
		return;
	}

	Env->Console->DisplayInfoTip( Resource->GetFileName() );
	Env->Console->Display( "Recaching " + Resource->ClassName() );

	Resource->ReloadResource( true );

	unguard();
}

void clResourcesManager::ReCacheNewC( const LString& Param )
{
	Env->Console->Display( "Recaching updated resources..." );

	clResourcesGraph::const_iterator i   = FResourcesGraph.begin();
	clResourcesGraph::const_iterator End = FResourcesGraph.end();

	for ( ; i != End; ++i )
	{
		iResource* Resource = ( *i );

		if ( Resource->GetFileName().empty() )
		{
			continue;
		}

		Lint64 TimeAtLoad  = Resource->GetFileTimeAtLoad();
		Lint64 TimeCurrent = Env->FileSystem->GetFileTime( Resource->GetFileName() );

//      if ( !Resource->IsCached() )
		if ( TimeCurrent > TimeAtLoad )
		{
			Env->Console->Display( "...reloading: " + Resource->GetFileName() );

			Resource->ReloadResource( true );

			Env->RenderFrame( 0.0f, 0 );
		}
	}
}

/*
 * 30/09/2010
     CreateSphere() implemented
 * 10/07/2010
     Reimplemented
 * 07/07/2007
     LoadMaterial()
 * 15/02/2007
     Caches disabling via configuration file
 * 26/11/2006
     ReCacheAllC()
     ReCacheNewC()
 * 12/10/2006
     ReCacheC()
8 * 06/08/2006
     PreCacheC()
 * 04/01/2005
     LoadShader()
     LoadTexture()
 * 31/12/2004
     It's here
*/
