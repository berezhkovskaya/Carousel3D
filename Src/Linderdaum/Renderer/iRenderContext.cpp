/**
 * \file iRenderContext.cpp
 * \brief Abstract render context representation - implementation of common methods
 * \version 0.5.91
 * \date 30/07/2010
 * \author Sergey Kosarevsky, 2009-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include <stdio.h>

#include "Environment.h"

#include "iRenderContext.h"
#include "iGPUBuffer.h"
#include "iShaderProgram.h"
#include "iQuery.h"
#include "iTexture.h"
#include "Canvas.h"
#include "Core/Linker.h"
#include "Core/Logger.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "Geometry/VAMender.h"
#include "GUI/GUIManager.h"
#include "Utils/Viewport.h"
#include "LColors.h"
#include "Math/LProjection.h"
#include "Scene/Heightmaps/HeightDataProvider.h"
#include "Scene/Heightmaps/HeightMapFacetter.h"
#include "Resources/ResourcesManager.h"

#include "Scene/Postprocess/RenderingTechnique.h"
#include "Scene/Postprocess/iPostprocessor.h"
#include "Scene/Postprocess/Filter.h"

#include "Images/Bitmap.h"
#include "Images/FI_Utils.h"

#include "Generated/LGL/LGL.h"

clVAMender* iRenderContext::CreateVAMender() const
{
	return new clVAMender();
}

clRenderingTechnique* iRenderContext::CreatePostprocessChain() const
{
	return Env->Linker->Instantiate( "clRenderingTechnique" );
}

clFilterPostprocessor* iRenderContext::CreateFilterPostprocessor( const LString& FilterName, const LString& EnableVarName ) const
{
	clFilterPostprocessor* PP = Env->Linker->Instantiate( "clFilterPostprocessor" );
	PP->SetFilterName( FilterName );
	PP->SetEnableVarName( EnableVarName );

	return PP;
}

iPostprocessor* iRenderContext::CreatePostprocessor( const LString& PProcName ) const
{
	return Env->Linker->Instantiate( PProcName );
}

clPtr<iHeightDataProvider> iRenderContext::CreateHeightDataProvider( const int SizeX,
                                                                     const int SizeY,
                                                                     const int Bits,
                                                                     const LString& RawFileName ) const
{
	clPtr<iHeightDataProvider> Provider;

	if ( Bits == 8 )
	{
		Provider = clPtr<iHeightDataProvider>( new clHeightDataProvider<Lubyte>( SizeX, SizeY, RawFileName ) );
	}
	else if ( Bits == 16 )
	{
		Provider = clPtr<iHeightDataProvider>( new clHeightDataProvider<Lushort>( SizeX, SizeY, RawFileName ) );
	}
	else
	{
		FATAL_MSG( "Only 8-bit or 16-bit heightdata could be provided" );
	}

	Provider->Env = Env;
	Provider->AfterConstruction();

	return Provider;
}

clPtr<iHeightMapFacetter> iRenderContext::CreateHeightMapFacetter() const
{
	clPtr<iHeightMapFacetter> Facetter( new clHeightMapFacetter );

	Facetter->Env = Env;
	Facetter->AfterConstruction();

	return Facetter;
}

clCanvas* iRenderContext::GetCanvas() const
{
	if ( !FCanvas )
	{
		FCanvas = Env->Linker->Instantiate( "clCanvas" );
	}

	return FCanvas;
}

clGlyphsCache* iRenderContext::GetGlyphsCache() const
{
	if ( !FGlyphsCache )
	{
		FGlyphsCache = Env->Linker->Instantiate( "clGlyphsCache" );
	}

	return FGlyphsCache;
}

clRenderState* iRenderContext::GetEmptyShader() const
{
	if ( !FEmptyShader )
	{
		LString ESName = "Shaders/empty.shader";

		FEmptyShader = Env->Resources->LoadShader( ESName );
	}

	return FEmptyShader;
};

iVertexArray* iRenderContext::AllocateEmptyVA() const
{
	return Env->Linker->Instantiate( FClassName_VertexArray );
}

iShaderProgram* iRenderContext::CreateShaderProgram() const
{
	guard();

	return Env->Linker->Instantiate( FClassName_ShaderProgram );

	unguard();
}

iTexture* iRenderContext::CreateTexture() const
{
	guard();

	iTexture* Texture = Env->Linker->Instantiate( FClassName_Texture );

	FCreatedTextures.push_back( Texture );

	return Texture;

	unguard();
};

void iRenderContext::RemoveTexture( iTexture* Texture )
{
	for ( size_t i = 0; i != FCreatedTextures.size(); i++ )
	{
		if ( FCreatedTextures[i] == Texture )
		{
			FCreatedTextures[i] = FCreatedTextures.back();
			FCreatedTextures.pop_back();
			return;
		}
	}
}

iRenderTarget* iRenderContext::CreateRenderTarget( const int  DimX,
                                                   const int  DimY,
                                                   const int  DimZ,
                                                   const int  BitsPerChannel,
                                                   const bool DepthBuffer,
                                                   const int  MRTCount ) const
{
	iRenderTarget* RenderTarget = Env->Linker->Instantiate( FClassName_RenderTarget );
	RenderTarget->InitRenderTarget( DimX, DimY, DimZ, BitsPerChannel, DepthBuffer, MRTCount );

	return RenderTarget;

}

iRenderTarget* iRenderContext::CreateRenderTargetV( const LArray<LVector4i>& WidthHeightDepthBitsPerChannel,
                                                    const bool HasDepthBuffer )
{
	iRenderTarget* RenderTarget = Env->Linker->Instantiate( FClassName_RenderTarget );
	RenderTarget->InitRenderTargetV( WidthHeightDepthBitsPerChannel, HasDepthBuffer );

	return RenderTarget;
}

iGPUBuffer* iRenderContext::CreateBuffer() const
{
	return Env->Linker->Instantiate( FClassName_Buffer );
}

iQuery* iRenderContext::CreateQuery() const
{
	return Env->Linker->Instantiate( FClassName_Query );
}

iRenderContext::iRenderContext(): FEmptyShader( NULL ),
	FCanvas( NULL ),
	FGlyphsCache( NULL ),
	FRenderedTriangles( 0 ),
	FTakeoverContext( false ),
	FInsideFrame( false ),
	FCurrentState( NULL ),
	FCurrentBindedSP( NULL )
{
}

void iRenderContext::InitBuffers()
{
	/*
	   if ( FMatricesBuffer ) delete( FMatricesBuffer );

	   FMatricesBuffer = Env->Renderer->CreateBuffer();
	   FMatricesBuffer->CreateBuffer( false, true, false, sizeof( sMatrices ), NULL );
	*/
}

