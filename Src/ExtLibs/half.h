///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002, Industrial Light & Magic, a division of Lucas
// Digital Ltd. LLC
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

// Primary authors:
//     Florian Kainz <kainz@ilm.com>
//     Rod Bogart <rgb@ilm.com>
// Copyright (c) 2002, Industrial Light & Magic, a division of Lucas Digital Ltd. LLC

// Modified for Linderdaum Engine by Viktor Latypov <viktor@linderdaum.com>
//  - removed conversion lookup tables
//  - merged into single file
//  - reorganized the code to make it smaller

#ifndef __L_math_half_h
#define __L_math_half_h

#if defined(_MSC_VER) && (_MSC_VER < 1500)
typedef unsigned short uint16_t;
#else
// GCC and VS2010+ has stdint
#include <stdint.h>
#endif

// Smallest positive HalfFloat
#define HALF_MIN  (5.96046448e-08f)
// Smallest positive normalized HalfFloat
#define HALF_NRM_MIN (6.10351562e-05f)
// Largest positive HalfFloat
#define HALF_MAX  (65504.0f)
// Smallest positive e for which HalfFloat (1.0 + e) != HalfFloat (1.0)
#define HALF_EPSILON (0.00097656f)
// Number of digits in mantissa (significand + hidden leading 1)
#define HALF_MANT_DIG   (11)
// Number of base 10 digits that can be represented without change
#define HALF_DIG  (2)
// Base of the exponent
#define HALF_RADIX   (2)
// Minimum negative integer such that HALF_RADIX raised to the power of one less than that integer is a normalized HalfFloat
#define HALF_MIN_EXP (-13)
// Maximum positive integer such that HALF_RADIX raised to the power of one less than that integer is a normalized HalfFloat
#define HALF_MAX_EXP (16)
// Minimum positive integer such that 10 raised to that power is a normalized HalfFloat
#define HALF_MIN_10_EXP (-4)
// Maximum positive integer such that 10 raised to that power is a normalized HalfFloat
#define HALF_MAX_10_EXP (4)

/*!   \brief 16-bit floating point number

   Type HalfFloat can represent positive and negative numbers, whose magnitude is between roughly 6.1e-5 and 6.5e+4,
   with a relative   error of 9.8e-4; numbers smaller than 6.1e-5 can be represented with an absolute error of 6.0e-8.
   All integers from -2048 to +2048 can be represented exactly.
*/
class HalfFloat
{
public:
	//! Construct uninitialized number
	inline HalfFloat() {}

	//! Construct from 32-bit floating point number
	inline HalfFloat( float f )
	{
		// Common special case - zero. For speed, we don't preserve the zero's sign.
		if ( f == 0 )  { _h = 0; return; }

		// We extract the combined sign and exponent, e, from our floating-point number, f.
		//
		// For the most common case, where a normalized HalfFloat is produced,
		// all we have to do, is round f's significand to 10 bits and combine the result with e.
		//
		// For all other cases (overflow, zeroes, denormalized numbers resulting from underflow, infinities and NANs),
		// we call a non-inline function.

		register int e = ( ( *( unsigned int* )( &f ) ) >> 23 ) & 0x000001ff;

		// Simple cases - round the significand and combine it with the sign and exponent.
		if ( e > 112 && e < 142 )
		{
			e = ( ( e - 113 ) + 1 ) << 10;
		}
		else if ( e > 368 && e < 397 )
		{
			e = ( ( e - 369 ) + 33 ) << 10;
		}
		else
		{
			_h = convert ( *( unsigned int* )( &f ) );
			return;
		}

		_h = e + ( ( ( ( *( unsigned int* )( &f ) ) & 0x007fffff ) + 0x00001000 ) >> 13 );
	}

	//! Convert to 32-bit floating point number
	inline operator   float () const { return toFloat(); }

	inline HalfFloat  operator - () const { HalfFloat h; h._h = _h ^ 0x8000; return h; }

	inline HalfFloat& operator = ( HalfFloat h )  { _h = h._h; return *this; }
	inline HalfFloat& operator = ( float f )      { return ( *this = HalfFloat( f ) ); }

	inline HalfFloat& operator += ( HalfFloat h ) { return ( *this = HalfFloat( float( *this ) + float( h ) ) ); }
	inline HalfFloat& operator += ( float f )     { return ( *this = HalfFloat( float( *this ) + f ) ); }
	inline HalfFloat& operator -= ( HalfFloat h ) { return ( *this = HalfFloat( float( *this ) - float( h ) ) ); }
	inline HalfFloat& operator -= ( float f )     { return ( *this = HalfFloat( float( *this ) - f ) ); }
	inline HalfFloat& operator *= ( HalfFloat h ) { return ( *this = HalfFloat( float( *this ) * float( h ) ) ); }
	inline HalfFloat& operator *= ( float f )     { return ( *this = HalfFloat( float( *this ) * f ) ); }
	inline HalfFloat& operator /= ( HalfFloat h ) { return ( *this = HalfFloat( float( *this ) / float( h ) ) ); }
	inline HalfFloat& operator /= ( float f )     { return ( *this = HalfFloat( float( *this ) / f ) ); }

