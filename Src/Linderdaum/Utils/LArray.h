/**
 * \file LArray.h
 * \brief Fast array, replacement of std::vector
 * \version 0.6.00
 * \date 11/12/2011
 * \author Sergey Kosarevsky, 2010-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LArray_
#define _LArray_

#include <stdio.h>
#include <stdlib.h>
#include "Platform.h"

#include <algorithm>

/**
   \brief Fast unsafe array

   Replacement of std::vector

   LV: it would be extremely useful to add a "OwnsMemory" flag
   to indicate a fixed-size array in a user-specified memory block

   i.e.,  "SomeType Fixed[100]"  can be wrapped by "LArray<SomeType>( true (OwnsMemory) , &Fixed[0], 100) )"

   Introducing the LFixedArray type causes incompatibility with LArray

   Maximum size limit would also be useful for recursive splittings of the array.
   I.e., if we want to split source array into "left" and "right" parts based on some condition,
   we allocate two arrays and each of them is not allowed to allocate more than source.size()
**/
template <typename T> class LArray
{
public:
	typedef T value_type;
	typedef T& pointer;
	typedef T* reference;
private:
	static const size_t InitialElements = 10;
	typedef T Type;
	typedef const T ConstType;
	class LReverseIterator
	{
	public:
		explicit LReverseIterator( T* Ptr ) : FPtr( Ptr ) {}
		inline bool operator == ( const LReverseIterator& Other ) const { return FPtr == Other.FPtr; }
		inline bool operator != ( const LReverseIterator& Other ) const { return FPtr != Other.FPtr; }
		inline void operator ++ () { FPtr--; }
		inline T& operator -> () { return *FPtr; }
		inline T& operator * () { return *FPtr; }
	private:
		T* FPtr;
	};
	class LReverseConstIterator
	{
	public:
		explicit LReverseConstIterator( T* Ptr ) : FPtr( Ptr ) {}
		inline bool operator == ( const LReverseConstIterator& Other ) const { return FPtr == Other.FPtr; }
		inline bool operator != ( const LReverseConstIterator& Other ) const { return FPtr != Other.FPtr; }
		inline void operator ++ () { FPtr--; }
		inline const T& operator -> () const { return *FPtr; }
		inline const T& operator * () const { return *FPtr; }
	private:
		const T* FPtr;
	};
public:
	typedef typename LArray<T>::Type*      iterator;
	typedef typename LArray<T>::ConstType* const_iterator;

	typedef typename LArray<T>::LReverseIterator reverse_iterator;
	typedef typename LArray<T>::LReverseConstIterator const_reverse_iterator;
public:
	/// default constructor
	LArray() : FSize( 0 ),
		FAllocated( InitialElements* sizeof( T ) )
	{
		FArray = static_cast<T*>( malloc( FAllocated ) );
	};

#ifdef LARRAY_INITIALIZER_LIST
	/// initializer list constructor
	LArray( const std::initializer_list<T>& list ) : FSize( list.size() ),
		FAllocated( list.size() * sizeof( T ) )
	{
		FArray = static_cast<T*>( malloc( FAllocated ) );
		iterator i = begin();
		const_iterator j = list.begin();

		for ( iterator i = begin(); i != end(); i++, j++ ) { new ( i ) T( *j ); }
	}
#endif

	/// constructor with an explicit number of elements
	explicit LArray( size_t Size ) : FSize( Size ),
		FAllocated( Size* sizeof( T ) )
	{
		FArray = static_cast<T*>( malloc( FAllocated ) );
		ConstructDefault( begin(), end() );
	}

	/// constructor from an unmanaged preallocated memory block
	LArray( T* UnmanagedMemory, size_t MemSize ) : FSize( 0 ),
		FAllocated( MemSize ),
		FArray( UnmanagedMemory )
	{
		for ( iterator i = begin(); i != end(); i++ ) { new ( i ) T(); }
	}

	/// constructor from a C-style array
	LArray( const T* Begin, const T* End ) : FSize( End - Begin ),
		FAllocated( ( End - Begin ) * sizeof( T ) )
	{
		FArray = static_cast<T*>( malloc( FAllocated ) );

		for ( iterator i = begin(); i != end(); i++, Begin++ ) { new ( i ) T( *Begin ); }
	}

	/// copy constructor
	LArray( const LArray<T>& Other ) : FSize( Other.FSize  ),
		FAllocated( Other.FAllocated )
	{
		if ( this == &Other ) { return; }

		FArray = static_cast<T*>( malloc( FAllocated ) );

		const_iterator j = Other.begin();

		for ( iterator i = begin(); i != end(); i++, j++ ) { new ( i ) T( *j ); }
	}

	/// destructor
	~LArray()
	{
		clear();

		FSize = 0;
		FAllocated = 0;
		free( FArray );
	};

	/// assignment operator
	inline LArray& operator = ( const LArray<T>& Other )
	{
		LArray<T> Copy( Other );

		swap( Copy );

		return *this;
	}

	/// Fast removal of the element (copy the last item at the given position and pop_back
	inline void EraseNoShift( size_t Idx ) { FArray[Idx] = FArray[FSize - 1]; pop_back(); }

	inline void swap( LArray<T>& Other )
	{
		std::swap( FSize, Other.FSize );
		std::swap( FAllocated, Other.FAllocated );
		std::swap( FArray, Other.FArray );
	}

	inline bool operator == ( const LArray<T>& Other ) const
	{
		if ( this == &Other ) { return true; }

		if ( FSize != Other.FSize ) { return false; }

		const_iterator j = Other.begin();

		for ( const_iterator i = begin(); i != end(); i++, j++ )
		{
			// invoke operator T::==
			if ( !( *i == *j ) ) { return false; }
		}

		return true;
	}

	inline void    resize( size_t NewSize )
	{
		if ( NewSize > FSize )
		{
			// grow
			reserve( NewSize );
			ConstructDefault( begin() + FSize, begin() + NewSize );
		}
		else
		{
			// truncate
			Destruct( begin() + NewSize, begin() + FSize );
		}

		FSize = NewSize;
	}

	inline void    reserve( size_t NewSize )
	{
		if ( NewSize <= FSize ) { return; }

		FAllocated = NewSize * sizeof( T );
		FArray = static_cast<T*>( realloc( FArray, FAllocated ) );
	}

	inline void    clear()
	{
		Destruct( begin(), end() );
		FSize = 0;
	};

	inline bool     empty() const { return FSize == 0; };
	inline size_t   size() const { return FSize; };
	inline T*       GetPtr( size_t Index ) { return &FArray[Index]; }

	inline const T& operator[]( size_t Index ) const { return FArray[Index]; };
	inline T&       operator[]( size_t Index )       { return FArray[Index]; };

	inline const T& at( size_t Index ) const { return FArray[Index]; };
	inline T&       at( size_t Index )       { return FArray[Index]; };

	inline const T& back() const { return FArray[FSize-1]; };
	inline T&       back()       { return FArray[FSize-1]; };

	inline const_iterator begin() const { return FArray; };
	inline iterator       begin()       { return FArray; };

	inline const_iterator end() const { return FArray + FSize; };
	inline iterator       end()       { return FArray + FSize; };

	inline const_reverse_iterator rbegin() const { return LReverseConstIterator( FArray + FSize - 1 ); };
	inline reverse_iterator       rbegin()       { return LReverseIterator( FArray + FSize - 1 ); };

	inline const_reverse_iterator rend() const { return LReverseConstIterator( FArray - 1 ); };
	inline reverse_iterator       rend()       { return LReverseIterator( FArray - 1 ); };

	/*
	   inline T    pop_back()
	   {
	      FSize -= 1;
	      // call copy ctor
	      T Copy( FArray[ FSize ] );
	      // call dtor
	      FArray[ FSize ].~T();
	      // call copy ctor and dtor
	      return Copy;
	   }
	*/
	inline void    pop_back()
	{
		FSize -= 1;
		FArray[ FSize ].~T();
	}

	inline void    push_back( const T& Value )
	{
		size_t NewSize = FSize + 1;

		if ( NewSize * sizeof( T ) > FAllocated ) { GrowArray(); }

		new ( FArray + FSize ) T( Value );

		FSize = NewSize;
	}

private:
	inline void GrowArray()
	{
		FAllocated = ( FAllocated + InitialElements * sizeof( T ) ) * 2;
		FArray = static_cast<T*>( realloc( FArray, FAllocated ) );
	}
	inline void ConstructDefault( iterator From, iterator To )
	{
		for ( iterator i = From; i != To; i ++ ) { new ( i ) T; }
	}
	inline void Destruct( iterator From, iterator To )
	{
		for ( iterator i = From; i != To; i ++ ) { i->~T(); }
	}
private:
	T*       FArray;
	/// in elements
	size_t   FSize;
	/// in bytes
	size_t   FAllocated;
};

#pragma region Forward declarations
#ifndef PLATFORM_MSVC
#include "Math/LVector.h"
#include "Math/LMatrix.h"
extern template class LArray<const void*>;
extern template class LArray<int>;
extern template class LArray<Luint>;
extern template class LArray<LVector2>;
extern template class LArray<LVector3>;
extern template class LArray<LVector4>;
extern template class LArray<LVector4i>;
extern template class LArray<LMatrix4>;
#endif
#pragma endregion

#endif

/*
 * 11/02/2011
     Added ::value_type, ::pointer, ::reference
 * 04/11/2010
     Reverse iterators
 * 03/11/2010
     Forward declarations
     Internal container iteration optimized
 * 02/11/2010
     Constructors and destructors are called
 * 01/11/2010
     First usable version
 * 29/10/2010
     It's here
*/
