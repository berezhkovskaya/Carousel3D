/**
 * \file GLRenderContext.cpp
 * \brief OpenGL rendering context
 * \version 0.5.91
 * \date 23/08/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "GLRenderContext.h"
#include "GLTexture.h"

#include "Utils/Utils.h"
#include "Core/Linker.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "Core/Logger.h"
#include "Core/VFS/FileSystem.h"
#include "Utils/Screen.h"
#include "Utils/Viewport.h"
#include "Renderer/RenderState.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iShaderProgram.h"
#include "Renderer/iVertexArray.h"
#include "Renderer/iGPUBuffer.h"
#include "Renderer/iTexture.h"
#include "Renderer/Canvas.h"
#include "Resources/ResourcesManager.h"
#include "Math/LProjection.h"
#include "LColors.h"
#include "GLDebug.h"
#include "Generated/LGL/LGL.h"
#include "Generated/LGL/LGLTracer.h"

#include <string.h>


clGLRenderContext::clGLRenderContext(): FViewport( NULL ),
	FDebugger( NULL ),
	FCurrentViewportWidth( 0 ),
	FCurrentViewportHeight( 0 ),
	FActiveTextureUnit( -1 ),
	FActiveVBO_ArrayBuffer( -1 ),
	FActiveVBO_ElementBuffer( -1 )
{
	FClassName_RenderTarget  = "clGLFrameBuffer";
	FClassName_ShaderProgram = "clGLSLShaderProgram";
	FClassName_Texture       = "clGLTexture";
	FClassName_VertexArray   = "clGLVertexArray";
	FClassName_Query         = "clGLQuery";
	FClassName_Buffer        = "clGLBuffer";

	FOpenGL = new LGL::clGLExtRetriever;
}

sLGLAPI* clGLRenderContext::GetLGLAPI() const
{
	return LGL3;
}

iTexture* clGLRenderContext::CreateTexture() const
{
	clGLTexture* Texture = Env->Linker->Instantiate( FClassName_Texture );

	Texture->SetLocalRenderer( const_cast<clGLRenderContext*>( this ) );

	return Texture;
}

void clGLRenderContext::SetActiveTextureUnit( int TextureUnit )
{
	if ( FActiveTextureUnit == TextureUnit ) return;

	FActiveTextureUnit = TextureUnit;

	LGL3->glActiveTexture( GL_TEXTURE0 + TextureUnit );
}

void clGLRenderContext::SetActiveVBO( int ArrayBuffer, int ElementBuffer )
{
	if ( FActiveVBO_ArrayBuffer != ArrayBuffer )
	{
		FActiveVBO_ArrayBuffer = ArrayBuffer;

		if ( FActiveVBO_ArrayBuffer ) LGL3->glBindBuffer( GL_ARRAY_BUFFER, FActiveVBO_ArrayBuffer );
	}

	if ( FActiveVBO_ElementBuffer != ElementBuffer )
	{
		FActiveVBO_ElementBuffer = ElementBuffer;

		if ( FActiveVBO_ElementBuffer ) LGL3->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, FActiveVBO_ElementBuffer );
	}
}

void clGLRenderContext::AfterConstruction()
{
	iRenderContext::AfterConstruction();

	Env->Connect( L_EVENT_SURFACE_CHANGED, BIND( &clGLRenderContext::Event_SURFACE_CHANGED ) );
}

void clGLRenderContext::EVENT_HANDLER( Event_SURFACE_CHANGED )
{
	Env->Logger->LogP( L_DEBUG, "clGLRenderContext::EVENT_HANDLER( Event_SURFACE_CHANGED )" );

	RestoreViewport();
}

bool clGLRenderContext::InitContext( clViewport* Viewport,
                                     int BitsPerPixel,
                                     int zBufferBits,
                                     int StencilBits,
                                     int Multisample,
                                     bool TakeoverContext )
{
	guard();

	iRenderContext::InitContext( Viewport, BitsPerPixel, zBufferBits, StencilBits, Multisample, TakeoverContext );

	FViewport = Viewport;

	clViewport*     TempViewport = NULL;
	GL_CONTEXT_TYPE TempContext  = 0;

	// use it if present
	int GLVerMajor_Optional = 4;
	int GLVerMinor_Optional = 1;

	// fallback version
	int GLVerMajor = 3;
	int GLVerMinor = 2;

	if ( !LGL3 ) { LGL3 = new sLGLAPI; }

	FOpenGL->Reload( LGL3, false );

	Env->Logger->Log( L_PARANOID, "OpenGL prepared" );

	if ( !TakeoverContext )
	{
		TempViewport = Env->Screen->OpenViewport( "", 0, "", 0, 0, false, false, false, false );

		Env->Logger->Log( L_DEBUG, "Creating dummy rendering context..." );

		// create a dummy OpenGL context to retrieve GL extensions
		TempContext = FOpenGL->CreateContextFull( LGL3, TempViewport->GetDevice(), BitsPerPixel, zBufferBits, StencilBits, Multisample, GLVerMajor, GLVerMinor, false );

		FATAL( !TempContext, FOpenGL->GetError() );

		CheckError( __FILE__, LStr::ToStr( __LINE__ ), "" );
	}

	if ( FViewport && !TakeoverContext )
	{
		Env->Logger->Log( L_DEBUG, "Creating main rendering context..." );

		bool RequireDebugContext = Env->Console->GetVarValueBool( "Viewport.DebugContext", false );

		// recreate a new context using WGL extensions if possible
		FRenderContext = FOpenGL->CreateContextFull( LGL3, FViewport->GetDevice(), BitsPerPixel, zBufferBits, StencilBits, Multisample, GLVerMajor_Optional, GLVerMinor_Optional, RequireDebugContext );

		if ( !FRenderContext )
		{
			if ( FOpenGL->GetErrorCode() == LGLError_NoGLVersion )
			{
				Env->Logger->LogP( L_DEBUG, "No OpenGL %i.%i present. Trying fallback to OpenGL %i.%i", GLVerMajor_Optional, GLVerMinor_Optional, GLVerMajor, GLVerMinor );

				// try to fallback to old OpenGL 3.x
				FRenderContext = FOpenGL->CreateContext( LGL3, FViewport->GetDevice(), GLVerMajor, GLVerMinor, RequireDebugContext );
			}

			FATAL( !FRenderContext, FOpenGL->GetError() );
		}

		// remove old context
		FOpenGL->DeleteContext( LGL3, TempViewport->GetDevice(), TempContext );

		delete( TempViewport );

		// restore new context
		FOpenGL->MakeCurrent( LGL3, FViewport->GetDevice(), FRenderContext );

		CheckError( __FILE__, LStr::ToStr( __LINE__ ), "" );
	}
	else
	{
		Env->Logger->Log( L_DEBUG, "Getting current context..." );
#if defined( OS_ANDROID ) && !defined( ANDROID9 )
		FRenderContext = NULL;
#else

#if defined(OS_LINUX)
		FRenderContext = glXGetCurrentContext();
#else
		FRenderContext = LGL3->lglGetCurrentContext();
#endif

#endif
	}

	if ( TakeoverContext ) { FOpenGL->Reload( LGL3, true ); }

#if L_ENABLE_OPENGL_TRACING
	LGL::InjectAPITracer( Env, LGL3 );
#endif

	FCurrentVideoInfo = RecheckVideoSystemInfo();

	FDebugger = Env->Linker->Instantiate( "clGLDebugger" );
	FDebugger->SetEnable( true );

	LogExtensions();
	CheckExtensions();

	FCurrentState = Env->Resources->CreateShader();

	CheckError( __FILE__, LStr::ToStr( __LINE__ ), "" );

	LGL3->glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	LGL3->glClearColor( 0, 0, 0, 0 );
	LGL3->glClearDepth( 1 );
	LGL3->glClearStencil( 255 );

	CheckError( __FILE__, LStr::ToStr( __LINE__ ), "" );

	RestoreViewport();

	InitBuffers();

	CheckError( __FILE__, LStr::ToStr( __LINE__ ), "" );

#if defined( OS_WINDOWS )
	LGL3->wglSwapIntervalEXT( 0 );
#endif

	return true;

	unguard();
}

clGLRenderContext::~clGLRenderContext()
{
	Env->DisconnectObject( this );

	delete( FDebugger );

	if ( !FTakeoverContext )
	{
		Env->Logger->Log( L_DEBUG, "Deleting context" );

		FOpenGL->DeleteContext( LGL3, FViewport->GetDevice(), FRenderContext );
	}

	Env->Logger->Log( L_DEBUG, "Unloading GL library" );

	delete( FOpenGL );

	Env->Logger->Log( L_DEBUG, "Done deleting GL context" );

	delete( LGL3 );
}

void clGLRenderContext::SetPolygonFillMode( bool Fill )
{
	LGL3->glPolygonMode( GL_FRONT_AND_BACK, Fill ? GL_FILL : GL_LINE );
}

void clGLRenderContext::Finish()
{
	LGL3->glFinish();
}

void clGLRenderContext::EndFrame( bool SwapBuffer )
{
	FDebugger->ProcessLog();

	iRenderContext::EndFrame( SwapBuffer );
}

void clGLRenderContext::SetViewport( int X, int Y, int Width, int Height )
{
	FCurrentViewportWidth = Width;
	FCurrentViewportHeight = Height;

	LASSERT( Width  > 0 );
	LASSERT( Height > 0 );

	LGL3->glViewport( X, Y, Width, Height );
}

void clGLRenderContext::SetViewportV( const LVector4& ViewportSize )
{
	FCurrentViewportWidth  = static_cast<int>( ViewportSize[2] );
	FCurrentViewportHeight = static_cast<int>( ViewportSize[3] );

	LASSERT( ViewportSize[2] > 0 );
	LASSERT( ViewportSize[3] > 0 );

	LGL3->glViewport( static_cast<int>( ViewportSize[0] ),
	                  static_cast<int>( ViewportSize[1] ),
	                  static_cast<int>( ViewportSize[2] ),
	                  static_cast<int>( ViewportSize[3] ) );
}

void clGLRenderContext::RestoreViewport()
{
	SetViewport( FViewport->GetLeft(),  FViewport->GetTop(),
	             FViewport->GetWidth(), FViewport->GetHeight() );
}

void clGLRenderContext::CheckError( const LString& File, const LString& Line, const LString& Info ) const
{
	static const LString ErrStrings[7] =
	{
		"GL_INVALID_ENUM",
		"GL_INVALID_VALUE",
		"GL_INVALID_OPERATION",
		"GL_STACK_OVERFLOW",
		"GL_STACK_UNDERFLOW",
		"GL_OUT_OF_MEMORY",
		"GL_INVALID_FRAMEBUFFER_OPERATION"
	};

	Luint Err = LGL3->glGetError();

	if ( Err != GL_NO_ERROR )
	{
		LString ES = "OpenGL error: " + LStr::ToStr( ( int )Err );

		if ( ( Err >= 0x0500 ) && ( Err <= 0x0506 ) )
		{
			ES += " (" + ErrStrings[Err-0x0500] + ")";
		}

#if defined( OS_ANDROID )
		Env->Logger->Fatal( ES + "\nInfo: " + Info, File, Line, "" );
#else
		Env->Logger->Error( ES + "\nInfo: " + Info, File, Line, "" );
#endif
	}
}

int clGLRenderContext::GetScreenshotSize() const
{
#if defined( OS_ANDROID )
	// RGBA only
	return FViewport->GetWidth() * FViewport->GetHeight() * 4;
#else
	return FViewport->GetWidth() * FViewport->GetHeight() * 3;
#endif
}

void clGLRenderContext::GetScreenshot( void* Ptr ) const
{
	guard();

	CheckError( __FILE__, LStr::ToStr( __LINE__ ), "" );

	int Width = FViewport->GetWidth();
	int Height = FViewport->GetHeight();
#if defined( OS_ANDROID )
	LGL3->glReadPixels( 0, 0, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, Ptr );
#else
	LGL3->glReadPixels( 0, 0, Width, Height, GL_BGR, GL_UNSIGNED_BYTE, Ptr );
#endif

	CheckError( __FILE__, LStr::ToStr( __LINE__ ), "" );

	LGL3->glFinish();

	unguard();
}

Luint clGLRenderContext::GetPixel( const LVector2& Pnt ) const
{
	Lint X = static_cast<Lint>( Pnt.X        * FCurrentViewportWidth );
	Lint Y = static_cast<Lint>( ( 1.0f - Pnt.Y ) * FCurrentViewportHeight );

	Luint C;

	LGL3->glReadPixels( X, Y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &C );
	LGL3->glFinish();

	return C;
}

void clGLRenderContext::ClearRenderTarget( bool Color, bool Depth, bool Stencil )
{
	Lenum Mask = 0;

	Mask |= Color   ? GL_COLOR_BUFFER_BIT   : 0;
	Mask |= Depth   ? GL_DEPTH_BUFFER_BIT   : 0;
	Mask |= Stencil ? GL_STENCIL_BUFFER_BIT : 0;

	LGL3->glClear( Mask );
}

void clGLRenderContext::SetClearColor4v( const LVector4& Color ) const
{
	LGL3->glClearColor( Color.X, Color.Y, Color.Z, Color.W );
}

void clGLRenderContext::LogExtensions()
{
	sVideoSystemInfo Info = GetVideoSystemInfo();

	Env->Logger->Log( L_LOG, "OpenGL version   : " + Info.FVersion  );
	Env->Logger->Log( L_LOG, "OpenGL vendor    : " + Info.FVendor   );
	Env->Logger->Log( L_LOG, "OpenGL renderer  : " + Info.FRenderer );
	Env->Logger->Log( L_LOG, "GLSL version     : " + Info.FShadingLangVersion );
	Env->Logger->Log( L_NOTICE, "OpenGL & WGL extensions: " );

	LString Ext = LStr::ReplaceAll( Info.FExtensions, ' ', '\n' );
	Env->Logger->Log( L_NOTICE, '\n' + Ext );

	Env->Logger->LogP( L_NOTICE, "OpenGL max texture units: %i", Info.FMaxTextureUnits );
	Env->Logger->LogP( L_NOTICE, "OpenGL max texture size: %i", Info.FMaxTextureSize );
	Env->Logger->LogP( L_NOTICE, "OpenGL max vertex texture units: %i", Info.FMaxVertexTextureUnits );
	Env->Logger->LogP( L_NOTICE, "OpenGL max combined texture units: %i", Info.FMaxCombinedTextureUnits );
	Env->Logger->LogP( L_NOTICE, "OpenGL GL_MAX_FRAGMENT_UNIFORM_VECTORS: %i", Info.FMaxFragmentUniformVectors );

}

bool clGLRenderContext::GuardExtension( bool Optional, const LString& ExtName )
{
	guard( "%s,%s", LStr::ToStr( Optional ).c_str(), ExtName.c_str() );

	bool Supported = LGL::IsExtensionSupported( LGL3, ExtName );

	Env->Logger->Log( L_NOTICE, ExtName + " support: " + LStr::ToStr( Supported ) );

	if ( !Optional )
	{
		FATAL( !Supported, ExtName + " extension needed" );
	}

	return Supported;

	unguard();
}

void clGLRenderContext::CheckExtensions()
{
	guard();

	sVideoSystemInfo Info = GetVideoSystemInfo();

	// create nice error message
	LString ErrorMsg = LString( "\n\n Your system has only: " ) + "\n\n" +
	                   "   OpenGL version: \n" + LStr::Populate( ' ', 20 ) + Info.FVersion  + "\n" +
	                   "   OpenGL vendor:  \n" + LStr::Populate( ' ', 20 ) + Info.FVendor   + "\n" +
	                   "   OpenGL renderer:\n" + LStr::Populate( ' ', 20 ) + Info.FRenderer + "\n" +
	                   "   GLSL version:   \n" + LStr::Populate( ' ', 20 ) + Info.FShadingLangVersion;

	// check OpenGL version
#if defined( USE_OPENGL_ES_2 )
	GuardVersion( 2, 0, "At least OpenGL ES version 2.0 needed" + ErrorMsg );

	GuardExtension( true, "GL_OES_rgb8_rgba8" );
	GuardExtension( true, "GL_OES_depth24" );

	GuardExtension( true, "GL_OES_depth_texture" );
	GuardExtension( true, "GL_OES_texture_3D" );
	GuardExtension( true, "GL_OES_texture_npot" );
	GuardExtension( true, "GL_OES_texture_float_linear" );
#else
	GuardVersion( 3, 1, "At least OpenGL version 3.1 needed" + ErrorMsg );

	GuardExtension( false, "WGL_EXT_swap_control" );

	// in OpenGL 2.1
	GuardExtension( false, "GL_ARB_pixel_buffer_object" );

	// in OpenGL 3.0
	GuardExtension( false, "GL_EXT_gpu_shader4" );
	GuardExtension( false, "GL_ARB_vertex_array_object" );
	GuardExtension( false, "GL_ARB_framebuffer_object" );

	// in OpenGL 3.1
	GuardExtension( false, "GL_EXT_geometry_shader4" );
	GuardExtension( false, "GL_ARB_uniform_buffer_object" );
	GuardExtension( false, "GL_ARB_draw_instanced" );

	// in OpenGL 3.2

	// not yet in core OpenGL
	GuardExtension( false, "GL_EXT_bgra" );
	GuardExtension( false, "GL_EXT_direct_state_access" );
	GuardExtension( true, "GL_ARB_get_program_binary"  );

	if ( GuardExtension( true, "GL_GREMEDY_string_marker" ) )
	{
//      FATAL_MSG( "Unable to start engine under gDEBugger" );
	}

#endif // OS_ANDROID

	unguard();
}

sVideoSystemInfo clGLRenderContext::GetVideoSystemInfo() const
{
	return FCurrentVideoInfo;
}

sVideoSystemInfo clGLRenderContext::RecheckVideoSystemInfo() const
{
	sVideoSystemInfo Info;

	Info.FVersion    = reinterpret_cast<const char*>( LGL3->glGetString( GL_VERSION   ) );
	Info.FVendor     = reinterpret_cast<const char*>( LGL3->glGetString( GL_VENDOR    ) );
	Info.FRenderer   = reinterpret_cast<const char*>( LGL3->glGetString( GL_RENDERER  ) );
	//
	Info.FShadingLangVersion = "not supported";
	Info.FExtensions = LGL::GetExtensionsString( LGL3 );

	LGL3->glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &Info.FMaxTextureUnits );
	LGL3->glGetIntegerv( GL_MAX_TEXTURE_SIZE,        &Info.FMaxTextureSize  );
	LGL3->glGetIntegerv( GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,   &Info.FMaxVertexTextureUnits   );
	LGL3->glGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &Info.FMaxCombinedTextureUnits );
	LGL3->glGetIntegerv( GL_MAX_FRAGMENT_UNIFORM_VECTORS, &Info.FMaxFragmentUniformVectors );

	if ( LGL3->glGetString( GL_SHADING_LANGUAGE_VERSION ) != 0 )
	{
		Info.FShadingLangVersion = reinterpret_cast<const char*>( LGL3->glGetString( GL_SHADING_LANGUAGE_VERSION ) );
	}

	return Info;
}

Lint64 clGLRenderContext::GetTotalVideoMemory() const
{
	const Lenum GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX = 0x9048;
	const Lenum GL_TOTAL_PHYSICAL_MEMORY_ATI            = 0x87FE;

	Lint TotalMem[4];

	if ( LGL3->Has_GL_NVX_gpu_memory_info )
	{
		LGL3->glGetIntegerv( GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &TotalMem[0] );
	}
	else if ( LGL3->Has_GL_ATI_meminfo )
	{
		LGL3->glGetIntegerv( GL_TOTAL_PHYSICAL_MEMORY_ATI, &TotalMem[0] );
	}

	return Lint64( TotalMem[0] ) * 1024;
}

Lint64 clGLRenderContext::GetFreeVideoMemory() const
{
#if defined(OS_LINUX) && !defined(OS_ANDROID)
	// no time to figure out yet
	return 0;
#else

	const Lenum GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX = 0x9049;
	const Lenum GL_TEXTURE_FREE_MEMORY_ATI                = 0x87FC;

	/*
	   http://www.opengl.org/registry/specs/ATI/meminfo.txt

	   FreeMem[0] - total memory free in the pool
	   FreeMem[1] - largest available free block in the pool
	   FreeMem[2] - total auxiliary memory free
	   FreeMem[3] - largest auxiliary free block
	*/
	Lint FreeMem[4];

	if ( LGL3->Has_GL_NVX_gpu_memory_info )
	{
		LGL3->glGetIntegerv( GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &FreeMem[0] );
	}
	else if ( LGL3->Has_GL_ATI_meminfo )
	{
		LGL3->glGetIntegerv( GL_TEXTURE_FREE_MEMORY_ATI, &FreeMem[0] );
	}

	return Lint64( FreeMem[0] ) * 1024;
