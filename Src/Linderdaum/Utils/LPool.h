/**
 * \file LPool.h
 * \brief Allocation pool
 * \version 0.5.93
 * \date 09/11/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LPool_
#define _LPool_

#include "Utils/LArray.h"

/// Pool of abstract items
template <typename T> class iPool
{
public:
	typedef T Type;
	typedef const T ConstType;
public:
	iPool(): FFreeItems( 0 ) {};
	virtual void Clear()
	{
		for ( typename LArray<T>::const_iterator i = FFreeItems.begin(); i != FFreeItems.end(); i++ )
		{
			DeallocateItem( *i );
		}

		FFreeItems.clear();
	}
	//
	// iPool
	//
	virtual T   GetItem()
	{
		if ( FFreeItems.empty() )
		{
			return this->AllocateItem();
		}

		T Item( FFreeItems.back() );
		FFreeItems.pop_back();
		return Item;
	}
	virtual void FreeItem( ConstType& Item )
	{
		FFreeItems.push_back( Item );
	}
protected:
	virtual T    AllocateItem() const = 0;
	virtual void DeallocateItem( ConstType& Item ) const = 0;
protected:
	LArray<T> FFreeItems;
};

#endif

/*
 * 09/11/2010
     It's here
*/
