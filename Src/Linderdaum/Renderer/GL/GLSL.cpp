/**
 * \file GLSL.cpp
 * \brief OpenGL Shading Language
 * \version 0.5.91
 * \date 11/08/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "GLSL.h"
#include "GLRenderContext.h"

#include "Core/Console.h"
#include "Core/CVars.h"
#include "Core/Logger.h"
#include "Core/VFS/FileSystem.h"
#include "Geometry/VertexAttribs.h"
#include "Resources/ResourcesManager.h"

#include "Scene/Scene.h"

#include "Generated/LGL/LGL.h"

clGLSLShaderProgram::clGLSLShaderProgram(): FUniforms(),
	FCVarsBindedUniforms(),
	FProgramID( 0 ),
	FShaderID( L_FRAGMENT_STAGE + 1 ),
	FTesselation( false ),
	FAlreadyLoaded( false ),
	FInputType( "" ),
	FOutputType( "" ),
	FOutputVertices( 0 ),
	FDefinesList( "" ),
	//FMatricesBufferID(0),
	FMatricesBlockIndex( -1 ),
	FMatricesBlockSize( -1 )
{
	SetRenderTargetName( 0, "out_FragColor" );
#if defined( USE_OPENGL_3 )
	SetRenderTargetName( 1, "out_PickMask" );
	SetRenderTargetName( 2, "out_Normals" );
	SetRenderTargetName( 3, "out_WorldPos" );
#endif

	for ( size_t i = 0; i != FShaderID.size(); i++ )
	{
		FShaderID[i] = 0;
	}
}

void clGLSLShaderProgram::AfterConstruction()
{
	iShaderProgram::AfterConstruction();

	GetAPI( Env );

	Env->Connect( L_EVENT_SURFACE_ATTACHED, BIND( &clGLSLShaderProgram::Event_SURFACE_ATTACHED ) );
	Env->Connect( L_EVENT_SURFACE_DETACHED, BIND( &clGLSLShaderProgram::Event_SURFACE_DETACHED ) );

	FProgramRebinds = Env->Console->GetVar( "Stats.ProgramRebinds" );
	FProgramRebinds->SetInt( 0 );
}

void clGLSLShaderProgram::EVENT_HANDLER( Event_SURFACE_DETACHED )
{
	Env->Logger->LogP( L_DEBUG, "clGLSLShaderProgram::EVENT_HANDLER( Event_SURFACE_DETACHED )" );

	FProgramID = 0;

	for ( size_t i = 0; i != FShaderID.size(); i++ )
	{
		FShaderID[i] = 0;
	}

	SetRelinkPending( true );
}

void clGLSLShaderProgram::EVENT_HANDLER( Event_SURFACE_ATTACHED )
{
	Env->Logger->LogP( L_DEBUG, "clGLSLShaderProgram::EVENT_HANDLER( Event_SURFACE_ATTACHED )" );

	RelinkShaderProgram();
}

LString clGLSLShaderProgram::GetCachedFileName()
{
	LString CachingDir = GetCachingDir();
	LStr::AddTrailingChar( &CachingDir, PATH_SEPARATOR );

	LString FN = clResourcesManager::ConvertName( GetFileName() + GetDefinesList() );

	return ( CachingDir + FN + ".cached" );
}

clGLSLShaderProgram::~clGLSLShaderProgram()
{
	Env->DisconnectObject( this );

	if ( FProgramID )
	{
		LGL3->glDeleteProgram( FProgramID );
	}

	for ( size_t i = 0; i != FShaderID.size(); i++ )
	{
		if ( FShaderID[i] )
		{
			LGL3->glDeleteShader( FShaderID[i] );
		}
	}

//   if ( FMatricesBufferID ) glDeleteBuffers( 1, &FMatricesBufferID );
}

LString clGLSLShaderProgram::GetCachingDir() const
{
	return Env->Console->GetVarValueStr( "Cache.ShaderProgramsCachingDir", "Cache/ShaderPrograms" );
}

void clGLSLShaderProgram::CacheTo( const LString& FileName )
{
	guard( "%s", FileName.c_str() );

	// shader program was not linked successfuly
	if ( !IsLinked() )
	{
		return;
	}

	// shader program saving is not supported
	if ( LGL3->glGetProgramBinary == NULL || LGL3->glProgramBinary == NULL || LGL3->glProgramParameteri == NULL )
	{
		return;
	}

	Lenum BinaryFormat    = 0;
	Lint  BinaryImageSize = 0;
	char* BinaryImage     = NULL;

	LGL3->glUseProgram( FProgramID );
	Env->Renderer->SetCurrentBindedSP( this );

	LGL3->glGetProgramiv( FProgramID, GL_PROGRAM_BINARY_LENGTH, &BinaryImageSize );

	// got nothing interesting
	if ( !BinaryImageSize )
	{
		return;
	}

	BinaryImage = new char[BinaryImageSize];

	LGL3->glGetProgramBinary( FProgramID, BinaryImageSize, NULL, &BinaryFormat, BinaryImage );

	iShaderProgram::CacheTo( FileName );

	iOStream* Stream = Env->FileSystem->CreateFileWriter( FileName );

	Stream->BlockWrite( &BinaryFormat,    sizeof( BinaryFormat    ) );
	Stream->BlockWrite( &BinaryImageSize, sizeof( BinaryImageSize ) );
	Stream->WriteLine( GetDefinesList() );
	Stream->BlockWrite( BinaryImage,      BinaryImageSize         );

	delete( Stream );
	delete[]( BinaryImage );

	unguard();
}

bool clGLSLShaderProgram::CacheFrom( const LString& FileName )
{
	// FIXME: implement
	return false;

	/////
	/////
	/////
	/*
	   guard( "%s", FileName.c_str() );

	   if ( glGetProgramBinary == NULL || glProgramBinary == NULL || glProgramParameteri == NULL ) return false;

	   iResource::CacheFrom( FileName );

	   iIStream* Stream = Env->FileSystem->CreateFileReader( FileName );

	   Lenum BinaryFormat    = 0;
	   Lint  BinaryImageSize = 0;

	   Stream->BlockRead( &BinaryFormat,    sizeof( BinaryFormat    ) );
	   Stream->BlockRead( &BinaryImageSize, sizeof( BinaryImageSize ) );
	   SetDefinesList( Stream->ReadLine() );

	   FProgramID = glCreateProgram();

	   glProgramBinary( FProgramID, BinaryFormat, Stream->MapStreamFromCurrentPos(), BinaryImageSize );

	   CheckRendererError("");

	   delete( Stream );

	   // fallback to default loading routine
	   if ( !IsLinked() )
	   {
	      glDeleteProgram( FProgramID );

	      FProgramID = 0;

	      return false;
	   }

	   // don't try to relink precached shader program later
	   SetRelinkPending( false );

	   RebindAllUniforms();

	   CheckRendererError("");

	   return IsLinked();

	   unguard();
	   */
}