void sMatrices::UpdateMatricesBuffer() const
{
	if ( !FMatricesBufferDirty )
	{
		return;
	}

	in_ViewMatrixInverse         = in_ViewMatrix.GetInversed();
#if defined( USE_OPENGL_3 )
	in_ModelMatrixInverse        = in_ModelMatrix.GetInversed();
#endif
	in_ModelViewMatrix           = in_ModelMatrix * in_ViewMatrix;
#if defined( USE_OPENGL_3 )
	in_ModelViewMatrixInverse    = in_ModelViewMatrix.GetInversed();
#endif
	in_ModelViewProjectionMatrix = in_ModelViewMatrix * in_ProjectionMatrix;
	in_NormalMatrix              = LMatrix4( in_ModelViewMatrixInverse.ToMatrix3().GetTransposed() );

//   FMatricesBuffer->BufferData( false, &FMatrices );

	FMatricesBufferDirty = false;
}

sMatrices::~sMatrices()
{
	if ( FMatricesBuffer ) { delete( FMatricesBuffer ); }
}

bool iRenderContext::InitContext( clViewport* Viewport, int BitsPerPixel, int zBufferBits, int StencilBits, int Multisample, bool TakeoverContext )
{
	Env->ConnectWithPriority( L_EVENT_DRAWOVERLAY, BIND( &iRenderContext::Event_DRAWOVERLAY ), 255 );

	FTakeoverContext = TakeoverContext;

	FWireFrame = Env->Console->GetVar( "WireFrame" );
	FWireFrame->SetBool( false );

	FDips = Env->Console->GetVar( "Stats.DipsPerFrame" );
	FDips->SetInt( 0 );

	FDipsBlended = Env->Console->GetVar( "Stats.DipsBlendedPerFrame" );
	FDipsBlended->SetInt( 0 );

	FStateChanges = Env->Console->GetVar( "Stats.StateChanges" );
	FStateChanges->SetInt( 0 );

	Env->Console->RegisterCommand( "ShowLogo",       BIND( &iRenderContext::ShowLogoC )       );
	Env->Console->RegisterCommand( "ShowSplash",     BIND( &iRenderContext::ShowSplashC )     );
	Env->Console->RegisterCommand( "SetClearColor",  BIND( &iRenderContext::SetClearColorC )  );
	Env->Console->RegisterCommand( "SaveScreenshot", BIND( &iRenderContext::SaveScreenshotC ) );
	Env->Console->RegisterCommand( "RendererInfo",   BIND( &iRenderContext::RendererInfoC )   );
	Env->Console->RegisterCommand( "DumpTextures",   BIND( &iRenderContext::DumpTexturesC )    );

	return true;
}

