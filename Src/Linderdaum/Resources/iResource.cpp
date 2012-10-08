/**
 * \file iResource.cpp
 * \brief Generic resource base class
 * \version 0.5.93
 * \date 28/03/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "iResource.h"

#include "Environment.h"

#include "Engine.h"
#include "Core/Linker.h"
#include "Core/iObject.h"
#include "Core/Logger.h"
#include "Core/Console.h"
#include "Core/VFS/FileSystem.h"
#include "ResourcesManager.h"

#include <memory>
#include <algorithm>

iResource::iResource()
	: FResourceState( L_RESOURCE_EMPTY ),
	  FAsyncComplete( true ),
	  FFileName(),
	  FFileTimeAtLoad( 0 ),
	  FResourcePtr( NULL ),
	  FResourceID( 0 )
{
}

void iResource::AfterConstruction()
{
	FResourcePtr = this;

	FResourceID = Env->Resources->FResourcesGraph.size();

	Env->Resources->FResourcesGraph.push_back( this );
}

iResource::~iResource()
{
	//Env->Logger->LogP( L_DEBUG, "Deleting resource %s (%s)", this->ClassName().c_str(), this->GetObjectID().c_str() );

	clResourcesManager::clResourcesGraph::iterator i = std::find( Env->Resources->FResourcesGraph.begin(),
	                                                              Env->Resources->FResourcesGraph.end(), FResourcePtr );

	if ( i != Env->Resources->FResourcesGraph.end() )
	{
		Env->Resources->FResourcesGraph.erase( i );
	}
	else
	{
		Env->Logger->Log( L_WARNING, "Cannot find the resource " + this->ClassName() + "(" + this->GetObjectID() + ") in FResourcesGraph on deallocation" );
	}
}

void iResource::SetAsyncLoadComplete( bool Complete ) volatile
{
	FResourceState = Complete ? L_RESOURCE_LOADED : L_RESOURCE_LOADING;
	FAsyncComplete = Complete;
}

bool iResource::Load( const LString& FileName, bool CacheResource )
{
	guard( "%s, %i", FileName.c_str(), CacheResource );

	if ( Env->FileSystem->FileExists( FileName ) )
	{
		FFileTimeAtLoad = Env->FileSystem->GetFileTime( FileName );
	}

	SetFileName( FileName );

	bool ReadFromCacheSuccess = false;

	if ( IsCached() )
	{
		TODO( "implement multithreaded loading here" )
		ReadFromCacheSuccess = CacheFrom( GetCachedFileName() );
	}

	if ( !ReadFromCacheSuccess )
	{
		return ReloadResource( CacheResource );
	}

	return true;

	unguard();
}

void iResource::Reload()
{
	if ( FResourceState == L_RESOURCE_UNLOADED )
	{
		// bring it back
		ReloadResource( false );
	}
}

void iResource::WaitLoad()
{
	Reload();

	if ( !IsAsyncLoadComplete() )
	{
		double StartTime = Env->GetSeconds();

		Env->Logger->Log( L_DEBUG, "Waiting for resource " + GetFileName() + " to load" );

		while ( !IsAsyncLoadComplete() );

		Env->Logger->Log( L_DEBUG, "...resource become available in " + LStr::ToStr( Env->GetSeconds() - StartTime ) + " seconds" );
	}
}

void iResource::Unload()
{
	if ( FResourceState != L_RESOURCE_LOADED ) { return; }

	Env->Logger->LogP( L_DEBUG, "Unloading resource: %s", GetFileName().c_str() );

	if ( UnloadResource() ) { FResourceState = L_RESOURCE_UNLOADED; }
}

bool iResource::UnloadResource()
{
	// dummy implementation, for those who cannot unload
	return false;
}

bool iResource::ReloadResource( bool Recache )
{
	guard()

	if ( Env->FileSystem->FileExists( GetFileName() ) )
	{
		FFileTimeAtLoad = Env->FileSystem->GetFileTime( GetFileName() );
	}

	if ( !LoadFromFile( GetFileName() ) )
	{
		return false;
	}

	if ( Recache )
	{
		CacheTo( GetCachedFileName() );
	}

	return true;

	unguard();
}

TODO( "fix cache updates on renewed files" )

LString iResource::GetCachedFileName()
{
	LString CachingDir = GetCachingDir();
	LStr::AddTrailingChar( &CachingDir, PATH_SEPARATOR );

	LString FN = clResourcesManager::ConvertName( GetFileName() );

	return ( CachingDir + FN + ".cached" );
}

bool iResource::IsCached()
{
	LString CachedFN = GetCachedFileName();

	if ( Env->FileSystem->FileExists( GetFileName() ) )
	{
		if ( !Env->FileSystem->FileExistsAndNewer( GetFileName(), CachedFN ) )
		{
//			Env->Logger->Log( L_NOTICE, "Not cached: " + CachedFN );

			return false;
		}
	}
	else
	{
		return Env->FileSystem->FileExists( CachedFN );
	}

	return true;
}

void iResource::CacheTo( const LString& FileName )
{
	if ( FileName == GetCachedFileName() )
	{
		Env->FileSystem->CreateDirs( GetCachingDir() );
	}
}

bool iResource::CacheFrom( const LString& FileName )
{
	// should be empty
	return false;
}

bool iResource::IsSameResource( iResource* Other )
{
	if ( !Other ) { return false; }

	if ( GetFileName() != Other->GetFileName() ) { return false; }

	return true;
}

bool iResource::LoadFromFile( const LString& FileName )
{
	guard();

	// store the file name
	SetFileName( FileName );

	// TODO : make this work. Add EndLoad() method with "decyphering" of loaded vector<Image>
	iObject* Ptr = this;

	if ( !Env->Linker->LoadObjectFromFile( GetFileName(), &Ptr ) ) { return false; }

	SetObjectID( FileName );

	return true;

	unguard();
}

void iResource::Event_FILE_NOTIFICATION( LEvent Event, const LEventArgs& Args )
{
	Env->Console->Display( "...changed resource: " + FFileName );

	clFileWatchHandle* Info = dynamic_cast<clFileWatchHandle*>( Args.FObjArg );

	if ( Info ) { ResourceFileChanged( Info ); }
}

void iResource::ResourceFileChanged( clFileWatchHandle* File )
{
	this->ReloadResource( true );
}

void iResource::SetFileName( const LString& FileName )
{
	if ( FFileName != FileName )
	{
		this->Connect( L_EVENT_FILE_NOTIFICATION, Utils::Bind( &iResource::Event_FILE_NOTIFICATION, this ) );

		Env->FileSystem->AddFileWatch( FileName, this );
	}

	FFileName = FileName;
};

/*
 * 28/10/2010
     LoadFromFile()
 * 08/03/2010
     Timestamps for files
 * 28/05/2009
     FindResourceInGraph()
 * 21/11/2008
     Minor refactoring
 * 04/09/2006
     ReloadResource()
 * 04/01/2005
     It's here
*/
