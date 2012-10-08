/**
 * \file iShaderProgram.cpp
 * \brief Shader program
 * \version 0.5.91
 * \date 20/07/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "iShaderProgram.h"
#include "iRenderContext.h"

#include "Core/Console.h"
#include "Core/Logger.h"
#include "Core/VFS/FileSystem.h"
#include "Core/VFS/iOStream.h"

#include "GUI/GUIManager.h"

#include "Geometry/VertexAttribs.h"

iShaderProgram::iShaderProgram(): FShaderText( L_FRAGMENT_STAGE + 1 ),
	FRelinkPending( true ),
	FRenderTargets( 1 ),
	FStreams( L_VS_TOTAL_ATTRIBS )
{
	for ( size_t i = 0; i != FShaderText.size(); i++ )
	{
		FShaderText[i] = "";
	}
}

LString iShaderProgram::GetCachingDir() const
{
	return Env->Console->GetVarValueStr( "Cache.ShaderProgramsCachingDir", "Cache/ShaderPrograms" );
}

void iShaderProgram::ReadSegment( iIStream* IStream, const LString& IgnoreString, LString* Stage )
{
	guard();

	LString* Segment = Stage;

	LString EOL;

	EOL += char( 13 );
	EOL += char( 10 );

	while ( !IStream->Eof() )
	{
		LString Line = IStream->ReadLine();

		LString TrimmedLine = LStr::GetTrimmedSpaces( Line );

		if ( TrimmedLine.empty() ) { continue; }

		if ( LStr::StartsWith( TrimmedLine, IgnoreString ) ) { continue; }

		if ( TrimmedLine == "/*VERTEX_PROGRAM*/"      )
		{
			Segment = &FShaderText[ L_VERTEX_STAGE      ];
		}
		else if ( TrimmedLine == "/*TESSCONTROL_PROGRAM*/" )
		{
			Segment = &FShaderText[ L_TESSCONTROL_STAGE ];
		}
		else if ( TrimmedLine == "/*TESSEVAL_PROGRAM*/"    )
		{
			Segment = &FShaderText[ L_TESSEVAL_STAGE    ];
		}
		else if ( TrimmedLine == "/*GEOMETRY_PROGRAM*/"    )
		{
			Segment = &FShaderText[ L_GEOMETRY_STAGE    ];
		}
		else if ( TrimmedLine == "/*FRAGMENT_PROGRAM*/"    )
		{
			Segment = &FShaderText[ L_FRAGMENT_STAGE    ];
		}
		else if ( TrimmedLine == "/*INPUT_TYPE*/" )
		{
			LString Type = IStream->ReadLine();

			this->SetInputPrimitiveType( Type );
		}
		else if ( TrimmedLine == "/*OUTPUT_TYPE*/" )
		{
			LString Type = IStream->ReadLine();

			SetOutputPrimitiveType( Type );
		}
		else if ( TrimmedLine == "/*OUTPUT_VERTICES*/" )
		{
			LString Count = IStream->ReadLine();

			FATAL( !LStr::IsCorrectInt( Count ), "Integer number expected for output vertices count" );

			SetOutputVerticesCount( LStr::ToInt( Count ) );
		}
		else if ( LStr::StartsWith( TrimmedLine, "#include" ) )
		{
			LString FileName = LStr::GetToken( TrimmedLine, 2 );

			LString DefinesList = GetDefinesList();

			// process defines list - file name can contain a macro
			if ( !DefinesList.empty() ) for ( int i = 1;  ; i++ )
				{
					LString Token = LStr::GetToken( DefinesList, i );

					if ( Token.empty() ) { break; }

					std::vector<LString> MacroDef( 3 );

					LStr::TrimBrackets( &Token );
					LStr::FastSplitLine( 1, 2, Token, MacroDef, false );

					if ( !MacroDef[2].empty() ) { FileName = LStr::ReplaceAllSubStr( FileName, MacroDef[1], MacroDef[2] ); }
				}

			// locate folder
			FileName = Env->FileSystem->FindFile( FileName, GetFileName() );

			iIStream* Include = Env->FileSystem->CreateFileReader( FileName );

			// load ignoring recursive includes
			ReadSegment( Include, "#include", Segment );

			delete( Include );
		}
		else
		{
			( *Segment ) += Line + EOL;
		}
	}

	unguard();
}

bool iShaderProgram::LoadFromFile( const LString& FileName )
{
	guard( "%s", FileName.c_str() );

	iIStream* Stream = Env->FileSystem->CreateFileReader( GetFileName() );

	if ( Stream == NULL )
	{
		// some error happened
		return false;
	}

	for ( size_t i = 0; i != FShaderText.size(); i++ )
	{
		FShaderText[i] = "";
	}

	ReadSegment( Stream, "////", &FShaderText[ L_VERTEX_STAGE ] );

	FRelinkPending = true;

	delete ( Stream );

	FATAL( GetShaderCode( L_VERTEX_STAGE ).empty() ||
	       GetShaderCode( L_FRAGMENT_STAGE ).empty(), "Empty shader program: " + FileName );

	RelinkShaderProgram();

	return true;

	unguard();
}

void iShaderProgram::SetShaderCode( LShaderStage Stage, const LString& ShaderCode )
{
	if ( GetShaderCode( Stage ) == ShaderCode )
	{
		return;
	}

	FShaderText[ Stage ] = ShaderCode;

	FRelinkPending = true;
}

