/**
 * \file iObject.cpp
 * \brief Base object
 * \version 0.6.02
 * \date 13/10/2011
 * \author Sergey Kosarevsky, 2004-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "iObject.h"

#include "Engine.h"
#include "Utils/Mutex.h"
#include "Core/Logger.h"
#include "Core/Linker.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "Core/RTTI/iStaticClass.h"
#include "Core/RTTI/Method.h"
#include "Core/RTTI/Parameters.h"
#include "Core/VFS/ML.h"
#include "Environment.h"

class clPropertyMap : public std::map<LString, LString> {};
class clFieldMap    : public std::map<LString, void*>   {};

#if !defined(L_USE_LIBUUID)
#include <objbase.h>
LString MakeStringFromGUID( GUID FromGUID )
{
	const int GUID_BUFFER_LENGTH = 80;
	const char Format[] = "%0.8X-%0.4X-%0.4X-%0.2X%0.2X-%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X";
	char Buf[ GUID_BUFFER_LENGTH ];
	Lsnprintf(  Buf, GUID_BUFFER_LENGTH - 1, Format, FromGUID.Data1, FromGUID.Data2, FromGUID.Data3,
	            FromGUID.Data4[0], FromGUID.Data4[1], FromGUID.Data4[2], FromGUID.Data4[3],
	            FromGUID.Data4[4], FromGUID.Data4[5], FromGUID.Data4[6], FromGUID.Data4[7] );
	return LString( Buf );
}
#else
#include "Core/UUID.h"
#endif

#include <algorithm>

iIntrusiveCounter::iIntrusiveCounter(): FRefCounter( 0 )
{
}

void iIntrusiveCounter::IncRefCount()
{
#if defined(OS_WINDOWS)
	InterlockedIncrement( &FRefCounter );
#else
	__sync_fetch_and_add( &FRefCounter, 1 );
#endif
}

void iIntrusiveCounter::DecRefCount()
{
#if defined(OS_WINDOWS)

	if ( InterlockedDecrement( &FRefCounter ) == 0 ) { delete this; }

#else

	if ( __sync_sub_and_fetch( &FRefCounter, 1 ) == 0 ) { delete this; }

#endif
}

long iIntrusiveCounter::GetReferenceCounter() const
{
	return FRefCounter;
}

FIXME( "clFieldMap is slow" )

iObject::iObject(): Env( NULL ),
	FPrivateStaticClass( NULL ),
	FObjectID( "" ),
	FReturnValue( NULL ),
	FActiveState( NULL ),
	FFieldMap( NULL ),
	FPropertyMap( NULL ),
	FEventsMap( NULL ),
	FInheritedCall( 0 ),
	FMutex()
#if L_TRACK_ALLOCATION_CONTEXT
	, FAllocationContext( "unknown" )
#endif
{
};

void iObject::BeforeDestruction()
{
	if ( Env ) { Env->RemoveFromObjectsGraph( this ); }
}

void iObject::AfterConstruction()
{
	Env->AddToObjectsGraph( this );

#if L_TRACK_ALLOCATION_CONTEXT

	if ( Env->Logger ) { FAllocationContext = Env->Logger->GetCurrentProcsNesting(); }

#endif

}

void iObject::DisposeObject()
{
	delete this;
}

iObject::~iObject()
{
	if ( Env ) { SendSync( L_EVENT_OBJECT_DESTROYED, LEventArgs( this ), false ); }

	DoneEvents();

	// lock the mutex only after DoneEvents() to prevent deadlock
	LMutex Lock( &FMutex );

	if ( FFieldMap )
	{
		// deallocate fields
		for ( clFieldMap::iterator i = FFieldMap->begin() ; i != FFieldMap->end() ; ++i )
		{
			delete [] ( reinterpret_cast<char*>( i->second ) );
		}

		delete FFieldMap;
	}

	if ( FPropertyMap ) { delete FPropertyMap; }

	// never deallocate FReturnValue

	if ( Env )
	{
		Env->DisconnectObject( this );
		Env->RemoveFromObjectsGraph( this );
	}
};

LString iObject::NativeClassName() const
{
	LString Name( typeid( *this ).name() );

	GCC_CONVERT_TYPEINFO( Name );

	return Name;
}

LString iObject::ClassName() const
{
	if ( FPrivateStaticClass ) { return FPrivateStaticClass->ClassName(); }

	return NativeClassName();
}

void iObject::SetStaticClass( const iStaticClass* StaticClass )
{
	FPrivateStaticClass = const_cast<iStaticClass*>( StaticClass );
}

iStaticClass* iObject::GetStaticClass() const
{
	return FPrivateStaticClass;
}

LString iObject::GenerateGUID()
{
#if !defined(L_USE_LIBUUID)
	GUID G;

	CoCreateGuid( &G );

	return MakeStringFromGUID( G );
#else
	UUID_Next();

	return LString( uuid_buffer );
#endif
}

void* iObject::GetFieldPtr( const LString& FieldName )
{
	if ( FFieldMap && FFieldMap->count( FieldName ) > 0 )
	{
		return ( *FFieldMap )[FieldName];
	}

	return NULL;
}

void iObject::ResizeField( const LString& FieldName, int NewSize )
{
	if ( !FFieldMap ) { return; }

	char* ptr = new char[NewSize];

	( *FFieldMap )[FieldName] = reinterpret_cast<void*>( ptr );
}

void iObject::RegisterField( const LString& FieldName, int Size )
{
	if ( !FFieldMap )
	{
		FFieldMap = new clFieldMap();
	}

	( *FFieldMap )[FieldName] = NULL;

	if ( Size > 0 )
	{
		ResizeField( FieldName, Size );
	}
}

iParameter* iObject::GetReturnValue() const
{
	FATAL( FReturnValue == NULL, "Method was requested to return a value, but no iPararameter* was given" );

	return FReturnValue;
}

void iObject::SetReturnValue( iParameter* ReturnValue )
{
	FReturnValue = ReturnValue;
}

bool iObject::IsMethodOverriden( const LString& MethodName ) const
{
	iMethod* Method = GetStaticClass()->FindMethod( MethodName, false );

	return Method ? Method->IsScripted() : false;
};

clState* iObject::GetActiveState() const
{
	if ( FActiveState == NULL )
	{
		return GetStaticClass();
	}

	return FActiveState;
}

void iObject::SetActiveState( const LString& StateName )
{
	if ( StateName.empty() )
	{
		// set the default state and exit
		FActiveState = NULL;

		return;
	}

	FActiveState = GetStaticClass()->FindState( LStr::GetUpper( StateName ) );

	FATAL( FActiveState == NULL, "State \"" + StateName + "\" not found in class " + LString( ClassName() ) );
}

iMethod* iObject::FindMethodWithinStates( const LString& MethodName, int Inherited ) const
{
	/**
	   Methods search order:

	    1) look within active state
	    2) look within default state
	    3) return NULL - look base classes
	**/

	iMethod* Method = NULL;

	if ( FActiveState )
	{
		Method = FActiveState->FindMethod( MethodName, Inherited );
	}

	if ( Method == NULL )
	{
		if ( GetStaticClass() == NULL ) { return NULL; }

		Method = GetStaticClass()->FindMethod( MethodName, Inherited );
	}

	return Method;
}