iRenderContext::~iRenderContext()
{
	Env->DisconnectObject( this );

	if ( Env->Console )
	{
		Env->Console->UnRegisterCommand( "RendererInfo",   BIND( &iRenderContext::RendererInfoC )   );
		Env->Console->UnRegisterCommand( "SaveScreenshot", BIND( &iRenderContext::SaveScreenshotC ) );
		Env->Console->UnRegisterCommand( "SetClearColor",  BIND( &iRenderContext::SetClearColorC )  );
		Env->Console->UnRegisterCommand( "ShowSplash",     BIND( &iRenderContext::ShowSplashC )     );
		Env->Console->UnRegisterCommand( "ShowLogo",       BIND( &iRenderContext::ShowLogoC )       );
		Env->Console->UnRegisterCommand( "DumpTextures",   BIND( &iRenderContext::DumpTexturesC )   );
	}

	delete( FCanvas );
	delete( FGlyphsCache );
}

void iRenderContext::DumpTexturesC( const LString& Param )
{
	clBitmap* Bitmap = Env->Linker->Instantiate( "clBitmap" );

	for ( size_t i = 0; i != FCreatedTextures.size(); i++ )
	{
		Env->Logger->Log( L_NOTICE, "Texture (" + LStr::ToStr( i ) + "): " );

		FCreatedTextures[i]->SaveToBitmap( Bitmap );

		Bitmap->Save2DImage( Env, "Texture" + LStr::ToStr( i ) + ".tif" );
	}
}

LString iRenderContext::GetRendererSignature() const
{
	sVideoSystemInfo Info = GetVideoSystemInfo();

	return ENGINE_NAME + " " + ENGINE_VERSION + " on " + Info.FRenderer + " version " + Info.FVersion + " via Shading Language " + Info.FShadingLangVersion;
}

bool iRenderContext::GuardExtension( bool Optional, const LString& ExtName )
{
	return false;
}

void iRenderContext::GuardVersion( int Major, int Minor, const LString& ErrorMessage )
{
	guard();

// on Android we have OpenGL ES 2.0 statically linked
#if !defined( OS_ANDROID )
	sVideoSystemInfo Info = GetVideoSystemInfo();

	int VerMajor, VerMinor;

	if ( sscanf( Info.FVersion.c_str(), "%d.%d", &VerMajor, &VerMinor ) == 2 )
	{
		if ( VerMajor > Major )
		{
			return;
		}

		if ( VerMajor == Major && VerMinor >= Minor )
		{
			return;
		}
	};

	FATAL_MSG( ErrorMessage );

#endif

	unguard();
}

