/**
 * \file ImageCache.cpp
 * \brief Image/texture cache
 * \version 0.6.08
 * \date 03/02/2012
 * \author Sergey Kosarevsky, 2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"
#include "Utils/Exceptions.h"
#include "Images/ImageCache.h"
#include "Images/Guillotine.h"
#include "Renderer/iTexture.h"

class clImageCachePage: public iObject
{
public:
	clImageCachePage():FBinPack(NULL), FImage(NULL), FDirty( true )
	{
	}
	virtual ~clImageCachePage()
	{
		delete( FBinPack );
	}
	//
	// clImageCachePage
	//
	virtual void    InitPage( int Width, int Height, LBitmapFormat Format )
	{
		LASSERT( Width  > 0 );
		LASSERT( Height > 0 );

		if ( FBinPack ) delete( FBinPack );

		FBinPack = new Guillotine::GuillotineBinPack( Width, Height );
		FImage   = Construct<clImage>( Env );
		FImage->SetBitmap( clBitmap::CreateBitmap( Env, Width, Height, 1, Format, L_TEXTURE_2D ) );
		FImage->GetCurrentBitmap()->Clear( LC_Black );
		FImage->GetTexture()->SetClamping( L_CLAMP_TO_EDGE );
	}
	virtual LRect   RemapRect( const LRect& R )
	{
		float W = 1.0f / FImage->GetCurrentBitmap()->GetWidth();
		float H = 1.0f / FImage->GetCurrentBitmap()->GetHeight();

		return LRect( R.X1() * W, R.Y1() * H, R.X2() * W, R.Y2() * H);
	}
	virtual bool    InsertRect( clBitmap* Bitmap, const LVector2i& Separation, LRect* Rect, bool* Rotated )
	{
		int Width  = Bitmap->GetWidth() + Separation.x;
		int Height = Bitmap->GetHeight() + Separation.y;

		Guillotine::Rect R = FBinPack->Insert( Width, Height, true, 
															Guillotine::GuillotineBinPack::RectBestShortSideFit, 
															Guillotine::GuillotineBinPack::SplitMinimizeArea );

		if ( Rect    ) *Rect    = RemapRect( LRect( R.x, R.y, R.x + R.width - Separation.x, R.y + R.height - Separation.y ) );
		if ( Rotated ) *Rotated = ( R.width == Height && R.height == Width && Width != Height );
		
		bool Success = ( R.width == Width && R.height == Height ) || ( R.width == Height && R.height == Width );

		if ( Success )
		{
			clBitmap* B = FImage->GetCurrentBitmap();

			if ( *Rotated )
			{
				B->PutBitmapRotated90CCW( R.x, R.y, *Bitmap );
			}
			else
			{
				B->PutBitmap( R.x, R.y, *Bitmap );
			}

			FDirty = true;
		}

		return Success;
	}
	virtual clImage* GetImage() const
	{
		if ( FDirty )
		{
			FImage->SendAsync( L_EVENT_CHANGED, LEventArgs(), false );
         FDirty = false;
		}

		return FImage;
	}
private:
	Guillotine::GuillotineBinPack* FBinPack;
	clImage*                       FImage;
	mutable bool                   FDirty;
};

clImageCache::clImageCache()
 : FWidth(-1),
	FHeight(-1),
	FFormat(L_BITMAP_INVALID_FORMAT),
	FSeparation()
{
}

clImageCache::~clImageCache()
{
}

void clImageCache::InitCache( int Width, int Height, LBitmapFormat Format )
{
	LASSERT( FPages.empty() );

	FWidth  = Width;
	FHeight = Height;
	FFormat = Format;

	InsertPage();
}

clPtr<clImageCachePage> clImageCache::InsertPage()
{
	LASSERT( FWidth  > 0 );
	LASSERT( FHeight > 0 );

	clPtr<clImageCachePage> Page( Construct<clImageCachePage>( Env ) );

	Page->InitPage( FWidth, FHeight, FFormat );

	FPages.push_back( Page );

	return Page;
}

sCacheEntry clImageCache::InsertBitmap( clBitmap* Bitmap )
{
	LRect Rect;
	bool Rotated;

	LVector2i Sep( FSeparation );

	// special case
	if ( Bitmap->GetWidth()  == FWidth  ) Sep.x = 0;
	if ( Bitmap->GetHeight() == FHeight ) Sep.y = 0;

	// check all available pages
	for ( size_t i = 0; i != FPages.size(); i++ )
	{
		if ( FPages[i]->InsertRect( Bitmap, Sep, &Rect, &Rotated ) )
		{
			return sCacheEntry( Rect, Rotated, FPages[i]->GetImage(), i );
		}
	}

	size_t PageIdx = FPages.size();

	// place to the new page
	clPtr<clImageCachePage> Page = InsertPage();

	CHECK_ERROR( !Page->InsertRect( Bitmap, Sep, &Rect, &Rotated ), "Unable to insert bitmap into cache" ); 

	return sCacheEntry( Rect, Rotated, Page->GetImage(), PageIdx );
}

clBitmap* clImageCache::GetPageBitmap( size_t PageNum ) const
{
	if ( PageNum >= FPages.size() ) return NULL;

	return FPages[ PageNum ]->GetImage()->GetCurrentBitmap();
}

iTexture* clImageCache::GetPageTexture( size_t PageNum ) const
{
	if ( PageNum >= FPages.size() ) return NULL;

	return FPages[ PageNum ]->GetImage()->GetTexture();
}

/*
 * 03/02/2012
     It's here
*/
