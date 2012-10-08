/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _LRandom_
#define _LRandom_

#include "LMath.h"

namespace Linderdaum
{
	namespace Math
	{
		scriptexport( Math ) void     Randomize( int Seed );
		scriptexport( Math ) int      Random( int L );
		scriptexport( Math ) float    Random( float L );
		scriptexport( Math ) float    Random();
		scriptexport( Math ) float    RandomInRange( float RMin, float RMax );
		scriptexport( Math ) LVector3 RandomVector3InRange( const LVector3& RMin,
		                                                    const LVector3& RMax );
		scriptexport( Math ) LVector4 RandomVector4InRange( const LVector4& RMin,
		                                                    const LVector4& RMax );
		scriptexport( Math ) int      RandomInRange( int RMin, int RMax );
	};
};

/// Max noise dimension
const int MAX_DIMENSIONS = 3;

/// Maximum number of octaves in an fBm object
const int MAX_OCTAVES = 128;

/**
 \brief Multidimensional noise generator

 This class implements the Perlin noise function. Initialize it with the number
 of dimensions (1 to 4) and a random seed. I got the source for the first 3
 dimensions from "Texturing & Modeling: A Procedural Approach".
 The noise buffers are set up as member variables so that
 there may be several instances of this class in use at the same time, each
 initialized with different parameters.

 This class also implements fBm, or fractal Brownian motion.
 H (roughness ranging from 0 to 1), and the lacunarity (2.0 is often used).

 Taken from source code which is Copyright (c) 2000, Sean O'Neil (s_p_oneil@hotmail.com, http://sponeil.net/)
 All rights reserved.  (BSD/MIT-like license)
*/
class LNoise
{
public:

	LNoise() {}
	LNoise( int nDimensions, unsigned int nSeed ) { InitNoise( nDimensions, nSeed ); }

	LNoise( int nDimensions, unsigned int nSeed, float H, float Lacunarity )
	{
		InitNoise( nDimensions, nSeed );
		InitFractal( H, Lacunarity );
	}

	/// Initialize internal random generator and tables
	void InitNoise( int nDimensions, unsigned int nSeed );

	/// Initialize internal fractal parameters
	void InitFractal( float H, float Lacunarity )
	{
		FH = H;
		FLacunarity = Lacunarity;

		float f = 1;

		for ( int i = 0 ; i < MAX_OCTAVES ; i++ )
		{
			FExponent[i] = powf( f, -H );
			f *= Lacunarity;
		}
	}

	/// Random noise
	float Noise( float* f );

	/// Fractal brownian motion
	float fBm( float* f, float Octaves );

protected:

#pragma region Noise parameters

	/// Number of dimensions used by this object
	int    FDimensions;

	/// Randomized map of indexes into buffer
	unsigned char FMap[256];

	/// Random n-dimensional buffer
	float  FBuffer[256][MAX_DIMENSIONS];

#pragma endregion

#pragma region Fractal properties

	/// Roughness
	float FH;

	/// Lacunarity parameter
	float FLacunarity;

	/// Precalculated exponents
	float FExponent[MAX_OCTAVES];

#pragma endregion

	/// Internal noise function
	float Lattice( int ix, float fx, int iy = 0, float fy = 0, int iz = 0, float fz = 0, int iw = 0, float fw = 0 )
	{
		int   n[4] = {ix, iy, iz, iw};
		float f[4] = {fx, fy, fz, fw};

		int i, Idx = 0;

		for ( i = 0; i < FDimensions ; i++ )
		{
			Idx = FMap[( Idx + n[i] ) & 0xFF];
		}

		float Value = 0;

		for ( i = 0 ; i < FDimensions ; i++ )
		{
			Value += FBuffer[Idx][i] * f[i];
		}

		return Value;
	}
};

#endif

/*
 * 25/01/2011
     (1,2,3)-D Noise generator
 * 17/04/2005
     It's here
*/
