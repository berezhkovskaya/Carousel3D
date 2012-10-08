/**
 * \file GLTexture.cpp
 * \brief OpenGL textures
 * \version 0.5.91
 * \date 03/07/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "GLTexture.h"
#include "GLRenderContext.h"

#include "Engine.h"
#include "Core/Logger.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "Core/VFS/FileSystem.h"

#include "Resources/iVideoDecoder.h"
#include "Renderer/iRenderContext.h"

#include "Generated/LGL/LGL.h"

const Lenum CubeTarget[6] =
{
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

struct sTextureFormatRec
{
	LBitmapFormat EngineFormat; // L_BITMAP_... format for the engine

	LString GLFmtDescr; // "Creating ... " string

	Lenum Fmt_Compressed;
	Lenum Fmt_Uncompressed;

	Lenum GLFormat;
};

#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3

// L_BITMAP_INVALID_FORMAT = -1,
static const sTextureFormatRec GLTextureFormatTable[] =
{
#if defined( OS_ANDROID )
	// not supported on Vivante GC800
	{ L_BITMAP_GRAYSCALE8,   "GRAYSCALE8 texture",            GL_LUMINANCE,       GL_LUMINANCE, GL_LUMINANCE},
	{ L_BITMAP_GRAYSCALE16,  "GRAYSCALE16 texture",           GL_LUMINANCE,       GL_LUMINANCE, GL_LUMINANCE},
	//
	{ L_BITMAP_BGR8,         "BGR8 texture",                  GL_COMPRESSED_RGB,  GL_RGB,       GL_RGB  },
	{ L_BITMAP_BGRA8,        "BGRA8 texture",                 GL_COMPRESSED_RGBA, GL_RGBA,      GL_RGBA },
#else
	{ L_BITMAP_GRAYSCALE8,   "GRAYSCALE8 texture",            GL_COMPRESSED_RED,  GL_RED,     GL_RED  },
	{ L_BITMAP_GRAYSCALE16,  "GRAYSCALE16 texture",           GL_R16,             GL_R16,     GL_RED  },
	{ L_BITMAP_BGR8,         "BGR8 texture",                  GL_COMPRESSED_RGB,  GL_RGB8,    GL_BGR  },
	{ L_BITMAP_BGRA8,        "BGRA8 texture",                 GL_COMPRESSED_RGBA, GL_RGBA8,   GL_BGRA },
#endif
	{ L_BITMAP_FLOAT16_RGBA, "FLOAT16 texture with alpha",    GL_RGBA16F,         GL_RGBA16F, GL_RGBA },
	{ L_BITMAP_FLOAT32_RGBA, "FLOAT32 texture with alpha",    GL_RGBA32F,         GL_RGBA32F, GL_RGBA },
	{ L_BITMAP_FLOAT16_RGB,  "FLOAT16 texture without alpha", GL_RGB16F,          GL_RGB16F,  GL_RGB  },
	{ L_BITMAP_FLOAT32_RGB,  "FLOAT32 texture without alpha", GL_RGB32F,          GL_RGB32F,  GL_RGB  },
	{ L_BITMAP_FLOAT32_R,    "FLOAT32 R",                     GL_R32F,            GL_R32F,    GL_RED  },
	{ L_BITMAP_DXT1,         "DXT1 texture",                  GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT },
	{ L_BITMAP_DXT3,         "DXT3 texture",                  GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT },
	{ L_BITMAP_DXT5,         "DXT5 texture",                  GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT }
};

static const int NumTextureFormats = sizeof( GLTextureFormatTable ) / sizeof( sTextureFormatRec );


bool ChooseInternalFormat( sEnvironment* Env, const sBitmapParams& BMPRec, bool IsCompressed, Lenum* Format, Lenum* InternalFormat )
{
	for ( int i = 0; i != NumTextureFormats; i++ )
	{
		if ( BMPRec.FBitmapFormat == GLTextureFormatTable[i].EngineFormat )
		{
			*InternalFormat = IsCompressed ? GLTextureFormatTable[i].Fmt_Compressed : GLTextureFormatTable[i].Fmt_Uncompressed;
			*Format = GLTextureFormatTable[i].GLFormat;

			Env->Logger->Log( L_NOTICE, LString( "Creating " ) + GLTextureFormatTable[i].GLFmtDescr );

			// everything is OK
			return true;
		}
	}

	// unknown format
	return false;
}

LBitmapFormat ConvertInternalFormatToBitmapFormat( Lenum InternalFormat )
{
	for ( int i = 0; i != NumTextureFormats; i++ )
	{
		if ( GLTextureFormatTable[i].Fmt_Uncompressed == InternalFormat )
		{
			return GLTextureFormatTable[i].EngineFormat;
		}
	}

	return L_BITMAP_INVALID_FORMAT;
}

clGLTexture::clGLTexture(): FTexID( 0 ),
	FPBO( 0 ),
	FAttachedToFBO( false ),
	FTextureTarget( 0 ), //GL_TEXTURE_2D
	FCompressed( false ),
#if defined( USE_OPENGL_3 )
	FMinFilter( GL_LINEAR_MIPMAP_LINEAR ),
#else
	FMinFilter( GL_LINEAR ),
#endif
	FMagFilter( GL_LINEAR ),
	FClamping( 0 )
{
	FInternalFormat = 0;
	FFormat = 0;
}

clGLTexture::~clGLTexture()
{
	Env->DisconnectObject( this );

	if ( FTexID ) { LGL3->glDeleteTextures( 1, &FTexID ); }

	if ( FPBO   ) { LGL3->glDeleteBuffers( 1, &FPBO ); }
}

void clGLTexture::AfterConstruction()
{
	iTexture::AfterConstruction();

	GetAPI( Env );

	Env->Connect( L_EVENT_SURFACE_ATTACHED, BIND( &clGLTexture::Event_SURFACE_ATTACHED ) );
	Env->Connect( L_EVENT_SURFACE_DETACHED, BIND( &clGLTexture::Event_SURFACE_DETACHED ) );

	FTextureRebinds = Env->Console->GetVar( "Stats.TextureRebinds" );
	FTextureRebinds->SetInt( 0 );
}

void clGLTexture::EVENT_HANDLER( Event_SURFACE_DETACHED )
{
	Env->Logger->LogP( L_DEBUG, "clGLTexture::EVENT_HANDLER( Event_SURFACE_DETACHED )" );

	FTexID = 0;
	FPBO   = 0;
}

void clGLTexture::EVENT_HANDLER( Event_SURFACE_ATTACHED )
{
	Env->Logger->LogP( L_DEBUG, "clGLTexture::EVENT_HANDLER( Event_SURFACE_ATTACHED )" );

	if ( FAttachedToFBO ) { return; }

	CommitChanges();
}

Lenum clGLTexture::GetDataFormat( const sBitmapParams& Bitmap ) const
{
	return ( Bitmap.FBitmapFormat == L_BITMAP_GRAYSCALE16 ) ?
	       GL_UNSIGNED_SHORT : ( ( Bitmap.IsFloatFormat() ) ?
	                             GL_FLOAT : GL_UNSIGNED_BYTE );
}

void clGLTexture::UpdateFromBitmap( clBitmap* Bitmap )
{
	guard();

	sBitmapParams Params = GetBitmapFormat();

	FATAL( Params.FBitmapFormat != Bitmap->FBitmapParams.FBitmapFormat, "Should have the same format: " + LStr::ToStr( Params.FBitmapFormat ) + ":" + LStr::ToStr( Bitmap->FBitmapParams.FBitmapFormat ) );
//	FATAL( Params.FWidth  != Bitmap->FBitmapParams.FWidth ||
//	       Params.FHeight != Bitmap->FBitmapParams.FHeight, "Should have the same size" );
	FATAL( FTextureTarget != GL_TEXTURE_2D, "Should be 2D texture" );

	TexImage( FInternalFormat,
	          Bitmap->FBitmapParams.FWidth,
	          Bitmap->FBitmapParams.FHeight,
	          Bitmap->FBitmapParams.FDepth,
	          FFormat,
	          GetDataFormat( Bitmap->FBitmapParams ),
	          Bitmap->FBitmapData );

//	Bind( 0 );

//	LGL3->glTexImage2D( GetTextureTarget(), 0, FInternalFormat, Params.FWidth, Params.FHeight, 0, FFormat, GetDataFormat( Bitmap->FBitmapParams ), Bitmap->FBitmapData );
//	LGL3->glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, Params.FWidth, Params.FHeight, FFormat, GetDataFormat( Bitmap->FBitmapParams ), Bitmap->FBitmapData );

	unguard();
}

void clGLTexture::UpdateSubImageFromBitmap( clBitmap* Bitmap, int X, int Y, int MIPLevel )
{
	guard();

	sBitmapParams Params = GetBitmapFormat();

	FATAL( Params.FBitmapFormat != Bitmap->FBitmapParams.FBitmapFormat, "Should have the same format: " + LStr::ToStr( Params.FBitmapFormat ) + ":" + LStr::ToStr( Bitmap->FBitmapParams.FBitmapFormat ) );
	FATAL( FTextureTarget != GL_TEXTURE_2D, "Should be 2D texture" );

	TexSubImage( MIPLevel, X, Y, 
					 Bitmap->FBitmapParams.FWidth, 
					 Bitmap->FBitmapParams.FHeight, 
					 Bitmap->FBitmapParams.FDepth, 
					 FFormat,
					 GetDataFormat( Bitmap->FBitmapParams ),
					 Bitmap->FBitmapData );

	unguard();
}

#if 0
void clGLTexture::RefreshBitmap( const Lubyte* RawBitmap )
{
	ChooseInternalFormat( FImage, &FFormat, &FInternalFormat );

	Bind( 0 );

	/*   if ( Env->Renderer->GetRendererExtensions()->IsPBOSupported() )
	   {
	      glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, FPBO );
	      glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, FImage.GetStorageSize(), NULL, GL_STREAM_DRAW );
	      void* Mem = glMapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY );

	      memcpy( Mem, RawBitmap, FImage.GetStorageSize() );

	      glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB );
	      glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, FImage.FWidth, FImage.FHeight, Format, GL_UNSIGNED_BYTE, 0 );
	      glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
	   }
	   else
	*/
	{
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, FImage.FWidth, FImage.FHeight, Format, GL_UNSIGNED_BYTE, RawBitmap );
	}
}
#endif

