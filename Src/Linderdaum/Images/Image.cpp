/**
 * \file Image.cpp
 * \brief Generic image container
 * \version 0.5.93
 * \date 19/11/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */


#include "Image.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/Linker.h"
#include "Core/Console.h"
#include "Renderer/Canvas.h"
#include "Renderer/iTexture.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iShaderProgram.h"
#include "Resources/ResourcesManager.h"

#include "Environment.h"

void clImage::SetBitmap( clBitmap* NewBitmap )
{
	LMutex Lock( &FBitmapMutex );

	FCurrentBitmap = NewBitmap;

	CommitChanges();
}

clImage* clImage::CloneImage() const
{
	clImage* TheCopy = Env->Resources->CreateImage();

	TheCopy->SetBitmap( FCurrentBitmap->MakeCopy() );

	return TheCopy;
}

clImage::clImage()
	: FTexture( NULL ),
	  FDefaultBitmap( NULL ),
	  FCurrentBitmap( NULL )
{
}

void clImage::CommitChanges()
{
	SendAsync( L_EVENT_CHANGED, LEventArgs(), false );
}

void clImage::AfterConstruction()
{
	guard();

	iResource::AfterConstruction();

	static const int Default_W = 16;
	static const int Default_H = 16;
	static const int Default_D = 6; // for cubemaps

	// create the default 1x1x1x32bit empty image
	FDefaultBitmap = clBitmap::CreateBitmap( Env, Default_W, Default_H, Default_D, L_BITMAP_BGRA8, L_TEXTURE_2D );

	// fill the checkerboard pattern
	int Offset = 0;

	for ( int x = 0 ; x < Default_W ; x++ )
	{
		for ( int y = 0 ; y < Default_H ; y++ )
		{
			for ( int z = 0 ; z < Default_D ; z++ )
			{
				Lubyte Color = static_cast<Lubyte>( ( x ^ y ^ z ) & 0xFF ) % 2;

				FDefaultBitmap->FBitmapData[Offset++] = Color * 255;
				FDefaultBitmap->FBitmapData[Offset++] = Color * 255;
				FDefaultBitmap->FBitmapData[Offset++] = Color * 255;
				FDefaultBitmap->FBitmapData[Offset++] = 0xFF;
			}
		}
	}

	// FCurrentBitmap contains only reference to the current bitmap and does not have ownership
	FCurrentBitmap = FDefaultBitmap;

	unguard();
}

clImage::~clImage()
{
	// deallocate the image
	if ( FDefaultBitmap != FCurrentBitmap ) { delete( FCurrentBitmap ); }

	delete( FDefaultBitmap );

	delete( FTexture );
}

bool clImage::LoadFromFile( const LString& FileName )
{
	SetAsyncLoadComplete( false );
	Env->Resources->EnqueueLoading( new clLoaderThread::clLoadOp_Image( this ) );

//	TODO( "refactor LoadTexture*() using this" )

	return false;
}

/*
clVideoImage::clVideoImage() : FVideoDecoder(NULL),
                               FCurrentFrame(0),
                               FFrameTime(0.0f),
                               FFlippedFrame(NULL)
{
}

clVideoImage::MakeStep(float FloatParameter)
{
   if ( FVideoDecoder )
   {
      float VideoTime = FVideoDecoder->GetFrameSeconds();

      FFrameTime += FloatParameter;

      if ( FFrameTime > VideoTime )
      {
         while ( FFrameTime > VideoTime )
         {
            FFrameTime -= VideoTime;
            FCurrentFrame++;
         }

         FCurrentFrame = FCurrentFrame % FVideoDecoder->GetMaxFrames();

         Lubyte* Image = static_cast<Lubyte*>( FVideoDecoder->ExtractFrame( FCurrentFrame ) );

         FImage.FlipBitmapVertical( Image, FFlippedFrame );

         RefreshBitmap( FFlippedFrame );
      }
   }
}

clVideoImage::~clVideoImage()
{
   if ( FFlippedFrame ) free( FFlippedFrame );
   delete( FVideoDecoder );
}

void clVideoImage::LoadVideo(const LString& FileName)
{
   if ( Ext == ".AVI" )
   {
#ifdef OS_WINDOWS
      iVideoDecoder* Video = Env->Linker->Instantiate<iVideoDecoder>( "clVFWVideoDecoder" );

      Video->OpenStream( Stream->GetVirtualFileName() );
#else
      FIXME( "implement" )
#endif

      sBitmapParams BMPRec = sBitmapParams( Video->GetWidth(), Video->GetHeight(), 1, L_BITMAP_BGR8 );

      AttachVideo(BMPRec, Video );
   }
}
*/

LString clImage::GetCachingDir() const
{
	return Env->Console->GetVarValueStr( "Cache.TexturesCachingDir", "Cache/Textures" );
}

