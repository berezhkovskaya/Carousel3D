/**
 * \file LBlending.h
 * \brief Typical bitmaps blending operators
 * \version 0.5.91
 * \date 26/08/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LBlending_
#define _LBlending_

#include "Math/LMath.h"

namespace Linderdaum
{
	/// Linderdaum Math Library
	namespace Math
	{
		inline float Blend_Normal( float Base, float Overlay )
		{
			return Base;
		}
		inline float Blend_Lighten( float Base, float Overlay )
		{
			return ( Overlay > Base ) ? Overlay : Base;
		}
		inline float Blend_Darken( float Base, float Overlay )
		{
			return ( Overlay > Base ) ? Base : Overlay;
		}
		inline float Blend_Multiply( float Base, float Overlay )
		{
			return Base * Overlay;
		}
		inline float Blend_Average( float Base, float Overlay )
		{
			return ( Base + Overlay ) / 2.0f;
		}
		inline float Blend_Add( float Base, float Overlay )
		{
			return LMin( Base + Overlay, 1.0f );
		}
		inline float Blend_Subtract( float Base, float Overlay )
		{
			return LMax( Base + Overlay - 1.0f, 0.0f );
		}
		inline float Blend_Difference( float Base, float Overlay )
		{
			return fabs( Base - Overlay );
		}
		inline float Blend_Negation( float Base, float Overlay )
		{
			return 1.0f - fabs( 1.0f - Base - Overlay );
		}
		inline float Blend_Screen( float Base, float Overlay )
		{
			return 1.0f - ( 1.0f - Base ) * ( 1.0f - Overlay );
		}
		inline float Blend_Exclusion( float Base, float Overlay )
		{
			return Base + Overlay - 2 * Base * Overlay;
		}
		inline float Blend_Overlay( float Base, float Overlay )
		{
			return ( Overlay < 0.5f ) ? ( 2.0f * Base * Overlay ) : ( 2.0f * Base - 1.0f ) * ( 1.0f - Overlay );
		}
		inline float Blend_SoftLight( float Base, float Overlay )
		{
			return ( Overlay < 0.5f ) ? ( Base + 0.5f ) * Overlay : ( Base - 0.5f ) * ( 1.0f - Overlay );
		}
		inline float Blend_HardLight( float Base, float Overlay )
		{
			return Blend_Overlay( Overlay, Base );
		}
		inline float Blend_ColorDodge( float Base, float Overlay )
		{
			return ( Overlay > 1.0f - Math::EPSILON ) ? Overlay : LMin( 1.0f, Base / ( 1.0f - Overlay ) );
		}
		inline float Blend_ColorBurn( float Base, float Overlay )
		{
			return ( Overlay < Math::EPSILON ) ? Overlay : LMax( 0.0f, 1.0f - ( 1.0f - Base ) / Overlay );
		}
		inline float Blend_LinearDodge( float Base, float Overlay )
		{
			return Blend_Add( Base, Overlay );
		}
		inline float Blend_LinearBurn( float Base, float Overlay )
		{
			return Blend_Subtract( Base, Overlay );
		}
		inline float Blend_LinearLight( float Base, float Overlay )
		{
			return ( Overlay < 0.5f ) ? Blend_LinearBurn( Base, 2 * Overlay ) : Blend_LinearDodge( Base, ( 2 * ( Overlay - 0.5f ) ) );
		}
		inline float Blend_VividLight( float Base, float Overlay )
		{
			return ( Overlay < 0.5f ) ? Blend_ColorBurn( Base, 2 * Overlay ) : Blend_ColorDodge( Base, ( 2 * ( Overlay - 0.5f ) ) );
		}
		inline float Blend_PinLight( float Base, float Overlay )
		{
			return ( Overlay < 0.5f ) ? Blend_Darken( Base, 2 * Overlay ) : Blend_Lighten( Base, ( 2 * ( Overlay - 0.5f ) ) );
		}
		inline float Blend_HardMix( float Base, float Overlay )
		{
			return ( Blend_VividLight( Base, Overlay ) < 0.5f ) ? 0.0f : 1.0f;
		}
		inline float Blend_Reflect( float Base, float Overlay )
		{
			return ( Overlay  > 1.0f - Math::EPSILON ) ? Overlay : LMin( 1.0f, Base * Base / ( 1.0f - Overlay ) );
		}
		inline float Blend_Glow( float Base, float Overlay )
		{
			return Blend_Reflect( Overlay, Base );
		}
		inline float Blend_Phoenix( float Base, float Overlay )
		{
			return LMin( Base, Overlay ) - LMax( Base, Overlay ) + 1.0f;
		}
	}
}

#endif

/*
 * 26/08/2010
     It's here
*/