bool clGLSLShaderProgram::IsSameResource( iResource* Other )
{
	clGLSLShaderProgram* OtherSP = dynamic_cast<clGLSLShaderProgram*>( Other );

	if ( !OtherSP )
	{
		return false;
	}

	if ( GetFileName() != OtherSP->GetFileName() )
	{
		return false;
	}

	if ( GetDefinesList() != OtherSP->GetDefinesList() )
	{
		return false;
	}

	return true;
}

Luint clGLSLShaderProgram::AttachShaderID( Luint Target, const LString& ShaderCode, Luint OldShaderID )
{
	CheckRendererError( "" );

	double Time_Start;
	double Time_Create, Time_Source, Time_Compile, Time_Subst, Time_Define;

	Time_Start = Env->GetSeconds();

#if defined( USE_OPENGL_3 )
	LString ShaderStr      = LGL3->Has_GL_ARB_tessellation_shader ? "#version 410 core\n" : "#version 150 core\n";
#elif defined( USE_OPENGL_ES_2 )
	LString ShaderStr      = "#version 100\n";
	ShaderStr             += "precision highp float;\n";
	ShaderStr             += "#define USE_OPENGL_ES_2\n";
#endif

	LString ShaderCodeUsed = ShaderCode;

#if defined( USE_OPENGL_ES_2 )
	// do some preprocessing to downgrade GLSL 1.50 to GLSL 1.00
	ShaderCodeUsed = LStr::ReplaceAllSubStr( ShaderCodeUsed, "texture(", "texture2D(" );
	ShaderCodeUsed = LStr::ReplaceAllSubStr( ShaderCodeUsed, "textureProj(", "texture2DProj(" );
	ShaderCodeUsed = LStr::ReplaceAllSubStr( ShaderCodeUsed, "textureLod(", "texture2DLod(" );
	ShaderCodeUsed = LStr::ReplaceAllSubStr( ShaderCodeUsed, "sampler3D ", "sampler2D " );

	if ( Target == GL_VERTEX_SHADER )
	{
		ShaderCodeUsed = LStr::ReplaceAllSubStr( ShaderCodeUsed, "in ", "attribute " );
		ShaderCodeUsed = LStr::ReplaceAllSubStr( ShaderCodeUsed, "out ", "varying " );
	}

	if ( Target == GL_FRAGMENT_SHADER )
	{
		ShaderCodeUsed = LStr::ReplaceAllSubStr( ShaderCodeUsed, "out vec4 out_FragColor;", "" );
		ShaderCodeUsed = LStr::ReplaceAllSubStr( ShaderCodeUsed, "out vec4 out_PickMask;", "" );

		ShaderCodeUsed = LStr::ReplaceAllSubStr( ShaderCodeUsed, "out_FragColor", "gl_FragColor" );

		ShaderCodeUsed = LStr::ReplaceAllSubStr( ShaderCodeUsed, "in ", "varying " );
	}

#endif

	Time_Subst = Env->GetSeconds() - Time_Start;

	Time_Start = Env->GetSeconds();

	bool UseTesselation = false;

	// process defines list
	for ( int i = 1;  ; i++ )
	{
		LString Token = LStr::GetToken( FDefinesList, i );

		if ( Token.empty() ) { break; }

		// tokens can be of two types:
		//   - single definition
		//   - macro  definition

		LStr::TrimBrackets( &Token );

		std::vector<LString> MacroDef( 3 );

		LStr::FastSplitLine( 1, 2, Token, MacroDef, false );

		if ( MacroDef[2].empty() )
		{
			ShaderStr += "#define " + Token + "\n";
//			Env->Logger->Log( L_DEBUG, "Adding shader define: " + Token );
		}
		else
		{
			ShaderCodeUsed = LStr::ReplaceAllSubStr( ShaderCodeUsed, MacroDef[1], MacroDef[2] );
//			Env->Logger->Log( L_DEBUG, "Adding macro define: " + Token );
		}

		if ( Token == "MATSYS_TESS" ) { UseTesselation = true; }
	}

	if ( Target == GL_TESS_EVALUATION_SHADER || Target == GL_TESS_CONTROL_SHADER )
	{
		if ( !LGL3->Has_GL_ARB_tessellation_shader ) { return 0; }

		if ( !UseTesselation ) { return 0; }

		FTesselation = true;
	}

	ShaderStr += ShaderCodeUsed;

	Time_Define = Env->GetSeconds() - Time_Start;

//	Env->Logger->Log( L_NOTICE, LString( "\n" ) + ShaderStr );

	Time_Start = Env->GetSeconds();
	Luint Shader = LGL3->glCreateShader( Target );
	Time_Create = Env->GetSeconds() - Time_Start;

	Time_Start = Env->GetSeconds();
	const char* Code = ShaderStr.c_str();

	CheckRendererError( "" );

	LGL3->glShaderSource( Shader, 1, &Code, NULL );
	Time_Source = Env->GetSeconds() - Time_Start;

	Time_Start = Env->GetSeconds();
	Env->Logger->LogP( L_DEBUG, "Compiling shader for stage: %X", Target );

	LGL3->glCompileShader( Shader );
	Time_Compile = Env->GetSeconds() - Time_Start;

	if ( !CheckStatus( Shader, GL_COMPILE_STATUS, "Shader wasn''t compiled: " + GetFileName() ) )
	{
		CheckRendererError( "" );

		LGL3->glDeleteShader ( Shader );

		CheckRendererError( "" );

		return OldShaderID;

//      FATAL_MSG( "INTERNAL ERROR: invalid shader target" );
	}

	if ( OldShaderID )
	{
		LGL3->glDeleteShader( OldShaderID );
	}

	/// The timing code for PC shows that Time_Subst/Time_Define are neglibile
#if 0
	Env->Logger->LogP( L_DEBUG, "Time[Create ] = %f", Time_Create );
	Env->Logger->LogP( L_DEBUG, "Time[Source ] = %f", Time_Source );
	Env->Logger->LogP( L_DEBUG, "Time[Compile] = %f", Time_Compile );
	Env->Logger->LogP( L_DEBUG, "Time[Subst  ] = %f", Time_Subst );
	Env->Logger->LogP( L_DEBUG, "Time[Define ] = %f", Time_Define );
#endif

	return Shader;
}

