/**
 * \file FI_Utils.cpp
 * \brief FreeImage binding code
 * \version 0.5.93
 * \date 18/09/2010
 * \author Sergey Kosarevsky, 2008-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"
#include "Core/VFS/FileSystem.h"
#include "Core/VFS/iIStream.h"
#include "Core/VFS/iOStream.h"
#include "Core/Logger.h"
#include "Renderer/iTexture.h"
#include "Utils/Library.h"

#include <memory.h>

#include "FI_Utils.h"
#include "FI_SaveLoadFlags.h"

// init counter
static int FI_NumInitialized = 0;

#if !defined( OS_ANDROID )
#if defined( OS_32BIT )
const LString FreeImageDLL = "freeimage32.dll";
#endif
#if defined( OS_64BIT )
const LString FreeImageDLL = "freeimage64.dll";
#endif

static clLibrary* Lib = NULL;
#else
// Android uses statically linked FreeImage. LV: But this doesn't mean we use some of its headers directly
//	#include "ExtLibs/FreeImage.h"
extern "C"
{
//DLL_API void DLL_CALLCONV FreeImage_Initialise(BOOL load_local_plugins_only FI_DEFAULT(FALSE));
	DLL_API void DLL_CALLCONV FreeImage_Initialise( bool load_local_plugins_only );
	DLL_API void DLL_CALLCONV FreeImage_DeInitialise( void );

// Allocate / Clone / Unload routines ---------------------------------------

//DLL_API FIBITMAP *DLL_CALLCONV FreeImage_Allocate(int width, int height, int bpp, unsigned red_mask FI_DEFAULT(0), unsigned green_mask FI_DEFAULT(0), unsigned blue_mask FI_DEFAULT(0));
	DLL_API FIBITMAP* DLL_CALLCONV FreeImage_Allocate( int width, int height, int bpp, unsigned red_mask, unsigned green_mask, unsigned blue_mask );
	DLL_API void DLL_CALLCONV FreeImage_Unload( FIBITMAP* dib );

// Load / Save routines -----------------------------------------------------

//DLL_API BOOL DLL_CALLCONV FreeImage_Save(FREE_IMAGE_FORMAT fif, FIBITMAP *dib, const char *filename, int flags FI_DEFAULT(0));
	DLL_API bool DLL_CALLCONV FreeImage_Save( FREE_IMAGE_FORMAT fif, FIBITMAP* dib, const char* filename, int flags );

// Memory I/O stream routines -----------------------------------------------

//DLL_API FIMEMORY *DLL_CALLCONV FreeImage_OpenMemory(BYTE *data FI_DEFAULT(0), DWORD size_in_bytes FI_DEFAULT(0));
	DLL_API FIMEMORY* DLL_CALLCONV FreeImage_OpenMemory( Lubyte data, Luint32 size_in_bytes );
	DLL_API bool      DLL_CALLCONV FreeImage_AcquireMemory(FIMEMORY *stream, Lubyte** data, Luint32* size_in_bytes);
	DLL_API void DLL_CALLCONV FreeImage_CloseMemory( FIMEMORY* stream );

//DLL_API FIBITMAP *DLL_CALLCONV FreeImage_LoadFromMemory(FREE_IMAGE_FORMAT fif, FIMEMORY *stream, int flags FI_DEFAULT(0));
//DLL_API BOOL DLL_CALLCONV FreeImage_SaveToMemory(FREE_IMAGE_FORMAT fif, FIBITMAP *dib, FIMEMORY *stream, int flags FI_DEFAULT(0));
//DLL_API FREE_IMAGE_FORMAT DLL_CALLCONV FreeImage_GetFileTypeFromMemory(FIMEMORY *stream, int size FI_DEFAULT(0));

	DLL_API FIBITMAP* DLL_CALLCONV FreeImage_LoadFromMemory( FREE_IMAGE_FORMAT fif, FIMEMORY* stream, int flags );
	DLL_API bool DLL_CALLCONV FreeImage_SaveToMemory( FREE_IMAGE_FORMAT fif, FIBITMAP* dib, FIMEMORY* stream, int flags );
	DLL_API FREE_IMAGE_FORMAT DLL_CALLCONV FreeImage_GetFileTypeFromMemory( FIMEMORY* stream, int size );

// Pixel access routines ----------------------------------------------------
	DLL_API Lubyte* DLL_CALLCONV FreeImage_GetBits( FIBITMAP* dib );

// DIB info routines --------------------------------------------------------

	DLL_API unsigned DLL_CALLCONV FreeImage_GetBPP( FIBITMAP* dib );
	DLL_API unsigned DLL_CALLCONV FreeImage_GetWidth( FIBITMAP* dib );
	DLL_API unsigned DLL_CALLCONV FreeImage_GetHeight( FIBITMAP* dib );

// Smart conversion routines ------------------------------------------------
	DLL_API FIBITMAP* DLL_CALLCONV FreeImage_ConvertTo24Bits( FIBITMAP* dib );
	DLL_API FIBITMAP* DLL_CALLCONV FreeImage_ConvertTo32Bits( FIBITMAP* dib );
	DLL_API FIBITMAP* DLL_CALLCONV FreeImage_ConvertToRGBF( FIBITMAP* dib );
	DLL_API bool DLL_CALLCONV FreeImage_IsTransparent( FIBITMAP* dib );
	DLL_API bool DLL_CALLCONV FreeImage_FlipHorizontal( FIBITMAP* dib );
	DLL_API bool DLL_CALLCONV FreeImage_FlipVertical( FIBITMAP* dib );
}

#endif // OS_ANDROID

PFNFreeImage_Initialise            FI_Initialise = NULL;
PFNFreeImage_DeInitialise          FI_DeInitialise = NULL;
PFNFreeImage_OpenMemory            FI_OpenMemory = NULL;
PFNFreeImage_CloseMemory           FI_CloseMemory = NULL;
PFNFreeImage_LoadFromMemory        FI_LoadFromMemory = NULL;
PFNFreeImage_AcquireMemory         FI_AcquireMemory = NULL;
PFNFreeImage_GetFileTypeFromMemory FI_GetFileTypeFromMemory = NULL;
PFNFreeImage_GetBPP                FI_GetBPP = NULL;
PFNFreeImage_GetWidth              FI_GetWidth = NULL;
PFNFreeImage_GetHeight             FI_GetHeight = NULL;
PFNFreeImage_ConvertToRGBF         FI_ConvertToRGBF = NULL;
PFNFreeImage_ConvertTo24Bits       FI_ConvertTo24Bits = NULL;
PFNFreeImage_ConvertTo32Bits       FI_ConvertTo32Bits = NULL;
PFNFreeImage_Unload                FI_Unload = NULL;
PFNFreeImage_IsTransparent         FI_IsTransparent = NULL;
PFNFreeImage_FlipVertical          FI_FlipVertical = NULL;
PFNFreeImage_GetBits               FI_GetBits = NULL;
PFNFreeImage_Allocate              FI_Allocate = NULL;
PFNFreeImage_Save                  FI_Save = NULL;
PFNFreeImage_SaveToMemory          FI_SaveToMemory = NULL;

void ::Linderdaum::FreeImage_Unload()
{
#if !defined( OS_ANDROID )

	if ( FI_NumInitialized != 1 ) { if ( FI_NumInitialized > 0 ) { FI_NumInitialized--; } return; }

	FI_NumInitialized--;

	delete( Lib );

	FI_OpenMemory = NULL;
	Lib = NULL;
#endif // OS_ANDROID
}

bool FreeImage_Init( sEnvironment* Env )
{
	if ( FI_NumInitialized > 0 ) { FI_NumInitialized++; return true; }

#if defined( OS_ANDROID )
	FI_Initialise = ( PFNFreeImage_Initialise )&FreeImage_Initialise;
	FI_DeInitialise = ( PFNFreeImage_DeInitialise )&FreeImage_DeInitialise;
	FI_OpenMemory = ( PFNFreeImage_OpenMemory )&FreeImage_OpenMemory;
	FI_CloseMemory = ( PFNFreeImage_CloseMemory )&FreeImage_CloseMemory;
	FI_LoadFromMemory = ( PFNFreeImage_LoadFromMemory )&FreeImage_LoadFromMemory;
	FI_AcquireMemory = (PFNFreeImage_AcquireMemory)&FreeImage_AcquireMemory;
	FI_GetFileTypeFromMemory = ( PFNFreeImage_GetFileTypeFromMemory )&FreeImage_GetFileTypeFromMemory;
	FI_GetBPP = ( PFNFreeImage_GetBPP )&FreeImage_GetBPP;
	FI_GetWidth = ( PFNFreeImage_GetWidth )&FreeImage_GetWidth;
	FI_GetHeight = ( PFNFreeImage_GetHeight )&FreeImage_GetHeight;
	FI_ConvertToRGBF = ( PFNFreeImage_ConvertToRGBF )&FreeImage_ConvertToRGBF;
	FI_ConvertTo24Bits = ( PFNFreeImage_ConvertTo24Bits )&FreeImage_ConvertTo24Bits;
	FI_ConvertTo32Bits = ( PFNFreeImage_ConvertTo32Bits )&FreeImage_ConvertTo32Bits;
	FI_Unload = ( PFNFreeImage_Unload )&FreeImage_Unload;
	FI_IsTransparent = ( PFNFreeImage_IsTransparent )&FreeImage_IsTransparent;
	FI_FlipVertical = ( PFNFreeImage_FlipVertical )&FreeImage_FlipVertical;
	FI_GetBits = ( PFNFreeImage_GetBits )&FreeImage_GetBits;
	FI_Allocate = ( PFNFreeImage_Allocate )&FreeImage_Allocate;
	FI_Save = ( PFNFreeImage_Save )&FreeImage_Save;
	FI_SaveToMemory = ( PFNFreeImage_SaveToMemory )&FreeImage_SaveToMemory;
#else // OS_ANDROID
	Lib = Env->LoadDynamicLibrary( FreeImageDLL );

	FI_Initialise = ( PFNFreeImage_Initialise )Lib->GetProcAddress( "FreeImage_Initialise" );
	FI_DeInitialise = ( PFNFreeImage_DeInitialise )Lib->GetProcAddress( "FreeImage_DeInitialise" );
	FI_OpenMemory = ( PFNFreeImage_OpenMemory )Lib->GetProcAddress( "FreeImage_OpenMemory" );
	FI_CloseMemory = ( PFNFreeImage_CloseMemory )Lib->GetProcAddress( "FreeImage_CloseMemory" );
	FI_LoadFromMemory = ( PFNFreeImage_LoadFromMemory )Lib->GetProcAddress( "FreeImage_LoadFromMemory" );
	FI_AcquireMemory = ( PFNFreeImage_AcquireMemory )Lib->GetProcAddress( "FreeImage_AcquireMemory" );
	FI_GetFileTypeFromMemory = ( PFNFreeImage_GetFileTypeFromMemory )Lib->GetProcAddress( "FreeImage_GetFileTypeFromMemory" );
	FI_GetBPP = ( PFNFreeImage_GetBPP )Lib->GetProcAddress( "FreeImage_GetBPP" );
	FI_GetWidth = ( PFNFreeImage_GetWidth )Lib->GetProcAddress( "FreeImage_GetWidth" );
	FI_GetHeight = ( PFNFreeImage_GetHeight )Lib->GetProcAddress( "FreeImage_GetHeight" );
	FI_ConvertToRGBF = ( PFNFreeImage_ConvertToRGBF )Lib->GetProcAddress( "FreeImage_ConvertToRGBF" );
	FI_ConvertTo24Bits = ( PFNFreeImage_ConvertTo24Bits )Lib->GetProcAddress( "FreeImage_ConvertTo24Bits" );
	FI_ConvertTo32Bits = ( PFNFreeImage_ConvertTo32Bits )Lib->GetProcAddress( "FreeImage_ConvertTo32Bits" );
	FI_Unload = ( PFNFreeImage_Unload )Lib->GetProcAddress( "FreeImage_Unload" );
	FI_IsTransparent = ( PFNFreeImage_IsTransparent )Lib->GetProcAddress( "FreeImage_IsTransparent" );
	FI_FlipVertical = ( PFNFreeImage_FlipVertical )Lib->GetProcAddress( "FreeImage_FlipVertical" );
	FI_GetBits = ( PFNFreeImage_GetBits )Lib->GetProcAddress( "FreeImage_GetBits" );
	FI_Allocate = ( PFNFreeImage_Allocate )Lib->GetProcAddress( "FreeImage_Allocate" );
	FI_Save = ( PFNFreeImage_Save )Lib->GetProcAddress( "FreeImage_Save" );
	FI_SaveToMemory = ( PFNFreeImage_SaveToMemory )Lib->GetProcAddress( "FreeImage_SaveToMemory" );

	if ( !FI_Initialise )
	{
		// some Win32 / C++ name mangling black magic
		FI_Initialise = ( PFNFreeImage_Initialise )Lib->GetProcAddress( "_FreeImage_Initialise@4" );
		FI_DeInitialise = ( PFNFreeImage_DeInitialise )Lib->GetProcAddress( "_FreeImage_DeInitialise@0" );
		FI_OpenMemory = ( PFNFreeImage_OpenMemory )Lib->GetProcAddress( "_FreeImage_OpenMemory@8" );
		FI_CloseMemory = ( PFNFreeImage_CloseMemory )Lib->GetProcAddress( "_FreeImage_CloseMemory@4" );
		FI_LoadFromMemory = ( PFNFreeImage_LoadFromMemory )Lib->GetProcAddress( "_FreeImage_LoadFromMemory@12" );
		FI_AcquireMemory = ( PFNFreeImage_AcquireMemory )Lib->GetProcAddress( "_FreeImage_AcquireMemory@12" );
		FI_GetFileTypeFromMemory = ( PFNFreeImage_GetFileTypeFromMemory )Lib->GetProcAddress( "_FreeImage_GetFileTypeFromMemory@8" );
		FI_GetBPP = ( PFNFreeImage_GetBPP )Lib->GetProcAddress( "_FreeImage_GetBPP@4" );
		FI_GetWidth = ( PFNFreeImage_GetWidth )Lib->GetProcAddress( "_FreeImage_GetWidth@4" );
		FI_GetHeight = ( PFNFreeImage_GetHeight )Lib->GetProcAddress( "_FreeImage_GetHeight@4" );
		FI_ConvertToRGBF = ( PFNFreeImage_ConvertToRGBF )Lib->GetProcAddress( "_FreeImage_ConvertToRGBF@4" );
		FI_ConvertTo24Bits = ( PFNFreeImage_ConvertTo24Bits )Lib->GetProcAddress( "_FreeImage_ConvertTo24Bits@4" );
		FI_ConvertTo32Bits = ( PFNFreeImage_ConvertTo32Bits )Lib->GetProcAddress( "_FreeImage_ConvertTo32Bits@4" );
		FI_Unload = ( PFNFreeImage_Unload )Lib->GetProcAddress( "_FreeImage_Unload@4" );
		FI_IsTransparent = ( PFNFreeImage_IsTransparent )Lib->GetProcAddress( "_FreeImage_IsTransparent@4" );
		FI_FlipVertical = ( PFNFreeImage_FlipVertical )Lib->GetProcAddress( "_FreeImage_FlipVertical@4" );
		FI_GetBits = ( PFNFreeImage_GetBits )Lib->GetProcAddress( "_FreeImage_GetBits@4" );
		FI_Allocate = ( PFNFreeImage_Allocate )Lib->GetProcAddress( "_FreeImage_Allocate@24" );
		FI_Save = ( PFNFreeImage_Save )Lib->GetProcAddress( "_FreeImage_Save@16" );
		FI_SaveToMemory = ( PFNFreeImage_SaveToMemory )Lib->GetProcAddress( "_FreeImage_SaveToMemory@16" );
	}

#endif // OS_ANDROID

	FI_Initialise( false );

	FI_NumInitialized = 1;

	return true;
}

bool ::Linderdaum::FreeImage_LoadFromStream( iIStream* IStream, clBitmap* OutBitmap, bool DoFlipV )
{
	if ( !FI_OpenMemory )
	{
		FreeImage_Init( IStream->Env );
	}

	FIMEMORY* Mem = FI_OpenMemory( const_cast<Lubyte*>( IStream->MapStreamFromCurrentPos() ), static_cast<Luint>( IStream->GetFileSize() ) );

	FREE_IMAGE_FORMAT FIF = FI_GetFileTypeFromMemory( Mem, 0 );

	FIBITMAP* Bitmap = FI_LoadFromMemory( FIF, Mem, 0 );
	FIBITMAP* ConvBitmap;

	FI_CloseMemory( Mem );

	bool FloatFormat = FI_GetBPP( Bitmap ) > 32;

	if ( FloatFormat )
	{
		// process high-dynamic range images
		ConvBitmap = FI_ConvertToRGBF( Bitmap );
	}
	else
	{
		if ( FI_IsTransparent( Bitmap ) )
		{
			ConvBitmap = FI_ConvertTo32Bits( Bitmap );
		}
		else
		{
			ConvBitmap = FI_ConvertTo24Bits( Bitmap );
		}
	}

	FI_Unload( Bitmap );

	Bitmap = ConvBitmap;

	if ( DoFlipV ) { FI_FlipVertical( Bitmap ); }

	LBitmapFormat BitmapFormat = sBitmapParams::SuggestBitmapFormat( OutBitmap->Env, FI_GetBPP( Bitmap ), FloatFormat );

	if ( BitmapFormat == L_BITMAP_INVALID_FORMAT )
	{
		IStream->Env->Logger->LogP( L_WARNING, "L_BITMAP_INVALID_FORMAT: BPP = %i", ( int )FI_GetBPP( Bitmap ) );

		return false;
	}

	int Width  = FI_GetWidth( Bitmap );
	int Height = FI_GetHeight( Bitmap );

	if ( !Math::IsPowerOf2( Width ) || !Math::IsPowerOf2( Height ) )
	{
		IStream->Env->Logger->LogP( L_DEBUG, "Performance notice: non power of 2 image (%s): %i x %i", IStream->GetFileName().c_str(), Width, Height );
	}

	sBitmapParams BMPRec = sBitmapParams( OutBitmap->Env,
	                                      Width,
	                                      Height,
	                                      1,                   // depth
	                                      BitmapFormat,
	                                      L_TEXTURE_2D );

	OutBitmap->ReallocImageData( &BMPRec );

	Lubyte* RawBGRBitmap = FI_GetBits( Bitmap );

	TODO( "avoid redundant data copying in FreeImage_GetBits()" )

	memcpy( OutBitmap->FBitmapData, RawBGRBitmap, BMPRec.GetStorageSize() );

#ifdef OS_ANDROID
	// not all devices have BGRA support
	OutBitmap->Convert_BGRAToRGBA();
#endif

	FI_Unload( Bitmap );

	return true;
}

void ::Linderdaum::FreeImage_SaveScreenshot( sEnvironment* Env, const LString& FileName, Lubyte* RawBGRImage, int Width, int Height )
{
	FreeImage_SaveScreenshot_BPP( Env, FileName, RawBGRImage, Width, Height, 24 );
}

void ::Linderdaum::FreeImage_SaveScreenshot_BPP( sEnvironment* Env, const LString& FileName, Lubyte* RawBGRImage, int Width, int Height, int BitsPP )
{
	if ( !FI_OpenMemory )
	{
		FreeImage_Init( Env );
	}

	LString PhysName = Env->FileSystem->VirtualNameToPhysical( FileName );

	LString Ext;
	Env->FileSystem->SplitPath( FileName, NULL, NULL, NULL, &Ext );
	LStr::ToUpper( &Ext );

	FIBITMAP* Bitmap = FI_Allocate( Width, Height, BitsPP, 0, 0, 0 );

	memcpy( FI_GetBits( Bitmap ), RawBGRImage, Width * Height*( BitsPP / 8 ) );

	int OutSubFormat = TIFF_LZW;
	FREE_IMAGE_FORMAT OutFormat = FIF_TIFF;

	if ( Ext == ".PNG" )
	{
		OutFormat = FIF_PNG;
		OutSubFormat = PNG_DEFAULT;
	}
	else if ( Ext == ".BMP" )
	{
		OutFormat = FIF_BMP;
		OutSubFormat = BMP_DEFAULT;
	}
	else if ( Ext == ".JPG" )
	{
		OutFormat = FIF_JPEG;
		OutSubFormat = JPEG_ACCURATE;
	}
	else if ( Ext == ".EXR" )
	{
		OutFormat = FIF_EXR;
		OutSubFormat = EXR_FLOAT;
	}
/*
	FIMEMORY* Mem = FI_OpenMemory( 0, 0 ); 

	if( !FI_SaveToMemory( OutFormat, Bitmap, Mem, OutSubFormat ) );
	{
		Env->Logger->LogP( L_WARNING, "Could not save 2d image to memory" );
	}

	Lubyte* Data = NULL;
	Luint32 Size = 0;

	FI_AcquireMemory( Mem, &Data, &Size );

	iOStream* Out = Env->FileSystem->CreateFileWriter( FileName );

	Out->BlockWrite( Data, Size );

	delete( Out );

	FI_CloseMemory( Mem );
*/

	if ( !FI_Save( OutFormat, Bitmap, PhysName.c_str(), OutSubFormat ) )
	{
		Env->Logger->LogP( L_WARNING, "Could not save 2d image to %s", PhysName.c_str() );
	}

	FI_Unload( Bitmap );
}

/*
 * 07/03/2011
     Added format detection (BMP, JPG, EXR)
 * 18/09/2010
     freeimage32.dll / freeimage64.dll instead of a single freeimage.dll
 * 02/04/2008
     Log section added
*/
