/**
 * \file Mesh.cpp
 * \brief
 * \version 0.5.91
 * \date 11/09/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"
#include "Core/Linker.h"
#include "Core/Logger.h"
#include "Core/Console.h"
#include "Scene/Material.h"
#include "Resources/ResourcesManager.h"
#include "Renderer/iTexture.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iShaderProgram.h"
#include "Renderer/iVertexArray.h"

#include "Mesh.h"
#include "ASE.h"
#include "MD5.h"

// to make difference between 32/64 bit platforms and MSVC/GCC builds
#ifdef PLATFORM_GCC
const float MeshVersion = 0.926f + sizeof( size_t ) + 0.0001f;
#else
const float MeshVersion = 0.926f + sizeof( size_t );
#endif

const int CACHED_MESH_MARKER = 0xDEADBEEF;

clMesh::clMesh(): FRigids( 0 ),
	FLocalTransforms( 0 ),
	FGlobalTransforms( 0 ),
	FParentRef( 0 ),
	FMaterialRef( 0 ),
	FMaterials( 0 ),
	FAnimations( NULL ),
	FVertexArrays( 0 ),
	FAABB(),
	FAABBValid( false )
{
}

clMesh::~clMesh()
{
	for ( size_t i = 0; i != FRigids.size(); i++ )
	{
		delete( FRigids[i] );
	}

	for ( size_t i = 0; i != FVertexArrays.size(); i++ )
	{
		delete( FVertexArrays[i] );
	}

	for ( size_t i = 0; i != FPackedSkeletons.size(); i++ )
	{
		delete( FPackedSkeletons[i] );
	}
}

void clMesh::Transform( const LMatrix4& T )
{
	for ( size_t j = 0 ; j < GetRigidsCount() ; j++ )
		if ( FParentRef[j] == -1 )
		{
			FLocalTransforms[j] = T * FLocalTransforms[j];
		}
}

void clMesh::MergeRigids( int Idx1, int Idx2 )
{
//	mtx4 M = FLocalTransforms[Idx1].GetInversed() * FLocalTransforms[Idx2];
//	FRigids[Idx2]->Transform(M);

	FRigids[Idx1]->MergeWith( FRigids[Idx2] );

	this->RemoveVertexAttribs( Idx2 );
}

void clMesh::FlattenMesh()
{
	for ( size_t j = 0 ; j < GetRigidsCount() ; j++ )
	{
		FRigids[j]->Transform( FGlobalTransforms[j] );
		FGlobalTransforms[j] = LMatrix4::Identity();
		FLocalTransforms[j] = LMatrix4::Identity();
	}
}

void clMesh::MergeSimilarRigids()
{
	FlattenMesh();

	/// While there are still items to merge
	while ( FRigids.size() > 1 )
	{
		bool NoMerges = true;

		// TODO: sort by name first !
		for ( size_t j = 0 ; j < FRigids.size() ; j++ )
		{
			for ( size_t k = j + 1 ; k < FRigids.size() ; k++ )
			{
				if ( FNames[j] == FNames[k] )
				{
					NoMerges = false;
					MergeRigids( ( int )j, ( int )k );
					break;
				}
			}

			if ( !NoMerges ) { break; }
		}

		if ( NoMerges ) { break; }
	}
}

void clMesh::CopyMaterialsFrom( const clMesh* M )
{
	size_t Sz = M->GetMaterialsCount();
	FMaterials.FStream.reserve( Sz );

	for ( size_t j = 0 ; j < Sz ; j++ )
	{
		FMaterials.FStream[j] = M->GetMaterialDesc( ( int )j );
	}
}

void clMesh::MergeWith( clMesh* M )
{
	for ( size_t j = 0 ; j < M->GetRigidsCount() ; j++ )
	{
		this->AddVertexAttribs( M->GetRigidName( j ), M->GetRigid( j )->CloneVA(), M->GetLocalTransform( j ), M->GetParentRef( j ), M->GetRigidMaterial( j ) );
	}

	// add materials

	// shift material references

	// add animation sequences
}

void clMesh::RemoveVertexAttribs( int RigidID )
{
	if ( RigidID < 0 || RigidID >= static_cast<int>( FRigids.size() ) ) { return; }

	delete( FRigids[RigidID] );
	delete( FVertexArrays[RigidID] );
	delete( FPackedSkeletons[RigidID] );

	int Last = static_cast<int>( FRigids.size() ) - 1;

	if ( RigidID < Last )
	{
		FRigids[RigidID] = FRigids[Last];
		FNames[RigidID] = FNames[Last];
		FLocalTransforms.FStream[RigidID] = FLocalTransforms.FStream[Last];
		FGlobalTransforms.FStream[RigidID] = FGlobalTransforms[Last];
		FParentRef.FStream[RigidID] = FParentRef.FStream[Last];
		FMaterialRef.FStream[RigidID] = FMaterialRef.FStream[Last];
		FVertexArrays[RigidID] = FVertexArrays[Last];
		FPackedSkeletons[RigidID] = FPackedSkeletons[Last];
	}

	FRigids.pop_back();
	FNames.pop_back();
	FLocalTransforms.FStream.pop_back();
	FGlobalTransforms.FStream.pop_back();
	FParentRef.FStream.pop_back();
	FMaterialRef.FStream.pop_back();
	FVertexArrays.pop_back();
	FPackedSkeletons.pop_back();
}

void clMesh::SaveToBinaryFile( const LString& FName ) const
{
	iOStream* OutStream = Env->FileSystem->CreateFileWriter( FName );
	this->SaveToStream( OutStream );
	delete OutStream;
}

void clMesh::LoadFromBinaryFile( const LString& FName )
{
	iIStream* InStream = Env->FileSystem->CreateFileReader( FName );
	LoadFromStream( InStream );
	delete InStream;
}

clMesh* clMesh::CloneMesh() const
{
	LString TmpFileName = "____tmp_clone.mesh";
	/*
	   iOStream* OutStream = Env->FileSystem->CreateFileWriter( TmpFileName );
	   this->SaveToStream(OutStream);
	   delete OutStream;
	*/
	SaveToBinaryFile( TmpFileName );

	clMesh* Res = Env->Linker->Instantiate( "clMesh" );
	/*
	   iIStream* InStream = Env->FileSystem->CreateFileReader( TmpFileName );
	   Res->LoadFromStream(InStream);
	   delete InStream;
	*/
	Res->LoadFromBinaryFile( TmpFileName );

	return Res;
}

