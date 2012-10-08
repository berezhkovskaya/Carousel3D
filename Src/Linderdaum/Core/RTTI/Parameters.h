/**
 * \file Parameters.h
 * \brief Helpers to work with the class' methods and controls how parameters
 *        are tunnelled into and from LinderScript
 *
 *  Based on work of Viktor Latypov and provides a more type-save implementation
 *  of his rsParameter.
 *
 * \version 0.5.60
 * \date 02/12/2005
 * \author Sergey Kosarevsky, 2005
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iParameter_
#define _iParameter_

#include "Platform.h"
#include "Core/iObject.h"
#include "Utils/TypeLists.h"
#include "Utils/TypeTraits.h"
#include "Utils/iStack.h"
#include "Utils/Utils.h"

#include <memory>

class iParameter;

/// Generic parameter interface for LinderScript
class iParameter
{
public:
	iParameter() {};
	virtual ~iParameter() {};
	//
	// iParameter
	//
	/// Return symbolic type name
	virtual LString  GetParamType() const = 0;

	/// Get the storage size for this parameter
	virtual int      GetParamSize() const = 0;

	/// Load value from some memory block
	virtual void     ReadValue( const void* UntypedNativeBlock ) = 0;

	/// Load value from string representation
	virtual void     ReadValueFromString( const LString& Str ) = 0;

	/// Convert value to string
	virtual LString  ToString() const = 0;

	/// Get direct usable void pointer to the parameter container (can be passed to actual function)
	virtual void*    GetNativeBlock() = 0;

	/// Store the value on top of the VM stack
	virtual void     PushToStack( iStack* Stack ) const = 0;

	/// Fetch the value from the top of the VM stack
	virtual void     PopFromStack( iStack* Stack ) = 0;
};

/// implementation for std::string
class clStringParameter: public iParameter
{
public:
	//
	// iParameter interface
	//
	virtual LString  GetParamType() const { return "STRING"; }
	virtual int      GetParamSize() const { return sizeof( LStringBuffer ); }
	virtual void     ReadValue( const void* UntypedNativeBlock ) { FString = *reinterpret_cast<const LString*>( UntypedNativeBlock ); }
	virtual void     ReadValueFromString( const LString& Str ) { FString = Str; }
	virtual void*    GetNativeBlock() { return &FString; }
	virtual LString  ToString() const { return FString; }
	virtual void     PushToStack( iStack* Stack ) const { Stack->Push( LStringBuffer( FString ) ); }
	virtual void     PopFromStack( iStack* Stack ) { FString = LStringBuffer( Stack->Pop<LStringBuffer>() ).ToString(); }
private:
	LString    FString;
};

/// implementation for plain old data (POD) parameters
class clScriptedPODParameter_Buffer: public iParameter
{
public:
	clScriptedPODParameter_Buffer( int Size, const LString& TypeName, void* TheBuffer ): FSize( Size ),
		FTypeName( TypeName ), FParameter( TheBuffer ) {};

	virtual ~clScriptedPODParameter_Buffer() {}
	//
	// iParameter interface
	//
	virtual LString  GetParamType() const { return "SCRIPTED_POD"; }
	virtual int      GetParamSize() const { return FSize; }
	virtual void     ReadValue( const void* UntypedNativeBlock ) { memcpy( FParameter, UntypedNativeBlock, FSize ); }
	virtual void*    GetNativeBlock() { return FParameter; }
	virtual void     PushToStack( iStack* Stack ) const { Stack->PushBytes( FSize, FParameter ); }
	virtual void     PopFromStack( iStack* Stack ) { Stack->PopBytes( FSize, FParameter ); }

	virtual void     ReadValueFromString( const LString& Str )
	{
		FIXME( "why hardcoded types?" )

		if ( FTypeName == "int"    )
		{
			*( int* )FParameter    = LStr::ToInt( Str );
		}
		else if ( FTypeName == "float"  )
		{
			*( float* )FParameter  = LStr::ToFloat( Str );
		}
		else if ( FTypeName == "double" )
		{
			*( double* )FParameter = LStr::ToDouble( Str );
		}
		else if ( FTypeName == "bool"   )
		{
			*( bool* )FParameter   = LStr::ToBool( Str );
		}
	}

	virtual LString  ToString() const
	{
		if ( FTypeName == "int"    )
		{
			return LStr::ToStr( *( int* )FParameter );
		}
		else if ( FTypeName == "float"  )
		{
			return LStr::ToStr( *( float* )FParameter );
		}
		else if ( FTypeName == "double" )
		{
			return LStr::ToStr( *( double* )FParameter );
		}
		else if ( FTypeName == "bool"   )
		{
			return LStr::ToStr( *( bool* )FParameter );
		}
		else
		{
			return "POD_Value_" + FTypeName;
		}
	}
protected:
	int        FSize;
	LString    FTypeName;
	void*      FParameter;
};

class clScriptedPODParameter: public clScriptedPODParameter_Buffer
{
public:
	clScriptedPODParameter( int Size, const LString& TypeName ): clScriptedPODParameter_Buffer( Size, TypeName, malloc( Size ) ) {}

	virtual ~clScriptedPODParameter() { free( FParameter ); }
};

/// template for automatic extraction of POD size
template <class T> class clPODParameter: public clScriptedPODParameter
{
public:
	clPODParameter(): clScriptedPODParameter( sizeof( T ), typeid( T ).name() ) {};
};

///implementation for pointers and references
template <typename T> class clPointerParameter: public iParameter
{
public:
	clPointerParameter(): FPointer( NULL ) {};
	//
	// iParameter interface
	//
	virtual LString  GetParamType() const { return "PTR"; }
	virtual int      GetParamSize() const { return sizeof( T* ); }
	virtual void     ReadValue( const void* UntypedNativeBlock ) { FPointer = *reinterpret_cast<T**>( const_cast<void*>( UntypedNativeBlock ) ); }
	/// Later we might decode the GUID to some iObject* pointer
	virtual void     ReadValueFromString( const LString& Str ) { }
	virtual void*    GetNativeBlock() { return &FPointer; }
	virtual void     PushToStack( iStack* Stack ) const { Stack->Push<T*>( FPointer ); }
	virtual void     PopFromStack( iStack* Stack ) { FPointer = Stack->Pop<T*>(); }
	virtual LString  ToString() const
	{
		iObject* Ptr = ::Linderdaum::Utils::ultimate_cast<iObject*>( FPointer );
		iObject* Obj = ::Linderdaum::Utils::safe_cast<iObject*>( Ptr );

		return ( Obj ) ? Obj->GetObjectID() : LString( "pointer" );
	}
private:
	mutable T*   FPointer;
};

#define PARAMETER( selector, type ) typename SelectType< \
                                             TypeTraits<T>::selector, type,

#define PTR_TRAITS typename ::Linderdaum::Utils::TypeTraits<T>::PointeeType
#define REF_TRAITS typename ::Linderdaum::Utils::TypeTraits<T>::ReferredType

using namespace ::Linderdaum::Utils;

template <class T> struct ParameterType
{
	typedef
	PARAMETER( IsString,         clStringParameter              )
	PARAMETER( IsReference,      clPODParameter<REF_TRAITS>     )
	PARAMETER( IsPointer,        clPointerParameter<PTR_TRAITS> )
	PARAMETER( IsFundamental,    clPODParameter<T>              )
	// default
	clPODParameter<T>
	>::Result
	>::Result
	>::Result
	>::Result
	Type;
};

template <class T> inline iParameter* CreateNativeParameter()
{
	return new ( typename ParameterType<T>::Type );
}


template <class T> inline iParameter* CreateInvokeParameter( T P )
{
	iParameter* Param = CreateNativeParameter<T>();

	Param->ReadValue( &P );

	return Param;
}
/**
   For each parameter we allocate some space on the stack
   and call the in-place new()

   The parameter binder has the static sizeof(ParamType<T>::Type) size.
*/
#define DECLARE_AND_ALLOC_PARAMETER(ParamIdx, ParamType, ParamValue) \
   /* Call inplace new() */ \
   ParamBinders[ParamIdx] = new ( alloca( sizeof(ParameterType<ParamType>::Type) ) ) (typename ParameterType<T>::Type ); \
   /* Read the actual value */ \
   ParamVar ->ReadValue( &ParamValue );

