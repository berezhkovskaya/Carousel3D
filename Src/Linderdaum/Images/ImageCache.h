/**
 * \file ImageCache.h
 * \brief Image/texture cache
 * \version 0.6.08
 * \date 03/02/2012
 * \author Sergey Kosarevsky, 2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __ImageCache__h__included__
#define __ImageCache__h__included__

#include "Images/Image.h"
#include "Core/iIntrusivePtr.h"
#include "Math/LRect.h"

class clImageCachePage;

struct sCacheEntry
{
	sCacheEntry() : FRect(), FRotated(false), FImage(NULL), FCachePage(0) {};
	sCacheEntry( const LRect& Rect, bool Rotated, clImage* Image, size_t Page ) : FRect(Rect), FRotated(Rotated), FImage(Image), FCachePage(Page) {};
	/// position of this entry within the cache
	LRect FRect;
	/// if the rect was rotated to fit the cache
	bool FRotated;
	/// attached image
	clImage* FImage;
	/// index of the cache page we came from
	size_t FCachePage;
};

class scriptfinal clImageCache: public iObject
{
public:
	clImageCache();
	virtual ~clImageCache();
	//
	// clImageCache
	//
	/**
		Allocate the cache's internal structures, will clear the cache if called more than once
	**/
	virtual void           InitCache( int Width, int Height, LBitmapFormat Format );
	/**
		Set distance (in pixels) between bitmaps in the cache. Affects only bitmaps added after this call.
	**/
	virtual void           SetSeparation( LVector2i S ) { FSeparation = S; }
	/**
		Inserts a bitmap to the cache
	**/
	virtual sCacheEntry    InsertBitmap( clBitmap* Bitmap );
	virtual size_t         GetTotalPages() const { return FPages.size(); };
	virtual clBitmap*      GetPageBitmap( size_t PageNum ) const;
	virtual iTexture*      GetPageTexture( size_t PageNum ) const;
private:
	clPtr<clImageCachePage> InsertPage();
private:
	int           FWidth;
	int           FHeight;
	LBitmapFormat FFormat;
	LVector2i     FSeparation;
	std::vector< clPtr<clImageCachePage> >    FPages;
};

#endif

/*
 * 03/02/2012
     It's here
*/