sAnimSequence clMesh::GetAnimSequenceByName( const LString& Name ) const
{
	if ( !FAnimations ) { return sAnimSequence(); }

	return FAnimations->GetCurrentAnimSet()->GetAnimSequenceByName( Name );
}

LString clMesh::GetAnimSequenceName( size_t i ) const
{
	if ( !FAnimations ) { return LString(); }

	return FAnimations->GetCurrentAnimSet()->GetAnimSequenceName( i );
}

size_t clMesh::GetNumAnimSequences() const
{
	if ( !FAnimations ) { return 0; }

	return FAnimations->GetCurrentAnimSet()->GetNumAnimSequences();
}

int clMesh::AddVertexAttribs( const LString& Name,
                              clVertexAttribs* Attribs,
                              const LMatrix4& GlobalTransform,
                              int ParentID,
                              int MaterialID )
{
	FRigids.push_back( Attribs );
	FNames.push_back( Name );
	FLocalTransforms.FStream.push_back( GlobalTransform );
	FGlobalTransforms.FStream.push_back( GlobalTransform );
	FParentRef.FStream.push_back( ParentID );
	FMaterialRef.FStream.push_back( MaterialID );
	FVertexArrays.push_back( NULL );
	FPackedSkeletons.push_back( NULL );

	return static_cast<int>( FRigids.size() ) - 1;
}

int clMesh::AddMaterial( const sMaterialDesc& Material )
{
	FMaterials.FStream.push_back( Material );

	return static_cast<int>( FMaterials.size() ) - 1;
}

bool clMesh::LoadMesh( const LString& FileName )
{
	guard( "%s", FileName.c_str() );

	iIStream* Stream = Env->FileSystem->CreateFileReader( FileName );

	if ( Stream == NULL ) { return false; }

	LString Ext;
	Env->FileSystem->SplitPath( FileName, NULL, NULL, NULL, &Ext );
	LStr::ToUpper( &Ext );

	iMeshLoader* Loader = NULL;

	if ( Ext == ".ASE" )
	{
		Loader = Env->Linker->Instantiate( "clASELoader" );
	}
	else if ( Ext == ".LCM" )
	{
		Loader = Env->Linker->Instantiate( "clLCMLoader" );
	}
	else if ( Ext == ".MD5MESH" )
	{
		Loader = Env->Linker->Instantiate( "clMD5Loader" );
	}

	if ( !Loader )
	{
		delete( Stream );

		return false;
	}

	bool Result = Loader->LoadMesh( Stream, this );

	delete( Loader );
	delete( Stream );

	return Result;

	unguard();
}

