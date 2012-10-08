#pragma once

#include "Linderdaum.h"

void Test_11( sEnvironment* Env )
{
	// bitmap load and save
	{
		clBitmap* Bitmap = clBitmap::CreateEmptyBitmap( Env );

		Bitmap->Load2DImage( Env, "Test11_LoadSave.bmp" );
		Bitmap->Save2DImage( Env, "Test11_LoadSave_test.bmp" );

		delete( Bitmap );

//      size_t Compare = Env->FileSystem->CompareFiles( "Test11_LoadSave.bmp", "Test11_LoadSave_test.bmp" );

//      TEST_ASSERT( Compare != 0 );
	}
	// test font rendering
	/*
	{
	   iRenderTarget* RT = Env->Renderer->CreateRenderTarget( 512, 512, 8, false, 1 );

	   RT->Bind(0);

	   Env->Context->AddTextString( Env->GUI->GetDefaultFontWidth(),
	                                Env->GUI->GetDefaultFontHeight(),
	                                "Test string",
	                                LC_Red,
	                                NULL );
	   RT->UnBind(0);

	   clBitmap* Bitmap = clBitmap::CreateEmptyBitmap();

	   RT->GetColorTexture(0)->SaveToBitmap( Bitmap );

	   Bitmap->Save2DImage( "text.bmp" );

	  // test RT updates
	  CheckRendererError("");

	  RT->GetColorTexture(0)->LoadFromBitmap( Bitmap );

	   CheckRendererError("");
	}
	*/
}

/*
 * 23/03/2005
     It's here
*/
