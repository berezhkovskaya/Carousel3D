/**
 * \file World.cpp
 * \brief Game world
 * \version 0.5.74
 * \date 28/04/2007
 * \author Sergey Kosarevsky, 2005-2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "World.h"
#include "Environment.h"
#include "Utils/Utils.h"
#include "Core/iObject.h"
#include "Core/Console.h"
#include "Core/VFS/FileSystem.h"
#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iShaderProgram.h"
#include "Scene/Scene.h"
#include "Core/Linker.h"

#include "Core/Logger.h"
#include "Environment.h"

#include <algorithm>

namespace Linderdaum
{
	/// Internal helper classes namespace
	namespace Helpers
	{
		/// Update actors
		class clActorTicker
		{
		public:
			clActorTicker( float DeltaSeconds,
			               sEnvironment* Env ): FDeltaSeconds( DeltaSeconds ),
				FEnvironment( Env ) {};
			inline void operator ( ) ( const std::pair<LString, iActor*>& Pair )
			{
				FStart = FEnvironment->GetSeconds();

				Pair.second->UpdateActor( FDeltaSeconds );

				FEnd = FEnvironment->GetSeconds();

				Pair.second->SetTimeProfile( FEnd - FStart );
			};
		private:
			sEnvironment*   FEnvironment;
			float           FDeltaSeconds;
			double          FStart;
			double          FEnd;
		};
		/// Update actors in engine pause mode
		class clActorPauseTicker
		{
		public:
			clActorPauseTicker( float DeltaSeconds ): FDeltaSeconds( DeltaSeconds ) {};
			inline void operator ( ) ( const std::pair<LString, iActor*>& Pair ) const
			{
				if ( Pair.second->IsUpdatedInPause() )
				{
					Pair.second->UpdateActor( FDeltaSeconds );
				}
			};
		private:
			float FDeltaSeconds;
		};
		/// Actors overlay renderer
		class clActorOverlayRenderer
		{
		public:
			void SetProjection( const LMatrix4& Projection )
			{
				FProjection = Projection;
			};
			void SetTransform( const LMatrix4& Transform )
			{
				FTransform = Transform;
			};
			inline void operator ( ) ( const std::pair<LString, iActor*>& Pair ) const
			{
				Pair.second->RenderOverlay( FProjection, FTransform );
			};
		private:
			LMatrix4    FProjection;
			LMatrix4    FTransform;
		};
		/// Compares order of actors serialization
		class clActorComparator
		{
		public:
			inline int operator () ( iActor* A1, iActor* A2 ) const
			{
				return A1->GetOrder() < A2->GetOrder();
			};
		};
		//
	}
}

const LString WorldRootNodeName = "WORLD_ROOT_NODE";

clWorld::clWorld()
	: FGlobalActorsList(),
	  FActorsList(),
	  FPrecacheFiles(),
	  FActorsToAdd(),
	  FActorsToDelete(),
	  FRenderingTechnique( NULL ),
	  FPlaying( false ),
	  FCurrentOrder( 0 )
{
}

void clWorld::AfterConstruction()
{
	guard();

	Env->Console->RegisterCommand( "SaveWorld",         Utils::Bind( &clWorld::SaveWorldC, this ) );
	Env->Console->RegisterCommand( "LoadWorld",         Utils::Bind( &clWorld::LoadWorldC, this ) );
	Env->Console->RegisterCommand( "LoadRenderingTechnique", Utils::Bind( &clWorld::LoadRenderingTechniqueC, this ) );
	Env->Console->RegisterCommand( "SpawnTemplate",     Utils::Bind( &clWorld::SpawnTemplateC, this ) );
	Env->Console->RegisterCommand( "SaveSceneGraph",    Utils::Bind( &clWorld::SaveSceneGraphC, this ) );

	Env->Connect( L_EVENT_TIMER,       BIND( &clWorld::Event_TIMER       ) );
	Env->Connect( L_EVENT_DRAWOVERLAY, BIND( &clWorld::Event_DRAWOVERLAY ) );

	FScene = NULL;

	unguard();
}

clWorld::~clWorld()
{
	guard();

	Env->DisconnectObject( this );

	Env->Console->UnRegisterCommand( "SaveWorld",         Utils::Bind( &clWorld::SaveWorldC, this ) );
	Env->Console->UnRegisterCommand( "LoadWorld",         Utils::Bind( &clWorld::LoadWorldC, this ) );
	Env->Console->UnRegisterCommand( "LoadRenderingTechnique", Utils::Bind( &clWorld::LoadRenderingTechniqueC, this ) );
	Env->Console->UnRegisterCommand( "SpawnTemplate",     Utils::Bind( &clWorld::SpawnTemplateC, this ) );
	Env->Console->UnRegisterCommand( "SaveSceneGraph",    Utils::Bind( &clWorld::SaveSceneGraphC, this ) );

	//SaveProfilingInfo();

	ClearWorld();

	delete( FRenderingTechnique );

	unguard();
}

void clWorld::RegisterActor( iActor* Actor )
{
	FGlobalActorsList.push_back( Actor );
}

void clWorld::UnRegisterActor( iActor* Actor )
{
	FGlobalActorsList.erase( std::remove( FGlobalActorsList.begin(), FGlobalActorsList.end(), Actor ), FGlobalActorsList.end() );
}

void clWorld::ClearWorld()
{
	Env->Console->SendCommand( "ResetGUI" );

	if ( FRenderingTechnique )
	{
		FRenderingTechnique->Reset();
	}

	DeletePendingActors();

	clActorsList List = FActorsList;

	// everyone should correctly leave the world
	for ( clActorsList::iterator i = List.begin();
	      i != List.end();
	      ++i )
	{
		i->second->LeaveWorld();
	}

	FActorsList.clear();

	DeletePendingActors();

	FPrecacheFiles.clear();
	FPlaying = false;
	FCurrentOrder = 0;

	delete( FScene );
	FScene = NULL;
}

void clWorld::SpawnTemplateC( const LString& Param )
{
	if ( Param.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: SPAWNTEMPLATE <file name>" );
		return;
	}

	if ( !FScene ) { FScene = Env->Linker->Instantiate( "clScene" ); }

	iActor* Actor = NULL;

	Env->Linker->LoadObjectFromFile( Param, reinterpret_cast<iObject**>( &Actor ) );

	//iActor* Actor = dynamic_cast<iActor*>( Env->Linker->CreateHierarchyFromFile( Param, NULL, true, NULL ) );

	FATAL( !Actor, "No valid actor was specified in template file: " + Param );

	FGlobalActorsList.push_back( Actor );

	PrepareActor( Actor );
}

void clWorld::SaveSceneGraphC( const LString& Param )
{
	if ( Param.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: SAVESCENEGRAPH <file name>" );
		return;
	}

	/*
	   if ( !GetSceneRoot() )
	   {
	      Env->Console->DisplayError( "clWorld::GetSceneRoot() returned NULL" );
	      return;
	   }

	  Env->Linker->SaveObjectToFile( Param, GetSceneRoot() );
	*/
}

