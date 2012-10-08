/**
 * \file World.h
 * \brief Game world
 * \version 0.5.74
 * \date 18/04/2007
 * \author Sergey Kosarevsky, 2005-2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clWorld_
#define _clWorld_

#include "LString.h"

#include "iActor.h"

#include "Core/iObject.h"
#include "Scene/Postprocess/RenderingTechnique.h"

#include <map>
#include <list>

class clScene;

/// Container for iActor's
class netexportable scriptfinal clWorld: public iObject
{
public:
	clWorld();
	virtual ~clWorld();

	//
	// iObject interface
	//
	virtual void    AfterConstruction();

	//
	// clWorld
	//
	virtual void                    LoadRenderingTechnique( const LString& FileName );
	virtual void                    ReplaceRenderingTechnique( clRenderingTechnique* Technique );
	virtual clRenderingTechnique*   GetRenderingTechnique();
	virtual void                    BeginPlay();
	virtual clScene*                GetScene() const { return FScene; };
	virtual void                    AddActor( iActor* Actor );
	virtual void                    RemoveActor( iActor* Actor );
	virtual iActor*                 FindActor( const LString& ID ) const;
	virtual int                     GetTotalActors() const;
	virtual iActor*                 GetActor( int N ) const;
	virtual void                    SetActor( int i, iActor* A ) {}
	virtual void                    RegisterActor( iActor* Actor );
	virtual void                    UnRegisterActor( iActor* Actor );
	virtual void                    BroadcastEvent( const LString& EventName, bool Trigger, const LString& InstigatorID ) const;
	virtual void                    LoadWorld( const LString& FileName, const LString& SplashScreen );
	virtual void                    SaveWorld( const LString& FileName );
	// events
	FWD_EVENT_HANDLER( Event_TIMER );
	FWD_EVENT_HANDLER( Event_DRAWOVERLAY );
private:
	typedef std::vector<iActor*>        clGlobalActorsList;
	typedef std::map<LString, iActor*>  clActorsList;
	typedef std::map<LString, double>   clProfileList;
private:
	void       AddActorInternal( iActor* Actor );
	void       RemoveActorInternal( iActor* Actor );
	void       PrepareActor( iActor* Actor );
	void       ClearWorld();
	void       DeletePendingActors();
	void       UpdateProfilingInfo();
	void       SaveProfilingInfo();
	// command handlers
	virtual void    SaveWorldC( const LString& Param );
	virtual void    LoadWorldC( const LString& Param );
	virtual void    LoadRenderingTechniqueC( const LString& Param );
	virtual void    SpawnTemplateC( const LString& Param );
	virtual void    SaveSceneGraphC( const LString& Param );
public:
	/* Property(Name="GlobalActorsList", Type=iActor, IndexType=int, FieldName=FGlobalActorsList, NetIndexedGetter=GetActor, NetIndexedSetter=SetActor) */

	/// vector of all actors
	clGlobalActorsList       FGlobalActorsList;

	/// map ActorID -> iActor*
	clActorsList             FActorsList;
private:
	//
	clProfileList            FProfileList;
	LStr::clStringsVector    FIDsList;
	LStr::clStringsVector    FPrecacheFiles;
	std::vector<iActor*>     FActorsToAdd;
	std::vector<iActor*>     FActorsToDelete;
	clRenderingTechnique*    FRenderingTechnique;
	bool                     FPlaying;
	LString                  FTransformNodeName;
	int                      FCurrentOrder;
	clScene*                 FScene;
};

#endif

/*
 * 27/07/2007
     Merged with iWorld
 * 18/07/2007
     BroadcastEvent()
 * 18/04/2007
     CreateActorFrom*() methods deprecated
 * 08/04/2007
     CreateActorFromLoadedPattern()
 * 25/03/2007
     CreateActorFromPattern()
 * 14/03/2007
     GetTotalActors()
     GetActor()
 * 14/02/2007
     BeginPlay()
 * 13/02/2007
     Players deprecated
 * 10/02/2007
     FindSceneNode()
     SaveToXLMLStream()
 * 15/01/2007
     clActorPauseTicker
 * 07/01/2007
     AttachNodeToSceneGraph()
     DetachNodeFromSceneGraph()
 * 07/08/2006
     Inherited from iObject
 * 26/12/2005
     GetRenderingTechnique()
 * 27/06/2005
     ReplaceRenderingTechnique()
 * 05/05/2005
     FindPlayer()
 * 25/04/2005
     FindActor()
 * 22/02/2005
     It's here
*/
