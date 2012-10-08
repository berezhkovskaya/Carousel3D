/*
   (Part of Linderdaum Engine)
   Version 0.5.89
   (02/01/2009)
   (C) Sergey Kosarevsky, 2009
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _LHistogram_
#define _LHistogram_

#include "Platform.h"

#include <vector>

#include "Math/LMath.h"
//#include <math.h>

/// 1D histogram, useful for volume rendering. Support 8 and 16 bit data
class LHistogram1D
{
public:
	LHistogram1D() {};
	//
	// LHistogram1D
	//

	/// Get calculated density
	int      GetDensity( int OfValue ) const
	{
		if ( OfValue < 0 || OfValue >= static_cast<int>( FBuckets.size() ) ) { return 0; }

		return FBuckets[ OfValue ];
	}

	/// Get normalized calculated density (0..1 value)
	float    GetNormalizedDensity( float OfValue ) const
	{
		if ( OfValue < 0.0f || OfValue > 1.0f || FBuckets.size() == 0 ) { return 0.0f; }

		return FBucketsNormalized[ static_cast<int>( OfValue * ( FBucketsNormalized.size() - 1 ) ) ];
	}

	/// Total number of buckets
	int      GetNumBuckets() { return static_cast<int>( FBuckets.size() ); }

	/// Build histogram for the linear list of 8-bit samples
	void     AnalyzeData8( int Elements, const Lubyte* Data )
	{
		InitBuckets( 256 );

		for ( int i = 0; i != Elements; ++i ) { FBuckets[ Data[i] ] ++; }

		NormalizeBuckets();
	}

	/// Build histogram for interleaved array of (1..NumComponents) using CompIdx index. Elemnts is the number of tuples.
	void     AnalyzeData8Skip( int Elements, const Lubyte* Data, int NumComponents, int CompIdx )
	{
		InitBuckets( 256 );

		for ( int i = 0; i != Elements; ++i ) { FBuckets[ Data[ i * NumComponents + CompIdx ] ] ++; }

		NormalizeBuckets();
	}

	/// Build histogram for intensity of the RGB triplets
	void     AnalyzeData8RGBIntensity( int Elements, const Lubyte* Data, int NumComponents )
	{
		InitBuckets( 256 );

		for ( int i = 0; i != Elements; ++i )
		{
			float R = Data[ i * NumComponents + 0 ];
			float G = Data[ i * NumComponents + 1 ];
			float B = Data[ i * NumComponents + 2 ];

			int Intensity = ( int )( 0.11f * R + 0.59f * G + 0.3f * B );

			Intensity = Linderdaum::Math::Clamp( Intensity, 0, 255 );

			FBuckets[ Intensity ] ++;
		}

		NormalizeBuckets();
	}

	/// Build histogram for the linear list of 16-bit samples
	void     AnalyzeData16( int Elements, const Lushort* Data )
	{
		InitBuckets( 65536 );

		for ( int i = 0; i != Elements; ++i ) { FBuckets[ Data[i] ] ++; }

		NormalizeBuckets();
	}

private:
	void    InitBuckets( int TotalBuckets )
	{
		FBuckets.resize( TotalBuckets );

		for ( int i = 0; i != TotalBuckets; ++i ) { FBuckets[ i ] = 0; }
	}

	void    NormalizeBuckets()
	{
		FBucketsNormalized.resize( FBuckets.size() );

		int MaxValue = 0;

		for ( size_t i = 0; i != FBuckets.size(); ++i )
		{
			if ( FBuckets[ i ] > MaxValue ) { MaxValue = FBuckets[ i ]; }
		}

		for ( size_t i = 0; i != FBuckets.size(); ++i )
		{
			FBucketsNormalized[ i ] = static_cast<float>( FBuckets[i] ) / static_cast<float>( MaxValue );
		}
	}

private:
	std::vector<int>     FBuckets;
	std::vector<float>   FBucketsNormalized;
};

#endif

/*
 * 02/01/2009
     It's here
*/