LString iShaderProgram::GetShaderCode( LShaderStage Stage ) const
{
	return FShaderText[ Stage ];
}

bool iShaderProgram::RelinkShaderProgram()
{
	guard();

	FRelinkPending = false;

	FIDin_ProjectionMatrix   = CreateUniform( "in_ProjectionMatrix" );
	FIDin_ViewMatrix         = CreateUniform( "in_ViewMatrix" );
	FIDin_ViewMatrixInverse  = CreateUniform( "in_ViewMatrixInverse" );
	FIDin_ModelMatrix        = CreateUniform( "in_ModelMatrix" );
	FIDin_ModelMatrixInverse = CreateUniform( "in_ModelMatrixInverse" );
	FIDin_ModelViewMatrix    = CreateUniform( "in_ModelViewMatrix" );
	FIDin_ModelViewMatrixInverse    = CreateUniform( "in_ModelViewMatrixInverse" );
	FIDin_ModelViewProjectionMatrix = CreateUniform( "in_ModelViewProjectionMatrix" );
	FIDin_NormalMatrix              = CreateUniform( "in_NormalMatrix" );

	FID_MTL_AMBIENT_COLOR    = CreateUniform( "MTL_AMBIENT_COLOR" );
	FID_MTL_DIFFUSE_COLOR    = CreateUniform( "MTL_DIFFUSE_COLOR" );
	FID_MTL_SPECULAR_COLOR   = CreateUniform( "MTL_SPECULAR_COLOR" );
	FID_MTL_SHININESS        = CreateUniform( "MTL_SHININESS" );
	FID_MTL_TRANSPARENCY     = CreateUniform( "MTL_TRANSPARENCY" );
	FID_MTL_REFLECTIVITY     = CreateUniform( "MTL_REFLECTIVITY" );
	FID_MTL_ROUGHNESS        = CreateUniform( "MTL_COOK_TORRANCE_ROUGHNESS" );
	FID_MTL_RECEIVESHADOW    = CreateUniform( "MTL_RECEIVESHADOW" );
	FID_MTL_PLANARREFLECTION = CreateUniform( "MTL_PLANARREFLECTION" );
	FID_MTL_USEAMBIENTONLY   = CreateUniform( "MTL_USEAMBIENTONLY" );

	FID_MTL_TESSINNERLEVEL   = CreateUniform( "MTL_TESSINNERLEVEL" );
	FID_MTL_TESSOUTERLEVEL   = CreateUniform( "MTL_TESSOUTERLEVEL" );

	FID_GUI_Pos            = CreateUniform( "GUI_Pos" );
	FID_GUI_Opacity        = CreateUniform( "GUI_Opacity" );
	FID_GUI_ViewLifeTime   = CreateUniform( "GUI_LifeTime" );

	FID_PICKING_COLOR      = CreateUniform( "PICKING_COLOR" );
	FID_KeyFrame           = CreateUniform( "KeyFrame" );
	FID_Skinned            = CreateUniform( "Skinned" );

	FID_EnableClipPlane    = CreateUniform( "EnableClipPlane" );
	FID_ClipPlane          = CreateUniform( "ClipPlane" );
	FID_ShowNormals        = CreateUniform( "ShowNormals" );

	FID_ShadowMatrix       = CreateUniform( "in_ShadowMatrix" );

	FID_NumLights          = CreateUniform( "in_NumLights" );
	FID_LightEnabledFlag   = CreateUniform( "in_LightEnabled" );
	FID_LightTransform     = CreateUniform( "in_LightTransform" );
	FID_LightAmbientColor  = CreateUniform( "in_LightAmbientColor" );
	FID_LightDiffuseColor  = CreateUniform( "in_LightDiffuseColor" );
	FID_LightSpecularColor = CreateUniform( "in_LightSpecularColor" );
	FID_LightType          = CreateUniform( "in_LightType" );
	FID_LightAttenuation   = CreateUniform( "in_LightAttenuation" );
	FID_LightSpotParams    = CreateUniform( "in_LightSpotParameters" );
	FID_LightSpotDirection = CreateUniform( "in_LightSpotDirections" );

	SetTransformationUniforms( *Env->Renderer->GetCanvas()->GetOrthoMatrices() );

	unguard();

	return true;
}

void iShaderProgram::SetRenderTargetName( size_t i, const LString& Name )
{
	if ( i >= FRenderTargets.size() ) { FRenderTargets.resize( i + 1 ); }

	FRenderTargets[i] = Name;
}

LString iShaderProgram::GetRenderTargetName( size_t i ) const
{
	if ( i >= FRenderTargets.size() ) { return LString(); }

	return FRenderTargets[i];
}

size_t iShaderProgram::GetNumRenderTargets() const
{
	return FRenderTargets.size();
}

void iShaderProgram::SetNumRenderTargets( size_t NumTargets )
{
	FRenderTargets.resize( NumTargets );
}

/*
 * 20/07/2010
     Loading of tesselation control and tesselation evaluation programs
 * 09/03/2010
     Includes supported with "#include filename.sp"
 * 13/12/2008
     BindCVarToUniform()
 * 21/03/2005
     It's here
*/
