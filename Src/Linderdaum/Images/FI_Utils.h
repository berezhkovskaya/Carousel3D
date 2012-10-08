/**
 * \file FI_Utils.h
 * \brief FreeImage binding code
 * \version 0.5.93
 * \date 18/09/2010
 * \author Sergey Kosarevsky, 2008-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */


#ifndef _FI_Utils_h_
#define _FI_Utils_h_

#include "Platform.h"
#include "LString.h"

class clBitmap;
class iIStream;

namespace Linderdaum
{
	/// Unload FreeImage library
	void FreeImage_Unload();

	/// Load image from stream into the resource with optopnal vertical flip
	bool FreeImage_LoadFromStream( iIStream* IStream, clBitmap* Resource, bool DoFlipV );

	/// Save screenshot with 24 bits per pixel (image file format is defined iternally)
	void FreeImage_SaveScreenshot( sEnvironment* Env, const LString& FileName, Lubyte* RawBGRImage, int Width, int Height );

	/// Save screenshot with a specific number of bits per pixel (image file format is defined iternally)
	void FreeImage_SaveScreenshot_BPP( sEnvironment* Env, const LString& FileName, Lubyte* RawBGRImage, int Width, int Height, int BitsPP );
};

#endif

/*
 * 02/04/2008
     Log section added
*/