	// Round to n-bit precision (n should be between 0 and 10).
	// After rounding, the significand's 10-n least significant bits will be zero.
	inline HalfFloat round( unsigned int n ) const
	{
		// Parameter check.
		if ( n >= 10 ) { return *this; }

		// Disassemble h into the sign, s and the combined exponent and significand, e.
		unsigned short s = _h & 0x8000, e = _h & 0x7fff;

		// Round the exponent and significand to the nearest value where ones occur only in the (10-n) most significant bits.
		// Note that the exponent adjusts automatically if rounding up causes the significand to overflow.
		e >>= 9 - n;
		e  += e & 1;
		e <<= 9 - n;

		// Check for exponent overflow.
		if ( e >= 0x7c00 )
		{
			// Overflow occurred -- truncate instead of rounding.
			e = _h;
			e >>= 10 - n;
			e <<= 10 - n;
		}

		// Put the original sign bit back.
		HalfFloat h;
		h._h = s | e;
		return h;
	}

	inline bool isNormalized()   const { unsigned short e = ( _h >> 10 ) & 0x001f; return e > 0 && e < 31; }
	inline bool isDenormalized() const { return ( ( ( _h >> 10 ) & 0x001f ) == 0 )  && ( ( _h & 0x3ff ) != 0 ); }

	inline bool isNaN() const      { return ( ( ( _h >> 10 ) & 0x001f ) == 31 ) && ( ( _h & 0x3ff ) != 0 ); }
	inline bool isInfinite() const { return ( ( ( _h >> 10 ) & 0x001f ) == 31 ) && ( ( _h & 0x3ff ) == 0 ); }
	inline bool isFinite() const   { return ( ( ( _h >> 10 ) & 0x001f )  < 31 ); }

	inline bool isNegative() const { return ( _h & 0x8000 ) != 0; }
	inline bool isZero() const     { return ( _h & 0x7fff ) == 0; }

	static inline HalfFloat posInf() { HalfFloat h; h._h = 0x7c00; return h; }
	static inline HalfFloat negInf() { HalfFloat h; h._h = 0xfc00; return h; }
	static inline HalfFloat qNaN()   { HalfFloat h; h._h = 0x7fff; return h; }
	static inline HalfFloat sNaN()   { HalfFloat h; h._h = 0x7dff; return h; }

private:
	float toFloat() const
	{
		float fl_f;
		unsigned int& fl_i = *( unsigned int* )( &fl_f );

		unsigned int s = ( _h >> 15 ) & 0x00000001, e = ( _h >> 10 ) & 0x0000001f, m = _h & 0x000003ff;

		// signed zero
		if ( e == 0 && m == 0 ) { fl_i = s << 31; return fl_f; }

		if ( e == 0 )
		{
			// renormalize number
			while ( !( m & 0x00000400 ) ) { m <<= 1; e -=  1; }

			e += 1;
			m &= ~0x00000400;
		}
		else if ( e == 31 )
		{
			// infinity or NaN -- preserve sign and significand bits
			fl_i = ( s << 31 ) | 0x7f800000 | ( m << 13 );
		}
		else
		{
			// Normalized number. Assemble s, e and m.
			fl_i = ( s << 31 ) | ( ( e + ( 127 - 15 ) ) << 23 ) | ( m << 13 );
		}

		return fl_f;
	}

	static short convert( int i )
	{
		// Shift s into the position where it will go in in the resulting HalfFloat number.
		// Adjust e, accounting for the different exponent bias of float and HalfFloat (127 versus 15).
		register int s =  ( i >> 16 ) & 0x00008000, e = ( ( i >> 23 ) & 0x000000ff ) - ( 127 - 15 ), m = i & 0x007fffff;

		// Assemble the HalfFloat from s, e (zero) and m.
		if ( e <= 0 )
		{
			// E is less than -10.  The absolute value of f is less than HALF_MIN.
			if ( e < -10 ) { return 0; }

			// E is between -10 and 0.  F is a normalized float, whose magnitude is less than HALF_NRM_MIN.
			// We convert f to a denormalized HalfFloat.
			m = ( m | 0x00800000 ) >> ( 1 - e );

			// Round to nearest, round "0.5" up.
			// Rounding may cause the significand to overflow and make our number normalized.
			// Because of the way a HalfFloat's bits are laid out,
			// we don't have to treat this case separately; the code below will handle it correctly.
			if ( m &  0x00001000 ) { m += 0x00002000; }

			return s | ( m >> 13 );
		}
		else if ( e == 0xff - ( 127 - 15 ) )
		{
			// F is an infinity; convert f to a HalfFloat infinity with the same sign as f.
			if ( m == 0 ) { return s | 0x7c00; }

			// F is a NAN; we produce a HalfFloat NAN that preserves the sign bit and the 10 leftmost bits of the
			// significand of f, with one exception: If the 10 leftmost bits are all zero, the NAN would turn
			// into an infinity, so we have to set at least one bit in the significand.
			m >>= 13;
			return s | 0x7c00 | m | ( m == 0 );
		}
		else
		{
			// E is greater than zero. F is a normalized float. We try to convert f to a normalized HalfFloat.

			// Round to nearest, round "0.5" up
			if ( m &  0x00001000 )
			{
				m += 0x00002000;

				// overflow in significand, adjust exponent
				if ( m & 0x00800000 ) { m =  0; e += 1; }
			}

			// Handle exponent overflow. Cause a hardware floating point overflow;
			// if overflow() returns, the HalfFloat becomes an infinity with the same sign as f.
			if ( e > 30 ) { overflow (); return s | 0x7c00; }
			else { return s | ( e << 10 ) | ( m >> 13 ); }
		}
	}

	// Overflow handler for float-to-HalfFloat conversion;
	// generates a hardware floating-point overflow, which may be trapped by the operating system.
	static float overflow()
	{
		volatile float f = 1e10;

		for ( int i = 0; i < 10; i++ ) { f *= f; } // this will overflow before the forloop terminates

		return f;
	}

	unsigned short _h;
};

typedef HalfFloat flt16;

#endif
