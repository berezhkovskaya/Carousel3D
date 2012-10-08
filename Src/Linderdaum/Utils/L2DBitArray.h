/**
 * \file L2DBitArray.h
 * \brief 2D bit array 64x64
 * \version 0.5.96
 * \date 29/10/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _L2DBitArray_
#define _L2DBitArray_

#include "Platform.h"

/// Bitmap for memory allocation mapping
class L2DBitArray64x64
{
public:
	L2DBitArray()
	{
		SetAllBits( false );
	}
	explicit L2DBitArray( bool SetAllBits )
	{
		SetAllBits( SetAllBits );
	}
	inline void SetAllBits( bool SetBits )
	{
		memset( &FBitArray[0], SetBits ? 0xff : 0, sizeof( FBitArray ) );
	}
	inline void SetBit( int X, int Y )
	{
		FBitArray[X] |= ( Bit << Y );
	}
	inline void ClearBit( int X, int Y )
	{
		FBitArray[X] &= ~( Bit << Y );
	}
	inline void SetBitBool( int X, int Y, bool SetBit )
	{
		SetBit ? ( FBitArray[X] |= ( Bit << Y ) ) : ( FBitArray[X] &= ~( Bit << Y ) );
	}
	inline bool IsBitSet( int X, int Y ) const
	{
		return FBitArray[X] & ( Bit << Y );
	}
	inline bool AreAllBitsSet( int X ) const
	{
		return FBitArray[X] == AllBitsSetLine;
	}
private:
	static const Luint64 Bit      = 1;
	static const int     NUM_BITS = 64;
	///
	static const Luint64 AllBitsSetLine = ( Luint64 )( -1 );

	Luint64 FBitArray[ NUM_BITS ];
};

#endif

/*
 * 29/10/2010
     It's here
*/
