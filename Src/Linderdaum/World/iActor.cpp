/**
 * \file iActor.cpp
 * \brief Game actor
 * \version 0.5.72
 * \date 12/02/2007
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "iActor.h"
#include "World.h"

#include "Core/Console.h"
#include "Environment.h"

const int ACTOR_ORDER_LAST = 999999999;

iActor::iActor():
	FTimeProfile( 0 ),
	FOrder( ACTOR_ORDER_LAST ),
	FWorld( NULL )
{
}

void iActor::Actor_EnteredWorld( clWorld* World )
{
	FWorld = World;
}

void iActor::LeaveWorld()
{
	FWorld->RemoveActor( this );

	FWorld = NULL;
}

void iActor::SendEvent( const LString& EventName, bool Trigger ) const
{
	if ( FWorld )
	{
		FWorld->BroadcastEvent( EventName, Trigger, GetObjectID() );
	}
}

clScene* iActor::GetScene() const
{
	return FWorld->GetScene();
}

/*
 * 12/02/2007
     SaveToXLMLStream()
 * 05/05/2005
     Owners
 * 22/02/2005
     It's here
*/
