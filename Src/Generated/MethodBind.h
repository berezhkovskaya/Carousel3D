/**
 * \file MethodBind.h
 * \brief Native methods binders
 * \version 0.8.0
 * \date 22/01/2011
 * \author Sergey Kosarevsky, 2005-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 * \author Viktor Latypov, 2007-2011
 * \author viktor@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iMethodBind_
#define _iMethodBind_

#include "Core/RTTI/Method.h"
#include "Core/RTTI/Parameters.h"

/// \cond



//////////////////
// Params count: 0
//////////////////
template <class T, typename ReturnType>
class clNativeMethodParams0_Func: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )();
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams0_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )();
	}
	virtual int         GetParamsCount() const
	{
		return 0;
	}
	virtual int         GetParamSize( int Index )
	{

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template <class T, typename ReturnType>
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )() , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams0_Func<T, ReturnType>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 0
//////////////////
template <class T>
class clNativeMethodParams0_Func<T, void>: public iMethod
{
private:
	typedef void ( T::*MethodPtr )();
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams0_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		( ( ( T* )ObjectAddr )->*FMethodPtr )();
	}
	virtual int         GetParamsCount() const
	{
		return 0;
	}
	virtual int         GetParamSize( int Index )
	{

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 0
//////////////////
template <typename ReturnType>
class clNativeStaticMethodParams0_Func: public iMethod
{
private:
	typedef ReturnType ( *MethodPtr )();
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams0_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( *FMethodPtr )();
	}
	virtual int         GetParamsCount() const
	{
		return 0;
	}
	virtual int         GetParamSize( int Index )
	{

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template <typename ReturnType>
inline iMethod* BindNativeMethod( ReturnType ( *MethodPtr )() , const LString& MethodName )
{
	iMethod* Method = new clNativeStaticMethodParams0_Func<ReturnType>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 0
//////////////////
template <>
class clNativeStaticMethodParams0_Func<void>: public iMethod
{
private:
	typedef void ( *MethodPtr )();
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams0_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		( *FMethodPtr )();
	}
	virtual int         GetParamsCount() const
	{
		return 0;
	}
	virtual int         GetParamSize( int Index )
	{

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 0
//////////////////
template <class T, typename ReturnType>
class clNativeMethodParams0_Func_Const: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )() const;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams0_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )();
	}
	virtual int         GetParamsCount() const
	{
		return 0;
	}
	virtual int         GetParamSize( int Index )
	{

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template <class T, typename ReturnType>
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )() const , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams0_Func_Const<T, ReturnType>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 0
//////////////////
template <class T>
class clNativeMethodParams0_Func_Const<T, void>: public iMethod
{
private:
	typedef void ( T::*MethodPtr )() const;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams0_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		( ( ( T* )ObjectAddr )->*FMethodPtr )();
	}
	virtual int         GetParamsCount() const
	{
		return 0;
	}
	virtual int         GetParamSize( int Index )
	{

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 0
//////////////////
template <class T, typename ReturnType>
class clNativeMethodParams0_Func_Const_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )() const volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams0_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )();
	}
	virtual int         GetParamsCount() const
	{
		return 0;
	}
	virtual int         GetParamSize( int Index )
	{

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template <class T, typename ReturnType>
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )() const volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams0_Func_Const_Volatile<T, ReturnType>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 0
//////////////////
template <class T>
class clNativeMethodParams0_Func_Const_Volatile<T, void>: public iMethod
{
private:
	typedef void ( T::*MethodPtr )() const volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams0_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		( ( ( T* )ObjectAddr )->*FMethodPtr )();
	}
	virtual int         GetParamsCount() const
	{
		return 0;
	}
	virtual int         GetParamSize( int Index )
	{

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 0
//////////////////
template <class T, typename ReturnType>
class clNativeMethodParams0_Func_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )() volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams0_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )();
	}
	virtual int         GetParamsCount() const
	{
		return 0;
	}
	virtual int         GetParamSize( int Index )
	{

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template <class T, typename ReturnType>
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )() volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams0_Func_Volatile<T, ReturnType>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 0
//////////////////
template <class T>
class clNativeMethodParams0_Func_Volatile<T, void>: public iMethod
{
private:
	typedef void ( T::*MethodPtr )() volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams0_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		( ( ( T* )ObjectAddr )->*FMethodPtr )();
	}
	virtual int         GetParamsCount() const
	{
		return 0;
	}
	virtual int         GetParamSize( int Index )
	{

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 1
//////////////////
template <class T, typename ReturnType, typename P0>
class clNativeMethodParams1_Func: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams1_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 1, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 1;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template <class T, typename ReturnType, typename P0>
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams1_Func<T, ReturnType, P0>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 1
//////////////////
template <class T, typename P0>
class clNativeMethodParams1_Func<T, void, P0>: public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0 );
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams1_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 1, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 1;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 1
//////////////////
template <typename ReturnType, typename P0>
class clNativeStaticMethodParams1_Func: public iMethod
{
private:
	typedef ReturnType ( *MethodPtr )( P0 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams1_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 1, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 1;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template <typename ReturnType, typename P0>
inline iMethod* BindNativeMethod( ReturnType ( *MethodPtr )( P0 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeStaticMethodParams1_Func<ReturnType, P0>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 1
//////////////////
template <typename P0>
class clNativeStaticMethodParams1_Func<void, P0>: public iMethod
{
private:
	typedef void ( *MethodPtr )( P0 );
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams1_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 1, "Not enough parameters passed in clParametersList" );

		( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 1;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 1
//////////////////
template <class T, typename ReturnType, typename P0>
class clNativeMethodParams1_Func_Const: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0 ) const;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams1_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 1, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 1;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template <class T, typename ReturnType, typename P0>
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0 ) const , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams1_Func_Const<T, ReturnType, P0>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 1
//////////////////
template <class T, typename P0>
class clNativeMethodParams1_Func_Const<T, void, P0>: public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0 ) const;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams1_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 1, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 1;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 1
//////////////////
template <class T, typename ReturnType, typename P0>
class clNativeMethodParams1_Func_Const_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0 ) const volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams1_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 1, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 1;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template <class T, typename ReturnType, typename P0>
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0 ) const volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams1_Func_Const_Volatile<T, ReturnType, P0>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 1
//////////////////
template <class T, typename P0>
class clNativeMethodParams1_Func_Const_Volatile<T, void, P0>: public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0 ) const volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams1_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 1, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 1;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 1
//////////////////
template <class T, typename ReturnType, typename P0>
class clNativeMethodParams1_Func_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0 ) volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams1_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 1, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 1;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template <class T, typename ReturnType, typename P0>
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0 ) volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams1_Func_Volatile<T, ReturnType, P0>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 1
//////////////////
template <class T, typename P0>
class clNativeMethodParams1_Func_Volatile<T, void, P0>: public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0 ) volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams1_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 1, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 1;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 2
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1 >
class clNativeMethodParams2_Func: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams2_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 2, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 2;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams2_Func<T, ReturnType, P0, P1>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 2
//////////////////
template < class T, typename P0,
         typename P1 >
class clNativeMethodParams2_Func < T, void, P0,
	P1 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1 );
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams2_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 2, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 2;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 2
//////////////////
template < typename ReturnType, typename P0,
         typename P1 >
class clNativeStaticMethodParams2_Func: public iMethod
{
private:
	typedef ReturnType ( *MethodPtr )( P0, P1 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams2_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 2, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 2;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < typename ReturnType, typename P0,
         typename P1 >
inline iMethod* BindNativeMethod( ReturnType ( *MethodPtr )( P0, P1 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeStaticMethodParams2_Func<ReturnType, P0, P1>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 2
//////////////////
template < typename P0,
         typename P1 >
class clNativeStaticMethodParams2_Func < void, P0,
	P1 > : public iMethod
{
private:
	typedef void ( *MethodPtr )( P0, P1 );
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams2_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 2, "Not enough parameters passed in clParametersList" );

		( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 2;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 2
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1 >
class clNativeMethodParams2_Func_Const: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1 ) const;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams2_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 2, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 2;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1 ) const , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams2_Func_Const<T, ReturnType, P0, P1>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 2
//////////////////
template < class T, typename P0,
         typename P1 >
class clNativeMethodParams2_Func_Const < T, void, P0,
	P1 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1 ) const;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams2_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 2, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 2;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 2
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1 >
class clNativeMethodParams2_Func_Const_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1 ) const volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams2_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 2, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 2;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1 ) const volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams2_Func_Const_Volatile<T, ReturnType, P0, P1>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 2
//////////////////
template < class T, typename P0,
         typename P1 >
class clNativeMethodParams2_Func_Const_Volatile < T, void, P0,
	P1 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1 ) const volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams2_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 2, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 2;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 2
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1 >
class clNativeMethodParams2_Func_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1 ) volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams2_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 2, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 2;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1 ) volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams2_Func_Volatile<T, ReturnType, P0, P1>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 2
//////////////////
template < class T, typename P0,
         typename P1 >
class clNativeMethodParams2_Func_Volatile < T, void, P0,
	P1 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1 ) volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams2_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 2, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 2;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 3
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2 >
class clNativeMethodParams3_Func: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams3_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 3, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 3;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams3_Func<T, ReturnType, P0, P1, P2>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 3
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2 >
class clNativeMethodParams3_Func < T, void, P0,
	P1,
	P2 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2 );
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams3_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 3, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 3;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 3
//////////////////
template < typename ReturnType, typename P0,
         typename P1,
         typename P2 >
class clNativeStaticMethodParams3_Func: public iMethod
{
private:
	typedef ReturnType ( *MethodPtr )( P0, P1, P2 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams3_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 3, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 3;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < typename ReturnType, typename P0,
         typename P1,
         typename P2 >
inline iMethod* BindNativeMethod( ReturnType ( *MethodPtr )( P0, P1, P2 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeStaticMethodParams3_Func<ReturnType, P0, P1, P2>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 3
//////////////////
template < typename P0,
         typename P1,
         typename P2 >
class clNativeStaticMethodParams3_Func < void, P0,
	P1,
	P2 > : public iMethod
{
private:
	typedef void ( *MethodPtr )( P0, P1, P2 );
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams3_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 3, "Not enough parameters passed in clParametersList" );

		( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 3;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 3
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2 >
class clNativeMethodParams3_Func_Const: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2 ) const;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams3_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 3, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 3;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2 ) const , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams3_Func_Const<T, ReturnType, P0, P1, P2>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 3
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2 >
class clNativeMethodParams3_Func_Const < T, void, P0,
	P1,
	P2 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2 ) const;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams3_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 3, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 3;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 3
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2 >
class clNativeMethodParams3_Func_Const_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2 ) const volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams3_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 3, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 3;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2 ) const volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams3_Func_Const_Volatile<T, ReturnType, P0, P1, P2>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 3
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2 >
class clNativeMethodParams3_Func_Const_Volatile < T, void, P0,
	P1,
	P2 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2 ) const volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams3_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 3, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 3;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 3
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2 >
class clNativeMethodParams3_Func_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2 ) volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams3_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 3, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 3;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2 ) volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams3_Func_Volatile<T, ReturnType, P0, P1, P2>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 3
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2 >
class clNativeMethodParams3_Func_Volatile < T, void, P0,
	P1,
	P2 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2 ) volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams3_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 3, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 3;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 4
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
class clNativeMethodParams4_Func: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams4_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 4, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 4;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams4_Func<T, ReturnType, P0, P1, P2, P3>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 4
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3 >
class clNativeMethodParams4_Func < T, void, P0,
	P1,
	P2,
	P3 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3 );
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams4_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 4, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 4;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 4
//////////////////
template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
class clNativeStaticMethodParams4_Func: public iMethod
{
private:
	typedef ReturnType ( *MethodPtr )( P0, P1, P2, P3 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams4_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 4, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 4;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
inline iMethod* BindNativeMethod( ReturnType ( *MethodPtr )( P0, P1, P2, P3 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeStaticMethodParams4_Func<ReturnType, P0, P1, P2, P3>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 4
//////////////////
template < typename P0,
         typename P1,
         typename P2,
         typename P3 >
class clNativeStaticMethodParams4_Func < void, P0,
	P1,
	P2,
	P3 > : public iMethod
{
private:
	typedef void ( *MethodPtr )( P0, P1, P2, P3 );
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams4_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 4, "Not enough parameters passed in clParametersList" );

		( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 4;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 4
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
class clNativeMethodParams4_Func_Const: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3 ) const;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams4_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 4, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 4;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3 ) const , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams4_Func_Const<T, ReturnType, P0, P1, P2, P3>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 4
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3 >
class clNativeMethodParams4_Func_Const < T, void, P0,
	P1,
	P2,
	P3 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3 ) const;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams4_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 4, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 4;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 4
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
class clNativeMethodParams4_Func_Const_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3 ) const volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams4_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 4, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 4;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3 ) const volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams4_Func_Const_Volatile<T, ReturnType, P0, P1, P2, P3>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 4
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3 >
class clNativeMethodParams4_Func_Const_Volatile < T, void, P0,
	P1,
	P2,
	P3 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3 ) const volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams4_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 4, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 4;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 4
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
class clNativeMethodParams4_Func_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3 ) volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams4_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 4, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 4;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3 ) volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams4_Func_Volatile<T, ReturnType, P0, P1, P2, P3>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 4
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3 >
class clNativeMethodParams4_Func_Volatile < T, void, P0,
	P1,
	P2,
	P3 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3 ) volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams4_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 4, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 4;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 5
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
class clNativeMethodParams5_Func: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams5_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 5, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 5;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams5_Func<T, ReturnType, P0, P1, P2, P3, P4>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 5
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
class clNativeMethodParams5_Func < T, void, P0,
	P1,
	P2,
	P3,
	P4 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4 );
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams5_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 5, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 5;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 5
//////////////////
template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
class clNativeStaticMethodParams5_Func: public iMethod
{
private:
	typedef ReturnType ( *MethodPtr )( P0, P1, P2, P3, P4 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams5_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 5, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 5;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
inline iMethod* BindNativeMethod( ReturnType ( *MethodPtr )( P0, P1, P2, P3, P4 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeStaticMethodParams5_Func<ReturnType, P0, P1, P2, P3, P4>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 5
//////////////////
template < typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
class clNativeStaticMethodParams5_Func < void, P0,
	P1,
	P2,
	P3,
	P4 > : public iMethod
{
private:
	typedef void ( *MethodPtr )( P0, P1, P2, P3, P4 );
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams5_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 5, "Not enough parameters passed in clParametersList" );

		( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 5;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 5
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
class clNativeMethodParams5_Func_Const: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4 ) const;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams5_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 5, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 5;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4 ) const , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams5_Func_Const<T, ReturnType, P0, P1, P2, P3, P4>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 5
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
class clNativeMethodParams5_Func_Const < T, void, P0,
	P1,
	P2,
	P3,
	P4 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4 ) const;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams5_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 5, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 5;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 5
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
class clNativeMethodParams5_Func_Const_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4 ) const volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams5_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 5, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 5;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4 ) const volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams5_Func_Const_Volatile<T, ReturnType, P0, P1, P2, P3, P4>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 5
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
class clNativeMethodParams5_Func_Const_Volatile < T, void, P0,
	P1,
	P2,
	P3,
	P4 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4 ) const volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams5_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 5, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 5;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 5
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
class clNativeMethodParams5_Func_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4 ) volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams5_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 5, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 5;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4 ) volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams5_Func_Volatile<T, ReturnType, P0, P1, P2, P3, P4>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 5
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 >
class clNativeMethodParams5_Func_Volatile < T, void, P0,
	P1,
	P2,
	P3,
	P4 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4 ) volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams5_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 5, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 5;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 6
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
class clNativeMethodParams6_Func: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams6_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 6, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 6;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams6_Func<T, ReturnType, P0, P1, P2, P3, P4, P5>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 6
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
class clNativeMethodParams6_Func < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 );
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams6_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 6, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 6;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 6
//////////////////
template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
class clNativeStaticMethodParams6_Func: public iMethod
{
private:
	typedef ReturnType ( *MethodPtr )( P0, P1, P2, P3, P4, P5 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams6_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 6, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 6;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
inline iMethod* BindNativeMethod( ReturnType ( *MethodPtr )( P0, P1, P2, P3, P4, P5 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeStaticMethodParams6_Func<ReturnType, P0, P1, P2, P3, P4, P5>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 6
//////////////////
template < typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
class clNativeStaticMethodParams6_Func < void, P0,
	P1,
	P2,
	P3,
	P4,
	P5 > : public iMethod
{
private:
	typedef void ( *MethodPtr )( P0, P1, P2, P3, P4, P5 );
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams6_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 6, "Not enough parameters passed in clParametersList" );

		( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 6;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 6
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
class clNativeMethodParams6_Func_Const: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ) const;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams6_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 6, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 6;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ) const , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams6_Func_Const<T, ReturnType, P0, P1, P2, P3, P4, P5>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 6
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
class clNativeMethodParams6_Func_Const < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ) const;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams6_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 6, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 6;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 6
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
class clNativeMethodParams6_Func_Const_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ) const volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams6_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 6, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 6;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ) const volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams6_Func_Const_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 6
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
class clNativeMethodParams6_Func_Const_Volatile < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ) const volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams6_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 6, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 6;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


//////////////////
// Params count: 6
//////////////////
template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
class clNativeMethodParams6_Func_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ) volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams6_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 6, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 6;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ) volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams6_Func_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 6
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 >
class clNativeMethodParams6_Func_Volatile < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5 ) volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams6_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 6, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 6;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeMethodParams7_Func: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams7_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 7, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 7;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams7_Func<T, ReturnType, P0, P1, P2, P3, P4, P5, P6>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 7
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
class clNativeMethodParams7_Func < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 );
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams7_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 7, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 7;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeStaticMethodParams7_Func: public iMethod
{
private:
	typedef ReturnType ( *MethodPtr )( P0, P1, P2, P3, P4, P5, P6 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams7_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 7, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 7;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
inline iMethod* BindNativeMethod( ReturnType ( *MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeStaticMethodParams7_Func<ReturnType, P0, P1, P2, P3, P4, P5, P6>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 7
//////////////////
template < typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
class clNativeStaticMethodParams7_Func < void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6 > : public iMethod
{
private:
	typedef void ( *MethodPtr )( P0, P1, P2, P3, P4, P5, P6 );
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams7_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 7, "Not enough parameters passed in clParametersList" );

		( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 7;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeMethodParams7_Func_Const: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) const;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams7_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 7, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 7;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) const , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams7_Func_Const<T, ReturnType, P0, P1, P2, P3, P4, P5, P6>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 7
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
class clNativeMethodParams7_Func_Const < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) const;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams7_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 7, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 7;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeMethodParams7_Func_Const_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) const volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams7_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 7, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 7;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) const volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams7_Func_Const_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 7
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
class clNativeMethodParams7_Func_Const_Volatile < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) const volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams7_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 7, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 7;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeMethodParams7_Func_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams7_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 7, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 7;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams7_Func_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 7
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 >
class clNativeMethodParams7_Func_Volatile < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6 ) volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams7_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 7, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 7;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeMethodParams8_Func: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams8_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 8, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 8;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams8_Func<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 8
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
class clNativeMethodParams8_Func < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 );
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams8_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 8, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 8;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeStaticMethodParams8_Func: public iMethod
{
private:
	typedef ReturnType ( *MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams8_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 8, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 8;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
inline iMethod* BindNativeMethod( ReturnType ( *MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeStaticMethodParams8_Func<ReturnType, P0, P1, P2, P3, P4, P5, P6, P7>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 8
//////////////////
template < typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
class clNativeStaticMethodParams8_Func < void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7 > : public iMethod
{
private:
	typedef void ( *MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 );
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams8_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 8, "Not enough parameters passed in clParametersList" );

		( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                 *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 8;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeMethodParams8_Func_Const: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) const;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams8_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 8, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 8;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) const , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams8_Func_Const<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 8
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
class clNativeMethodParams8_Func_Const < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) const;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams8_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 8, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 8;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeMethodParams8_Func_Const_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) const volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams8_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 8, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 8;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) const volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams8_Func_Const_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 8
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
class clNativeMethodParams8_Func_Const_Volatile < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) const volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams8_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 8, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 8;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeMethodParams8_Func_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams8_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 8, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 8;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};

template < class T, typename ReturnType, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams8_Func_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 8
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 >
class clNativeMethodParams8_Func_Volatile < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7 ) volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams8_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 8, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 8;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeMethodParams9_Func: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams9_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 9, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 9;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
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
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams9_Func<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 9
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8 >
class clNativeMethodParams9_Func < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7,
	P8 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 );
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams9_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 9, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 9;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeStaticMethodParams9_Func: public iMethod
{
private:
	typedef ReturnType ( *MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams9_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 9, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 9;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
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
inline iMethod* BindNativeMethod( ReturnType ( *MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeStaticMethodParams9_Func<ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 9
//////////////////
template < typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8 >
class clNativeStaticMethodParams9_Func < void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7,
	P8 > : public iMethod
{
private:
	typedef void ( *MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 );
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams9_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 9, "Not enough parameters passed in clParametersList" );

		( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                 *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                 *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 9;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeMethodParams9_Func_Const: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) const;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams9_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 9, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 9;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
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
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) const , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams9_Func_Const<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 9
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8 >
class clNativeMethodParams9_Func_Const < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7,
	P8 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) const;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams9_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 9, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 9;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeMethodParams9_Func_Const_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) const volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams9_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 9, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 9;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
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
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) const volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams9_Func_Const_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 9
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8 >
class clNativeMethodParams9_Func_Const_Volatile < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7,
	P8 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) const volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams9_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 9, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 9;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeMethodParams9_Func_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams9_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 9, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 9;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
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
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams9_Func_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 9
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8 >
class clNativeMethodParams9_Func_Volatile < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7,
	P8 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams9_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 9, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 9;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeMethodParams10_Func: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams10_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 10, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P9>::ReferredType* )( Params[9]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 10;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		if ( Index == 9 ) { return sizeof( P9 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Index == 9 ) { Param = CreateNativeParameter<P9>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
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
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams10_Func<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 10
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9 >
class clNativeMethodParams10_Func < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7,
	P8,
	P9 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 );
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams10_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 10, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P9>::ReferredType* )( Params[9]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 10;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		if ( Index == 9 ) { return sizeof( P9 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Index == 9 ) { Param = CreateNativeParameter<P9>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeStaticMethodParams10_Func: public iMethod
{
private:
	typedef ReturnType ( *MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 );
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams10_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 10, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ),
		                                                                  *( typename TypeTraits<P9>::ReferredType* )( Params[9]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 10;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		if ( Index == 9 ) { return sizeof( P9 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Index == 9 ) { Param = CreateNativeParameter<P9>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
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
inline iMethod* BindNativeMethod( ReturnType ( *MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) , const LString& MethodName )
{
	iMethod* Method = new clNativeStaticMethodParams10_Func<ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 10
//////////////////
template < typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9 >
class clNativeStaticMethodParams10_Func < void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7,
	P8,
	P9 > : public iMethod
{
private:
	typedef void ( *MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 );
	MethodPtr    FMethodPtr;
public:
	clNativeStaticMethodParams10_Func( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 10, "Not enough parameters passed in clParametersList" );

		( *FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                 *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                 *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ),
		                 *( typename TypeTraits<P9>::ReferredType* )( Params[9]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 10;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		if ( Index == 9 ) { return sizeof( P9 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Index == 9 ) { Param = CreateNativeParameter<P9>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeMethodParams10_Func_Const: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams10_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 10, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P9>::ReferredType* )( Params[9]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 10;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		if ( Index == 9 ) { return sizeof( P9 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Index == 9 ) { Param = CreateNativeParameter<P9>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
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
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams10_Func_Const<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 10
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9 >
class clNativeMethodParams10_Func_Const < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7,
	P8,
	P9 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams10_Func_Const( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 10, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P9>::ReferredType* )( Params[9]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 10;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		if ( Index == 9 ) { return sizeof( P9 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Index == 9 ) { Param = CreateNativeParameter<P9>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeMethodParams10_Func_Const_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams10_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 10, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P9>::ReferredType* )( Params[9]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 10;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		if ( Index == 9 ) { return sizeof( P9 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Index == 9 ) { Param = CreateNativeParameter<P9>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
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
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams10_Func_Const_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 10
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9 >
class clNativeMethodParams10_Func_Const_Volatile < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7,
	P8,
	P9 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams10_Func_Const_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 10, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P9>::ReferredType* )( Params[9]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 10;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		if ( Index == 9 ) { return sizeof( P9 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Index == 9 ) { Param = CreateNativeParameter<P9>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};


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
class clNativeMethodParams10_Func_Volatile: public iMethod
{
private:
	typedef ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) volatile;
	typedef typename TypeTraits<ReturnType>::ReferredType ReturnValueType;
	typedef typename TypeTraits<ReturnValueType>::NonConstType ReturnNonConstType;
	typedef typename TypeTraits<ReturnValueType>::UnqualifiedType ReturnUnqualifiedType;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams10_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
		SetReturnValue( CreateNativeParameter<ReturnType>() );
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 10, "Not enough parameters passed in clParametersList" );

		*( ReturnUnqualifiedType* )GetReturnValuePtr() = ( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ),
		                                                 *( typename TypeTraits<P9>::ReferredType* )( Params[9]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 10;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		if ( Index == 9 ) { return sizeof( P9 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Index == 9 ) { Param = CreateNativeParameter<P9>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
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
inline iMethod* BindNativeMethod( ReturnType ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) volatile , const LString& MethodName )
{
	iMethod* Method = new clNativeMethodParams10_Func_Volatile<T, ReturnType, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9>( MethodPtr );

	Method->SetMethodName( MethodName );

	return Method;
}



//////////////////
// Params count: 10
//////////////////
template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8,
         typename P9 >
class clNativeMethodParams10_Func_Volatile < T, void, P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7,
	P8,
	P9 > : public iMethod
{
private:
	typedef void ( T::*MethodPtr )( P0, P1, P2, P3, P4, P5, P6, P7, P8, P9 ) volatile;
	MethodPtr    FMethodPtr;
public:
	clNativeMethodParams10_Func_Volatile( MethodPtr Ptr ): FMethodPtr( Ptr )
	{
	}
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params )
	{
		FATAL( Params.size() != 10, "Not enough parameters passed in clParametersList" );

		( ( ( T* )ObjectAddr )->*FMethodPtr )( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ),
		                                       *( typename TypeTraits<P9>::ReferredType* )( Params[9]->GetNativeBlock() ) );
	}
	virtual int         GetParamsCount() const
	{
		return 10;
	}
	virtual int         GetParamSize( int Index )
	{
		if ( Index == 0 ) { return sizeof( P0 ); }

		if ( Index == 1 ) { return sizeof( P1 ); }

		if ( Index == 2 ) { return sizeof( P2 ); }

		if ( Index == 3 ) { return sizeof( P3 ); }

		if ( Index == 4 ) { return sizeof( P4 ); }

		if ( Index == 5 ) { return sizeof( P5 ); }

		if ( Index == 6 ) { return sizeof( P6 ); }

		if ( Index == 7 ) { return sizeof( P7 ); }

		if ( Index == 8 ) { return sizeof( P8 ); }

		if ( Index == 9 ) { return sizeof( P9 ); }

		return 0;
	}
	virtual iParameter* CreateParameter( int Index, void* InitialValue )
	{
		iParameter* Param = NULL;

		if ( Index == 0 ) { Param = CreateNativeParameter<P0>(); }

		if ( Index == 1 ) { Param = CreateNativeParameter<P1>(); }

		if ( Index == 2 ) { Param = CreateNativeParameter<P2>(); }

		if ( Index == 3 ) { Param = CreateNativeParameter<P3>(); }

		if ( Index == 4 ) { Param = CreateNativeParameter<P4>(); }

		if ( Index == 5 ) { Param = CreateNativeParameter<P5>(); }

		if ( Index == 6 ) { Param = CreateNativeParameter<P6>(); }

		if ( Index == 7 ) { Param = CreateNativeParameter<P7>(); }

		if ( Index == 8 ) { Param = CreateNativeParameter<P8>(); }

		if ( Index == 9 ) { Param = CreateNativeParameter<P9>(); }

		if ( Param && InitialValue ) { Param->ReadValue( InitialValue ); }

		return Param;
	}
};
#endif

/// \endcond

/*
 * 08/07/2010
     Autogenerated via LSDC
*/