iTexture* clImage::GetTexture() const
{
	guard()

	if ( !FTexture )
	{
		LMutex Lock( &FTextureMutex );

		if ( !FTexture )
		{
			FTexture = Env->Renderer->CreateTexture();
			FTexture->SetImage( const_cast<clImage*>( this ) );
		}
	}

	return FTexture;

	unguard();
}

/// Process this image with a given shader program / render state. If Output is NULL, then 'this' is used
/// TODO : cache RT/OurRT creation
bool clImage::ProcessWithShader( clRenderState* State, clImage* Output )
{
	clBitmap* Bmp = GetCurrentBitmap();

	int W = Bmp->FBitmapParams.FWidth;
	int H = Bmp->FBitmapParams.FHeight;

	int OutW, OutH;

	int BitsPerChannel = 8;
	int OutBitsPerChannel = 8;

	LBitmapFormat Fmt = Bmp->FBitmapParams.FBitmapFormat;

	if ( Fmt == L_BITMAP_FLOAT32_R || Fmt == L_BITMAP_FLOAT32_RGB || Fmt == L_BITMAP_FLOAT32_RGBA )
	{
		BitsPerChannel = 32;
	}

	if ( Fmt == L_BITMAP_FLOAT16_RGB || Fmt == L_BITMAP_FLOAT16_RGBA )
	{
		BitsPerChannel = 16;
	}

	if ( !Output )
	{
		OutW = W;
		OutH = H;

		OutBitsPerChannel = BitsPerChannel;
	}
	else
	{
		OutW = Output->GetCurrentBitmap()->FBitmapParams.FWidth;
		OutH = Output->GetCurrentBitmap()->FBitmapParams.FWidth;

		// get out parameters
		LBitmapFormat OutFmt = Output->GetCurrentBitmap()->FBitmapParams.FBitmapFormat;

		if ( OutFmt == L_BITMAP_FLOAT32_R || OutFmt == L_BITMAP_FLOAT32_RGB || OutFmt == L_BITMAP_FLOAT32_RGBA )
		{
			OutBitsPerChannel = 32;
		}

		if ( OutFmt == L_BITMAP_FLOAT16_RGB || OutFmt == L_BITMAP_FLOAT16_RGBA )
		{
			OutBitsPerChannel = 16;
		}
	}

	// 1. create auxillary output render target
	iRenderTarget* OutRT = Env->Renderer->CreateRenderTarget( OutW, OutH, 1, OutBitsPerChannel, false, 1 );

	// 2. bind image State's first texture
	State->SetTexture( 0, GetTexture(), false );

	// 3. run the full-screen-quad processor

	OutRT->Bind( 0 );
	Env->Renderer->GetCanvas()->FullscreenRect( State );
	OutRT->UnBind();

	// 4. extract image bits to output (from OutRT)
	OutRT->GetColorTexture( 0 )->SaveToBitmap( Output ? Output->GetCurrentBitmap() : Bmp );

	// 5. cleanup
	OutRT->DisposeObject();

	return false;
}

bool clImage::ProcessWithShaderProgram( iShaderProgram* SP, clImage* Output )
{
	clRenderState* Sh = Env->Resources->CreateShader();

	Sh->SetShaderProgram( SP );

	bool Res = ProcessWithShader( Sh, Output );

	Sh->SetShaderProgram( NULL );

	Sh->DisposeObject();

	return Res;
}

clBitmap* clImage::GetDefaultBitmap() const
{
	LMutex Lock( &FBitmapMutex );

	return FDefaultBitmap;
}

clBitmap* clImage::GetCurrentBitmap() const
{
	LMutex Lock( &FBitmapMutex );

	return FCurrentBitmap;
}

bool clImage::UnloadResource()
{
	guard();

	iResource::UnloadResource();

	clBitmap* Bitmap;

	{
		LMutex Lock( &FBitmapMutex );

		Bitmap = FCurrentBitmap;

		FCurrentBitmap = FDefaultBitmap;
	}

	{
		LMutex Lock( &FTextureMutex );

		delete( FTexture );

		FTexture = NULL;
	}

	delete( Bitmap );

	Env->Logger->Log( L_DEBUG, "...unloaded image" );

	return true;

	unguard();
}

/*
 * 19/11/2010
     GetTexture()
 * 02/02/2010
     Splitted into Image.cpp and Bitmap.cpp
 * 10/07/2006
     ConvertCrossFormatToCubeMap() works correctly at last
 * 30/06/2006
     Implementing ConvertCrossFormatToCubeMap()
 * 19/06/2006
     ConvertCrossFormatToCubeMap()
     ConvertRecCrossFormatToCubeMap()
 * 13/06/2006
     Fixed bug in GetStorageSize()
 * 03/06/2006
     Floating point textures support
 * 30/12/2005
     Power of two restriction for textures removed
 * 28/03/2005
     Fixed bug in decoding of 8bpp images
 * 11/01/2005
     It's here
*/
