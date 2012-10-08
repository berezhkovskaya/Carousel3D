/**
 * \file ImgLoad.cpp
 * \brief Image loaders
 * \version 0.5.99f
 * \date 06/07/2011
 * \author Sergey Kosarevsky, 2005-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __ImgLoad__h__included__
#define __ImgLoad__h__included__

#include <string.h>

class clBitmap;

namespace Linderdaum
{
	const char* LoadBMP( const void* Buffer, size_t BufferSize, clBitmap* Resource );
	void SaveBMP( void* OutBuffer, size_t MaxBufferSize, const void* RawBGRImage, int Width, int Height, int BitsPP );

	const char* LoadDDS( const void* Buffer, size_t BufferSize, clBitmap* Resource );
}

#endif

/*
 * 06/07/2011
     Stream-agnostic saving/loading to/from byte buffers
*/
