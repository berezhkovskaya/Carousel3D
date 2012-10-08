/**
 * \file iStaticClass.h
 * \brief Represents a metaclass of the specified class
 * \version 0.5.99
 * \date 13/12/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author Viktor Latypov, 2007-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iStaticClass_
#define _iStaticClass_

#include "Platform.h"
#include "Method.h"
#include "State.h"
#include "Field.h"
#include "Property.h"
#include "Parameters.h"
#include "Utils/Exceptions.h"

class iObject;

/**
   \brief RTTI metaclass

   Contains layout of the class : list of states, methods, fields and properties.
   Virtual method/field/property/state tables are used in LinderScript virtual machine
   to speed up searching of fields and classes.

   Property list is the basis for serialization facilities.
   LSDC tool autogenerates the serializer function for each property.

   This metaclass is becoming more and more like rsClass in the script compiler.
   Also, the LSDC tool itself might be based on this to store the class information.
**/
class scriptfinal iStaticClass: public clState
{
public:
	EXCEPTIONABLE;
	EXCEPTION( clExcept_AbstractError,         "Unable to instantiate abstract class" );
	EXCEPTION( clExcept_TunnellerIsNotDefined, "Tunneller is not defined for class" );
protected:
	iStaticClass(): FMetaObject( NULL ), FSuperClass( NULL ) {};
public:
	virtual ~iStaticClass();
	//
	// iStaticClass
	//
	virtual iObject*    DefaultConstructor() const
	{
		clParametersList EmptyParams;

		return VirtualConstructor( EmptyParams );
	};

	SERIALIZABLE_CLASS()
	NET_EXPORTABLE()

	/// Check if this class is derived from SClassName
	virtual bool             InheritsFrom( const LString& SClassName );

	/// Create an instance of this class using a constructor with parameters
	virtual iObject*         VirtualConstructor( const clParametersList& Params ) const = 0;

	virtual int              GetConstructorParamsCount( size_t ConstructorID ) const = 0;

	/// Add state to the class
	virtual void             RegisterState( clState* State );

	/// Add field to the class
	virtual void             RegisterField( iField* Field );

	/// Add property to the class
	virtual void             RegisterProperty( iProperty* Property );

	/// Return default constructed object of this class (to know the initial values of the properties)
	virtual iObject*         GetMetaObject() const;

	/// If this class is abstract
	virtual bool             IsAbstract() const = 0;

#pragma region Superclass management and access
	virtual iStaticClass*    GetSuperClass() const;
	virtual void             SetSuperClass( iStaticClass* SuperClass );
	virtual void             SetSuperClassName( const LString& SuperClassName )
	{
		FSuperClassName = SuperClassName;
	};
	virtual LString          GetSuperClassName()
	{
		return FSuperClassName;
	};

	/// We override this to return SuperClass (this is used in virtual method table building)
	virtual clState*         GetSuperState() const
	{
		return FSuperClass;
	}
#pragma endregion

	virtual iMethod*         FindMethod( const LString& MethodName, int Inherited ) const;
	virtual clState*         FindState( const LString& StateName );
	virtual iField*          FindField( const LString& FieldName );
	virtual iProperty*       FindProperty( const LString& PropertyName );

	/// Get a total number of properties for this class (including ancestors)
	virtual int              GetPropertiesCount() const;

	/// Get a property for specified index in a global (this + ancestors) class list
	virtual iProperty*       GetPropertyByIndex( int Idx ) const;

	/// Get a total number of fields for this class (including ancestors)
	virtual int              GetFieldsCount() const;

	/// Get a field for specified index in a global (this + ancestors) class list
	virtual iField*          GetFieldByIndex( int Idx ) const;

	/// Get a total number of methods for this class (including ancestors)
	virtual int              GetMethodsCount() const;

	/// Get a method for specified index in a global method list
	virtual iMethod*         GetMethodByLinearIndex( int Idx ) const;

