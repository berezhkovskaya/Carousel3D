/**
 * \file TypeLists.h
 * \brief Type lists
 * \version 0.5.91
 * \date 17/01/2005
 * \author
 */

#ifndef _TypeLists_
#define _TypeLists_

#define  TYPELIST_0()   ::Linderdaum::Utils::NullType
#define  TYPELIST_1(T1)  ::Linderdaum::Utils::Typelist<T1, ::Linderdaum::Utils::NullType>
#define  TYPELIST_2(T1, T2)  ::Linderdaum::Utils::Typelist<T1, TYPELIST_1(T2) >
#define  TYPELIST_3(T1, T2, T3)  ::Linderdaum::Utils::Typelist<T1, TYPELIST_2(T2, T3) >
#define  TYPELIST_4(T1, T2, T3, T4)  ::Linderdaum::Utils::Typelist<T1, TYPELIST_3(T2, T3, T4) >
#define  TYPELIST_5(T1, T2, T3, T4, T5)  ::Linderdaum::Utils::Typelist<T1, TYPELIST_4(T2, T3, T4, T5) >
#define  TYPELIST_6(T1, T2, T3, T4, T5, T6)  ::Linderdaum::Utils::Typelist<T1, TYPELIST_5(T2, T3, T4, T5, T6) >
#define  TYPELIST_7(T1, T2, T3, T4, T5, T6, T7)  ::Linderdaum::Utils::Typelist<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7) >
#define  TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8)  ::Linderdaum::Utils::Typelist<T1, TYPELIST_7(T2, T3, T4, T5, T6, T7, T8) >
#define  TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9)  ::Linderdaum::Utils::Typelist<T1, TYPELIST_8(T2, T3, T4, T5, T6, T7, T8, T9) >
#define TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)  ::Linderdaum::Utils::Typelist<T1, TYPELIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10) >
#define TYPELIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)  ::Linderdaum::Utils::Typelist<T1, TYPELIST_10(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) >
#define TYPELIST_12(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)  ::Linderdaum::Utils::Typelist<T1, TYPELIST_11(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) >

#include <string>

namespace Linderdaum
{
	namespace Utils
	{

		// Used as a class type that doesn't hold anything
		class EmptyType {};

		// Used as a placeholder for "no type here"
		class NullType {};

		// The building block of typelists of any length
		// Use it through the TYPELIST_NN macros
		// Defines nested types:
		//     Head (first element, a non-typelist type by convention)
		//     Tail (second element, can be another typelist)
		template <class T, class U>
		struct Typelist
		{
			typedef T Head;
			typedef U Tail;
		};
		//
		template <int I>
		struct Int2Type
		{
			enum { value = I };
		};
		//
		template <class T>
		struct Type2Type
		{
			typedef T OriginalType;
		};
		// Compute length of the typelist
		template <class TList> struct Length;
		template <> struct Length<NullType>
		{
			enum { Value = 0 };
		};
		template <class T, class U>
		struct Length< Typelist<T, U> >
		{
			enum { Value = 1 + Length<U>::Value };
		};
		// Returns the type in position Index in the typelist
		template <class TList, int Index> struct TypeAt;
		template <class Head, class Tail>
		struct TypeAt<Typelist<Head, Tail>, 0>
		{
			typedef Head Result;
		};
		template <class Head, class Tail, int i>
		struct TypeAt<Typelist<Head, Tail>, i>
		{
			typedef typename TypeAt < Tail, i - 1 >::Result Result;
		};
		// IndexOf()
		template <class TList, class T> struct IndexOf;
		template <class T>
		struct IndexOf<NullType, T>
		{
			enum { Value = -1 };
		};
		template <class T, class Tail>
		struct IndexOf<Typelist<T, Tail>, T>
		{
			enum { Value = 0 };
		};
		template <class Head, class Tail, class T>
		struct IndexOf< Typelist<Head, Tail>, T>
		{
		private:
			enum { Temp = IndexOf<Tail, T>::Value };
		public:
			enum { Value = Temp == -1 ? -1 : 1 + Temp };
		};
		// SelectType
		template <bool flag, class T, class F>
		struct SelectType
		{
			typedef T Result;
		};
		template <class T, class F>
		struct SelectType<false, T, F>
		{
			typedef F Result;
		};
		// Returns the type in position Index in the typelist, or DefaultType
		// if Index is out-of-range
		template < class TList, int Index, class DefaultType = NullType >
		struct TypeAtNonStrict
		{
			typedef typename TypeAtNonStrict < typename TList::Tail, Index - 1, DefaultType >::Result Result;
		};
		template<class Head, class Tail, class DefaultType>
		struct TypeAtNonStrict<Typelist<Head, Tail>, 0, DefaultType>
		{
			typedef Head Result;
		};
		template<int Index, class DefaultType>
		struct TypeAtNonStrict<NullType, Index, DefaultType>
		{
			typedef DefaultType Result;
		};
		// IsSameType()
		template <class T1, class T2>
		struct IsSameType
		{
			enum { Result = false };
		};
		template <class T1>
		struct IsSameType<T1, T1>
		{
			enum { Result = true };
		};

		// Conversion

		/// Internal namespace for type lists
		namespace Private
		{
			template <class T, class U>
			struct ConversionHelper
			{
				typedef char Small;
				struct Big
				{
					char dummy[2];
				};
				static Big   Test( ... );
				static Small Test( U );
				static T MakeT();
			};
		}
////////////////////////////////////////////////////////////////////////////////
// class template Conversion
// Figures out the conversion relationships between two types
// Invocations (T and U are types):
// a) Conversion<T, U>::exists
// returns (at compile time) true if there is an implicit conversion from T
// to U (example: Derived to Base)
// b) Conversion<T, U>::exists2Way
// returns (at compile time) true if there are both conversions from T
// to U and from U to T (example: int to char and back)
// c) Conversion<T, U>::sameType
// returns (at compile time) true if T and U represent the same type
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////

		template <class T, class U>
		struct Conversion
		{
			typedef Private::ConversionHelper<T, U> H;
			enum { Exists = sizeof( typename H::Small ) == sizeof( ( H::Test( H::MakeT() ) ) ) };
			enum { Exists2Way = Exists && Conversion<U, T>::Exists };
			enum { SameType = false };
		};

		template <class T>
		struct Conversion<T, T>
		{
			enum { Exists = 1, Exists2Way = 1, SameType = 1 };
		};

		template <class T>
		struct Conversion<void, T>
		{
			enum { Exists = 0, Exists2Way = 0, SameType = 0 };
		};

		template <class T>
		struct Conversion<T, void>
		{
			enum { Exists = 0, Exists2Way = 0, SameType = 0 };
		};

		template <>
		struct Conversion<void, void>
		{
		public:
			enum { Exists = 1, Exists2Way = 1, SameType = 1 };
		};
	}
}

#endif

/*
 * 17/01/2005
     TYPELIST_0()
 * 16/01/2005
     It's here
*/
