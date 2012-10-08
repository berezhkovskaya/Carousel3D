/**
 * \file LMath.h
 * \brief
 * \version 0.5.91
 * \date 16/09/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LMath_
#define _LMath_

#include "LVector.h"

#include <cmath>

#ifndef scriptexport
#define scriptexport(X)
#endif

#ifndef scriptmethod
#define scriptmethod
#endif

namespace Linderdaum
{
	/// Linderdaum Math Library
	namespace Math
	{
#undef INFINITY
		const float INFINITY     = 1e30f;
		const float EPSILON      = 1e-8f;
		const float PI           = 3.14159265358979323846f;
		const float PI2          = 2.0f * PI;
		const float TWOPI        = PI2;
		const float HALFPI       = PI / 2.0f;
		const float SQRT2        = 1.41421356f;
		const float DTOR         = PI / 180.0f;
		const float RTOD         = 180.0f / PI;
		const float LOGHALF      = -0.693147f; // log(0.5)
		const float LOGHALFI     = -1.442695f;  // Inverse of log(0.5)

		inline float LAbs( float A )
		{
			return ( A > 0.0f ) ? A : -A;
		}

		inline float LMin( float A, float B )
		{
			return ( A < B ) ? A : B;
		}

		inline float LMax( float A, float B )
		{
			return ( A > B ) ? A : B;
		}

		inline float    DegToRad( float F )
		{
			return F * DTOR;
		}

		inline float    RadToDeg( float F )
		{
			return F * RTOD;
		}

		template <typename T> void SwapValues( T& V1, T& V2 )
		{
			T Tmp = V1;
			V1 = V2;
			V2 = Tmp;
		}

		template <typename T> T Clamp( const T Value, const T Min, const T Max )
		{
			if ( Value > Max ) { return Max; }

			if ( Value < Min ) { return Min; }

			return Value;
		}

		template <typename T> T ClampPeriodic( const T Value, const T Min, const T Max )
		{
		}

#pragma region Misc functions usefull in texture generation

		template <class T> T Step( T a, T x )          { return ( T )( x >= a ); }
		template <class T> T Boxstep( T a, T b, T x )     { return Clamp( ( x - a ) / ( b - a ), 0, 1 ); }
		template <class T> T Pulse( T a, T b, T x )    { return ( T )( ( x >= a ) - ( x >= b ) ); }
		template <class T> T Cubic( T a )              { return a * a * ( 3 - 2 * a ); }
		template <class T> T Lerp( T a, T b, T x )     { return ( a + x * ( b - a ) ); }

		inline int Floor( float a )                 { return ( ( int )a - ( a < 0 && a != ( int )a ) ); }
		inline int Ceil( float a )                  { return ( ( int )a + ( a > 0 && a != ( int )a ) ); }

		inline float Gamma( float a, float g )         { return powf( a, 1 / g ); }

		inline float Bias( float a, float b )          { return powf( a, logf( b ) * LOGHALFI ); }

		inline float Exposure( float l, float k )        { return ( 1 - expf( -l * k ) ); }

		inline float Gain( float a, float b )
		{
			if ( a <= EPSILON ) { return 0; }

			if ( a >= 1.0f - EPSILON ) { return 1; }

			float p = ( logf( 1.0f - b ) * LOGHALFI );

			return ( ( a < 0.5 ) ? powf( 2.0f * a, p ) : 1.0f - powf( 2.0f * ( 1.0f - a ), p ) ) * 0.5f;
		}

		/// Cubically-interpolated "smooth" transition from a to b
		inline float SmoothStep( float a, float b, float x )
		{
			if ( x <= a ) { return 0; }

			if ( x >= b ) { return 1; }

			return Cubic( ( x - a ) / ( b - a ) );
		}

#pragma endregion

		/// Floating-point division remainder
		inline float Mod( float a, float b )
		{
			a -= b * floor( a / b );

			if ( a < 0 )
			{
				a += b;
			}

			return a;
		}

		inline bool    IsPowerOf2( const int Num )
		{
			return ( Num & ( Num - 1 ) ) == 0;
		}

		inline unsigned int    GetNextPowerOf2( unsigned int Num )
		{
			Num |= ( Num >> 1 );
			Num |= ( Num >> 2 );
			Num |= ( Num >> 4 );
			Num |= ( Num >> 8 );
			Num |= ( Num >> 16 );

			return Num + 1;
			/*
			         if ( IsPowerOf2( Num ) )
			         {
			            return Num;
			         }

			         float lg2 = log( static_cast<float>( Num ) ) / 0.6931471f;
			         return static_cast<int>( pow( 2.0f, static_cast<int>( ceil( lg2 ) ) ) );
			*/
		}

		inline bool IsMaskSet( int Value, int Mask )
		{
			return ( Value & Mask ) == Mask;
		}

		inline bool IsBitSet( int Value, int BitNumber )
		{
			int Mask = 1 << BitNumber;

			return ( Value & Mask ) == Mask;
		}

		inline int SetBit( int Value, int BitNumber )
		{
			int Mask = 1 << BitNumber;

			return Value | Mask;
		}

		inline int    IntPow( int A, int B )
		{
			return static_cast<int>( pow( static_cast<float>( A ), B ) );
		}

		inline LVector3 SphericalToOrtho( float Radius, float Phi, float Theta )
		{
			return LVector3( Radius * cosf( Phi   * DTOR ),
			                 Radius * sinf( Phi   * DTOR ),
			                 Radius * sinf( Theta * DTOR ) );
		}

		template<class T>
		inline T Sign( T Val )
		{
			return ( Val > 0 ) ? Val : -Val;
		}

		inline float fsign( float Val )
		{
			return ( Val > 0.0f ) ? 1.0f : -1.0f;
		}

		inline float ClipAngleTo0_360( float Angle )
		{
			/*         double IntPart = 0;
			         double ClampedPart = modf( static_cast<double>(Angle), &IntPart );

			         return static_cast<float>( ClampedPart * 360.0 );
			*/
			return std::fmod( Angle, 360.0f );
		}

		inline float ClipAngleTo180( float angle )
		{
			return ClipAngleTo0_360( angle ) - 180.0f;
		}

		inline double Round( double x, int Digits )
		{
			double Power = pow( 10.0, Digits );

			return static_cast<double>( static_cast<int>( x * Power ) ) / Power;
		}

		inline float Vec2Angle( float x, float y )
		{
			return 180.0f + RadToDeg ( atan2( y, x ) );
			/*
			         if ( x >= 0 )
			         {
			            if ( y >= 0 )
			            {
			               // x > 0, y > 0
			               // first Q
			               return RadToDeg( acos( x ) );
			            }
			            else
			            {
			               // x > 0, y < 0
			               // fourth Q
			               return 360.0f - RadToDeg( acos( x ) );
			            }
			         }
			         else
			         {
			            if ( y >= 0 )
			            {
			               // x < 0, y > 0
			               // second Q
			               return 180.0f - RadToDeg( asin( y ) );
			            }
			            else
			            {
			               // x < 0, y < 0
			               // third Q
			               return 180.0f + RadToDeg( asin( -y ) );
			            }
			         }
			*/
		}

		inline LVector2 ToPolar( float RR, float Angle ) { return RR * vec2( cosf( DegToRad( Angle ) ), sinf( DegToRad( Angle ) ) ); }

		// find angle1-angle2 clipping it to [0..360]
		inline float FindAngleDelta( float angle1, float angle2 )
		{
			/*
			         float delta = angle1 - angle2;

			         delta = ClipAngleTo0_360( delta );

			         if ( delta > 180.0 )
			         {
			            delta = delta - 360.0f;   // invert delta
			         }

			         return delta;
			*/

			float From = ClipAngleTo0_360( angle2 );
			float To   = ClipAngleTo0_360( angle1 );

			float Dist  = To - From;

			if ( Dist < -180.0f )
			{
				Dist += 360.0f;
			}
			else if ( Dist > 180.0f )
			{
				Dist -= 360.0f;
			}

			return Dist;
		}

		/**
		   Spherical and Cartesian coordinates

		   r     = |(x,y,z)|
		   phi   = arccos(y)
		   theta = atan2(x,z)

		   x = r * cos(theta) * sin(phi)
		   y = r * cos(theta) * cos(phi)
		   z = r * sin(theta)
		*/

		/// Convert (x,y,z) to (r, phi, theta)
		inline LVector3 CartesianToSpherical( const LVector3& Pos )
		{
			LVector3 Result;
			LVector3 NPos = Pos.GetNormalized();

			Result.X = Pos.Length();
			Result.Y = Math::Vec2Angle( NPos.X, NPos.Y );
			Result.Z = RadToDeg( acos( NPos.Z ) );

			return Result;
		}

		/// Convert (r, phi, theta) to (x,y,z)
		inline LVector3 SphericalToCartesian( const LVector3& Sph )
		{
			float sinPhi   = sin( Sph.Y );
			float cosPhi   = cos( Sph.Y );
			float sinTheta = sin( Sph.Z );
			float cosTheta = cos( Sph.Z );

			return Sph.X * LVector3( cosTheta * sinPhi, cosTheta * cosPhi, sinTheta );
		}

		inline LVector3 Barycentric2D( float x, float y, float x1, float y1, float x2, float y2, float x3, float y3 )
		{
			float detT = ( y2 - y3 ) * ( x1 - x3 ) + ( x3 - x2 ) * ( y1 - y3 );

			float l1 = ( ( y2 - y3 ) * ( x - x3 ) + ( x3 - x2 ) * ( y - y3 ) ) / detT;
			float l2 = ( ( y3 - y1 ) * ( x - x3 ) + ( x1 - x3 ) * ( y - y3 ) ) / detT;

			return LVector3( l1, l2, 1.0f - l1 - l2 );
		}

		inline float FactorAdjust( float Color, float Factor, float IntensityMax, float Gamma )
		{
			return ( Color > 0.001f ) ? IntensityMax * pow( Color * Factor, Gamma ) : 0.0f;
		}

		/// http://miguelmoreno.net/sandbox/wavelengthtoRGB/
		/// and http://www.midnightkite.com/color.html
		/// Wavelength from 350 to 780
		inline LVector3 ColorFromWaveLength( float W )
		{
			float Gamma  = 1.0f;
			float Blue   = 0.0f;
			float Green  = 0.0f;
			float Red    = 0.0f;
			float Factor = 0.0f;

			if ( ( W >= 350.0f ) && ( W < 440.0f ) )
			{
				Red    = -( W - 440.0f ) / ( 440.0f - 350.0f );
				Blue   = 1.0f;
			}
			else if ( ( W >= 440.0f ) && ( W < 490.0f ) )
			{
				Green  = ( W - 440.0f ) / ( 490.0f - 440.0f );
				Blue   = 1.0f;
			}
			else if ( ( W >= 490.0f ) && ( W < 510.0f ) )
			{
				Green  = 1.0f;
				Blue   = -( W - 510.0f ) / ( 510.0f - 490.0f );
			}
			else if ( ( W >= 510.0f ) && ( W < 580.0f ) )
			{
				Red    = ( W - 510.0f ) / ( 580.0f - 510.0f );
				Green  = 1.0f;
			}
			else if ( ( W >= 580.0f ) && ( W < 645.0f ) )
			{
				Red    = 1.0f;
				Green  = -( W - 645.0f ) / ( 645.0f - 580.0f );
			}
			else if ( ( W >= 645.0f ) && ( W <= 780.0f ) )
			{
				Red    = 1.0f;
			}

			if ( ( W >= 350.0f ) && ( W < 420.0f ) )
			{
				Factor = 0.3f + 0.7f * ( W - 350.0f ) / ( 420.0f - 350.0f );
			}
			else if ( ( W >= 420.0f ) && ( W < 700.0f ) )
			{
				Factor = 1.0f;
			}
			else if ( ( W >= 700.0f ) && ( W <= 780.0f ) )
			{
				Factor = 0.3f + 0.7f * ( 780.0f - W ) / ( 780.0f - 700.0f );
			}

			Red   = FactorAdjust( Red,   Factor, 255.0f, Gamma );
			Green = FactorAdjust( Green, Factor, 255.0f, Gamma );
			Blue  = FactorAdjust( Blue,  Factor, 255.0f, Gamma );

			return vec3( Red, Green, Blue ) / 255.0f;
		}

		/// [0..360] angle to the red..blue "rainbow"
		inline LVector3 ColorFromAngle( float phi )
		{
			float startA = 0.0f;

			float dA = 10.0f;

			if ( phi < startA + dA || phi > 360.0f + startA - dA )
			{
				float t = 0.0f;

				if ( phi > startA + dA )
				{
					t = ( phi - startA + dA - 360.0f ) / ( 2.0f * dA );
				}
				else
				{
					t = ( phi - startA + dA ) / ( 2.0f * dA );
				}

				return t * ColorFromWaveLength( 350.0f ) + ( 1.0f - t ) * ColorFromWaveLength( 780.0f );
			}

			// map [startA + dA, 360 + startA - dA] to [0..360]

			float phiN = ( phi - dA - startA ) / ( 360.0f - 2 * dA );

			return ColorFromWaveLength( 780.0f + phiN * ( 350.0f - 780.0f ) );
		}

	}
}

#endif

/*
 * 25/12/2010
     SphericalToCartesian/CartesianToSpherical
     Bugs fixed in FindAngleDelta
 * 16/09/2010
     Deg2Rad() and Rad2Deg() removed since we have DegToRad() and RadToDeg()
 * 08/02/2010
     IsMaskSet()
     IsBitSet()
     SetBit()
 * 24/11/2007
     ClipAngleTo0_360/180
     FindAngleDelta
     Vec2Angle
 * 04/02/2007
     IntPow()
 * 18/01/2007
     SphericalToOrtho()
 * 12/01/2007
     Clamp()
 * 28/02/2005
     It's here
*/
