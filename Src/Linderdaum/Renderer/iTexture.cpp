/**
 * \file iTexture.cpp
 * \brief Texture interface
 * \version 0.5.91
 * \date 10/02/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Core/Logger.h"
#include "Utils/Exceptions.h"
#include "iTexture.h"
#include "iRenderContext.h"
#include "Environment.h"

iTexture::~iTexture()
{
	if ( FAttachedImage ) { FAttachedImage->DisconnectObject( this ); }

	if ( Env ) { Env->Renderer->RemoveTexture( this ); }
}

void iTexture::SetImage( clImage* TheImage )
{
	guard();

	if ( FAttachedImage ) { FAttachedImage->DisconnectObject( this ); }

	// If someone calls this twice the memory is lost
	FAttachedImage = TheImage;

	CommitChanges();

	if ( FAttachedImage ) { FAttachedImage->Connect( L_EVENT_CHANGED, BIND( &iTexture::Event_CHANGED ) ); }

	unguard();
}

void iTexture::Event_CHANGED( LEvent Event, const LEventArgs& Args )
{
	guard();

	Env->Logger->Log( L_DEBUG, ClassName() + " received Event_CHANGED for " + FAttachedImage->GetFileName() );

	CommitChanges();

	unguard();
}

clImage* iTexture::GetImage() const
{
	return FAttachedImage;
}

/*
 * 10/02/2010
     It's here
*/
