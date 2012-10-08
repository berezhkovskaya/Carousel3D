/**
 * \file Canvas.cpp
 * \brief Drawing canvas
 * \version 0.5.99
 * \date 16/12/2010
 * \author Sergey Kosarevsky, 2007-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Canvas.h"

#include "Environment.h"

#include "iRenderContext.h"
#include "iShaderProgram.h"
#include "iVertexArray.h"

#include "Core/CVars.h"
#include "Core/Linker.h"
#include "Core/Console.h"
#include "Geometry/VertexAttribs.h"
#include "Geometry/GeomServ.h"
#include "Geometry/Mesh.h"
#include "Geometry/Geom.h"

#include "Images/TextRenderer.h"
#include "Utils/Viewport.h"
#include "Utils/Localizer.h"
#include "GUI/GUIManager.h"
#include "Scene/Scene.h"
#include "Resources/ResourcesManager.h"
#include "Renderer/iTexture.h"

#include "Math/LMath.h"
#include "Math/LProjection.h"
#include "Math/LGeomUtils.h"
#include "Math/LTransform.h"
#include "Math/LMathStrings.h"

#include "Physics/RigidBody.h"
#include "Physics/CollisionShape.h"

#include "LColors.h"

#if defined( OS_ANDROID )
const int MAX_RENDER_LINES = 5000;
#else
const int MAX_RENDER_LINES = 100000;
#endif // OS_ANDROID

bool clFixedFontProperties::EndLoad()
{
	FFontShader = Env->Resources->LoadShader( FFontShaderName );

	// adjust text transparency
	bool HiRes = ( Env->Viewport->GetWidth() >= 1024 ) && ( Env->Viewport->GetHeight() >= 768 );

	FFontShader->GetShaderProgram()->BindUniforms();
	FFontShader->GetShaderProgram()->SetUniformNameFloat( "OUTLINE_DISCARD", HiRes ? 0.6f : 0.3f );

	return true;
}

bool clFreeTypeFontProperties::EndLoad()
{
	if ( !FFontName.empty() )
	{
		FFontFace = Env->Renderer->GetCanvas()->CreateFontFace( FFontName );
	}

	return true;
}

clFreeTypeFontProperties::~clFreeTypeFontProperties()
{
	delete( FFontFace );
}

LString clFreeTypeFontProperties::GetAlignment() const
{
	switch ( FAlignment )
	{
		case TextAlign_Left:
			return "Left";
		case TextAlign_Right:
			return "Right";
		case TextAlign_Center:
			return "Center";
		case TextAlign_CenterWidth:
			return "CenterWidth";
		case TextAlign_CenterHeight:
			return "CenterHeight";
		case TextAlign_FitWidth:
			return "FitWidth";
		case TextAlign_FitHeight:
			return "FitHeight";
		case TextAlign_Fit:
			return "Fit";
	}

	FATAL_MSG( "Unknown LTextAlignment value" );

	return "";
}

void clFreeTypeFontProperties::SetAlignment( const LString& Alignment )
{
	LString Align = LStr::GetUpper( Alignment );

	if ( Align == "LEFT"      ) { FAlignment = TextAlign_Left; }
	else if ( Align == "RIGHT"       ) { FAlignment = TextAlign_Right; }
	else if ( Align == "CENTER"      ) { FAlignment = TextAlign_Center; }
	else if ( Align == "CENTERWIDTH" ) { FAlignment = TextAlign_CenterWidth; }
	else if ( Align == "CENTERHEIGHT") { FAlignment = TextAlign_CenterHeight;}
	else if ( Align == "FITWIDTH"  ) { FAlignment = TextAlign_FitWidth; }
	else if ( Align == "FITHEIGHT" ) { FAlignment = TextAlign_FitHeight; }
	else if ( Align == "FIT"       ) { FAlignment = TextAlign_Fit; }
	else
	{
		FATAL_MSG( "Unknown alignment value" );
	}
}

clCanvas::clCanvas(): FMatrices(),
	FLinesShader( NULL ),
	FLineCount( 0 ),
	FScreenQuad( NULL ),
	FOverlayScene( NULL ),
	FTextShader( NULL ),
	FTextAtlasShader( NULL ),
	FTextVA( NULL ),
	FTextPacketStarted( false ),
	FTextPacketNumGlyphs( 0 ),
	FRect( NULL ),
	FDefaultFont( NULL ),
	FDefaultFreeTypeFont( NULL ),
	FTextRenderer( NULL ),
	FFontTexture( NULL ),
	FFontShader( NULL )
{
	FLinesArray[0] = NULL;
	FLinesArray[1] = NULL;
}

void clCanvas::Cleanup()
{
	delete( FTextVA );
	delete( FRect );
	delete( FLinesArray[0] );
	delete( FLinesArray[1] );
	delete( FScreenQuad );
	delete( FOverlayScene );
	delete( FDefaultFont );
	delete( FTextRenderer );
	delete( FFontTexture );

	for ( size_t i = 0; i != FTextFreeTypeGeometry.size(); i++ ) FTextFreeTypeGeometry[i].Done();
	FTextFreeTypeGeometry.clear();

	FTextVA = NULL;
	FRect = NULL;
	FLinesArray[0] = NULL;
	FLinesArray[1] = NULL;
	FScreenQuad = NULL;
	FOverlayScene = NULL;
	FDefaultFont = NULL;
	FDefaultFreeTypeFont = NULL;
	FTextRenderer = NULL;
	FFontTexture = NULL;
	FFontShader = NULL;
}

clCanvas::~clCanvas()
{
//	Cleanup();
}

void clCanvas::AfterConstruction()
{
}

void clCanvas::TurnOnLineClipping()
{
	if ( FLinesShader == NULL )
	{
		InitCanvas();
	}

	FLinesShader->FDepthTest = true;
	FLinesShader->FDepthWrites = true;
}

void clCanvas::TurnOffLineClipping()
{
	if ( FLinesShader == NULL )
	{
		InitCanvas();
	}

	FLinesShader->FDepthTest = false;
	FLinesShader->FDepthWrites = false;
}

clFixedFontProperties* clCanvas::GetDefaultFont() const
{
	if ( !FDefaultFont )
	{
		FDefaultFont  = Env->Linker->Instantiate( "clFixedFontProperties" );
	}

	return FDefaultFont;
}

void clCanvas::InitCanvas()
{
	guard();

	if ( !FLinesArray[0] )
	{
		FUseOverlayRendering = false;
		FTextShader   = FDefaultFont->GetFontShader();
		FTextAtlasShader = Env->Resources->LoadShader( "Shaders/text_atlas.shader" );

		FGlyphSizeX = Env->Console->GetVarDefaultFloat( "Glyph.SizeX", 0.4f );
		FGlyphSizeY = Env->Console->GetVarDefaultFloat( "Glyph.SizeY", 1.0f );
		FFontWidth  = Env->Console->GetVarDefaultFloat( "Font.Width", 3.0f / 128.0f );
		FFontHeight = Env->Console->GetVarDefaultFloat( "Font.Height", 4.0f / 128.0f );

		FTextRenderer = Env->Linker->Instantiate( "clTextRenderer" );
		FTextRenderer->SetMaskMode( true );

		FDefaultFontName = Env->Console->GetVarValueStr( "Canvas.Font", "default.ttf" );

		//

		FCurrentLinesArray = 0;

		FRect = Env->Renderer->AllocateEmptyVA();
		FRect->SetVertexAttribs( clVertexAttribs::Create( 4, L_TEXCOORDS_BIT ) );
		FRect->GetVertexAttribs()->FPrimitiveType = L_PT_TRIANGLE_STRIP;
		FRect->GetVertexAttribs()->FDynamic = true;

		clVertexAttribs* VA = FRect->GetVertexAttribs();

		VA->FVertices.GetPtr()[0] = LVector3( 0, 0, 0 );
		VA->FVertices.GetPtr()[1] = LVector3( 0, 1, 0 );
		VA->FVertices.GetPtr()[2] = LVector3( 1, 0, 0 );
		VA->FVertices.GetPtr()[3] = LVector3( 1, 1, 0 );

		VA->FTexCoords.GetPtr()[0] = LVector4( 0, 0, 0, 0 );
		VA->FTexCoords.GetPtr()[1] = LVector4( 0, 1, 0, 0 );
		VA->FTexCoords.GetPtr()[2] = LVector4( 1, 0, 0, 0 );
		VA->FTexCoords.GetPtr()[3] = LVector4( 1, 1, 0, 0 );

		FRect->CommitChanges();

		FLinesArray[0] = Env->Renderer->AllocateEmptyVA();
		FLinesArray[0]->SetVertexAttribs( clVertexAttribs::Create( 2 * MAX_RENDER_LINES, L_TEXCOORDS_BIT ) );
		FLinesArray[0]->GetVertexAttribs()->FPrimitiveType = L_PT_LINE;
		FLinesArray[0]->GetVertexAttribs()->FDynamic = true;

		FLinesArray[1] = Env->Renderer->AllocateEmptyVA();
		FLinesArray[1]->SetVertexAttribs( clVertexAttribs::Create( 2 * MAX_RENDER_LINES, L_TEXCOORDS_BIT ) );
		FLinesArray[1]->GetVertexAttribs()->FPrimitiveType = L_PT_LINE;
		FLinesArray[1]->GetVertexAttribs()->FDynamic = true;

		FLinesShader = Env->Resources->CreateCustomShader( "Shaders/Canvas/line.sp", "" );

		FGlowLinesShader = Env->Resources->CreateCustomShader( "Shaders/Canvas/lineglow.sp", "" );
		FGlowLinesShader->FDepthTest = false;
		FGlowLinesShader->FBlended = true;
		FGlowLinesShader->FBlendSrc = L_SRC_ALPHA;
		FGlowLinesShader->FBlendDst = L_ONE_MINUS_SRC_ALPHA;
		FGlowLinesShader->SetTexture( 0, Env->Resources->LoadImg( "Shaders/Canvas/Glow.png", L_TEXTURE_2D )->GetTexture(), false );

		// линии должны рисоваться с отсечением, хотя иногда может понадобиться отключать эту функцию
		TurnOnLineClipping();

		FRectShader  = Env->Resources->CreateCustomShader( "Shaders/Canvas/rect.sp", "" );
		FRectShader->FDepthTest = false;
		FRectShader->FBlended = true;
		FRectShader->FBlendSrc = L_SRC_ALPHA;
		FRectShader->FBlendDst = L_ONE_MINUS_SRC_ALPHA;

		FTexRectShader = Env->Resources->CreateCustomShader( "Shaders/Canvas/texrect.sp", "" );
		FTexRectShader->FDepthTest = false;
		FTexRectShader->FBlended = true;
		FTexRectShader->FBlendSrc = L_SRC_ALPHA;
		FTexRectShader->FBlendDst = L_ONE_MINUS_SRC_ALPHA;

		FTexRectShader3D = Env->Resources->CreateCustomShader( "Shaders/Canvas/texrect3d.sp", "" );
		FTexRectShader3D->FDepthTest = false;

		FFontShader  = Env->Resources->LoadShader( "Shaders/text_cached.shader" );

		iShaderProgram* SP = FLinesShader->GetShaderProgram();

		if ( SP )
		{
			FProjectionMatrixUniform = SP->CreateUniform( "ProjectionMatrix" );
			FModelViewMatrixUniform  = SP->CreateUniform( "ModelViewMatrix"  );
		}

		SP = FTexRectShader3D->GetShaderProgram();

		if ( SP )
		{
			FProjectionMatrixUniformTex = SP->CreateUniform( "ProjectionMatrix" );
			FModelViewMatrixUniformTex  = SP->CreateUniform( "ModelViewMatrix"  );
		}

		FRectColorUniform = FRectShader->GetShaderProgram()->CreateUniform( "RectColor" );
		FRectPosUniform   = FRectShader->GetShaderProgram()->CreateUniform( "RectPos" );

		FTexRectPosUniform = FTexRectShader->GetShaderProgram()->CreateUniform( "RectPos" );
		FTexRectColorUniform = FTexRectShader->GetShaderProgram()->CreateUniform( "RectColor" );
		FTexRectTilesUniform = FTexRectShader->GetShaderProgram()->CreateUniform( "RectTiles" );
		FTexRectUVUniform = FTexRectShader->GetShaderProgram()->CreateUniform( "RectUV" );
	}

	if ( FTextVA == NULL )
	{
		// enough to render 4096 glyphs as triangles
		const int MaxGlyphs = 4096;

		clVertexAttribs* Geometry = clVertexAttribs::Create( 6 * MaxGlyphs, L_TEXCOORDS_BIT | L_COLORS_BIT );

		// pregenerate triangles
		for ( int i = 0; i != MaxGlyphs; i++ )
		{
			Geometry->FVertices.GetPtr()[ 6 * i + 0 ] = LVector3( 0.0f + i, 0.0f, 0.0f );
			Geometry->FVertices.GetPtr()[ 6 * i + 1 ] = LVector3( 0.0f + i, 1.0f, 0.0f );
			Geometry->FVertices.GetPtr()[ 6 * i + 2 ] = LVector3( 1.0f + i, 0.0f, 0.0f );
			Geometry->FVertices.GetPtr()[ 6 * i + 3 ] = LVector3( 0.0f + i, 1.0f, 0.0f );
			Geometry->FVertices.GetPtr()[ 6 * i + 4 ] = LVector3( 1.0f + i, 0.0f, 0.0f );
			Geometry->FVertices.GetPtr()[ 6 * i + 5 ] = LVector3( 1.0f + i, 1.0f, 0.0f );
		}

		FTextVA = Env->Renderer->AllocateEmptyVA();
		FTextVA->SetVertexAttribs( Geometry );
	}

	FLineCount = 0;

	unguard();
}

void clCanvas::SetMatrices( const LMatrix4& TheProjection, const LMatrix4& TheModelView )
{
	if ( !FLinesShader )
	{
		InitCanvas();
	}

	iShaderProgram* SP = FLinesShader->GetShaderProgram();

	if ( SP )
	{
		SP->BindUniforms();
		SP->SetUniformMat4Array( FProjectionMatrixUniform, 1, TheProjection );
		SP->SetUniformMat4Array( FModelViewMatrixUniform, 1, TheModelView  );
	}

	SP = FTexRectShader3D->GetShaderProgram();

	if ( SP )
	{
		SP->BindUniforms();
		SP->SetUniformMat4Array( FProjectionMatrixUniformTex, 1, TheProjection );
		SP->SetUniformMat4Array( FModelViewMatrixUniformTex, 1, TheModelView  );
//      SP->SetUniformNameMat4( "ProjectionMatrix", TheProjection );
//      SP->SetUniformNameMat4( "ModelViewMatrix",  TheModelView  );

		/*
		   // LV : gui shows some nice performance with these lines uncommented

		     FMatrices.SetModelMatrix( LMatrix4::Identity() );
		     FMatrices.SetViewMatrix( TheModelView );
		      FMatrices.SetProjectionMatrix( TheProjection );
		      FMatrices.UpdateMatricesBuffer();
		   */
	}

	SP = FGlowLinesShader->GetShaderProgram();

	if ( SP )
	{
		SP->BindUniforms();
		SP->SetUniformNameMat4Array( "ProjectionMatrix", 1, TheProjection );
		SP->SetUniformNameMat4Array( "ModelViewMatrix", 1, TheModelView  );
	}
}

