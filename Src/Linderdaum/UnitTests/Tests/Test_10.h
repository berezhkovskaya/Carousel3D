#pragma once

#include "Engine.h"
#include "Core/CVars.h"
#include "Core/Console.h"

void Test_10( sEnvironment* Env )
{
	// Test mounting of .RAR files
	{
		//Env->Console->SendCommand( "Mount RESOURCE:testdata_rar" );
		Env->Console->SendCommand( "Mount ../../CommonMedia/Data_Tests/testdata.rar" );
		Env->Console->SendCommand( "Exec testres.cfg" );

		Env->Console->Event_TIMER( L_EVENT_TIMER, LEventArgs() );

		clCVar* Var = Env->Console->GetVar( "TestVar" );

		TEST_ASSERT( Var->GetString() != "Hello From Archive" );
	}

#if defined(_MSC_VER)
	// not for VS 2010
#if _MSC_VER < 1600
	// Test mounting of resources
	{
		Env->Console->SendCommand( "Exec RESOURCE:test_res.cfg" );

		Env->Console->Event_TIMER( L_EVENT_TIMER, LEventArgs( 0.0f ) );

		clCVar* Var = Env->Console->GetVar( "TestVar" );

		TEST_ASSERT( Var->GetString() != "Hello From Resource" );
	}
#endif
#endif

}

/*
 * 12/04/2005
     It's here
*/