void clGLTexture::SetupTexParameters() const
{
	Bind( 0 );

#if defined( USE_OPENGL_ES_2 )

	if ( GetTextureTarget() == GL_TEXTURE_3D ) { return; }

#endif

//   LGL3->glTexParameterf( GetTextureTarget(), GL_TEXTURE_MAX_ANISOTROPY, 16.0f );
	LGL3->glTexParameteri( GetTextureTarget(), GL_TEXTURE_MIN_FILTER, FMinFilter );
	LGL3->glTexParameteri( GetTextureTarget(), GL_TEXTURE_MAG_FILTER, FMagFilter );

	if ( FClamping )
	{
		LGL3->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, FClamping );
		LGL3->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, FClamping );
#if defined( USE_OPENGL_3 )
		LGL3->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, FClamping );
#endif
	}
}

void clGLTexture::SetClamping( const LClamping Clamping )
{
	switch ( Clamping )
	{
		case L_CLAMP_DONT_CARE:
			FClamping = 0;
			break;
		case L_CLAMP_TO_EDGE:
			FClamping = GL_CLAMP_TO_EDGE;
			break;
		case L_CLAMP_TO_BORDER:
#if defined( USE_OPENGL_3 )
			FClamping = GL_CLAMP_TO_BORDER;
#else
			FClamping = GL_CLAMP_TO_EDGE;
#endif
			break;
		case L_REPEAT:
			FClamping = GL_REPEAT;
			break;
	}
}