void iRenderContext::RendererInfoC( const LString& Param )
{
	sVideoSystemInfo Info = GetVideoSystemInfo();

	Env->Console->Display( "OpenGL version   : " + Info.FVersion  );
	Env->Console->Display( "OpenGL vendor    : " + Info.FVendor   );
	Env->Console->Display( "OpenGL renderer  : " + Info.FRenderer );
	Env->Console->Display( "GLSL version     : " + Info.FShadingLangVersion );
}

void iRenderContext::ShowLogoC( const LString& Param )
{
	guard();

	if ( Param.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: ShowLogo <Text>" );
		return;
	}

	// reset the pipeline
	BeginFrame();
	EndFrame( true );

	// draw text
	BeginFrame();

	float TextWidth  = 1.0f;
	float TextHeight = 1.0f;

	if ( Env->GUI )
	{
		TextWidth  = Env->GUI->GetStringWidth( Param, NULL );
		TextHeight = Env->GUI->GetStringHeight();
	}

	// center the text vertical and horizontal
	const float PositionX = 0.5f - 0.5f * TextWidth;
	const float PositionY = 0.5f - TextHeight;

	// show the text
	Env->Renderer->GetCanvas()->TextStr( PositionX, PositionY, Param, LC_White, NULL );

	EndFrame( true );

	unguard();
}

void iRenderContext::ShowSplashC( const LString& Param )
{
	// we can call BeginFrame/EndFrame only from the rendering thread
#ifndef OS_ANDROID
	if ( Param.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: ShowSplash <ShaderName>" );
		return;
	}

	// reset the pipeline
	BeginFrame();
	EndFrame( true );

	// draw text
	BeginFrame();

	clCVar* Lock = Env->Console->GetVar( "Resources.LockedLoading" );

	bool LockedLoading = Lock->GetBool();

	Lock->SetBool( true );

	// show the text
	Env->Renderer->GetCanvas()->FullscreenRect( Env->Resources->LoadShader( Param ) );

	Lock->SetBool( LockedLoading );

	EndFrame( true );
#endif
}

void iRenderContext::SetClearColorC( const LString& Param )
{
	if ( Param.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: SetClearColor <R> <G> <B> <A>" );
		return;
	}

	LVector4 Color;

	Color.X = LStr::ToFloat( LStr::GetToken( Param, 1 ) );
	Color.Y = LStr::ToFloat( LStr::GetToken( Param, 2 ) );
	Color.Z = LStr::ToFloat( LStr::GetToken( Param, 3 ) );
	Color.W = LStr::ToFloat( LStr::GetToken( Param, 4 ) );

	SetClearColor4v( Color );
}

clBitmap* iRenderContext::MakeScreenshot()
{
	guard();

	int Width  = Env->Viewport->GetWidth();
	int Height = Env->Viewport->GetHeight();

#if defined( OS_ANDROID )
	clBitmap* Result = clBitmap::CreateBitmap( Env, Width, Height, 1, L_BITMAP_BGRA8, L_TEXTURE_2D );
#else
	clBitmap* Result = clBitmap::CreateBitmap( Env, Width, Height, 1, L_BITMAP_BGR8, L_TEXTURE_2D );
#endif

//	Lubyte* RawBGRBitmap = new Lubyte[ GetScreenshotSize() ];
	GetScreenshot( Result->FBitmapData /*RawBGRBitmap*/ );
//	delete[] RawBGRBitmap;

	return Result;

	unguard();
}

