/**
 * \file iTexture.h
 * \brief Texture interface
 * \version 0.5.91
 * \date 10/02/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iTexture_
#define _iTexture_

#include "Images/Image.h"

enum LMipMapping
{
	L_NEAREST,
	L_LINEAR,
	L_NEAREST_MIPMAP_NEAREST,
	L_LINEAR_MIPMAP_NEAREST,
	L_NEAREST_MIPMAP_LINEAR,
	L_LINEAR_MIPMAP_LINEAR
};

enum LClamping
{
	L_CLAMP_DONT_CARE,
	L_CLAMP_TO_EDGE,
	L_CLAMP_TO_BORDER,
	L_REPEAT
};

/// Interface to anything bindable to texture unit
class scriptfinal iTexture: public iObject
{
public:
	iTexture(): FAttachedImage( NULL ) {};
	virtual ~iTexture();

	NET_EXPORTABLE()

	//
	// iTexture
	//

	/// Bind texture to specified texture unit
	virtual void        Bind( int TextureUnit ) const = 0;

	/// Write current texture content to the bitmap
	virtual void        SaveToBitmap( clBitmap* Bitmap ) const = 0;

	/// Reload current texture content from the bitmap
	virtual void        LoadFromBitmap( clBitmap* Bitmap ) = 0;

	/** Update current texture content from the bitmap.
	    Bitmap should have the same format as was previosly set with LoadFromBitmap(). Fast uploads are possible.
	**/
	virtual void        UpdateFromBitmap( clBitmap* Bitmap )
	{
		LoadFromBitmap( Bitmap );
	}

	/** Update part of the current texture from the bitmap
		 Bitmap should have the same format as was previosly set with LoadFromBitmap(). Fast uploads are possible.
	**/
	virtual void        UpdateSubImageFromBitmap( clBitmap* Bitmap, int X, int Y, int MIPLevel ) = 0;

	/// Get format for clBitmap
	virtual sBitmapParams GetBitmapFormat() const = 0;

	/// Read texture image from TheImage
	virtual void        SetImage( clImage* TheImage );

	/// Get texture image to clImage. Can be slow.
	virtual clImage*    GetImage() const;

	/// Set mipmapping modes for the texture
	virtual void        SetMipMapping( const LMipMapping MinFilter, const LMipMapping MagFilter ) = 0;

	/// Set clamping mode for the texture
	virtual void        SetClamping( const LClamping Clamping ) = 0;

	/// Regenerate mipmaps. Useful to update the texture dynamicaly
	virtual void        UpdateMipmaps() = 0;

	/// Set format of the texture's data
	virtual void        SetFormat( Lenum Target, Lenum InternalFormat, Lenum Format, int Width, int Height, int Depth ) = 0;

	/// Attach texture to the currently binded framebuffer
	virtual void        AttachToCurrentFB( Lenum Attachment, int ZSlice ) = 0;

	/// Update from attached Image if it changed
	virtual void        CommitChanges() = 0;

private:
	FWD_EVENT_HANDLER( Event_CHANGED );

private:
	clImage*    FAttachedImage;
};

#endif

/*
 * 10/02/2010
     Interface pulled up from iTextureObject
 * 02/02/2010
     SaveToBitmap()
     LoadFromBitmap()
 * 11/12/2008
     L_BITMAP_GRAYSCALE16
 * 19/06/2006
     ConvertCrossFormatToCubeMap()
 * 03/06/2006
     IsFloatFormat()
 * 03/12/2005
     Part of the interface pushed down into iTextureObject
 * 26/11/2005
     FRAWPixelData deprecated
 * 11/01/2005
     AttachBitmap()
 * 03/01/2005
     It's here
*/
