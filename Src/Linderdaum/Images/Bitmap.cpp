/**
 * \file Bitmap.cpp
 * \brief Bitmap image
 * \version 0.6.08
 * \date 02/02/2012
 * \author Sergey Kosarevsky, 2010-2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "Bitmap.h"
#include "Core/VFS/FileSystem.h"
#include "Core/VFS/iIStream.h"
#include "Core/VFS/iOStream.h"
#include "Core/Logger.h"
#include "Core/Linker.h"

#include "Math/LMath.h"
#include "Math/LRandom.h"
#include "Math/LMathStrings.h"
#include "Math/LHistogram.h"
#include "Math/LFFT.h"

#if L_USE_FREEIMAGE
#  include "FI_Utils.h"
#endif

#include "ImgLoad.h"
#include "RAW.h"

// helper structure to describe image formats and avoid the switch() constructions
struct sBitmapFormatDescription
{
	int   Index;
	bool  IsFloat;
	bool  IsCompressed;

	int   BitsPerPixel;
	int   BytesPerPixel;
};

sBitmapFormatDescription FormatDescriptions[] =
{
//     Index               Float  Compr   Bits  Bytes
	{ L_BITMAP_GRAYSCALE8,   false, false,     8,    1  },
	{ L_BITMAP_GRAYSCALE16,  false, false,    16,    2  },
	{ L_BITMAP_BGR8,         false, false,    24,    3  },
	{ L_BITMAP_BGRA8,        false, false,    32,    4  },
	{ L_BITMAP_FLOAT16_RGBA,  true, false,    64,    8  },
	{ L_BITMAP_FLOAT32_RGBA,  true, false,   128,   16  },
	{ L_BITMAP_FLOAT16_RGB,   true, false,    48,    6  },
	{ L_BITMAP_FLOAT32_RGB,   true, false,    96,   12  },
	{ L_BITMAP_FLOAT32_R,     true, false,    32,    4  },
	{ L_BITMAP_DXT1,         false,  true,     4,    0  },
	{ L_BITMAP_DXT3,         false,  true,     8,    1  },
	{ L_BITMAP_DXT5,         false,  true,     8,    1  }
};

LBitmapFormat sBitmapParams::SuggestBitmapFormat( sEnvironment* Env, int BitsPerPixel, bool IsFloat )
{
	// do not suggest compressed formats
	for ( int i = 0 ; i <= L_BITMAP_FLOAT32_R; i++ )
	{
		const sBitmapFormatDescription& Fmt = FormatDescriptions[i];

		if ( Fmt.IsFloat == IsFloat && Fmt.BitsPerPixel == BitsPerPixel )
		{
			return ( LBitmapFormat )( i );
		}
	}

	TODO( "store some error in logger" )

	if ( IsFloat )
	{
		FATAL_MSG( "Support is only for 48, 64, 96 or 128 bits per pixel float bitmaps" );
	}
	else
	{
		FATAL_MSG( "Support is only for 8, 16 (grayscale), 24 or 32 bits per pixel bitmaps" );
	}

	return L_BITMAP_INVALID_FORMAT;
}

int sBitmapParams::GetStorageSize() const
{
	return ( FWidth * FHeight * FDepth * GetBitsPerPixel() ) / 8;
}

#define CHECK_IMG_FORMAT(Msg, RetCode) \
   int FmtIndex = static_cast<int>(FBitmapFormat); \
   if (FmtIndex < 0 || FmtIndex > L_BITMAP_DXT5) { FATAL_MSG(Msg); return RetCode; };

int sBitmapParams::GetBitsPerPixel() const
{
	CHECK_IMG_FORMAT( "Unknown bitmap format in sBitmapParams::GetBitsPerPixel()", -1 )

	return FormatDescriptions[FmtIndex].BitsPerPixel;
}

int sBitmapParams::GetBytesPerPixel() const
{
	CHECK_IMG_FORMAT( "Unknown bitmap format in sBitmapParams::GetBytesPerPixel()", -1 )

	return FormatDescriptions[FmtIndex].BytesPerPixel;
}

bool sBitmapParams::IsFloatFormat() const
{
	CHECK_IMG_FORMAT( "Unknown bitmap format in sBitmapParams::IsFloatFormat()", false )

	return FormatDescriptions[FmtIndex].IsFloat;
}

bool sBitmapParams::IsCompressedFormat() const
{
	CHECK_IMG_FORMAT( "Unknown bitmap format in sBitmapParams::IsCompressedFormat()", false )

	return FormatDescriptions[FmtIndex].IsCompressed;
}

#undef CHECK_IMG_FORMAT

sBitmapParams::sBitmapParams( sEnvironment* E,
                              const int Width,
                              const int Height,
                              const int Depth,
                              const LBitmapFormat BitmapFormat,
                              const LTextureType  TextureType ): Env( E )
{
	FWidth = Width;
	FHeight = Height;
	FDepth = Depth;
	FBitmapFormat = BitmapFormat;
	FTextureType = TextureType;
}

FIXME( "possibly buggy for float images" )
void clBitmap::FlipBitmapVertical( const Lubyte* Src, Lubyte* Dst ) const
{
	// NOTE: doesn't work with DXT-compressed images

	int Channels = FBitmapParams.GetBitsPerPixel() / 8;

	int A = FBitmapParams.FWidth *   FBitmapParams.FHeight * Channels;
	int B = FBitmapParams.FWidth * ( FBitmapParams.FHeight - 1 ) * Channels;
	int C = FBitmapParams.FWidth * Channels;

	int Pos = 0;
	int PixelIndex = 0;

	for ( int k = 0; k != FBitmapParams.FDepth; ++k )
	{
		PixelIndex = A * k + B;

		for ( int i = 0; i != FBitmapParams.FHeight; ++i )
		{
			memcpy( &Dst[PixelIndex], &Src[Pos], C );

			Pos += C;
			PixelIndex -= C;
		}
	}
}

/// Create exact copy of the bitmap
clBitmap* clBitmap::MakeCopy() const
{
	clBitmap* Res = clBitmap::CreateEmptyBitmap( Env );
	Res->ReallocImageData( &FBitmapParams );

	memcpy( Res->FBitmapData, FBitmapData, Res->FBitmapParams.GetStorageSize() );

	return Res;
}

void clBitmap::GenerateRandomVoronoiDiagram( int NumPoints )
{
	LArray<LVector2> Pts;
	LArray<LVector4> Colors;

	for ( int i = 0 ; i < NumPoints ; i++ )
	{
		LVector3 Pt = Math::RandomVector3InRange( vec3( 0, 0, 0 ), vec3( 1, 1, 1 ) );
		LVector4 Col = Math::RandomVector4InRange( vec4( 0.0f, 0.0f, 0.0f, 0.5f ), vec4( 1, 1, 1, 1 ) );

		Pts.push_back( Pt.ToVector2() );
		Colors.push_back( Col );
	}

	GenerateVoronoiDiagram( NumPoints, Pts.GetPtr( 0 ), Colors.GetPtr( 0 ) );
}

void clBitmap::GenerateVoronoiDiagram( int NumPoints, const LVector2* Points, const LVector4* Colors )
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;

	/// O(W * H * n) brute-force algorithm
	for ( int j = 0 ; j != H ; j++ )
	{
		for ( int i = 0 ; i != W ; i++ )
		{
			// find minimum distance
			int min_idx = 0;
			float min_dist = 1000000.0f;

			for ( int k = 0 ; k < NumPoints ; k++ )
			{
				float dX = ( ( Points[k].X * ( float )W ) - ( float )i );
				float dY = ( ( Points[k].Y * ( float )H ) - ( float )j );
				float dist = dX * dX + dY * dY;

				if ( dist < min_dist ) { min_dist = dist; min_idx = k; }
			}

			// assign color
			SetPixel( i, j, 0, Colors[min_idx] );
		}
	}
}

void clBitmap::Convert_BGRAToRGBA()
{
	for ( int k = 0; k != FBitmapParams.FDepth; k++ )
	{
		for ( int j = 0; j != FBitmapParams.FHeight; j++ )
		{
			for ( int i = 0; i != FBitmapParams.FWidth; i++ )
			{
				this->SetPixel( i, j, k, this->GetPixel( i, j, k ).BGRA() );
			}
		}
	}
}

void clBitmap::Substract( clBitmap* Another, clBitmap* Out )
{
	clBitmap* Dest = Out ? Out : this;

	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	int D = FBitmapParams.FDepth;

	for ( int z = 0 ; z < D ; z++ )
	{
		for ( int y = 0 ; y < H ; y++ )
		{
			for ( int x = 0 ; x < W ; x++ )
			{
				LVector4 SrcC = GetPixel( x, y, z );
				Dest->SetPixel( x, y, z, SrcC - Another->GetPixel( x, y, z ) );
			}
		}
	}
}

clBitmap* clBitmap::DifferenceWith( clBitmap* Another )
{
	clBitmap* Res = clBitmap::CreateBitmap( Another->Env, FBitmapParams.FWidth, FBitmapParams.FHeight, FBitmapParams.FDepth, FBitmapParams.FBitmapFormat, FBitmapParams.FTextureType );

	Substract( Another, Res );

	return Res;
}

void clBitmap::Linearize32BitZBuffer( float zNear, float zFar )
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;

	float* fptr = ( float* )FBitmapData;

	float a = zFar / ( zFar - zNear );
	float b = zNear * a; // zFar * zNear / ( zNear - zFar )

	for ( int y = 0 ; y < H ; y++ )
	{
		for ( int x = 0 ; x < W ; x++ )
		{
			*fptr++ = b / ( ( *fptr ) - a );
		}
	}
}

clBitmap* clBitmap::FloatToRGBA8() const
{
	LTextureType Fmt = FBitmapParams.FTextureType;

	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	int D = FBitmapParams.FDepth;

	clBitmap* Res = clBitmap::CreateBitmap( Env, W, H, D, L_BITMAP_BGRA8, Fmt );

	for ( int z = 0 ; z < D ; z++ )
	{
		for ( int y = 0 ; y < H ; y++ )
		{
			for ( int x = 0 ; x < W ; x++ )
			{
				LVector4 SrcC = GetPixel( x, y, z );
				Res->SetPixel( x, y, z, SrcC );
			}
		}
	}

	return Res;
}

/// Create a new bitmap with single intensity channel from a given RGB image
clBitmap* clBitmap::ConvertToGrayscale8bit() const
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	int D = FBitmapParams.FDepth;

	clBitmap* Res = clBitmap::CreateBitmap( Env, W, H, D, L_BITMAP_GRAYSCALE8, FBitmapParams.FTextureType );

	for ( int k = 0; k != FBitmapParams.FDepth; k++ )
	{
		for ( int j = 0; j != FBitmapParams.FHeight; j++ )
		{
			for ( int i = 0; i != FBitmapParams.FWidth; i++ )
			{
				LVector4 V = GetPixel( i, j, k );
				float C = 0.11f * V.X + 0.59f * V.Y + 0.3f * V.Z;
				Res->SetPixel( i, j, k, LVector4( C ) );
			}
		}
	}

	return Res;
}

clBitmap* clBitmap::ExtractColorPlane8bit( int Idx ) const
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	int D = FBitmapParams.FDepth;

	clBitmap* Res = clBitmap::CreateBitmap( Env, W, H, D, L_BITMAP_GRAYSCALE8, FBitmapParams.FTextureType );

	for ( int k = 0; k != D; k++ )
	{
		for ( int j = 0; j != H; j++ )
		{
			for ( int i = 0; i != W; i++ )
			{
				Res->SetPixel( i, j, k, LVector4( GetPixelComponent( i, j, k, Idx ) ) );
			}
		}
	}

	return Res;
}

/// Assign specified color plane from Src bitmap to this
void clBitmap::CopyColorPlane( clBitmap* Src, int FromIdx, int ToIdx )
{
	for ( int k = 0; k != FBitmapParams.FDepth; k++ )
	{
		for ( int j = 0; j != FBitmapParams.FHeight; j++ )
		{
			for ( int i = 0; i != FBitmapParams.FWidth; i++ )
			{
				SetPixelComponent( i, j, k, ToIdx, Src->GetPixelComponent( i, j, k, FromIdx ) );
			}
		}
	}
}

inline LVector4 ExposeColor( float Exposure, const LVector4& In )
{
	float NewR = 1.0f - expf( Exposure * In.X );
	float NewG = 1.0f - expf( Exposure * In.Y );
	float NewB = 1.0f - expf( Exposure * In.Z );

	return LVector4( NewR, NewG, NewB, In.W );
}

void clBitmap::ApplyExposure( float Exposure, clBitmap* Out )
{
	clBitmap* To = Out ? Out : this;

	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	int D = FBitmapParams.FDepth;

	int i, j, k;
	LVector4 V, NewV;

	for ( k = 0; k != D; k++ )
	{
		for ( j = 0; j != H; j++ )
		{
			for ( i = 0; i != W; i++ )
			{
				To->SetPixel( i, j, k, ExposeColor( Exposure, GetPixel( i, j, k ) ) );
			}
		}
	}
}

clBitmap* clBitmap::CopyBitmap( int X1, int Y1, int X2, int Y2 ) const
{
	if ( FBitmapParams.FTextureType != L_TEXTURE_2D ) { return NULL; }

	clBitmap* Bitmap = new clBitmap( X2 - X1, Y2 - Y1, 1, FBitmapParams.FBitmapFormat, L_TEXTURE_2D );

	for ( int j = Y1; j != Y2; j++ )
	{
		for ( int i = X1; i != X2; i++ )
		{
			Bitmap->SetPixel( i - X1, j - Y1, 0, GetPixel( i, j, 0 ) );
		}
	}

	return Bitmap;
}

void clBitmap::PutBitmap( int X, int Y, const clBitmap& Other )
{
	if ( FBitmapParams.FTextureType != L_TEXTURE_2D ) { return; }

	if ( Other.FBitmapParams.FTextureType != L_TEXTURE_2D ) { return; }

	for ( int j = 0; j != Other.FBitmapParams.FHeight; j++ )
	{
		for ( int i = 0; i != Other.FBitmapParams.FWidth; i++ )
		{
			this->SetPixel( i + X, j + Y, 0, Other.GetPixel( i, j, 0 ) );
		}
	}
}

void clBitmap::PutBitmapRotated90CCW( int X, int Y, const clBitmap& Other )
{
	if ( FBitmapParams.FTextureType != L_TEXTURE_2D ) { return; }

	if ( Other.FBitmapParams.FTextureType != L_TEXTURE_2D ) { return; }

	for ( int j = 0; j != Other.FBitmapParams.FWidth; j++ )
	{
		for ( int i = 0; i != Other.FBitmapParams.FHeight; i++ )
		{
			this->SetPixel( i + X, j + Y, 0, Other.GetPixel( j, i, 0 ) );
		}
	}
}

void clBitmap::PutBitmapRotated90CW( int X, int Y, const clBitmap& Other )
{
	if ( FBitmapParams.FTextureType != L_TEXTURE_2D ) { return; }

	if ( Other.FBitmapParams.FTextureType != L_TEXTURE_2D ) { return; }

	for ( int j = 0; j != Other.FBitmapParams.FWidth; j++ )
	{
		for ( int i = 0; i != Other.FBitmapParams.FHeight; i++ )
		{
			this->SetPixel( i + X, j + Y, 0, Other.GetPixel( Other.FBitmapParams.FWidth-j-1, Other.FBitmapParams.FHeight-i-1, 0 ) );
		}
	}
}

void clBitmap::Swap( clBitmap* Other )
{
	if ( !Other ) { return; }

	std::swap( FBitmapParams, Other->FBitmapParams );
	std::swap( FBitmapData,   Other->FBitmapData   );
}

void clBitmap::RescaleBitmap( int NewWidth, int NewHeight )
{
	if ( FBitmapParams.FTextureType != L_TEXTURE_2D ) { return; }

	if ( FBitmapParams.FWidth == NewWidth && FBitmapParams.FHeight == NewHeight ) { return; }

	clBitmap* NewBitmap = new clBitmap( NewWidth, NewHeight, 1, FBitmapParams.FBitmapFormat, FBitmapParams.FTextureType );

	RescaleImage( FBitmapData, NewBitmap->FBitmapData, NewWidth, NewHeight );

	Swap( NewBitmap );

	this->FBitmapParams.FWidth = NewWidth;
	this->FBitmapParams.FHeight = NewHeight;

	delete( NewBitmap );
}
/*
clBitmap::clBitmap( const clBitmap& Other )
{
   if ( &Other == this ) { return; }

   clBitmap Bitmap( Other );

   Swap( &Bitmap );
}

clBitmap& clBitmap::operator = ( const clBitmap& Other )
{
   if ( &Other == this ) { return *this; }

   clBitmap Bitmap( Other );

   Swap( &Bitmap );

   return *this;
}
*/
clBitmap::~clBitmap()
{
	if ( FBitmapData ) { delete( FBitmapData ); }

	FBitmapData = NULL;
}