void clGLTexture::SetMipMapping( const LMipMapping MinFilter, const LMipMapping MagFilter )
{
	switch ( MinFilter )
	{
#if defined( USE_OPENGL_3 )
		case L_NEAREST:
			FMinFilter = GL_NEAREST;
			break;
		case L_LINEAR:
			FMinFilter = GL_LINEAR;
			break;
		case L_NEAREST_MIPMAP_NEAREST:
			FMinFilter = GL_NEAREST_MIPMAP_NEAREST;
			break;
		case L_LINEAR_MIPMAP_NEAREST:
			FMinFilter = GL_LINEAR_MIPMAP_NEAREST;
			break;
		case L_NEAREST_MIPMAP_LINEAR:
			FMinFilter = GL_NEAREST_MIPMAP_LINEAR;
			break;
		case L_LINEAR_MIPMAP_LINEAR:
			FMinFilter = GL_LINEAR_MIPMAP_LINEAR;
			break;
#else
		case L_NEAREST:
		case L_NEAREST_MIPMAP_NEAREST:
		case L_NEAREST_MIPMAP_LINEAR:
			FMinFilter = GL_NEAREST;
			break;
		case L_LINEAR:
		case L_LINEAR_MIPMAP_NEAREST:
		case L_LINEAR_MIPMAP_LINEAR:
			FMinFilter = GL_LINEAR;
			break;
#endif
	}

	switch ( MagFilter )
	{
#if defined( USE_OPENGL_3 )
		case L_NEAREST:
			FMagFilter = GL_NEAREST;
			break;
		case L_LINEAR:
			FMagFilter = GL_LINEAR;
			break;
		case L_NEAREST_MIPMAP_NEAREST:
			FMagFilter = GL_NEAREST_MIPMAP_NEAREST;
			break;
		case L_LINEAR_MIPMAP_NEAREST:
			FMagFilter = GL_LINEAR_MIPMAP_NEAREST;
			break;
		case L_NEAREST_MIPMAP_LINEAR:
			FMagFilter = GL_NEAREST_MIPMAP_LINEAR;
			break;
		case L_LINEAR_MIPMAP_LINEAR:
			FMagFilter = GL_LINEAR_MIPMAP_LINEAR;
			break;
#else
		case L_NEAREST:
		case L_NEAREST_MIPMAP_NEAREST:
		case L_NEAREST_MIPMAP_LINEAR:
			FMagFilter = GL_NEAREST;
			break;
		case L_LINEAR:
		case L_LINEAR_MIPMAP_NEAREST:
		case L_LINEAR_MIPMAP_LINEAR:
			FMagFilter = GL_LINEAR;
			break;
#endif
	}
}