void clCanvas::Line3D( const LVector3& Point1,
                       const LVector3& Point2,
                       const LVector4& Color )
{
	if ( !FLinesArray[0] )
	{
		InitCanvas();
	}

	// force flush on buffer overflow
	if ( FLineCount > MAX_RENDER_LINES )
	{
		Flush();
	}

	int Idx0 = FLineCount * 2;
	int Idx1 = Idx0 + 1;

	clVertexAttribs* VA = FLinesArray[FCurrentLinesArray]->GetVertexAttribs();

	VA->FVertices.GetPtr()[ Idx0 ] = Point1;
	VA->FVertices.GetPtr()[ Idx1 ] = Point2;

	VA->FTexCoords.GetPtr()[ Idx0 ] = Color;
	VA->FTexCoords.GetPtr()[ Idx1 ] = Color;

	FLineCount++;
}

void clCanvas::Line3DGlow( const LVector3& Point1,
                           const LVector3& Point2,
                           const LVector4& Color,
                           float Thickness )
{
	if ( !FLinesArray[0] )
	{
		InitCanvas();
	}

	LMatrix4 LineParams( LVector4( Point1, 1.0f ), LVector4( Point2, 1.0f ), Color, LVector4( Thickness ) );

	FGlowLinesShader->GetShaderProgram()->BindUniforms();
	FGlowLinesShader->GetShaderProgram()->SetUniformNameMat4Array( "LineParams", 1, LineParams );

	Env->Renderer->AddBuffer( FRect, FGlowLinesShader, 1, false );
}