bool iObject::CallMethod( const LString& MethodName, clParametersList& Params, int Inherited ) const
{
	guard( "%s", MethodName.c_str() );

	iMethod* Method = FindMethodWithinStates( MethodName, Inherited );

	if ( Method == NULL )
	{
		MethodMissing( MethodName, Params, Inherited );

		return false;
	}

	Method->Invoke( const_cast<iObject*>( this ), Params );

	return true;

	unguard();
}

iProperty* iObject::FindProperty( const LString& PropertyName )
{
	iStaticClass* Class = GetStaticClass();

	if ( !Class ) { return NULL; }

	iProperty* Prop = Class->FindProperty( PropertyName );

	return Prop;
}

void iObject::Event_CVARCHANGED( LEvent Event, const LEventArgs& Args )
{
	if ( !FPropertyMap ) { return; }

	clCVar* CVar = dynamic_cast<clCVar*>( Args.FObjArg );

	LString VarName = CVar->GetObjectID();

	SetPropertyValue( ( *FPropertyMap )[VarName], CVar->GetString() );
}

void iObject::BindPropertyToCVar( const LString& PropName, const LString& CVarName )
{
	if ( !FPropertyMap )
	{
		FPropertyMap = new clPropertyMap();
	}

	clCVar* CVar = Env->Console->GetVar( CVarName );

	( *FPropertyMap )[CVarName] = PropName;

	// Initially we have to load the value to CVar
	iProperty* P = this->FindProperty( PropName );
	CVar->SetString( P->GetValue( this ) );

	// Subscribe to the CVarChange events
	CVar->Connect( L_EVENT_CHANGED, Utils::Bind( &iObject::Event_CVARCHANGED, this ) );
}

LString iObject::GetPropertyValue( const LString& PropertyName )
{
	if ( iProperty* Prop = FindProperty( PropertyName ) ) { return Prop->GetValue( this ); }

	return "";
}

bool iObject::SetPropertyValue( const LString& PropertyName, const LString& PropertyValue )
{
	iProperty* Prop = FindProperty( PropertyName );

	if ( !Prop ) { return false; }

	return Prop->SetValue( this, PropertyValue );
}

iObject* iObject::DeepCopy()
{
	mlNode* Node = NULL;

	if ( !this->Save( &Node ) )
	{
		// Env->Logger->Log(L_DEBUG, "Serialization error");
		// FLastError = "Error serializing object: " + FLastError;
		return NULL;
	}

	iObject* NewObject = NULL;

	if ( !Env->Linker->LoadObject( Node, &NewObject ) )
	{
		// Env->Logger->Log(L_DEBUG, "Serialization error");
		// FLastError = "Error loading object: " + Env->Linker->FLastError;
		return NULL;
	}

	return NewObject;
}