#endif
}

static const Lenum GLBlendMode[L_INVALID_ENUM+1] =
{
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA_SATURATE,
	GL_CONSTANT_COLOR,
	GL_ONE_MINUS_CONSTANT_COLOR,
	GL_CONSTANT_ALPHA,
	GL_ONE_MINUS_CONSTANT_ALPHA,
	GL_INVALID_ENUM
};

void clGLRenderContext::SetupDepthTest( bool DepthTest )
{
	FStateChanges->AddInt( 1 );
	DepthTest ? LGL3->glEnable( GL_DEPTH_TEST ) : LGL3->glDisable( GL_DEPTH_TEST );
}

void clGLRenderContext::SetupDepthWrites( bool DepthWrites )
{
	FStateChanges->AddInt( 1 );
	DepthWrites ? LGL3->glDepthMask( GL_TRUE ) : LGL3->glDepthMask( GL_FALSE );
}

void clGLRenderContext::SetupColorWrites( bool ColorWrites )
{
	FStateChanges->AddInt( 1 );
	ColorWrites ? LGL3->glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE ) : LGL3->glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
}

void clGLRenderContext::SetupBackfaceCulling( bool BackfaceCulling, bool BackFacing )
{
	FStateChanges->AddInt( 1 );

	if ( BackfaceCulling )
	{
		BackFacing ? LGL3->glCullFace( GL_BACK ) : LGL3->glCullFace( GL_FRONT );
		LGL3->glEnable( GL_CULL_FACE );
	}
	else
	{
		LGL3->glDisable( GL_CULL_FACE );
	}
}