void clCanvas::Flush()
{
	if ( FLineCount <= 0 )
	{
		return;
	}

	if ( !FLinesArray[0] )
	{
		return;
	}

	FLinesArray[FCurrentLinesArray]->GetVertexAttribs()->SetActiveVertexCount( FLineCount * 2 );
	FLinesArray[FCurrentLinesArray]->CommitChanges();

	Env->Renderer->AddBuffer( FLinesArray[FCurrentLinesArray], FLinesShader, 1, false );

	FCurrentLinesArray = ( FCurrentLinesArray + 1 ) % 2;

	FLineCount = 0;

	TextStrFreeType_Flush();

	if ( FUseOverlayRendering && FOverlayScene )
	{
		// TODO : blend FOverlayScene->GetRenderTarget() with current screen
		FOverlayScene->RenderForward();
		// AlphaTexture(0, 0, 1, 1, FOverlayScene->GetRenderTarget()->GetColorTexture(0));
	}
}

void clCanvas::FullscreenRect( clRenderState* Shader )
{
	if ( !FScreenQuad )
	{
		FScreenQuad = Env->Renderer->AllocateEmptyVA();
		FScreenQuad->SetVertexAttribs( clGeomServ::CreateRect2D( 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, true, 0 ) );
	}

	Env->Renderer->AddBuffer( FScreenQuad, Shader, 1, false );
}

void clCanvas::FullscreenRectSP( iShaderProgram* ShaderProgram )
{
	iShaderProgram* OldSP = FTexRectShader->GetShaderProgram();

	if ( ShaderProgram )
	{
		FTexRectShader->SetShaderProgram( ShaderProgram );
	}

	FullscreenRect( FTexRectShader );

	if ( OldSP )
	{
		FTexRectShader->SetShaderProgram( OldSP );
	}
}

void clCanvas::Circle3D( const LVector3& Center,
                         float Radius,
                         const LVector4& Color,
                         bool AspectCorrection )
{
	const int Subdivisions = 32;

	const LVector3 X( Radius, 0.0f, 0.0f );
	const LVector3 Y( 0.0f, Radius, 0.0f );

	const float Delta = 2.0f * Math::PI / Subdivisions;

	const float Aspect = AspectCorrection ? Env->Viewport->GetAspectRatio() : 1.0f;

	for ( int i = 0; i != Subdivisions; ++i )
	{
		float Angle = Delta * i;

		LVector3 Point1 = Center + X * cosf( Angle         ) + Y * sinf( Angle         ) * Aspect;
		LVector3 Point2 = Center + X * cosf( Angle + Delta ) + Y * sinf( Angle + Delta ) * Aspect;

		Line3D( Point1, Point2, Color );
	}
}

void clCanvas::Arrow3D( const LVector3& Point1,
                        const LVector3& Point2,
                        const float     TipSize,
                        const LVector4& TipColor,
                        const LVector4& ArrowColor )
{
	Line3D( Point1, Point2, ArrowColor );

	// build coordsys for tip
	LVector3 Arrow = Point2 - Point1;
	LVector3 Up    = Arrow.OrthogonalVector();
	LVector3 Left  = Up.Cross( Arrow );

	Arrow.Normalize();
	Up.Normalize();
	Left.Normalize();

	Arrow *= TipSize;
	Up    *= 0.5f * TipSize;
	Left  *= 0.5f * TipSize;

	LVector3 Pt1 = Point1 + Arrow + Left;
	LVector3 Pt2 = Point1 + Arrow + Up;
	LVector3 Pt3 = Point1 + Arrow - Left;
	LVector3 Pt4 = Point1 + Arrow - Up;

	Line3D( Pt1, Point1, TipColor );
	Line3D( Pt2, Point1, TipColor );
	Line3D( Pt3, Point1, TipColor );
	Line3D( Pt4, Point1, TipColor );

	Line3D( Pt1, Pt2, TipColor );
	Line3D( Pt2, Pt3, TipColor );
	Line3D( Pt3, Pt4, TipColor );
	Line3D( Pt4, Pt1, TipColor );
}

void clCanvas::BoundingBox( const LAABoundingBox& Box,
                            const LVector4& Color )
{
	LVector3 Top1 = LVector3( Box.FMin.X, Box.FMin.Y, Box.FMax.Z );
	LVector3 Top2 = LVector3( Box.FMin.X, Box.FMax.Y, Box.FMax.Z );
	LVector3 Top3 = LVector3( Box.FMax.X, Box.FMax.Y, Box.FMax.Z );
	LVector3 Top4 = LVector3( Box.FMax.X, Box.FMin.Y, Box.FMax.Z );

	LVector3 Bottom1 = LVector3( Box.FMin.X, Box.FMin.Y, Box.FMin.Z );
	LVector3 Bottom2 = LVector3( Box.FMin.X, Box.FMax.Y, Box.FMin.Z );
	LVector3 Bottom3 = LVector3( Box.FMax.X, Box.FMax.Y, Box.FMin.Z );
	LVector3 Bottom4 = LVector3( Box.FMax.X, Box.FMin.Y, Box.FMin.Z );

	// top
	Line3D( Top1, Top2, Color );
	Line3D( Top2, Top3, Color );
	Line3D( Top3, Top4, Color );
	Line3D( Top4, Top1, Color );
	// bottom
	Line3D( Bottom1, Bottom2, Color );
	Line3D( Bottom2, Bottom3, Color );
	Line3D( Bottom3, Bottom4, Color );
	Line3D( Bottom4, Bottom1, Color );
	// vertical lines
	Line3D( Top1, Bottom1, Color );
	Line3D( Top2, Bottom2, Color );
	Line3D( Top3, Bottom3, Color );
	Line3D( Top4, Bottom4, Color );
}

void clCanvas::FullSphere( const LSphere&  Sphere, const LVector4& Color )
{
	LVector3 Vtx[4], VV;

	const float dTheta = 30.0f;
	const float dPhi   = 15.0f;

	float PhiOfs[] = {0.0f, dPhi, 0.0f, dPhi};
	float ThetaOfs[] = {0.0f, 0.0f, dTheta, dTheta};

	LVector3 TempVec;

	float Phi = 0;

	while ( Phi <= 180.0f - dPhi )
	{
		float Theta = 0;

		while ( Theta <= 360 - dTheta )
		{
			Theta += dTheta;

			for ( int j = 0 ; j < 4 ; j++ )
			{
				VV = LVector3( sin( ( Phi + PhiOfs[j] ) * Math::DTOR ) * cos( Math::DTOR * ( Theta + ThetaOfs[j] ) ),
				               sin( ( Phi + PhiOfs[j] ) * Math::DTOR ) * sin( Math::DTOR * ( Theta + ThetaOfs[j] ) ),
				               cosf( ( Phi + PhiOfs[j] ) * Math::DTOR ) );
				Vtx[j] = VV * Sphere.GetRadius() + Sphere.GetOrigin();
			}

			Line3D( Vtx[0], Vtx[1], Color );
			Line3D( Vtx[1], Vtx[2], Color );
			/*
			         if ( ( Phi > -90 ) && ( Phi < 90 ) )
			         {
			            Line3D( Vtx[2], Vtx[3], Color );
			         }
			*/
		}

		Phi += dPhi;
	}
}

void clCanvas::Sphere( const LSphere&  Sphere, const LVector4& Color )
{
	LVector3 Vtx[4], VV;

	const float dTheta = 30.0f;
	const float dPhi   = 15.0f;

	float PhiOfs[] = {0.0f, dPhi, 0.0f, dPhi};
	float ThetaOfs[] = {0.0f, 0.0f, dTheta, dTheta};

	LVector3 TempVec;

	float Phi = 0;

	while ( Phi <= 90.0f - dPhi )
	{
		float Theta = 0;

		while ( Theta <= 360 - dTheta )
		{
			Theta += dTheta;

			for ( int j = 0 ; j < 4 ; j++ )
			{
				VV = LVector3( sin( ( Phi + PhiOfs[j] ) * Math::DTOR ) * cos( Math::DTOR * ( Theta + ThetaOfs[j] ) ),
				               sin( ( Phi + PhiOfs[j] ) * Math::DTOR ) * sin( Math::DTOR * ( Theta + ThetaOfs[j] ) ),
				               cosf( ( Phi + PhiOfs[j] ) * Math::DTOR ) );
				Vtx[j] = VV * Sphere.GetRadius() + Sphere.GetOrigin();
			}

			Line3D( Vtx[0], Vtx[1], Color );
			Line3D( Vtx[1], Vtx[2], Color );

			if ( ( Phi > -90 ) && ( Phi < 90 ) )
			{
				Line3D( Vtx[2], Vtx[3], Color );
			}
		}

		Phi += dPhi;
	}
}

