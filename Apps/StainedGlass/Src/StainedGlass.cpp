/**
 * \file StainedGlass.cpp
 * \brief StainedGlass
 * \version 1.0.00
 * \date 08/10/2012
 */

#include "Linderdaum.h"

sEnvironment* Env = NULL;

void DrawOverlay( LEvent Event, const LEventArgs& Args )
{
	Env->Renderer->GetCanvas()->Rect( 0.2f, 0.2f, 0.8f, 0.8f, LC_Red );
	Env->Renderer->GetCanvas()->Flush();
}

APPLICATION_ENTRY_POINT
{
	Env = new sEnvironment;

	Env->DeployDefaultEnvironment( NULL, "..\\..\\CommonMedia" );

	Env->Connect( L_EVENT_DRAWOVERLAY, Utils::Bind( &DrawOverlay ) );

	Env->RunApplication( DEFAULT_CONSOLE_AUTOEXEC );

	APPLICATION_EXIT_POINT( Env );
}

APPLICATION_SHUTDOWN
{
};

/*
 * 08/10/2012
     Created with Linderdaum ProjectWizard
*/
