/**
 * \file GLTexture.h
 * \brief OpenGL textures
 * \version 0.5.90
 * \date 04/02/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clGLTexture_
#define _clGLTexture_

#include "Renderer/iTexture.h"

#include "GLAPIContainer.h"

class clGLRenderContext;

class scriptfinal clGLTexture: public iTexture, public clGLAPIContainer
{
public:
	clGLTexture();
	virtual ~clGLTexture();
	//
	// iObject interface
	//
	virtual void    AfterConstruction();
	//
	// iTexture interface
	//
	virtual void    Bind( int TextureUnit ) const;
	virtual void    SaveToBitmap( clBitmap* Bitmap ) const;
	virtual void    LoadFromBitmap( clBitmap* Bitmap );
	virtual void    UpdateFromBitmap( clBitmap* Bitmap );
	virtual void    UpdateSubImageFromBitmap( clBitmap* Bitmap, int X, int Y, int MIPLevel );
	virtual sBitmapParams GetBitmapFormat() const;
	virtual void    SetImage( clImage* TheImage );
	virtual void    SetMipMapping( const LMipMapping MinFilter, const LMipMapping MagFilter );
	virtual void    SetClamping( const LClamping Clamping );
	virtual void    UpdateMipmaps();
	virtual void    SetFormat( Lenum Target, Lenum InternalFormat, Lenum Format, int Width, int Height, int Depth );
	virtual void    AttachToCurrentFB( Lenum Attachment, int ZSlice );
	virtual void    CommitChanges();

	FWD_EVENT_HANDLER( Event_SURFACE_DETACHED );
	FWD_EVENT_HANDLER( Event_SURFACE_ATTACHED );
protected:
	virtual Lenum   GetTextureTarget() const { return FTextureTarget; }
	virtual void    TexSubImage( Luint Level, Lint OffsetX, Lint OffsetY, Luint Width, Luint Height, Luint Depth, Lenum Format, Lenum DataType, const Lubyte* RawPixelData );
	virtual void    TexImage( Lenum InternalFormat, Luint Width, Luint Height, Luint Depth, Lenum Format, Lenum DataType, const Lubyte* RawPixelData );
	virtual void    TexImageCompressed( Lenum InternalFormat, Luint Width, Luint Height, Luint Depth, Luint CompressedSize, const Lubyte* RawPixelData );

#pragma region Save texture in native internal format
	virtual void    GetTexImageCompressed( Lubyte* Image ) const;
	virtual void    GetTexImageCompressedSizeAndInternalFormat( Lint* Size, Lenum* InternalFormat ) const;
#pragma endregion

private:
	Lenum         GetDataFormat( const sBitmapParams& Bitmap ) const;
	void          SetupTexParameters() const;
private:
	Lenum         FTextureTarget;
	Luint         FTexID;
	bool          FCompressed;
	Lenum         FMinFilter;
	Lenum         FMagFilter;
	Lenum         FClamping;
	Luint         FPBO;
	bool          FAttachedToFBO;
	///
	Lenum         FInternalFormat;
	Lenum         FFormat;
	/// stats
	clCVar*       FTextureRebinds;
};

#endif

/*
 * 04/02/2010
     Merged with GLTextures.h
 * 02/02/2010
     SaveToBitmap()
     LoadFromBitmap()
 * 08/03/2007
     ReversedRGB parameter
 * 02/03/2006
     SetMipMapping()
 * 30/03/2005
     TexImage()
     TexImageCompressed()
 * 21/02/2005
     It's here
*/
