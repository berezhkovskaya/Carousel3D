#include "I_MtxView.h"

#include "Core/Console.h"
#include "Environment.h"
#include "Resources/ResourcesManager.h"

#include "Math/LMath.h"
#include "Renderer/iShaderProgram.h"
#include "Renderer/RenderState.h"

#include "LColors.h"

clGUIMtxView::clGUIMtxView():
	FLineThickness( 0.03f ),
	FMtxPtr( NULL ),
	FInVariable( "" )
{
}

void clGUIMtxView::Event_Timer( iGUIResponder* Source, float DeltaTime )
{
	clGUIPanel::Event_Timer( Source, DeltaTime );

	if ( FMtxPtr )
	{
		FView = *FMtxPtr;
	}
	else
	{
		if ( !FInVariable.empty() )
		{
			FView = Env->Console->GetVar( FInVariable )->GetMatrix4();
		}
		else
		{
			FView = LMatrix4::Identity();
		}
	}

	ViewSP->BindUniforms();
	ViewSP->SetUniformFloat( FThicknessUniform, FLineThickness );
	ViewSP->SetUniformMat4Array( FMatrixUniform, 1, FView );
}

void clGUIMtxView::AfterConstruction()
{
	clGUIPanel::AfterConstruction();

	ViewShader = Env->Resources->CreateCustomShader( "Interface/Programs/MtxView.sp", "" );

	FView = LMatrix4::Identity();

	SetShader( ViewShader );
	ViewSP = ViewShader->GetShaderProgram();

	FMatrixUniform       = ViewSP->CreateUniform( "ViewMtx" );
	FThicknessUniform = ViewSP->CreateUniform( "LineThickness" );

	ViewSP->SetUniformFloat( FThicknessUniform, FLineThickness );
	ViewSP->SetUniformMat4Array( FMatrixUniform, 1, FView );

	ViewShader->FBlended = true;
	ViewShader->SetBlending( L_SRC_ALPHA, L_ONE_MINUS_SRC_ALPHA );
}