void clCanvas::OrientedBox( const LBox& B, const LVector4& Color )
{
	LVector3 Top1 = B.FCenter - B.FAxis[0] * B.FExtent[0] - B.FAxis[1] * B.FExtent[1] + B.FAxis[2] * B.FExtent[2];
	LVector3 Top2 = B.FCenter - B.FAxis[0] * B.FExtent[0] + B.FAxis[1] * B.FExtent[1] + B.FAxis[2] * B.FExtent[2];
	LVector3 Top3 = B.FCenter + B.FAxis[0] * B.FExtent[0] + B.FAxis[1] * B.FExtent[1] + B.FAxis[2] * B.FExtent[2];
	LVector3 Top4 = B.FCenter + B.FAxis[0] * B.FExtent[0] - B.FAxis[1] * B.FExtent[1] + B.FAxis[2] * B.FExtent[2];

	LVector3 Bottom1 = B.FCenter - B.FAxis[0] * B.FExtent[0] - B.FAxis[1] * B.FExtent[1] - B.FAxis[2] * B.FExtent[2];
	LVector3 Bottom2 = B.FCenter - B.FAxis[0] * B.FExtent[0] + B.FAxis[1] * B.FExtent[1] - B.FAxis[2] * B.FExtent[2];
	LVector3 Bottom3 = B.FCenter + B.FAxis[0] * B.FExtent[0] + B.FAxis[1] * B.FExtent[1] - B.FAxis[2] * B.FExtent[2];
	LVector3 Bottom4 = B.FCenter + B.FAxis[0] * B.FExtent[0] - B.FAxis[1] * B.FExtent[1] - B.FAxis[2] * B.FExtent[2];

	// top
	Line3D( Top1, Top2, Color );
	Line3D( Top2, Top3, Color );
	Line3D( Top3, Top4, Color );
	Line3D( Top4, Top1, Color );
	// bottom
	Line3D( Bottom1, Bottom2, Color );
	Line3D( Bottom2, Bottom3, Color );
	Line3D( Bottom3, Bottom4, Color );
	Line3D( Bottom4, Bottom1, Color );
	// vertical lines
	Line3D( Top1, Bottom1, Color );
	Line3D( Top2, Bottom2, Color );
	Line3D( Top3, Bottom3, Color );
	Line3D( Top4, Bottom4, Color );
}

void clCanvas::Plane( const LVector3& p0, const LVector3& v1, const LVector3& v2, int numx, int numy, float step1, float step2, const LVector4& color )
{
	LVector3 _p1, _p2;

	for ( int i = 0 ; i < numx + 1; i++ )
	{
		_p1 = p0 + static_cast<float>( i - numx / 2 ) * step1 * v1 + ( step2 * static_cast<float>( numy / 2 ) ) * v2;
		_p2 = p0 + static_cast<float>( i - numx / 2 ) * step1 * v1 - ( step2 * static_cast<float>( numy / 2 ) ) * v2;

		Line3D( _p1, _p2, color );
	}

	for ( int i = 0 ; i < numy + 1; i++ )
	{
		_p1 = p0 + static_cast<float>( i - numy / 2 ) * step2 * v2 + ( step1 * static_cast<float>( numx / 2 ) ) * v1;
		_p2 = p0 + static_cast<float>( i - numy / 2 ) * step2 * v2 - ( step1 * static_cast<float>( numx / 2 ) ) * v1;

		Line3D( _p1, _p2, color );
	}
}

void clCanvas::Rect( float X1, float Y1, float X2, float Y2, const LVector4& Color )
{
	if ( !FLinesShader )
	{
		InitCanvas();
	}

	LVector4 Pos( X1, Y1, X2, Y2 );

	FRectShader->GetShaderProgram()->BindUniforms();
	FRectShader->GetShaderProgram()->SetUniformVec4Array( FRectColorUniform, 1, Color );
	FRectShader->GetShaderProgram()->SetUniformVec4Array( FRectPosUniform,   1, Pos   );

	Env->Renderer->AddBuffer( Env->GUI->GetDefaultRect(), FRectShader, 1, false );
}

void clCanvas::TexturedRect( float X1, float Y1, float X2, float Y2, iTexture* Texture, iShaderProgram* ShaderProgram, const LVector4& Color )
{
	TexturedRectTiled( LVector4( X1, Y1, X2, Y2 ), 1.0f, 1.0f, Texture, ShaderProgram, Color, NULL, NULL );
}

void clCanvas::TexturedRectTiled( const LVector4& Pos, float TilesX, float TilesY, iTexture* Texture, iShaderProgram* ShaderProgram, const LVector4& Color, iVertexArray* VA, const LRect* UV )
{
	if ( !FLinesShader )
	{
		InitCanvas();
	}

	LVector4 Tiles( TilesX, TilesY, 0.0f, 0.0f );
	LVector4 UVRect( UV ? UV->ToVector4() : LVector4( 0.0f, 0.0f, 1.0f, 1.0f ) );

	FTexRectShader->SetTexture( 0, Texture, false );

	iShaderProgram* OldSP = FTexRectShader->GetShaderProgram();

	if ( ShaderProgram )
	{
		FTexRectShader->SetShaderProgram( ShaderProgram );
		FTexRectShader->GetShaderProgram()->BindUniforms();
		FTexRectShader->GetShaderProgram()->SetUniformNameVec4Array( "RectPos",   1, Pos    );
		FTexRectShader->GetShaderProgram()->SetUniformNameVec4Array( "RectColor", 1, Color  );
		FTexRectShader->GetShaderProgram()->SetUniformNameVec4Array( "RectTiles", 1, Tiles  );
		FTexRectShader->GetShaderProgram()->SetUniformNameVec4Array( "RectUV",    1, UVRect );
	}
	else
	{
		FTexRectShader->GetShaderProgram()->BindUniforms();
		FTexRectShader->GetShaderProgram()->SetUniformVec4Array( FTexRectPosUniform,   1, Pos    );
		FTexRectShader->GetShaderProgram()->SetUniformVec4Array( FTexRectColorUniform, 1, Color  );
		FTexRectShader->GetShaderProgram()->SetUniformVec4Array( FTexRectTilesUniform, 1, Tiles  );
		FTexRectShader->GetShaderProgram()->SetUniformVec4Array( FTexRectUVUniform,    1, UVRect );
	}

	Env->Renderer->AddBuffer( VA ? VA : Env->GUI->GetDefaultRect(), FTexRectShader, 1, false );

	FTexRectShader->SetShaderProgram( OldSP );
}

void clCanvas::Rect3DShader( const LVector3& v1, const LVector3& v2, const LVector3& v3, const LVector3& v4, clRenderState* State )
{
	if ( !FLinesShader ) { InitCanvas(); }

	clVertexAttribs* VA = FRect->GetVertexAttribs();

	VA->FVertices.GetPtr()[0] = v1;
	VA->FVertices.GetPtr()[1] = v2;
	VA->FVertices.GetPtr()[2] = v4;
	VA->FVertices.GetPtr()[3] = v3;

	FRect->CommitChanges();

	Env->Renderer->AddBuffer( FRect, State, 1, false );
}

void clCanvas::TexturedRect3D( const LVector3& v1, const LVector3& v2, const LVector3& v3, const LVector3& v4,
                               iTexture* Texture, iShaderProgram* ShaderProgram, bool UseDepthTest )
{
	if ( !FLinesShader )
	{
		InitCanvas();
	}

	clVertexAttribs* VA = FRect->GetVertexAttribs();

	VA->FVertices.GetPtr()[0] = v1;
	VA->FVertices.GetPtr()[1] = v2;
	VA->FVertices.GetPtr()[2] = v4;
	VA->FVertices.GetPtr()[3] = v3;

	FRect->CommitChanges();

	FTexRectShader3D->SetTexture( 0, Texture, false );

	TODO( "bind texture" )

	bool oldDepthTest = FTexRectShader3D->FDepthTest;
	bool oldDepthWrites = FTexRectShader3D->FDepthWrites;

	FTexRectShader3D->FDepthWrites = UseDepthTest;
	FTexRectShader3D->FDepthTest = UseDepthTest;

	iShaderProgram* OldSP = FTexRectShader3D->GetShaderProgram();

	if ( ShaderProgram )
	{
		FTexRectShader3D->SetShaderProgram( ShaderProgram );
	}

	Env->Renderer->AddBuffer( FRect, FTexRectShader3D, 1, false );

	FTexRectShader3D->FDepthTest = oldDepthTest;
	FTexRectShader3D->FDepthWrites = oldDepthWrites;

	FTexRectShader3D->SetShaderProgram( OldSP );
}

clFontFace* clCanvas::CreateFontFace( const LString& FileName )
{
	return FTextRenderer->GetFontHandle( FileName );
}

clRenderableString::~clRenderableString()
{
	delete( FTexture );
	delete( FBitmap );
}

