/**
 * \file iObject.h
 * \brief Base object (foundation of RTTI, events, nested structures)
 * \version 0.6.02
 * \date 30/08/2011
 * \author Sergey Kosarevsky, 2004-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iObject_
#define _iObject_

#include <deque>

#include "Engine.h"
#include "Platform.h"
#include "LString.h"

#include "iIntrusivePtr.h"
#include "Utils/Event.h"
#include "Utils/Mutex.h"
#include "Math/LVector.h"

// Save/Load method forwarding for old serialization scheme
#define SERIALIZABLE_CLASS()

// Mark the class as exported to .NET (LSDC will generate binding code for such classes)
#define NET_EXPORTABLE()

class iObject;
class iParameter;
class iProperty;
class iMethod;
class iStaticClass;
class clState;
class clFieldMap;
class clPropertyMap;
class clViewport;
class sEnvironment;
class mlNode;
class clMutex;

typedef std::vector<iParameter*>    clParametersList;

#define FWD_EVENT_HANDLER( handler ) virtual void handler(LEvent Event, const LEventArgs& Args)
#define EVENT_HANDLER( handler ) handler(LEvent Event, const LEventArgs& Args)

#define CONNECTOBJ( Event, Method, From ) Env->Connect( Event, Utils::Bind( Method, From ) )
#define CONNECT( Event, Method       ) Env->Connect( Event, Utils::Bind( Method, this ) )

/// Intrusive reference counter for garbage collection
class iIntrusiveCounter
{
public:
	iIntrusiveCounter();
protected:
	virtual ~iIntrusiveCounter() {}
public:
	//
	// iIntrusiveCounter
	//
	void    IncRefCount();
	void    DecRefCount();
	long    GetReferenceCounter() const;
private:
	volatile long    FRefCounter;
};

/**
   \brief Base class for every Engine's objects

   Provides hierarchy support, serialization, reflection and event handling
   Static methods track allocated objects

   - AfterConstruction(), BeforeDestruction() methods help to manage the construction and destruction order
   - Dispose() method ensures that the appropriate destructor code is called
     (this is crucial if we instantiate classes coming from dynamically loaded libraries)
**/
class iObject: public iIntrusiveCounter
{
public:
	/// Link to the environment that we are in (exported to script)
	nativefield    sEnvironment* Env;
private:
	iStaticClass*      FPrivateStaticClass;
	LString            FObjectID;
#if L_TRACK_ALLOCATION_CONTEXT
	LString            FAllocationContext;
#endif
private:
	// copy disabled (these methods should not be implemented)
	iObject( const iObject& Obj );
	iObject& operator = ( const iObject& Obj );
protected:
	virtual ~iObject();
public:
	iObject();

	NET_EXPORTABLE()

	scriptmethod  LString    GetObjectID() const { return FObjectID; };
	scriptmethod  void       SetObjectID( const LString& ID ) { FObjectID = ID; };

	/// Allocation tracking and memory leaks prevention
	scriptmethod  LString    GetAllocationContext() const
	{
#if L_TRACK_ALLOCATION_CONTEXT
		return FAllocationContext;
#else
		return "unknown(d)";
#endif
	};

#pragma region Lifetime management
	virtual void    AfterConstruction();
	virtual void    BeforeDestruction();
	/// DisposeObject() method is used by script. It just calls the destructor. Also it is useful with modular architechture
	virtual void    DisposeObject();
#pragma endregion

#pragma region Event handling
	virtual void    Connect( LEvent Event, const LEventSubscriber::LEventHandler& Handler );
	virtual void    ConnectWithTag( LEvent Event, const LEventSubscriber::LEventHandler& Handler, int Tag );
	virtual void    ConnectWithPriority( LEvent Event, const LEventSubscriber::LEventHandler& Handler, int Priority );
	virtual void    Disconnect( LEvent Event, const LEventSubscriber::LEventHandler& Handler );
	virtual void    DisconnectObject( iObject* Object );
	virtual void    DisconnectAll();
	virtual void    SendSync( LEvent Event, const LEventArgs& Args, bool UseTag ) const;

	/// Put event with empty parameters into all target objects' events queue
	virtual void    SendAsyncNoArgs( LEvent Event ) const;

	/// Put event into all target objects' events queue
	virtual void    SendAsync( LEvent Event, const LEventArgs& Args, bool UseTag ) const;

	/// Put event with empty parameters (to be processed after DeltaSeconds time has passed) into all target objects' events queue
	virtual void    SendAsyncDelayedNoArgs( LEvent Event, double DeltaSeconds ) const;

	/// Put event into all target objects' events queue
	virtual void    SendAsyncDelayed( LEvent Event, const LEventArgs& Args, bool UseTag, double DeltaSeconds ) const;

	/// Put the capsule into the Env's events queue
	virtual void    SendAsyncCapsule( iAsyncCapsule* Capsule, double DeltaSeconds );

	/// Cancel all async calls for this event
	virtual void    SendAsyncCancel( LEvent Event );
#pragma endregion

#pragma region Serialization routines

	/// Custom preparation for the loading process
	virtual bool    BeginLoad() { return true; }

	/// Custom termination of the loading process
	virtual bool    EndLoad() { return true; }

	/// Custom preparation for the saving process
	virtual bool    BeginSave() { return true; }

	/// Custom termination of the saving process
	virtual bool    EndSave() { return true; }

