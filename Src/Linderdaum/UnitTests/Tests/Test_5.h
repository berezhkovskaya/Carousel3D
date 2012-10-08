#pragma once

#include "Engine.h"
#include "Core/Linker.h"
#include "GUI/iGUIView.h"

void Test_5( sEnvironment* Env )
{
	// Test "special" functionality of iGUIRegion::SetHeight()
	{
		iGUIView* View = Env->Linker->Instantiate( "clGUIVerticalSplitter" );

		View->SetCoords( 1.0f,
		                 1.0f,
		                 11.0f,
		                 12.0f );

		TEST_ASSERT( View->GetWidth()           != 11.0f );
		TEST_ASSERT( View->GetHeight()          != 12.0f );
		TEST_ASSERT( View->GetMaximizedHeight() != 12.0f );

		View->SetHeight( 15.0f );

		// MaximizedHeight should remain unchanged
		TEST_ASSERT( View->GetHeight()          != 15.0f );
		TEST_ASSERT( View->GetMaximizedHeight() != 12.0f );

		delete( View );
	}
}

/*
 * 12/04/2005
     It's here
*/
