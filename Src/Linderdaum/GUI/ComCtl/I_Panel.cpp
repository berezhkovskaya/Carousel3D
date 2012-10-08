/**
 * \file I_Panel.cpp
 * \brief GUI panel
 * \version 0.5.74
 * \date 06/05/2007
 * \author Sergey Kosarevsky, 2005-2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "I_Panel.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/Logger.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "Images/Image.h"
#include "GUI/GUIManager.h"
#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"
#include "Resources/ResourcesManager.h"

clGUIPanel::clGUIPanel(): FHookSet( false ),
	FBevelRadius( 0 ),
	FShader( NULL ),
	FShaderMouseOver( NULL ),
	FShaderSwitchedOn( NULL ),
	FImage( NULL ),
	FImageMouseOver( NULL ),
	FImageSwitchedOn( NULL ),
	FGeometry( NULL ),
	FExecCommandsOnClick( false )
{
}

clGUIPanel::~clGUIPanel()
{
}

void clGUIPanel::AfterConstruction()
{
	FGeometry = Env->GUI->GetDefaultBevelRect();

	SetBevelRadius( Env->GUI->GetDefaultPanelBevelRadius() );
}

void clGUIPanel::SetBevelRadius( float Radius )
{
	FBevelRadius = Radius;

	FGeometry = Env->GUI->GetDefaultBevelRect();
}

void clGUIPanel::PreRender()
{
	guard();

	if ( !FShader )
	{
		ERROR_MSG( "Shader wasn't loaded in " + ClassName() );

		return;
	}

	clRenderState* Shader = FShader;

	if ( GetConsoleVariable() )
	{
		if ( GetConsoleVariable()->GetBool() )
		{
			if ( FShaderSwitchedOn ) { Shader = FShaderSwitchedOn; }
		}
	}

	UpdateUniforms( Shader );
	UpdateUniforms( FShaderMouseOver );

	Env->Renderer->AddBuffer( FGeometry, IsMouseOver() ? ( FShaderMouseOver ? FShaderMouseOver : Shader ) : Shader, 1, false );

	unguard();
}

void clGUIPanel::PostRender()
{
}

void clGUIPanel::SetShaderName( const LString& FileName )
{
	SetShader( Env->Resources->LoadShader( FileName ) );
}

void clGUIPanel::SetShaderMouseOverName( const LString& FileName )
{
	SetShaderMouseOver( Env->Resources->LoadShader( FileName ) );
}

void clGUIPanel::SetShaderSwitchedOnName( const LString& FileName )
{
	SetShaderSwitchedOn( Env->Resources->LoadShader( FileName ) );
}

void clGUIPanel::SetImage( clImage* Image )
{
	FImage = Image;

	if ( Image )
	{
		clRenderState* Shader = Env->Resources->CreateShader();

		if ( !FShader ) { SetShader( Env->Resources->LoadShader( "interface/shaders/panel.shader" ) ); }

		FShader->CopyTo( Shader );

		FShader = Shader;
		FShader->SetTexture( 0, Image->GetTexture(), false );
	}
}

void clGUIPanel::SetImageName( const LString& FileName )
{
	SetImage( Env->Resources->LoadImg( FileName, L_TEXTURE_2D ) );
}

void clGUIPanel::SetImageMouseOver( clImage* Image )
{
	FImageMouseOver = Image;

	if ( Image )
	{
		clRenderState* Shader = Env->Resources->CreateShader();

		if ( FShaderMouseOver ) { FShaderMouseOver->CopyTo( Shader ); }

		FShaderMouseOver = Shader;
		FShaderMouseOver->SetTexture( 0, Image->GetTexture(), false );
	}
}

void clGUIPanel::SetImageMouseOverName( const LString& FileName )
{
	SetImageMouseOver( Env->Resources->LoadImg( FileName, L_TEXTURE_2D ) );
}

void clGUIPanel::SetImageSwitchedOn( clImage* Image )
{
	FImageSwitchedOn = Image;

	if ( Image )
	{
		clRenderState* Shader = Env->Resources->CreateShader();

		if ( FShaderSwitchedOn ) { FShaderSwitchedOn->CopyTo( Shader ); }

		FShaderSwitchedOn = Shader;
		FShaderSwitchedOn->SetTexture( 0, Image->GetTexture(), false );
	}
}

void clGUIPanel::SetImageSwitchedOnName( const LString& FileName )
{
	SetImageSwitchedOn( Env->Resources->LoadImg( FileName, L_TEXTURE_2D ) );
}

bool clGUIPanel::Event_Key( iGUIResponder* Source, const int Key, const bool KeyState )
{
	if ( FHookSet )
	{
		if ( !KeyState )
		{
			FHook();
		}
	}

	return iGUIView::Event_Key( Source, Key, KeyState );
}

void clGUIPanel::Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState )
{
	iGUIView::Event_MouseLeft( Source, Pnt, KeyState );

	if ( FHookSet )
	{
		if ( !KeyState ) { FHook(); }
	}

	if ( KeyState && FExecCommandsOnClick )
	{
		if ( GetConsoleVariable() )
		{
			GetConsoleVariable()->SetBool( !GetConsoleVariable()->GetBool() );
		}

		ExecuteCommandsStack();
	}
}

void clGUIPanel::Event_LDoubleClick( iGUIResponder* Source, const LVector2& Pnt )
{
	iGUIView::Event_LDoubleClick( Source, Pnt );
}

/*
 * 06/05/2007
     SetShaderSwitchedOn()
 * 05/04/2007
     SaveToXLMLStream()
 * 08/03/2007
     Hooking of mouse clicks
 * 22/04/2005
     FShaderMouseOver
 * 16/04/2005
     AcceptParameter()
 * 30/01/2005
     It's here
*/