	/// Save object to ml tree
	bool       Save( mlNode** Node );

	/// Load object from ml tree
	bool       Load( mlNode*  Node );

	/// Wrapper method for file loading
	scriptmethod bool LoadFromFile( const LString& FileName );

	/// Wrapper method for file saving
	scriptmethod bool SaveToFile( const LString& FileName );
#pragma endregion

	//
	static LString    GenerateGUID();

#pragma region Reflection and Scripting

	/// Make an automatic copy of the object's properties using serialziation
	virtual iObject* DeepCopy();

	/// Try to set the value of the property for this object
	virtual bool    SetPropertyValue( const LString& PropertyName, const LString& PropertyValue );

	/// Get string representation of the property for this object (TODO: bool result type)
	virtual LString GetPropertyValue( const LString& PropertyName );

	/// Find property with specified name
	virtual iProperty*     FindProperty( const LString& PropertyName );

	/// Update property on every change of a CVar
	virtual void    BindPropertyToCVar( const LString& PropName, const LString& CVarName );

	/// The following methods are non-virtual and should be final to warranty LinderScript functionality

	/// Get the metaclass for this object
	virtual iStaticClass*    GetStaticClass() const;

	/// Set the metaclass for this object (used for non-native objects)
	virtual void    SetStaticClass( const iStaticClass* StaticClass );

	/// Actual class name for this object
	virtual LString    ClassName() const;

	/// The name of the native class (scripted classes are derived from natives, so the ClassName() function returns another name)
	virtual LString    NativeClassName() const;

	virtual iParameter*    GetReturnValue() const;
	virtual void           SetReturnValue( iParameter* ReturnValue );
	virtual bool           IsMethodOverriden( const LString& MethodName ) const;
	virtual bool           CallMethod( const LString& MethodName, clParametersList& Params, int Inherited ) const;
	virtual void           SetInheritedCall() const { FInheritedCall++; };
	virtual void           ResetInheritedCall() const { if ( FInheritedCall > 0 ) { FInheritedCall--; } };
	virtual int            GetInheritedCall() const { return FInheritedCall; };
	virtual void           SetActiveState( const LString& StateName );
	virtual clState*       GetActiveState() const;
	virtual iMethod*       FindMethodWithinStates( const LString& MethodName, int Inherited ) const;
#pragma endregion

#pragma region Support of "artificial" (non-native) fields

	/// Get pointer to the raw field buffer
	void*    GetFieldPtr( const LString& FieldName );

	/// Allocate new non-native field
	void     RegisterField( const LString& FieldName, int Size );

	/// Change size for the new non-native field
	void     ResizeField( const LString& FieldName, int NewSize );
#pragma endregion

	/* Property(Name="ID",      Type=string, Setter=SetObjectID, Getter=GetObjectID  ) */
	// _rop(Name=<unknown>, Type=<raw>,  Setter=UnknownToken                     ) */
protected:
	/// called by CallMethod() if named method was not found
	virtual void MethodMissing( const LString& MethodName, clParametersList& Params, int Inherited ) const {};

protected:
	/// Variable call flag for the script tunnelers
	mutable int    FInheritedCall;

	FWD_EVENT_HANDLER( Event_CVARCHANGED );
private:

#pragma region Initialize/deinitialize all events handlers
	void InitEvents();
	void DoneEvents();
#pragma endregion

	iParameter*                   FReturnValue;
	clState*                      FActiveState;
	clFieldMap*                   FFieldMap;
	clPropertyMap*                FPropertyMap;

	LEventSubscriber*             FEventsMap;
	mutable clMutex               FMutex;
};

#endif

/*
 * 12/10/2011
     SendAsyncDelayed()
     SendAsyncDelayedNoArgs()
 * 30/08/2011
     SendSync() -> SendAsync()
     Send()     -> SendSync()
 * 09/12/2010
     SendSync()
     EnqueueEvent()
     DemultiplexEvents()
 * 07/10/2010
     Children and parent removed
 * 11/08/2010
     MethodMissing()
 * 29/07/2010
     Connect()
     Disconnect()
     Send()
     FEventsMap
 * 13/07/2010
     Merged with iGUIResponder
 * 08/07/2010
     L_EVENT_ERROR
 * 01/05/2010
     Merged with iParametrizable
 * 15/01/2010
     L_EVENT_BEGINPLAY
     clEventsDispatcher
 * 14/04/2009
     SubscribeEvent()
     UnSubscribeEvent()
 * 14/03/2009
     Event_CONSOLELOG
 * 07/12/2007
     Events list extended
 * 29/11/2007
     Events are first-class citizens now
 * 25/07/2007
     AddParentObject()
     RemoveParentObject()
     GetTotalParentObjects()
     GetParentObject()
 * 11/06/2007
     ClearSubObjects() shortcut
 * 13/04/2007
     AddSubObject()
     RemoveSubObject()
     GetSubObjects()
     GetSubObjectsConst()
     GetParentObject()
 * 11/04/2007
     GetObjectID()
     SetObjectID()
 * 04/04/2007
     GenerateGUID()
 * 23/07/2005
     GetPrivateStaticClass()
     SetPrivateStaticClass()
 * 13/06/2005
     Initialization of FInstClassName with "iObject"
 * 13/01/2005
     Private copy constructor and operator= for iObject
 * 01/12/2004
     It's here
*/
