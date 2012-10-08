/**
 * \file Bitmap.h
 * \brief Bitmap image
 * \version 0.6.08
 * \date 02/02/2012
 * \author Sergey Kosarevsky, 2010-2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _Bitmap__h_
#define _Bitmap__h_

#include "Platform.h"
#include "LString.h"
#include "LColors.h"
#include "Math/LMatrix.h"
#include "Math/LBlending.h"
#include "Core/iObject.h"

class sEnvironment;
class iIStream;

class LHistogram1D;

class LFFT;

TODO( "extend with DXT compression types" )

enum LBitmapFormat
{
	L_BITMAP_INVALID_FORMAT = -1,
	L_BITMAP_GRAYSCALE8     =  0,
	L_BITMAP_GRAYSCALE16    =  1,
	L_BITMAP_BGR8           =  2,
	L_BITMAP_BGRA8          =  3,
	L_BITMAP_FLOAT16_RGBA   =  4,
	L_BITMAP_FLOAT32_RGBA   =  5,
	L_BITMAP_FLOAT16_RGB    =  6,
	L_BITMAP_FLOAT32_RGB    =  7,
	L_BITMAP_FLOAT32_R      =  8,
	L_BITMAP_DXT1           =  9,
	L_BITMAP_DXT3           =  10,
	L_BITMAP_DXT5           =  11
};

/// Texture type identifier
enum LTextureType
{
	L_TEXTURE_2D,
	L_TEXTURE_3D,
	L_TEXTURE_CUBE
};

#pragma pack(push, 1)
struct sBitmapParams
{
public:
	explicit sBitmapParams( sEnvironment* E ) : Env( E ),
		FWidth( 0 ),
		FHeight( 0 ),
		FDepth( 0 ),
		FBitmapFormat( L_BITMAP_BGR8 ),
		FTextureType( L_TEXTURE_2D ),
		FAutoGradient( false ),
		FAutoESL( false ) {};
	sBitmapParams( sEnvironment* E,
	               const int Width,
	               const int Height,
	               const int Depth,
	               const LBitmapFormat BitmapFormat,
	               const LTextureType  TextureType );

	/// Returns how many bytes of memory the texture image will take
	int        GetStorageSize() const;

	/// Rreturns how many bits per pixel the texture image takes
	int        GetBitsPerPixel() const;

	/// Rreturns how many bits per pixel the texture image takes
	int        GetBytesPerPixel() const;

	/// Returns 'true' if texture  has 16 or 32 bits per component
	bool       IsFloatFormat() const;

	/// Returns 'true' if texture is DXT-packed
	bool       IsCompressedFormat() const;

	/// Returns params of cube map that can be created from this 2D bitmap assuming it is stored in cross format.
	sBitmapParams  ConvertRecCrossFormatToCubeMap() const;

	static LBitmapFormat SuggestBitmapFormat( sEnvironment* Env, int BitsPerPixel, bool IsFloat );
public:
	sEnvironment*    Env;
	//
	int              FWidth;
	int              FHeight;
	int              FDepth;
	LBitmapFormat    FBitmapFormat;
	LTextureType     FTextureType;
	bool             FAutoGradient;
	bool             FAutoESL;
};
#pragma pack(pop)


/**
   \brief Class for bitmap image representation

   Most of the time bitmaps are assumed to be 2D, but many functions support 3D volumes also
*/
class scriptfinal netexportable clBitmap: public iObject
{
public:
	clBitmap(): FBitmapParams( NULL ),
		FBitmapData( NULL ),
		FSetPixelProc( &clBitmap::SetPixel_Generic ) {};
	clBitmap( const int Width,
	          const int Height,
	          const int Depth,
	          const LBitmapFormat BitmapFormat,
	          const LTextureType  TextureType );
	explicit clBitmap( const sBitmapParams& Params );
private:
	clBitmap( const clBitmap& Other );
	clBitmap& operator = ( const clBitmap& Other );
public:
	virtual ~clBitmap();

	virtual void AfterConstruction() { iObject::AfterConstruction(); FBitmapParams.Env = Env; };

	/// Exchange image data pointers between two images
	void    Swap( clBitmap* Other );

	/// (Re)allocate the image with specified parameters
	bool    ReallocImageData( const sBitmapParams* Params );

