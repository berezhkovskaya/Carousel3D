#include "Prefab.h"

#include "Environment.h"
#include "Resources/ResourcesManager.h"

#include "Core/Logger.h"
#include "Core/VFS/FileSystem.h"
#include "Core/VFS/ML.h"

#include "Geometry/Mesh.h"
#include "Geometry/Geom.h"

clPrefab::clPrefab() :
	FMesh( NULL ), FCollisionMesh( NULL ), FSourceFileName( "" )
{
}

clPrefab::~clPrefab()
{
	if ( FMesh ) { FMesh->DisposeObject(); }

	if ( FCollisionMesh ) { FCollisionMesh->DisposeObject(); }
}

void clPrefab::ImportASE( const LString& fname )
{
	Env->Logger->LogP( L_NOTICE, "Loading prefab: %s", fname.c_str() );
	FSourceFileName = fname;

	double secs = Env->GetEngineTime();

	LString f1 = GetCachedCollisionMeshName();
	LString f2 = GetCachedMeshName();

	if ( Env->FileSystem->FileExists( f1 ) && Env->FileSystem->FileExists( f2 ) )
	{
		FMesh = Env->Resources->CreateGeom();
		FCollisionMesh = Env->Resources->CreateGeom();

		FMesh->CacheFrom( f2 );
		FCollisionMesh->CacheFrom( f1 );

		Env->Logger->LogP( L_NOTICE, "Loaded %s and %s from cache\n", f1.c_str(), f2.c_str() );

		return;
	}

	Env->Logger->LogP( L_NOTICE, "Parsing prefab: %s", fname.c_str() );

	/// Parse ASE file and determine collision/static/dynamic geoms
	mlNode* SrcASE = Env->FileSystem->LoadXLML_ASE( fname );

	std::vector<mlNode*> CollisionNodes;

	std::vector<LString> CollisionPrefixes;
	CollisionPrefixes.push_back( "P_" );
	CollisionPrefixes.push_back( "C_" );
	CollisionPrefixes.push_back( "S_" );
	CollisionPrefixes.push_back( "F_" );

	std::vector<LString> CollisionPartNames;

	std::vector<mlNode*> Nodes;
	FindAllNodesWithName( SrcASE, "*GEOMOBJECT", Nodes );

	// filter all collision geometry
	for ( size_t i = 0 ; i < Nodes.size() ; i++ )
	{
		LString ID = FindSubNode( Nodes[i], "*NODE_NAME" )->getValue();

		for ( size_t j = 0 ; j < CollisionPrefixes.size() ; j++ )
			if ( LStr::StartsWith( ID, CollisionPrefixes[j] ) )
			{
				CollisionPartNames.push_back( ID );
				break;
			}

//		FindAllNodesWithNameStartingFrom(CollisionPrefixes, SrcASE, CollisionNodes);
	}

	/// Load ASE using internal loader
	FCollisionMesh = Env->Resources->LoadGeom( fname );
	FCollisionMesh->WaitLoad();

	/// Close geom to CollisionGeom
	FMesh = Env->Resources->CreateGeom();

	clMesh* Tmp = FCollisionMesh->GetCurrentMesh()->CloneMesh();

	/// Remove collision mesh from the copy
	for ( size_t j = 0 ; j < CollisionPartNames.size() ; j++ )
	{
		LString ID = CollisionPartNames[j];
		int RID = Tmp->FindRigidByName( ID );

		Tmp->RemoveVertexAttribs( RID );
	}

	FMesh->SetMesh( Tmp );

	clMesh* Coll = FCollisionMesh->GetCurrentMesh();

	/// Remove source mesh from CollisionGeom
	for ( size_t k = 0 ; k < Tmp->GetRigidsCount() ; k++ )
	{
		int CollIdx = Coll->FindRigidByName( Tmp->GetRigidName( k ) );
		Coll->RemoveVertexAttribs( CollIdx );
	}

	/// cleanup
	delete SrcASE;

	// save caches
	FMesh->CacheTo( GetCachedMeshName() );
	FCollisionMesh->CacheTo( GetCachedCollisionMeshName() );

	secs = Env->GetEngineTime() - secs;
	Env->Logger->LogP( L_NOTICE, "Done processing prefab: %s, took %f seconds", fname.c_str(), secs );

}

clPrefab* clPrefab::CloneTemplate()
{
	clPrefab* Copy = new clPrefab();
	Copy->Env = Env;

	Copy->FName = FName;
	Copy->FMesh = FMesh->CloneGeom();
	Copy->FCollisionMesh = FCollisionMesh->CloneGeom();

	return Copy;
}

/*
void clPrefab::SaveTo(const LString& OutFileName)
{
}

void clPrefab::LoadFrom(const LString& InFileName)
{
}
*/

void clPrefabCollection::LoadDirectory( const LString& DirName )
{
	LStr::clStringsVector V;
	Env->FileSystem->EnumerateFiles( DirName, false, &V );

	int idx = 0;

	for ( size_t i = 0 ; i < V.size() ; i++ )
	{
		Env->Logger->LogP( L_NOTICE, "File[%d] = %s", i, V[i].c_str() );

		LString FName;
		LString FExt;
		Env->FileSystem->SplitPath( V[i], NULL, NULL, &FName, &FExt );

		FExt = LStr::GetUpper( FExt );

		if ( FExt != ".ASE" && FExt != "ASE" )
		{
			Env->Logger->LogP( L_NOTICE, "Skipping, not an ase" );
			continue;
		}

		FIndices[FName] = ( int )idx;
		idx++;

		LString SrcFile = DirName + "/" + V[i];

		clPrefab* T = new clPrefab();
		T->Env = Env;

		T->ImportASE( SrcFile );

		T->FName = FName;

		FItems.push_back( T );
	}
}

clPrefab* clPrefabCollection::GetObjectByName( const LString& Name )
{
	if ( FIndices.count( Name ) <= 0 ) { return NULL; }

	int idx = FIndices[Name];

	return FItems[idx]/*->CloneTemplate()*/;
}
