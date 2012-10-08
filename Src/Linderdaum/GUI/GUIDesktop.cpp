/**
 * \file GUIDesktop.cpp
 * \brief GUI desktop - root object
 * \version 0.5.60
 * \date 17/12/2005
 * \author Sergey Kosarevsky, 2005
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "GUIDesktop.h"

#include "LColors.h"
#include "Engine.h"
#include "Environment.h"
#include "Core/CVars.h"
#include "Core/Console.h"
#include "GUI/GUIManager.h"
#include "Utils/Viewport.h"
#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iShaderProgram.h"
#include "Resources/ResourcesManager.h"
#include "Geometry/GeomServ.h"

const LVector3 CursorSpots[4] =
{
	LVector3( 0.0f,             0.0f,            0.0f ),
	LVector3( 2.0f  / 1024.0f,  2.0f / 768.0f,   0.0f ),
	LVector3( 16.0f / 1024.0f, 16.0f / 768.0f,   0.0f ),
	LVector3( 16.0f / 1024.0f, 16.0f / 768.0f,   0.0f )
};

clGUIDesktop::clGUIDesktop(): FCursorGeometry( NULL ),
	FArrowCursorShader( NULL ),
	FSizeAllCursorShader( NULL ),
	FSizeNWSECursorShader( NULL )
{
}

void clGUIDesktop::AfterConstruction()
{
	guard();

	SetCoords( 0.0f, 0.0f, 1.0f, 1.0f );
	SetOpacity( 1.0f );

	//
	FShowFPS = Env->Console->GetVar( "ShowFPS" );
	FShowFPS->SetBool( true );

	FShowStats = Env->Console->GetVar( "ShowStats" );
	FShowStats->SetBool( false );

	FShowTraversers = Env->Console->GetVar( "ShowTraversers" );
	FShowTraversers->SetBool( false );

	FLastProjection = Env->Console->GetVar( "LastProjectionMatrix" );
	FLastProjection->SetMatrix4( LMatrix4::Identity() );

	FLastModelView  = Env->Console->GetVar( "LastModelViewMatrix" );
	FLastModelView->SetMatrix4( LMatrix4::Identity() );

	FShowModelView = Env->Console->GetVar( "ShowModelView" );
	FShowModelView->SetBool( false );

	FFPS = Env->Console->GetVar( "Stats.FPS" );
	FFPS->SetDouble( 0.0 );

	FUpdateTime = Env->Console->GetVar( "Stats.UpdateTime" );
	FUpdateTime->SetDouble( 0.0 );

	FUpdatesPerFrame = Env->Console->GetVar( "UpdatesPerFrame" );

	// from rendering context
	FDips = Env->Console->GetVar( "Stats.DipsPerFrame" );
	FDipsBlended = Env->Console->GetVar( "Stats.DipsBlendedPerFrame" );
	FStateChanges = Env->Console->GetVar( "Stats.StateChanges" );
	// from texture
	FTextureRebinds = Env->Console->GetVar( "Stats.TextureRebinds" );
	// from shader program
	FProgramRebinds = Env->Console->GetVar( "Stats.ProgramRebinds" );
	// from VBO vertex array
	FVALocksCount = Env->Console->GetVar( "Stats.VALocksCount" );
	// from framebuffer
	FAllocatedBytesRT = Env->Console->GetVar( "AllocatedBytesRT" );

	// from traversers
	FRenderingTime_T = Env->Console->GetVar( "Stats.RenderingTime_T" );

	unguard();
}

clGUIDesktop::~clGUIDesktop()
{
	delete( FCursorGeometry );
};

void clGUIDesktop::RenderCursor()
{
	ShowStats();

	const sMouseCursorInfo MCI = Env->Viewport->GetMouseCursorInfo();
	const LVector3 Translation = LVector3( MCI.FMouseX, MCI.FMouseY, 0.0f ) - CursorSpots[ Env->GUI->GetCursorForm() ];

	// lazy loading
	if ( ( Env->GUI->GetCursorForm() != CFORM_NOCURSOR ) && ( FArrowCursorShader == NULL ) )
	{
		FArrowCursorShader = Env->Resources->LoadShader( "Interface/Shaders/Cursors/Arrow.Shader" );
		FSizeAllCursorShader = Env->Resources->LoadShader( "Interface/Shaders/Cursors/SizeAll.Shader" );
		FSizeNWSECursorShader = Env->Resources->LoadShader( "Interface/Shaders/Cursors/SizeNWSE.Shader" );

		const float CURSOR_SIZE_X = ( 32.0f / 1024.0f );
		const float CURSOR_SIZE_Y = CURSOR_SIZE_X * Env->Viewport->GetAspectRatio();

		FCursorGeometry = Env->Renderer->AllocateEmptyVA();
		FCursorGeometry->SetVertexAttribs( clGeomServ::CreateRect2D( 0, 0, CURSOR_SIZE_X, CURSOR_SIZE_Y, 0, false, 0 ) );
	}

	clRenderState* Shader  = NULL;

	switch ( Env->GUI->GetCursorForm() )
	{
		case CFORM_NOCURSOR:
			break;
		case CFORM_ARROW:
			Shader = FArrowCursorShader;
			break;
		case CFORM_SIZEALL:
			Shader = FSizeAllCursorShader;
			break;
		case CFORM_SIZENWSE:
			Shader = FSizeNWSECursorShader;
			break;
		default:
			FATAL_MSG( "Unknown cursor form" );
	}

	if ( Shader )
	{
		Shader->GetShaderProgram()->BindUniforms();
		Shader->GetShaderProgram()->SetUniformNameVec3Array( "CURSOR_POS", 1, Translation );

		Env->Renderer->AddBuffer( FCursorGeometry, Shader, 1, false );
	}

	Env->Renderer->GetCanvas()->TextStrFreeType_Flush();
}

void clGUIDesktop::ShowStats()
{
	Env->Renderer->GetCanvas()->BeginTextPacket();

	// display FPS
	if ( FShowFPS->GetBool() )
	{
		double FPS = Env->GetCurrentTickRate();

		LString FPSStr = ( FPS > 2.0 ) ? LStr::ToStr( ( int )FPS ) : LStr::ToStr( FPS, 2 );

		Env->Renderer->GetCanvas()->TextStr( 0.85f, 0.010f, "FPS     : " + FPSStr, LC_White, NULL );

		if ( FPS > 0.0 )
		{
			Env->Renderer->GetCanvas()->TextStr( 0.85f, 0.035f, "Sec/Frm : " + LStr::ToStr( 1.0 / FPS ), LC_White, NULL );
		}

		Env->Renderer->GetCanvas()->TextStr( 0.85f, 0.060f, "Upd/Frm : " + LStr::ToStr( FUpdatesPerFrame->GetInt() ), LC_White, NULL );
	}

	// display stats
	if ( FShowStats->GetBool() )
	{
		const size_t TrianglesRenderered = Env->Renderer->GetRenderedTrianglesCount();
		const double MTrianglesPerSecond = 0.000001 * TrianglesRenderered * Env->GetCurrentTickRate();

		Env->Renderer->GetCanvas()->TextStr( 0.01f, 0.010f, "Triangles       : " + LStr::ToStr( TrianglesRenderered ), LC_White, NULL );
		Env->Renderer->GetCanvas()->TextStr( 0.01f, 0.035f, "MTriangles/sec  : " + LStr::ToStr( MTrianglesPerSecond ), LC_White, NULL );
		Env->Renderer->GetCanvas()->TextStr( 0.01f, 0.060f, "UpdateTime      : " + LStr::ToStr( FUpdateTime->GetDouble() ), LC_White, NULL );
		Env->Renderer->GetCanvas()->TextStr( 0.01f, 0.185f, "DIPs/Frame      : " + LStr::ToStr( FDips->GetInt() ), LC_White, NULL );
		Env->Renderer->GetCanvas()->TextStr( 0.01f, 0.210f, "DIPsBlend/Frame : " + LStr::ToStr( FDipsBlended->GetInt() ), LC_White, NULL );
		Env->Renderer->GetCanvas()->TextStr( 0.01f, 0.235f, "StateChgs/Frame : " + LStr::ToStr( FStateChanges->GetInt() ), LC_White, NULL );
		Env->Renderer->GetCanvas()->TextStr( 0.01f, 0.260f, "TextureReb/Frame: " + LStr::ToStr( FTextureRebinds->GetInt() ), LC_White, NULL );
		Env->Renderer->GetCanvas()->TextStr( 0.01f, 0.285f, "ProgramReb/Frame: " + LStr::ToStr( FProgramRebinds->GetInt() ), LC_White, NULL );
		Env->Renderer->GetCanvas()->TextStr( 0.01f, 0.310f, "VALocksCount    : " + LStr::ToStr( FVALocksCount->GetInt() ), LC_Red, NULL );
		Env->Renderer->GetCanvas()->TextStr( 0.01f, 0.335f, "AllocatedRT (Mb): " + LStr::ToStr( FAllocatedBytesRT->GetDouble() / ( 1024.0 * 1024.0 ), 1 ), LC_White, NULL );
		Env->Renderer->GetCanvas()->TextStr( 0.01f, 0.360f, "Free GPU memory : " + LStr::ToStr( ( double )Env->Renderer->GetFreeVideoMemory() / ( 1024.0 * 1024.0 ), 1 ), LC_White, NULL );
	}

	// display traversers
	if ( FShowTraversers->GetBool() )
	{
		Env->Renderer->GetCanvas()->TextStr( 0.01f, 0.390f, "Rendering   : " + LStr::ToStr( FRenderingTime_T->GetFloat() ), LC_White, NULL );
	}

	Env->Renderer->GetCanvas()->EndTextPacket();

	if ( FShowModelView->GetBool() )
	{
		Env->Renderer->GetCanvas()->DrawOrientation( FLastProjection->GetMatrix4(), FLastModelView->GetMatrix4() );
	}

	// reset per-frame counters
	FVALocksCount->SetInt( 0 );
	FRenderingTime_T->SetFloat( 0 );
	FStateChanges->SetInt( 0 );
	FTextureRebinds->SetInt( 0 );
	FProgramRebinds->SetInt( 0 );
}

/*
 * 15/05/2009
     Redundant dependencies remover (LGL)
 * 01/05/2009
     Merged with iGUIManager
 * 05/02/2005
     Different cursors support
 * 27/01/2005
     It's here
*/