void clBitmap::FlipVertical()
{
	if ( FBitmapParams.FTextureType != L_TEXTURE_2D ) { return; }

	clBitmap* NewBitmap = new clBitmap( FBitmapParams.FWidth, FBitmapParams.FHeight, 1, FBitmapParams.FBitmapFormat, FBitmapParams.FTextureType );

	FlipBitmapVertical( FBitmapData, NewBitmap->FBitmapData );

	Swap( NewBitmap );

	delete( NewBitmap );
}

void clBitmap::Rotate90CW()
{
	if ( FBitmapParams.FTextureType != L_TEXTURE_2D ) { return; }

	clBitmap* NewBitmap = new clBitmap( FBitmapParams.FHeight, FBitmapParams.FWidth, 1, FBitmapParams.FBitmapFormat, FBitmapParams.FTextureType );

	for ( int y = 0 ; y < FBitmapParams.FHeight ; y++ )
	{
		for ( int x = 0 ; x < FBitmapParams.FWidth ; x++ )
		{
			NewBitmap->SetPixel( FBitmapParams.FHeight-y-1, FBitmapParams.FWidth-x-1, 0, GetPixel( x, y, 0 ) );
		}
	}

	Swap( NewBitmap );

	delete( NewBitmap );
}

void clBitmap::Rotate90CCW()
{
	if ( FBitmapParams.FTextureType != L_TEXTURE_2D ) { return; }

	clBitmap* NewBitmap = new clBitmap( FBitmapParams.FHeight, FBitmapParams.FWidth, 1, FBitmapParams.FBitmapFormat, FBitmapParams.FTextureType );

	for ( int y = 0 ; y < FBitmapParams.FHeight ; y++ )
	{
		for ( int x = 0 ; x < FBitmapParams.FWidth ; x++ )
		{
			NewBitmap->SetPixel( y, x, 0, GetPixel( x, y, 0 ) );
		}
	}

	Swap( NewBitmap );

	delete( NewBitmap );
}

void clBitmap::Rotate180()
{
	if ( FBitmapParams.FTextureType != L_TEXTURE_2D ) { return; }

	clBitmap* NewBitmap = new clBitmap( FBitmapParams.FWidth, FBitmapParams.FHeight, 1, FBitmapParams.FBitmapFormat, FBitmapParams.FTextureType );

	for ( int y = 0 ; y < FBitmapParams.FHeight ; y++ )
	{
		for ( int x = 0 ; x < FBitmapParams.FWidth ; x++ )
		{
			NewBitmap->SetPixel( FBitmapParams.FWidth-x-1, y, 0, GetPixel( x, y, 0 ) );
		}
	}

	Swap( NewBitmap );

	delete( NewBitmap );
}

FIXME( "possibly buggy for float images" )
void clBitmap::RescaleImage( const Lubyte* Src, Lubyte* Dst, int NewWidth, int NewHeight ) const
{
	guard();

	// NOTE: doesn't work with DXT-compressed images

	FATAL( FBitmapParams.FDepth > 1, "Unable to rescale 3D textures or cube maps" );

	float ScaleFactorW = static_cast<float>( FBitmapParams.FWidth  ) / NewWidth;
	float ScaleFactorH = static_cast<float>( FBitmapParams.FHeight ) / NewHeight;

	int BPP = FBitmapParams.GetBytesPerPixel();

	for ( int NewX = 0; NewX != NewWidth; ++NewX )
	{
		for ( int NewY = 0; NewY != NewHeight; ++NewY )
		{
			int OldX = static_cast<int>( ScaleFactorW * static_cast<float>( NewX ) );
			int OldY = static_cast<int>( ScaleFactorH * static_cast<float>( NewY ) );

			int SrcOffset = BPP * ( FBitmapParams.FWidth * OldY + OldX );
			int DstOffset = BPP * ( NewWidth * NewY + NewX );

			memcpy( &Dst[DstOffset], &Src[SrcOffset], BPP );
		}
	}

	unguard();
}

sBitmapParams sBitmapParams::ConvertRecCrossFormatToCubeMap() const
{
	FIXME( "return INVALID_FORMAT instead of FATAL" )
	FATAL( FTextureType != L_TEXTURE_2D, "Only 2D texture can be converted" );
	FATAL( FWidth  % 3 != 0, "Width of 2D texture for cross format cube map should be multiple of 3" );
	FATAL( FHeight % 4 != 0, "Height of 2D texture for cross format cube map should be multiple of 4" );

	int FaceWidth  = FWidth  / 3;
	int FaceHeight = FHeight / 4;

	sBitmapParams CubeBMPRec = sBitmapParams( Env,
	                                          FaceWidth,
	                                          FaceHeight,
	                                          6,          // depth
	                                          FBitmapFormat,
	                                          L_TEXTURE_CUBE );

	return CubeBMPRec;
}

