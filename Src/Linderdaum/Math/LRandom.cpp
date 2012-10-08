/*
   (Part of Linderdaum Engine)
   Version 0.5.81
   (02/10/2007)
   (C) Sergey Kosarevsky, 2005-2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "LRandom.h"

#include <cstdlib>

namespace Linderdaum
{
	namespace Math
	{
		void Randomize( int Seed )
		{
			std::srand( Seed );
		}
		int Random( int L )
		{
			return ( int ) ( ( float ) L * ( float ) std::rand() / ( float ) RAND_MAX );
		}

		float Random( float L )
		{
			return L * ( float ) std::rand() / ( float ) RAND_MAX;
		}

		float Random()
		{
			return ( float ) std::rand() / ( float ) RAND_MAX;
		}

		float RandomInRange( float RMin, float RMax )
		{
			return RMin + Random() * ( RMax - RMin );
		}
		int RandomInRange( int RMin, int RMax )
		{
			return RMin + Random( RMax - RMin );
		}
		LVector3 RandomVector3InRange( const LVector3& RMin,
		                               const LVector3& RMax )
		{
			return LVector3( RandomInRange( RMin.X, RMax.X ),
			                 RandomInRange( RMin.Y, RMax.Y ),
			                 RandomInRange( RMin.Z, RMax.Z ) );
		}
		LVector4 RandomVector4InRange( const LVector4& RMin,
		                               const LVector4& RMax )
		{
			return LVector4( RandomInRange( RMin.X, RMax.X ),
			                 RandomInRange( RMin.Y, RMax.Y ),
			                 RandomInRange( RMin.Z, RMax.Z ),
			                 RandomInRange( RMin.W, RMax.W ) );
		}
	}
}

using namespace Linderdaum;

/// Normalize 1,2,3 or 4 dimensional vector
inline void NormalizeN( float* f, int n )
{
	int i;

	float Maximum = -1.0f;

	for ( i = 0 ; i < n ; i++ )
	{
		Maximum = Math::LMax( Math::LAbs( f[i] ), Maximum );
	}

	if ( Maximum < Math::EPSILON ) { return; }

	float Magnitude = 0.0f;

	for ( i = 0 ; i < n ; i++ )
	{
		Magnitude += ( f[i] / Maximum ) * ( f[i] / Maximum );
	}

	Magnitude = 1 / ( Maximum * sqrtf( Magnitude ) );

	for ( i = 0 ; i < n ; i++ )
	{
		f[i] *= Magnitude;
	}
}

void LNoise::InitNoise( int nDimensions, unsigned int nSeed )
{
	Math::Randomize( nSeed );

	FDimensions = nDimensions;

	if ( nDimensions > MAX_DIMENSIONS ) { nDimensions = MAX_DIMENSIONS; }

	int i, j;

	for ( i = 0; i < 256; i++ )
	{
		FMap[i] = ( unsigned char )i;

		for ( j = 0; j < FDimensions; j++ )
		{
			FBuffer[i][j] = Math::RandomInRange( -0.5f, 0.5f );
		}

		NormalizeN( FBuffer[i], FDimensions );
	}

	while ( --i )
	{
		j = Math::RandomInRange( 0, 255 );
		unsigned char Tmp = FMap[i];
		FMap[i] = FMap[j];
		FMap[j] = Tmp;
	}
}

float LNoise::Noise( float* f )
{
	int i, n[MAX_DIMENSIONS];  // Indexes to pass to lattice function
	float r[MAX_DIMENSIONS];   // Remainders to pass to lattice function
	float w[MAX_DIMENSIONS];   // Cubic values to pass to interpolation function

	for ( i = 0 ; i < FDimensions ; i++ )
	{
		n[i] = Math::Floor( f[i] );
		r[i] = f[i] - n[i];
		w[i] = Math::Cubic( r[i] );
	}

	float Value, v1, v2, v3, v4, l1, l2;

	switch ( FDimensions )
	{
		case 1:
			Value = Math::Lerp( Lattice( n[0], r[0] ), Lattice( n[0] + 1, r[0] - 1 ), w[0] );
			break;

		case 2:
			v1 = Math::Lerp( Lattice( n[0], r[0], n[1],   r[1] ),   Lattice( n[0] + 1, r[0] - 1, n[1],   r[1] ),   w[0] );
			v2 = Math::Lerp( Lattice( n[0], r[0], n[1] + 1, r[1] - 1 ), Lattice( n[0] + 1, r[0] - 1, n[1] + 1, r[1] - 1 ), w[0] );

			Value = Math::Lerp( v1, v2, w[1] );
			break;

		case 3:
			// fallthrough
		default:
			l1 = Lattice( n[0],   r[0],   n[1],   r[1],   n[2],   r[2] );
			l2 = Lattice( n[0] + 1, r[0] - 1, n[1],   r[1],   n[2],   r[2] );
			v1 = Math::Lerp( l1, l2, w[0] );

			l1 = Lattice( n[0],   r[0],   n[1] + 1, r[1] - 1, n[2],   r[2] );
			l2 = Lattice( n[0] + 1, r[0] - 1, n[1] + 1, r[1] - 1, n[2],   r[2] );
			v2 = Math::Lerp( l1, l2, w[0] );

			l1 = Lattice( n[0],   r[0],   n[1],   r[1],   n[2] + 1, r[2] - 1 );
			l2 = Lattice( n[0] + 1, r[0] - 1, n[1],   r[1],   n[2] + 1, r[2] - 1 );
			v3 = Math::Lerp( l1, l2, w[0] );

			l1 = Lattice( n[0],   r[0],   n[1] + 1, r[1] - 1, n[2] + 1, r[2] - 1 );
			l2 = Lattice( n[0] + 1, r[0] - 1, n[1] + 1, r[1] - 1, n[2] + 1, r[2] - 1 );
			v4 = Math::Lerp( l1, l2, w[0] );
			Value = Math::Lerp( Math::Lerp( v1, v2, w[1] ), Math::Lerp( v3, v4, w[1] ), w[2] );
	}

	return Math::Clamp( Value * 2.0f, -0.99999f, 0.99999f );
}

float LNoise::fBm( float* f, float Octaves )
{
	// Initialize locals
	int i;
	float Value = 0;
	float Temp[MAX_DIMENSIONS];

	for ( i = 0 ; i < FDimensions ; i++ )
	{
		Temp[i] = f[i];
	}

	// Inner loop of spectral construction, where the fractal is built
	for ( i = 0; i < Octaves; i++ )
	{
		Value += Noise( Temp ) * FExponent[i];

		for ( int j = 0 ; j < FDimensions ; j++ )
		{
			Temp[j] *= FLacunarity;
		}
	}

	// Take care of remainder in fOctaves
	Octaves -= ( int )Octaves;

	if ( Octaves > Math::EPSILON )
	{
		Value += Octaves * Noise( Temp ) * FExponent[i];
	}

	return Math::Clamp( Value, -0.99999f, 0.99999f );
}

/*
 * 25/01/2011
     Noise generator implementation
 * 02/10/2007
     std::rand() used
 * 15/05/2005
     Randomize()
 * 01/05/2005
     RandomInRange() for ints
 * 17/04/2005
     It's here
*/