void clGLSLShaderProgram::SetInputPrimitiveType( const LString& Type )
{
	FInputType = Type;
}

void clGLSLShaderProgram::SetOutputPrimitiveType( const LString& Type )
{
	FOutputType = Type;
}

void clGLSLShaderProgram::SetOutputVerticesCount( Luint OutputVerticesCount )
{
	FOutputVertices = OutputVerticesCount;
}

bool clGLSLShaderProgram::IsLinked() const
{
	Lint Linked = 0;

	LGL3->glGetProgramiv( FProgramID, GL_LINK_STATUS, &Linked );

	return Linked != 0;
}

bool clGLSLShaderProgram::CheckStatus( Luint ObjectID, Lenum Target, const LString& Message ) const
{
	Lint   SuccessFlag = 0;
	Lsizei Length      = 0;
	Lsizei MaxLength   = 0;

	char* InfoLog = NULL;

	//
	if ( LGL3->glIsProgram( ObjectID ) )
	{
		LGL3->glGetProgramiv( ObjectID, Target, &SuccessFlag );
		LGL3->glGetProgramiv( ObjectID, GL_INFO_LOG_LENGTH, &MaxLength );

		InfoLog = new char[MaxLength];

		LGL3->glGetProgramInfoLog( ObjectID, MaxLength, &Length, InfoLog );

		Env->Logger->LogP( L_DEBUG, "Program info:\n%s", InfoLog );
	}
	else if ( LGL3->glIsShader( ObjectID ) )
	{
		LGL3->glGetShaderiv( ObjectID, Target, &SuccessFlag );
		LGL3->glGetShaderiv( ObjectID, GL_INFO_LOG_LENGTH, &MaxLength );

		InfoLog = new char[MaxLength];

		LGL3->glGetShaderInfoLog( ObjectID, MaxLength, &Length, InfoLog );

		Env->Logger->LogP( L_DEBUG, "Shader info:\n%s", InfoLog );
	}
	else
	{
		FATAL_MSG( "Unknown object type" );
	}

	if ( FAlreadyLoaded )
	{
		// flush the error code
		LGL3->glGetError();
	}

	if ( SuccessFlag == 0 )
	{
		Env->Console->DisplayError( Message + ": \n\n" + LString( InfoLog ) );

		delete[]( InfoLog );

		return false;
	}

	delete[]( InfoLog );

	CheckRendererError( "" );

	return true;
}