	/**
	  Register each field from metaclass in a concrete instance of an iObject

	 Field description resides in a metaclass,
	 so after the iObject creation we must allocate the memory for field's content.
	 This is done, of course, only for the scripted classes. Native fields are already allocated by native compiler.
	*/
	virtual void             RegisterNonNativeFields( iObject* Object );

	/**
	  VMT/VFT/VPT/StateTable building : called for each metaclass after registration/loading

	  Remark : this method is called only after the whole hierarchy is loaded and registered, because it requires all the base classes
	 */
	virtual void             BuildVirtualTables();

#pragma region Linear entity accessors
	virtual int              GetStateIndexByName( const LString& StateName ) const;
	virtual int              GetFieldIndexByName( const LString& FieldName ) const;
	virtual int              GetPropertyIndexByName( const LString& PropertyName ) const;
#pragma endregion

	/** Property(Name=SuperClass, Type=string, Getter=GetSuperClassName, Setter=SetSuperClassName) */
protected:

#pragma region Propery/Field/State containers
	typedef std::map<LString, clState*>    clStatesList;
	typedef std::map<LString, iField*>     clFieldsList;
	typedef std::map<LString, iProperty*>  clPropertiesList;

	clStatesList                           FStates;
	clFieldsList                           FFields;
	clPropertiesList                       FProperties;
#pragma endregion

protected:

#pragma region VFT/VPT/VMT/StateTable stuff
	typedef std::map<LString, int>         clNameToIndexMap;

	typedef std::vector<clState*>          clLinearStatesList;
	typedef std::vector<iField*>           clLinearFieldsList;
	typedef std::vector<iProperty*>        clLinearPropertiesList;

	mutable clNameToIndexMap               FFieldNameToIndex;
	mutable clNameToIndexMap               FPropertyNameToIndex;
	mutable clNameToIndexMap               FStateNameToIndex;
	clLinearStatesList                     FLinearStatesList;
	clLinearFieldsList                     FLinearFieldsList;
	clLinearPropertiesList                 FLinearPropertiesList;

	void                       BuildVirtualFieldTable( int CurrentFieldIndex, clLinearFieldsList* LinearList );
	void                       BuildVirtualPropertyTable( int CurrentPropertyIndex, clLinearPropertiesList* LinearList );
	void                       BuildStateTable( int CurrentStateIndex, clLinearStatesList* LinearList );
#pragma endregion

protected:
	mutable iObject*           FMetaObject;
	mutable iStaticClass*      FSuperClass;
	LString                    FSuperClassName;
};

//
//
//

template <class T> class clNativeAbstractStaticClass: public iStaticClass
{
public:
	clNativeAbstractStaticClass() {};
	virtual ~clNativeAbstractStaticClass() {};
	//
	// iObject interface
	//
	virtual LString     ClassName() const
	{
		LString Name( typeid( T ).name() );

		GCC_CONVERT_TYPEINFO( Name );

		return Name;
	}
	//
	// iStaticClass interface
	//
	virtual iObject*    VirtualConstructor( const clParametersList& Params ) const
	{
		RAISE_MSG( clExcept_AbstractError, ClassName() );

		return NULL;
	}
	virtual int         GetConstructorParamsCount( size_t ConstructorID ) const { return 0; };
	virtual bool        IsAbstract() const { return true; };
};

#define SET_BASE_AND_RETURN() Obj->SetStaticClass( this ); \
                              Obj->Env = this->Env; \
                              Obj->AfterConstruction(); \
                              return Obj;
#define CHECK_PARAMS(N) FATAL_ENV( this->Env, Params.size() < N, this->ClassName() + " constructor expects "#N" parameters, but only " + LStr::ToStr( Params.size() ) + " was given." );

template <class T> class clNativeStaticClass: public clNativeAbstractStaticClass<T>
{
public:
	clNativeStaticClass() {};
	virtual ~clNativeStaticClass() {};
	//
	// iStaticClass interface
	//
	virtual iObject*    VirtualConstructor( const clParametersList& Params ) const
	{
		CHECK_PARAMS( 0 );

		iObject* Obj = new T;

		SET_BASE_AND_RETURN();
	}
	virtual int         GetConstructorParamsCount( size_t ConstructorID ) const { return 0; };
	virtual bool        IsAbstract() const { return false; };
};

