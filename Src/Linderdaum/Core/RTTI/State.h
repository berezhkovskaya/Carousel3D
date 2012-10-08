/**
 * \file State.h
 * \brief State of the object in engine's reflection system
 * \version 0.5.89a
 * \date 01/04/2009
 * \author Sergey Kosarevsky, 2005-2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clState_
#define _clState_

#include "Core/iObject.h"

#include "Method.h"

#include <functional>
#include <map>

/// State of the object in engine's reflection system
class scriptfinal clState: public iObject
{
public:
	typedef std::vector<iMethod*>    clMethodsList;
public:
	clState(): FStateName( "DEFAULT" ), FSuperState( NULL ) {};
	virtual ~clState();

	SERIALIZABLE_CLASS()
	NET_EXPORTABLE()

	//
	// clState
	//
	virtual LString        GetStateName() const { return FStateName; };
	virtual void           SetStateName( const LString& StateName ) { FStateName = StateName; };
	virtual void           RegisterMethod( iMethod* Method );
	virtual iMethod*       FindMethod( const LString& MethodName, int Inherited ) const;
	// techically, states may be inherited...
	// iStaticClass overrides this functionality
	virtual void           SetSuperState( clState* SuperState ) { FSuperState = SuperState; }
	virtual clState*       GetSuperState() const
	{
		return FSuperState;
	}
	virtual iMethod*       GetMethodByIndex( int idx, int Inherited ) const;
	virtual int            GetMethodIndexByName( const LString& MethodName ) const;

	scriptmethod iMethod*  GetLocalMethodByIndex( int idx ) const { return GetMethodByIndex( idx, 0 ); }
	virtual clMethodsList* GetLinearMethodsListPtr() { return &FLinearMethodsList; }

	// dummy method for .NET export
	virtual void           SetMethodByIndex( int i, iMethod* Mtd ) {}

	/** Property(Name=Methods, Type=iMethod, IndexType=int, FieldName=FMethods, NetIndexedGetter=GetLocalMethodByIndex, NetIndexedSetter=SetMethodByIndex) */
	/** Property(Name=StateName, Type=string, FieldName=FStateName) */
	/** Property(Name=SuperStateName, Type=string, Getter=GetSuperStateName, Setter=SetSuperStateName) */

	virtual LString GetSuperStateName();
	virtual void    SetSuperStateName( const LString& SSName );

	virtual bool    EndLoad();

	/// actual state name
	LString         FStateName;

	clMethodsList   FMethods;
public:
//   typedef std::map<LString, iMethod*>    clMethodsList;

	virtual void    BuildVirtualMethodTable( int CurrentMtdIndex, clMethodsList* LocalLinearMethodsList );
protected:
	//  VMT-related stuff
	typedef std::map<LString, int>   clMethodNameToIndexMap;
	mutable clMethodNameToIndexMap   FMethodNameToIndex;
	// linear list of method pointers
	clMethodsList                    FLinearMethodsList;
private:
	clState*   FSuperState;
};


#endif

/*
 * 26/09/2010
     Serialization
 * 01/04/2009
     VMT-related stuff
 * 06/07/2005
     It's here
*/