void clBitmap::ConvertCrossFormatToCubeMap( const Lubyte* Src, Lubyte* Dst ) const
{
	// NOTE: doesn't work with DXT-compressed images

	sBitmapParams CubeMap = FBitmapParams.ConvertRecCrossFormatToCubeMap();

	int PixelSize  = FBitmapParams.GetBitsPerPixel() / 8;
	int FaceWidth  = CubeMap.FWidth;
	int FaceHeight = CubeMap.FHeight;

	Lubyte* Destination = Dst;

	/*
	        ------
	        | +Y |
	   ----------------
	   | -X | -Z | +X |
	   ----------------
	        | -Y |
	        ------
	        | +Z |
	        ------
	*/

	for ( int Face = 0; Face != 6; ++Face )
	{
		for ( int j = 0; j != CubeMap.FHeight; ++j )
		{
			for ( int i = 0; i != CubeMap.FWidth; ++i )
			{
				int X = 0;
				int Y = 0;

				switch ( Face )
				{
						// GL_TEXTURE_CUBE_MAP_POSITIVE_X
					case 0:
						X = i;
						Y = FaceHeight + j;
						break;
						// GL_TEXTURE_CUBE_MAP_NEGATIVE_X
					case 1:
						X = 2 * FaceWidth  + i;
						Y = 1 * FaceHeight + j;
						break;
						// GL_TEXTURE_CUBE_MAP_POSITIVE_Y
					case 2:
						X = 2 * FaceWidth  - ( i + 1 );
						Y = 1 * FaceHeight - ( j + 1 );
						break;
						// GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
					case 3:
						X = 2 * FaceWidth  - ( i + 1 );
						Y = 3 * FaceHeight - ( j + 1 );
						break;
						// GL_TEXTURE_CUBE_MAP_POSITIVE_Z
					case 4:
						X = 2 * FaceWidth - ( i + 1 );
						Y =     FBitmapParams.FHeight   - ( j + 1 );
						break;
						// GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
					case 5:
						X = FaceWidth  + i;
						Y = FaceHeight + j;
						break;
				}

				TODO( "copy 1 pixel - this could be done more efficient, but needs a memcpy_r() to correctly reverse scanlines" )

				memcpy( Destination, Src + ( Y * FBitmapParams.FWidth + X ) * PixelSize, PixelSize );

				Destination += PixelSize;
			}
		}
	}
}

void clBitmap::UpdateSetPixelProc()
{
	FSetPixelProc = &clBitmap::SetPixel_Generic;
	if ( FBitmapParams.FBitmapFormat == L_BITMAP_GRAYSCALE8 ) FSetPixelProc = &clBitmap::SetPixel_Grayscale8;
}

bool clBitmap::ReallocImageData( const sBitmapParams* Params )
{
	if ( Params != NULL )
	{
		if ( FBitmapParams.FWidth        == Params->FWidth  &&
		     FBitmapParams.FHeight       == Params->FHeight &&
		     FBitmapParams.FDepth        == Params->FDepth  &&
		     FBitmapParams.FBitmapFormat == Params->FBitmapFormat &&
		     FBitmapData                 != NULL )
		{
			return true;
		}

		FBitmapParams = *Params;

		UpdateSetPixelProc();
	}

	if ( FBitmapData != NULL )
	{
		delete[]( FBitmapData );

		FBitmapData = NULL;
	}

	size_t Size = FBitmapParams.GetStorageSize();

	if ( Size == 0 )
	{
		return false;
	}

	FBitmapData = new Lubyte[ Size ];

	return true;
}

bool clBitmap::Load2DImage( sEnvironment* Env, const LString& FileName )
{
	if ( !Env->FileSystem->FileExists( FileName ) )
	{
		Env->Logger->LogP( L_WARNING, "File not found: %s", FileName.c_str() );

		return false;
	}

	iIStream* Stream = Env->FileSystem->CreateFileReader( FileName );

	bool Result = Load2DImageFromStream( Env, Stream );

	delete( Stream );

	return Result;
}

bool clBitmap::Load2DImageFromStream( sEnvironment* Env, iIStream* Stream )
{
	if ( Stream == NULL ) { return false; }

	LString Ext;
	Env->FileSystem->SplitPath( Stream->GetFileName(), NULL, NULL, NULL, &Ext );
	LStr::ToUpper( &Ext );

	bool Result = false;

#if L_USE_FREEIMAGE

	if ( Ext == ".TGA" || Ext == ".GIF" || Ext == ".PNG" || Ext == ".TIF" || Ext == ".TIFF" ||
	     Ext == ".BMP" || Ext == ".JPG" || Ext == ".HDR" || Ext == ".EXR" || Ext == "" )
	{
		Result = FreeImage_LoadFromStream( Stream, this, true );
	}
	else
#else
	if ( Ext == ".BMP" )
	{
		const char* ErrorCode = LoadBMP( Stream->MapStream(), Stream->GetFileSize(), this );
		Result = ( ErrorCode == NULL );

		if ( ErrorCode ) { FATAL_MSG( ErrorCode ); }
	}
	else
#endif
		if ( Ext == ".DDS" )
		{
			const char* ErrorCode = LoadDDS( Stream->MapStream(), static_cast<size_t>( Stream->GetFileSize() ), this );
			Result = ( ErrorCode == NULL );

			if ( ErrorCode ) { FATAL_MSG( ErrorCode ); }
		}
		else if ( Ext == ".RAW" )
		{
			Result = ReallocImageData( NULL );

			if ( Result ) { Stream->BlockRead( FBitmapData, Stream->GetFileSize() ); }
		}

	if ( !Result ) { Env->Logger->Log( L_WARNING, "Failed to load 2D image: " + Stream->GetFileName() ); }

	return Result;
}

bool clBitmap::LoadCubeImage( sEnvironment* Env, const LString& FileName )
{
	iIStream* Stream = Env->FileSystem->CreateFileReader( FileName );

	if ( Stream == NULL ) { return false; }

	LString Ext;
	Env->FileSystem->SplitPath( FileName, NULL, NULL, NULL, &Ext );
	LStr::ToUpper( &Ext );

	bool Result = false;

#if L_USE_FREEIMAGE

	if ( Ext == ".TGA" || Ext == ".GIF" || Ext == ".PNG" ||
	     Ext == ".BMP" || Ext == ".JPG" || Ext == ".HDR" || Ext == ".EXR" )
#else
	if ( Ext == ".BMP" )
#endif
	{
#if L_USE_FREEIMAGE
		Result = FreeImage_LoadFromStream( Stream, this, true );
#else
		const char* ErrorCode = LoadBMP( Stream->MapStream(), Stream->GetFileSize(), this );
		Result = ( ErrorCode == NULL );
#endif

		sBitmapParams CubeBMPRec = FBitmapParams.ConvertRecCrossFormatToCubeMap();

		Lubyte* CubeMap = static_cast<Lubyte*>( malloc( CubeBMPRec.GetStorageSize() ) );

		ConvertCrossFormatToCubeMap( FBitmapData, CubeMap );

		ReallocImageData( &CubeBMPRec );

		memcpy( FBitmapData, CubeMap, CubeBMPRec.GetStorageSize() );

		free( CubeMap );
	}
	else if ( Ext == ".DDS" )
	{
		const char* ErrorCode = LoadDDS( Stream->MapStream(), static_cast<size_t>( Stream->GetFileSize() ), this );
		Result = ( ErrorCode == NULL );
	}
	else if ( Ext == ".RAW" )
	{
		Result = ReallocImageData( NULL );

		if ( Result ) { Stream->BlockRead( FBitmapData, Stream->GetFileSize() ); }
	}

	delete( Stream );

	if ( !Result ) { Env->Logger->Log( L_WARNING, "Failed to load Cube image: " + FileName ); }

	return Result;
}

bool clBitmap::Load3D( sEnvironment* Env, const LString& FileName, bool Gradients, bool ESL )
{
	guard();

	iIStream* Stream = Env->FileSystem->CreateFileReader( FileName );

	if ( Stream == NULL )
	{
		return false;
	}

	LString Ext;
	Env->FileSystem->SplitPath( FileName, NULL, NULL, NULL, &Ext );
	LStr::ToUpper( &Ext );

	bool Result = false;

	if ( Ext == ".DDS" )
	{
		if ( !Gradients && !ESL )
		{
			const char* ErrorCode = LoadDDS( Stream->MapStream(), static_cast<size_t>( Stream->GetFileSize() ), this );
			Result = ( ErrorCode == NULL );
		}
		else
		{
			// FLastError = "DDS loader does not support gradients and ESL maps";
		}
	}
	else /*if ( Ext == ".RAW" )*/
	{
		clRAWLoader* Loader = Env->Linker->Instantiate( "clRAWLoader" );

		Result = Loader->Load( Stream, this, Gradients, ESL );

		delete( Loader );
	}

	delete( Stream );

	if ( !Result ) { Env->Logger->Log( L_WARNING, "Failed to load 3D image: " + FileName ); }

	return Result;

	unguard();
}

void clBitmap::DisposeObject()
{
	// really unsafe...
	delete this;
}

bool clBitmap::Save2DImage( sEnvironment* Env, const LString& FileName )
{
	LBitmapFormat Fmt = FBitmapParams.FBitmapFormat;

	int TheBits = Fmt == L_BITMAP_BGR8 ? 24 : ( Fmt == L_BITMAP_GRAYSCALE8 ? 8 : 32 );

	bool Result = true;

#if L_USE_FREEIMAGE
	FreeImage_SaveScreenshot_BPP( Env, FileName, FBitmapData, FBitmapParams.FWidth, FBitmapParams.FHeight, TheBits );
#else
	iOStream* OStream = Env->FileSystem->CreateFileWriter( FileName );

	Env->Logger->LogP( L_DEBUG, "Image size: %i x %i @ %i", FBitmapParams.FWidth, FBitmapParams.FHeight, TheBits );

	int OutSize = 0x36 + FBitmapParams.FWidth * FBitmapParams.FHeight * ( TheBits / 8 );
	Lubyte* OutBuffer = new Lubyte[OutSize];

	SaveBMP( OutBuffer, OutSize, FBitmapData, FBitmapParams.FWidth, FBitmapParams.FHeight, TheBits );

	OStream->BlockWrite( OutBuffer, OutSize );

	delete[] OutBuffer;

	delete( OStream );
#endif

	return Result;
}

bool clBitmap::Save3DImage( sEnvironment* Env, const LString& FileName )
{
	iOStream* OStream = Env->FileSystem->CreateFileWriter( FileName );

	if ( OStream == NULL )
	{
		return false;
	}

	OStream->BlockWrite( FBitmapData, FBitmapParams.GetStorageSize() );

	delete OStream;

	return true;
}