iTexture* clRenderableString::GetTexture() const
{
	if ( !FTexture && FBitmap )
	{
		FTexture = Env->Renderer->CreateTexture();
		FTexture->LoadFromBitmap( FBitmap );
		FTexture->SetClamping( L_CLAMP_TO_EDGE );
	}

	return FTexture;
}

clRenderableString* clCanvas::TextStrGetBitmapFreeType( const LString& Str, const float Size, clFontFace* FontFace )
{
	guard();

	clFontFace* UsedFont = FontFace;

	if ( !UsedFont )
	{
		if ( !FDefaultFreeTypeFont )
		{
			FDefaultFreeTypeFont = FTextRenderer->GetFontHandle( FDefaultFontName );
		}

		UsedFont = FDefaultFreeTypeFont;
	}

	int MinY, MaxY, StrWidth, BaseLine;
	const int PixelFontHeight = static_cast<int>( Size * Env->Viewport->GetHeight() );

	FTextRenderer->LoadTextStringWithFont( Str, UsedFont, PixelFontHeight, Encoding_UTF8 );
	FTextRenderer->CalculateLineParameters( &StrWidth, &MinY, &MaxY, &BaseLine );

	int StrHeight = MinY + MaxY;

	if ( StrWidth  < 0 ) { StrWidth = 0; }

	if ( StrHeight < 0 ) { StrHeight = 0; }

#if defined( OS_ANDROID )
	// scale to power-of-2 image
	int BitmapStrWidth  = Math::GetNextPowerOf2( StrWidth  );
	int BitmapStrHeight = Math::GetNextPowerOf2( StrHeight );

	float TruncX = ( float )StrWidth  / BitmapStrWidth;
	float TruncY = ( float )StrHeight / BitmapStrHeight;

	LVector4 TextTexCoords( TruncX, TruncY, 0.0f, 0.0f );

	clBitmap* BMP = clBitmap::CreateBitmap( Env, BitmapStrWidth, BitmapStrHeight, 1, L_BITMAP_GRAYSCALE8, L_TEXTURE_2D );
//	BMP->Clear( LC_Black );
#else
	LVector4 TextTexCoords( 1.0f, 1.0f, 0.0f, 0.0f );

	clBitmap* BMP = clBitmap::CreateBitmap( Env, StrWidth, StrHeight, 1, L_BITMAP_GRAYSCALE8, L_TEXTURE_2D );
//	BMP->Clear( LC_Black );

	Env->Logger->LogP( L_DEBUG, "Bitmap size for text: %i x %i", StrWidth, StrHeight );
#endif

	BMP->Clear( LVector4( 0 ) );

	FTextRenderer->RenderLineOnBitmap( Str, UsedFont, PixelFontHeight, 0, MinY, LVector4( 1, 0, 0, 1 ), Encoding_UTF8, Env->Localizer->IsLeftToRight(), BMP );

	float RelStrWidth  = ( float )StrWidth  / ( float )Env->Viewport->GetWidth();
	float RelStrHeight = ( float )StrHeight / ( float )Env->Viewport->GetHeight();

	clRenderableString* Result = new clRenderableString( BMP, RelStrWidth, RelStrHeight, TextTexCoords );
	Result->Env = Env;
	Result->AfterConstruction();

	return Result;

	unguard();
}

void clCanvas::TextStrBitmapFreeType( const LRect& Rect, clRenderableString* Str, const LVector4& Color, LTextAlignment Alignment )
{
	guard();

	if ( !FFontTexture ) { FFontTexture = Env->Renderer->CreateTexture(); }

	FFontTexture->LoadFromBitmap( Str->GetBitmap() );
	FFontTexture->SetClamping( L_CLAMP_TO_EDGE );
//	FFontTexture->UpdateFromBitmap(BMP); // for no-texture ?

	Env->Renderer->Finish();

	LVector4 TextPosSize( Rect.X1(), Rect.Y1(), Str->GetWidth(), Str->GetHeight() );

	switch ( Alignment )
	{
		case TextAlign_Left:
			// already aligned - do nothing
			break;
		case TextAlign_Right:
			TextPosSize.X = Rect.GetWidth() - Str->GetWidth();
			break;
		case TextAlign_Center:
			TextPosSize.X = Rect.GetCenterX() - 0.5f * Str->GetWidth();
			TextPosSize.Y = Rect.GetCenterY() - 0.5f * Str->GetHeight();
			break;
		case TextAlign_CenterWidth:
			TextPosSize.X = Rect.GetCenterX() - 0.5f * Str->GetWidth();
			break;
		case TextAlign_CenterHeight:
			TextPosSize.Y = Rect.GetCenterY() - 0.5f * Str->GetHeight();
			break;
		case TextAlign_FitWidth:
		{
			float Coef = Rect.GetWidth() / Str->GetWidth();
			TextPosSize.X = Rect.GetCenterX() - 0.5f * Str->GetWidth() * Coef;
			TextPosSize.Y = Rect.GetCenterY() - 0.5f * Str->GetHeight() * Coef;
			TextPosSize.Z *= Coef;
			TextPosSize.W *= Coef;
		}
		break;
		case TextAlign_FitHeight:
		{
			float Coef = Rect.GetHeight() / Str->GetHeight();
			TextPosSize.X = Rect.GetCenterX() - 0.5f * Str->GetWidth() * Coef;
			TextPosSize.Y = Rect.GetCenterY() - 0.5f * Str->GetHeight() * Coef;
			TextPosSize.Z *= Coef;
			TextPosSize.W *= Coef;
		}
		break;
		case TextAlign_Fit:
		{
			float Coef1 = Rect.GetWidth() / Str->GetWidth();
			float Coef2 = Rect.GetHeight() / Str->GetHeight();
			float Coef = Math::LMin( Coef1, Coef2 );
			TextPosSize.X = Rect.GetCenterX() - 0.5f * Str->GetWidth() * Coef;
			TextPosSize.Y = Rect.GetCenterY() - 0.5f * Str->GetHeight() * Coef;
			TextPosSize.Z *= Coef;
			TextPosSize.W *= Coef;
		}
		break;
	};

	FFontShader->GetShaderProgram()->BindUniforms();

	FFontShader->GetShaderProgram()->SetUniformNameVec4Array( "TextPosSize",   1, TextPosSize );

	FFontShader->GetShaderProgram()->SetUniformNameVec4Array( "TextColor",     1, Color );

	FFontShader->GetShaderProgram()->SetUniformNameVec4Array( "TextTexCoords", 1, Str->GetTextTexCoords() );

	FFontShader->SetTexture( 0, FFontTexture, false );

	Env->Renderer->AddBuffer( Env->GUI->GetDefaultRect(), FFontShader, 1, false );

	unguard();
}