/*
   Later the LSDC generates binding code:

class SomeTuneller
{
public:
   virtual void SomeMethod(Type1* Param1, Type2 Param2, const LString& Param3)
   {
      iParameter* ParamBinders[3];

      DECLARE_AND_ALLOC_PARAMETER(0, Type1*)
      DECLARE_AND_ALLOC_PARAMETER(1, Type2)
      DECLARE_AND_ALLOC_PARAMETER(2, const LString&)

      ...
   }
};

The old code

         clParametersList Params;

         ParameterType<const LMatrix4&>::Type Param0;
         ParameterType<const LMatrix4&>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "RenderOverlay", Params, iObject::FInheritedCall );

is replaced by

   iParameter* ParamBinders[2];

   DECLARE_AND_ALLOC_PARAMETER(0, const LMatrix4&)
   DECLARE_AND_ALLOC_PARAMETER(1, const LMatrix4&)

   bool MethodCalled = iObject::CallMethod_Fast( "RenderOverlay", ParamBinders, iObject::FInheritedCall );
*/

#endif

/*
 * 02/12/2005
     Native block allocators removed
 * 07/11/2005
     CreateInvokeParameter()
 * 02/11/2005
     clParametersList moved here
 * 01/11/2005
     Small fixes of ParameterType struct to make it GCC-compatible
 * 30/10/2005
     It's here
*/