// should match LShaderStage enum
static const Lenum LStageToGLStage[ L_FRAGMENT_STAGE + 1 ] =
{
	GL_VERTEX_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER,
	GL_GEOMETRY_SHADER,
	GL_FRAGMENT_SHADER
};

bool clGLSLShaderProgram::RelinkShaderProgram()
{
	guard();

	if ( !IsRelinkPending() )
	{
		return true;
	}

	CheckRendererError( "" );

	Luint ProgramID = LGL3->glCreateProgram();

	for ( size_t i = 0; i != FShaderID.size(); i++ )
	{
		LString ShaderCode = GetShaderCode( ( LShaderStage )i );

		if ( !ShaderCode.empty() )
		{
			FShaderID[ i ] = AttachShaderID( LStageToGLStage[ i ], ShaderCode, FShaderID[i] );

			if ( FShaderID[ i ] )
			{
				LGL3->glAttachShader( ProgramID, FShaderID[i] );
			}
		}
	}

	BindDefaultLocations( ProgramID );

	if ( LGL3->Has_GL_ARB_get_program_binary )
	{
		LGL3->glProgramParameteri( ProgramID, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE );
	}

	LGL3->glLinkProgram( ProgramID );

	CheckRendererError( "" );

	if ( !CheckStatus( ProgramID, GL_LINK_STATUS, "Program wasn''t linked" ) )
	{
		Env->Logger->Log( L_WARNING, "INTERNAL ERROR: Error while shader relinking" );

		return false;
	}

	// not for AMD Radeon
	//LGL3->glValidateProgram( ProgramID );

	//CheckStatus( ProgramID, GL_VALIDATE_STATUS, "Program wasn''t validated" );

	CheckRendererError( "" );

	// replace old program
	if ( FProgramID )
	{
		LGL3->glDeleteProgram( FProgramID );
	}

	FProgramID = ProgramID;

	RebindAllUniforms();

	FAlreadyLoaded = true;

	return iShaderProgram::RelinkShaderProgram();

	unguard();
}

void clGLSLShaderProgram::BindDefaultLocations( Luint ProgramID )
{
	// bind default attribs
	LGL3->glBindAttribLocation( ProgramID, L_VS_VERTEX,   "in_Vertex" );      // vec3
	LGL3->glBindAttribLocation( ProgramID, L_VS_TEXCOORD, "in_TexCoord" );    // vec4
	LGL3->glBindAttribLocation( ProgramID, L_VS_NORMAL,   "in_Normal" );      // vec3
	LGL3->glBindAttribLocation( ProgramID, L_VS_TANGENT,  "in_Tangent" );     // vec3
	LGL3->glBindAttribLocation( ProgramID, L_VS_BINORMAL, "in_Binormal" );    // vec3
	LGL3->glBindAttribLocation( ProgramID, L_VS_COLORS,   "in_Color" );       // vec4
	LGL3->glBindAttribLocation( ProgramID, L_VS_BONES,    "in_Bones" );       // vec2
	LGL3->glBindAttribLocation( ProgramID, L_VS_AUX0,     "in_Aux" );         // mat4

	// bind default fragment program output
	for ( size_t i = 0; i != GetNumRenderTargets(); i++ )
	{
		LGL3->glBindFragDataLocation( ProgramID, ( GLuint )i, GetRenderTargetName( i ).c_str() );
	}

	// bind custom outputs
	for ( size_t i = 0; i != FFragDataLocations.size(); i++ )
	{
		LGL3->glBindFragDataLocation( ProgramID, FFragDataLocations[i].FLocation, FFragDataLocations[i].FName.c_str() );
	}

	// bind custom input attribs
	for ( size_t i = 0; i != FFragDataLocations.size(); i++ )
	{
		LGL3->glBindAttribLocation( ProgramID, FAttribsLocations[i].FLocation, FAttribsLocations[i].FName.c_str() );
	}
}

void clGLSLShaderProgram::SetFragDataLocationName( int Location, const LString& Name )
{
	FFragDataLocations.push_back( sUniform( Location, Name ) );

	SetRelinkPending( true );
}

void clGLSLShaderProgram::SetAttribLocationName( int Location, const LString& Name )
{
	FAttribsLocations.push_back( sUniform( Location, Name ) );

	SetRelinkPending( true );
}

