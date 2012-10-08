/**
 * \file iIntrusivePtr.h
 * \brief Intrusive smart pointer
 * \version 0.5.60
 * \date 17/12/2005
 * \author Sergey Kosarevsky, 2005
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iIntrusivePtr_
#define _iIntrusivePtr_

/// Intrusive smart pointer
template <class T> class clPtr
{
private:
	class clProtector
	{
	private:
		void operator delete( void* );
	};
public:
	/// default constructor
	clPtr(): FObject( 0 )
	{
	}
	/// copy constructor
	clPtr( const clPtr& Ptr ): FObject( Ptr.FObject )
	{
		if ( FObject ) { FObject->IncRefCount(); }
	}
	template <typename U> clPtr( const clPtr<U>& Ptr ): FObject( Ptr.GetInternalPtr() )
	{
		if ( FObject ) { FObject->IncRefCount(); }
	}
	/// constructor from T*
	explicit clPtr( T* const Object ): FObject( Object )
	{
		if ( FObject ) { FObject->IncRefCount(); }
	}
	/// destructor
	~clPtr()
	{
		if ( FObject ) { FObject->DecRefCount(); }
	}
	/// check consistency
	bool IsValid() const
	{
		return FObject != 0;
	}
	/// assignment of clPtr
	clPtr& operator = ( const clPtr& Ptr )
	{
		T* Temp = FObject;
		FObject = Ptr.FObject;

		if ( Ptr.FObject ) { Ptr.FObject->IncRefCount(); }

		if ( Temp ) { Temp->DecRefCount(); }

		return *this;
	}
	/// -> operator
	T* operator -> () const
	{
		return FObject;
	}
	/// allow "if ( clPtr )" construction
	operator clProtector* () const
	{
		if ( !FObject )
		{
			return NULL;
		}

		static clProtector Protector;

		return &Protector;
	}
	/// cast
	template <typename U> clPtr<U> DynamicCast() const
	{
		return clPtr<U>( dynamic_cast<U*>( FObject ) );
	}
	/// compare
	template <typename U> bool operator == ( const clPtr<U>& Ptr1 ) const
	{
		return FObject == Ptr1.GetInternalPtr();
	}
	template <typename U> bool operator != ( const clPtr<U>& Ptr1 ) const
	{
		return FObject != Ptr1.GetInternalPtr();
	}
	/// helper
	T* GetInternalPtr() const
	{
		return FObject;
	}
private:
	T*    FObject;
};

#endif

/*
 * 08/12/2005
     It's here
*/
