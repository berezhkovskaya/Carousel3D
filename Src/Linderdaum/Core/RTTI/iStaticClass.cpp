/**
 * \file iStaticClass.cpp
 * \brief Represents a metaclass for the specified class
 * \version 0.5.99
 * \date 13/12/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "iStaticClass.h"

#include "Engine.h"
#include "Core/Linker.h"
#include "Core/iObject.h"
#include "Environment.h"

#include <algorithm>
#include <functional>

iStaticClass::~iStaticClass()
{
	::Linderdaum::Utils::DeallocatePairs( FStates.begin(), FStates.end() );
	::Linderdaum::Utils::DeallocatePairs( FFields.begin(), FFields.end() );
	::Linderdaum::Utils::DeallocatePairs( FProperties.begin(), FProperties.end() );

	FStates.clear();

	if(FMetaObject) { FMetaObject->DisposeObject(); FMetaObject = NULL; }
}

iObject* iStaticClass::GetMetaObject() const
{
	if ( !FMetaObject )
	{
		if ( IsAbstract() ) { return NULL; }

		FMetaObject = DefaultConstructor();
	}

	return FMetaObject;
}

bool iStaticClass::InheritsFrom( const LString& SClassName )
{
	if ( ClassName() == SClassName )
	{
		return true;
	}

	iStaticClass* ThisSuperClass = GetSuperClass();

	if ( ThisSuperClass != NULL )
	{
		return ThisSuperClass->InheritsFrom( SClassName );
	}

	return false;
}

void iStaticClass::RegisterState( clState* State )
{
	FATAL( FindState( State->GetStateName() ), "Duplicate state registration: " + State->GetStateName() );

	FStates[ State->GetStateName() ] = State ;
}

void iStaticClass::RegisterField( iField* Field )
{
	FATAL( FindField( Field->GetFieldName() ), "Duplicate field registration: " + Field->GetFieldName() );

	FFields[ Field->GetFieldName() ] = Field ;
}

void iStaticClass::RegisterProperty( iProperty* Property )
{
	FATAL( FindProperty( Property->GetName() ), "Duplicate property registration: " + Property->GetName() );

	FProperties [ Property->GetName() ] = Property;
}

iProperty* iStaticClass::FindProperty( const LString& PropertyName )
{
	iProperty* Property = NULL;

	// named property
	clPropertiesList::const_iterator i = FProperties.find( PropertyName );

	if ( i != FProperties.end() ) { Property = i->second; }

	if ( Property == NULL )
	{
		iStaticClass* SuperClass = GetSuperClass();

		if ( SuperClass ) { Property = SuperClass->FindProperty( PropertyName ); }
	}

	return Property;
}

iField* iStaticClass::FindField( const LString& FieldName )
{
	iField* Field = NULL;

	// named field
	clFieldsList::const_iterator i = FFields.find( FieldName );

	if ( i != FFields.end() )
	{
		Field = i->second;
	}

	if ( Field == NULL )
	{
		iStaticClass* SuperClass = GetSuperClass();

		if ( SuperClass )
		{
			Field = SuperClass->FindField( FieldName );
		}
	}

	return Field;
}

iMethod* iStaticClass::FindMethod( const LString& MethodName, int Inherited ) const
{
	iMethod* Method = ( Inherited > 0 ) ? NULL : clState::FindMethod( MethodName, false );

	if ( Method == NULL )
	{
		iStaticClass* SuperClass = GetSuperClass();

		if ( SuperClass )
		{
			Method = SuperClass->FindMethod( MethodName, Inherited > 0 ? Inherited - 1 : 0 );
		}
	}

	return Method;
}

iStaticClass* iStaticClass::GetSuperClass() const
{
	if ( FSuperClass == NULL )
	{
		if ( !FSuperClassName.empty() )
		{
			FSuperClass = Env->Linker->FindStaticClass( FSuperClassName );
		}
	}

	return FSuperClass;
}

clState* iStaticClass::FindState( const LString& StateName )
{
	// default state
	if ( StateName.empty() )
	{
		return this;
	}

	// named state
	clStatesList::const_iterator i = FStates.find( StateName );

	if ( i != FStates.end() )
	{
		return i->second;
	}

	// state not found
	return NULL;
}

void iStaticClass::SetSuperClass( iStaticClass* SuperClass )
{
	FSuperClass = SuperClass;
	// super state for "default" state
	SetSuperState( SuperClass );
	// super state for each other state
	clStatesList::const_iterator i = FStates.begin();

	for ( ; i != FStates.end() ; ++i )
	{
		// find state in super class with the same name
		clState* SuperStateForThisState = SuperClass->FindState( i->first );

		if ( SuperStateForThisState != NULL )
		{
			// assign the superstate
			i->second->SetSuperState( SuperStateForThisState );
		}
	}
};

void iStaticClass::RegisterNonNativeFields( iObject* Object )
{
	// do nothing for native classes
}

/// Get a total number of fields for this class (including ancestors)
int iStaticClass::GetFieldsCount() const
{
	return static_cast<int>( FLinearFieldsList.size() );
}

/// Get a field for specified index in a global (this + ancestors) class list
iField* iStaticClass::GetFieldByIndex( int Idx ) const
{
	return FLinearFieldsList[Idx];
}

int iStaticClass::GetMethodsCount() const
{
	return static_cast<int>( FLinearMethodsList.size() );
}

iMethod* iStaticClass::GetMethodByLinearIndex( int Idx ) const
{
	return FLinearMethodsList[Idx];
}

int iStaticClass::GetPropertiesCount() const
{
	return static_cast<int>( FLinearPropertiesList.size() );
}

iProperty* iStaticClass::GetPropertyByIndex( int Idx ) const
{
	return FLinearPropertiesList[Idx];
}

void iStaticClass::BuildVirtualFieldTable( int CurrentFieldIndex, clLinearFieldsList* LinearList )
{
	int FieldIndex = CurrentFieldIndex;

	// iterate fields in current class
	clFieldsList::iterator i = FFields.begin();

	for ( ; i != FFields.end() ; ++i )
	{
		// if there is no such field
		if ( FFieldNameToIndex.count( i->first ) == 0 )
		{
			// add this field
			FFieldNameToIndex[i->first] = FieldIndex;

			FieldIndex++;
		}

		// always add this to linear list
		LinearList->push_back( i->second );
	}

	// go to super class
	iStaticClass* ThisSuper = GetSuperClass();

	if ( ThisSuper != NULL )
	{
		ThisSuper->BuildVirtualFieldTable( FieldIndex, LinearList );
	}
}

void iStaticClass::BuildVirtualPropertyTable( int CurrentPropertyIndex, clLinearPropertiesList* LinearList )
{
	int PropertyIndex = CurrentPropertyIndex;

	// iterate properties in current metaclass
	clPropertiesList::iterator i = FProperties.begin();

	for ( ; i != FProperties.end() ; ++i )
	{
		// if there is no such property
		if ( FPropertyNameToIndex.count( i->first ) == 0 )
		{
			// add this property
			FPropertyNameToIndex[i->first] = PropertyIndex;

			PropertyIndex++;
		}

		// always add this to linear list
		LinearList->push_back( i->second );
	}

	// go to super class
	iStaticClass* ThisSuper = GetSuperClass();

	if ( ThisSuper != NULL )
	{
		ThisSuper->BuildVirtualPropertyTable( PropertyIndex, LinearList );
	}
}

void iStaticClass::BuildStateTable( int CurrentStateIndex, clLinearStatesList* LinearList )
{
	int StateIndex = CurrentStateIndex;

	for ( clStatesList::iterator i = FStates.begin() ; i != FStates.end() ; ++i )
	{
		if ( FStateNameToIndex.count( i->first ) == 0 )
		{
			FStateNameToIndex[i->first] = StateIndex;
			StateIndex++;
		}

		// always add this to linear list
		LinearList->push_back( i->second );
	}

	iStaticClass* ThisSuperClass = GetSuperClass();

	if ( ThisSuperClass != NULL )
	{
		ThisSuperClass->BuildStateTable( StateIndex, LinearList );
	}
}

void iStaticClass::BuildVirtualTables()
{
	// 1. build state_name->state_idx table
	FLinearStatesList.clear();
	this->BuildStateTable( 0, &FLinearStatesList );

	// 2. precompute iMethod* pointer table for each state

	// a) default state
	FLinearMethodsList.clear();
	this->BuildVirtualMethodTable( 0, &FLinearMethodsList );

	// b) each state
	for ( size_t i = 0 ; i != FLinearStatesList.size() ; ++i )
	{
		FLinearStatesList[i]->GetLinearMethodsListPtr()->clear();
		FLinearStatesList[i]->BuildVirtualMethodTable( 0, FLinearStatesList[i]->GetLinearMethodsListPtr() );
	}

	// 3. build iField* pointer table for each registered field
	FLinearFieldsList.clear();
	this->BuildVirtualFieldTable( 0, &FLinearFieldsList );

	// 4. build iProperty* pointer table for each registered property
	FLinearPropertiesList.clear();
	this->BuildVirtualPropertyTable( 0, &FLinearPropertiesList );
}

int iStaticClass::GetStateIndexByName( const LString& StateName ) const
{
	return FStateNameToIndex[StateName];
}

int iStaticClass::GetFieldIndexByName( const LString& FieldName ) const
{
	return FFieldNameToIndex[FieldName];
}

int iStaticClass::GetPropertyIndexByName( const LString& PropertyName ) const
{
	return FPropertyNameToIndex[PropertyName];
}

/*
 * 13/12/2010
     Constructors with parameters
 * 04/07/2010
     Preliminary support for properties
 * 01/04/2009
     Virtual Field/Method tables building API
 * 23/03/2009
     Property Get/Set interface
 * 13/06/2007
     Fields
 * 20/12/2005
     FindState()
 * 06/11/2005
     Scripted staticclass moved to a separate file
 * 05/11/2005
     LoadStaticClass()
 * 25/10/2005
     It's here
*/