void clGLSLShaderProgram::RebindAllUniforms()
{
	guard();

	BindUniforms();

	FUniforms.clear();

	FStreams[ L_VS_VERTEX   ] = true;
	FStreams[ L_VS_TEXCOORD ] = false;
	FStreams[ L_VS_NORMAL   ] = false;
	FStreams[ L_VS_TANGENT  ] = false;
	FStreams[ L_VS_BINORMAL ] = false;
	FStreams[ L_VS_COLORS   ] = false;
	FStreams[ L_VS_BONES    ] = false;
	FStreams[ L_VS_AUX0     ] = false;

	CheckRendererError( "" );

	Lint ActiveUniforms;
	char Buff[256];

	LGL3->glGetProgramiv( FProgramID, GL_ACTIVE_UNIFORMS, &ActiveUniforms );

	CheckRendererError( "" );

	for ( int i = 0; i != ActiveUniforms; ++i )
	{
		Lsizei Length;
		Lint   Size;
		Lenum  Type;

		LGL3->glGetActiveUniform( FProgramID, i, sizeof( Buff ), &Length, &Size, &Type, Buff );

		LString Name( Buff, Length );

		Env->Logger->Log( L_DEBUG, "Active uniform: " + Name + " at location: " + LStr::ToStr( LGL3->glGetUniformLocation( FProgramID, Name.c_str() ) ) );

		// 1. Create new uniform
		sUniform Uniform( Name );

		Uniform.FLocation = LGL3->glGetUniformLocation( FProgramID, Name.c_str() );

		// 2. Cache
		FUniforms.push_back( Uniform );

		if ( Name == "ENGINE_TIME" )
		{
			BindConsoleVariable( "ENGINE_TIME", Env->Console->GetVar( "Engine.EngineTime" ) );

			// restore state
			LGL3->glUseProgram( FProgramID );
			Env->Renderer->SetCurrentBindedSP( this );
		}
		else if ( Name == "ShapeFactor" )
		{
			BindConsoleVariable( "ShapeFactor", Env->Console->GetVar( "Scene.ShapeFactor" ) );

			// restore state
			LGL3->glUseProgram( FProgramID );
			Env->Renderer->SetCurrentBindedSP( this );
		}
		else if ( Name == "LIGHTING_MODEL" )
		{
			BindConsoleVariable( "LIGHTING_MODEL", Env->Console->GetVarDefaultFloat( "Renderer.LightingModel", 0.0f ) );

			// restore state
			LGL3->glUseProgram( FProgramID );
			Env->Renderer->SetCurrentBindedSP( this );
		}
	}

/////////
	CheckRendererError( "" );

	LGL3->glGetProgramiv( FProgramID, GL_ACTIVE_ATTRIBUTES, &FActiveAttribStreams );

	for ( int i = 0; i != FActiveAttribStreams; i++ )
	{
		Lsizei Length;
		Lint   Size;
		Lenum  Type;

		LGL3->glGetActiveAttrib( FProgramID, i, sizeof( Buff ), &Length, &Size, &Type, Buff );

		LString Name( Buff, Length );

		int Location = LGL3->glGetAttribLocation( FProgramID, Name.c_str() );

		if ( Location >= 0 && Location < static_cast<int>( FStreams.size() ) ) FStreams[ Location ] = true;

		Env->Logger->Log( L_DEBUG, "Active attrib: " + Name + " at location: " + LStr::ToStr( Location ) );
	}

	int MaxTextureUnits = Env->Renderer->GetVideoSystemInfo().FMaxTextureUnits;

	// bind samplers
	for ( int i = 0; i != MaxTextureUnits; i++ )
	{
		Lint Texture = LGL3->glGetUniformLocation( FProgramID, ( "Texture" + LStr::ToStr( i ) ).c_str() );

		if ( Texture >= 0 )
		{
			LGL3->glUniform1i( Texture, i );
		}
	}

	// bind uniform buffers
//   FMatricesBlockIndex = glGetUniformBlockIndex( ProgramID, "Matrices" );

	if ( FMatricesBlockIndex >= 0 )
	{
//      if ( FMatricesBufferID ) glDeleteBuffers( 1, &FMatricesBufferID );

//      glGenBuffers( 1, &FMatricesBufferID );

		LGL3->glGetActiveUniformBlockiv( FProgramID, FMatricesBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &FMatricesBlockSize );

		FATAL( FMatricesBlockSize != sizeof( sMatrices ), "FMatricesBlockSize != sizeof( sMatrices ) : " + LStr::ToStr( FMatricesBlockSize ) + " != " + LStr::ToStr( sizeof( sMatrices ) ) );

		Env->Logger->Log( L_DEBUG, "Matrices uniform block at location: " + LStr::ToStr( FMatricesBlockIndex ) + " with size " + LStr::ToStr( FMatricesBlockSize ) );

//      glBindBuffer( GL_UNIFORM_BUFFER, FMatricesBufferID );
//      glBufferData( GL_UNIFORM_BUFFER, FMatricesBlockSize, NULL, GL_DYNAMIC_DRAW );
//      glBindBufferBase( GL_UNIFORM_BUFFER, 0, FMatricesBufferID );
//		glUniformBlockBinding( FProgramID, FMatricesBlockIndex, 0 );
	}

	CheckRendererError( "" );

	unguard();
}

void clGLSLShaderProgram::SetUniformNameInt( const LString& Name, const int Int )
{
	Lint Loc = CreateUniform( Name );

	LGL3->glUniform1i( Loc, Int );
}

void clGLSLShaderProgram::SetUniformNameIntArray( const LString& Name, int Count, const int& Int )
{
	Lint Loc = CreateUniform( Name );

	LGL3->glUniform1iv( Loc, Count, &Int );
}

void clGLSLShaderProgram::SetUniformNameFloat( const LString& Name, const float Float )
{
	Lint Loc = CreateUniform( Name );

	LGL3->glUniform1f( Loc, Float );
}

void clGLSLShaderProgram::SetUniformNameFloatArray( const LString& Name, int Count, const float& Float )
{
	Lint Loc = CreateUniform( Name );

	LGL3->glUniform1fv( Loc, Count, &Float );
}