template <class T, typename P0> class clNativeStaticClass1: public clNativeAbstractStaticClass<T>
{
public:
	clNativeStaticClass1() {};
	virtual ~clNativeStaticClass1() {};
	//
	// iStaticClass interface
	//
	virtual iObject*    VirtualConstructor( const clParametersList& Params ) const
	{
		CHECK_PARAMS( 1 );

		iObject* Obj = new T( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ) );

		SET_BASE_AND_RETURN();
	}
	virtual int         GetConstructorParamsCount( size_t ConstructorID ) const { return 1; };
	virtual bool        IsAbstract() const { return false; };
};

template < class T, typename P0,
         typename P1 > class clNativeStaticClass2: public clNativeAbstractStaticClass<T>
{
public:
	clNativeStaticClass2() {};
	virtual ~clNativeStaticClass2() {};
	//
	// iStaticClass interface
	//
	virtual iObject*    VirtualConstructor( const clParametersList& Params ) const
	{
		CHECK_PARAMS( 2 );

		iObject* Obj = new T( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                      *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ) );

		SET_BASE_AND_RETURN();
	}
	virtual int         GetConstructorParamsCount( size_t ConstructorID ) const { return 2; };
	virtual bool        IsAbstract() const { return false; };
};

template < class T, typename P0,
         typename P1,
         typename P2 > class clNativeStaticClass3: public clNativeAbstractStaticClass<T>
{
public:
	clNativeStaticClass3() {};
	virtual ~clNativeStaticClass3() {};
	//
	// iStaticClass interface
	//
	virtual iObject*    VirtualConstructor( const clParametersList& Params ) const
	{
		CHECK_PARAMS( 3 );

		iObject* Obj = new T( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                      *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                      *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ) );

		SET_BASE_AND_RETURN();
	}
	virtual int         GetConstructorParamsCount( size_t ConstructorID ) const { return 3; };
	virtual bool        IsAbstract() const { return false; };
};

template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3 > class clNativeStaticClass4: public clNativeAbstractStaticClass<T>
{
public:
	clNativeStaticClass4() {};
	virtual ~clNativeStaticClass4() {};
	//
	// iStaticClass interface
	//
	virtual iObject*    VirtualConstructor( const clParametersList& Params ) const
	{
		CHECK_PARAMS( 4 );

		iObject* Obj = new T( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                      *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                      *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                      *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ) );

		SET_BASE_AND_RETURN();
	}
	virtual int         GetConstructorParamsCount( size_t ConstructorID ) const { return 4; };
	virtual bool        IsAbstract() const { return false; };
};

template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4 > class clNativeStaticClass5: public clNativeAbstractStaticClass<T>
{
public:
	clNativeStaticClass5() {};
	virtual ~clNativeStaticClass5() {};
	//
	// iStaticClass interface
	//
	virtual iObject*    VirtualConstructor( const clParametersList& Params ) const
	{
		CHECK_PARAMS( 5 );

		iObject* Obj = new T( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                      *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                      *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                      *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                      *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ) );

		SET_BASE_AND_RETURN();
	}
	virtual int         GetConstructorParamsCount( size_t ConstructorID ) const { return 5; };
	virtual bool        IsAbstract() const { return false; };
};

template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5 > class clNativeStaticClass6: public clNativeAbstractStaticClass<T>
{
public:
	clNativeStaticClass6() {};
	virtual ~clNativeStaticClass6() {};
	//
	// iStaticClass interface
	//
	virtual iObject*    VirtualConstructor( const clParametersList& Params ) const
	{
		CHECK_PARAMS( 6 );

		iObject* Obj = new T( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                      *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                      *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                      *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                      *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                      *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ) );

		SET_BASE_AND_RETURN();
	}
	virtual int         GetConstructorParamsCount( size_t ConstructorID ) const { return 6; };
	virtual bool        IsAbstract() const { return false; };
};