void clGLRenderContext::SetState( clRenderState* State )
{
	int TexturesCount = static_cast<int>( State->GetTexturesCount() );

	// Setup textures
	for ( int Tex = 0; Tex != TexturesCount; Tex++ )
	{
		iTexture* NewTex = State->GetTextureForTextureUnit( Tex );

		if ( NewTex )
		{
			NewTex->Bind( Tex );
		}
	}

	// Setup blending parameters
	FStateChanges->AddInt( 1 );

	if ( State->FBlended )
	{
		LGL3->glBlendFunc( GLBlendMode[ State->FBlendSrc ], GLBlendMode[ State->FBlendDst ] );
		LGL3->glEnablei( GL_BLEND, 0 );
	}
	else
	{
		LGL3->glDisablei( GL_BLEND, 0 );
	}

	SetupBackfaceCulling( State->FBackfaceCulling, State->FBackFacing );
	SetupDepthTest( State->FDepthTest );
	SetupDepthWrites( State->FDepthWrites );
	SetupColorWrites( State->FColorWrites );

	iShaderProgram* SP = State->GetShaderProgram();

	SP->Bind();
}

void clGLRenderContext::UpdateState( clRenderState* State )
{
	int TexturesCount = static_cast<int>( State->GetTexturesCount() );

	// Update textures
	for ( int Tex = 0; Tex != TexturesCount; Tex++ )
	{
		iTexture* CurrentTex = FCurrentState->GetTextureForTextureUnit( Tex );
		iTexture* NewTex     = State->GetTextureForTextureUnit( Tex );

		if ( CurrentTex != NewTex && NewTex )
		{
			NewTex->Bind( Tex );
		}
	}

	// Update blending
	if ( State->FBlended )
	{
		if ( State->FBlendSrc != FCurrentState->FBlendSrc ||
			  State->FBlendDst != FCurrentState->FBlendDst )
		{
			FStateChanges->AddInt( 1 );
			LGL3->glBlendFunc( GLBlendMode[ State->FBlendSrc ], GLBlendMode[ State->FBlendDst ] );
		}

		if ( FCurrentState->FBlended != State->FBlended )
		{
			FStateChanges->AddInt( 1 );
			LGL3->glEnablei( GL_BLEND, 0 );
		}
	}
	else
	{
		// Here: Blending == False
		if ( FCurrentState->FBlended != State->FBlended )
		{
			FStateChanges->AddInt( 1 );
			LGL3->glDisablei( GL_BLEND, 0 );
		}
	}

	// Update backface culling
	if ( ( FCurrentState->FBackfaceCulling != State->FBackfaceCulling ) ||
	     ( FCurrentState->FBackFacing      != State->FBackFacing      ) )
	{
		SetupBackfaceCulling( State->FBackfaceCulling, State->FBackFacing );
	}

	if ( FCurrentState->FDepthTest   != State->FDepthTest   )
	{
		SetupDepthTest( State->FDepthTest );
	}

	if ( FCurrentState->FDepthWrites != State->FDepthWrites )
	{
		SetupDepthWrites( State->FDepthWrites );
	}

	if ( FCurrentState->FColorWrites != State->FColorWrites )
	{
		SetupColorWrites( State->FColorWrites );
	}

	iShaderProgram* SP = State->GetShaderProgram();

	SP->Bind();

	// replace states
	State->CopyTo( FCurrentState );
}

