/**
 * \file Entity.cpp
 * \brief Game entity
 * \version 0.5.91
 * \date 04/07/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Entity.h"

#include "Environment.h"
#include "Core/Logger.h"
#include "Scene/Scene.h"
#include "World/World.h"
#include "Geometry/Geom.h"
#include "Resources/ResourcesManager.h"

clEntity::clEntity()
	: FGeom( NULL ),
	  FScene( NULL ),
	  FGeomHandle( -1 ),
	  FMeshName( "" ),
	  FMeshHeight( 0.0f ),
	  FCenterMesh( false ),
	  FTransform(),
	  FLoadedFromFile( false )
{
}

clEntity::~clEntity()
{
}

bool clEntity::EndLoad()
{
	// preload mesh, good for multithreading
	if ( !FMeshName.empty() )
	{
		FGeom = Env->Resources->LoadGeom( FMeshName );

		FLoadedFromFile = true;
	}

	return iActor::EndLoad();
}

void clEntity::Actor_EnteredWorld( clWorld* World )
{
	guard();

	iActor::Actor_EnteredWorld( World );

	FScene = GetWorld()->GetScene();

	if ( GetGeomHandle() == -1 )
	{
		if ( FGeom == NULL )
		{
			FATAL( FMeshName.empty(), "Mesh isn't defined for clEntity: " + GetObjectID() );

			FGeom = Env->Resources->LoadGeom( FMeshName );

			FLoadedFromFile = true;
		}

		int ParentID = GetParentGeomHandle();

		if ( ParentID != -1 )
		{
			FGeomHandle = GetScene()->AddGeomToParent( FGeom, ParentID );
		}
		else
		{
			FGeomHandle = GetScene()->AddGeom( FGeom );
		}

		if ( FLoadedFromFile && ( FMeshHeight != 0.0f ) )
		{
			Env->Logger->Log( L_DEBUG, "Resizing " + FGeom->GetFileName() + " to " + LStr::ToStr( FMeshHeight ) + " with centering " + LStr::ToStr( GetCenterMesh() ) );
			GetScene()->Resize( FGeomHandle, FMeshHeight, GetCenterMesh() );
		}

		GetScene()->SetOwner( FGeomHandle, this );
	}

	unguard();
}

void clEntity::Actor_LeavedWorld()
{
	GetScene()->RemoveItem( FGeomHandle );

	iActor::Actor_LeavedWorld();

	FScene = NULL;
}

void clEntity::UpdateActor( float DeltaSeconds )
{
	int Handle = GetGeomHandle();

	if ( Handle > -1 )
	{
		GetScene()->SetLocalTransform( Handle, FTransform.GetMatrix4() );
	}
}

/*
 * 04/07/2010
     It's here
*/
