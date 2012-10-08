/**
 * \file RenderState.cpp
 * \brief Generic render state
 * \version 0.5.93
 * \date 04/11/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "RenderState.h"
#include "iRenderContext.h"

#include "Core/Linker.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "Core/Logger.h"
#include "Core/VFS/FileSystem.h"
#include "Core/VFS/iOStream.h"
#include "Renderer/iRenderTarget.h"
#include "Renderer/iTexture.h"
#include "Renderer/iShaderProgram.h"
#include "Resources/ResourcesManager.h"

namespace Linderdaum
{

/// Internal helpers for Linderdaum Surface Shaders parsing
	namespace LSS
	{
		typedef std::pair<LString, LBlendMode> clLookupPairBlend;

		clLookupPairBlend BlendModesArray[] =
		{
			clLookupPairBlend( "ZERO",                     L_ZERO ),
			clLookupPairBlend( "ONE",                      L_ONE ),
			clLookupPairBlend( "SRC_COLOR",                L_SRC_COLOR ),
			clLookupPairBlend( "ONE_MINUS_SRC_COLOR",      L_ONE_MINUS_SRC_COLOR ),
			clLookupPairBlend( "SRC_ALPHA",                L_SRC_ALPHA ),
			clLookupPairBlend( "ONE_MINUS_SRC_ALPHA",      L_ONE_MINUS_SRC_ALPHA ),
			clLookupPairBlend( "DST_ALPHA",                L_DST_ALPHA ),
			clLookupPairBlend( "ONE_MINUS_DST_ALPHA",      L_ONE_MINUS_DST_ALPHA ),
			clLookupPairBlend( "DST_COLOR",                L_DST_COLOR ),
			clLookupPairBlend( "ONE_MINUS_DST_COLOR",      L_ONE_MINUS_DST_COLOR ),
			clLookupPairBlend( "SRC_ALPHA_SATURATE",       L_SRC_ALPHA_SATURATE ),
			clLookupPairBlend( "CONSTANT_COLOR",           L_CONSTANT_COLOR ),
			clLookupPairBlend( "ONE_MINUS_CONSTANT_COLOR", L_ONE_MINUS_CONSTANT_COLOR ),
			clLookupPairBlend( "CONSTANT_ALPHA",           L_CONSTANT_ALPHA ),
			clLookupPairBlend( "ONE_MINUS_CONSTANT_ALPHA", L_ONE_MINUS_CONSTANT_ALPHA ),
			clLookupPairBlend( "",                         L_INVALID_ENUM )
		};

		LBlendMode LookupBlendMode( const LString& Mode )
		{
			for ( size_t i = 0; ; i++ )
			{
				if ( BlendModesArray[i].first == Mode ) { return BlendModesArray[i].second; }

				if ( BlendModesArray[i].second == L_INVALID_ENUM ) { return L_INVALID_ENUM; }
			}
		}

		LString LookupBlendModeName( LBlendMode Mode )
		{
			for ( size_t i = 0; ; i++ )
			{
				if ( BlendModesArray[i].second == Mode ) { return BlendModesArray[i].first; }

				if ( BlendModesArray[i].second == L_INVALID_ENUM ) { return ""; }
			}
		}
	} // namespace LSS
} // namespace Linderdaum

clRenderState::clRenderState(): FTextureID( 0 ),
	FTextureDeallocateOnExit( 0 ),
	FShaderProgram( NULL ),
	FBlended( false ),
	FBlendSrc( L_ZERO ),
	FBlendDst( L_ONE ),
	FDepthTest( true ),
	FDepthWrites( true ),
	FColorWrites( true ),
	FBackfaceCulling( false ),
	FBackFacing( true ),
	FMaxSupportedTextures( 8 ),
	// loader
	FRenderStateMask( 0 ),
	FShaderProgramName( "" )
{
}

void clRenderState::AfterConstruction()
{
	iResource::AfterConstruction();

	if ( Env->Renderer )
	{
		FMaxSupportedTextures = Env->Renderer->GetVideoSystemInfo().FMaxTextureUnits;
	}
}

clRenderState::~clRenderState()
{
	for ( size_t i = 0; i != FTextureID.size(); i++ )
	{
		if ( FTextureDeallocateOnExit[ i ] ) { delete( FTextureID[i] ); }
	}
}

void clRenderState::SetTexture( int TexUnit, iTexture* Tex, bool Ownership )
{
	if ( TexUnit < 0 ) { return; }

	if ( TexUnit >= FMaxSupportedTextures ) { return; }

	if ( static_cast<size_t>( TexUnit ) >= FTextureID.size() )
	{
		FTextureID.resize( TexUnit + 1 );
		FTextureDeallocateOnExit.resize( TexUnit + 1 );
	}

	if ( FTextureDeallocateOnExit[ TexUnit ] )
	{
		delete( FTextureID[TexUnit] );
	}

	FTextureID[TexUnit] = Tex;
	FTextureDeallocateOnExit[TexUnit] = Ownership;
}

iTexture* clRenderState::GetTextureForTextureUnit( int Tex ) const
{
	if ( Tex >= static_cast<int>( FTextureID.size() ) ) { return NULL; }

	return FTextureID[Tex];
}

LString clRenderState::GetCachingDir() const
{
	return Env->Console->GetVarValueStr( "Cache.ShadersCachingDir", "Cache/Shaders" );
}

void clRenderState::UpdateTextures( iRenderTarget* RenderTarget )
{
	SetTexture( 0, RenderTarget->GetColorTexture( 0 ), false );
	SetTexture( 1, RenderTarget->GetDepthTexture(), false );
}

void clRenderState::SetBlending( LBlendMode BlendSrc, LBlendMode BlendDst )
{
	FBlended  = true;
	FBlendSrc = BlendSrc;
	FBlendDst = BlendDst;
}

bool clRenderState::IsSameResource( iResource* Other )
{
	clRenderState* OtherLSS = dynamic_cast<clRenderState*>( Other );

	if ( !OtherLSS ) { return false; }

	if ( GetFileName() != OtherLSS->GetFileName() ) { return false; }

	if ( FDefines != OtherLSS->FDefines ) { return false; }

	return true;
}

void clRenderState::SetBlendFunc( const LString& ParamValue )
{
	if ( ParamValue.empty() )
	{
		FBlended = false;
		return;
	}

	LString SrcName = LStr::GetToken( ParamValue, 1 );
	LString DstName = LStr::GetToken( ParamValue, 2 );

	LBlendMode BlendSrc = ::Linderdaum::LSS::LookupBlendMode( SrcName );
	LBlendMode BlendDst = ::Linderdaum::LSS::LookupBlendMode( DstName );

	if ( BlendSrc == L_INVALID_ENUM )
	{
		Env->Console->DisplayError( "Invalid enum for \"" + SrcName + "\" in " + GetFileName() );
	}

	if ( BlendDst == L_INVALID_ENUM )
	{
		Env->Console->DisplayError( "Invalid enum for \"" + DstName + "\"in " + GetFileName() );
	}

	if ( BlendSrc != L_INVALID_ENUM && BlendDst != L_INVALID_ENUM )
	{
		SetBlending( BlendSrc, BlendDst );
	}
}

LString clRenderState::GetBlendFunc() const
{
	LString Res = ::Linderdaum::LSS::LookupBlendModeName( FBlendSrc ) + LString( " " ) +
	              ::Linderdaum::LSS::LookupBlendModeName( FBlendDst );

	// empty blending is ok
	if ( Res == " " ) { Res.clear(); }

	return Res;
}

void clRenderState::SetConsoleVar( const LString& ParamValue )
{
	if ( ParamValue.empty() ) { return; }

	if ( !GetShaderProgram() )
	{
		Env->Console->DisplayError( "FILE should be defined before BINDCVAR in " + GetFileName() );
	}
	else
	{
		LString VarName = LStr::GetToken( ParamValue, 1 );
		GetShaderProgram()->BindConsoleVariable( VarName, Env->Console->GetVar( VarName ) );
	}
}

LString clRenderState::GetShaderProgramName() const
{
	return FShaderProgramName;
}

void clRenderState::SetShaderProgramName( const LString& ParamValue )
{
	// just store the name
	FShaderProgramName = ParamValue;
}

void clRenderState::UpdateRenderStateMask()
{
	FRenderStateMask = 0;

	FRenderStateMask |= ( FBlended         ? L_STATE_BLENDED         : 0 );
	FRenderStateMask |= ( FDepthTest       ? L_STATE_DEPTH_TEST      : 0 );
	FRenderStateMask |= ( FDepthWrites     ? L_STATE_DEPTH_WRITES    : 0 );
	FRenderStateMask |= ( FColorWrites     ? L_STATE_COLOR_WRITES    : 0 );
	FRenderStateMask |= ( FBackfaceCulling ? L_STATE_BACKFACECULLING : 0 );
	FRenderStateMask |= ( FBackFacing      ? L_STATE_BACKFACING      : 0 );
}

void clRenderState::CopyTo( clRenderState* Other )
{
	Other->FTextureID = FTextureID;
	Other->FTextureDeallocateOnExit = FTextureDeallocateOnExit;
	Other->FBlended = FBlended;
	Other->FBlendSrc = FBlendSrc;
	Other->FBlendDst = FBlendDst;
	Other->FDepthTest = FDepthTest;
	Other->FDepthWrites = FDepthWrites;
	Other->FColorWrites = FColorWrites;
	Other->FBackfaceCulling = FBackfaceCulling;
	Other->FBackFacing = FBackFacing;
	Other->FShaderProgram = FShaderProgram;

	// don't pass textures ownership
	for ( size_t i = 0; i != FTextureDeallocateOnExit.size(); i++ )
	{
		Other->FTextureDeallocateOnExit[ i ] = false;
	}
}

/// Compile shader program, create textures and combine defines
bool clRenderState::EndLoad()
{
	// 1. Combine defines
	LString DefinesList;

	for ( size_t i = 0 ; i < FDefines.size() ; i++ )
	{
		DefinesList += FDefines[i] + LString( " " );
	}

	// 2. Compile shader program

	// check in local directory first
	LString ProgramName = Env->FileSystem->FindFile( FShaderProgramName, GetFileName() );

	/// load and compile
	FATAL( ProgramName.empty(), "No shader program specified" );

	SetShaderProgram( Env->Resources->LoadSP( ProgramName, DefinesList ) );

	// 3. Bind console variables
	for ( size_t i = 0 ; i != FUniforms.size() ; i++ )
	{
		GetShaderProgram()->BindConsoleVariable( FUniforms[i], Env->Console->GetVar( FUniforms[i] ) );
	}

	// 4. Create textures
	for ( size_t i = 0 ; i != FImages.size() ; i++ )
	{
		LString TexType   = LStr::GetUpper( LStr::GetToken( FImages[i], 1 ) );
		LString TexNumber = LStr::GetToken( FImages[i], 2 );
		LString FileName  = LStr::GetToken( FImages[i], 3 );
		int Iterations    = LStr::ToInt( LStr::GetToken( FImages[i], 4 ) );
		//int FPS           = LStr::ToInt( LStr::GetToken( FImages[i], 5 ) );
		int ExpandZero    = LStr::ToInt( LStr::GetToken( FImages[i], 6 ) );

		// 1. Check for correct texture ID
		FATAL( !LStr::IsCorrectInt( TexNumber ), "Invalid texture unit number: " + TexNumber );
		FATAL( static_cast<int>( i ) != LStr::ToInt( TexNumber ), "Texture units must go in ascending order: \n Got " + TexNumber + " expected " + LStr::ToStr( i ) );

		// 2. Check for correct texture type
		LTextureType TextureType = L_TEXTURE_2D;

		int SizeXHint = 0;
		int SizeYHint = 0;
		int SizeZHint = 0;
		bool AutoGradient = false;
		bool AutoESL = false;

		if ( TexType == "TEXTURE" || TexType == "TEXTURE2D" )
		{
			TextureType = L_TEXTURE_2D;
		}
		else if ( TexType == "TEXTURE3D" )
		{
			TextureType = L_TEXTURE_3D;
			Iterations = 1;
			SizeXHint = LStr::ToInt( LStr::GetToken( FImages[i], 3 ) );
			SizeYHint = LStr::ToInt( LStr::GetToken( FImages[i], 4 ) );
			SizeZHint = LStr::ToInt( LStr::GetToken( FImages[i], 5 ) );

			LString TokenNum6 = LStr::GetUpper( LStr::GetToken( FImages[i], 7 ) );

			AutoGradient = ( TokenNum6 == "AUTOGRADIENT" );
			AutoESL      = ( TokenNum6 == "ESL" );
		}
		else if ( TexType == "TEXTURECUBE" )
		{
			TextureType = L_TEXTURE_CUBE;
		}

		// 3. Extract file name
		// preserve file name pattern - cann't load here, because of possible texture animation
		if ( ( FileName.length() > 2 ) && ( FileName[1] == '%' ) && ( FileName[2] == '%' ) )
		{
			clCVar* FileNameVar = Env->Console->GetVar( FileName );

			if ( !FileNameVar->GetString().empty() ) { FileName = FileNameVar->GetString(); }
		}

		bool ParametrizedName = LStr::ContainsSubStr( FileName, "%i" );

		Iterations = 1;

		// loading textures from files
		for ( int TexIter = 0; TexIter != Iterations; TexIter++ )
		{
			LString TextureName = ParametrizedName ? LStr::GetFormattedString( FileName, ExpandZero, TexIter + 1 ) : FileName;

			clImage* Image = Env->Resources->LoadImgHints( TextureName, TextureType, SizeXHint, SizeYHint, SizeZHint, AutoGradient, AutoESL );

			SetTexture( static_cast<int>( i ), Image->GetTexture(), false );

			CheckRendererError( "" );

			Env->Logger->Log( L_NOTICE, "Texture loaded from file: " + TextureName );
		}
	}

	return true;
}

/*
 * 23/03/2011
     UpdateRenderStateMask()
 * 04/11/2010
     Legacy code removed
 * 23/08/2010
     API-dependant parts moved to render context
 * 05/02/2010
     Merged with GLShaderSetup.cpp
 * 03/02/2010
     Renamed. Log truncated.
 * 04/01/2005
     It's here
*/