void clGLSLShaderProgram::SetUniformNameVec3Array( const LString& Name, int Count, const LVector3& Vector )
{
	Lint Loc = CreateUniform( Name );

	LGL3->glUniform3fv( Loc, Count, Vector.ToFloatPtr() );
}

void clGLSLShaderProgram::SetUniformNameVec4Array( const LString& Name, int Count, const LVector4& Vector )
{
	Lint Loc = CreateUniform( Name );

	LGL3->glUniform4fv( Loc, Count, Vector.ToFloatPtr() );
}

void clGLSLShaderProgram::SetUniformNameMat3Array( const LString& Name, int Count, const LMatrix3& Matrix )
{
	Lint Loc = CreateUniform( Name );

	LGL3->glUniformMatrix3fv( Loc, Count, false, Matrix.ToFloatPtr() );
}

void clGLSLShaderProgram::SetUniformNameMat4Array( const LString& Name, int Count, const LMatrix4& Matrix )
{
	Lint Loc = CreateUniform( Name );

	LGL3->glUniformMatrix4fv( Loc, Count, false, Matrix.ToFloatPtr() );
}

void clGLSLShaderProgram::SetUniformInt( Lint Uniform, const int Int )
{
	LGL3->glUniform1i( Uniform, Int );
}

void clGLSLShaderProgram::SetUniformIntArray( Lint Uniform, int Count, const int& Int )
{
	LGL3->glUniform1iv( Uniform, Count, &Int );
}

void clGLSLShaderProgram::SetUniformFloat( Lint Uniform, const float Float )
{
	LGL3->glUniform1f( Uniform, Float );
}

void clGLSLShaderProgram::SetUniformFloatArray( Lint Uniform, int Count, const float& Float )
{
	LGL3->glUniform1fv( Uniform, Count, &Float );
}

void clGLSLShaderProgram::SetUniformVec3Array( Lint Uniform, int Count, const LVector3& Vector )
{
	LGL3->glUniform3fv( Uniform, Count, Vector.ToFloatPtr() );
}

void clGLSLShaderProgram::SetUniformVec4Array( Lint Uniform, int Count, const LVector4& Vector )
{
	LGL3->glUniform4fv( Uniform, Count, Vector.ToFloatPtr() );
}

void clGLSLShaderProgram::SetUniformMat3Array( Lint Uniform, int Count, const LMatrix3& Matrix )
{
	LGL3->glUniformMatrix3fv( Uniform, Count, false, Matrix.ToFloatPtr() );
}

void clGLSLShaderProgram::SetUniformMat4Array( Lint Uniform, int Count, const LMatrix4& Matrix )
{
	LGL3->glUniformMatrix4fv( Uniform, Count, false, Matrix.ToFloatPtr() );
}

Lint clGLSLShaderProgram::CreateUniform( const LString& Name )
{
	// 1. Look in already created uniforms
	for ( size_t i = 0; i != FUniforms.size(); ++i )
	{
		if ( FUniforms[i].FName == Name )
		{
			// simple LRU cache gives about 25% performance boost
			std::swap( FUniforms[0], FUniforms[i] );

			return FUniforms[0].FLocation;
		}
	}

//	Env->Logger->LogP(L_DEBUG, "Uniform %s is not declared in shader %s", Name.c_str(), this->GetFileName().c_str());

	return -1;
}

void clGLSLShaderProgram::SetTransformationUniforms( const sMatrices& Matrices )
{
	Matrices.UpdateMatricesBuffer();

	/*
	   if ( FMatricesBlockIndex >= 0 )
	   {
	      glBindBuffer( GL_UNIFORM_BUFFER, FMatricesBufferID );
	      glBufferSubData( GL_UNIFORM_BUFFER, 0, FMatricesBlockSize, &Matrices );
	      glBindBufferBase( GL_UNIFORM_BUFFER, 0, FMatricesBufferID );
	      glUniformBlockBinding( FProgramID, FMatricesBlockIndex, 0);
	   }
	*/

// TODO: deprecated - remove
	BindUniforms();
	LGL3->glUniformMatrix4fv( FIDin_ProjectionMatrix,   1, false, Matrices.in_ProjectionMatrix.ToFloatPtr() );
	LGL3->glUniformMatrix4fv( FIDin_ViewMatrix,         1, false, Matrices.in_ViewMatrix.ToFloatPtr() );
	LGL3->glUniformMatrix4fv( FIDin_ViewMatrixInverse,  1, false, Matrices.in_ViewMatrixInverse.ToFloatPtr() );
	LGL3->glUniformMatrix4fv( FIDin_ModelMatrix,        1, false, Matrices.in_ModelMatrix.ToFloatPtr() );
#if defined( USE_OPENGL_3 )
	LGL3->glUniformMatrix4fv( FIDin_ModelMatrixInverse, 1, false, Matrices.in_ModelMatrixInverse.ToFloatPtr() );
#endif
	LGL3->glUniformMatrix4fv( FIDin_ModelViewMatrix,    1, false, Matrices.in_ModelViewMatrix.ToFloatPtr() );
#if defined( USE_OPENGL_3 )
	LGL3->glUniformMatrix4fv( FIDin_ModelViewMatrixInverse,    1, false, Matrices.in_ModelViewMatrixInverse.ToFloatPtr() );
#endif
	LGL3->glUniformMatrix4fv( FIDin_ModelViewProjectionMatrix, 1, false, Matrices.in_ModelViewProjectionMatrix.ToFloatPtr() );
	LGL3->glUniformMatrix4fv( FIDin_NormalMatrix,              1, false, Matrices.in_NormalMatrix.ToFloatPtr() );
}

