/**
 * \file LRect.h
 * \brief Rectangle
 * \version 0.5.99d
 * \date 26/04/2011
 * \author Sergey Kosarevsky, 2010-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __LRect__h__included__
#define __LRect__h__included__

#include "LMath.h"

/// 2D rectangle (usually represent a 2D screen area)
class LRect
{
public:
	LRect() : FExtents() {};
	LRect( const LRect& Rect ) : FExtents( Rect.FExtents ) {};
	explicit LRect( float Value ) : FExtents( Value ) {};
	explicit LRect( const LVector4& Extents ) : FExtents( Extents ) {};
	LRect( float X1, float Y1 ) : FExtents( X1, Y1, 1.0f, 1.0f ) {};
	LRect( float X1, float Y1, float X2, float Y2 ) : FExtents( X1, Y1, X2, Y2 ) {};
	LRect( int X1, int Y1, int X2, int Y2 ) : FExtents( static_cast<float>(X1), static_cast<float>(Y1), static_cast<float>(X2), static_cast<float>(Y2) ) {};

	inline float X1() const { return FExtents.X; };
	inline float Y1() const { return FExtents.Y; };
	inline float X2() const { return FExtents.Z; };
	inline float Y2() const { return FExtents.W; };
	inline float& X1() { return FExtents.X; };
	inline float& Y1() { return FExtents.Y; };
	inline float& X2() { return FExtents.Z; };
	inline float& Y2() { return FExtents.W; };

	inline float    GetWidth() const  { return fabs( FExtents.Z - FExtents.X ); };
	inline float    GetHeight() const { return fabs( FExtents.W - FExtents.Y ); };

	inline void     SetWidth( float Width ) { FExtents.Z = FExtents.X + Width; };
	inline void     SetHeight( float Height ) { FExtents.W = FExtents.Y + Height; };

	inline float    GetCenterX() const { return ( FExtents.X + FExtents.Z ) * 0.5f; };
	inline float    GetCenterY() const { return ( FExtents.Y + FExtents.W ) * 0.5f; };
	inline LVector2 GetCenter() const { return LVector2( GetCenterX(), GetCenterY() ); };

	/// fit Other rect inside this rect respecting aspect ratio of Other
	LRect Fit( const LRect& Other )
	{
		float OldWidth  = Other.GetWidth();
		float OldHeight = Other.GetHeight();

		float Aspect1 = OldWidth  / GetWidth();
		float Aspect2 = OldHeight / GetHeight();

		float Aspect = Linderdaum::Math::LMax( Aspect1, Aspect2 );

		LRect Result( Other );

		Result.SetWidth(  OldWidth  / Aspect );
		Result.SetHeight( OldHeight / Aspect );

		return Result;
	}

	/// center Other rect inside this rect
	LRect Center( const LRect& Other )
	{
		float W = Other.GetWidth();
		float H = Other.GetHeight();

		LVector2 C = GetCenter();

		float X = C.X - W / 2;
		float Y = C.Y - H / 2;

		return LRect( X, Y, X + W, Y + H );
	}

	/// construct a LRect enclosing this and Other assuming FixOrder() has been already called on both
	void    Combine( const LRect& Other )
	{
		FExtents.X = Linderdaum::Math::LMin( FExtents.X, Other.FExtents.X );
		FExtents.Z = Linderdaum::Math::LMax( FExtents.Z, Other.FExtents.Z );

		FExtents.Y = Linderdaum::Math::LMin( FExtents.Y, Other.FExtents.Y );
		FExtents.W = Linderdaum::Math::LMax( FExtents.W, Other.FExtents.W );
	}

	void    MoveTo( const LVector2& LeftTop )
	{
		/*
		      float W = GetWidth();
		      float H = GetHeight();

		      FExtents.X = LeftTop.X;
		      FExtents.Y = LeftTop.Y;

		      FExtents.Z = FExtents.X + W;
		      FExtents.W = FExtents.Y + H;
		*/
		FExtents = LVector4( LeftTop.X, LeftTop.Y, LeftTop.X + GetWidth(), LeftTop.Y + GetHeight() );
	}

	void    MoveRel( const LVector2& Delta ) { FExtents += LVector4( Delta.X, Delta.Y, Delta.X, Delta.Y ); }

	/// Check if the Point is inside this rectangle
	bool    ContainsPoint( const LVector2& Point ) const
	{
		return Point.X >= FExtents.X && Point.Y >= FExtents.Y && Point.X <= FExtents.Z && Point.Y <= FExtents.W;
	}

	/// try to align coords (X, Y) to the coords of this rect
	void    DockCoordsToThisRect( float* X, float* Y, const float W, const float H, float DockingDistance ) const
	{
		if ( fabsf( *X - FExtents.X ) < DockingDistance ) { *X = FExtents.X; }

		if ( fabsf( *Y - FExtents.Y ) < DockingDistance ) { *Y = FExtents.Y; }

		if ( fabsf( *X - FExtents.Z ) < DockingDistance ) { *X = FExtents.Z; }

		if ( fabsf( *Y - FExtents.W ) < DockingDistance ) { *Y = FExtents.W; }

		if ( fabsf( *X + W - FExtents.X ) < DockingDistance ) { *X = FExtents.X - W; }

		if ( fabsf( *Y + H - FExtents.Y ) < DockingDistance ) { *Y = FExtents.Y - H; }

		if ( fabsf( *X + W - FExtents.Z ) < DockingDistance ) { *X = FExtents.Z - W; }

		if ( fabsf( *Y + H - FExtents.W ) < DockingDistance ) { *Y = FExtents.W - H; }
	}

	/// try to align size (W, H) to the coords of this rect
	void    DockSizeToThisRect( const float X, const float Y, float* W, float* H, float DockingDistance ) const
	{
		if ( fabsf( X + *W - FExtents.X ) < DockingDistance ) { *W = FExtents.X - X; }

		if ( fabsf( Y + *H - FExtents.Y ) < DockingDistance ) { *H = FExtents.Y - Y; }

		if ( fabsf( X + *W - FExtents.Z ) < DockingDistance ) { *W = FExtents.Z - X; }

		if ( fabsf( Y + *H - FExtents.W ) < DockingDistance ) { *H = FExtents.W - Y; }
	}

	/// ensure X2>=X1 and Y2>=Y1
	void    FixOrder()
	{
		if ( FExtents.X > FExtents.Z ) { Linderdaum::Math::SwapValues( FExtents.X, FExtents.Z ); }

		if ( FExtents.Y > FExtents.W ) { Linderdaum::Math::SwapValues( FExtents.Y, FExtents.W ); }
	}

	inline const LVector4& ToVector4() const { return FExtents; };
	inline       LVector4& ToVector4()       { return FExtents; };
	inline const float* ToFloatPtr() const { return FExtents.ToFloatPtr(); };
	inline float*       ToFloatPtr()       { return FExtents.ToFloatPtr(); };
public:
	/// X:X1, Y:Y1, Z:X2, W:Y2
	LVector4 FExtents;
};

#endif

/*
 * 26/04/2011
     Fit()
 * 21/10/2010
     It's here
*/