void clWorld::LoadWorld( const LString& FileName, const LString& SplashScreen )
{
	ClearWorld();

	if ( !FScene ) { FScene = Env->Linker->Instantiate( "clScene" ); }

	if ( !SplashScreen.empty() )
	{
		Env->Console->SendCommand( "AddGUI " + SplashScreen );

		Env->RenderFrame( 0, 0 );
	}

	iIStream* Stream = Env->FileSystem->CreateFileReader( FileName );

	bool WorldLoaded = Env->Linker->LoadTypedObjectsListFromStream<iActor>( Stream, &FGlobalActorsList );

	delete( Stream );

	if ( !WorldLoaded )
	{
		FGlobalActorsList.clear();

		Env->Console->DisplayError( "Invalid world: " + FileName );

		return;
	}

	// prepare all actors
	for ( size_t i = 0; i != FGlobalActorsList.size(); i++ )
	{
		PrepareActor( FGlobalActorsList[i] );
	}

	BeginPlay();

//   Env->Console->SendCommand( "ResetGUI" );

	bool Paused = Env->IsPaused();

	if ( Paused )
	{
		Env->Resume();
	}

	// generate local event
	Event_TIMER( L_EVENT_TIMER, LEventArgs( 0.0f ) );

	if ( Paused )
	{
		Env->Pause();
	}
}

void clWorld::SaveWorld( const LString& FileName )
{
	Env->Linker->SaveObjectToFile( FileName, this );
}