void clCanvas::TextStrTextureFreeType( const LRect& Rect, clRenderableString* Str, const LVector4& Color, LTextAlignment Alignment )
{
	LVector4 TextPosSize( Rect.X1(), Rect.Y1(), Str->GetWidth(), Str->GetHeight() );

	switch ( Alignment )
	{
		case TextAlign_Left:
			// already aligned - do nothing
			break;
		case TextAlign_Right:
			TextPosSize.X = Rect.GetWidth() - Str->GetWidth();
			break;
		case TextAlign_Center:
			TextPosSize.X = Rect.GetCenterX() - 0.5f * Str->GetWidth();
			TextPosSize.Y = Rect.GetCenterY() - 0.5f * Str->GetHeight();
			break;
		case TextAlign_CenterWidth:
			TextPosSize.X = Rect.GetCenterX() - 0.5f * Str->GetWidth();
			break;
		case TextAlign_CenterHeight:
			TextPosSize.Y = Rect.GetCenterY() - 0.5f * Str->GetHeight();
			break;
		case TextAlign_FitWidth:
		{
			float Coef = Rect.GetWidth() / Str->GetWidth();
			TextPosSize.X = Rect.GetCenterX() - 0.5f * Str->GetWidth() * Coef;
			TextPosSize.Y = Rect.GetCenterY() - 0.5f * Str->GetHeight() * Coef;
			TextPosSize.Z *= Coef;
			TextPosSize.W *= Coef;
		}
		break;
		case TextAlign_FitHeight:
		{
			float Coef = Rect.GetHeight() / Str->GetHeight();
			TextPosSize.X = Rect.GetCenterX() - 0.5f * Str->GetWidth() * Coef;

			TextPosSize.Y = Rect.GetCenterY() - 0.5f * Str->GetHeight() * Coef;
			TextPosSize.Z *= Coef;
			TextPosSize.W *= Coef;
		}
		break;
		case TextAlign_Fit:
		{
			float Coef1 = Rect.GetWidth() / Str->GetWidth();
			float Coef2 = Rect.GetHeight() / Str->GetHeight();
			float Coef = Math::LMin( Coef1, Coef2 );
			TextPosSize.X = Rect.GetCenterX() - 0.5f * Str->GetWidth() * Coef;
			TextPosSize.Y = Rect.GetCenterY() - 0.5f * Str->GetHeight() * Coef;
			TextPosSize.Z *= Coef;
			TextPosSize.W *= Coef;
		}
		break;
	};

	FFontShader->GetShaderProgram()->BindUniforms();

	FFontShader->GetShaderProgram()->SetUniformNameVec4Array( "TextPosSize",   1, TextPosSize );

	FFontShader->GetShaderProgram()->SetUniformNameVec4Array( "TextColor",     1, Color );

	FFontShader->GetShaderProgram()->SetUniformNameVec4Array( "TextTexCoords", 1, Str->GetTextTexCoords() );

	FFontShader->SetTexture( 0, Str->GetTexture(), false );

	Env->Renderer->AddBuffer( Env->GUI->GetDefaultRect(), FFontShader, 1, false );
}
/*
void clCanvas::TextStrFreeType( const LRect& Rect, const LString& Str, const float Size, const LVector4& Color, clFontFace* FontFace, LTextAlignment Alignment )
{
	guard( "%s", Str.c_str() );

	clRenderableString* BMP = TextStrGetBitmapFreeType( Str, Size, FontFace );

	TextStrBitmapFreeType( Rect, BMP, Color, Alignment );

	delete( BMP );

	unguard();
}
*/
void clCanvas::TextStrFreeType( const LRect& Rect, const LString& Str, const float Size, const LVector4& Color, clFontFace* FontFace, LTextAlignment Alignment )
{
//	TextStrFreeType( Rect, Str, Size, Color, FontFace, Alignment );

//	this->Rect( Rect.X1(), Rect.Y1(), Rect.X2(), Rect.Y2(), LVector4( 1.0f, 1.0f, 1.0f, 0.3f ) );
	
	clFontFace* UsedFont = FontFace;

	if ( !UsedFont )
	{
		if ( !FDefaultFreeTypeFont )
		{
			FDefaultFreeTypeFont = FTextRenderer->GetFontHandle( FDefaultFontName );
		}

		UsedFont = FDefaultFreeTypeFont;
	}

	int MinY;
	int PixelFontHeight = static_cast<int>( Size * Env->Viewport->GetHeight() );
	float TextWidth, TextHeight;
	
	FTextRenderer->LoadTextStringWithFont( Str, UsedFont, PixelFontHeight, Encoding_UTF8 );
	FTextRenderer->CalculateLineParametersFloat( NULL, &MinY, NULL, NULL, &TextWidth, &TextHeight );

	LVector2 TextPosSize( Rect.X1(), Rect.Y1() );

	switch ( Alignment )
	{
	case TextAlign_Left:
		// already aligned - do nothing
		break;
	case TextAlign_Right:
		TextPosSize.X = Rect.GetWidth() - TextWidth;
		break;
	case TextAlign_CenterWidth:
		TextPosSize.X = Rect.GetCenterX() - 0.5f * TextWidth;
		break;
	case TextAlign_CenterHeight:
		TextPosSize.Y = Rect.GetCenterY() - 0.5f * TextHeight;
		break;
	case TextAlign_Center:
		TextPosSize.X = Rect.GetCenterX() - 0.5f * TextWidth;
		TextPosSize.Y = Rect.GetCenterY() - 0.5f * TextHeight;
		break;
	case TextAlign_FitWidth:
		{
			float Coef = Rect.GetWidth() / TextWidth;
			PixelFontHeight = static_cast<int>( Coef * PixelFontHeight );
			FTextRenderer->LoadTextStringWithFont( Str, UsedFont, PixelFontHeight, Encoding_UTF8 );
			FTextRenderer->CalculateLineParametersFloat( NULL, &MinY, NULL, NULL, &TextWidth, &TextHeight );
			TextPosSize.X = Rect.GetCenterX() - 0.5f * TextWidth;
			TextPosSize.Y = Rect.GetCenterY() - 0.5f * TextHeight;
		}
		break;
	case TextAlign_FitHeight:
		{			
			float Coef = Rect.GetHeight() / TextHeight;
			PixelFontHeight = static_cast<int>( Coef * PixelFontHeight );
			FTextRenderer->LoadTextStringWithFont( Str, UsedFont, PixelFontHeight, Encoding_UTF8 );
			FTextRenderer->CalculateLineParametersFloat( NULL, &MinY, NULL, NULL, &TextWidth, &TextHeight );
			TextPosSize.X = Rect.GetCenterX() - 0.5f * TextWidth;
			TextPosSize.Y = Rect.GetCenterY() - 0.5f * TextHeight;
		}
		break;
	case TextAlign_Fit:
		{
			float Coef1 = Rect.GetWidth() / TextWidth;
			float Coef2 = Rect.GetHeight() / TextHeight;
			float Coef = Math::LMin( Coef1, Coef2 );
			PixelFontHeight = static_cast<int>( Coef * PixelFontHeight );
			FTextRenderer->LoadTextStringWithFont( Str, UsedFont, PixelFontHeight, Encoding_UTF8 );
			FTextRenderer->CalculateLineParametersFloat( NULL, &MinY, NULL, NULL, &TextWidth, &TextHeight );
			TextPosSize.X = Rect.GetCenterX() - 0.5f * TextWidth;
			TextPosSize.Y = Rect.GetCenterY() - 0.5f * TextHeight;
		}
		break;
	};

	FTextRenderer->RenderLineInVertexAttribs( TextPosSize, Str, UsedFont, PixelFontHeight, 0, MinY, Color, Encoding_UTF8, Env->Localizer->IsLeftToRight(), FTextFreeTypeGeometry );

//	FTextAtlasShader->GetShaderProgram()->BindUniforms();
//	FTextAtlasShader->GetShaderProgram()->SetUniformNameVec4Array( "TextPosSize",   1, TextPosSize );
//	FTextAtlasShader->GetShaderProgram()->SetUniformNameVec4Array( "TextColor",     1, Color );
//	FTextAtlasShader->GetShaderProgram()->SetUniformNameVec4Array( "TextTexCoords", 1, Str->GetTextTexCoords() );

	// wait for TextStrFreeType_Flush()
}

void clCanvas::TextStrFreeType_Flush()
{
	for ( size_t i = 0; i != FTextFreeTypeGeometry.size(); i++ )
	{
		sVAContainer& C = FTextFreeTypeGeometry[i];

		if ( !C.empty() && C.FVertexAttribs->GetActiveVertexCount() )
		{
			C.FVertexArray->CommitChanges();

			FTextAtlasShader->SetTexture( 0, Env->Renderer->GetGlyphsCache()->GetPageTexture( i ), false );

			Env->Renderer->AddBuffer( C.FVertexArray, FTextAtlasShader, 1, false );
		}
	}

	for ( size_t i = 0; i != FTextFreeTypeGeometry.size(); i++ ) FTextFreeTypeGeometry[i].Reset();
}

LRect clCanvas::GetTextStrRectFreeType( const LString& Str, const float Size, clFontFace* FontFace )
{
	guard( "%s", Str.c_str() );

	clFontFace* UsedFont = FontFace;

	if ( !UsedFont )
	{
		if ( !FDefaultFreeTypeFont ) { FDefaultFreeTypeFont = FTextRenderer->GetFontHandle( FDefaultFontName ); }

		UsedFont = FDefaultFreeTypeFont;
	}

	int MinY, MaxY, StrWidth, BaseLine;
	const int PixelFontHeight = static_cast<int>( Size * Env->Viewport->GetHeight() );

	FTextRenderer->LoadTextStringWithFont( Str, UsedFont, PixelFontHeight, Encoding_UTF8 );
	FTextRenderer->CalculateLineParameters( &StrWidth, &MinY, &MaxY, &BaseLine );

	const int StrHeight = MinY + MaxY;

	float RelStrWidth = ( float )StrWidth / ( float )Env->Viewport->GetWidth();
	float RelStrHeight = ( float )StrHeight / ( float )Env->Viewport->GetHeight();

	return LRect( 0.0f, 0.0f, RelStrWidth, RelStrHeight );

	unguard();
}

