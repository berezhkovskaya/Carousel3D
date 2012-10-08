/**
 * \file SoftTexture.h
 * \brief Software texture
 * \version 0.5.95
 * \date 11/06/2010
 * \author Viktor Latypov, 2010
 * \author viktor@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clSoftTexture_
#define _clSoftTexture_

#include "Renderer/iTexture.h"
#include "Core/VFS/iIStream.h"
#include "Core/VFS/iOStream.h"

class scriptfinal clSoftTexture: public iTexture
{
public:
	clSoftTexture() {}
	virtual ~clSoftTexture() {}
	//
	// iTexture interface
	//
	virtual void    Bind( int TextureUnit ) const {}
	virtual void    SaveToBitmap( clBitmap* Bitmap ) const {}
	virtual sBitmapParams GetBitmapFormat() const { return sBitmapParams( NULL ); };
	virtual void    LoadFromBitmap( clBitmap* Bitmap ) {}
	virtual void    UpdateSubImageFromBitmap( clBitmap* Bitmap, int X, int Y, int MIPLevel ) {};
	virtual void    SetImage( clImage* TheImage ) {}
	virtual void    CommitChanges() {}
	virtual void    SetMipMapping( const LMipMapping MinFilter, const LMipMapping MagFilter ) {}
	virtual void    SetClamping( const LClamping Clamping ) {}
	virtual void    UpdateMipmaps() {}
	virtual void    SetFormat( Lenum Target, Lenum InternalFormat, Lenum Format, int Width, int Height, int Depth ) {}
	virtual void    AttachToCurrentFB( Lenum Attachment, int ZSlice ) {}
};

#endif

/*
 * 11/06/2010
     Initial version
*/
