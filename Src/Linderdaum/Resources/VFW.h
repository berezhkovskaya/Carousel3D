/*
   (Part of Linderdaum Engine)
   Version 0.5.89
   (15/02/2009)
   (C) Sergey Kosarevsky, 2009
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clVFWVideoEncoder_
#define _clVFWVideoEncoder_

#include "Platform.h"
#include "Resources/iVideoEncoder.h"
#include "Resources/iVideoDecoder.h"

#ifdef OS_WINDOWS

#  include <windows.h>
#  undef min
#  undef max

#  ifdef L_VIDEO_USE_VFW
#    include <vfw.h>
#  endif // L_VIDEO_USE_VFW

#endif // OS_WINDOWS

/// Video encoder implemented via Video For Windows
class scriptfinal clVFWVideoEncoder: public iVideoEncoder
{
public:
	clVFWVideoEncoder();
	virtual ~clVFWVideoEncoder();
	//
	// iVideoEncoder interface
	//
	virtual void      AddVideoFrame( void* Frame );
	virtual bool      CreateStream( const LString& FileName, int Width, int Height, int FrameRate );
private:
#ifdef L_VIDEO_USE_VFW
	BITMAPINFOHEADER FBMIH;
	PAVIFILE         FAVIFile;
	PAVISTREAM       FAVIStream;
	PAVISTREAM       FAVICompStream;
	int              FFrame;
#endif
};

/// Video decoder implemented via Video For Windows
class scriptfinal clVFWVideoDecoder: public iVideoDecoder
{
public:
	clVFWVideoDecoder();
	virtual ~clVFWVideoDecoder();
	//
	// iVideoDecoder interface
	//
	virtual int       GetMaxFrames() const;
	virtual void*     ExtractFrame( int FrameNum );
	virtual float     GetFrameSeconds() const { return FFrameSeconds; }
	virtual int       GetWidth() const { return FWidth; }
	virtual int       GetHeight() const { return FHeight; }
	virtual void      OpenStream( const LString& FileName );
private:
	int              FWidth;
	int              FHeight;
	float            FFrameSeconds;
#ifdef L_VIDEO_USE_VFW
	AVISTREAMINFO    FPSI;     // Pointer To A Structure Containing Stream Info
	PAVISTREAM       FAVI;     // Handle To An Open Stream
	PGETFRAME        FPGF;     // Pointer To A GetFrame Object
	BITMAPINFOHEADER FBMIH;    // Header Information For DrawDibDraw Decoding
	long             FLastFrame;
	int              FInternalWidth;
	int              FInternalHeight;
	char*            FPData;   // Pointer To Texture Data
	int              FMPF;     // Will Hold Rough Milliseconds Per Frame
	HDRAWDIB         FHDD;     // Handle For Our Dib
	HBITMAP          FBitmap;  // Handle To A Device Dependant Bitmap
	HDC              FDC;      // Creates A Compatible Device Context
	unsigned char*   FData;    // Pointer To Our Resized Image
#endif // L_VIDEO_USE_VFW
};

#endif

/*
 * 15/02/2009
     It's here
*/