void clWorld::LoadWorldC( const LString& Param )
{
	if ( Param.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: LOADWORLD <world file> <splash screen.GUI>" );

		return;
	}

	LString WorldFileName   = LStr::GetToken( Param, 1 );
	LString SpawnGUIOnReset = LStr::GetToken( Param, 2 );

	LoadWorld( WorldFileName, SpawnGUIOnReset );
}

void clWorld::SaveWorldC( const LString& Param )
{
	if ( Param.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: SAVEWORLD <world file>" );
		return;
	}

	SaveWorld( Param );
}

void clWorld::PrepareActor( iActor* Actor )
{
	for ( clActorsList::iterator i = FActorsList.begin(); i != FActorsList.end(); ++i )
	{
		if ( Actor == i->second ) { return; }
	}

	Actor->SetOrder( FCurrentOrder++ );

	FATAL( FActorsList[ Actor->GetObjectID() ] != NULL, "Actors with duplicate IDs found: " + Actor->GetObjectID() );

	FActorsList[ Actor->GetObjectID() ] = Actor;

	FIDsList.push_back( Actor->GetObjectID() );

	// generate event
	Actor->Actor_EnteredWorld( this );

	if ( FPlaying )
	{
		Actor->Actor_PostBeginPlay();
	}
}

void clWorld::AddActor( iActor* Actor )
{
	FActorsToAdd.push_back( Actor );
}

void clWorld::AddActorInternal( iActor* Actor )
{
	FGlobalActorsList.push_back( Actor );

	PrepareActor( Actor );

	for ( clActorsList::iterator i = FActorsList.begin(); i != FActorsList.end(); ++i )
	{
		if ( Actor != i->second )
		{
			i->second->Actor_SomeoneEnteredWorld( Actor );
		}
	}
}

void clWorld::RemoveActor( iActor* Actor )
{
	RemoveActorInternal( Actor );
}

void clWorld::RemoveActorInternal( iActor* Actor )
{
	LStr::clStringsVector::iterator i = std::find( FIDsList.begin(), FIDsList.end(), Actor->GetObjectID() );

	FATAL( i == FIDsList.end(), "Unable to find actor" );

	FIDsList.erase( i );

	FActorsToDelete.push_back( Actor );

	// generate event
	Actor->Actor_LeavedWorld();
}

void clWorld::BeginPlay()
{
	FPlaying = true;

	for ( clActorsList::iterator i = FActorsList.begin(); i != FActorsList.end(); ++i )
	{
		i->second->Actor_PostBeginPlay();
	}

	for ( size_t i = 0; i != FPrecacheFiles.size(); ++i )
	{
		Env->Console->SendCommand( "PRECACHE " + FPrecacheFiles[i] );
	}
}

void clWorld::LoadRenderingTechniqueC( const LString& Param )
{
	if ( Param.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: LOADRENDERINGTECHNIQUE <RT file>" );

		return;
	}

	LoadRenderingTechnique( Param );
}

void clWorld::LoadRenderingTechnique( const LString& FileName )
{
	if ( !FScene ) { FScene = Env->Linker->Instantiate( "clScene" ); }

	clRenderingTechnique* RenderTech = NULL;

	if ( !Env->Linker->LoadObjectFromFile( FileName, reinterpret_cast<iObject**>( &RenderTech ) ) )
	{
		Env->Console->DisplayError( "Unable to load rendering technique from " + FileName );
	}

	ReplaceRenderingTechnique( RenderTech );
	/*
	   dynamic_cast<clRenderingTechnique*>(
	      Env->Linker->CreateHierarchyFromFile( FileName, NULL, false, NULL )
	   )
	);
	*/
}

void clWorld::ReplaceRenderingTechnique( clRenderingTechnique* Technique )
{
	delete( FRenderingTechnique );

	FRenderingTechnique = Technique;
}

clRenderingTechnique* clWorld::GetRenderingTechnique()
{
	return FRenderingTechnique;
}

void clWorld::DeletePendingActors()
{
	guard();

	while ( !FActorsToDelete.empty() )
	{
		iActor* Actor = FActorsToDelete.back();

		FActorsList.erase( Actor->GetObjectID() );

		// send event
		for ( clActorsList::iterator i = FActorsList.begin(); i != FActorsList.end(); ++i )
		{
			if ( Actor != i->second )
			{
				i->second->Actor_SomeoneLeavedWorld( Actor );
			}
		}

		delete( Actor );

		FActorsToDelete.pop_back();
	}

	unguard();
}