bool clBitmap::IsCompressed() const
{
	return FBitmapParams.IsCompressedFormat();
}

clBitmap::clBitmap( const int Width,
                    const int Height,
                    const int Depth,
                    const LBitmapFormat BitmapFormat,
                    const LTextureType TextureType ): FBitmapParams( NULL, Width, Height, Depth, BitmapFormat, L_TEXTURE_2D ),
	FBitmapData( NULL ),
	FSetPixelProc( &clBitmap::SetPixel_Generic )
{
	ReallocImageData( NULL );
	UpdateSetPixelProc();
}

clBitmap::clBitmap( const sBitmapParams& Params ): FBitmapParams( Params ), FBitmapData( NULL )
{
	ReallocImageData( &Params );
	UpdateSetPixelProc();
}

bool clBitmap::IsEqual( const clBitmap* Other ) const
{
	if ( memcmp( &FBitmapParams, &Other->FBitmapParams, sizeof( FBitmapParams )     ) != 0 )
	{
		return false;
	}

	if ( memcmp( FBitmapData,    Other->FBitmapData, FBitmapParams.GetStorageSize() ) != 0 )
	{
		return false;
	}

	return true;
}

clBitmap* clBitmap::CreateEmptyBitmap( sEnvironment* Env )
{
	clBitmap* Bitmap = new clBitmap();
	Bitmap->Env = Env;
	Bitmap->AfterConstruction();

	return Bitmap;
}

clBitmap* clBitmap::CreateBitmap( sEnvironment* Env, int W, int H, int D, LBitmapFormat BMPFormat, LTextureType TextureType )
{
	clBitmap* Bitmap = new clBitmap( W, H, D, BMPFormat, TextureType );
	Bitmap->Env = Env;
	Bitmap->AfterConstruction();

	return Bitmap;
}

/// Set pixel component (R,G,B,A)
void  clBitmap::SetPixelComponent( int X, int Y, int Z, int Idx, float Val )
{
	int Offset = PixelOffset( X, Y, Z );

	Lubyte ByteVal = static_cast<Lubyte>( Val * 255.0f );

	float* FloatData = reinterpret_cast<float*>( &FBitmapData[Offset] );

	switch ( FBitmapParams.FBitmapFormat )
	{
		case L_BITMAP_GRAYSCALE8:

			if ( Idx == 0 )
			{
				FBitmapData[Offset] = ByteVal;
			}

			break;

		case L_BITMAP_BGR8:
		case L_BITMAP_BGRA8:
			FBitmapData[Offset + Idx] = ByteVal;
			break;

		case L_BITMAP_FLOAT32_RGBA:
		case L_BITMAP_FLOAT32_RGB:
			FloatData[Idx] = Val;
			break;

		case L_BITMAP_FLOAT32_R:

			if ( Idx == 0 )
			{
				FloatData[0] = Val;
			}

			break;
			/*
			      TODO( "word-greyscale support" )
			      case L_BITMAP_GRAYSCALE16:
			         break;
			      TODO( "half-float support ?" )
			      case L_BITMAP_FLOAT16_RGBA:
			         return 2 * 4;
			      case L_BITMAP_FLOAT16_RGB:
			         return 2 * 3;
			*/
		default:
			break;
	}
}

/// Get pixel component (R,G,B,A)
float clBitmap::GetPixelComponent( int X, int Y, int Z, int Idx ) const
{
	int Offset = PixelOffset( X, Y, Z );

	float* FloatData = reinterpret_cast<float*>( &FBitmapData[Offset] );

	switch ( FBitmapParams.FBitmapFormat )
	{
		case L_BITMAP_GRAYSCALE8:
			return ( Idx == 0 ) ? ( static_cast<float>( FBitmapData[Offset] ) / 255.0f ) : 0.0f;

		case L_BITMAP_BGR8:
			return FBitmapData[Offset + ( 2 - Idx )] / 255.0f;

		case L_BITMAP_BGRA8:

			if ( Idx == 3 ) { return FBitmapData[Offset + 3]; }

			return FBitmapData[Offset + ( 2 - Idx )];

		case L_BITMAP_FLOAT32_RGBA:
		case L_BITMAP_FLOAT32_RGB:
			return FloatData[Idx];

		case L_BITMAP_FLOAT32_R:

			if ( Idx == 0 )
			{
				return FloatData[0];
			}

			break;
			/*
			      TODO( "word-greyscale support" )
			      case L_BITMAP_GRAYSCALE16:
			         break;
			      TODO( "half-float support ?" )
			      case L_BITMAP_FLOAT16_RGBA:
			         return 2 * 4;
			      case L_BITMAP_FLOAT16_RGB:
			         return 2 * 3;
			*/
	}

	return 0;
}

void clBitmap::SetPixel( int X, int Y, int Z, const LVector4& Color )
{
	(this->*FSetPixelProc)( X, Y, Z, Color );
}

void clBitmap::SetPixel_Generic( int X, int Y, int Z, const LVector4& Color )
{
	if ( X >= FBitmapParams.FWidth  || X < 0 ) { return; }

	if ( Y >= FBitmapParams.FHeight || Y < 0 ) { return; }

	if ( Z >= FBitmapParams.FDepth  || Z < 0 ) { return; }

	int Offset = PixelOffset( X, Y, Z );

	Lubyte ByteR = static_cast<Lubyte>( Color.X * 255.0f );
	Lubyte ByteG = static_cast<Lubyte>( Color.Y * 255.0f );
	Lubyte ByteB = static_cast<Lubyte>( Color.Z * 255.0f );
	Lubyte ByteA = static_cast<Lubyte>( Color.W * 255.0f );

	float* FloatDataRGBA = reinterpret_cast<float*>( &FBitmapData[Offset] );
	float* FloatDataRGB = reinterpret_cast<float*>( &FBitmapData[Offset] );

	switch ( FBitmapParams.FBitmapFormat )
	{
		case L_BITMAP_GRAYSCALE8:
			FBitmapData[Offset] = ByteR;
			break;

		case L_BITMAP_BGR8:
			FBitmapData[Offset + 0] = ByteB;
			FBitmapData[Offset + 1] = ByteG;
			FBitmapData[Offset + 2] = ByteR;
			break;

		case L_BITMAP_BGRA8:
			FBitmapData[Offset + 0] = ByteB;
			FBitmapData[Offset + 1] = ByteG;
			FBitmapData[Offset + 2] = ByteR;
			FBitmapData[Offset + 3] = ByteA;
			break;

		case L_BITMAP_FLOAT32_RGBA:
			FloatDataRGBA[0] = Color.X;
			FloatDataRGBA[1] = Color.Y;
			FloatDataRGBA[2] = Color.Z;
			FloatDataRGBA[3] = Color.W;
			break;

		case L_BITMAP_FLOAT32_RGB:
			FloatDataRGB[0] = Color.X;
			FloatDataRGB[1] = Color.Y;
			FloatDataRGB[2] = Color.Z;
			break;

		case L_BITMAP_FLOAT32_R:
			FloatDataRGB[0] = Color.X;
			break;
			/*
			      TODO( "word-greyscale support" )
			      case L_BITMAP_GRAYSCALE16:
			         break;
			      TODO( "half-float support ?" )
			      case L_BITMAP_FLOAT16_RGBA:
			         return 2 * 4;
			      case L_BITMAP_FLOAT16_RGB:
			         return 2 * 3;
			*/
		default:
			break;
	}
}

void clBitmap::SetPixel_Grayscale8( int X, int Y, int Z, const LVector4& Color )
{
	if ( X >= FBitmapParams.FWidth  || X < 0 ) { return; }

	if ( Y >= FBitmapParams.FHeight || Y < 0 ) { return; }

	if ( Z >= FBitmapParams.FDepth  || Z < 0 ) { return; }

	int Offset = PixelOffset( X, Y, Z );

	FBitmapData[Offset] = static_cast<Lubyte>( Color.X * 255.0f );
}

LVector4 clBitmap::GetPixel( int X, int Y, int Z ) const
{
	int Offset = PixelOffset( X, Y, Z );

	float* FloatDataRGBA = reinterpret_cast<float*>( &FBitmapData[Offset] );

	switch ( FBitmapParams.FBitmapFormat )
	{
		case L_BITMAP_GRAYSCALE8:
			return LVector4( static_cast<float>( FBitmapData[Offset] ) / 255.0f );

		case L_BITMAP_BGR8:
			return LVector4( static_cast<float>( FBitmapData[Offset + 2] ) / 255.0f,
			                 static_cast<float>( FBitmapData[Offset + 1] ) / 255.0f,
			                 static_cast<float>( FBitmapData[Offset + 0] ) / 255.0f, 0.0f );

		case L_BITMAP_BGRA8:
			return LColors::BGRA8toRGBAvec4( ( Luint32* )&FBitmapData[Offset] );
			/*     return LVector4( static_cast<float>( FBitmapData[Offset + 2] ) / 255.0f,
			                      static_cast<float>( FBitmapData[Offset + 1] ) / 255.0f,
			                      static_cast<float>( FBitmapData[Offset + 0] ) / 255.0f,
			                      static_cast<float>( FBitmapData[Offset + 3] ) / 255.0f );*/

		case L_BITMAP_FLOAT32_RGBA:
			return LVector4( FloatDataRGBA[0], FloatDataRGBA[1], FloatDataRGBA[2], FloatDataRGBA[3] );

		case L_BITMAP_FLOAT32_RGB:
			return LVector4( FloatDataRGBA[0], FloatDataRGBA[1], FloatDataRGBA[2], 0.0f );

		case L_BITMAP_FLOAT32_R:
			return LVector4( FloatDataRGBA[0], 0.0f, 0.0f, 0.0f );
			/*
			      TODO( "word-greyscale support" )
			      case L_BITMAP_GRAYSCALE16:
			         break;
			      TODO( "half-float support ?" )
			      case L_BITMAP_FLOAT16_RGBA:
			         return 2 * 4;
			      case L_BITMAP_FLOAT16_RGB:
			         return 2 * 3;
			*/
	}

	return LVector4();
}

/// Bilinearly filtered GetPixel
LVector4 clBitmap::BilinearInterpolate( float x, float y )
{
	float fX = x * ( float )( FBitmapParams.FWidth - 1 );
	float fY = y * ( float )( FBitmapParams.FHeight - 1 );

	int nX = Math::Clamp( 0, ( int )x, FBitmapParams.FWidth - 2 );
	int nY = Math::Clamp( 0, ( int )y, FBitmapParams.FHeight - 2 );

	float ratioX = fX - nX;
	float ratioY = fY - nY;

	LVector4 Result =
	   GetPixel( nX + 0, nY + 0, 0 ) * ( 1.0f - ratioX ) * ( 1.0f - ratioY ) +
	   GetPixel( nX + 1, nY + 0, 0 ) * ratioX * ( 1.0f - ratioY ) +
	   GetPixel( nX + 0, nY + 1, 0 ) * ( 1.0f - ratioX ) * ratioY +
	   GetPixel( nX + 1, nY + 1, 0 ) * ratioX * ratioY;

	return Result;
}