void clMesh::RecalculateLocalFromGlobal()
{
	for ( size_t i = 0; i != FRigids.size(); i++ )
	{
		int ParentIdx = FParentRef.FStream[ i ];

		if ( ParentIdx != -1 )
		{
			// do not calculate anything
			LMatrix4 ParentTransform = FGlobalTransforms.FStream[ ParentIdx ];

			FLocalTransforms.FStream[ i ] = ParentTransform.GetInversed() * FGlobalTransforms.FStream[ i ];
		}
		else
		{
			FLocalTransforms.FStream[ i ] = FGlobalTransforms.FStream[ i ];
		}
	}
}

int clMesh::FindRigidByName( const LString& Name ) const
{
	for ( size_t i = 0; i != FNames.size(); i++ )
	{
		if ( FNames[ i ] == Name )
		{
			return static_cast<int>( i );
		}
	}

	return -1;
}

iVertexArray* clMesh::GetVertexArray( size_t i ) const
{
	guard( "%i", i );

	if ( FVertexArrays[i] == NULL )
	{
		FVertexArrays[i] = Env->Renderer->AllocateEmptyVA();
		FVertexArrays[i]->SetVertexAttribs( FRigids[i] );
	}

	return FVertexArrays[i];

	unguard();
}

iTexture* clMesh::GetPackedSkeleton( size_t i ) const
{
	guard( "%i", i );

	if ( FPackedSkeletons[i] == NULL )
	{
		if ( GetRigid( i )->FSkeletonFrames.size() == 0 )
		{
			return NULL;
		}

		FPackedSkeletons[i] = Env->Renderer->CreateTexture();
		FPackedSkeletons[i]->LoadFromBitmap( GetRigid( i )->GetMorphTargetsBitmap( Env ) );
	}

	return FPackedSkeletons[i];

	unguard();
}

bool clMesh::SaveMesh( const LString& FileName )
{
	return false;
}

LAABoundingBox clMesh::GetBoundingBox() const
{
	LAABoundingBox AABB = LAABoundingBox();

	for ( size_t i = 0; i != FRigids.size(); i++ )
	{
		FIXME( "use current frame" )
		LAABoundingBox Box = FRigids[ i ]->GetBoundingBox( 0 );

		Box.Transform( FLocalTransforms.FStream[ i ] );

		AABB.Combine( Box );
	}

	return AABB;
}

LAABoundingBox clMesh::GetCachedBoundingBox() const
{
	if ( !FAABBValid )
	{
		// store bounding box
		FAABB = GetBoundingBox();

		FAABBValid = true;
	}

	return FAABB;
}

void clMesh::PrecalculateBoundingBoxes()
{
	for ( size_t i = 0; i != FRigids.size(); i++ )
	{
		FRigids[i]->PrecalculateBoundingBoxes();
	}
}

void clMesh::SaveToStream( iOStream* Stream ) const
{
	// write mesh version
	Stream->BlockWrite( &MeshVersion, sizeof( MeshVersion ) );

	// write bounding volumes
	Stream->BlockWrite( &FAABB, sizeof( FAABB ) );
	Stream->BlockWrite( &FAABBValid, sizeof( FAABBValid ) );

	// write geometry
	size_t NumRigids = FRigids.size();

	Stream->BlockWrite( &NumRigids, sizeof( NumRigids ) );

	for ( size_t i = 0; i != NumRigids; i++ )
	{
		FRigids[ i ]->SaveToStream( Stream );
		Stream->WriteLine( FNames[ i ] );
	}

	FLocalTransforms.SaveToStream( Stream );
	FGlobalTransforms.SaveToStream( Stream );
	FParentRef.SaveToStream( Stream );
	FMaterialRef.SaveToStream( Stream );
	FMaterials.SaveToStream( Stream );
	Stream->BlockWrite( &CACHED_MESH_MARKER, sizeof( CACHED_MESH_MARKER ) );

	// write the marker
	Stream->BlockWrite( &FAnimations, sizeof( FAnimations ) );

	if ( FAnimations )
	{
		LStringBuffer AnimFileName( FAnimations->GetFileName() );

		Stream->BlockWrite( &AnimFileName, sizeof( AnimFileName ) );
	}
}

