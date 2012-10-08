/**
 * \file AttribStream.h
 * \brief
 * \version 0.6.00
 * \date 14/04/2011
 * \author Sergey Kosarevsky, 2010-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __sAttribStream__
#define __sAttribStream__

#include "LString.h"

#include "Core/VFS/iIStream.h"
#include "Core/VFS/iOStream.h"

/// Stream of abstract attribs (vertices, texcoords, bones etc.)
template <class T>
class sAttribStream
{
public:
	sAttribStream(): FStream( 0 ) {};
	explicit sAttribStream( size_t Size ): FStream( Size ) {};

	// Merge operation
	void MergeWith( const sAttribStream<T>& Other )
	{
		Luint64 NumItems = Other.FStream.size();

		for ( size_t i = 0; i != NumItems; i++ ) { FStream.push_back( Other.FStream[i] ); }
	}

	void push_back( const T& Element )
	{
		FStream.push_back( Element );
	}

#pragma region Caching in internal format
	inline void LoadFromStream( iIStream* Stream )
	{
		Luint64 NumItems = 0;
		Stream->BlockRead( &NumItems, sizeof( NumItems ) );
		FStream.resize( static_cast<size_t>( NumItems ) );

		if ( NumItems > 0 ) { Stream->BlockRead( &FStream[0], NumItems * sizeof( T ) ); }
	}
	inline void SaveToStream( iOStream* Stream ) const
	{
		Luint64 NumItems = FStream.size();
		Stream->BlockWrite( &NumItems, sizeof( NumItems ) );

		if ( NumItems > 0 ) { Stream->BlockWrite( &FStream[0], NumItems * sizeof( T ) ); }
	}
	inline void LoadFromStreamArray( iIStream* Stream )
	{
		Luint64 NumItems = 0;
		Stream->BlockRead( &NumItems, sizeof( NumItems ) );
		FStream.resize( static_cast<size_t>( NumItems ) );

		for ( size_t i = 0; i != NumItems; i++ ) { FStream[i].LoadFromStream( Stream ); }
	}
	inline void SaveToStreamArray( iOStream* Stream ) const
	{
		Luint64 NumItems = FStream.size();
		Stream->BlockWrite( &NumItems, sizeof( NumItems ) );

		for ( size_t i = 0; i != NumItems; i++ ) { FStream[i].SaveToStream( Stream ); }
	}
#pragma endregion
	/// Get const pointer to FStream data array or NULL if it contains no elements
	inline const T*    GetPtr() const { return ( FStream.size() > 0 ) ? &FStream[0] : NULL; }

	/// Get pointer to FStream data array or NULL if it contains no elements
	inline T*    GetPtr() { return ( FStream.size() > 0 ) ? &FStream[0] : NULL; }

	/// Get number of elements
	inline size_t    size() const { return FStream.size(); }

	/// Resize the array
	inline void    resize( size_t NewSize ) { return FStream.resize( NewSize ); }

	const T& operator[]( const int Index ) const { return FStream[Index]; };

	T& operator[]( const int Index ) { return FStream[Index]; };

#ifdef OS_64BIT
	const T& operator[]( const size_t Index ) const { return FStream[Index]; };
	T& operator[]( const size_t Index ) { return FStream[Index]; };
#endif
	//
	LArray<T>    FStream;
};

#endif

/*
 * 14/04/2011
     Split from VertexAttribs.h
*/