void clWorld::UpdateProfilingInfo()
{
	for ( clActorsList::const_iterator i = FActorsList.begin(); i != FActorsList.end(); ++i )
	{
		FProfileList[ i->second->GetObjectID() ] += i->second->GetTimeProfile();
	}
}

void clWorld::SaveProfilingInfo()
{
	iOStream* Stream = Env->FileSystem->CreateFileWriter( "actors.profile" );

	for ( clProfileList::const_iterator i = FProfileList.begin(); i != FProfileList.end(); ++i )
	{
		Stream->WriteLine( "Actor: '" + i->first + "': " + LStr::ToStr( i->second ) );
	}

	delete( Stream );
}

iActor* clWorld::FindActor( const LString& ID ) const
{
	clActorsList::const_iterator i = FActorsList.find( ID );

	if ( i != FActorsList.end() )
	{
		return i->second;
	}

	return NULL;
}

void clWorld::BroadcastEvent( const LString& EventName, bool Trigger, const LString& InstigatorID ) const
{
	for ( clActorsList::const_iterator i = FActorsList.begin(); i != FActorsList.end(); ++i )
	{
		iActor* Actor = i->second;

		if ( Actor->GetObjectID() == InstigatorID )
		{
			if ( Actor->ReceiveSelfEvents() )
			{
				Actor->Actor_Event( EventName, Trigger, InstigatorID );
			}
		}
		else
		{
			Actor->Actor_Event( EventName, Trigger, InstigatorID );
		}
	}
}

int clWorld::GetTotalActors() const
{
	return static_cast<int>( FIDsList.size() );
}

iActor* clWorld::GetActor( int N ) const
{
	FATAL( N >= static_cast<int>( FIDsList.size() ), "Invalid N" );

	LString Name = FIDsList[N];

	return FindActor( Name );
}

void clWorld::Event_DRAWOVERLAY( LEvent Event, const LEventArgs& Args )
{
	guard();

	if ( !FRenderingTechnique )
	{
		return;
	}

	if ( FScene )
	{
		// set transform
		::Linderdaum::Helpers::clActorOverlayRenderer ActorOverlayRenderer = ::Linderdaum::Helpers::clActorOverlayRenderer();

		FRenderingTechnique->Render( FScene );

		// render overlays
		FIXME( "restore overlays" )
		ActorOverlayRenderer.SetProjection( FScene->GetCameraProjection() );
		ActorOverlayRenderer.SetTransform( FScene->GetCameraTransform() );
		std::for_each( FActorsList.begin(), FActorsList.end(), ActorOverlayRenderer );
	}

	unguard();
}

void clWorld::Event_TIMER( LEvent Event, const LEventArgs& Args )
{
	DeletePendingActors();

	while ( !FActorsToAdd.empty() )
	{
		AddActorInternal( FActorsToAdd.back() );

		FActorsToAdd.pop_back();
	}

	const ::Linderdaum::Helpers::clActorTicker      ActorTicker( Args.FFloatArg, Env );

	const ::Linderdaum::Helpers::clActorPauseTicker ActorPauseTicker( Args.FFloatArg );

	if ( Env->IsPaused() )
	{
		std::for_each( FActorsList.begin(), FActorsList.end(), ActorPauseTicker );
	}
	else
	{
		std::for_each( FActorsList.begin(), FActorsList.end(), ActorTicker );
	}

	UpdateProfilingInfo();
}

/*
 * 28/06/2007
     Precaching works
 * 18/04/2007
     CreateActorFrom*() methods deprecated
 * 25/03/2007
     CreateActorFromPattern()
 * 14/03/2007
     GetTotalActors()
     GetActor()
 * 27/02/2007
     Built-in profiling capabilities
 * 14/02/2007
     BeginPlay()
 * 12/02/2007
     Improved serialization
 * 10/02/2007
     FindSceneNode()
     SaveToXLMLStream()
 * 06/02/2007
     Yet another bug in destructor fixed
 * 30/01/2007
     Bug in destructor fixed
 * 07/01/2007
     AttachNodeToSceneGraph()
     DetachNodeFromSceneGraph()
 * 06/01/2007
     Overlays rendering
 * 15/06/2005
     All actors correctly leave the world on its destruction
 * 05/05/2005
     FindPlayer()
     AddPlayer()
     DeletePlayer()
 * 22/02/2005
     It's here
*/