void clGLSLShaderProgram::SetGUIUniforms( const LVector4& GUIPos, float Opacity, float ViewLifeTime )
{
	BindUniforms();

	LGL3->glUniform4fv( FID_GUI_Pos, 1, GUIPos.ToFloatPtr() );
	LGL3->glUniform1f( FID_GUI_Opacity, Opacity );
	LGL3->glUniform1f( FID_GUI_ViewLifeTime, ViewLifeTime );
}

void clGLSLShaderProgram::SetMaterialUniforms( const sMaterialDesc& Desc, bool ReceiveShadow )
{
	BindUniforms();

	LGL3->glUniform4fv( FID_MTL_AMBIENT_COLOR,  1, Desc.FProperties.FAmbientColor.ToFloatPtr() );
	LGL3->glUniform4fv( FID_MTL_DIFFUSE_COLOR,  1, Desc.FProperties.FDiffuseColor.ToFloatPtr() );
	LGL3->glUniform4fv( FID_MTL_SPECULAR_COLOR, 1, Desc.FProperties.FSpecularColor.ToFloatPtr() );
	LGL3->glUniform1f( FID_MTL_SHININESS, Desc.FProperties.FShineStrength );
	LGL3->glUniform1f( FID_MTL_TRANSPARENCY, Desc.FProperties.FTransparency );
	LGL3->glUniform1f( FID_MTL_REFLECTIVITY, Desc.FProperties.FReflectivity );
	LGL3->glUniform1f( FID_MTL_ROUGHNESS, Desc.FProperties.FRoughness );

	float ShValue = 1.0f * int( ReceiveShadow && Desc.FReceiveShadow );
	LGL3->glUniform1f( FID_MTL_RECEIVESHADOW, ShValue );
	LGL3->glUniform1f( FID_MTL_PLANARREFLECTION, 1.0f * int( Desc.FPlanarReflection ) );
	LGL3->glUniform1f( FID_MTL_USEAMBIENTONLY, 1.0f * int( Desc.FUseAmbientOnly ) );
}

void clGLSLShaderProgram::SetSceneUniformsPerShader( bool UseShadowMatrix, const LMatrix4& ShadowMatrix, float EnableClipPlane, bool UseClipPlane, const LVector4& ClipPlane, bool ShowNormals )
{
	BindUniforms();

	if ( UseShadowMatrix ) { LGL3->glUniformMatrix4fv( FID_ShadowMatrix, 1, false, ShadowMatrix.ToFloatPtr() ); }

	LGL3->glUniform1f( FID_EnableClipPlane, EnableClipPlane );

	if ( UseClipPlane )
	{
		LGL3->glUniform4fv( FID_ClipPlane, 1, ClipPlane.ToFloatPtr() );
	}

	LGL3->glUniform1f( FID_ShowNormals, ShowNormals ? 1.0f : 0.0f );
}

void clGLSLShaderProgram::SetLightUniformsPerShader( clScene* Scene )
{
	BindUniforms();

	GLsizei NumLights = static_cast<GLsizei>( Scene->FLightTransforms.size() );

	LGL3->glUniform1f( FID_NumLights, ( GLfloat )NumLights );

	int MaxLights = 8;

	LGL3->glUniform4fv( FID_LightAmbientColor, MaxLights, Scene->FLightAmbientColors[0].ToFloatPtr() );
	LGL3->glUniform4fv( FID_LightDiffuseColor, MaxLights, Scene->FLightDiffuseColors[0].ToFloatPtr() );
	LGL3->glUniform4fv( FID_LightSpecularColor, MaxLights, Scene->FLightSpecularColors[0].ToFloatPtr() );

	LGL3->glUniformMatrix4fv( FID_LightTransform, MaxLights, false, Scene->FLightTransforms[0].ToFloatPtr() );

	LGL3->glUniform1fv( FID_LightEnabledFlag,   MaxLights, &Scene->FLightEnabled[0] );
	LGL3->glUniform1fv( FID_LightType,          MaxLights, &Scene->FLightTypes[0] );
	LGL3->glUniform3fv( FID_LightAttenuation,   MaxLights, Scene->FLightAttenuations[0].ToFloatPtr() );
	LGL3->glUniform3fv( FID_LightSpotParams,    MaxLights, Scene->FLightSpotParams[0].ToFloatPtr() );
	LGL3->glUniform3fv( FID_LightSpotDirection, MaxLights, Scene->FLightSpotDirections[0].ToFloatPtr() );
}