	/// Create exact copy of the bitmap
	clBitmap* MakeCopy() const;

	/// Bit-by-bit comparison with Other bitmap. True if the bitmpas coincide
	virtual bool    IsEqual( const clBitmap* Other ) const;

#pragma region Load data from file
	scriptmethod bool    Load2DImage( sEnvironment* Env, const LString& FileName );
	scriptmethod bool    Load2DImageFromStream( sEnvironment* Env, iIStream* Stream );
	scriptmethod bool    Load3DImage( sEnvironment* Env, const LString& FileName ) { return Load3D( Env, FileName, false, false ); }

	/// Load 3D image
	scriptmethod bool    Load3D ( sEnvironment* Env, const LString& FileName, bool Gradients, bool ESL );

	/// Load precomputed ESL map for a given 3D volume file (precompute if it doesn't exist)
	scriptmethod bool    Load3DImageESL( sEnvironment* Env, const LString& FileName ) { return Load3D( Env, FileName, false, true ); }

	/// Load precomputed gradients for a given 3D volume file (precompute if it doesn't exist)
	scriptmethod bool    Load3DImageGradients( sEnvironment* Env, const LString& FileName ) { return Load3D( Env, FileName, true, false ); }

	/// Load cubemap with format autodetection
	scriptmethod bool    LoadCubeImage( sEnvironment* Env, const LString& FileName );

#pragma endregion

#pragma region Save data to file

	/// Save 2d image to file using VFS (Bitmap is not yet an iObject, so we must pass Env here)
	virtual bool    Save2DImage( sEnvironment* Env, const LString& FileName );

	/// Save 3d image to file using VFS (Bitmap is not yet an iObject, so we must pass Env here)
	virtual bool    Save3DImage( sEnvironment* Env, const LString& FileName );

#pragma endregion

#pragma region Special image generation

	/// Render horizontal lines representing the histogram
	scriptmethod void RenderHistogram( LHistogram1D* Hist, const LVector4& Color );

	/// Service function to generate intensity histogram
	scriptmethod clBitmap* RenderIntensityHistogram( int Width, int Height );

	/// Service function to generate the channel histogram
	scriptmethod clBitmap* RenderChannelHistogram( int Width, int Height, int Channel );

	/// Two-dimensional alpha-transparent spot
	scriptmethod void MakeGlow2( float ExposureFactor, float DiscSize );

	/// Calculate normal map for this RGB image
	scriptmethod clBitmap* MakeNormalMap();

	/// Generate 2D noise with specified seed
	scriptmethod void NoiseFill2( int nSeed );

	/// Generate 3D noise with specified seed
	scriptmethod void NoiseFill3( int nSeed );

	/// Generate 2D Voronoi diagram
	noexport     void GenerateVoronoiDiagram( int NumPoints, const LVector2* Points, const LVector4* Colors );

	/// Generate a Voronoi diagram for a random set of points
	scriptmethod void GenerateRandomVoronoiDiagram( int NumPoints );

	/// Generator the (i ^ j ^ k) & 0xFF pattern
	scriptmethod void MakeXorPattern();

	/// Convert Color to ( 1 - exp(Exposure * Color) ). If Out is NULL, "this" is used
	scriptmethod void ApplyExposure( float Exposure, clBitmap* Out );

	/// Make 2D rainbow gradient
	scriptmethod void MakeRainbowGradient( const LVector2& From, const LVector2& To );

	/// Rainbow-colored square
	scriptmethod void FillColorSquare();

	/// Make a single 2D sine wave
	scriptmethod void MakeWave2D( const LVector2& Center, float Ampl, const LVector2& K, float Omega, float t, float Phi, bool Add );

	/// Make a single 3D sine wave
	scriptmethod void MakeWave3D( const LVector3& Center, float Ampl, const LVector3& K, float Omega, float t, float Phi, bool Add );

	/// Fill rectangle/box
	scriptmethod void FillBox( const LVector3& Min, const LVector3& Max, const LVector4& Color );

	/// Fill the triangle with color
	scriptmethod void SolidTriangle( int x1, int y1, int x2, int y2, int x3, int y3, const vec4& Color );

#pragma endregion

#pragma region Image processing