void clBitmap::CalculateSecondMoments3D( double& Area, double& CX, double& CY, double& CZ, double& mu200, double& mu020, double& mu002, double& mu110, double& mu101, double& mu011 ) const
{
	CalculateCentroid3D( Area, CX, CY, CZ );

	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	int D = FBitmapParams.FDepth;

	double Denom = 1.0 / static_cast<double>( 255.0f );

	unsigned char* DataPtr = FBitmapData;

	double ThisX, ThisY, ThisZ;

	double dW = static_cast<double>( W );
	double dH = static_cast<double>( H );
	double dD = static_cast<double>( D );

	int X, Y, Z;

	mu200 = 0.0;
	mu020 = 0.0;
	mu002 = 0.0;
	mu110 = 0.0;
	mu101 = 0.0;
	mu011 = 0.0;

	double Xbar, Ybar, Zbar, Val;

	for ( Z = 0 ; Z < D ; Z++ )
	{
		for ( Y = 0 ; Y < H ; Y++ )
		{
			for ( X = 0 ; X < W ; X++ )
			{
				Val = static_cast<double>( *DataPtr++ ) * Denom;

				ThisX = ( static_cast<double>( X ) - dW / 2 ) / dW;
				ThisY = ( static_cast<double>( Y ) - dH / 2 ) / dH;
				ThisZ = ( static_cast<double>( Z ) - dD / 2 ) / dD;

				Xbar = ThisX - CX;
				Ybar = ThisY - CY;
				Zbar = ThisZ - CZ;

				mu200 += Xbar * Xbar * Val;
				mu110 += Xbar * Ybar * Val;
				mu101 += Xbar * Zbar * Val;
				mu020 += Ybar * Ybar * Val;
				mu011 += Ybar * Zbar * Val;
				mu002 += Zbar * Zbar * Val;
			}
		}
	}
}

// only 8-bit images yet
void clBitmap::CalculateCentroid3D( double& Area, double& Xbar, double& Ybar, double& Zbar ) const
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	int D = FBitmapParams.FDepth;

	double Denom = 1.0 / 255.0;

	unsigned char* DataPtr = FBitmapData;

	double tSum = 0.0;
	double tX = 0.0;
	double tY = 0.0;
	double tZ = 0.0;

	double ThisX, ThisY, ThisZ;

	double tW = static_cast<double>( W );
	double tH = static_cast<double>( H );
	double tD = static_cast<double>( D );

	int X, Y, Z;

	for ( Z = 0 ; Z < D ; Z++ )
	{
		for ( Y = 0 ; Y < H ; Y++ )
		{
			for ( X = 0 ; X < W ; X++ )
			{
				double Val = static_cast<double>( *DataPtr++ ) * Denom;
				tSum += Val;

				ThisX = ( static_cast<double>( X ) - tW / 2 ) / tW;
				ThisY = ( static_cast<double>( Y ) - tH / 2 ) / tH;
				ThisZ = ( static_cast<double>( Z ) - tD / 2 ) / tD;

				tX += Val * ThisX;
				tY += Val * ThisY;
				tZ += Val * ThisZ;
			}
		}
	}

	if ( tSum > 0.0 )
	{
		tX /= tSum;
		tY /= tSum;
		tZ /= tSum;
	}

	Xbar = tX;
	Ybar = tY;
	Zbar = tZ;
	Area = tSum;
}

// This can be used for 2D also, the moments with 'z' will just be zero or something like that
LMatrix3 clBitmap::CalculateCovariance3D() const
{
	/**
	   mu_{200} -> M[0][0]
	   mu_{110} -> M[0][1], M[1][0]
	   mu_{101} -> M[0][2], M[2][0]
	   mu_{020} -> M[1][1]
	   mu_{011} -> M[1][2], M[2][1]
	   mu_{002} -> M[2][2]
	*/

	LMatrix3 Result;

	double Area, Xbar, Ybar, Zbar;
	double mu200, mu020, mu002, mu110, mu101, mu011;

	CalculateSecondMoments3D( Area, Xbar, Ybar, Zbar, mu200, mu020, mu002, mu110, mu101, mu011 );

	double Denom = 1.0 / ( ( Area > 0.000001 ) ? Area : 1.0 );

	Result[0][0] = static_cast<float>( mu200 * Denom );
	Result[1][1] = static_cast<float>( mu020 * Denom );
	Result[2][2] = static_cast<float>( mu002 * Denom );
	Result[1][0] = static_cast<float>( mu110 * Denom );
	Result[2][0] = static_cast<float>( mu101 * Denom );
	Result[1][2] = static_cast<float>( mu011 * Denom );
	Result[0][1] = Result[1][0];
	Result[0][2] = Result[2][0];
	Result[2][1] = Result[1][2];

	return Result;
}

void clBitmap::HoughTransform2D( clBitmap* Out ) const
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;

	int NumAngles = Out->FBitmapParams.FHeight - 1;

	Out->Clear( LC_Black );

	float Angle = 0.0f;
	float dA = 180.0f / NumAngles;
	float r;

	LArray<float> s_a( NumAngles + 1 );
	LArray<float> c_a( NumAngles + 1 );

	for ( int i = 0 ; i <= NumAngles ; i++ )
	{
		s_a[i] = sinf( Math::DegToRad( Angle ) );
		c_a[i] = cosf( Math::DegToRad( Angle ) );
		Angle += dA;
	}

	float* fptr = ( float* )Out->FBitmapData;

	int i, x, y, Dx;

	for ( y = 0 ; y < H ; y++ )
	{
		for ( x = 0 ; x < W ; x++ )
		{
			if ( GetPixel( x, y, 0 ).SqrLength() < Math::EPSILON ) { continue; }

			// for each possible angle
			Angle = 0.0f;

			for ( i = 0 ; i <= NumAngles ; i++ )
			{
				// r(theta) = x*cos(theta)+y*sin(theta);
				r = x * c_a[i] + y * s_a[i];

				Dx = ( static_cast<int>( r ) + W ) / 2;

				fptr[i * ( NumAngles + 1 ) + ( static_cast<int>( r ) + W )/2 ] += 1.0f;
//				Out->SetPixel(Dx, i, 0, vec4(Out->GetPixel(Dx, i, 0).X + 1.0f, 0.0f, 0.0f, 0.0f));
//				add_value(out, MaxR, NumAngles + 1, (static_cast<int>(r) + W)/2, i);

				Angle += dA;
			}
		}
	}

//	Out->NormalizeImage();
}

void clBitmap::CalculateChannelHistogram( LHistogram1D* Out, int Channel )
{
	if ( !Out ) { return; }

	int NumPixels = FBitmapParams.FWidth * FBitmapParams.FHeight * FBitmapParams.FDepth;

	Out->AnalyzeData8Skip( NumPixels, FBitmapData, FBitmapParams.GetBytesPerPixel(), Channel );
}

void clBitmap::CalculateIntensityHistogram( LHistogram1D* Out )
{
	if ( !Out ) { return; }

	int NumPixels = FBitmapParams.FWidth * FBitmapParams.FHeight * FBitmapParams.FDepth;

	Out->AnalyzeData8RGBIntensity( NumPixels, FBitmapData, FBitmapParams.GetBytesPerPixel() );
}

void clBitmap::FindMinMax( LVector4* Min, LVector4* Max )
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	int D = FBitmapParams.FDepth;

	*Max = LVector4( -1000000.0f );
	*Min = LVector4( +1000000.0f );

	for ( int z = 0 ; z < D ; z++ )
	{
		for ( int y = 0 ; y < H ; y++ )
		{
			for ( int x = 0 ; x < W ; x++ )
			{
				LVector4 C = GetPixel( x, y, z );

				if ( C.X > Max->X ) { Max->X = C.X; }

				if ( C.Y > Max->Y ) { Max->Y = C.Y; }

				if ( C.Z > Max->Z ) { Max->Z = C.Z; }

				if ( C.W > Max->W ) { Max->W = C.W; }

				if ( C.X < Min->X ) { Min->X = C.X; }

				if ( C.Y < Min->Y ) { Min->Y = C.Y; }

				if ( C.Z < Min->Z ) { Min->Z = C.Z; }

				if ( C.W < Min->W ) { Min->W = C.W; }
			}
		}
	}
}

void clBitmap::NormalizeImage( clBitmap* Out )
{
	LVector4 Min, Max, TheScale;

	FindMinMax( &Min, &Max );

	TheScale = Max - Min;

	if ( TheScale.W < Math::EPSILON ) { TheScale.W = 1.0f; }

	if ( TheScale.X < Math::EPSILON ) { TheScale.X = 1.0f; }

	if ( TheScale.Y < Math::EPSILON ) { TheScale.Y = 1.0f; }

	if ( TheScale.Z < Math::EPSILON ) { TheScale.Z = 1.0f; }

	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	int D = FBitmapParams.FDepth;

	clBitmap* Ptr = Out ? Out : this;

	for ( int z = 0 ; z < D ; z++ )
	{
		for ( int y = 0 ; y < H ; y++ )
		{
			for ( int x = 0 ; x < W ; x++ )
			{
				LVector4 V = ( GetPixel( x, y, z ) - Min );
				LVector4 N( V.X / TheScale.X, V.Y / TheScale.Y, V.Z / TheScale.Z, V.W / TheScale.W );

				Ptr->SetPixel( x, y, z,  N );
			}
		}
	}
}

clBitmap* clBitmap::NormalizeImageAlloc()
{
	clBitmap* Res = clBitmap::CreateBitmap( Env, FBitmapParams.FWidth, FBitmapParams.FHeight, FBitmapParams.FDepth, L_BITMAP_BGRA8, FBitmapParams.FTextureType );

	NormalizeImage( Res );

	return Res;
}

clBitmap* clBitmap::MakeNormalMap()
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	clBitmap* Res = clBitmap::CreateBitmap( Env, W, H, 1, L_BITMAP_BGR8, L_TEXTURE_2D );

	for ( int y = 0 ; y < H ; y++ )
	{
		for ( int x = 0 ; x < W ; x++ )
		{
			LVector4 left = GetPixel( ( x > 0 ) ? x - 1 : x, y, 0 );
			LVector4 right = GetPixel( ( x < ( W - 1 ) ) ? x + 1 : x, y, 0 );
			LVector4 down = GetPixel( x, ( y < ( H - 1 ) ) ? y + 1 : y, 0 );
			LVector4 up = GetPixel( x, ( y > 0 ) ? y - 1 : y, 0 );

			float leftH = ( left.X + left.Y + left.Z ) / 3.0f;
			float rightH = ( right.X + right.Y + right.Z ) / 3.0f;
			float downH = ( down.X + down.Y + down.Z ) / 3.0f;
			float upH = ( up.X + up.Y + up.Z ) / 3.0f;

			float dX = ( rightH - leftH ) / 2.0f;
			float dY = ( downH - upH ) / 2.0f;
			float dZ = sqrtf( 1.0f - dX * dX - dY * dY );

			LVector4 C;
			C.X = ( dX + 1.0f ) / 2.0f;
			C.Y = ( dY + 1.0f ) / 2.0f;
			C.Z = ( dZ + 1.0f ) / 2.0f;
			C.W = 1.0f;

			Res->SetPixel( x, y, 0, C );
		}
	}

	return Res;
}

