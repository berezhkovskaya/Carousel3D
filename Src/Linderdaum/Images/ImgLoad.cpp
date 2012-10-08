/**
 * \file ImgLoad.cpp
 * \brief Windows BMP loader / DirectX DDS loader
 * \version 0.5.99f
 * \date 06/07/2011
 * \author Sergey Kosarevsky, 2005-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "ImgLoad.h"
#include "Bitmap.h"

#include <string.h>

namespace Linderdaum
{

#pragma pack(push, 1)
	struct GCC_PACK( 1 ) sBMPHeader
	{
	   // BITMAPFILEHEADER
	   unsigned short    bfType;
	   unsigned long     bfSize;
	   unsigned short    bfReserved1;
	   unsigned short    bfReserved2;
	   unsigned long     bfOffBits;
	   // BITMAPINFOHEADER
	   unsigned long     biSize;
	   unsigned long     biWidth;
	   unsigned long     biHeight;
	   unsigned short    biPlanes;
	   unsigned short    biBitCount;
	   unsigned long     biCompression;
	   unsigned long     biSizeImage;
	   unsigned long     biXPelsPerMeter;
	   unsigned long     biYPelsPerMeter;
	   unsigned long     biClrUsed;
	   unsigned long     biClrImportant;
	};

	struct GCC_PACK( 1 ) sDDPixelFormat
	{
	   unsigned long dwSize;                         // always 32
	   unsigned long dwFlags;                        // indicator for valid fields
	   unsigned long dwFourCC;                       // "DXT1".."DXT5"
	   unsigned long dwRGBBitCount;                  // 16,24 or 32 bits
	   unsigned long dwRBitMask;                     // mask for red channel
	   unsigned long dwGBitMask;                     // mask for green channel
	   unsigned long dwBBitMask;                     // mask for blue channel
	   unsigned long dwABitMask;                     // mask for alpha channel
	};

	struct GCC_PACK( 1 ) sDDSurfaceDesc
	{
	   unsigned long  dwSize;                               // always 124...
	   unsigned long  dwFlags;                              // texture type
	   unsigned long  dwHeight;                             // height
	   unsigned long  dwWidth;                              // width
	   unsigned long  dwPitchOrLinearSize;                  // bytes per scanline
	   unsigned long  dwDepth;                              // depth for volume textures
	   unsigned long  dwMipMapCount;                        // number of mipmap items
	   unsigned long  dwReserved1[11];                      // ...
	   sDDPixelFormat ddpfPixelFormat;
	   unsigned long  dwCaps1;
	   unsigned long  dwCaps2;
	   unsigned long  dwReserved2[3];
	};

	struct GCC_PACK( 1 ) sDDSHeader
	{
	   unsigned long  FourCC;                              // 'magic' FourCC
	   sDDSurfaceDesc SurfaceFormat;                       // surface format
	};

// The dwCaps2 member of the DDSCAPS2 structure
// can be set to one or more of the following values.
	static const unsigned long    DDSCAPS2_CUBEMAP            = 0x00000200;
	static const unsigned long    DDSCAPS2_CUBEMAP_POSITIVEX  = 0x00000600;
	static const unsigned long    DDSCAPS2_CUBEMAP_NEGATIVEX  = 0x00000a00;
	static const unsigned long    DDSCAPS2_CUBEMAP_POSITIVEY  = 0x00001200;
	static const unsigned long    DDSCAPS2_CUBEMAP_NEGATIVEY  = 0x00002200;
	static const unsigned long    DDSCAPS2_CUBEMAP_POSITIVEZ  = 0x00004200;
	static const unsigned long    DDSCAPS2_CUBEMAP_NEGATIVEZ  = 0x00008200;
	static const unsigned long    DDSCAPS2_VOLUME             = 0x00200000;

#pragma pack(pop)

	const char* LoadBMP( const void* Buffer, size_t BufferSize, clBitmap* Resource )
	{
		const sBMPHeader* Header = ( const sBMPHeader* )Buffer;

		if ( Header->biCompression != 0 ) { return "Compressed bitmaps are not supported"; }

		if ( Header->biBitCount != 24 && Header->biBitCount != 32 ) { return "Only 24-bit and 32-bit bitmaps are supported"; }

		LBitmapFormat BitmapFormat = sBitmapParams::SuggestBitmapFormat( Resource->Env, Header->biBitCount, false );

		sBitmapParams BMPRec =  sBitmapParams( Resource->Env, Header->biWidth, Header->biHeight, 1, BitmapFormat, L_TEXTURE_2D );

		Resource->ReallocImageData( &BMPRec );

		memcpy( Resource->FBitmapData, ( Lubyte* )Buffer + 0x36, BMPRec.GetStorageSize() );

		Resource->FlipVertical();
		Resource->Convert_BGRAToRGBA();

		return NULL;
	}

	void SaveBMP( void* OutBuffer, size_t MaxBufferSize, const void* RawBGRImage, int Width, int Height, int BitsPP )
	{
		sBMPHeader* Header = ( sBMPHeader* )OutBuffer;

		int BytesPP   = BitsPP / 8;
		int ImageSize = Width * Height * BytesPP;

		Header->bfType          = 0x4D * 256 + 0x42;
		Header->bfSize          = ImageSize + sizeof( sBMPHeader );
		Header->bfReserved1     = 0;
		Header->bfReserved2     = 0;
		Header->bfOffBits       = 0x36;
		Header->biSize          = 40;
		Header->biWidth         = Width;
		Header->biHeight        = Height;
		Header->biPlanes        = 1;
		Header->biBitCount      = static_cast<Lushort>( BitsPP );
		Header->biCompression   = 0;
		Header->biSizeImage     = 0;
		Header->biXPelsPerMeter = 6000;
		Header->biYPelsPerMeter = 6000;
		Header->biClrUsed       = 0;
		Header->biClrImportant  = 0;

		memcpy( ( Lubyte* )OutBuffer + 0x36, RawBGRImage, ImageSize );
	}

	const char* LoadDDS( const void* Buffer, size_t BufferSize, clBitmap* OutBitmap )
	{
		const sDDSHeader* FileHeader = ( const sDDSHeader* )Buffer;
		const sDDSurfaceDesc* SurfaceDesc = &FileHeader->SurfaceFormat;

		int BPP = SurfaceDesc->ddpfPixelFormat.dwRGBBitCount;

		LBitmapFormat BitmapFormat = sBitmapParams::SuggestBitmapFormat( OutBitmap->Env, BPP, false );

		if ( BitmapFormat == L_BITMAP_INVALID_FORMAT ) { return "Unsupported DDS bitmap format"; }

		sBitmapParams BMPRec =  sBitmapParams( OutBitmap->Env, SurfaceDesc->dwWidth, SurfaceDesc->dwHeight, SurfaceDesc->dwDepth, BitmapFormat, L_TEXTURE_3D );

		unsigned long Caps = SurfaceDesc->dwCaps2;

		bool Is3DTexture = ( Caps & DDSCAPS2_VOLUME ) > 0;

		bool IsCubeMap =         ( Caps & DDSCAPS2_CUBEMAP_POSITIVEX ) > 0;
		IsCubeMap = IsCubeMap && ( Caps & DDSCAPS2_CUBEMAP_POSITIVEY ) > 0;
		IsCubeMap = IsCubeMap && ( Caps & DDSCAPS2_CUBEMAP_POSITIVEZ ) > 0;
		IsCubeMap = IsCubeMap && ( Caps & DDSCAPS2_CUBEMAP_NEGATIVEX ) > 0;
		IsCubeMap = IsCubeMap && ( Caps & DDSCAPS2_CUBEMAP_NEGATIVEY ) > 0;
		IsCubeMap = IsCubeMap && ( Caps & DDSCAPS2_CUBEMAP_NEGATIVEZ ) > 0;

		if ( IsCubeMap )
		{
			if ( ( BPP != 24 ) && ( BPP != 32 ) ) { return "Only 24-bit and 32-bit cubemaps are supported"; }

			BMPRec.FDepth = 6;
			BMPRec.FTextureType = L_TEXTURE_CUBE;
		}
		else if ( !Is3DTexture )
		{
			return "Not a correct volume texture or cubemap inside DDS file";
		}

		OutBitmap->ReallocImageData( &BMPRec );

		memcpy( OutBitmap->FBitmapData, ( const Lubyte* )Buffer + sizeof( sDDSHeader ), BMPRec.GetStorageSize() );

		return NULL;
	}

} // namespace Linderdaum

/*
 * 06/07/2011
     Stream-agnostic saving/loading to/from byte buffers
 * 08/06/2006
     More debug logging info added
 * 12/01/2005
     Loading completly implemented
 * 11/01/2005
     SaveBitmap()
 * 10/01/2005
     It's here
*/
