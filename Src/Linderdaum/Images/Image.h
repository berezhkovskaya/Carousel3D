/**
 * \file Image.h
 * \brief Generic image container
 * \version 0.5.93
 * \date 19/11/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __Image__h__included__
#define __Image__h__included__

#include "Platform.h"
#include "LString.h"

#include "Core/iObject.h"
#include "Images/Bitmap.h"
#include "Resources/iResource.h"

class iTexture;
class clRenderState;
class iShaderProgram;

class netexportable scriptfinal clImage: public iResource
{
public:
	clImage();
	virtual ~clImage();

	NET_EXPORTABLE()

	//
	// iObject interface
	//
	virtual void AfterConstruction();

	//
	// iCacheable interface
	//
	virtual LString GetCachingDir() const;
	//
	// clImage
	//
	virtual clBitmap* GetDefaultBitmap() const;
	virtual clBitmap* GetCurrentBitmap() const;
	virtual clBitmap* GetCurrentBitmapLocked() { WaitLoad(); return GetCurrentBitmap(); };

	/// Make a copy of the image
	virtual clImage* CloneImage() const;

	/// change current bitmap
	virtual void SetBitmap( clBitmap* NewBitmap );

	virtual iTexture* GetTexture() const;

	/// Process this image with a given shader program / render state. If Output is NULL, then 'this' is used
	virtual bool ProcessWithShader( clRenderState* State, clImage* Output );

	/// Process this image with a given shader program (if no supplementary textures are needed);
	virtual bool ProcessWithShaderProgram( iShaderProgram* SP, clImage* Output );

	/// a lot of utility methods:
	/// for a volume we can call volume_proc,
	/// for a 2d image we can apply some filter or extract features

	virtual void CommitChanges();
protected:
	virtual bool LoadFromFile( const LString& FileName );
	virtual bool UnloadResource();
private:
	/// empty bitmap to insure the clImage validity
	clBitmap*    FDefaultBitmap;

	/// the pointer to the used bitmap
	clBitmap*    FCurrentBitmap;

	/// lazy-binded texture
	mutable iTexture*    FTexture;

	clMutex    FBitmapMutex;
	clMutex    FTextureMutex;
};

/*
class clStaticImage: public clImage
{
public:
   virtual bool IsCurrentImageChanged() const
   {
       if ( FProcess->IsRunning() )
       {
       }
   }
private:
   // iImageLoadingProcess *FProcess; // loads something into FLoadingBitmap

   clBitmap*    FLoadingBitmap;
};


// to use the complete audio/video file we should manage clVideoImage and some audiostream in a wrapper class

class clVideoImage: public clImage
{
public:
   clVideoImage();
   virtual ~clVideoImage();

   // create appropriate decompressor rand allocate decompression buffers
   void LoadVideo();

   // maybe, add some compression here ?
   // void StartVideo, AddFrame, FinishVideo ?

private:
   iVideoDecoder*    FVideoDecoder;
   int               FCurrentFrame;
   float             FFrameTime;
   Lubyte*           FFlippedFrame;
};
*/

#endif

/*
 * 19/11/2010
     GetTexture()
 * 02/02/2010
     It's here
*/