void clGLSLShaderProgram::SetSceneUniformsPerRigid( const sMaterialDesc& Desc, const LVector4& PickingColor, const LVector4& KeyFrame, bool Skinned, bool ReceiveShadow )
{
	BindUniforms();

#if defined( USE_OPENGL_3 )
	LGL3->glUniform4fv( FID_PICKING_COLOR, 1, PickingColor.ToFloatPtr() );
	LGL3->glUniform4fv( FID_KeyFrame,      1, KeyFrame.ToFloatPtr()     );
	LGL3->glUniform1f( FID_Skinned, Skinned ? 1.0f : 0.0f );
#endif

	LGL3->glUniform4fv( FID_MTL_AMBIENT_COLOR,  1, Desc.FProperties.FAmbientColor.ToFloatPtr() );
	LGL3->glUniform4fv( FID_MTL_DIFFUSE_COLOR,  1, Desc.FProperties.FDiffuseColor.ToFloatPtr() );
	LGL3->glUniform4fv( FID_MTL_SPECULAR_COLOR, 1, Desc.FProperties.FSpecularColor.ToFloatPtr() );
	LGL3->glUniform1f( FID_MTL_TRANSPARENCY, Desc.FProperties.FTransparency );
	LGL3->glUniform1f( FID_MTL_REFLECTIVITY, Desc.FProperties.FReflectivity );
	LGL3->glUniform1f( FID_MTL_ROUGHNESS, Desc.FProperties.FRoughness );

#if defined( USE_OPENGL_3 )
	float ShValue = 1.0f * int( ReceiveShadow && Desc.FReceiveShadow );
	LGL3->glUniform1f( FID_MTL_RECEIVESHADOW, ShValue );
	LGL3->glUniform1f( FID_MTL_PLANARREFLECTION, 1.0f * int( Desc.FPlanarReflection ) );
	LGL3->glUniform1f( FID_MTL_USEAMBIENTONLY, 1.0f * int( Desc.FUseAmbientOnly ) );

	// TODO: implement separate inner & outer levels
	LGL3->glUniform1f( FID_MTL_TESSINNERLEVEL, Desc.FTesselation );
	LGL3->glUniform1f( FID_MTL_TESSOUTERLEVEL, Desc.FTesselation );
#endif
}

void clGLSLShaderProgram::BindConsoleVariableType( const LString& Name, clCVar* CVar, LVariantType VarType )
{
	CheckRendererError( "" );

	for ( size_t i = 0; i != FUniforms.size(); ++i )
	{
		if ( FUniforms[i].FName == Name )
		{
			FUniforms[i].FConsoleVariable = CVar;
			FUniforms[i].FType = VarType;

			if ( FUniforms[i].FLocation >= 0 )
			{
				FCVarsBindedUniforms.push_back( FUniforms[i] );
			}
		}
	}

	CheckRendererError( "" );
}

void clGLSLShaderProgram::BindUniforms()
{
	if ( Env->Renderer->GetCurrentBindedSP() == this ) { return; }

	LGL3->glUseProgram( FProgramID );

	FProgramRebinds->AddInt( 1 );

	Env->Renderer->SetCurrentBindedSP( this ) ;
}

void clGLSLShaderProgram::Bind()
{
	if ( IsRelinkPending() ) { RelinkShaderProgram(); }

//	iShaderProgram* Binded = Env->Renderer->GetCurrentBindedSP();

	BindUniforms();

	// update binded console variables
//	if ( Binded == this ) return;

	for ( size_t i = 0; i != FCVarsBindedUniforms.size(); ++i )
	{
		sUniform& U = FCVarsBindedUniforms[i];

		switch ( U.FType )
		{
			case L_VARIANT_FLOAT:
				SetUniformFloat( U.FLocation, U.FConsoleVariable->GetFloat() );
				break;
			case L_VARIANT_VEC3:
				SetUniformVec3Array( U.FLocation, 1, U.FConsoleVariable->GetVector3() );
				break;
			case L_VARIANT_VEC4:
				SetUniformVec4Array( U.FLocation, 1, U.FConsoleVariable->GetVector4() );
				break;
			case L_VARIANT_MAT4:
				SetUniformMat4Array( U.FLocation, 1, U.FConsoleVariable->GetMatrix4() );
				break;
		}
	}
}

void clGLSLShaderProgram::UnBind() const
{
}

/*
 * 11/08/2010
     GL_ARB_get_program_binary used to cache shader programs
 * 10/08/2010
     GetCachingDir()
     CacheTo()
     CacheFrom()
 * 12/03/2010
     Unified shaders
 * 11/03/2010
     SetUniformMat4Array()
     SetUniformNameMat4Array()
 * 08/03/2010
     Safe shader program relinking
 * 01/03/2010
     GLSL #version 150
 * 28/06/2009
     GLSL #version 130
 * 06/09/2007
     Initial support of geometry shaders
 * 02/06/2007
     SetInterpolationCoef()
 * 02/05/2007
     More versatile shaders reloading
 * 04/01/2007
     Workarounds for systems without GLSL support
 * 06/08/2006
     RebindUniforms()
 * 26/06/2005
     Modifications to conform the GL 2.0 spec
     Shader program validation disabled
 * 31/03/2005
     Checking before attaching objects to program
 * 29/03/2005
     Texture samplers binding
 * 25/03/2005
     Initial support code implemented
 * 24/03/2005
     It's here
*/
