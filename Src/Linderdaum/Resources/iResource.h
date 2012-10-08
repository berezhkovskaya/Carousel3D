/**
 * \file iResource.h
 * \brief Generic resource base class
 * \version 0.5.90
 * \date 17/01/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iResource_
#define _iResource_

#include "Platform.h"
#include "Core/iObject.h"

#include <list>

enum LResourceState
{
	/// resource is not linked to a file, dummy resource is available
	L_RESOURCE_EMPTY,
	/// background thread is loading this resource, dummy resource is available
	L_RESOURCE_LOADING,
	/// background thread finished loading and resource is available
	L_RESOURCE_LOADED,
	/// resource is linked to a file but unloaded, dummy resource is available
	L_RESOURCE_UNLOADED
};

class clFileWatchHandle;

/**
   Generic resource base class.
   Knows from what file it came from; what loader should be used to load it; where to cache it
**/
class scriptfinal netexportable iResource: public iObject
{
public:
	iResource();
	virtual ~iResource();

	//
	// iObject interface
	//
	virtual void       AfterConstruction();
	//
	// iResource
	//
#pragma region Resource caching
	virtual bool       IsCached();
	virtual bool       IsAsyncLoadComplete() const volatile { return FAsyncComplete; };
	virtual void       SetAsyncLoadComplete( bool Complete ) volatile;
	/// block until the background thread loads this resource
	virtual void       WaitLoad();
	/// start a background thread to load this resource if it is unloaded
	virtual void       Reload();
	/// unload this resource, can be brought back in by WaitLoad() or Preload()
	virtual void       Unload();
	virtual void       CacheTo( const LString& FileName );
	virtual bool       CacheFrom( const LString& FileName );
	virtual LString    GetCachingDir() const = 0;
	virtual LString    GetCachedFileName();
#pragma endregion

	virtual size_t     GetResourceID() const
	{
		return FResourceID;
	};
	virtual Lint64     GetFileTimeAtLoad() const
	{
		return FFileTimeAtLoad;
	};
	virtual LString    GetFileName() const
	{
		return FFileName;
	};
	virtual void       SetFileName( const LString& FileName );
	virtual bool       ReloadResource( bool Recache );
	virtual bool       IsSameResource( iResource* Other );

	friend class clResourcesManager;
	friend class clLoaderThread;
protected:
	virtual bool       Load( const LString& FileName, bool CacheResource );
	/// just do a simple deserialization by default
	virtual bool       LoadFromFile( const LString& FileName );
	/// override in subclasses to unload heavy data
	virtual bool       UnloadResource();

	virtual void       ResourceFileChanged( clFileWatchHandle* File );
private:
	FWD_EVENT_HANDLER( Event_FILE_NOTIFICATION );
private:
	volatile LResourceState FResourceState;
	volatile bool           FAsyncComplete;
	LString                 FFileName;
	Lint64                  FFileTimeAtLoad;
	size_t                  FResourceID;
	iResource*              FResourcePtr;
};

#endif

/*
 * 08/03/2010
     Timestamps for files
 * 17/01/2010
     Timestamps removed
 * 28/05/2009
     FindResourceInGraph()
     PerformLoading() deprecated
 * 04/09/2006
     ReloadResource()
 * 02/01/2005
     It's here
*/