bool iObject::Save( mlNode** Result )
{
	// Log error, return NULL
	if ( !BeginSave() ) { return NULL; }

	// simple redirection
	if ( !Env->Linker->SaveObject( this, Result ) ) { return false; }

	if ( EndSave() ) { return true; }

	// delete partially created tree
	delete *Result;

	return false;
}

bool iObject::Load( mlNode* Node )
{
	iObject* This = this;

	return Env->Linker->LoadObject( Node, &This );
}

bool iObject::LoadFromFile( const LString& FileName )
{
	iObject* This = this;

	return Env->Linker->LoadObjectFromFile( FileName, &This );
}

bool iObject::SaveToFile( const LString& FileName )
{
	return Env->Linker->SaveObjectToFile( FileName, this );
}

void iObject::InitEvents()
{
	LMutex Lock( &FMutex );

	if ( !FEventsMap )
	{
		FEventsMap = new LEventSubscriber();
	}
}

void iObject::DoneEvents()
{
	LMutex Lock( &FMutex );

	if ( FEventsMap ) { delete( FEventsMap ); }

	FEventsMap = NULL;
}

void iObject::Connect( LEvent Event, const LEventSubscriber::LEventHandler& Handler )
{
	InitEvents();

	FEventsMap->Connect( Event, Handler );
}

void iObject::ConnectWithTag( LEvent Event, const LEventSubscriber::LEventHandler& Handler, int Tag )
{
	InitEvents();

	FEventsMap->ConnectWithTag( Event, Handler, Tag );
}

void iObject::ConnectWithPriority( LEvent Event, const LEventSubscriber::LEventHandler& Handler, int Priority )
{
	InitEvents();

	FEventsMap->ConnectWithPriority( Event, Handler, Priority );
}

void iObject::Disconnect( LEvent Event, const LEventSubscriber::LEventHandler& Handler )
{
	if ( FEventsMap ) { FEventsMap->Disconnect( Event, Handler ); }
}

void iObject::DisconnectObject( iObject* Object )
{
	if ( FEventsMap ) { FEventsMap->DisconnectObject( Object ); }
}

void iObject::DisconnectAll()
{
	if ( FEventsMap ) { FEventsMap->DisconnectAll(); }
}

void iObject::SendSync( LEvent Event, const LEventArgs& Args, bool UseTag ) const
{
	if ( FEventsMap )
	{
		LEventArgs EventArgs( Args );

		EventArgs.FTimeStamp    = Env->GetSeconds();
		EventArgs.FTimeToHandle = EventArgs.FTimeStamp;

		FEventsMap->SendSync( Event, EventArgs, UseTag );
	}
}

void iObject::SendAsyncNoArgs( LEvent Event ) const
{
	SendAsync( Event, LEventArgs(), false );
}

void iObject::SendAsync( LEvent Event, const LEventArgs& Args, bool UseTag ) const
{
	SendAsyncDelayed( Event, Args, UseTag, 0.0 );
}

void iObject::SendAsyncDelayedNoArgs( LEvent Event, double DeltaSeconds ) const
{
	SendAsyncDelayed( Event, LEventArgs(), false, DeltaSeconds );
}

void iObject::SendAsyncDelayed( LEvent Event, const LEventArgs& Args, bool UseTag, double DeltaSeconds ) const
{
	if ( FEventsMap )
	{
		LEventArgs EventArgs( Args );

		EventArgs.FTimeStamp    = Env->GetSeconds();
		EventArgs.FTimeToHandle = EventArgs.FTimeStamp + DeltaSeconds;

		FEventsMap->SendAsync( Env, Event, EventArgs, UseTag );
	}
}

void iObject::SendAsyncCapsule( iAsyncCapsule* Capsule, double DeltaSeconds )
{
	double TimeStamp = Env->GetSeconds();

	Env->EnqueueCapsule( Capsule, TimeStamp, TimeStamp + DeltaSeconds );
}
/*
void iObject::SendAsyncCancelCapsule( iAsyncCapsule* Capsule )
{
}
*/
void iObject::SendAsyncCancel( LEvent Event )
{
	Env->CancelEvent( Event, this );
}


/*
 * 13/10/2011
     Events demultiplexing moved to Environment
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
 * 04/09/2010
     Timestamps set in Send()
 * 29/07/2010
     FEventsMap
 * 23/07/2010
     Thread-safe reference counting
 * 14/04/2009
     SubscribeEvent()
     UnSubscribeEvent()
 * 22/03/2009
     GetPropertyValue()
     SetPropertyValue()
 * 04/07/2007
     Allocation context tracking could be configurated
 * 13/04/2007
     AddSubObject()
     RemoveSubObject()
 * 04/04/2007
     GenerateGUID()
 * 16/03/2007
     iIntrusiveCounter logic changed
 * 30/01/2007
     Dispose() deprecated
 * 12/06/2005
     FInstClassName
 * 28/12/2004
     It's here
*/
