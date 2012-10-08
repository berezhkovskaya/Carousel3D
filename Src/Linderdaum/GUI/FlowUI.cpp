/**
 * \file FlowUI.cpp
 * \brief 3D flow list renderer
 * \version 0.6.02
 * \date 24/12/2011
 * \author Viktor Latypov, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "FlowUI.h"

void clFlowSelectRenderer::AfterConstruction()
{
	iObject::AfterConstruction();

	FDarkImage = Env->Resources->LoadImg( "Interface/3DList/Dark.png", L_TEXTURE_2D );
	FSelImage = Env->Resources->LoadImg( "Interface/3DList/Assembled.png", L_TEXTURE_2D );

	FShader = Env->Resources->CreateCustomShader( "Interface/3DList/3dlist.sp", "" );
	FShader_R = Env->Resources->CreateCustomShader( "Interface/3DList/3dlist_r.sp", "" );
	FShader_S = Env->Resources->CreateCustomShader( "Interface/3DList/3dlist_s.sp", "" );

	FShader->SetBlending( L_SRC_ALPHA, L_ONE_MINUS_SRC_ALPHA );
	FShader_R->SetBlending( L_SRC_ALPHA, L_ONE_MINUS_SRC_ALPHA );
	FShader_S->SetBlending( L_SRC_ALPHA, L_ONE_MINUS_SRC_ALPHA );

	FShader_R->SetTexture( 1, FDarkImage->GetTexture(), false );
	FShader_S->SetTexture( 1, FSelImage->GetTexture(), false );

	FShader->FDepthTest = FShader_R->FDepthTest = FShader_S->FDepthTest = true;
	FShader->FDepthWrites = FShader_R->FDepthWrites = FShader_S->FDepthWrites = true;
}

void clFlowSelectRenderer::SetTransforms( clFlowSelector* Control, float t_current, int CurImg )
{
	mtx4 Reflection = LPlane( vec3( 0, 0, 0 ), vec3( 0, 0, 1 ) ).Mirror();

	vec3 _Pt[4] =
	{
		vec3( -FQuadSize * 0.5f, -FQuadSize * 0.5f, 0.0f ),
		vec3( FQuadSize * 0.5f, -FQuadSize * 0.5f, 0.0f ),
		vec3( FQuadSize * 0.5f,  FQuadSize * 0.5f, 0.0f ),
		vec3( -FQuadSize * 0.5f,  FQuadSize * 0.5f, 0.0f )
	};

	int Num = ( int )FImages.size();

	float t_c = t_current;

//		t_c -= CurImg * FImageDist * FQuadSize;

	for ( int in_i = 0 ; in_i != Num ; in_i++ )
	{
		float t = t_c + ( float )( in_i /*- Num / 2*/ ) * OneImageSize() /*FImageDist * FQuadSize*/;

		mtx4 T = Control->GetQuadTransform( t, FQuadSize );
		mtx4 T_R = T * Reflection;

		for ( int j = 0 ; j < 4 ; j++ )
		{
			FCoords[j][in_i] = T * _Pt[j];
			FCoords_R[j][in_i] = T_R * _Pt[j];
		}
	}
}

void clFlowSelectRenderer::SetImages( const std::vector<clImage*>& Imgs )
{
	guard();

	FImages.resize( Imgs.size() );
	FSelected.resize( Imgs.size() );

	for ( int j = 0 ; j < 4 ; j++ )
	{
		FCoords[j].resize( Imgs.size() );
		FCoords_R[j].resize( Imgs.size() );
	}

	for ( size_t i = 0 ; i != Imgs.size(); i++ )
	{
		FImages[i] = Imgs[i];
		FSelected[i] = false;
	}

	unguard();
}

void clFlowSelectRenderer::RenderDirect( const mtx4& Projection, const mtx4& View, int CurImg, const LVector4& OverlayColor )
{
	clCanvas* C = Env->Renderer->GetCanvas();
	iShaderProgram* SPs[] = { FShader->GetShaderProgram(), FShader_R->GetShaderProgram(), FShader_S->GetShaderProgram() };

	for ( int i = 0 ; i < 3 ; i++ )
	{
		if ( SPs[i] )
		{
			SPs[i]->BindUniforms();
			SPs[i]->SetUniformNameVec4Array( "OverlayColor", 1, OverlayColor );
			SPs[i]->SetUniformNameMat4Array( "ProjectionMatrix", 1, Projection );
			SPs[i]->SetUniformNameMat4Array( "ModelViewMatrix", 1, View  );
		}
	}

	C->SetMatrices( Projection, View );

	int NumImg = ( int )FImages.size();

	// index = [curr - 2 .. curr + 2]

	/// Left -> Right -> Selected rendering order
	int ImgOrder[] = { CurImg - 3, CurImg - 2, CurImg - 1, CurImg + 3, CurImg + 2, CurImg + 1, CurImg };

	for ( int in_i = 0 ; in_i < 7 ; in_i++ )
	{
		int i = ImgOrder[in_i];

		if ( i < 0 ) { i += ( 1 - ( ( int )( i / NumImg ) ) ) * NumImg; }

		if ( i >= NumImg ) { i -= ( ( int )( i / NumImg ) ) * NumImg; }

		if ( i < NumImg && i > -1 )
		{
			FShader->SetTexture( 0, FImages[i]->GetTexture(), false );
			FShader_R->SetTexture( 0, FImages[i]->GetTexture(), false );
			FShader_S->SetTexture( 0, FImages[i]->GetTexture(), false );

			clRenderState* SS = IsSelected( i ) ? FShader_S : FShader;

			C->Rect3DShader( FCoords[0][i], FCoords[1][i], FCoords[2][i], FCoords[3][i], SS );
			C->Rect3DShader( FCoords_R[0][i], FCoords_R[1][i], FCoords_R[2][i], FCoords_R[3][i], FShader_R );
		}
	}

	C->Flush();
}
