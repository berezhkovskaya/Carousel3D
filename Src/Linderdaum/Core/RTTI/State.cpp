/**
 * \file State.cpp
 * \brief State of the object in engine's reflection system
 * \version 0.5.60
 * \date 17/12/2005
 * \author Sergey Kosarevsky, 2005
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "State.h"
#include "Engine.h"

#include <algorithm>

clState::~clState()
{
//   ::Linderdaum::Utils::DeallocatePairs( FMethods.begin(), FMethods.end() );
	::Linderdaum::Utils::DeallocateAll( FMethods );

	FMethods.clear();
}

bool clState::EndLoad()
{
	// build NameToIndex table ?
	return true;
}

LString clState::GetSuperStateName()
{
	return "";//FSuperState
}

void clState::SetSuperStateName( const LString& SSName )
{
	//
}

void clState::RegisterMethod( iMethod* Method )
{
	Method->Env = Env;
	/*
	FATAL( FindMethod( Method->GetMethodName(), false ),
	       "Duplicate method registration: " + Method->GetMethodName() );
	       */
	clMethodsList::const_iterator i = std::find( FMethods.begin(),
	                                             FMethods.end(),
	                                             Method );

	if ( i == FMethods.end() )
	{
		FMethods.push_back( Method );
	}

	//FMethods[ Method->GetMethodName() ] = Method ;
}


iMethod* clState::FindMethod( const LString& MethodName, int Inherited ) const
{
	//clMethodsList::const_iterator i = FMethods.find( MethodName );
	clMethodsList::const_iterator i = FMethods.begin();

	for ( /*i = FMethods.begin()  ---   this is unnecessary ? */; i != FMethods.end();  ++i )
	{
		if ( ( *i )->GetMethodName() == MethodName )
		{
			break;
		}
	}

	//if ( i != FMethods.end() ) return i->second;
	if ( i != FMethods.end() )
	{
		return ( *i );
	}

	return NULL;
}

iMethod* clState::GetMethodByIndex( int idx, int Inherited ) const
{
	return FLinearMethodsList[idx];
}

int clState::GetMethodIndexByName( const LString& MethodName ) const
{
	return FMethodNameToIndex[MethodName];
}

void clState::BuildVirtualMethodTable( int CurrentMtdIndex, clMethodsList* LocalLinearMethodsList )
{
	int MtdIndex = CurrentMtdIndex;

	// reserve enough space for method pointers
//   LocalLinearMethodsList.resize(CurrentMtdIndex + FMethods.size());

	LString MethodName;

	// iterate every method
	clMethodsList::const_iterator i = FMethods.begin();

	for ( ; i != FMethods.end() ; ++i )
	{
		MethodName = ( *i )->GetMethodName();

		// if there is no such method yet
		if ( FMethodNameToIndex.count( MethodName ) == 0 )
		{
			// add this method
			FMethodNameToIndex[MethodName] = MtdIndex;
//         FLinearMethodsList[MtdIndex] = (*i);
			LocalLinearMethodsList->push_back( ( *i ) );

			MtdIndex++;
		}
	}

	// pass to superstate
	clState* ss = this->GetSuperState();

	if ( ss != NULL )
	{
		ss->BuildVirtualMethodTable( MtdIndex, LocalLinearMethodsList );
	}
}

/*
 * 01/04/2009
     VMT-related stuff
 * 28/11/2005
     Faster method lookup
 * 16/11/2005
     It's here
*/