void clCanvas::TextStr( float X, float Y, const LString& Str, const LVector4& Color, clFixedFontProperties* Font )
{
	guard( "%s", Str.c_str() );

	clFixedFontProperties* UsedFont = Font ? Font : FDefaultFont;

	if ( FTextShader != UsedFont->GetFontShader() )
	{
		if ( FTextPacketStarted )
		{
			EndTextPacket();
			BeginTextPacket();
		}

		FTextShader = UsedFont->GetFontShader();
	}

	//
	// glyph metrics
	//

	// size of glyph inside the cell (1.0 means use content of the full cell of our text texture)
	const float SX = UsedFont->FGlyphSizeX;
	const float SY = UsedFont->FGlyphSizeY;

	// 4:3
	const float FontWidth  = UsedFont->FFontWidth * SX;
	const float FontHeight = UsedFont->FFontHeight * SY;

	// NOTE: uncomment these two lines for proportional font
	// const float Aspect  = Env->Viewport->GetAspectRatio();
	// const float FontHeight = ( 3.0f / 128.0f  ) * Aspect * SY;

	// magic const to mimic old raster Font functionality
	const float YOffset = 0.8f * FontHeight;

	//
	// texture metrics
	//
	const int   NumCellsX = 16;
	const float Stride    = 1.0f / NumCellsX;
	const float USkip     = 0.0f;
	const float VSkip     = 0.122f * Stride;

	if ( FLinesShader == NULL )
	{
		InitCanvas();
	}

	FTextShader->GetShaderProgram()->BindUniforms();
	FTextShader->GetShaderProgram()->SetUniformNameVec4Array( "TextUVOfs", 1, LVector4( USkip, VSkip, 0.95f * FontWidth, FontHeight ) );

	clVertexAttribs* Geometry = FTextVA->GetVertexAttribs();

	LVector3* Vertices  = Geometry->FVertices.GetPtr();
	LVector4* TexCoords = Geometry->FTexCoords.GetPtr();
	LVector4* Colors    = Geometry->FColors.GetPtr();

	// prepare texture coords for the string
	for ( size_t i = 0; i != Str.length(); i++ )
	{
		int Ch = Str[ i ];
		float CellX = static_cast<float>(  Ch % NumCellsX    );
		float CellY = static_cast<float>( ( Ch / NumCellsX ) - 1 );

		size_t k = FTextPacketNumGlyphs + i;

		Vertices[ 6 * k + 0 ] = LVector3( 0.0f + i, 0.0f, 0.0f );
		Vertices[ 6 * k + 1 ] = LVector3( 0.0f + i, 1.0f, 0.0f );
		Vertices[ 6 * k + 2 ] = LVector3( 1.0f + i, 0.0f, 0.0f );
		Vertices[ 6 * k + 3 ] = LVector3( 0.0f + i, 1.0f, 0.0f );
		Vertices[ 6 * k + 4 ] = LVector3( 1.0f + i, 0.0f, 0.0f );
		Vertices[ 6 * k + 5 ] = LVector3( 1.0f + i, 1.0f, 0.0f );
		TexCoords[ 6 * k + 0 ] = LVector4( Stride * ( CellX + 0 ), Stride * ( CellY + 0 ), X, Y + YOffset );
		TexCoords[ 6 * k + 1 ] = LVector4( Stride * ( CellX + 0 ), Stride * ( CellY + SY ), X, Y + YOffset );
		TexCoords[ 6 * k + 2 ] = LVector4( Stride * ( CellX + SX ), Stride * ( CellY + 0 ), X, Y + YOffset );
		TexCoords[ 6 * k + 3 ] = LVector4( Stride * ( CellX + 0 ), Stride * ( CellY + SY ), X, Y + YOffset );
		TexCoords[ 6 * k + 4 ] = LVector4( Stride * ( CellX + SX ), Stride * ( CellY + 0 ), X, Y + YOffset );
		TexCoords[ 6 * k + 5 ] = LVector4( Stride * ( CellX + SX ), Stride * ( CellY + SY ), X, Y + YOffset );

		for ( size_t j = 0; j != 6; j++ )
		{
			Colors[ 6 * k + j ] = Color;
		}
	}

	FTextPacketNumGlyphs += Str.length();

	if ( !FTextPacketStarted )
	{
		FlushTextPacket();
	}

	unguard();
}

void clCanvas::FlushTextPacket()
{
	// set number of vertices to render
	FTextVA->GetVertexAttribs()->SetActiveVertexCount( FTextPacketNumGlyphs * 6 );
	FTextVA->CommitChanges();

	Env->Renderer->AddBuffer( FTextVA, FTextShader, 1, false );

	FTextPacketStarted   = false;
	FTextPacketNumGlyphs = 0;
}

void clCanvas::BeginTextPacket()
{
	if ( !FTextPacketStarted )
	{
		FTextPacketStarted   = true;
		FTextPacketNumGlyphs = 0;
	}
}

void clCanvas::EndTextPacket()
{
	if ( FTextPacketStarted )
	{
		FlushTextPacket();
	}
}

void clCanvas::DrawOrientation( const LMatrix4& Projection, const LMatrix4& ModelView )
{
	SetMatrices( Projection, ModelView );

	// size of the frame
	float sz = 0.25f;
	float hsz = sz / 2.0f;

	// a small rectangle in the lower right corner of the screen
	LVector3 p0( 1.0f - sz, -1.0f + sz, 1.0f - sz );
	LVector3 p1( 1.0f,    -1.0f + sz, 1.0f - sz );
	LVector3 p2( 1.0f,    -1.0f,    1.0f - sz );
	LVector3 p3( 1.0f - sz, -1.0f,    1.0f - sz );

	// the center of a small rectangle
	LVector3 p_c( 1.0f - hsz, -1.0f + hsz,    1.0f - hsz );

	// extract axes from View matrix
	LVector3 Xaxis( ModelView[0].X, ModelView[0].Y, ModelView[0].Z );
	LVector3 Yaxis( ModelView[1].X, ModelView[1].Y, ModelView[1].Z );
	LVector3 Zaxis( ModelView[2].X, ModelView[2].Y, ModelView[2].Z );

	// scale down each axis
	Xaxis *= hsz;
	Yaxis *= hsz;
	Zaxis *= hsz;

	// find the points in WorldSpace which
	// will be projected to the lower left corner of the screeen
	LVector3 aX = Unproject( Projection, ModelView, p_c + Xaxis );
	LVector3 aY = Unproject( Projection, ModelView, p_c + Yaxis );
	LVector3 aZ = Unproject( Projection, ModelView, p_c + Zaxis );
	LVector3 aC = Unproject( Projection, ModelView, p_c );

// Env->Renderer->GetCanvas()->Flush();
// Env->Renderer->GetCanvas()->TurnOffLineClipping();

	// render rectangle around the coordinate frame widget
	Env->Renderer->GetCanvas()->Rect( 1.0f - hsz, 1.0f - hsz, 1.0f, 1.0f, LC_Black );

	// render the orientation coordinate frame

	Line3D( aC, aX, LC_Red   );
	Line3D( aC, aY, LC_Green );
	Line3D( aC, aZ, LC_Blue  );

	// render camera coordinates
	LVector3 CameraPos;
	LMatrix4 CameraRotation;
	DecomposeCameraTransformation( ModelView, CameraPos, CameraRotation );

	LString s = LStr::ToStr( CameraPos.X, 3 ) + " " + LStr::ToStr( CameraPos.Y, 3 ) + " " + LStr::ToStr( CameraPos.Z, 3 );

	TextStr( 1.0f - 3.0f * sz / 4.0f, 1.0f - 3.0f * sz / 4.0f, s, LC_White, NULL );

	Flush();
// TurnOnLineClipping();
}

void clCanvas::Geom( const LMatrix4& Projection, const LMatrix4& ModelView, clGeom* Geom, clRenderState* OverrideShader )
{
	if ( !FOverlayScene ) { FOverlayScene = Env->Linker->Instantiate( "clScene" ); }

	int Idx = FOverlayScene->AddGeom( Geom );

	if ( OverrideShader ) { FOverlayScene->SetMtlFromShader( Idx, OverrideShader, NULL, NULL ); }

	FOverlayScene->SetCameraProjection( Projection );
	FOverlayScene->SetCameraTransform( ModelView );
	FOverlayScene->RenderForward();

	FOverlayScene->RemoveItem( Idx );
}

void clCanvas::AlphaTexture( float X1, float Y1, float X2, float Y2, iTexture* Texture )
{
	// TODO: use FAlphaBlendShader to render the texture
}

void clCanvas::Curve( const LMatrix4& _proj, const LMatrix4& _view, const LCurve& Curve )
{
	SetMatrices( _proj, _view );

	for ( size_t i = 0 ; i < Curve.T.size() ; i++ )
	{
		Point( _proj, _view, Curve.P[i], 0.04f, LC_Red );

		if ( i + 1 < Curve.T.size() )
		{
			Line3D( Curve.P[i], Curve.P[i+1], LC_Yellow );
		}
	}
}