	/// Calculate 8-bit histogram for R,G,B or A channel
	void CalculateChannelHistogram( LHistogram1D* Out, int Channel );

	/// Calculate histogram for intensity (8-bit)
	void CalculateIntensityHistogram( LHistogram1D* Out );

	/// Find minimum and maximum for each channel
	void FindMinMax( LVector4* Min, LVector4* Max );

	/// Convert [min..max] intensities for each channel to [0..1] range. If out is null, then 'this' is used (inplace normalization)
	scriptmethod void NormalizeImage( clBitmap* Out );

	/// Allocate appropriate RGBA image and normalize values
	scriptmethod clBitmap* NormalizeImageAlloc();

	/// Convert X to Log(1+X) for each point/channel
	scriptmethod void LogNormalize( clBitmap* Out );

	/// Allocate RGBA image and make Log normalization
	scriptmethod clBitmap* LogNormalizeAlloc();

	/**
	   \brief Linearize z-buffer values

	   Might be useful to inspect z-buffer in shadow mapping

	   z_linear = b / (z_buffer_value - a)

	   Where:
	   a = zFar / ( zFar - zNear )
	   b = zFar * zNear / ( zNear - zFar )
	*/
	scriptmethod void Linearize32BitZBuffer( float zNear, float zFar );

	/// 3D centroid - "mass center"
	scriptmethod void CalculateCentroid3D( double& Area, double& Xbar, double& Ybar, double& Zbar ) const;

	/// 3D covariance matrix
	scriptmethod LMatrix3 CalculateCovariance3D() const;

	/// mu_{i,j,k} central moments calculation
	scriptmethod void CalculateSecondMoments3D( double& Area, double& Xbar, double& Ybar, double& Zbar, double& m200, double& mu020, double& mu002, double& mu110, double& mu101, double& mu011 ) const;

	/// Make a Hough transform with the image. Out image contains W angles and H different R's
	scriptmethod void HoughTransform2D( clBitmap* Out ) const;

	/// Substract values of Another from This and put it in Out. If Out is null, 'this' is used
	scriptmethod void Substract( clBitmap* Another, clBitmap* Out );

	/// Allocate new bitmap and write the difference of 'this' with Another
	scriptmethod clBitmap* DifferenceWith( clBitmap* Another );

#pragma endregion

#pragma region Bitmap data manipulation
	/**
	   Flips the texture vertically

	   \param Src - source image, with storage properties defined in FBitmapFormat
	   \param Dst - pointer to store destination image with the same size and format
	**/
	void       FlipBitmapVertical( const Lubyte* Src, Lubyte* Dst ) const;

	/**
	   \brief Combine a number of 2D bitmaps to the single atlas

	   Takes Src[] array and tiles them into the [MaxWidth x Height] atlas
	   preserving 'InterDistance' number of pixels between adjacents images.

	   Calculates Offsets[] array for later texture coordinates fixing.

	   The algorithm is in no way space-optimal, it is not an optimal solution of '2d knapsack problem'.

	   It is a simple linear run through the Src[] array. This should give acceptable results
	   when the source images are equally sized.
	*/
	static       clBitmap* CombineImages( const std::vector<clBitmap*>& Src, int MaxWidth, int InterDistance, std::vector<vec2>& Offsets );

	/// Combine a number of images to a single atlas
	void CombineImages_Dense( const std::vector<clBitmap*>& Images, std::vector<vec2>& Offsets );

	/// Invert the image lines ( map 'y' to 'H/2-y')
	scriptmethod void      FlipVertical();

	/// Rotate the image clock-wise 90 degrees
	scriptmethod void      Rotate90CW();

	/// Rotate the image counter clock-wise 90 degrees
	scriptmethod void      Rotate90CCW();

	/// Rotate the image 180 degrees
	scriptmethod void      Rotate180();

	/// Swap R and B components for each pixel
	scriptmethod void      Convert_BGRAToRGBA();

	/// Create a new bitmap with single channel from a given RGB image
	scriptmethod clBitmap* ConvertToGrayscale8bit() const;

	/// Create a new colored 32-bit bitmap from a grayscale one
	scriptmethod clBitmap* ToColorFromGrayscale() const;

	/// Convert floating point image to RGBA8
	scriptmethod clBitmap* FloatToRGBA8() const;

