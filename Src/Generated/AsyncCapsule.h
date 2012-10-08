/**
 * \file AsyncCapsule.h
 * \brief Async calls capsule
 * \version 0.6.02
 * \date 29/12/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _AsyncCapsule_h_
#define _AsyncCapsule_h_

#include "Platform.h"

class iAsyncCapsule
{
public:
	virtual ~iAsyncCapsule() {};
	virtual void Invoke() = 0;
};



//////////////////
// Params count: 0
//////////////////
template <typename ReturnType>
class clCapsuleParams0_Func : public iAsyncCapsule
{
	typedef ReturnType ( *FuncPtr )();
	FuncPtr    FFuncPtr;
public:
	explicit clCapsuleParams0_Func( FuncPtr Ptr ): FFuncPtr( Ptr ) {}
	virtual void        Invoke() { ( *FFuncPtr )(  ); }
};

template <typename ReturnType>
inline iAsyncCapsule* BindCapsule( ReturnType ( *FuncPtr )() )
{
	return new clCapsuleParams0_Func<ReturnType>( FuncPtr );
}


//////////////////
// Params count: 0
//////////////////
template <class T, typename ReturnType>
class clCapsuleParams0_Method : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )();
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
public:
	explicit clCapsuleParams0_Method( MethodPtr Ptr, T* ObjectAddr ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )(  ); }
};

template <class T, typename ReturnType>
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )(), T* ObjectAddr )
{
	return new clCapsuleParams0_Method<T, ReturnType>( MethodPtr, ObjectAddr );
}


//////////////////
// Params count: 0
//////////////////
template <class T, typename ReturnType>
class clCapsuleParams0_Method_Const : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )() const;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
public:
	explicit clCapsuleParams0_Method_Const( MethodPtr Ptr, T* ObjectAddr ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )(  ); }
};

template <class T, typename ReturnType>
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )() const, T* ObjectAddr )
{
	return new clCapsuleParams0_Method_Const<T, ReturnType>( MethodPtr, ObjectAddr );
}


//////////////////
// Params count: 0
//////////////////
template <class T, typename ReturnType>
class clCapsuleParams0_Method_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )() volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
public:
	explicit clCapsuleParams0_Method_Volatile( MethodPtr Ptr, T* ObjectAddr ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )(  ); }
};

template <class T, typename ReturnType>
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )() volatile, T* ObjectAddr )
{
	return new clCapsuleParams0_Method_Volatile<T, ReturnType>( MethodPtr, ObjectAddr );
}


//////////////////
// Params count: 0
//////////////////
template <class T, typename ReturnType>
class clCapsuleParams0_Method_Const_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )() const volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
public:
	explicit clCapsuleParams0_Method_Const_Volatile( MethodPtr Ptr, T* ObjectAddr ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )(  ); }
};

template <class T, typename ReturnType>
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )() const volatile, T* ObjectAddr )
{
	return new clCapsuleParams0_Method_Const_Volatile<T, ReturnType>( MethodPtr, ObjectAddr );
}


//////////////////
// Params count: 1
//////////////////
template <typename ReturnType, typename P0>
class clCapsuleParams1_Func : public iAsyncCapsule
{
	typedef ReturnType ( *FuncPtr )( P0 );
	FuncPtr    FFuncPtr;
	P0 FP0;
public:
	explicit clCapsuleParams1_Func( FuncPtr Ptr, P0 p0 ): FFuncPtr( Ptr ), FP0( p0 ) {}
	virtual void        Invoke() { ( *FFuncPtr )( FP0 ); }
};

template <typename ReturnType, typename P0>
inline iAsyncCapsule* BindCapsule( ReturnType ( *FuncPtr )( P0 ), P0 p0 )
{
	return new clCapsuleParams1_Func<ReturnType, P0>( FuncPtr, p0 );
}


//////////////////
// Params count: 1
//////////////////
template <class T, typename ReturnType, typename P0>
class clCapsuleParams1_Method : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0 );
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
public:
	explicit clCapsuleParams1_Method( MethodPtr Ptr, T* ObjectAddr, P0 p0 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0 ); }
};

template <class T, typename ReturnType, typename P0>
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0 ), T* ObjectAddr, P0 p0 )
{
	return new clCapsuleParams1_Method<T, ReturnType, P0>( MethodPtr, ObjectAddr, p0 );
}


//////////////////
// Params count: 1
//////////////////
template <class T, typename ReturnType, typename P0>
class clCapsuleParams1_Method_Const : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0 ) const;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
public:
	explicit clCapsuleParams1_Method_Const( MethodPtr Ptr, T* ObjectAddr, P0 p0 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0 ); }
};

template <class T, typename ReturnType, typename P0>
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0 ) const, T* ObjectAddr, P0 p0 )
{
	return new clCapsuleParams1_Method_Const<T, ReturnType, P0>( MethodPtr, ObjectAddr, p0 );
}


//////////////////
// Params count: 1
//////////////////
template <class T, typename ReturnType, typename P0>
class clCapsuleParams1_Method_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0 ) volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
public:
	explicit clCapsuleParams1_Method_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0 ); }
};

template <class T, typename ReturnType, typename P0>
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0 ) volatile, T* ObjectAddr, P0 p0 )
{
	return new clCapsuleParams1_Method_Volatile<T, ReturnType, P0>( MethodPtr, ObjectAddr, p0 );
}


//////////////////
// Params count: 1
//////////////////
template <class T, typename ReturnType, typename P0>
class clCapsuleParams1_Method_Const_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0 ) const volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
public:
	explicit clCapsuleParams1_Method_Const_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0 ); }
};

template <class T, typename ReturnType, typename P0>
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0 ) const volatile, T* ObjectAddr, P0 p0 )
{
	return new clCapsuleParams1_Method_Const_Volatile<T, ReturnType, P0>( MethodPtr, ObjectAddr, p0 );
}


//////////////////
// Params count: 2
//////////////////
template < typename ReturnType, typename P0,
         typename P1 >
class clCapsuleParams2_Func : public iAsyncCapsule
{
	typedef ReturnType ( *FuncPtr )( P0, P1 );
	FuncPtr    FFuncPtr;
	P0 FP0;
	P1 FP1;
public:
	explicit clCapsuleParams2_Func( FuncPtr Ptr, P0 p0, P1 p1 ): FFuncPtr( Ptr ), FP0( p0 ), FP1( p1 ) {}
	virtual void        Invoke() { ( *FFuncPtr )( FP0, FP1 ); }
};

template < typename ReturnType, typename P0,
         typename P1 >
inline iAsyncCapsule* BindCapsule( ReturnType ( *FuncPtr )( P0, P1 ), P0 p0, P1 p1 )
{
	return new clCapsuleParams2_Func<ReturnType, P0, P1>( FuncPtr, p0, p1 );
}


//////////////////
// Params count: 2
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1 >
class clCapsuleParams2_Method : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1 );
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
public:
	explicit clCapsuleParams2_Method( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1 ), T* ObjectAddr, P0 p0, P1 p1 )
{
	return new clCapsuleParams2_Method<T, ReturnType, P0, P1>( MethodPtr, ObjectAddr, p0, p1 );
}


//////////////////
// Params count: 2
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1 >
class clCapsuleParams2_Method_Const : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1 ) const;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
public:
	explicit clCapsuleParams2_Method_Const( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1 ) const, T* ObjectAddr, P0 p0, P1 p1 )
{
	return new clCapsuleParams2_Method_Const<T, ReturnType, P0, P1>( MethodPtr, ObjectAddr, p0, p1 );
}


//////////////////
// Params count: 2
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1 >
class clCapsuleParams2_Method_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1 ) volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
public:
	explicit clCapsuleParams2_Method_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1 ) volatile, T* ObjectAddr, P0 p0, P1 p1 )
{
	return new clCapsuleParams2_Method_Volatile<T, ReturnType, P0, P1>( MethodPtr, ObjectAddr, p0, p1 );
}


//////////////////
// Params count: 2
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1 >
class clCapsuleParams2_Method_Const_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1 ) const volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
public:
	explicit clCapsuleParams2_Method_Const_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1 ) const volatile, T* ObjectAddr, P0 p0, P1 p1 )
{
	return new clCapsuleParams2_Method_Const_Volatile<T, ReturnType, P0, P1>( MethodPtr, ObjectAddr, p0, p1 );
}


//////////////////
// Params count: 3
//////////////////
template < typename ReturnType, typename P0,
         typename P1,
         typename P2 >
class clCapsuleParams3_Func : public iAsyncCapsule
{
	typedef ReturnType ( *FuncPtr )( P0, P1, P2 );
	FuncPtr    FFuncPtr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
public:
	explicit clCapsuleParams3_Func( FuncPtr Ptr, P0 p0, P1 p1, P2 p2 ): FFuncPtr( Ptr ), FP0( p0 ), FP1( p1 ), FP2( p2 ) {}
	virtual void        Invoke() { ( *FFuncPtr )( FP0, FP1, FP2 ); }
};

template < typename ReturnType, typename P0,
         typename P1,
         typename P2 >
inline iAsyncCapsule* BindCapsule( ReturnType ( *FuncPtr )( P0, P1, P2 ), P0 p0, P1 p1, P2 p2 )
{
	return new clCapsuleParams3_Func<ReturnType, P0, P1, P2>( FuncPtr, p0, p1, p2 );
}


//////////////////
// Params count: 3
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2 >
class clCapsuleParams3_Method : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2 );
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
public:
	explicit clCapsuleParams3_Method( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2 ), T* ObjectAddr, P0 p0, P1 p1, P2 p2 )
{
	return new clCapsuleParams3_Method<T, ReturnType, P0, P1, P2>( MethodPtr, ObjectAddr, p0, p1, p2 );
}


//////////////////
// Params count: 3
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2 >
class clCapsuleParams3_Method_Const : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2 ) const;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
public:
	explicit clCapsuleParams3_Method_Const( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2 ) const, T* ObjectAddr, P0 p0, P1 p1, P2 p2 )
{
	return new clCapsuleParams3_Method_Const<T, ReturnType, P0, P1, P2>( MethodPtr, ObjectAddr, p0, p1, p2 );
}


//////////////////
// Params count: 3
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2 >
class clCapsuleParams3_Method_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2 ) volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
public:
	explicit clCapsuleParams3_Method_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2 ) volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2 )
{
	return new clCapsuleParams3_Method_Volatile<T, ReturnType, P0, P1, P2>( MethodPtr, ObjectAddr, p0, p1, p2 );
}


//////////////////
// Params count: 3
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2 >
class clCapsuleParams3_Method_Const_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2 ) const volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
public:
	explicit clCapsuleParams3_Method_Const_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2 ) const volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2 )
{
	return new clCapsuleParams3_Method_Const_Volatile<T, ReturnType, P0, P1, P2>( MethodPtr, ObjectAddr, p0, p1, p2 );
}


//////////////////
// Params count: 4
//////////////////
template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
class clCapsuleParams4_Func : public iAsyncCapsule
{
	typedef ReturnType ( *FuncPtr )( P0, P1, P2, P3 );
	FuncPtr    FFuncPtr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
public:
	explicit clCapsuleParams4_Func( FuncPtr Ptr, P0 p0, P1 p1, P2 p2, P3 p3 ): FFuncPtr( Ptr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ) {}
	virtual void        Invoke() { ( *FFuncPtr )( FP0, FP1, FP2, FP3 ); }
};

template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
inline iAsyncCapsule* BindCapsule( ReturnType ( *FuncPtr )( P0, P1, P2, P3 ), P0 p0, P1 p1, P2 p2, P3 p3 )
{
	return new clCapsuleParams4_Func<ReturnType, P0, P1, P2, P3>( FuncPtr, p0, p1, p2, p3 );
}


//////////////////
// Params count: 4
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
class clCapsuleParams4_Method : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3 );
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
public:
	explicit clCapsuleParams4_Method( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3 ), T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3 )
{
	return new clCapsuleParams4_Method<T, ReturnType, P0, P1, P2, P3>( MethodPtr, ObjectAddr, p0, p1, p2, p3 );
}


//////////////////
// Params count: 4
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
class clCapsuleParams4_Method_Const : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3 ) const;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
public:
	explicit clCapsuleParams4_Method_Const( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3 ) const, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3 )
{
	return new clCapsuleParams4_Method_Const<T, ReturnType, P0, P1, P2, P3>( MethodPtr, ObjectAddr, p0, p1, p2, p3 );
}


//////////////////
// Params count: 4
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
class clCapsuleParams4_Method_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3 ) volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
public:
	explicit clCapsuleParams4_Method_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3 ) volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3 )
{
	return new clCapsuleParams4_Method_Volatile<T, ReturnType, P0, P1, P2, P3>( MethodPtr, ObjectAddr, p0, p1, p2, p3 );
}


//////////////////
// Params count: 4
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
class clCapsuleParams4_Method_Const_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3 ) const volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
public:
	explicit clCapsuleParams4_Method_Const_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3 ) const volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3 )
{
	return new clCapsuleParams4_Method_Const_Volatile<T, ReturnType, P0, P1, P2, P3>( MethodPtr, ObjectAddr, p0, p1, p2, p3 );
}


//////////////////
// Params count: 5
//////////////////
template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
class clCapsuleParams5_Func : public iAsyncCapsule
{
	typedef ReturnType ( *FuncPtr )( P0, P1, P2, P3, P4 );
	FuncPtr    FFuncPtr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
public:
	explicit clCapsuleParams5_Func( FuncPtr Ptr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4 ): FFuncPtr( Ptr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ) {}
	virtual void        Invoke() { ( *FFuncPtr )( FP0, FP1, FP2, FP3, FP4 ); }
};

template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
inline iAsyncCapsule* BindCapsule( ReturnType ( *FuncPtr )( P0, P1, P2, P3, P4 ), P0 p0, P1 p1, P2 p2, P3 p3, P4 p4 )
{
	return new clCapsuleParams5_Func<ReturnType, P0, P1, P2, P3, P4>( FuncPtr, p0, p1, p2, p3, p4 );
}


//////////////////
// Params count: 5
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
class clCapsuleParams5_Method : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4 );
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
public:
	explicit clCapsuleParams5_Method( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4 ), T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4 )
{
	return new clCapsuleParams5_Method<T, ReturnType, P0, P1, P2, P3, P4>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4 );
}


//////////////////
// Params count: 5
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
class clCapsuleParams5_Method_Const : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4 ) const;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
public:
	explicit clCapsuleParams5_Method_Const( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4 ) const, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4 )
{
	return new clCapsuleParams5_Method_Const<T, ReturnType, P0, P1, P2, P3, P4>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4 );
}


//////////////////
// Params count: 5
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
class clCapsuleParams5_Method_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4 ) volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
public:
	explicit clCapsuleParams5_Method_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4 ) volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4 )
{
	return new clCapsuleParams5_Method_Volatile<T, ReturnType, P0, P1, P2, P3, P4>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4 );
}


//////////////////
// Params count: 5
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
class clCapsuleParams5_Method_Const_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4 ) const volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
public:
	explicit clCapsuleParams5_Method_Const_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4 ) const volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4 )
{
	return new clCapsuleParams5_Method_Const_Volatile<T, ReturnType, P0, P1, P2, P3, P4>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4 );
}


//////////////////
// Params count: 6
//////////////////
template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
class clCapsuleParams6_Func : public iAsyncCapsule
{
	typedef ReturnType ( *FuncPtr )( P0, P1, P2, P3, P4, P5 );
	FuncPtr    FFuncPtr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
public:
	explicit clCapsuleParams6_Func( FuncPtr Ptr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 ): FFuncPtr( Ptr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ) {}
	virtual void        Invoke() { ( *FFuncPtr )( FP0, FP1, FP2, FP3, FP4, FP5 ); }
};

template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
inline iAsyncCapsule* BindCapsule( ReturnType ( *FuncPtr )( P0, P1, P2, P3, P4, P5 ), P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 )
{
	return new clCapsuleParams6_Func<ReturnType, P0, P1, P2, P3, P4, P5>( FuncPtr, p0, p1, p2, p3, p4, p5 );
}


//////////////////
// Params count: 6
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
class clCapsuleParams6_Method : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 );
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
public:
	explicit clCapsuleParams6_Method( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ), T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 )
{
	return new clCapsuleParams6_Method<T, ReturnType, P0, P1, P2, P3, P4, P5>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5 );
}


//////////////////
// Params count: 6
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
class clCapsuleParams6_Method_Const : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ) const;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
public:
	explicit clCapsuleParams6_Method_Const( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ) const, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 )
{
	return new clCapsuleParams6_Method_Const<T, ReturnType, P0, P1, P2, P3, P4, P5>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5 );
}


//////////////////
// Params count: 6
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
class clCapsuleParams6_Method_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ) volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
public:
	explicit clCapsuleParams6_Method_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ) volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 )
{
	return new clCapsuleParams6_Method_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5 );
}


//////////////////
// Params count: 6
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
class clCapsuleParams6_Method_Const_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ) const volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
public:
	explicit clCapsuleParams6_Method_Const_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ) const volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 )
{
	return new clCapsuleParams6_Method_Const_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5 );
}


//////////////////
// Params count: 7
//////////////////
template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
class clCapsuleParams7_Func : public iAsyncCapsule
{
	typedef ReturnType ( *FuncPtr )( P0, P1, P2, P3, P4, P5, P6 );
	FuncPtr    FFuncPtr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
public:
	explicit clCapsuleParams7_Func( FuncPtr Ptr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 ): FFuncPtr( Ptr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ) {}
	virtual void        Invoke() { ( *FFuncPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6 ); }
};

template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
inline iAsyncCapsule* BindCapsule( ReturnType ( *FuncPtr )( P0, P1, P2, P3, P4, P5, P6 ), P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 )
{
	return new clCapsuleParams7_Func<ReturnType, P0, P1, P2, P3, P4, P5, P6>( FuncPtr, p0, p1, p2, p3, p4, p5, p6 );
}


//////////////////
// Params count: 7
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
class clCapsuleParams7_Method : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 );
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
public:
	explicit clCapsuleParams7_Method( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ), T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 )
{
	return new clCapsuleParams7_Method<T, ReturnType, P0, P1, P2, P3, P4, P5, P6>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6 );
}


//////////////////
// Params count: 7
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
class clCapsuleParams7_Method_Const : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) const;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
public:
	explicit clCapsuleParams7_Method_Const( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) const, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 )
{
	return new clCapsuleParams7_Method_Const<T, ReturnType, P0, P1, P2, P3, P4, P5, P6>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6 );
}


//////////////////
// Params count: 7
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
class clCapsuleParams7_Method_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
public:
	explicit clCapsuleParams7_Method_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 )
{
	return new clCapsuleParams7_Method_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6 );
}


//////////////////
// Params count: 7
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
class clCapsuleParams7_Method_Const_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) const volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
public:
	explicit clCapsuleParams7_Method_Const_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) const volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 )
{
	return new clCapsuleParams7_Method_Const_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6 );
}


//////////////////
// Params count: 8
//////////////////
template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
class clCapsuleParams8_Func : public iAsyncCapsule
{
	typedef ReturnType ( *FuncPtr )( P0, P1, P2, P3, P4, P5, P6, P7 );
	FuncPtr    FFuncPtr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
public:
	explicit clCapsuleParams8_Func( FuncPtr Ptr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 ): FFuncPtr( Ptr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ) {}
	virtual void        Invoke() { ( *FFuncPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7 ); }
};

template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
inline iAsyncCapsule* BindCapsule( ReturnType ( *FuncPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ), P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 )
{
	return new clCapsuleParams8_Func<ReturnType, P0, P1, P2, P3, P4, P5, P6, P7>( FuncPtr, p0, p1, p2, p3, p4, p5, p6, p7 );
}


//////////////////
// Params count: 8
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
class clCapsuleParams8_Method : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 );
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
public:
	explicit clCapsuleParams8_Method( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ), T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 )
{
	return new clCapsuleParams8_Method<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6, p7 );
}


//////////////////
// Params count: 8
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
class clCapsuleParams8_Method_Const : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) const;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
public:
	explicit clCapsuleParams8_Method_Const( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) const, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 )
{
	return new clCapsuleParams8_Method_Const<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6, p7 );
}


//////////////////
// Params count: 8
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
class clCapsuleParams8_Method_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
public:
	explicit clCapsuleParams8_Method_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 )
{
	return new clCapsuleParams8_Method_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6, p7 );
}


//////////////////
// Params count: 8
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
class clCapsuleParams8_Method_Const_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) const volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
public:
	explicit clCapsuleParams8_Method_Const_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) const volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 )
{
	return new clCapsuleParams8_Method_Const_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6, p7 );
}


//////////////////
// Params count: 9
//////////////////
template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8 >
class clCapsuleParams9_Func : public iAsyncCapsule
{
	typedef ReturnType ( *FuncPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 );
	FuncPtr    FFuncPtr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
	P8 FP8;
public:
	explicit clCapsuleParams9_Func( FuncPtr Ptr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 ): FFuncPtr( Ptr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ), FP8( p8 ) {}
	virtual void        Invoke() { ( *FFuncPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8 ); }
};

template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8 >
inline iAsyncCapsule* BindCapsule( ReturnType ( *FuncPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ), P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 )
{
	return new clCapsuleParams9_Func<ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8>( FuncPtr, p0, p1, p2, p3, p4, p5, p6, p7, p8 );
}


//////////////////
// Params count: 9
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8 >
class clCapsuleParams9_Method : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 );
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
	P8 FP8;
public:
	explicit clCapsuleParams9_Method( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ), FP8( p8 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ), T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 )
{
	return new clCapsuleParams9_Method<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6, p7, p8 );
}


//////////////////
// Params count: 9
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8 >
class clCapsuleParams9_Method_Const : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) const;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
	P8 FP8;
public:
	explicit clCapsuleParams9_Method_Const( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ), FP8( p8 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) const, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 )
{
	return new clCapsuleParams9_Method_Const<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6, p7, p8 );
}


//////////////////
// Params count: 9
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8 >
class clCapsuleParams9_Method_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
	P8 FP8;
public:
	explicit clCapsuleParams9_Method_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ), FP8( p8 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 )
{
	return new clCapsuleParams9_Method_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6, p7, p8 );
}


//////////////////
// Params count: 9
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8 >
class clCapsuleParams9_Method_Const_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) const volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
	P8 FP8;
public:
	explicit clCapsuleParams9_Method_Const_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ), FP8( p8 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) const volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 )
{
	return new clCapsuleParams9_Method_Const_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6, p7, p8 );
}


//////////////////
// Params count: 10
//////////////////
template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9 >
class clCapsuleParams10_Func : public iAsyncCapsule
{
	typedef ReturnType ( *FuncPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 );
	FuncPtr    FFuncPtr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
	P8 FP8;
	P9 FP9;
public:
	explicit clCapsuleParams10_Func( FuncPtr Ptr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9 ): FFuncPtr( Ptr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ), FP8( p8 ), FP9( p9 ) {}
	virtual void        Invoke() { ( *FFuncPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9 ); }
};

template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9 >
inline iAsyncCapsule* BindCapsule( ReturnType ( *FuncPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ), P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9 )
{
	return new clCapsuleParams10_Func<ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9>( FuncPtr, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9 );
}


//////////////////
// Params count: 10
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9 >
class clCapsuleParams10_Method : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 );
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
	P8 FP8;
	P9 FP9;
public:
	explicit clCapsuleParams10_Method( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ), FP8( p8 ), FP9( p9 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ), T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9 )
{
	return new clCapsuleParams10_Method<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9 );
}


//////////////////
// Params count: 10
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9 >
class clCapsuleParams10_Method_Const : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
	P8 FP8;
	P9 FP9;
public:
	explicit clCapsuleParams10_Method_Const( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ), FP8( p8 ), FP9( p9 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9 )
{
	return new clCapsuleParams10_Method_Const<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9 );
}


//////////////////
// Params count: 10
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9 >
class clCapsuleParams10_Method_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
	P8 FP8;
	P9 FP9;
public:
	explicit clCapsuleParams10_Method_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ), FP8( p8 ), FP9( p9 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9 )
{
	return new clCapsuleParams10_Method_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9 );
}


//////////////////
// Params count: 10
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9 >
class clCapsuleParams10_Method_Const_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
	P8 FP8;
	P9 FP9;
public:
	explicit clCapsuleParams10_Method_Const_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ), FP8( p8 ), FP9( p9 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9 )
{
	return new clCapsuleParams10_Method_Const_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9 );
}


//////////////////
// Params count: 11
//////////////////
template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9,
         typename P10 >
class clCapsuleParams11_Func : public iAsyncCapsule
{
	typedef ReturnType ( *FuncPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 );
	FuncPtr    FFuncPtr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
	P8 FP8;
	P9 FP9;
	P10 FP10;
public:
	explicit clCapsuleParams11_Func( FuncPtr Ptr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10 ): FFuncPtr( Ptr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ), FP8( p8 ), FP9( p9 ), FP10( p10 ) {}
	virtual void        Invoke() { ( *FFuncPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9, FP10 ); }
};

template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9,
         typename P10 >
inline iAsyncCapsule* BindCapsule( ReturnType ( *FuncPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ), P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10 )
{
	return new clCapsuleParams11_Func<ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10>( FuncPtr, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10 );
}


//////////////////
// Params count: 11
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9,
         typename P10 >
class clCapsuleParams11_Method : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 );
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
	P8 FP8;
	P9 FP9;
	P10 FP10;
public:
	explicit clCapsuleParams11_Method( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ), FP8( p8 ), FP9( p9 ), FP10( p10 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9, FP10 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9,
         typename P10 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ), T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10 )
{
	return new clCapsuleParams11_Method<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10 );
}


//////////////////
// Params count: 11
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9,
         typename P10 >
class clCapsuleParams11_Method_Const : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ) const;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
	P8 FP8;
	P9 FP9;
	P10 FP10;
public:
	explicit clCapsuleParams11_Method_Const( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ), FP8( p8 ), FP9( p9 ), FP10( p10 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9, FP10 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9,
         typename P10 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ) const, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10 )
{
	return new clCapsuleParams11_Method_Const<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10 );
}


//////////////////
// Params count: 11
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9,
         typename P10 >
class clCapsuleParams11_Method_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ) volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
	P8 FP8;
	P9 FP9;
	P10 FP10;
public:
	explicit clCapsuleParams11_Method_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ), FP8( p8 ), FP9( p9 ), FP10( p10 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9, FP10 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9,
         typename P10 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ) volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10 )
{
	return new clCapsuleParams11_Method_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10 );
}


//////////////////
// Params count: 11
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9,
         typename P10 >
class clCapsuleParams11_Method_Const_Volatile : public iAsyncCapsule
{
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ) const volatile;
	MethodPtr    FMethodPtr;
	T*           FObjectAddr;
	P0 FP0;
	P1 FP1;
	P2 FP2;
	P3 FP3;
	P4 FP4;
	P5 FP5;
	P6 FP6;
	P7 FP7;
	P8 FP8;
	P9 FP9;
	P10 FP10;
public:
	explicit clCapsuleParams11_Method_Const_Volatile( MethodPtr Ptr, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10 ): FMethodPtr( Ptr ), FObjectAddr( ObjectAddr ), FP0( p0 ), FP1( p1 ), FP2( p2 ), FP3( p3 ), FP4( p4 ), FP5( p5 ), FP6( p6 ), FP7( p7 ), FP8( p8 ), FP9( p9 ), FP10( p10 ) {}
	virtual void        Invoke() { ( FObjectAddr->*FMethodPtr )( FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9, FP10 ); }
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9,
         typename P10 >
inline iAsyncCapsule* BindCapsule( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ) const volatile, T* ObjectAddr, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10 )
{
	return new clCapsuleParams11_Method_Const_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10>( MethodPtr, ObjectAddr, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10 );
}
#endif

/*
 * 29/12/2011
     Autogenerated via LSDC
*/

