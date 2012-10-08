/**
 * \file TypeTraits.h
 * \brief Type traits
 * \version 0.5.91
 * \date 30/10/2005
 * \author
 */

#ifndef _TypeTraits_
#define _TypeTraits_

#include "TypeLists.h"

#include <string>

using namespace ::Linderdaum::Utils;

namespace Linderdaum
{
	namespace Utils
	{
		//////////////////////////////////////////////////////////////////////////
		// Helper types for class template TypeTraits defined below
		//////////////////////////////////////////////////////////////////////////
		namespace Private
		{
			typedef TYPELIST_4( unsigned char, unsigned short int,
			                    unsigned int, unsigned long int )    StdUnsignedInts;
			typedef TYPELIST_4( signed char, short int,
			                    int, long int )                      StdSignedInts;
			typedef TYPELIST_3( bool, char, wchar_t )                StdOtherInts;
			typedef TYPELIST_3( float, double, long double )         StdFloats;

			template <class U> struct AddReference
			{
				typedef U& Result;
			};

			template <class U> struct AddReference<U&>
			{
				typedef U& Result;
			};

			template <> struct AddReference<void>
			{
				typedef NullType Result;
			};
		}

/// \cond

////////////////////////////////////////////////////////////////////////////////
// class template TypeTraits
// Figures out various properties of any given type
// Invocations (T is a type):
// a) TypeTraits<T>::isPointer
// returns (at compile time) true if T is a pointer type
// b) TypeTraits<T>::PointeeType
// returns the type to which T points is T is a pointer type, NullType otherwise
// a) TypeTraits<T>::isReference
// returns (at compile time) true if T is a reference type
// b) TypeTraits<T>::ReferredType
// returns the type to which T refers is T is a reference type, NullType
// otherwise
// c) TypeTraits<T>::isMemberPointer
// returns (at compile time) true if T is a pointer to member type
// d) TypeTraits<T>::isStdUnsignedInt
// returns (at compile time) true if T is a standard unsigned integral type
// e) TypeTraits<T>::isStdSignedInt
// returns (at compile time) true if T is a standard signed integral type
// f) TypeTraits<T>::isStdIntegral
// returns (at compile time) true if T is a standard integral type
// g) TypeTraits<T>::isStdFloat
// returns (at compile time) true if T is a standard floating-point type
// h) TypeTraits<T>::isStdArith
// returns (at compile time) true if T is a standard arithmetic type
// i) TypeTraits<T>::isStdFundamental
// returns (at compile time) true if T is a standard fundamental type
// j) TypeTraits<T>::isUnsignedInt
// returns (at compile time) true if T is a unsigned integral type
// k) TypeTraits<T>::isSignedInt
// returns (at compile time) true if T is a signed integral type
// l) TypeTraits<T>::isIntegral
// returns (at compile time) true if T is a integral type
// m) TypeTraits<T>::isFloat
// returns (at compile time) true if T is a floating-point type
// n) TypeTraits<T>::isArith
// returns (at compile time) true if T is a arithmetic type
// o) TypeTraits<T>::isFundamental
// returns (at compile time) true if T is a fundamental type
// p) TypeTraits<T>::ParameterType
// returns the optimal type to be used as a parameter for functions that take Ts
// q) TypeTraits<T>::isConst
// returns (at compile time) true if T is a const-qualified type
// r) TypeTraits<T>::NonConstType
// removes the 'const' qualifier from T, if any
// s) TypeTraits<T>::isVolatile
// returns (at compile time) true if T is a volatile-qualified type
// t) TypeTraits<T>::NonVolatileType
// removes the 'volatile' qualifier from T, if any
// u) TypeTraits<T>::UnqualifiedType
// removes both the 'const' and 'volatile' qualifiers from T, if any
////////////////////////////////////////////////////////////////////////////////

		template <typename T>
		class TypeTraits
		{
		private:
			template <class U> struct PointerTraits
			{
				enum { Result = false };
				typedef NullType PointeeType;
			};

			template <class U> struct PointerTraits<U*>
			{
				enum { Result = true };
				typedef U PointeeType;
			};

			template <class U> struct ReferenceTraits
			{
				enum { Result = false };
				typedef U ReferredType;
			};

			template <class U> struct ReferenceTraits<U&>
			{
				enum { Result = true };
				typedef U ReferredType;
			};


			template <class U> struct PToMTraits
			{
				enum { Result = false };
			};

			template <class U, class V>
			struct PToMTraits<U V::*>
			{
				enum { Result = true };
			};

			template <class U> struct UnConst
			{
				typedef U Result;
				enum { IsConst = 0 };
			};

			template <class U> struct UnConst<const U>
			{
				typedef U Result;
				enum { IsConst = 1 };
			};

			template <class U> struct UnVolatile
			{
				typedef U Result;
				enum { IsVolatile = 0 };
			};

			template <class U> struct UnVolatile<volatile U>
			{
				typedef U Result;
				enum { IsVolatile = 1 };
			};
		public:
			enum { IsPointer = PointerTraits<T>::Result };
			typedef typename PointerTraits<T>::PointeeType PointeeType;

			enum { IsReference = ReferenceTraits<T>::Result };
			typedef typename ReferenceTraits<T>::ReferredType ReferredType;

			enum { IsMemberPointer = PToMTraits<T>::Result };

			enum { IsStdUnsignedInt =
			          IndexOf<Private::StdUnsignedInts, T>::Value >= 0
			     };
			enum { IsStdSignedInt =
			          IndexOf<Private::StdSignedInts, T>::Value >= 0
			     };
			enum { IsStdIntegral = IsStdUnsignedInt || IsStdSignedInt ||
			                       IndexOf<Private::StdOtherInts, T>::Value >= 0
			     };
			enum { IsStdFloat = IndexOf<Private::StdFloats, T>::Value >= 0 };
			enum { IsStdArith = IsStdIntegral || IsStdFloat };
			enum { IsStdFundamental = IsStdArith || IsStdFloat ||
			                          Conversion<T, void>::SameType
			     };

			enum { IsUnsignedInt = IsStdUnsignedInt };
			enum { IsSignedInt = IsStdSignedInt };
			enum { IsIntegral = IsStdIntegral || IsUnsignedInt || IsSignedInt };
			enum { IsFloat = IsStdFloat };
			enum { IsArith = IsIntegral || IsFloat };
			enum { IsFundamental = IsStdFundamental || IsArith || IsFloat };
			enum { IsStruct = !IsStdFundamental };
			enum { IsString = Conversion< T, std::string >::Exists2Way || Conversion< T, std::string& >::Exists };

			typedef typename SelectType < IsStdArith || IsPointer || IsMemberPointer,
			        T, typename Private::AddReference<T>::Result >::Result ParameterType;

			enum { IsConst = UnConst<T>::IsConst };
			typedef typename UnConst<T>::Result NonConstType;
			enum { IsVolatile = UnVolatile<T>::IsVolatile };
			typedef typename UnVolatile<T>::Result NonVolatileType;
			typedef typename UnVolatile<typename UnConst<T>::Result>::Result
			UnqualifiedType;
		};

/// \endcond

	}
}

#endif

/*
 * 30/10/2005
     It's here
*/