void clBitmap::MakeXorPattern()
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	int D = FBitmapParams.FDepth;

	for ( int i = 0 ; i < W ; i++ )
	{
		for ( int j = 0 ; j < H ; j++ )
		{
			for ( int k = 0 ; k < D ; k++ )
			{
				unsigned char V = ( i ^ j ^ k ) & 0xFF;
				float c = static_cast<float>( V ) / 255.0f;
				LVector4 C = vec4( c, c, c, 1.0f );

				SetPixel( k, j, i, C );
			}
		}
	}
}

void clBitmap::Tangle2D( clBitmap* Out )
{
	// not yet
}

//void clBitmap::Untangle2D(clBitmap* Out)
//{
// not yet
//}

/// Store sqrt(r * r + g * g) to B channel
void clBitmap::CalculateComplexMagnitude()
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	int D = FBitmapParams.FDepth;

	for ( int i = 0 ; i < W ; i++ )
	{
		for ( int j = 0 ; j < H ; j++ )
		{
			for ( int k = 0 ; k < D ; k++ )
			{
				LVector4 V = GetPixel( i, j, k );
				SetPixelComponent( i, j, k, 2, sqrtf( V.X * V.X + V.Y * V.Y ) );
			}
		}
	}
}

/// Store arg( r + i* g ) to A channel
void clBitmap::CalculateComplexPhase()
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	int D = FBitmapParams.FDepth;

	for ( int i = 0 ; i < W ; i++ )
	{
		for ( int j = 0 ; j < H ; j++ )
		{
			for ( int k = 0 ; k < D ; k++ )
			{
				LVector4 V = GetPixel( i, j, k );
				SetPixelComponent( i, j, k, 2, atan2f( V.Y, V.X ) );
			}
		}
	}
}

void clBitmap::MakeWave2D( const LVector2& Center, float Ampl, const LVector2& K, float Omega, float t, float Phi, bool Add )
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;

	for ( int x = 0 ; x < W ; x++ )
	{
		for ( int y = 0 ; y < H ; y++ )
		{
			float Phase = K.Dot( LVector2( x, y ) ) - Omega * t + Phi;

			float Val = Ampl * sin( Phase );

			if ( !Add )
			{
				SetPixel( x, y, 0, vec4( Val, Val, Val, 1.0f ) );
			}
			else
			{
				float OldVal = GetPixel( x, y, 0 ).X;
				SetPixel( x, y, 0, vec4( Val + OldVal, Val + OldVal, Val + OldVal, 1.0f ) );
			}
		}
	}
}

void clBitmap::MakeWave3D( const LVector3& Center, float Ampl, const LVector3& K, float Omega, float t, float Phi, bool Add )
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	int D = FBitmapParams.FDepth;

	for ( int x = 0 ; x < W ; x++ )
	{
		for ( int y = 0 ; y < H ; y++ )
		{
			for ( int z = 0 ; z < D ; z++ )
			{
				float Phase = K.Dot( LVector3( x, y, z ) ) - Omega * t + Phi;

				float Val = Ampl * sin( Phase );

				if ( !Add )
				{
					SetPixel( x, y, 0, vec4( Val, Val, Val, 1.0f ) );
				}
				else
				{
					float OldVal = GetPixel( x, y, z ).X;
					SetPixel( x, y, z, vec4( Val + OldVal, Val + OldVal, Val + OldVal, 1.0f ) );
				}
			}
		}
	}
}

/// Rainbow fill
void clBitmap::FillColorSquare()
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	for ( int j = 0 ; j < H ; j++ )
		for ( int i = 0 ; i < W ; i++ )
		{
			float u = static_cast<float>( i ) / (float)W;
			float v = static_cast<float>( j ) / (float)H;

			this->SetPixel( i, j, 0, v * LVector4( Math::ColorFromWaveLength( 780.0f + u * ( 350.0f - 780.0f ) ), 1.0f ) );
		}
}

/// Make 2D rainbow gradient
scriptmethod void clBitmap::MakeRainbowGradient( const LVector2& From, const LVector2& To )
{
	/// Calculate the number of points in [From,To] segment
	int NumPoints = static_cast<int>( ( To - From ).Length() + 0.5f );

	if ( NumPoints <= 0 ) { return; }

	LVector2 Dir = ( To - From ).GetNormalized();
	LVector2 N = LVector2( -Dir.Y, Dir.X );

	const float StartWave = 350.0f;
	const float EndWave = 780.0f;

	float W = StartWave;

	float dW = ( EndWave - StartWave ) / static_cast<float>( NumPoints );

	LVector2 P = From;
	LVector2 dP = ( To - From ) / static_cast<float>( NumPoints );

	int Wd = FBitmapParams.FWidth;
	int Ht = FBitmapParams.FHeight;

	float Sz = sqrtf( static_cast<float>( Wd * Wd + Ht * Ht ) );

	for ( int Idx = 0 ; Idx < NumPoints + 1; Idx++, W += dW, P += dP )
	{
		LVector4 Color = LVector4( Math::ColorFromWaveLength( W ), 1.0f );

		LVector2 p1 = P + Sz * N;
		LVector2 p2 = P - Sz * N;

		/// Trace the line with N direction and base point P
		DrawLine2D( ( int )p1.x, ( int )p1.y, ( int )p2.x, ( int )p2.y, Color );
		DrawLine2D( ( int )p1.x + 1, ( int )p1.y, ( int )p2.x + 1, ( int )p2.y, Color );
		DrawLine2D( ( int )p1.x, ( int )p1.y + 1, ( int )p2.x, ( int )p2.y + 1, Color );
	}
}

void clBitmap::MakeGlow2( float Exposure, float DiscSize )
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;

	for ( int y = 0; y < H ; y++ )
	{
		float dy = ( y + 0.5f ) / ( float )H - 0.5f;

		for ( int x = 0 ; x < W ; x++ )
		{
			float dx = ( x + 0.5f ) / ( float )W - 0.5f;

			float Dist = sqrtf( dx * dx + dy * dy );

			float Intensity = 2.0f - Math::LMin( 2.0f, powf( 2.0f, Math::LMax( Dist - DiscSize, 0.0f ) * Exposure ) );

			SetPixel( x, y, 0, LVector4( Intensity, Intensity, Intensity, Intensity ) );
		}
	}
}

void clBitmap::NoiseFill3( int nSeed )
{
	LNoise fr( 3, nSeed, 0.5f, 2.0f );

	Math::Randomize( nSeed );

	int nx = FBitmapParams.FWidth;
	int ny = FBitmapParams.FHeight;
	int nz = FBitmapParams.FDepth;

	for ( int z = 0; z < nz; z++ )
	{
		for ( int y = 0; y < ny; y++ )
		{
			for ( int x = 0; x < nx; x++ )
			{
				float f[3] = {( float )( x << 2 ) / nx, ( float )( y << 2 ) / ny, ( float )z / nz};

				float v = Math::LMax( 0.0f, fr.fBm( f, 8.0f ) );

				LVector4 Color( v, v, v, 1.0f );

				SetPixel( x, y, z, Color );
			}
		}
	}
}

void clBitmap::NoiseFill2( int nSeed )
{
	LNoise fr( 2, nSeed, 0.5f, 2.0f );

	Math::Randomize( nSeed );

	int nx = FBitmapParams.FWidth;
	int ny = FBitmapParams.FHeight;

	for ( int y = 0; y < ny; y++ )
	{
		for ( int x = 0; x < nx; x++ )
		{
			float f[2] = {( float )( x << 2 ) / nx, ( float )( y << 2 ) / ny};

			float v = Math::LMax( 0.0f, fr.fBm( f, 8.0f ) );

			LVector4 Color( v, v, v, 1.0f );

			SetPixel( x, y, 0, Color );
		}
	}
}

/// Sure, this is far from being optimized
void clBitmap::HorizontalLine( int y, int x1, int x2, const vec4& Color )
{
	++x2; // avoid strict inequality in a loop below

	for ( int x = x1 ; x < x2 ; x++ )
	{
		SetPixel( x, y, 0, Color );
	}
}

#define _SWAP_INT(v1, v2) { tmp = v1; v1 = v2; v2 = tmp;  }
#define _SWAP_PT(i1, i2) { _SWAP_INT(x##i1, x##i2) _SWAP_INT(y##i1, y##i2) }

void clBitmap::SolidTriangle( int x1, int y1, int x2, int y2, int x3, int y3, const vec4& Color )
{
	int tmp;

	if ( y1 > y2 ) _SWAP_PT( 1, 2 )
		if ( y1 > y3 ) _SWAP_PT( 1, 3 )
			if ( y2 > y3 ) _SWAP_PT( 2, 3 )

				// y1 < y2 < y3
				// x2 < x3

				int y;

	int x_b = x1;
	int x_e = x1;
	int x_mid;

	if ( y1 != y2 )
		for ( y = y1 ; y <= y2 ; y++ )
		{
			x_b = x1 + ( x2 - x1 ) * ( y - y1 ) / ( y2 - y1 );
			x_e = x1 + ( x3 - x1 ) * ( y - y1 ) / ( y3 - y1 );

			if ( x_e < x_b ) _SWAP_INT( x_e, x_b )

				HorizontalLine( y, x_b, x_e, Color );
		}

	x_mid = ( x2 > x3 ) ? x_b : x_e;

	if ( y2 != y3 )
		for ( y = y2 ; y <= y3 ; y++ )
		{
			x_b = x2 + ( x3 - x2 ) * ( y - y2 ) / ( y3 - y2 );
			x_e = x_mid + ( x3 - x_mid ) * ( y - y2 ) / ( y3 - y2 );

			if ( x_e < x_b ) _SWAP_INT( x_e, x_b )

				HorizontalLine( y, x_b, x_e, Color );
		}
}

#undef _SWAP_INT
#undef _SWAP_PT

void clBitmap::FillBox( const LVector3& Min, const LVector3& Max, const LVector4& Color )
{
	for ( int z = ( int )Min.Z; z <= ( int )Max.Z; z++ )
	{
		for ( int y = ( int )Min.Y; y <= ( int )Max.Y; y++ )
		{
			for ( int x = ( int )Min.X; x <= ( int )Max.X; x++ )
			{
				SetPixel( x, y, z, Color );
			}
		}
	}
}