	/// Extract specified color component (0,1,2 - R,G,B)for each pixel and create a new 8-bit image. Can be used to extract R component or Alpha
	scriptmethod clBitmap* ExtractColorPlane8bit( int Idx ) const;

	/// Assign specified color plane from Src bitmap to this
	scriptmethod void  CopyColorPlane( clBitmap* Src, int FromIdx, int ToIdx );

	/// Multiply each channel by the factor
	scriptmethod void  ScalePixelComponents( const LVector4& Factor );

	/**
	   \brief "Tangle"  (R,G) [single complex number] array stored as RGB_float texture to the R_float texture with (R,G) pairs

	   Tangling is used to encode complex-valued array to the non-symmetric (r,g) array
	   suitable for real DFT
	*/
	scriptmethod void  Tangle2D( clBitmap* Out );

	/**
	   \brief "Untangle"  R_float texture with (R,G) pairs to to (R,G) [single complex number] array stored as RGB_float texture

	   This operation is the opposite of Tangle2D()
	*/
	scriptmethod void  Untangle2D( LFFT* fft );

	/// Store sqrt(r * r + g * g) to B channel
	scriptmethod void  CalculateComplexMagnitude();

	/// Store arg( r + i* g ) to A channel
	scriptmethod void  CalculateComplexPhase();

	/// Create new bitmap from this one (extract rectangular region)
	clBitmap*  CopyBitmap( int X1, int Y1, int X2, int Y2 ) const;

	/// Place a bitmap into this one
	void       PutBitmap( int X, int Y, const clBitmap& Other );

	/// Place a bitmap into this one, applying 90 CCW rotation
	void       PutBitmapRotated90CCW( int X, int Y, const clBitmap& Other );

	/// Place a bitmap into this one, applying 90 CW rotation
	void       PutBitmapRotated90CW( int X, int Y, const clBitmap& Other );

	/// Utility wrapper for script/.NET export
	scriptmethod void PutBmp( int X, int Y, clBitmap* Other ) { PutBitmap( X, Y, *Other ); }

	/// Rescales texture
	void       RescaleImage( const Lubyte* Src, Lubyte* Dst, int NewWidth, int NewHeight ) const;
	scriptmethod void       RescaleBitmap( int NewWidth, int NewHeight );

	/// Blend two bitmaps using specified blending operator (see LBlending.h for details)
	template <typename T> void    BlendBitmap( clBitmap* Overlay, T BlendingOp );

	/**
	   Converts 2D texture stored in cross format to cube map.
	   Cross format looks like this:

	        ------
	        | +Y |
	   ----------------
	   | -X | -Z | +X |
	   ----------------
	        | -Y |
	        ------
	        | +Z |
	        ------
	**/
	void       ConvertCrossFormatToCubeMap( const Lubyte* Src, Lubyte* Dst ) const;

	/// Create a 2D image with tiles created from slices of the original image
	scriptmethod clBitmap*  Tile3DImage( int AtlasW, int AtlasH ) const;

#pragma endregion


#pragma region Pixel data access

	/// Offset of the pixel in raw data. Unsupported (incorrect) for compressed formats
	inline int PixelOffset( int X, int Y, int Z ) const
	{
		return FBitmapParams.GetBytesPerPixel() * ( ( Z * FBitmapParams.FHeight + Y ) * FBitmapParams.FWidth + X );
	}

	/// Set pixel value with autoconversion from [0..1]^4 color
	scriptmethod void     SetPixel( int X, int Y, int Z, const LVector4& Color );

	/// Get pixel value with autoconversion to [0..1]^4
	scriptmethod LVector4 GetPixel( int X, int Y, int Z ) const;

	/// Set pixel component (R,G,B,A)
	scriptmethod void  SetPixelComponent( int X, int Y, int Z, int Idx, float Val );

	/// Get pixel component (R,G,B,A)
	scriptmethod float GetPixelComponent( int X, int Y, int Z, int Idx ) const;

	/// Draw the using Bresenham's algorithm
	scriptmethod void DrawLine2D( int x0, int y0, int x1, int y1, const LVector4& Color );

	/// Draw the line in a 3D volume using a Bresenham-like algorithm
	scriptmethod void DrawLine3D( int _x0, int _y0, int _z0, int _x1, int _y1, int _z1, const LVector4& Color );

