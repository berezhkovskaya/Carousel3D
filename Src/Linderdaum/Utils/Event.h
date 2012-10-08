/**
 * \file Event.h
 * \brief Base facilities for events
 * \version 0.6.00
 * \date 03/06/2011
 * \author Sergey Kosarevsky, 2010-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _Event__h_
#define _Event__h_

#include "Generated/Functors.h"
#include "LString.h"
#include "Utils/Mutex.h"

class iObject;
class iAsyncCapsule;

/// Event
enum LEvent
{

// Note: pointers are valid only inside the message handlers

#pragma region System events
	L_EVENT_TIMER             = 0,  // sent by Environment, indicates TimeQuantum seconds has passed
	//    FFloatArg - delta time
	L_EVENT_DRAWOVERLAY       = 1,  // sent by Environment when overlays has to be rendered
	L_EVENT_MOUSEMOVE         = 2,  // !!!not implemented!!!
	L_EVENT_KEY               = 3,  // sent by Environment on key up/down
	//    static_cast<int>( FFloatArg ) - key code
	//    FBoolArg - pressed/released
	L_EVENT_MOTION            = 4,  // sent by Environment on touchscreen motion, can handle multitouch
	//    static_cast<int>( FPtrArg ) - pointer ID
	//    FFloatArg - x position
	//    FFloatArg1 - y position
	//    FBoolArg - pressed/released
	//    FObjArg - LMotionFlag
	L_EVENT_LOG               = 5,  // sent by Logger, logging messages can be intercepted this way
	//    FPtrArg - pointer to std::string log message
	//    static_cast<int>(FFloatArg) - log level
	L_EVENT_FATAL             = 6,  // sent by Logger, application will be terminated instantly after this event is processed
	//    FPtrArg - pointer to std::string log message
	L_EVENT_ERROR             = 7,  // sent by Logger
	//    FPtrArg - pointer to std::string log message
	L_EVENT_POSTINIT          = 8,  // sent by Environment, after DeployEnvironment() finished work and before any config file was executed
	L_EVENT_CONSOLELOG        = 9,  // sent by Console in every DisplayMessage()
	//    FPtrArg - pointer to std::string log message
	L_EVENT_VIEWPORTRESIZED   = 10, // sent by Viewport
	//    no params
	L_EVENT_BEGINPLAY         = 11, // sent by Environment just before entering main loop
	//    no params
	L_EVENT_PAUSE             = 12, // sent by Environment on pause toggling
	//    FBoolArg - paused/unpaused
	L_EVENT_LOCALE_CHANGED    = 13, // sent by Environment if the system locale settings were changed
	L_EVENT_MENU              = 14, // sent by Environment if an item from the system menu was selected
	L_EVENT_FILE_SELECTED     = 15, // sent by Environment when a file selection dialog returns
	//    static_cast<int>( FFloatArg ) - menu ID
	L_EVENT_ACTIVATE          = 16, // sent by Environment when application gains or looses focus
	//    FBoolArg - activated/deactivated
	L_EVENT_SURFACE_ATTACHED  = 17, // sent by Environment when primary rendering surface is created
	L_EVENT_SURFACE_DETACHED  = 18, // sent by Environment when primary rendering surface is lost
	L_EVENT_SURFACE_CHANGED   = 19, // sent by Environment when primary rendering surface is changed (i.e. screen rotated from landscape to portrait)
#pragma endregion

#pragma region Inter-object events

	/**
	   \brief sent by Scene after animation completed
	   FFloatArg - current number of completed animation iterations
	   FTag - ID of geom instance
	*/
	L_EVENT_ANIMATIONCOMPLETE = 20,
	/**
	   \brief sent by Scene before rendering just after the framebuffer was cleared for normal pass
	   FObjArg - pointer to clScene
	*/
	L_EVENT_SCENE_PRERENDER   = 21,

	/**
	   \brief sent by Scene after rendering just before exiting RenderForward() / RenderDeffered()
	   FObjArg - pointer to clScene
	*/
	L_EVENT_SCENE_POSTRENDER  = 22,

	/**
	   \brief sent by any object that changed it's state (i.e. changed CVar, property, resource image loaded or vertex array changed, etc.)
	   FObjArg - pointer to object
	*/
	L_EVENT_CHANGED           = 23,

	/**
	   \brief sent by filesystem if file watcher detected file changes
	   FObjArg - pointer to clFileWatchHandle
	*/
	L_EVENT_FILE_NOTIFICATION = 24,

	/**
	   \brief sent after some lengthy process finishes (downloading, decompression, conversion etc.)
	*/
	L_EVENT_PROCESS_COMPLETE  = 25,

	/**
	   \brief sent periodicaly durimg some process supporting the information about its progress
	   FFloatArg - completion percentage
	   FIntArg   - completion ticks, if any
	*/
	L_EVENT_PROCESS_PROGRESS  = 26,

	/**
	   \brief Unsuccessful or preliminary termination
	*/
	L_EVENT_PROCESS_ABORTED   = 27,

	/**
	   \brief sent by iObject::~iObject() after destruction
	   FObjArg - pointer to object
	*/
	L_EVENT_OBJECT_DESTROYED  = 28,
