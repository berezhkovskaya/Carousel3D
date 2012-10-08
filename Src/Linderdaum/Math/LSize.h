/**
 * \file LSize.h
 * \brief 2D rectangle size
 * \version 0.6.08
 * \date 19/02/2012
 * \author Sergey Kosarevsky, 2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __LSize__h__included__
#define __LSize__h__included__

#include "LMath.h"

/// 2D rectangle size (usually represent the size of a 2D screen area)
class LSize
{
public:
	LSize() : FSize() {};
	LSize( const LSize& Size ) : FSize( Size.FSize ) {};

	explicit LSize( float Value ) : FSize( Value ) {};
	explicit LSize( const LVector2& Size ) : FSize( Size ) {};

	LSize( float Width, float Height ) : FSize( Width, Height ) {};
	LSize( int Width, int Height ) : FSize( static_cast<float>(Width), static_cast<float>(Height) ) {};

	inline float    GetWidth() const  { return fabs( FExtents.Z - FExtents.X ); };
	inline float    GetHeight() const { return fabs( FExtents.W - FExtents.Y ); };

	inline void     SetWidth( float Width ) { FSize.X = Width; };
	inline void     SetHeight( float Height ) { FSize.Y = Height; };

	inline const LVector2& ToVector2() const { return FSize; };
	inline       LVector2& ToVector2()       { return FSize; };
	inline const float* ToFloatPtr() const { return FSize.ToFloatPtr(); };
	inline float*       ToFloatPtr()       { return FSize.ToFloatPtr(); };
public:
	/// X:Width, Y:Height
	LVector2 FSize;
};

#endif

/*
 * 19/02/2012
     It's here
*/