void clGLTexture::Bind( int TextureUnit ) const
{
#if defined( OS_ANDROID )

	if ( GetTextureTarget() == GL_TEXTURE_3D ) { return; }

#endif

	FLocalRenderer->SetActiveTextureUnit( TextureUnit );
	LGL3->glBindTexture( GetTextureTarget(), FTexID );

	FTextureRebinds->AddInt( 1 );
}

void clGLTexture::UpdateMipmaps()
{
	Bind( 0 );

#if defined( USE_OPENGL_3 )
	// OpenGL ES 2.0 supports mipmaps only for power of two textures
	LGL3->glGenerateMipmap( GetTextureTarget() );
#endif
}

void clGLTexture::AttachToCurrentFB( Lenum Attachment, int ZSlice )
{
	guard();

	switch ( GetTextureTarget() )
	{
		case GL_TEXTURE_2D:
			LGL3->glFramebufferTexture2D( GL_FRAMEBUFFER, Attachment, GetTextureTarget(), FTexID, 0 );
			break;
		case GL_TEXTURE_3D:
#if defined( USE_OPENGL_3 )
			// OpenGL ES 2.0 has no default support for 3D textures
			LGL3->glFramebufferTexture3D( GL_FRAMEBUFFER, Attachment, GetTextureTarget(), FTexID, 0, ZSlice );
#endif
			break;
		default:
			FATAL_MSG( "Unsupported texture buffer type" );
	}

	FAttachedToFBO = true;

	unguard();
}