bool clMesh::LoadFromStream( iIStream* Stream )
{
	// check version
	{
		float Probe = 0;

		Stream->BlockRead( &Probe, sizeof( Probe ) );

		if ( MeshVersion != Probe )
		{
			Env->Logger->Log( L_NOTICE, "Regenerating cached mesh: version changed" );

			return false;
		}
	}

	// read bounding volumes
	Stream->BlockRead( &FAABB, sizeof( FAABB ) );
	Stream->BlockRead( &FAABBValid, sizeof( FAABBValid ) );

	// read geometry
	size_t NumRigids = 0;

	Stream->BlockRead( &NumRigids, sizeof( NumRigids ) );

	FRigids.resize( NumRigids );
	FPackedSkeletons.resize( NumRigids );
	FNames.resize( NumRigids );
	FVertexArrays.resize( NumRigids );

	for ( size_t i = 0; i != NumRigids; i++ )
	{
		FRigids[ i ] = clVertexAttribs::CreateEmpty();
		FRigids[ i ]->LoadFromStream( Stream );
		FNames[ i ] = Stream->ReadLine();
		FVertexArrays[ i ] = NULL;
	}

	int Marker = 0;

	FLocalTransforms.LoadFromStream( Stream );
	FGlobalTransforms.LoadFromStream( Stream );
	FParentRef.LoadFromStream( Stream );
	FMaterialRef.LoadFromStream( Stream );
	FMaterials.LoadFromStream( Stream );
	Stream->BlockRead( &Marker, sizeof( Marker ) );

	if ( Marker != CACHED_MESH_MARKER )
	{
		Env->Logger->Log( L_NOTICE, "Invalid cached mesh format: marker is corrupted" );
		return false;
	}

	// read the marker
	Stream->BlockRead( &FAnimations, sizeof( FAnimations ) );

	if ( FAnimations )
	{
		LStringBuffer AnimFileName;

		Stream->BlockRead( &AnimFileName, sizeof( AnimFileName ) );
		FAnimations = Env->Resources->LoadAnimation( AnimFileName.ToString() );
	}

	return true;
}

void TransformRayToCoordinates( const LVector3& P, const LVector3& A, const LMatrix4& Transform, LVector3& TransP, LVector3& TransA )
{
	// transform P to TransP
	TransP = Transform * P;

	// rotate A to TransA
	TransA = Transform.ExtractMatrix3() * A;
}

bool clMesh::IntersectWithRay( const LVector3& P, const LVector3& A, LVector3& isect ) const
{
	int Rigid = -1;
	return IntersectWithRayIdx( P, A, isect, Rigid );
}

bool clMesh::IntersectWithRayIdx( const LVector3& P, const LVector3& A, LVector3& isect, int& Rigid ) const
{
	float Dist = ::Linderdaum::Math::INFINITY;
	LVector3 result;
	bool Intersection = false;

	Rigid = -1;

	for ( int i = 0 ; i != static_cast<int>( FRigids.size() ) ; i++ )
	{
		// transform ray to the submesh coord system
		LVector3 TransP = P, TransA = A;

		TransformRayToCoordinates( P, A, FGlobalTransforms.GetPtr()[i], TransP, TransA );

		// intersect with the array
		Intersection = FRigids[i]->IntersectWithRay( TransP, TransA, result );

		if ( Intersection )
		{
			float NewDist = ( result - P ).Length();

			if ( NewDist < Dist )
			{
				isect = result;
				Dist = NewDist;

				Rigid = i;
			}
		}
	}

	return Intersection;
}

void clMesh::LoadAnimStates( const LString& FileName )
{
	guard( "%s", FileName.c_str() );

	FAnimations = Env->Resources->LoadAnimation( FileName );

	unguard();
}

/*
 * 11/09/2010
     CACHED_MESH_MARKER added for correct caching on 32/64-bit platforms
 * 09/02/2010
     It's here
*/