#pragma endregion

	L_EVENT_USER              = 29, // not used by the Engine, can be used at will
	// this should always be the last event
	L_EVENT_INVALID           = 30
};

const int L_MOTION_START = -1;
const int L_MOTION_END   = -2;

enum LMotionFlag
{
	L_MOTION_MOVE = 0,
	L_MOTION_UP   = 1,
	L_MOTION_DOWN = 2
};

class LEventArgs
{
public:
	LEventArgs(): FPtrArg( NULL ),
		FFloatArg( 0.0f ),
		FFloatArg1( 0.0f ),
		FObjArg( NULL ),
		FBoolArg( false ),
		FTimeStamp( 0.0 ),
		FTimeToHandle( 0.0 ),
		FTag( -1 ) {};
	LEventArgs( const void* PtrParam,
	            float       FloatParam,
	            iObject*    ObjParam,
	            bool        BoolParam ): FPtrArg( PtrParam ),
		FFloatArg( FloatParam ),
		FFloatArg1( 0.0f ),
		FObjArg( ObjParam ),
		FBoolArg( BoolParam ),
		FTimeStamp( 0.0 ),
		FTimeToHandle( 0.0 ),
		FTag( -1 ) {};
	LEventArgs( int  IntParam,
	            bool BoolParam ): FPtrArg( NULL ),
		FFloatArg( static_cast<float>( IntParam ) ),
		FFloatArg1( 0.0f ),
		FObjArg( NULL ),
		FBoolArg( BoolParam ),
		FTimeStamp( 0.0 ),
		FTimeToHandle( 0.0 ),
		FTag( -1 ) {};
	explicit LEventArgs( void* PtrParam ): FPtrArg( PtrParam ),
		FFloatArg( 0.0f ),
		FFloatArg1( 0.0f ),
		FObjArg( NULL ),
		FBoolArg( false ),
		FTimeStamp( 0.0 ),
		FTimeToHandle( 0.0 ),
		FTag( -1 ) {};
	explicit LEventArgs( int IntParam ): FPtrArg( NULL ),
		FFloatArg( static_cast<float>( IntParam ) ),
		FFloatArg1( 0.0f ),
		FObjArg( NULL ),
		FBoolArg( false ),
		FTimeStamp( 0.0 ),
		FTimeToHandle( 0.0 ),
		FTag( -1 ) {};
	explicit LEventArgs( float FloatParam ): FPtrArg( NULL ),
		FFloatArg( FloatParam ),
		FFloatArg1( 0.0f ),
		FObjArg( NULL ),
		FBoolArg( false ),
		FTimeStamp( 0.0 ),
		FTimeToHandle( 0.0 ),
		FTag( -1 ) {};
	LEventArgs( int IntParam, float FloatParam, float FloatParam1, bool BoolParam ): FPtrArg( ( void* )( size_t )IntParam ),
		FFloatArg( FloatParam ),
		FFloatArg1( FloatParam1 ),
		FObjArg( NULL ),
		FBoolArg( BoolParam ),
		FTimeStamp( 0.0 ),
		FTimeToHandle( 0.0 ),
		FTag( -1 ) {};
	LEventArgs( float FloatParam, float FloatParam1 ): FPtrArg( NULL ),
		FFloatArg( FloatParam ),
		FFloatArg1( FloatParam1 ),
		FObjArg( NULL ),
		FBoolArg( false ),
		FTimeStamp( 0.0 ),
		FTimeToHandle( 0.0 ),
		FTag( -1 ) {};
	explicit LEventArgs( bool BoolParam ): FPtrArg( NULL ),
		FFloatArg( 0.0f ),
		FFloatArg1( 0.0f ),
		FObjArg( NULL ),
		FBoolArg( BoolParam ),
		FTimeStamp( 0.0 ),
		FTimeToHandle( 0.0 ),
		FTag( -1 ) {};
	explicit LEventArgs( iObject* ObjParam ): FPtrArg( ObjParam ),
		FFloatArg( 0.0f ),
		FFloatArg1( 0.0f ),
		FObjArg( ObjParam ),
		FBoolArg( false ),
		FTimeStamp( 0.0 ),
		FTimeToHandle( 0.0 ),
		FTag( -1 ) {};
public:
	const void* FPtrArg;
	float       FFloatArg;
	float       FFloatArg1;
	iObject*    FObjArg;
	bool        FBoolArg;
	double      FTimeStamp;
	double      FTimeToHandle;
	int         FTag;
};

