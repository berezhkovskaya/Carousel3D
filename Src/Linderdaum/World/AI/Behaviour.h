#ifndef __Behvr__h__included__
#define __Behvr__h__included__

#include "Core/iObject.h"

class iOStream;

/// Completion code of a single update() cycle
enum eRetCode
{
	/// Finished, should switch to next
	B_FINISHED = 0,

	/// Not finished
	B_INPROGRESS = 1,

	/// Failed, must try another behaviour
	B_FAILED = 2,

	/// Fatal error
	B_ERROR = 3
};

class netexportable clBehaviour: public iObject
{
public:
	clBehaviour(): FParent( NULL ) {}
	virtual ~clBehaviour() {}

	scriptmethod void DumpDotFile( const LString& fname );

	scriptmethod void SetParent( clBehaviour* P ) { FParent = P; }
	scriptmethod clBehaviour* GetParent() const { return FParent; }

	scriptmethod void SetWeight( float W ) { FWeight = W; }
	scriptmethod float GetWeight() const { return FWeight; }

	/// Save the whole behaviour FSM as a .dot-file
	virtual void DumpDot( iOStream* Out );

	virtual eRetCode Update( float dt ) { return B_FINISHED; }

	/** Property(Name=Weight, Type=float, Getter=GetWeight, Setter=SetWeight) */
	/** Pro_perty(Name=Parent, Type=clBehaviour, Getter=GetParent, Setter=SetParent) */

public:
	float FWeight;

	/// "Parent" behaviour
	clBehaviour* FParent;
};

class netexportable clCompositeBehaviour: public clBehaviour
{
public:
	clCompositeBehaviour() {}
	virtual ~clCompositeBehaviour() { ClearBehaviours( true ); }

	virtual void DumpDot( iOStream* Out );

	scriptmethod void AddBehaviour( clBehaviour* B );
	scriptmethod void ClearBehaviours( bool ToDelete );

	scriptmethod size_t GetNumBehaviours() const { return FChildren.size(); }

	scriptmethod clBehaviour* GetBehaviour( int Index ) const { return FChildren[Index]; }
	scriptmethod void SetBehaviour( int Index, clBehaviour* Obj ) { FChildren[Index] = Obj; }

	/** Property(Name=SubObjects, Type=clBehaviour, IndexType=int, FieldName=FChildren, NetIndexedGetter=GetBehaviour, NetIndexedSetter=SetBehaviour) */

public:
	/// Internal list of sub-behaviours
	std::vector<clBehaviour*> FChildren;
};

/**
   \brief Selector (probability or priority) executor of a set of actions

   Terminates when every action finishes

   If some action fails, the whole Parallel Behaviour fails
*/
class netexportable clSelector: public clCompositeBehaviour
{
public:
	clSelector(): FCurrentBehaviour( -1 ) {}
	virtual ~clSelector() {}

	/// Restart action
	virtual void Restart() { FCurrentBehaviour = FChildren.size() == 0 ? -1 : 0; }

	virtual int SelectNext() { return -1; }

	virtual void DumpDot( iOStream* Out );

	virtual eRetCode Update( float dt );

	scriptmethod int GetCurrentBehaviour() const { return FCurrentBehaviour; }
	scriptmethod void SetCurrentBehaviour( int B ) { FCurrentBehaviour = B; }

	/** Property(Name=CurrentBehaviour, Type=int, Getter=GetCurrentBehaviour, Setter=SetCurrentBehaviour) */

public:
	int FCurrentBehaviour;
};

/**
   \brief Sequence selects the next behaviour
*/
class netexportable clSequence: public clSelector
{
public:
	clSequence() {}
	virtual ~clSequence() {}

	virtual void DumpDot( iOStream* Out );

	virtual int SelectNext();

	virtual eRetCode Update( float dt );
};

/**
   \brief Parallel executor of a number of actions

   Terminates when every action finishes.

   If some action fails, the whole Parallel Behaviour fails
*/
class netexportable clParallel: public clCompositeBehaviour
{
public:
	clParallel() {}
	virtual ~clParallel() {}

	virtual void DumpDot( iOStream* Out );

	virtual eRetCode Update( float dt );

public:
	/// Which sub-behaviour failed
	int FLastChildUpdated;
};

/**

Sequence

- If a child behavior succeeds, the sequence continues its execution.

- If a child behavior fails cleanly, then the sequence code backtracks to find other candidate behaviors.

- If a child behavior has an unexpected error, the sequence also returns that error since the search cannot continue.

Parallel

Selector (Probability, Priority)

- If a child behavior succeeds, the selector can terminate successfully also.

- In case a child behavior fails cleanly, the selector may backtrack and try the next child in order.

- When a critical error occurs, the selector should bail out with the same error.

*/

#endif