template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6 > class clNativeStaticClass7: public clNativeAbstractStaticClass<T>
{
public:
	clNativeStaticClass7() {};
	virtual ~clNativeStaticClass7() {};
	//
	// iStaticClass interface
	//
	virtual iObject*    VirtualConstructor( const clParametersList& Params ) const
	{
		CHECK_PARAMS( 7 );

		iObject* Obj = new T( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                      *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                      *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                      *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                      *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                      *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                      *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ) );

		SET_BASE_AND_RETURN();
	}
	virtual int         GetConstructorParamsCount( size_t ConstructorID ) const { return 7; };
	virtual bool        IsAbstract() const { return false; };
};

template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7 > class clNativeStaticClass8: public clNativeAbstractStaticClass<T>
{
public:
	clNativeStaticClass8() {};
	virtual ~clNativeStaticClass8() {};
	//
	// iStaticClass interface
	//
	virtual iObject*    VirtualConstructor( const clParametersList& Params ) const
	{
		CHECK_PARAMS( 8 );

		iObject* Obj = new T( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                      *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                      *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                      *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                      *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                      *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                      *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                      *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ) );

		SET_BASE_AND_RETURN();
	}
	virtual int         GetConstructorParamsCount( size_t ConstructorID ) const { return 8; };
	virtual bool        IsAbstract() const { return false; };
};

template < class T, typename P0,
         typename P1,
         typename P2,
         typename P3,
         typename P4,
         typename P5,
         typename P6,
         typename P7,
         typename P8 > class clNativeStaticClass9: public clNativeAbstractStaticClass<T>
{
public:
	clNativeStaticClass9() {};
	virtual ~clNativeStaticClass9() {};
	//
	// iStaticClass interface
	//
	virtual iObject*    VirtualConstructor( const clParametersList& Params ) const
	{
		CHECK_PARAMS( 9 );

		iObject* Obj = new T( *( typename TypeTraits<P0>::ReferredType* )( Params[0]->GetNativeBlock() ),
		                      *( typename TypeTraits<P1>::ReferredType* )( Params[1]->GetNativeBlock() ),
		                      *( typename TypeTraits<P2>::ReferredType* )( Params[2]->GetNativeBlock() ),
		                      *( typename TypeTraits<P3>::ReferredType* )( Params[3]->GetNativeBlock() ),
		                      *( typename TypeTraits<P4>::ReferredType* )( Params[4]->GetNativeBlock() ),
		                      *( typename TypeTraits<P5>::ReferredType* )( Params[5]->GetNativeBlock() ),
		                      *( typename TypeTraits<P6>::ReferredType* )( Params[6]->GetNativeBlock() ),
		                      *( typename TypeTraits<P7>::ReferredType* )( Params[7]->GetNativeBlock() ),
		                      *( typename TypeTraits<P8>::ReferredType* )( Params[8]->GetNativeBlock() ) );

		SET_BASE_AND_RETURN();
	}
	virtual int         GetConstructorParamsCount( size_t ConstructorID ) const { return 9; };
	virtual bool        IsAbstract() const { return false; };
};

#undef SET_BASE_AND_RETURN
#undef CHECK_PARAMS

#endif

/*
 * 13/12/2010
     Constructors with parameters
 * 26/09/2010
     Serialization support
 * 04/07/2010
     Property management
 * 01/04/2009
     VMT-related stuff
 * 02/02/2008
     iClass deprecated
 * 20/12/2005
     FindState()
 * 06/11/2005
     Scripted staticclass moved to a separate file
 * 05/11/2005
     Parametrization of clScriptedStaticClass
 * 04/11/2005
     clParametersList passed to VirtualConstructor()
 * 02/11/2005
     DefaultConstructor()
 * 06/09/2005
     clNativeAbstractStaticClass
 * 23/07/2005
     GetSuperClass()
     SetSuperClass()
 * 22/07/2005
     FindMethod()
 * 16/07/2005
     clScriptedStaticClass
 * 11/07/2005
     It's here
*/