class LEventSubscriber
{
public:
	typedef ::Linderdaum::Utils::clFunctor<void, TYPELIST_2( LEvent, const LEventArgs& ) > LEventHandler;
	struct sAsyncCall
	{
		sAsyncCall() : FEvent( L_EVENT_INVALID ), FArgs(), FHandler(), FCapsule( NULL ) {};
		sAsyncCall( iAsyncCapsule* Capsule, double TimeStamp, double TimeToHandle );
		sAsyncCall( LEvent Event, const LEventArgs& Args, const LEventHandler& Handler ) : FEvent( Event ), FArgs( Args ), FHandler( Handler ), FCapsule( NULL ) {};
		void Invoke();
		LEvent        FEvent;
		LEventArgs    FArgs;
		LEventHandler FHandler;
	private:
		iAsyncCapsule* FCapsule;
	};
public:
	LEventSubscriber():
		FHandlers( 0 ),
		FPendingClear( false ) {};
	explicit LEventSubscriber( LEvent Event ):
		FHandlers( 0 ),
		FPendingClear( false ) {};
	//
	// LEvent
	//
	Lint64  ConnectWithPriority( LEvent Event, const LEventHandler& Handler, int Priority );
	void    Connect( LEvent Event, const LEventHandler& Handler );
	void    ConnectWithTag( LEvent Event, const LEventHandler& Handler, int Tag );
	void    Disconnect( LEvent Event, const LEventHandler& Handler );
	void    DisconnectObject( iObject* Object );
	void    DisconnectAll();
	void    SendSync( LEvent Event, const LEventArgs& Args, bool UseTag );
	void    SendAsync( sEnvironment* Env, LEvent Event, const LEventArgs& Args, bool UseTag );
	bool    IsConnected( LEvent Event, const LEventHandler& Handler ) const;
private:
	struct sHandler
	{
		sHandler(): FEvent( L_EVENT_INVALID ), FHandler(), FObject( NULL ), FPriority( 0 ), FTag( -1 ) {};
		sHandler( LEvent Event, const LEventHandler& Handler, int Priority );
		LEvent        FEvent;
		LEventHandler FHandler;
		iObject*      FObject;
		int           FPriority;
		int           FTag;
	};
private:
	std::vector<sHandler>    FHandlers;
	clMutex                  FMutex;
	bool                     FPendingClear;
};

class scriptfinal iAsyncQueue
{
public:
	iAsyncQueue();

	/// Put the event into the events queue
	virtual void    EnqueueEvent( LEvent Event, const LEventArgs& Args, const LEventSubscriber::LEventHandler& Handler );

	/// Put the event into the events queue
	virtual void    EnqueueCapsule( iAsyncCapsule* Capsule, double TimeStamp, double TimeToHandle );

	/// Events demultiplexer as described in Reactor pattern
	virtual void    DemultiplexEvents( double CurrentSeconds );

	/// Remove all the events from queue
	virtual void    CancelEvent( LEvent Event, iObject* Receiver );
private:
	typedef std::vector<LEventSubscriber::sAsyncCall> clAsyncQueue;
	clAsyncQueue    FAsyncQueue;
	clMutex         FDemultiplexerMutex;
};

#endif

/*
 * 03/06/2011
     Removed text argument from event (use void* if needed)
 * 25/04/2011
     L_EVENT_LOCALE_CHANGED
 * 09/12/2010
     SendSync()
 * 04/09/2010
     Timestamp added
 * 28/07/2010
     It's here
*/
