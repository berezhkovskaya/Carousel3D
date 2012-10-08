/**
 * \file Filter.cpp
 * \brief Postprocessing convolution filter implementation
 * \version 0.5.89
 * \date 12/01/2009
 * \author Sergey Kosarevsky, 2007-2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "Engine.h"
#include "Core/CVars.h"
#include "Core/Console.h"
#include "Utils/Viewport.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iRenderTarget.h"
#include "Renderer/Canvas.h"
#include "Renderer/RenderState.h"
#include "Renderer/iShaderProgram.h"
#include "Resources/ResourcesManager.h"

#include "Filter.h"

clFilterPostprocessor::clFilterPostprocessor()
{
}

void clFilterPostprocessor::AfterConstruction()
{
	guard();

	FFilterShader = Env->Renderer->GetEmptyShader();

	unguard();
}

void clFilterPostprocessor::SetFilterName( const LString& FilterName )
{
	FFilterShader = Env->Resources->LoadShader( FilterName );
}

LString clFilterPostprocessor::GetFilterName() const
{
	if ( FFilterShader )
	{
		return FFilterShader->GetFileName();
	}

	return "";
}

clFilterPostprocessor::~clFilterPostprocessor()
{
}

bool clFilterPostprocessor::Render( iRenderTarget* RenderTarget, // default input
                                    iRenderTarget* Input,        // custom input (NULL)
                                    iRenderTarget* Output )      // custom output (NULL)
{
	if ( !IsEnabled() ) { return false; }

	Output->Bind( 0 );

	FFilterShader->UpdateTextures( RenderTarget );

	if ( Input )
	{
		FFilterShader->SetTexture( 2, Input->GetColorTexture( 0 ), false );
		FFilterShader->SetTexture( 3, Input->GetDepthTexture(), false );
	}

	Env->Renderer->GetCanvas()->FullscreenRect( FFilterShader );

	Output->UnBind();

	return true;
}

/*
 * 12/01/2009
     GetInputRenderTargetName()
     GetOutputRenderTargetName()
 * 11/06/2007
     Effect on/off toggling
 * 28/04/2007
     It's here
*/
