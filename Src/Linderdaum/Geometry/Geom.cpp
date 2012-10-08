/**
 * \file Geom.cpp
 * \brief
 * \version 0.5.93
 * \date 29/10/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Mesh.h"
#include "Geom.h"
#include "Environment.h"
#include "Core/Linker.h"
#include "Core/CVars.h"
#include "Core/Console.h"
#include "Core/Logger.h"
#include "Geometry/GeomServ.h"
#include "Resources/ResourcesManager.h"

clGeom::clGeom():
	FDefaultMesh( NULL ),
	FCurrentMesh( NULL )
{
}

void clGeom::AfterConstruction()
{
	iResource::AfterConstruction();

//	Env->Logger->Log( L_NOTICE, "Creating clGeom()" );

	FDefaultMesh = Env->Resources->CreateMesh();
	FDefaultMesh->SetObjectID( "FDefaultMesh" );

	FCurrentMesh = FDefaultMesh;
}

clGeom::~clGeom()
{
	if ( FCurrentMesh != FDefaultMesh )
	{
		delete( FCurrentMesh );
	}

	delete( FDefaultMesh );
}

clGeom* clGeom::CloneGeom() const
{
	clGeom* Res = Env->Resources->CreateGeom();

	Res->SetMesh( this->GetCurrentMesh()->CloneMesh() );

	return Res;
}

LString clGeom::GetCachingDir() const
{
	return Env->Console->GetVarValueStr( "Cache.GeomsCachingDir", "Cache/Geoms" );
}

bool clGeom::LoadFromFile( const LString& FileName )
{
	guard( "%s", FileName.c_str() );

	LString Ext;
	Env->FileSystem->SplitPath( FileName, NULL, NULL, NULL, &Ext );
	LStr::ToUpper( &Ext );

	if ( Ext == ".LCACHEDMESH" )
	{
		return CacheFrom( FileName );
	}

	GetCurrentMesh()->LoadMesh( FileName );

	return true;

	unguard();
}

void clGeom::CacheTo( const LString& FileName )
{
#if !defined( OS_ANDROID )

	guard( "%s", FileName.c_str() );

//	LMutex Lock( &FMutex );

	iResource::CacheTo( FileName );

	iOStream* Stream = Env->FileSystem->CreateFileWriter( FileName );

	// write mesh
	FCurrentMesh->SaveToStream( Stream );

	delete( Stream );

	unguard();
#endif
}

bool clGeom::CacheFrom( const LString& FileName )
{
	guard( "%s", FileName.c_str() );

	iResource::CacheFrom( FileName );

	iIStream* Stream = Env->FileSystem->CreateFileReader( FileName );

	clMesh* Mesh = Env->Resources->CreateMesh();

	// read mesh
	bool MeshLoaded = Mesh->LoadFromStream( Stream );

	delete( Stream );

	if ( MeshLoaded ) { SetMeshInternal( Mesh, false ); }

	Env->Logger->Log( L_DEBUG, "Mesh loaded from cache: " + LStr::ToStr( MeshLoaded ) );

	return MeshLoaded;

	unguard();
}

clMesh* clGeom::GetCurrentMesh() const
{
	LMutex Lock( &FMutex );

	return FCurrentMesh;
};

clMesh* clGeom::GetDefaultMesh() const
{
	LMutex Lock( &FMutex );

	return FDefaultMesh;
};

void clGeom::SetMesh( clMesh* Mesh )
{
	SetMeshInternal( Mesh, true );
}

void clGeom::SetMeshInternal( clMesh* Mesh, bool Notify )
{
	LMutex Lock( &FMutex );

	if ( FCurrentMesh != FDefaultMesh )
	{
		delete( FCurrentMesh );

		FCurrentMesh = Mesh;
	}
	else
	{
//		delete( FDefaultMesh );
//		FDefaultMesh = Mesh;
		FCurrentMesh = Mesh;
	}

	if ( Notify ) { SendAsync( L_EVENT_CHANGED, LEventArgs( this ), false ); }
}

/*
 * 24/02/2010
     Log added
 * 07/02/2010
     It is here
*/
