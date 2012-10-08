#include "Behaviour.h"

#include "Core/VFS/iOStream.h"
#include "Core/VFS/FileSystem.h"
#include "Environment.h"

void clBehaviour::DumpDot( iOStream* Out )
{
	// mark the behaviour
	Out->WriteLine( GetObjectID() );
}

void clBehaviour::DumpDotFile( const LString& fname )
{
	iOStream* Out = Env->FileSystem->CreateFileWriter( fname );

	Out->WriteLine( "graph TheBehaviour {" );

	DumpDot( Out );

	Out->WriteLine( "}" );

	Out->DisposeObject();
}

eRetCode clSequence::Update( float dt )
{
	( void )dt;

	return B_FINISHED;
}

void clSelector::DumpDot( iOStream* Out )
{
}

void clSequence::DumpDot( iOStream* Out )
{
}

void clCompositeBehaviour::DumpDot( iOStream* Out )
{
	Out->WriteLine( LString( "subgraph " ) + GetObjectID() + LString( " {" ) );

	for ( size_t i = 0 ; i != FChildren.size() ; ++i )
	{
		FChildren[i]->DumpDot( Out );
	}

	Out->WriteLine( "}" );
}

void clCompositeBehaviour::AddBehaviour( clBehaviour* B )
{
	FChildren.push_back( B );
}

void clCompositeBehaviour::ClearBehaviours( bool ToDelete )
{
	if ( ToDelete )
		for ( size_t i = 0 ; i < FChildren.size() ; i++ )
		{
			delete FChildren[i];
		}

	FChildren.clear();
}

void clParallel::DumpDot( iOStream* Out )
{
	Out->WriteLine( LString( "subgraph " ) + GetObjectID() + LString( " {" ) );

	for ( size_t i = 0 ; i < FChildren.size() ; i++ )
	{
		delete FChildren[i];
	}

	Out->WriteLine( "}" );
}

int clSequence::SelectNext()
{
	if ( FCurrentBehaviour < 0 ) { return -1; }

	FCurrentBehaviour++;

	// finished
	if ( FCurrentBehaviour == ( int )FChildren.size() ) { return -1; }

	return FCurrentBehaviour;
}

eRetCode clSelector::Update( float dt )
{
	if ( FCurrentBehaviour == -1 ) { return B_FINISHED; }

	eRetCode Ret = FChildren[FCurrentBehaviour]->Update( dt );

	if ( Ret == B_FINISHED )
	{
		int Next = SelectNext();

		if ( Next < 0 )
		{
			FCurrentBehaviour = -1;
			return B_FINISHED;
		}

		FCurrentBehaviour = Next;
	}

	if ( Ret == B_FAILED || Ret == B_ERROR ) { return Ret; }

	return B_INPROGRESS;
}

eRetCode clParallel::Update( float dt )
{
	eRetCode Ret = B_FINISHED;

	for ( size_t i = 0 ; i < FChildren.size() ; i++ )
	{
		eRetCode LocalRet = FChildren[i]->Update( dt );

		if ( LocalRet == B_FAILED || LocalRet == B_ERROR )
		{
			FLastChildUpdated = ( int )i;
			return LocalRet;
		}

		if ( LocalRet != B_FINISHED ) { Ret = B_INPROGRESS; }
	}

	return Ret;
}
