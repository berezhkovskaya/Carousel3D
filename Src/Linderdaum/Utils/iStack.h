/**
 * \file iStack.h
 * \brief Virtual Machine stack interface
 * \version 0.5.91
 * \date 04/08/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iStack_
#define _iStack_

#include "Platform.h"
#include "Utils/Exceptions.h"

#include <string.h>

#define _FAST_STACK_

#ifdef _FAST_STACK_
#define CHECK_UNDERFLOW( Size, Value )      // never check underflow
#define GROW_STACK( Size )                  // never grow stack
#else
#define CHECK_UNDERFLOW( Size, Value ) FATAL( (Size) < 0, "Cannot pop "+ LString( Value )+" bytes from the stack, while it contains only " + LString( FSize ) + " bytes" );
#define GROW_STACK( Size ) if ( (Size) > FAllocated ) GrowStack();
#endif

/**
   \brief Fast unsafe stack

   This class is the heart of the LinderScript RunTime. Speed does matter.
**/
class iStack
{
public:
	static const int InitialAllocationSize = 10000;
public:
	iStack(): FSize( 0 ),
		FAllocated( InitialAllocationSize )
	{
		FStack = static_cast<Lubyte*>( malloc( FAllocated ) );
	};
	~iStack()
	{
		free( FStack );
	};
	void GrowStack()
	{
		FAllocated *= 2;
		FStack = static_cast<Lubyte*>( realloc( FStack, FAllocated ) );
	}
	//
	// iStack
	//
	inline void    clear()
	{
		FSize = 0;
	};
	inline bool    empty() const
	{
		return FSize == 0;
	};
	inline int     size() const
	{
		return FSize;
	};
	template <typename T> int    GetItemsCount() const
	{
		return FSize / sizeof( T );
	};

	inline Lubyte*   GetPtr( int Index )
	{
		return &FStack[Index];
	}
	inline void    PopEmptyBytes( int Count )
	{
		FSize -= Count;
		CHECK_UNDERFLOW( FSize, Count );
	}
	inline void    PushEmptyBytes( int Count )
	{
		FSize += Count;

		GROW_STACK( FSize );
	}
	inline void    PopBytes( int Count, void* To )
	{
		int NewSize = FSize - Count;

		CHECK_UNDERFLOW( NewSize, Count );

		memcpy( To, &FStack[ NewSize ], Count );

		FSize = NewSize;
	}
	inline void    PushBytes( int Count, const void* From )
	{
		int NewSize = FSize + Count;

		GROW_STACK( NewSize );

		memcpy( &FStack[ FSize ], From, Count );

		FSize = NewSize;
	}
	template <typename T> void    Push( const T& Value )
	{
		int NewSize = FSize + sizeof( T );

		GROW_STACK( NewSize );

		*( T* )&FStack[ FSize ] = Value;

		FSize = NewSize;
	}
	template <typename T> void    PushRep()
	{
		int Offset = FSize - sizeof( T );
		int NewSize = FSize + sizeof( T );

		CHECK_UNDERFLOW( Offset, sizeof( T ) );

		GROW_STACK( NewSize );

		*( T* )&FStack[ FSize ] = *( T* )&FStack[ Offset ];

		FSize = NewSize;
	}
	template <typename T>  void    PushRepBytes( int NumBytes )
	{
		int Offset = FSize - sizeof( T ) * NumBytes;
		int NewSize = FSize + sizeof( T ) * NumBytes;

		CHECK_UNDERFLOW( Offset, sizeof( T ) * NumBytes );

		GROW_STACK( NewSize );

		memcpy( &FStack[ FSize ], &FStack[ Offset ], NumBytes );

		FSize = NewSize;
	}
	template <typename T> T       Pop()
	{
		int NewSize = FSize - sizeof( T );

		CHECK_UNDERFLOW( NewSize, sizeof( T ) );

		T To = *( T* )&FStack[ NewSize ];

		FSize = NewSize;

		return To;
	}
	template <typename T> void    PopEmpty()
	{
		FSize -= sizeof( T );

		CHECK_UNDERFLOW( FSize, sizeof( T ) );
	}
	template <typename T> T       GetTop() const
	{
		int NewSize = FSize - sizeof( T );

		CHECK_UNDERFLOW( NewSize, sizeof( T ) );

		T To = *( T* )&FStack[ NewSize ];

		return To;
	}
	template <typename T> T&      GetTopRef() const
	{
		int NewSize = FSize - sizeof( T );

		CHECK_UNDERFLOW( NewSize, sizeof( T ) );

		return *( T* )&FStack[ NewSize ];
	}
	template <typename T> T      GetBottom() const
	{
		T To = *( T* )&FStack[ 0 ];

		return To;
	}
	template <typename T> T&      GetBottomRef() const
	{
		return *( T* )&FStack[ 0 ];
	}
	template <typename T> T&      GetRef( int N ) const
	{
		int NewSize = FSize - sizeof( T ) * ( N + 1 );

		CHECK_UNDERFLOW( NewSize, sizeof( T ) * N );

		return *( T* )&FStack[ NewSize ];
	}
private:
	Lubyte*    FStack;
	int        FSize;
	int        FAllocated;
};


template <> inline void iStack::Push( const LString& Value )
{
	LStringBuffer Buffer( Value );

	Push<LStringBuffer>( Buffer );
}

template <> inline LString iStack::Pop()
{
	return Pop<LStringBuffer>().ToString();
}

template <> inline void iStack::PopEmpty<LString>()
{
	PopEmpty<LStringBuffer>();
}

template <> inline LString iStack::GetTop() const
{
	return GetTop<LStringBuffer>().ToString();
}

template <> inline LString& iStack::GetTopRef() const
{
	// invalid
}

template <> inline LString& iStack::GetBottomRef() const
{
	// invalid
}

template <> inline LString& iStack::GetRef( int N ) const
{
	// invalid
}

#endif

/*
 * 04/08/2010
     Inlined methods
 * 25/07/2007
     GetBottom()
     GetBottomRef()
 * 27/03/2007
     PopEmpty()
 * 29/11/2005
     Faster stack
 * 02/08/2005
     It's here
*/
