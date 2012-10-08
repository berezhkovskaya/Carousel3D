/**
 * \file Event.cpp
 * \brief Base facilities for events
 * \version 0.5.98
 * \date 09/12/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Event.h"

#include <algorithm>

#include "Core/iObject.h"
#include "Utils/Utils.h"
#include "Generated/AsyncCapsule.h"

LEventSubscriber::sHandler::sHandler( LEvent Event, const LEventHandler& Handler, int Priority )
	: FEvent( Event ),
	  FHandler( Handler ),
	  FObject( reinterpret_cast<iObject*>( Handler.GetObjectPtr() ) ),
	  FPriority( Priority ), FTag( -1 )
{
}

bool LEventSubscriber::IsConnected( LEvent Event, const LEventHandler& Handler ) const
{
	LMutex Lock( &FMutex );

	for ( std::vector<sHandler>::const_iterator i = FHandlers.begin(); i != FHandlers.end(); ++i )
	{
		if ( ( *i ).FEvent == Event && ( *i ).FHandler.IsEqual( Handler ) )
		{
			return true;
		}
	}

	return false;
}

void LEventSubscriber::Connect( LEvent Event, const LEventHandler& Handler )
{
	ConnectWithPriority( Event, Handler, 0 );
}

void LEventSubscriber::ConnectWithTag( LEvent Event, const LEventHandler& Handler, int Tag )
{
	Lint64 i = ConnectWithPriority( Event, Handler, 0 );

	LMutex Lock( &FMutex );

	if ( i >= 0 ) { FHandlers[ static_cast<Luint>( i ) ].FTag = Tag; }
}

Lint64 LEventSubscriber::ConnectWithPriority( LEvent Event, const LEventHandler& Handler, int Priority )
{
	if ( IsConnected( Event, Handler ) )
	{
		return -1;
	}

	LMutex Lock( &FMutex );

	// insert according to priority class
	std::vector<sHandler>::iterator InsertPos = FHandlers.begin();

	for ( ; InsertPos != FHandlers.end(); ++InsertPos )
	{
		if ( ( *InsertPos ).FPriority > Priority )
		{
			break;
		}
	}

	InsertPos = FHandlers.insert( InsertPos, sHandler( Event, Handler, Priority ) );

	return ( InsertPos - FHandlers.begin() );
}

void LEventSubscriber::Disconnect( LEvent Event, const LEventHandler& Handler )
{
	LMutex Lock( &FMutex );

	for ( std::vector<sHandler>::iterator i = FHandlers.begin(); i != FHandlers.end(); ++i )
	{
		if ( ( *i ).FEvent == Event && ( *i ).FHandler.IsEqual( Handler ) )
		{
			FHandlers.erase( i );

			return;
		}
	}
}

void LEventSubscriber::DisconnectObject( iObject* Object )
{
	LMutex Lock( &FMutex );

	for ( std::vector<sHandler>::iterator i = FHandlers.begin(); i != FHandlers.end(); ++i )
	{
		if ( ( *i ).FHandler.IsObject( Object ) )
		{
			FHandlers.erase( i );

			return;
		}
	}
}

void LEventSubscriber::DisconnectAll()
{
	FPendingClear = true;
}

void LEventSubscriber::SendSync( LEvent Event, const LEventArgs& Args, bool UseTag )
{
	std::vector<sHandler> Handlers;

	{
		LMutex Lock( &FMutex );

		if ( FPendingClear )
		{
			FPendingClear = false;
			FHandlers.clear();
		}

		// make a clean copy to prevent deadlock (FMutex should be unlocked)
		Handlers = FHandlers;
	}

	if ( UseTag )
	{
		for ( std::vector<sHandler>::iterator i = Handlers.begin(); i != Handlers.end(); ++i )
		{
			if ( Args.FTag == ( *i ).FTag && ( *i ).FEvent == Event ) { ( *i ).FHandler( Event, Args ); }
		}
	}
	else
	{
		for ( std::vector<sHandler>::iterator i = Handlers.begin(); i != Handlers.end(); ++i )
		{
			if ( ( *i ).FEvent == Event ) { ( *i ).FHandler( Event, Args ); }
		}
	}
}

void LEventSubscriber::SendAsync( sEnvironment* Env, LEvent Event, const LEventArgs& Args, bool UseTag )
{
	LMutex Lock( &FMutex );

	if ( FPendingClear )
	{
		FPendingClear = false;
		FHandlers.clear();
	}

	if ( UseTag )
	{
		for ( std::vector<sHandler>::iterator i = FHandlers.begin(); i != FHandlers.end(); ++i )
		{
			if ( Args.FTag == ( *i ).FTag && ( *i ).FEvent == Event )
			{
				Env->EnqueueEvent( Event, Args, ( *i ).FHandler );
			}
		}
	}
	else
	{
		for ( std::vector<sHandler>::iterator i = FHandlers.begin(); i != FHandlers.end(); ++i )
		{
			if ( ( *i ).FEvent == Event )
			{
				Env->EnqueueEvent( Event, Args, ( *i ).FHandler );
			}
		}
	}
}

LEventSubscriber::sAsyncCall::sAsyncCall( iAsyncCapsule* Capsule, double TimeStamp, double TimeToHandle )
	: FEvent( L_EVENT_INVALID ), FArgs(), FHandler(), FCapsule( Capsule )
{
	FArgs.FTimeStamp    = TimeStamp;
	FArgs.FTimeToHandle = TimeToHandle;
}

void LEventSubscriber::sAsyncCall::Invoke()
{
	if ( FCapsule )
	{
		FCapsule->Invoke();

		delete( FCapsule );
		FCapsule = NULL;
	}
	else
	{
		FHandler( FEvent, FArgs );
	}
}

iAsyncQueue::iAsyncQueue()
	: FDemultiplexerMutex(),
	  FAsyncQueue( 0 )
{
}

void iAsyncQueue::EnqueueEvent( LEvent Event, const LEventArgs& Args, const LEventSubscriber::LEventHandler& Handler )
{
	LMutex Mutex( &FDemultiplexerMutex );

	FAsyncQueue.push_back( LEventSubscriber::sAsyncCall( Event, Args, Handler ) );
}

void iAsyncQueue::EnqueueCapsule( iAsyncCapsule* Capsule, double TimeStamp, double TimeToHandle )
{
	LMutex Mutex( &FDemultiplexerMutex );

	FAsyncQueue.push_back( LEventSubscriber::sAsyncCall( Capsule, TimeStamp, TimeToHandle ) );
}

void iAsyncQueue::DemultiplexEvents( double CurrentSeconds )
{
	FDemultiplexerMutex.Lock();

	clAsyncQueue LocalQueue( FAsyncQueue );

	FAsyncQueue.clear();

	clAsyncQueue::iterator End( LocalQueue.end() );

	for ( clAsyncQueue::iterator i = LocalQueue.begin(); i != End; ++i )
	{
		if ( ( *i ).FArgs.FTimeToHandle > CurrentSeconds ) { FAsyncQueue.push_back( ( *i ) ); }
	}

	FDemultiplexerMutex.Unlock();

	for ( clAsyncQueue::iterator i = LocalQueue.begin(); i != End; ++i )
	{
		LEventSubscriber::sAsyncCall& Call = ( *i );

		if ( Call.FArgs.FTimeToHandle <= CurrentSeconds ) { Call.Invoke(); }
	}
}

void iAsyncQueue::CancelEvent( LEvent Event, iObject* Receiver )
{
	LMutex Mutex( &FDemultiplexerMutex );

	clAsyncQueue::iterator End( FAsyncQueue.end() );

	for ( size_t i = 0; i != FAsyncQueue.size(); )
	{
		const LEventSubscriber::sAsyncCall& Call = FAsyncQueue[i];

		if ( Call.FEvent == Event && Call.FHandler.IsObject( Receiver ) )
		{
			FAsyncQueue[i] = FAsyncQueue.back();
			FAsyncQueue.pop_back();
			continue;
		}

		i++;
	}
}


/*
 * 09/12/2010
     SendSync()
 * 28/07/2010
     It's here
*/