clBitmap* clBitmap::RenderIntensityHistogram( int Width, int Height )
{
	clBitmap* Res = clBitmap::CreateBitmap( Env, Width, Height, 1, L_BITMAP_BGR8, L_TEXTURE_2D );
	Res->Clear( LC_Black );

	LHistogram1D Hist;

	CalculateIntensityHistogram( &Hist );

	Res->RenderHistogram( &Hist, LC_White );

	return Res;
}

clBitmap* clBitmap::RenderChannelHistogram( int Width, int Height, int Channel )
{
	clBitmap* Res = clBitmap::CreateBitmap( Env, Width, Height, 1, L_BITMAP_BGR8, L_TEXTURE_2D );
	Res->Clear( LC_Black );

	LHistogram1D Hist;

	CalculateChannelHistogram( &Hist, Channel );

	LVector4 HistC[] = { LC_Red, LC_Green, LC_Blue, LC_White };

	Res->RenderHistogram( &Hist, HistC[Channel] );

	return Res;
}

void clBitmap::RenderHistogram( LHistogram1D* Hist, const LVector4& Color )
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;

	float dS = 1.0f / ( float )W;

	for ( int x = 0 ; x < W ; x++ )
	{
		float y_val = ( ( float )H ) * Hist->GetNormalizedDensity( ( float )x * dS );

		DrawLine2D( x, 0, x, ( int )y_val, Color );
	}
}

void clBitmap::Bresenham_SetPixel( int x, int y, int Sign, int Inverse, const LVector4& Color )
{
	int out_x, out_y;

	int Mult = ( Sign && !Inverse ) ? -1 : 1;

	out_x = Mult * ( Inverse ? x : y );
	out_y = Mult * ( Inverse ? y : x );

	SetPixel( out_x, out_y, 0, Color );
}

void clBitmap::BHM( int x0, int y0, int x1, int y1, int Sign, int Inverse, const LVector4& Color )
{
	int twoDY = 2 * ( y1 - y0 );
	int twoDX = 2 * ( x1 - x0 );

	int DX = x1 - x0;

	int incY = ( ( twoDY < 0 ) ? -1 : 1 );

	twoDY = abs( twoDY );

	int p = twoDY - DX;
	int x = x0, y = y0;

	Bresenham_SetPixel( x, y, Sign, Inverse, Color );

	while ( x < x1 )
	{
		++x;

		if ( !( p < 0 ) )
		{
			y += incY;
			p -= twoDX;
		}

		p += twoDY;
		Bresenham_SetPixel( x, y, Sign, Inverse, Color );
	}
}

// non-optimal Bresenham's implementation somewhere from wikipedia. used for debug 2d drawing
void clBitmap::DrawLine2D( int x0, int y0, int x1, int y1, const LVector4& Color )
{
	bool INVERSE = 1;
	bool SIGN = 0;

	float m = 0;
	int t;
	bool Is_INFINITY = ( x1 == x0 );

	if ( !Is_INFINITY )
	{
		m = ( y1 - y0 ) / ( float )( x1 - x0 );
	}

	if ( m < 0 )
	{
		SIGN = true;
	}

	if ( Is_INFINITY || ( fabs( m ) > 1 ) )
	{
		INVERSE = false;

		if ( SIGN )
		{
			t = -x0;
			x0 = -y0;
			y0 = t;
			t = -x1;
			x1 = -y1;
			y1 = t;
		}
		else
		{
			t = x0;
			x0 = y0;
			y0 = t;
			t = x1;
			x1 = y1;
			y1 = t;
		}
	}

	if ( x1 < x0 )
	{
		BHM( x1, y1, x0, y0, SIGN, INVERSE, Color );
	}
	else
	{
		BHM( x0, y0, x1, y1, SIGN, INVERSE, Color );
	}
}

void clBitmap::Clear( const LVector4& Color )
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	int D = FBitmapParams.FDepth;

	Lubyte ByteR = static_cast<Lubyte>( Color.X * 255.0f );
	Lubyte ByteG = static_cast<Lubyte>( Color.Y * 255.0f );
	Lubyte ByteB = static_cast<Lubyte>( Color.Z * 255.0f );
	Lubyte ByteA = static_cast<Lubyte>( Color.W * 255.0f );

	if ( FBitmapParams.FBitmapFormat == L_BITMAP_BGR8 )
	{
		for ( int i = 0 ; i < W * H * D ; i++ )
		{
			FBitmapData[i*3 + 0] = ByteB;
			FBitmapData[i*3 + 1] = ByteG;
			FBitmapData[i*3 + 2] = ByteR;
		}
	}

	if ( FBitmapParams.FBitmapFormat == L_BITMAP_GRAYSCALE8 )
	{
		for ( int i = 0 ; i < W * H * D ; i++ )
		{
			FBitmapData[i] = ByteB;
		}
	}

	if ( FBitmapParams.FBitmapFormat == L_BITMAP_BGRA8 )
	{
		for ( int i = 0 ; i < W * H * D ; i++ )
		{
			FBitmapData[i*3 + 0] = ByteB;
			FBitmapData[i*3 + 1] = ByteG;
			FBitmapData[i*3 + 2] = ByteR;
			FBitmapData[i*3 + 3] = ByteA;
		}
	}

	if ( FBitmapParams.FBitmapFormat == L_BITMAP_FLOAT32_RGBA )
	{
		LVector4* Ptr = reinterpret_cast<LVector4*>( FBitmapData );

		for ( int i = 0 ; i < W * H * D ; i++ )
		{
			Ptr[i] = Color;
		}
	}

	if ( FBitmapParams.FBitmapFormat == L_BITMAP_FLOAT32_RGB )
	{
		LVector3* Ptr = reinterpret_cast<LVector3*>( FBitmapData );
		LVector3 Color3 = Color.ToVector3();

		for ( int i = 0 ; i < W * H * D ; i++ )
		{
			Ptr[i] = Color3;
		}
	}

	if ( FBitmapParams.FBitmapFormat == L_BITMAP_FLOAT32_R )
	{
		float* Ptr = reinterpret_cast<float*>( FBitmapData );

		for ( int i = 0 ; i < W * H * D ; i++ )
		{
			Ptr[i] = Color.X;
		}
	}
}

// http://www.cobrabytes.com/index.php?topic=1150.0
void clBitmap::DrawLine3D( int _x0, int _y0, int _z0, int _x1, int _y1, int _z1, const LVector4& Color )
{
	int x0, x1, y0, y1, z0, z1;

	x0 = _x0;
	y0 = _y0;
	z0 = _z0;
	x1 = _x1;
	y1 = _y1;
	z1 = _z1;

	int x, y, z;
	int delta_x, delta_y, delta_z;
	int step_x,  step_y,  step_z;

	bool swap_xy, swap_xz;

	int drift_xy, drift_xz;

	int cx, cy, cz;

	// 'steep' xy Line, make longest delta x plane
	swap_xy = abs( y1 - y0 ) > abs( x1 - x0 );

	if ( swap_xy )
	{
		std::swap( x0, y0 );
		std::swap( x1, y1 );
	}

	//do same for xz
	swap_xz = abs( z1 - z0 ) > abs( x1 - x0 );

	if ( swap_xz )
	{
		std::swap( x0, z0 );
		std::swap( x1, z1 );
	}

	//delta is Length in each plane
	delta_x = abs( x1 - x0 );
	delta_y = abs( y1 - y0 );
	delta_z = abs( z1 - z0 );

	//drift controls when to step in 'shallow' planes
	//starting value keeps Line centred
	drift_xy  = ( delta_x / 2 );
	drift_xz  = ( delta_x / 2 );

	//direction of line

	step_x = ( x0 > x1 ) ? -1 : 1;
	step_y = ( y0 > y1 ) ? -1 : 1;
	step_z = ( z0 > z1 ) ? -1 : 1;

	//starting point
	y = y0;
	z = z0;

	//step through longest delta (which we have swapped to x)
	for ( x = x0 ; x <= x1 ; x += step_x )
	{
		//copy position
		cx = x;
		cy = y;
		cz = z;

		//unswap (in reverse)
		if ( swap_xz )
		{
			std::swap( cx, cz );
		}

		if ( swap_xy )
		{
			std::swap( cx, cy );
		}

		//passes through this point
		//debugmsg(":" + cx + ", " + cy + ", " + cz)

		this->SetPixel( cx, cy, cz, Color );

		//--------------

		//update progress in other planes
		drift_xy = drift_xy - delta_y;
		drift_xz = drift_xz - delta_z;

		//step in y plane
		if ( drift_xy < 0 )
		{
			y = y + step_y;
			drift_xy = drift_xy + delta_x;
		}

		//same in z
		if ( drift_xz < 0 )
		{
			z = z + step_z;
			drift_xz = drift_xz + delta_x;
		}
	}
}

scriptmethod void clBitmap::LogNormalize( clBitmap* Out )
{
	LVector4 Min, Max;

	FindMinMax( &Min, &Max );

	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	int D = FBitmapParams.FDepth;

	clBitmap* Ptr = Out ? Out : this;

	for ( int z = 0 ; z < D ; z++ )
	{
		for ( int y = 0 ; y < H ; y++ )
		{
			for ( int x = 0 ; x < W ; x++ )
			{
				LVector4 V = ( GetPixel( x, y, z ) - Min ) + vec4( 1.0f, 1.0f, 1.0f, 1.0f );
				LVector4 N( logf( 1.0f + V.X ), logf( 1.0f + V.Y ), logf( 1.0f + V.Z ), logf( 1.0f + V.W ) );

				Ptr->SetPixel( x, y, z,  N );
			}
		}
	}
}

clBitmap* clBitmap::LogNormalizeAlloc()
{
	clBitmap* Res = clBitmap::CreateBitmap( Env, FBitmapParams.FWidth, FBitmapParams.FHeight, FBitmapParams.FDepth, L_BITMAP_BGRA8, FBitmapParams.FTextureType );

	LogNormalize( Res );

	return Res;
}

