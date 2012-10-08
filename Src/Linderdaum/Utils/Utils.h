/**
 * \file Utils.h
 * \brief Utility functions
 * \version 0.5.86
 * \date 01/01/2008
 * \author Sergey Kosarevsky, 2005-2008
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _Utils_
#define _Utils_

#include "Platform.h"
#include "Utils/Exceptions.h"
#include "Environment.h"

#include <map>

namespace Linderdaum
{
	namespace Utils
	{
		/// Use dynamic_cast and invoke FATAL if result is NULL
		template <class Target, class Source>
		inline Target guarded_cast( Source* x, const LString& Message, sEnvironment* Env )
		{
			Target cast = dynamic_cast<Target>( x );
			FATAL( !cast, Message );
			return cast;
		}
		/// Cast any type into any other type
		template <class Target, class Source>
		inline Target ultimate_cast( Source x )
		{
			union
			{
				Source S;
				Target T;
			} Caster;
			Caster.S = x;
			return Caster.T;
		}
		/// Use dynamic_cast and return NULL on any exception caught
		template <class Target, class Source>
		inline Target safe_cast( Source* object )
		{
			try
			{
				return dynamic_cast<Target>( object );
			}
			catch ( ... )
			{
				return NULL;
			}
		}
		/// Deallocate a container of objects from _First to _Last iterators
		template <class _InIt>
		inline void Deallocate( _InIt _First, _InIt _Last )
		{
			for ( ; _First != _Last; ++_First )
			{
				delete( *_First );
			}
		}
		/// Deallocate a std::map container of objects from _First to _Last iterators
		template <class _InIt>
		inline void DeallocatePairs( _InIt _First, _InIt _Last )
		{
			for ( ; _First != _Last; ++_First )
			{
				delete( ( *_First ).second );
			}
		}
		/// Deallocate all objects in container
		template <class T>
		inline void DeallocateAll( T& Container )
		{
			Deallocate( Container.begin(), Container.end() );
			Container.clear();
		}
		/// For internal usage in sEnvironment
		template <class T>
		inline void PreClean( T& Container )
		{
			T Old = Container;

			Container = NULL;

			delete( Old );
		}
		/// For internal usage in sEnvironment
		template <class T>
		inline void PostClean( T& Container )
		{
			//delete( Container );
			if ( Container ) { Container->DisposeObject(); }

			Container = NULL;
		}
		/// Returns maximum of two value
		template <class T>
		inline T Max( const T a, const T b )
		{
			return ( a > b ) ? a : b;
		}
		/// Universal deletion template
		class clDeletionHelper
		{
		public:
			template <class T> inline void operator ()( T t ) const
			{
				delete( t );
			}
			template <class T, class U> inline void operator ()( std::pair<T, U>& t ) const
			{
				delete( t.second );
			}
		};
		/// Translate keycode into a character
		int TranslateKey( const int Key, const bool ShiftPressed = false );
		/// Returns 'true' if Key has is an alphabetic or a numeric value (ASCII only)
		inline bool IsKeyAlphaNumeric( const int Key )
		{
			const int B = TranslateKey( Key );

			return ( B >= 32  ) &&
			       ( B <= 122 ) &&
			       ( B != 96  );
		}
		//
	}
}

#endif

/*
 * 01/01/2008
     safe_cast() is now GCC compatible
 * 11/01/2005
     It's here
*/