	/// Draw colored horizontal line
	scriptmethod void HorizontalLine( int y, int x1, int x2, const vec4& Color );

	/// Clear the bitmap using specified color
	scriptmethod void Clear( const LVector4& Color );

	/// Bilinearly filtered GetPixel
	scriptmethod LVector4 BilinearInterpolate( float x, float y );

#pragma endregion

#pragma region For C-sharp and external memory management

	/// Create new empty bitmap
	static clBitmap* CreateEmptyBitmap( sEnvironment* Env );

	/// Create new bitmap with specified dimensions, bpp and type/format
	static clBitmap* CreateBitmap( sEnvironment* Env, int W, int H, int D, LBitmapFormat BMPFormat, LTextureType TextureType );

	/// Overload/Emulation of iObject::DisposeObject()
	virtual void DisposeObject();

#pragma endregion

#pragma region Information accessors

	/// Image width  (x-dimension)
	scriptmethod int  GetWidth()  const { return FBitmapParams.FWidth;  }

	/// Image height (y-dimension)
	scriptmethod int  GetHeight() const { return FBitmapParams.FHeight; }

	/// Image depth  (z-dimension)
	scriptmethod int  GetDepth()  const { return FBitmapParams.FDepth;  }

	/// Bits per pixel
	scriptmethod int  GetBitsPerPixel()  const { return FBitmapParams.GetBitsPerPixel();  }

	/// Check if this bitmap's data is compressed
	scriptmethod bool IsCompressed() const;

	/// Information about texture type
	scriptmethod LTextureType    GetTextureType() const { return FBitmapParams.FTextureType; };

	/// Information about bitmap format
	scriptmethod LBitmapFormat   GetBitmapFormat() const { return FBitmapParams.FBitmapFormat; };

#pragma endregion

public:
	/// The structure with bitmap parameters (format, type, size, bpp)
	sBitmapParams    FBitmapParams;

	/// Actual image data
	Lubyte*          FBitmapData;

private:
	void Bresenham_SetPixel( int x, int y, int Sign, int Inverse, const LVector4& Color );

	/// Internal implementation of Bresenham's 2d line
	void BHM( int x0, int y0, int x1, int y1, int Sign, int Inverse, const LVector4& Color );

#pragma region SetPixel implementations
	void UpdateSetPixelProc();

	void (clBitmap::*FSetPixelProc)( int X, int Y, int Z, const LVector4& Color );

	void     SetPixel_Generic( int X, int Y, int Z, const LVector4& Color );
	void     SetPixel_Grayscale8( int X, int Y, int Z, const LVector4& Color );
#pragma endregion
};

template <typename T> void clBitmap::BlendBitmap( clBitmap* Overlay, T BlendingOp )
{
	if ( FBitmapParams.FWidth  != Overlay->FBitmapParams.FWidth  ||
	     FBitmapParams.FHeight != Overlay->FBitmapParams.FHeight ||
	     FBitmapParams.FDepth  != Overlay->FBitmapParams.FDepth )
	{
		return;
	}

	for ( int i = 0; i != FBitmapParams.FWidth; i++ )
	{
		for ( int j = 0; j != FBitmapParams.FHeight; j++ )
		{
			for ( int k = 0; k != FBitmapParams.FDepth; k++ )
			{
				LVector4 B = GetPixel( i, j, k );
				LVector4 O = Overlay->GetPixel( i, j, k );

				SetPixel( i, j, k, LVector4( BlendingOp( B.X, O.X ),
				                             BlendingOp( B.Y, O.Y ),
				                             BlendingOp( B.Z, O.Z ),
				                             BlendingOp( B.W, O.W ) ) );
			}
		}
	}
};

#endif

/*
 * 02/02/2012
     Rotate90CW(), Rotate90CCW(), Rotate180()
 * 29/01/2011
     Utility methods, ToGreyscale() converter
 * 20/01/2011
     Image generators (noise), normal map calculation, glow texture for particles
 * 09/12/2010
     Copy constructor, virtual destructor, assignment operator
 * 10/02/2010
     Pixel operation
    DXT compression modes
 * 08/02/2010
     IsEqual()
 * 02/02/2010
     It's here
*/