void clGLTexture::SetFormat( Lenum Target, Lenum InternalFormat, Lenum Format, int Width, int Height, int Depth )
{
	guard();

	FTextureTarget  = Target;

	if ( FTexID )
	{
		LGL3->glDeleteTextures( 1, &FTexID );
	}

	LGL3->glGenTextures( 1, &FTexID );

#if defined( USE_OPENGL_ES_2 )
	int TextureSize = 2048;
#else
	int TextureSize = 4096;
#endif

	if ( Env->Renderer )
	{
		TextureSize = Env->Renderer->GetVideoSystemInfo().FMaxTextureSize;
	}

	FATAL( Width  > TextureSize, "Texture width should not exceed " + LStr::ToStr( TextureSize ) );
	FATAL( Height > TextureSize, "Texture height should not exceed " + LStr::ToStr( TextureSize ) );
	FATAL( Depth  > TextureSize, "Texture height should not exceed " + LStr::ToStr( TextureSize ) );

	CheckRendererError( "" );

	switch ( GetTextureTarget() )
	{
		case GL_TEXTURE_2D:
			LGL3->glBindTexture( GetTextureTarget(), FTexID );
			LGL3->glTexParameterf( GetTextureTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			LGL3->glTexParameterf( GetTextureTarget(), GL_TEXTURE_MAG_FILTER, GL_LINEAR );
#if defined( USE_OPENGL_ES_2 )
			LGL3->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			LGL3->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			LGL3->glTexImage2D( GetTextureTarget(), 0, InternalFormat, Width, Height, 0, Format, ( Format == GL_DEPTH_COMPONENT ) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE, NULL );

			Env->Logger->Log( L_DEBUG, "OGL_ES2 : Setting texture format" );

			CheckRendererError( "" );

#else
			LGL3->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
			LGL3->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
			LGL3->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER );
			LGL3->glTexImage2D( GetTextureTarget(), 0, InternalFormat, Width, Height, 0, Format, GL_FLOAT, NULL );
#endif
			LGL3->glBindTexture( GetTextureTarget(), 0 );
			break;
		case GL_TEXTURE_3D:
#if defined( USE_OPENGL_3 )
			// 3D textures are not supported in OpenGL ES 2.0
			LGL3->glBindTexture( GetTextureTarget(), FTexID );
			LGL3->glTexParameterf( GetTextureTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			LGL3->glTexParameterf( GetTextureTarget(), GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			LGL3->glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
			LGL3->glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
			LGL3->glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER );
			LGL3->glTexImage3D( GetTextureTarget(), 0, InternalFormat, Width, Height, Depth, 0, Format, GL_FLOAT, NULL );
			LGL3->glBindTexture( GetTextureTarget(), 0 );
#endif
			break;
		default:
			FATAL_MSG( "Unsupported texture buffer type" );
	}

	CheckRendererError( "" );

	unguard();
}

/*
void clGLTexture::CacheTo(const LString& FileName)
{
   if ( FVideoDecoder ) return;

   iTexture::CacheTo(FileName);

   iOStream* Stream = Env->FileSystem->CreateFileWriter( FileName );

   Stream->WriteLine( Env->Renderer->GetRendererSignature() );

   SaveTextureInInternalFormat( Stream, FImage );

   delete(Stream);
}

bool clGLTexture::CacheFrom(const LString& FileName)
{
   iTexture::CacheFrom(FileName);

   iIStream* Stream = Env->FileSystem->CreateFileReader( FileName );

   LString RendererSignature = Stream->ReadLine();

   if ( Env->Renderer->GetRendererSignature() != RendererSignature )
   {
      Env->Logger->Log( L_NOTICE, "Regenerating cached texture due to changed OpenGL driver version");

      delete(Stream);

      return false;
   }

   LoadTextureInInternalFormat( Stream );

   delete(Stream);

   return true;

   return false;
}
*/


void clGLTexture::GetTexImageCompressed( Lubyte* Image ) const
{
	CheckRendererError( "" );

	if ( GetTextureTarget() == GL_TEXTURE_CUBE_MAP )
	{
		Lubyte* Data = Image;

		for ( Luint i = 0; i != 6; ++i )
		{
			Lint FaceSize = 0;
			Lint InternalFormat = 0;

			LGL3->glGetTexLevelParameteriv( CubeTarget[i], 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &FaceSize );
			LGL3->glGetTexLevelParameteriv( CubeTarget[i], 0, GL_TEXTURE_INTERNAL_FORMAT,       &InternalFormat );

			*reinterpret_cast<Lint*>( Data ) = FaceSize;
			Data += sizeof( FaceSize );
			*reinterpret_cast<Lint*>( Data ) = InternalFormat;
			Data += sizeof( InternalFormat );

			LGL3->glGetCompressedTexImage( CubeTarget[i], 0, Data );

			Data += FaceSize;
		}
	}
	else
	{
		LGL3->glGetCompressedTexImage( GetTextureTarget(), 0, Image );
	}

	CheckRendererError( "" );
}

void clGLTexture::GetTexImageCompressedSizeAndInternalFormat( Lint* Size, Lenum* InternalFormat ) const
{
	guard();

	CheckRendererError( "" );

	if ( GetTextureTarget() == GL_TEXTURE_CUBE_MAP )
	{
		*Size = 0;
		*InternalFormat = 0;

		for ( Luint i = 0; i != 6; ++i )
		{
			Lint Compressed;
			LGL3->glGetTexLevelParameteriv( CubeTarget[i], 0, GL_TEXTURE_COMPRESSED, &Compressed );

			FATAL( Compressed != 1, "Can save only compressed cube map" );

			Lint FaceSize;
			LGL3->glGetTexLevelParameteriv( CubeTarget[i], 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &FaceSize );
			*Size += FaceSize;
		}

		CheckRendererError( "" );

		// preserve storage for sizes and internal formats
		*Size += 6 * ( sizeof( *InternalFormat ) + sizeof( *Size ) );
	}
	else
	{
		Lint    Compressed;

		LGL3->glGetTexLevelParameteriv( GetTextureTarget(), 0, GL_TEXTURE_COMPRESSED, &Compressed );
		FATAL( Compressed != 1, "Can save only compressed textures" );

		LGL3->glGetTexLevelParameteriv( GetTextureTarget(), 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, Size           );
		LGL3->glGetTexLevelParameteriv( GetTextureTarget(), 0, GL_TEXTURE_INTERNAL_FORMAT,       reinterpret_cast<Lint*>( InternalFormat ) );
	}

	CheckRendererError( "" );

	unguard();
}

/*
void clGLTexture::SaveTextureInInternalFormat(iOStream* Stream, const sImage& Bitmap)
{
   guard( "%s", Stream->GetFileName().c_str() );

   SetupTexParameters();

   Lint    CompressedSize = 0;
   Lenum   InternalFormat = 0;
   Lenum   Format         = 0;
   Lubyte* CompressedImage;

   CheckRendererError("");

   if ( FCompressed )
   {
      GetTexImageCompressedSizeAndInternalFormat(&CompressedSize, &InternalFormat);

      CompressedImage = new Lubyte[CompressedSize];

      GetTexImageCompressed( CompressedImage );

      CheckRendererError("");
   }
   else
   {
      CompressedSize = Bitmap.GetStorageSize();

      ChooseInternalFormat( Bitmap, &Format, &InternalFormat);

      CompressedImage = new Lubyte[CompressedSize];

      GetTexImage( Format, GetDataFormat( Bitmap ), CompressedImage );

      CheckRendererError("");
   }

   glFinish();

   // write headers
   Stream->BlockWrite( &Bitmap,         sizeof(Bitmap)         );
   Stream->BlockWrite( &CompressedSize, sizeof(CompressedSize) );
   Stream->BlockWrite( &InternalFormat, sizeof(InternalFormat) );
   Stream->BlockWrite( &Format,         sizeof(Format)         );

   // write image
   Stream->BlockWrite( CompressedImage, CompressedSize );

   delete[](CompressedImage);

   unguard();
}

void clGLTexture::LoadTextureInInternalFormat(iIStream* Stream)
{
   guard( "%s", Stream->GetFileName().c_str() );

   SetupTexParameters();

   sImage Bitmap;
   Lint    CompressedSize = 0;
   Lenum   InternalFormat = 0;
   Lenum   Format         = 0;

   // read headers
   Stream->BlockRead( &Bitmap,         sizeof(Bitmap)         );
   Stream->BlockRead( &CompressedSize, sizeof(CompressedSize) );
   Stream->BlockRead( &InternalFormat, sizeof(InternalFormat) );
   Stream->BlockRead( &Format,         sizeof(Format)         );

   CheckRendererError("");

   if ( FCompressed )
   {
      TexImageCompressed(InternalFormat,
                         Bitmap.FWidth,
                         Bitmap.FHeight,
                         Bitmap.FDepth,
                         CompressedSize,
                         Stream->MapStreamFromCurrentPos()
                        );
      CheckRendererError("");
   }
   else
   {
      TexImage(InternalFormat,
               Bitmap.FWidth,
               Bitmap.FHeight,
               Bitmap.FDepth,
               Format,
               GetDataFormat( Bitmap ),
               Stream->MapStreamFromCurrentPos()
              );
      CheckRendererError("");
   }

   unguard();
}
*/

void clGLTexture::SetImage( clImage* TheImage )
{
	guard();

	CheckRendererError( "" );

	if ( clImage* OldImage = GetImage() )
	{
		/// Change texture type
		if ( TheImage && TheImage->GetCurrentBitmap()->FBitmapParams.FTextureType != OldImage->GetCurrentBitmap()->FBitmapParams.FTextureType )
		{
			if ( FTexID )
			{
				LGL3->glDeleteTextures( 1, &FTexID );

				FTexID = 0;
			}
		}
	}

	iTexture::SetImage( TheImage );

	unguard();
}

void clGLTexture::SaveToBitmap( clBitmap* Bitmap ) const
{
	sBitmapParams Params = GetBitmapFormat();

	Bitmap->ReallocImageData( &Params );

	if ( !GetImage() )
	{
		// texture is a part of a framebuffer
		Bind( 0 );

		LGL3->glGetTexImage( GetTextureTarget(), 0, GL_RGBA, ( Params.IsFloatFormat() ? GL_FLOAT : GL_UNSIGNED_BYTE ), Bitmap->FBitmapData );
		LGL3->glFinish();

		Bitmap->Convert_BGRAToRGBA();

		return;
	}

	memcpy( Bitmap->FBitmapData,
	        GetImage()->GetCurrentBitmap()->FBitmapData,
	        Bitmap->FBitmapParams.GetStorageSize() );

	Bitmap->FlipVertical();
}

sBitmapParams clGLTexture::GetBitmapFormat() const
{
	if ( !GetImage() )
	{
		// texture is a part of a framebuffer
		Bind( 0 );

		int Width  = 0;
		int Height = 0;

		Lint InternalFormat = 0;

		LGL3->glGetTexLevelParameteriv( GetTextureTarget(), 0, GL_TEXTURE_WIDTH,  &Width  );
		LGL3->glGetTexLevelParameteriv( GetTextureTarget(), 0, GL_TEXTURE_HEIGHT, &Height );
		LGL3->glGetTexLevelParameteriv( GetTextureTarget(), 0, GL_TEXTURE_INTERNAL_FORMAT, &InternalFormat );

		// works only for uncompressed texture formats (but framebuffer is uncompressed)
		LBitmapFormat Format = ConvertInternalFormatToBitmapFormat( InternalFormat );

		if ( Format == L_BITMAP_INVALID_FORMAT )
		{
			Env->Logger->LogP( L_WARNING, "Unknown internal format: %i Falling back to L_BITMAP_BGRA8", InternalFormat );

			Format = L_BITMAP_BGRA8;
		}

		return sBitmapParams( Env, Width, Height, 1, Format, L_TEXTURE_2D );
	}

	return GetImage()->GetCurrentBitmap()->FBitmapParams;
}

void clGLTexture::LoadFromBitmap( clBitmap* Bitmap )
{
	guard();

	CheckRendererError( "" );

	if ( !FTexID )
	{
		LGL3->glGenTextures( 1, &FTexID );

		// Create PBO
		LGL3->glGenBuffers( 1, &FPBO );
	}

	switch ( Bitmap->FBitmapParams.FTextureType )
	{
		case L_TEXTURE_2D:
			Env->Logger->Log( L_DEBUG, "L_TEXTURE_2D" );
			FTextureTarget = GL_TEXTURE_2D;
			break;
		case L_TEXTURE_3D:
			Env->Logger->Log( L_DEBUG, "L_TEXTURE_3D" );
			FTextureTarget = GL_TEXTURE_3D;
			break;
		case L_TEXTURE_CUBE:
			Env->Logger->Log( L_DEBUG, "L_TEXTURE_CUBE_MAP" );
			FTextureTarget = GL_TEXTURE_CUBE_MAP;
			break;
		default:
			FATAL_MSG( "Unknown texture type" );
	}

	// refresh the bitmap
	ChooseInternalFormat( Env, Bitmap->FBitmapParams, FCompressed, &FFormat, &FInternalFormat );

	CheckRendererError( "" );

	int TextureSize = Env->Renderer->GetVideoSystemInfo().FMaxTextureSize;

	FATAL( Bitmap->FBitmapParams.FWidth  > TextureSize, "Texture width should not exceed " + LStr::ToStr( TextureSize ) );
	FATAL( Bitmap->FBitmapParams.FHeight > TextureSize, "Texture height should not exceed " + LStr::ToStr( TextureSize ) );
	FATAL( Bitmap->FBitmapParams.FDepth  > TextureSize, "Texture depth should not exceed " + LStr::ToStr( TextureSize ) );

	CheckRendererError( "" );

	TexImage( FInternalFormat,
	          Bitmap->FBitmapParams.FWidth,
	          Bitmap->FBitmapParams.FHeight,
	          Bitmap->FBitmapParams.FDepth,
	          FFormat,
	          GetDataFormat( Bitmap->FBitmapParams ),
	          Bitmap->FBitmapData );

	unguard();
}

/// If the attached image has changed then update the GL texture
void clGLTexture::CommitChanges()
{
	guard();

	CheckRendererError( "" );

	if ( !GetImage() ) { return; }

	FCompressed = GetImage()->GetCurrentBitmap()->IsCompressed();

	LoadFromBitmap( GetImage()->GetCurrentBitmap() );

	unguard();
}

void clGLTexture::TexSubImage( Luint Level, Lint OffsetX, Lint OffsetY, Luint Width, Luint Height, Luint Depth, Lenum Format, Lenum DataType, const Lubyte* RawPixelData )
{
	guard();

	Bind( 0 );

	SetupTexParameters();

	switch ( GetTextureTarget() )
	{
		case GL_TEXTURE_2D:
			LGL3->glTexSubImage2D( GetTextureTarget(), Level, OffsetX, OffsetY, Width, Height, Format, DataType, RawPixelData );
			break;
	}

	UpdateMipmaps();

	unguard();
}

void clGLTexture::TexImage( Lenum InternalFormat,
                            Luint Width, Luint Height, Luint Depth,
                            Lenum Format,
                            Lenum DataType,
                            const Lubyte* RawPixelData )
{
	guard();

	Bind( 0 );

	SetupTexParameters();

	switch ( GetTextureTarget() )
	{
		case GL_TEXTURE_2D:
			LGL3->glTexImage2D( GetTextureTarget(), 0, InternalFormat, Width, Height, 0, Format, DataType, RawPixelData );
			break;

		case GL_TEXTURE_3D:
			LGL3->glTexImage3D( GetTextureTarget(), 0, InternalFormat, Width, Height, Depth, 0, Format, DataType, RawPixelData );
			break;

		case GL_TEXTURE_CUBE_MAP:
			LGL3->glTexParameteri( GetTextureTarget(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			LGL3->glTexParameteri( GetTextureTarget(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
#if !defined( OS_ANDROID )
			LGL3->glTexParameteri( GetTextureTarget(), GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
#endif

			const Lubyte* Data = RawPixelData;

			Luint Step = 1;

			if ( DataType == GL_FLOAT )
			{
				Step = Width * Height * ( ( Format == GL_BGRA ) ? 16 : 12 );
			}
			else if ( DataType == GL_UNSIGNED_BYTE )
			{
				Step = Width * Height * ( ( Format == GL_BGRA ) ? 4 : 3 );
			}
			else if ( DataType == GL_UNSIGNED_SHORT )
			{
				Step = Width * Height * ( ( Format == GL_BGRA ) ? 8 : 6 );
			}

			for ( Luint i = 0; i != 6; ++i )
			{
				Env->Logger->Log( L_NOTICE, "Uploading cube map target: " + LStr::ToStr( i ) );

				LGL3->glTexImage2D( CubeTarget[i], 0, InternalFormat, Width, Height, 0, Format, DataType, Data );

				Data += Step;
			}

			break;
	}

	UpdateMipmaps();

	unguard();
}

void clGLTexture::TexImageCompressed( Lenum InternalFormat,
                                      Luint Width, Luint Height, Luint Depth,
                                      Luint CompressedSize,
                                      const Lubyte* RawPixelData )
{
	Bind( 0 );

	CheckRendererError( "" );

	switch ( GetTextureTarget() )
	{
		case GL_TEXTURE_2D:
			LGL3->glCompressedTexImage2D( GetTextureTarget(), 0, InternalFormat, Width, Height, 0, CompressedSize, RawPixelData );
			break;

		case GL_TEXTURE_3D:
#if defined( USE_OPENGL_3 )
			LGL3->glCompressedTexImage3D( GetTextureTarget(), 0, InternalFormat, Width, Height, Depth, 0, CompressedSize, RawPixelData );
#endif
			break;

		case GL_TEXTURE_CUBE_MAP:
			const Lubyte* Data = RawPixelData;

			for ( Luint i = 0; i != 6; ++i )
			{
				Env->Logger->Log( L_NOTICE, "Uploading compressed cube map target: " + LStr::ToStr( i ) );

				const Luint Step = *reinterpret_cast<const Luint*>( Data );
				Data += sizeof( Step );
				InternalFormat = *reinterpret_cast<const Lenum*>( Data );
				Data += sizeof( InternalFormat );

				LGL3->glCompressedTexImage2D( CubeTarget[i], 0, InternalFormat, Width, Height, 0, Step, Data );
				CheckRendererError( "" );

				Data += Step;
			}

			break;
	}

	CheckRendererError( "" );
}

/*
 * 03/07/2010
     Fixed L_BITMAP_FLOAT32_R mapping
 * 04/02/2010
     Merged with GLTextures.cpp
 * 02/02/2010
     SaveToBitmap()
     LoadFromBitmap()
 * 31/01/2010
     New imagebinding mechanism, with async updates
 * 11/12/2008
     L_BITMAP_GRAYSCALE16 support
 * 07/02/2007
     Check for maximal texture size
 * 06/02/2007
     Compatibility checks for float internal formats support
 * 07/10/2006
     Floating point textures use uncompressed internal formats
 * 28/07/2006
     1D textures support
 * 10/07/2006
     Some sad notes about ATI Catalyst 6.6 and 3D textures
 * 13/06/2006
     Fixed caching of float textures
 * 03/06/2006
     Floating point textures support
 * 03/03/2006
     Improvements in Bind() to apply filtering settings
 * 02/03/2006
     SetMipMapping()
 * 26/11/2005
     BGRA as internal format
 * 30/03/2005
     Unified textures caching model
 * 25/03/2005
     Driver signature in cached files
 * 01/03/2005
     Decompress()
 * 21/02/2005
     It's here
*/
