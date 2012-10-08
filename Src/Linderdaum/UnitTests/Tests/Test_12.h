#pragma once

#include "Engine.h"
#include "Math/LRotator.h"
#include "Math/LProjection.h"
#include "GUI/GUIManager.h"
#include "Utils/Viewport.h"
#include "Scene/Picker.h"
#include "Scene/SceneManager.h"
#include "Renderer/iTexture.h"
#include "LColors.h"

void Test_11( sEnvironment* Env )
{
	// picking traverser
	{
		iRenderTarget* RT = Env->Renderer->CreateRenderTarget( 512, 512, 8, false, 1 );

		RT->BindRenderTarget( 0 );

		// add projection and Transform (translation by 0,0,-5)

		clGeom::CreateBox( 3, 2, 1 )->RenderDirect( Env->Resources->LoadShader( "test_box.shader" ) );

		RT->UnBindRenderTarget( 0 );

		clBitmap* Bitmap = clBitmap::CreateEmptyBitmap();

		RT->GetColorTexture( 0 )->SaveToBitmap( Bitmap );

		Bitmap->Save2DImage( "box.bmp" );
	}
}

/*
 * 23/03/2005
     It's here
*/