void clCanvas::CoordFrame( const LMatrix4& _proj, const LMatrix4& _view, const LVector3& Center, const LVector3& V1, const LVector3& V2, const LVector3& V3 )
{
	SetMatrices( _proj, _view );
	Arrow3D( Center - V1 * 0.5f, Center + V1 * 0.5f, V1.Length() * 0.2f, LC_Red,   LC_Red );
	Arrow3D( Center - V2 * 0.5f, Center + V2 * 0.5f, V2.Length() * 0.2f, LC_Green, LC_Green );
	Arrow3D( Center - V3 * 0.5f, Center + V3 * 0.5f, V3.Length() * 0.2f, LC_Blue,  LC_Blue );
}

void clCanvas::CollisionShape( const LMatrix4& _proj, const LMatrix4& _view, LCollisionShape* Shape, clRigidBody* Body )
{
	if ( !Shape ) { return; }

	SetMatrices( _proj, _view );

	if ( LCollisionShape_Box* BB = dynamic_cast<LCollisionShape_Box*>( Shape ) )
	{
		OrientedBox( BB->FBox, LC_Yellow );
	}
	else if ( LCollisionShape_Sphere* Sph = dynamic_cast<LCollisionShape_Sphere*>( Shape ) )
	{
		FullSphere( LSphere( Sph->FPosition, Sph->FRadius ), LC_Yellow );
	}
	else if ( LCollisionShape_Plane* PP = dynamic_cast<LCollisionShape_Plane*>( Shape ) )
	{
		LPlane Plane( PP->FEquation );
		LVector3 v1, v2;
		Plane.BuildBasis( &v1, &v2 );
		LVector3 p0 = Plane.ProjectPoint( Body ? Body->GetPosition() : vec3( 0, 0, 0 ) );

		this->Plane( p0, v1, v2, 10, 10, 0.2f, 0.2f, LC_Yellow );
	}
}

void clCanvas::RigidBody( const LMatrix4& Proj, const LMatrix4& View, clRigidBody* Body, bool RenderVelocities, bool RenderForces, bool RenderCollision )
{
	Body->GetPosition();
	LMatrix4 Transform = Body->GetTransformation();
	LMatrix3 Orient;
	LMatrix4 Rot;
	LVector3 Pos;
	DecomposeTransformation( Transform, Pos, Rot );
	Orient = Rot.ExtractMatrix3();

	CoordFrame( Proj, View, Pos, Orient[0], Orient[1], Orient[2] );

	SetMatrices( Proj, View );

	if ( RenderVelocities )
	{
		LVector3 v = Body->FVelocity;
		// linear is yellow
		Arrow3D( Pos + v, Pos, v.Length() * 0.2f, LC_Yellow, LC_Yellow );

		v = Body->FAngularVelocity;
		// angular is light blue
		Arrow3D( Pos + v, Pos, v.Length() * 0.2f, LC_LightBlue, LC_LightBlue );
	}

	if ( RenderForces )
	{
		// linear is yellow
		Line3D( Pos, Body->FForceAccumulator, LC_Yellow );

		// angular is light blue
		Line3D( Pos, Body->FTorqueAccumulator, LC_LightBlue );
	}

	if ( RenderCollision )
	{
		CollisionShape( Proj, View, Body->FCollisionShape, Body );
	}
}

/**
   If Depth is not -1 then only nodes at the Depth level are rendered
*/
void clCanvas::OBBTree( const LMatrix4& _proj, const LMatrix4& _view, LOBBTree* Tree, int Depth, bool ShowPlanes, const LVector4& Color, const LVector4& PlaneColor )
{
	SetMatrices( _proj, _view );

	for ( size_t i = 0 ; i < Tree->FNodes.size() ; i++ )
	{
		if ( Depth != -1 ) if ( Tree->FNodes[i].FDepth != Depth ) { continue; }

		OrientedBox( Tree->FNodes[i].FBox, Color );

		if ( ShowPlanes )
		{
			LVector3 v1, v2;
			Tree->FNodes[i].FSplitPlane.BuildBasis( &v1, &v2 );
			LVector3 p0 = Tree->FNodes[i].FSplitPlane.ProjectPoint( Tree->FNodes[i].FBox.FCenter );

			// render the plane
			Plane( p0, v1, v2, 10, 10, 0.2f, 0.2f, PlaneColor );
		}
	}
}

/// If the normal size is more than zero, it is drawn
void clCanvas::Poly( const LMatrix4& _proj, const LMatrix4& _view, const Math::LPoly& Poly, float NormalSize, const LVector4& PolyColor, const LVector4& NormalColor )
{
	LVector3 V = Poly.GetCenter();

	SetMatrices( _proj, _view );

	for ( int i = 0 ; i < Poly.FNumVertices ; i++ )
	{
		LVector3 Pt1 = Poly.FVertices[i];
		LVector3 Pt2 = Poly.FVertices[( i + 1 ) % Poly.FNumVertices];

		Line3D( Pt1, Pt2, PolyColor );

		if ( NormalSize > 0 )
		{
			Line3D( Poly.FVertices[i], V, NormalColor );
		}
	}

	if ( NormalSize > 0 )
	{
		LVector3 P = V + NormalSize * Poly.GetNormal();

		Arrow3D( P, V, NormalSize * 0.1f, NormalColor, NormalColor );
	}
}

void clCanvas::PolyList( const LMatrix4& _proj, const LMatrix4& _view, const Math::LPolyList& Polys, float NormalSize, const LVector4& PolyColor, const LVector4& NormalColor )
{
	for ( size_t i = 0 ; i < Polys.FPolys.size() ; i++ )
	{
		Poly( _proj, _view, Polys.FPolys[i], NormalSize, PolyColor, NormalColor );
	}
}


/**
   Draw the plane hull near the hint point Extents.FCenter not larger than the Extents. Use plane "cell" size of CellSize units
*/
void clCanvas::PlaneHullOutlines( const LMatrix4& _proj, const LMatrix4& _view, const Math::LPlaneHull& Hull, const LBox& Extents, float CellSize, const LVector4& Color )
{
	Math::LPolyList L;
	L.FromPlaneHull( Hull );

	PolyList( _proj, _view, L, 0.3f, Color, Color );
}

void clCanvas::Wireframe( const LMatrix4& _proj, const LMatrix4& _view, int NumTris, LVector3* Pts, int* Idx, const LVector4& Color )
{
	SetMatrices( _proj, _view );

	int i1, i2, i3;

	for ( int i = 0 ; i < NumTris ; i++ )
	{
		i1 = Idx[i * 3 + 0];
		i2 = Idx[i * 3 + 1];
		i3 = Idx[i * 3 + 2];

		LVector3 p1 = Pts[ i1 ];
		LVector3 p2 = Pts[ i2 ];
		LVector3 p3 = Pts[ i3 ];

		Line3D( p1, p2, Color );
		Line3D( p3, p2, Color );
		Line3D( p1, p3, Color );
	}
}

void clCanvas::Point( const LMatrix4& _proj, const LMatrix4& _view, const LVector3& Pt, float PtSize, const LVector4& Color )
{
	SetMatrices( _proj, _view );

	const LMatrix3& v = LMatrix3::Identity();

	for ( int i = 0 ; i < 3 ; i++ )
	{
		Line3D( Pt - PtSize * v[i], Pt + PtSize * v[i], Color );
	}
}

void clCanvas::ContactInfo( const LMatrix4& _proj, const LMatrix4& _view, Math::sCollisionInfo* Info, float PointSize, float NormalSize, const LVector4& PointColor1, const LVector4& PointColor2, const LVector4& NormalColor1, const LVector4& NormalColor2 )
{
	SetMatrices( _proj, _view );

	// draw each point and normal

	for ( int i = 0 ; i < Info->FNumPoints ; i++ )
	{
		LVector3 pt1 = Info->FPoints1[i];
		LVector3 pt2 = Info->FPoints2[i];

		Point( _proj, _view, pt1, PointSize, PointColor1 );
		Point( _proj, _view, pt2, PointSize, PointColor2 );

		if ( NormalSize > 0.0f )
		{
			Arrow3D( pt1 + Info->N1 * NormalSize, pt1, NormalSize * 0.2f, NormalColor1, NormalColor1 );
			Arrow3D( pt1 + Info->N1 * NormalSize, pt1, NormalSize * 0.2f, NormalColor2, NormalColor2 );
		}
	}
}

/*
 * 05/08/2011
     Merged with DebugDraw.cpp
 * 25/04/2011
     TextStrFreeType()
 * 16/12/2010
     Geom()
     Mesh() initial implementation
 * 18/07/2010
     TextStr3D() initial implementation
 * 01/12/2009
     DrawRect() reimplemented
 * 18/10/2009
     Canvas interface changed
*/
