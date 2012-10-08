/**
 * \file iUpdater.cpp
 * \brief Generic updater interface
 * \version 0.5.91 ($Revision: 470 $)
 * \date 04/07/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "iUpdater.h"
#include "Scene.h"

void clAnimationUpdater::Update( float DeltaTime )
{
	if ( FScene->UpdateAnimation( FIdx, DeltaTime ) )
	{
		LEventArgs Args( FScene->GetKeyframe( FIdx )[3] );
		Args.FTag = FIdx;

		FScene->SendSync( L_EVENT_ANIMATIONCOMPLETE, Args, true );
	}
}

/*
 * 04/07/2010
     It's here
*/