void iRenderContext::SaveScreenshotC( const LString& Param )
{
	guard();

	LString Ext = ".tif";                    // use default;
	LString ScreenShotDir = Env->Console->GetVarValueStr( "Renderer.ScreenshotsDir", "ScreenShots" );

	LString FileName = Param;

	// parse file name
	if ( Param.empty() )
	{
		static int ScreenShotNumber = 0;
		FileName = "shot";
		LStr::AppendInt( &FileName, ScreenShotNumber, 4 );
		FileName.append( Ext );
		ScreenShotNumber++;
	}

	// create image
	/*
	   int Width  = Env->Viewport->GetWidth();
	   int Height = Env->Viewport->GetHeight();

	   Lubyte* RawBGRBitmap = new Lubyte[ GetScreenshotSize() ];

	   GetScreenshot( RawBGRBitmap );

	   FreeImage_SaveScreenshot( Env, PhysName, RawBGRBitmap, Width, Height );

	   delete( RawBGRBitmap );
	*/
	clBitmap* Bmp = MakeScreenshot();

	// write it
	LStr::AddTrailingChar( &ScreenShotDir, '/' );
	Env->FileSystem->CreateDirs( ScreenShotDir );
	LString PhysName = ScreenShotDir + FileName;

	Bmp->Save2DImage( Env, PhysName );

	Bmp->DisposeObject();

	clCVar* RecMode = Env->Console->GetVar( "RecordScreenshotSequence" );

	if ( !RecMode->GetBool() )
	{
		Env->Console->Display( "Screenshot saved to " + ScreenShotDir + FileName );
	}

	unguard();
}

size_t iRenderContext::GetRenderedTrianglesCount() const
{
	return FRenderedTriangles;
}

void iRenderContext::BeginFrame()
{
	FATAL( FInsideFrame, "BeginFrame() couldn't be called inside frame" );

	FDips->SetInt( 0 );
	FDipsBlended->SetInt( 0 );

	Env->Renderer->GetEmptyShader()->CopyTo( FCurrentState );
	SetState( FCurrentState );

	RestoreViewport();

//   FMatrices.SetProjectionMatrixDefaultOrtho();

	if ( !FTakeoverContext )
	{
		ClearRenderTarget( true, true, true );
	}

	FInsideFrame = true;

	FRenderedTriangles = 0;

	SetPolygonFillMode( !FWireFrame->GetBool() );
}

void iRenderContext::EndFrame( bool SwapBuffer )
{
	guard();

	GetCanvas()->TextStrFreeType_Flush();

	FATAL( !FInsideFrame, "EndFrame() should be called only after BeginFrame()" );

	FInsideFrame = false;

	CheckError( __FILE__, LStr::ToStr( __LINE__ ), "" );

	if ( SwapBuffer && !FTakeoverContext )
	{
		Env->Viewport->SwapBuffers();
	}

	unguard();
}

void iRenderContext::AddBuffer( iVertexArray* VertexArray,
                                clRenderState* Shader,
                                int Instances,
                                bool Wireframe )
{
	// move current matrices into GPU if they've changed
//	Matrices->UpdateMatricesBuffer();

	iShaderProgram* SP = Shader->GetShaderProgram();

//	SP->SetTransformationUniforms( *Matrices );

	UpdateState( Shader );

//   FMatricesBuffer->BindAsUniformBuffer();

	VertexArray->FeedIntoGPUInstanced( Wireframe, Instances, SP );

	FDips->AddInt( 1 );

	if ( Shader->FBlended )
	{
		FDipsBlended->AddInt( 1 );
	}
}

void iRenderContext::Event_DRAWOVERLAY( LEvent Event, const LEventArgs& Args )
{
	std::list<sConsoleMessage>* MessagesHistory = Env->Console->LockConsoleHistory();

	// display current message
	if ( Env->Console->GetTimeVisible() > 0 && !MessagesHistory->empty() )
	{
		GetCanvas()->TextStr( 0.0f, 0.0f, MessagesHistory->back().FMessage, MessagesHistory->back().FColor, NULL );
	};

	Env->Console->UnlockConsoleHistory();
}


/*
 * 30/07/2010
     RendererInfoC()
 * 23/05/2009
     It's here
*/
