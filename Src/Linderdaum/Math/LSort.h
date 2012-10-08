/**
 * \file LSort.h
 * \brief Radix sort
 * \version 0.6.00
 * \date 11/02/2011
 * \author Sergey Kosarevsky, 2010-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LSort_
#define _LSort_

#include "Platform.h"
#include "Math/LVector.h"
#include "Utils/LArray.h"

#include <math.h>
#include <cmath>

namespace Linderdaum
{
/// Linderdaum Math Library
	namespace Math
	{
		template <class T> void Radix8( int NumByte, const T& In, T& Out )
		{
			const int NUM_BITS  = 8;
			const int NUM_BYTES = 256;
			const int NumBit = NumByte * NUM_BITS;

			int Count[ NUM_BYTES ];
			int Index[ NUM_BYTES ];

			memset( Count, 0, sizeof( Count ) );

			for ( size_t i = 0; i != In.size(); i++ )
			{
				Count[ ( In[i].FSortKey >> NumBit ) & 0xFF ] ++;
			}

			Index[0] = 0;

			for ( int i = 1; i != NUM_BYTES; i++ )
			{
				Index[i] = Index[i-1] + Count[i-1];
			}

			for ( size_t i = 0; i != In.size(); i++ )
			{
				Out[ Index[( In[i].FSortKey >> NumBit ) & 0xFF]++ ] = In[i];
			}
		}

		template <class T> void RadixSort( T& In )
		{
			T Out;

			Out.resize( In.size() );

			int NumPasses = sizeof( typename T::value_type ) << 1;

			for ( int i = 0; i != NumPasses; i += 2 )
			{
				Radix8<T>( i + 0, In, Out );
				Radix8<T>( i + 1, Out, In );
			}
		}
	} // Math
} // Linderdaum

#endif

/*
 * 11/02/2011
     Can now sort both LArray and std::vector
 * 02/11/2010
     Container changed to LArray
 * 16/07/2010
     It's here
*/