/*
 * 23/08/2010
     SetState() and UpdateState() moved from clRenderState
 * 27/07/2010
     GL_ATI_meminfo bug fixed
 * 19/06/2010
     GetTotalVideoMemory()
     GetFreeVideoMemory()
 * 01/03/2010
     Deprecated GL functionality removed from GetVideoSystemInfo()
 * 28/02/2010
     Old extensions removed
 * 26/05/2009
     Fixed 2 very old bugs in LogExtensions()
 * 28/02/2009
     Usage of external GL context
 * 27/02/2009
     OpenGL 3 context works now
 * 23/10/2008
      Windows Vista 32 compatibility confirmed
 * 22/10/2008
     OpenGL 3 context initialization
 * 05/01/2007
     Outlined / Bitmap fonts selection
 * 13/11/2006
     Multisampling
 * 27/07/2006
     Generation of outlined glyphs for text output
 * 13/07/2006
     SetCurrentColor()
 * 18/01/2006
     BeginProjection()
     EndProjection()
 * 14/12/2005
     Implemented iDrawable
 * 29/11/2005
     Creation of TTF fonts with default charset, enabling Cyrilic fonts
 * 27/11/2005
     CheckError() checks for GL_INVALID_FRAMEBUFFER_OPERATION_EXT
 * 26/11/2005
     SaveScreenshotC() retrieves image in BGR format
 * 21/11/2005
     SetRenderState() refactored
 * 01/11/2005
     Calls DeleteContext() on exception in clGLExtensions
 * 16/05/2005
     FOV reduced downto 80
 * 04/04/2005
     BeginTransform() refactored
 * 26/03/2005
     GetRenderedTrianglesCount()
 * 17/02/2005
     Wireframe mode
 * 07/02/2005
     Screenshots saving
 * 05/02/2005
     Nested masking
 * 01/02/2005
     Fixes for NDC support in ortho projection
 * 24/01/2005
     Font size scaling
 * 08/01/2005
     Rendering state tracking
 * 15/12/2004
     LoadProjectionMatrix()
 * 14/12/2004
     BeginFrame()
     EndFrame()
 * 13/12/2004
     GetFramebuffer()
     CreateRenderTarget()
 * 09/12/2004
     SwapBuffers()
     ChoosePixelFormat()
     CreateContext()
     DeleteContext()
 * 08/12/2004
     It's here
*/