void clBitmap::Untangle2D( LFFT* fft )
{
	int n1 = fft->n1;
	int n2 = fft->n2;

	/// 0 < k1 < n1, 0 < k2 < n2/2,
	for ( int k1 = 1 ; k1 < n1 / 2 ; k1++ )
	{
		for ( int k2 = 1 ; k2 < n2 / 2 ; k2++ )
		{
			float Re = fft->a2D[k1][2 * k2 + 0];
			float Im = fft->a2D[k1][2 * k2 + 1];

			LVector4 Val1( Re,  Im, 0.0f, 0.0f );
			LVector4 Val2( Re, -Im, 0.0f, 0.0f );

			SetPixel( k1, k2, 0, Val1 );
			SetPixel( n1 - k1, n2 - k2, 0, Val2 );
			/*
			      R[k1][k2]       =  Re; I[k1][k2]       =  Im;
			      R[n1-k1][n2-k2] =  Re; I[n1-k1][n2-k2] = -Im;
			*/
		}
	}

	/// 0 < k2 < n2/2
	for ( int k2 = 1 ; k2 < n2 / 2 ; k2++ )
	{
		float Re = fft->a2D[0][2 * k2];
		float Im = fft->a2D[0][2 * k2 + 1];

		LVector4 Val1( Re,  Im, 0.0f, 0.0f );
		LVector4 Val2( Re, -Im, 0.0f, 0.0f );

		SetPixel( 0, k2, 0, Val1 );
		SetPixel( 0, n2 - k2, 0, Val2 );
		/*
		      R[0][k2]    =  Re; R[0][n2-k2] =  Re;
		      I[0][k2]    =  Im; I[0][n2-k2] = -Im;
		*/
	}

	/// 0 < k1 < n1/2,
	for ( int k1 = 1 ; k1 < n1 / 2 ; k1++ )
	{
		float Re = fft->a2D[k1][0];
		float Im = fft->a2D[k1][1];

		LVector4 Val1( Re,  Im, 0.0f, 0.0f );
		LVector4 Val2( Re, -Im, 0.0f, 0.0f );

		SetPixel( k1, 0, 0, Val1 );
		SetPixel( n1 - k1, 0, 0, Val2 );

		Re = fft->a2D[n1-k1][1];
		Im = fft->a2D[n1-k1][0];

		Val1.X = Re;
		Val1.Y = -Im;
		Val2.X = Re;
		Val2.Y =  Im;

		SetPixel( k1, n2 / 2, 0, Val1 );
		SetPixel( n1 - k1, n2 / 2, 0, Val2 );

		/*
		      R[k1][0]    =  Re; I[k1][0]    =  Im;
		      R[n1-k1][0] =  Re; I[n1-k1][0] = -Im;

		      R[k1][n2/2]    =  Re;
		      I[k1][n2/2]    = -Im;

		      R[n1-k1][n2/2] =  Re;
		      I[n1-k1][n2/2] =  Im;
		*/
	}

	SetPixel( 0,           0, 0, vec4( fft->a2D[0][0],    0.0f, 0.0f, 0.0f ) );
	SetPixel( 0,      n2 / 2, 0, vec4( fft->a2D[0][1],    0.0f, 0.0f, 0.0f ) );
	SetPixel( n1 / 2,      0, 0, vec4( fft->a2D[n1/2][0], 0.0f, 0.0f, 0.0f ) );
	SetPixel( n1 / 2, n2 / 2, 0, vec4( fft->a2D[n1/2][1], 0.0f, 0.0f, 0.0f ) );

	/*
	   R[0][0]       = a[0][0];
	   R[0][n2/2]    = a[0][1];
	   R[n1/2][0]    = a[n1/2][0];
	   R[n1/2][n2/2] = a[n1/2][1]

	   I[0][0]       = 0
	   I[0][n2/2]    = 0
	   I[n1/2][0]    = 0
	   I[n1/2][n2/2] = 0
	*/
}

void clBitmap::ScalePixelComponents( const LVector4& Factor )
{
	int W = FBitmapParams.FWidth;
	int H = FBitmapParams.FHeight;
	int D = FBitmapParams.FDepth;

	for ( int z = 0 ; z < D ; z++ )
	{
		for ( int y = 0 ; y < H ; y++ )
		{
			for ( int x = 0 ; x < W ; x++ )
			{
				LVector4 V = GetPixel( x, y, z );

				// make per-component multiplication
				SetPixel( x, y, z, V * Factor );
			}
		}
	}
}

clBitmap* clBitmap::CombineImages( const std::vector<clBitmap*>& Src, int MaxWidth, int InterDistance, std::vector<vec2>& Offsets )
{
	if ( Src.size() == 0 ) { return NULL; }

	LBitmapFormat CommonFormat = Src[0]->GetBitmapFormat();

	// Result dimensions
	int W = MaxWidth;
	int H = 0;

	// Offset counters
	int OfsX = 0;
	int OfsY = 0;

	// Height of the current image stride (maximum height of Src[j] in current row)
	int CurMaxHeight = 0;

	// 1. Step one: calculate offsets
	Offsets.resize( Src.size() );

	for ( size_t i = 0 ; i != Src.size() ; i++ )
	{
		int CurW = Src[i]->GetWidth();
		int CurH = Src[i]->GetHeight();

		/// If the current image does not fit into the row we adjust the offsets
		if ( OfsX + CurW + InterDistance > MaxWidth )
		{
			OfsX = 0;
			OfsY += CurMaxHeight + InterDistance;

			// Increment total size for the non-terminal image
			if ( i != Src.size() - 1 )
			{
				H += CurMaxHeight;
			}

			// Start new row by saving the image height
			CurMaxHeight = CurH;
		}
		else
		{
			// Adjust current row height
			if ( CurH > CurMaxHeight )
			{
				CurMaxHeight = CurH;
			}
		}

		// Then we store offsets for current image
		Offsets[i] = vec2( OfsX, OfsY );

		// And advance to the next image
		OfsX += CurW + InterDistance;
	}

	H += CurMaxHeight;

	// 2. Step two: copy images to the result
	clBitmap* Result = clBitmap::CreateBitmap( Src[0]->Env, W, H, 1, CommonFormat, L_TEXTURE_2D );

	Result->Clear( vec4( 0, 0, 0, 0 ) );

	for ( size_t j = 0 ; j != Src.size() ; j++ )
	{
		Result->PutBmp( ( int )Offsets[j].X, ( int )Offsets[j].Y, Src[j] );
	}

	return Result;
}

clBitmap* clBitmap::Tile3DImage( int AtlasW, int AtlasH ) const
{
	clBitmap* Res = clBitmap::CreateBitmap( Env, AtlasW, AtlasH, 1, L_BITMAP_GRAYSCALE8, L_TEXTURE_2D );

	int TileW = this->FBitmapParams.FWidth;
	int TileH = this->FBitmapParams.FHeight;
	int D = this->FBitmapParams.FDepth;

	int NumTilesX = AtlasW / TileW;
	int NumTilesY = D / NumTilesX;

	for ( int j = 0 ; j < NumTilesY ; j++ )
	{
		for ( int i = 0 ; i < NumTilesX ; i++ )
		{
			int SliceIdx = j * NumTilesX + i;

			if ( SliceIdx >= D ) { continue; }

			int OfsX = i * TileW;
			int OfsY = j * TileH;

			for ( int y = 0 ; y < TileH ; y++ )
				for ( int x = 0 ; x < TileW ; x++ )
				{
					vec4 C = this->GetPixel( x, y, SliceIdx );
					Res->SetPixel( x + OfsX, y + OfsY, 0, C );
				}
		}
	}

	return Res;
}

clBitmap* clBitmap::ToColorFromGrayscale() const
{
	sBitmapParams Params = this->FBitmapParams;

	clBitmap* Res = clBitmap::CreateEmptyBitmap( Env );
	Params.FBitmapFormat = L_BITMAP_BGRA8;
	Res->ReallocImageData( &Params );

	int W = Params.FWidth, H = Params.FHeight, D = Params.FDepth;

	for ( int i = 0 ; i < D ; i++ )
		for ( int j = 0 ; j < H ; j++ )
			for ( int k = 0 ; k < W ; k++ )
			{
				vec4 C = this->GetPixel( k, j, i );

				Res->SetPixel( k, j, i, vec4( C.X, C.X, C.X, 1.0f ) );
			}

	return Res;
}

////// Bitmap grouping

struct imgtree_tex_node
{
	/// This image
	clBitmap* img;

	/// Children
	imgtree_tex_node* left, *right;

	/// Rect
	int x, y, w, h;
};

// Idea from http://www.blackpawn.com/texts/lightmaps/default.html

void imgtree_delete_node( imgtree_tex_node* nn )
{
	if ( nn->left ) { imgtree_delete_node( nn->left ); }

	if ( nn->right ) { imgtree_delete_node( nn->right ); }

	delete nn;
}

imgtree_tex_node* imgtree_new_node( int x, int y, int w, int h )
{
	imgtree_tex_node* n = new imgtree_tex_node();
	n->x = x;
	n->y = y;
	n->w = w;
	n->h = h;
	n->img = 0;
	n->left = n->right = 0;
	return n;
}

imgtree_tex_node* imgtree_insert_img( imgtree_tex_node* n, clBitmap* img )
{
	/// Not a leaf
	if ( ( n->left != 0 ) || ( n->right != 0 ) )
	{
		/// use left child first
		imgtree_tex_node* newNode = imgtree_insert_img( n->left, img );

		if ( newNode != 0 ) { return newNode; }

		/// no room, insert into second
		return imgtree_insert_img( n->right, img );
	}

	/// if there's already an image here, return
	if ( n->img != 0 ) { return 0; }

	int w = img->GetWidth(), h = img->GetHeight();

	/// Image does not fit ?
	if ( w > n->w || h > n->h ) { return 0; }

	/// if we're just right, accept
	if ( w == n->w && h == n->h ) { return n; }

	/// otherwise, split this node and create some kids
	int dw = n->w - w, dh = n->h - h;

	/// decide which way to split
	if ( dw > dh )
	{
		n->left = imgtree_new_node( n->x, n->y, w, n->h );
		n->right = imgtree_new_node( n->x + w, n->y, n->w - w, n->h );
	}
	else
	{
		n->left = imgtree_new_node( n->x, n->y, n->w, h );
		n->right = imgtree_new_node( n->x, n->y + h, n->w, n->h - h );
	}

	/// insert into first child we created)
	return imgtree_insert_img( n->left, img );
}

vec2 imgtree_insert_to_atlas( clBitmap* img, clBitmap* atlas, imgtree_tex_node* root )
{
	imgtree_tex_node* n = imgtree_insert_img( root, img );

	vec2 v;
	v.x = v.y = -1;

	if ( n != 0 )
	{
		atlas->PutBitmap( n->x, n->y, *img );
		n->img = img;
		v.x = ( float )n->x;
		v.y = ( float )n->y;
	}

	return v;
}

void clBitmap::CombineImages_Dense( const std::vector<clBitmap*>& Images, std::vector<vec2>& Offsets )
{
	imgtree_tex_node* root = imgtree_new_node( 0, 0, this->GetWidth(), this->GetHeight() );

	for ( size_t j = 0 ; j != Images.size() ; j++ )
	{
		Offsets[j] = imgtree_insert_to_atlas( Images[j], this, root );
	}

	imgtree_delete_node( root );
}

/*
 * 02/02/2012
     Rotate90CW(), Rotate90CCW(), Rotate180()
 * 09/12/2010
     Copy constructor, virtual destructor, assignment operator
 * 17/07/2010
     Bitmap format descriptions
     Covariance matrix and second moments
     Load3D - duplicate code removal
 * 08/02/2010
     IsEqual()
 * 02/02/2010
     It's here
*/
