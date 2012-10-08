/**
 * \file Functors.h
 * \brief Functors
 * \version 0.5.91
 * \date 29/10/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _Functors_
#define _Functors_

#include <memory>
#include "Platform.h"
#include "Utils/TypeLists.h"

#define IMPLEMENT_CLONE_METHOD(Cls) virtual Cls* Clone() const { return new Cls(*this); };

#define BIND(Method) Utils::Bind(Method, this)

namespace Linderdaum
{
	namespace Utils
	{
		// forwards
		template <class R, class TList> class clFunctorImpl;
		template <class ParentFunctor, class Fun> class clFunctionHandler;
		template <class ParentFunctor, class Object, class Method> class clMethodHandler;
		// clFunctor class definition
		template <class ResultType, class TList>
		class clFunctor
		{
		private:
			typedef clFunctorImpl<ResultType, TList> Impl;
		public:
			typedef ResultType                                            ParentResultType;
			typedef TList                                                 ParamList;
			typedef typename TypeAtNonStrict<TList, 0, EmptyType>::Result Param1;
			typedef typename TypeAtNonStrict<TList, 1, EmptyType>::Result Param2;
			typedef typename TypeAtNonStrict<TList, 2, EmptyType>::Result Param3;
			typedef typename TypeAtNonStrict<TList, 3, EmptyType>::Result Param4;
			typedef typename TypeAtNonStrict<TList, 4, EmptyType>::Result Param5;
			typedef typename TypeAtNonStrict<TList, 5, EmptyType>::Result Param6;
			typedef typename TypeAtNonStrict<TList, 6, EmptyType>::Result Param7;
			typedef typename TypeAtNonStrict<TList, 7, EmptyType>::Result Param8;
			typedef typename TypeAtNonStrict<TList, 8, EmptyType>::Result Param9;
			typedef typename TypeAtNonStrict<TList, 9, EmptyType>::Result Param10;
			typedef typename TypeAtNonStrict<TList, 10, EmptyType>::Result Param11;
		public:
			clFunctor(): FImpl( 0 ) {}
			clFunctor( const clFunctor& Functor ): FImpl( clFunctor::Clone( Functor.FImpl.get() ) ) {}
			template <class Fun> explicit clFunctor( Fun fun ): FImpl( new clFunctionHandler<clFunctor, Fun>( fun ) ) {}
			template <class Object, class Method> explicit clFunctor( const Object& Obj, const Method Meth ): FImpl( new clMethodHandler<clFunctor, Object, Method>( Obj, Meth ) ) {}
			~clFunctor() {};
			inline bool IsEqual( const clFunctor& Functor ) const
			{
				return FImpl.get()->GetUID() == Functor.FImpl.get()->GetUID();
			}
			inline bool IsObject( void* Obj ) const
			{
				return reinterpret_cast<void*>( FImpl.get()->GetUID() ) == Obj;
			}
			inline void* GetObjectPtr() const
			{
				return FImpl.get()->GetObjectPtr();
			}
			template <class U>
			static U* Clone( U* pObj )
			{
				if ( !pObj ) { return 0; }

				U* pClone = static_cast<U*>( pObj->Clone() );

				return pClone;
			}
			clFunctor& operator = ( const clFunctor& Functor )
			{
				clFunctor copy( Functor );
				// swap auto_ptrs by hand
				Impl* p = FImpl.release();
				FImpl.reset( copy.FImpl.release() );
				copy.FImpl.reset( p );
				return *this;
			};
			ResultType operator()()
			{
				return ( *FImpl )();
			}
			ResultType operator()( Param1 P1 )
			{
				return ( *FImpl )( P1 );
			}
			ResultType operator()( Param1 P1, Param2 P2 )
			{
				return ( *FImpl )( P1, P2 );
			}
			ResultType operator()( Param1 P1, Param2 P2, Param3 P3 )
			{
				return ( *FImpl )( P1, P2, P3 );
			}
			ResultType operator()( Param1 P1, Param2 P2, Param3 P3, Param4 P4 )
			{
				return ( *FImpl )( P1, P2, P3, P4 );
			}
			ResultType operator()( Param1 P1, Param2 P2, Param3 P3, Param4 P4, Param5 P5 )
			{
				return ( *FImpl )( P1, P2, P3, P4, P5 );
			}
			ResultType operator()( Param1 P1, Param2 P2, Param3 P3, Param4 P4, Param5 P5, Param6 P6 )
			{
				return ( *FImpl )( P1, P2, P3, P4, P5, P6 );
			}
			ResultType operator()( Param1 P1, Param2 P2, Param3 P3, Param4 P4, Param5 P5, Param6 P6, Param7 P7 )
			{
				return ( *FImpl )( P1, P2, P3, P4, P5, P6, P7 );
			}
			ResultType operator()( Param1 P1, Param2 P2, Param3 P3, Param4 P4, Param5 P5, Param6 P6, Param7 P7, Param8 P8 )
			{
				return ( *FImpl )( P1, P2, P3, P4, P5, P6, P7, P8 );
			}
			ResultType operator()( Param1 P1, Param2 P2, Param3 P3, Param4 P4, Param5 P5, Param6 P6, Param7 P7, Param8 P8, Param9 P9 )
			{
				return ( *FImpl )( P1, P2, P3, P4, P5, P6, P7, P8, P9 );
			}
			ResultType operator()( Param1 P1, Param2 P2, Param3 P3, Param4 P4, Param5 P5, Param6 P6, Param7 P7, Param8 P8, Param9 P9, Param10 P10 )
			{
				return ( *FImpl )( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 );
			}
			ResultType operator()( Param1 P1, Param2 P2, Param3 P3, Param4 P4, Param5 P5, Param6 P6, Param7 P7, Param8 P8, Param9 P9, Param10 P10, Param11 P11 )
			{
				return ( *FImpl )( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 );
			}
		private:
			std::auto_ptr<Impl>    FImpl;
		};
		// partial specialization
		// 0 params
		template <class R>
		class clFunctorImpl<R, NullType>
		{
		public:
			virtual R operator()() = 0;
			virtual clFunctorImpl* Clone() const = 0;
			virtual void* GetObjectPtr() const = 0;
			virtual long long GetUID() const = 0;
			virtual ~clFunctorImpl() {};
		};
		// 1 param
		template <class R, class P1>
		class clFunctorImpl<R, TYPELIST_1( P1 )>
		{
		public:
			virtual R operator()( P1 ) = 0;
			virtual clFunctorImpl* Clone() const = 0;
			virtual void* GetObjectPtr() const = 0;
			virtual long long GetUID() const = 0;
			virtual ~clFunctorImpl() {};
		};
		// 2 params
		template <class R, class P1, class P2>
		class clFunctorImpl<R, TYPELIST_2( P1, P2 )>
		{
		public:
			virtual R operator()( P1, P2 ) = 0;
			virtual clFunctorImpl* Clone() const = 0;
			virtual void* GetObjectPtr() const = 0;
			virtual long long GetUID() const = 0;
			virtual ~clFunctorImpl() {};
		};
		// 3 params
		template <class R, class P1, class P2, class P3>
		class clFunctorImpl<R, TYPELIST_3( P1, P2, P3 )>
		{
		public:
			virtual R operator()( P1, P2, P3 ) = 0;
			virtual clFunctorImpl* Clone() const = 0;
			virtual void* GetObjectPtr() const = 0;
			virtual long long GetUID() const = 0;
			virtual ~clFunctorImpl() {};
		};
		// 4 params
		template <class R, class P1, class P2, class P3, class P4>
		class clFunctorImpl<R, TYPELIST_4( P1, P2, P3, P4 )>
		{
		public:
			virtual R operator()( P1, P2, P3, P4 ) = 0;
			virtual clFunctorImpl* Clone() const = 0;
			virtual void* GetObjectPtr() const = 0;
			virtual long long GetUID() const = 0;
			virtual ~clFunctorImpl() {};
		};
		// 5 params
		template <class R, class P1, class P2, class P3, class P4, class P5>
		class clFunctorImpl<R, TYPELIST_5( P1, P2, P3, P4, P5 )>
		{
		public:
			virtual R operator()( P1, P2, P3, P4, P5 ) = 0;
			virtual clFunctorImpl* Clone() const = 0;
			virtual void* GetObjectPtr() const = 0;
			virtual long long GetUID() const = 0;
			virtual ~clFunctorImpl() {};
		};
		// 6 params
		template <class R, class P1, class P2, class P3, class P4, class P5, class P6>
		class clFunctorImpl<R, TYPELIST_6( P1, P2, P3, P4, P5, P6 )>
		{
		public:
			virtual R operator()( P1, P2, P3, P4, P5, P6 ) = 0;
			virtual clFunctorImpl* Clone() const = 0;
			virtual void* GetObjectPtr() const = 0;
			virtual long long GetUID() const = 0;
			virtual ~clFunctorImpl() {};
		};
		// 7 params
		template <class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		class clFunctorImpl<R, TYPELIST_7( P1, P2, P3, P4, P5, P6, P7 )>
		{
		public:
			virtual R operator()( P1, P2, P3, P4, P5, P6, P7 ) = 0;
			virtual clFunctorImpl* Clone() const = 0;
			virtual void* GetObjectPtr() const = 0;
			virtual long long GetUID() const = 0;
			virtual ~clFunctorImpl() {};
		};
		// 8 params
		template <class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
		class clFunctorImpl<R, TYPELIST_8( P1, P2, P3, P4, P5, P6, P7, P8 )>
		{
		public:
			virtual R operator()( P1, P2, P3, P4, P5, P6, P7, P8 ) = 0;
			virtual clFunctorImpl* Clone() const = 0;
			virtual void* GetObjectPtr() const = 0;
			virtual long long GetUID() const = 0;
			virtual ~clFunctorImpl() {};
		};
		// 9 params
		template <class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
		class clFunctorImpl<R, TYPELIST_9( P1, P2, P3, P4, P5, P6, P7, P8, P9 )>
		{
		public:
			virtual R operator()( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) = 0;
			virtual clFunctorImpl* Clone() const = 0;
			virtual void* GetObjectPtr() const = 0;
			virtual long long GetUID() const = 0;
			virtual ~clFunctorImpl() {};
		};
		// 10 params
		template <class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
		class clFunctorImpl<R, TYPELIST_10( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 )>
		{
		public:
			virtual R operator()( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ) = 0;
			virtual clFunctorImpl* Clone() const = 0;
			virtual void* GetObjectPtr() const = 0;
			virtual long long GetUID() const = 0;
			virtual ~clFunctorImpl() {};
		};
		// 11 params
		template <class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11>
		class clFunctorImpl<R, TYPELIST_11( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 )>
		{
		public:
			virtual R operator()( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 ) = 0;
			virtual clFunctorImpl* Clone() const = 0;
			virtual void* GetObjectPtr() const = 0;
			virtual long long GetUID() const = 0;
			virtual ~clFunctorImpl() {};
		};
		//
		// For ordinary functions
		//
		template <class ParentFunctor, class Fun> class clFunctionHandler: public clFunctorImpl<typename ParentFunctor::ParentResultType, typename ParentFunctor::ParamList>
		{
		public:
			typedef typename ParentFunctor::ParentResultType ResultType;
			explicit clFunctionHandler( const Fun& fun ): FFun( fun ) {}
			IMPLEMENT_CLONE_METHOD( clFunctionHandler )
			virtual void* GetObjectPtr() const
			{
				return NULL;
			};
			virtual long long GetUID() const
			{
#ifdef OS_64BIT
				return ( long long )FFun;
#else
				return ( long )FFun;
#endif
			};
			ResultType operator()()
			{
				return FFun();
			}
			ResultType operator()( typename ParentFunctor::Param1 P1 )
			{
				return FFun( P1 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2 )
			{
				return FFun( P1, P2 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3 )
			{
				return FFun( P1, P2, P3 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3,
			                       typename ParentFunctor::Param4 P4 )
			{
				return FFun( P1, P2, P3, P4 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3,
			                       typename ParentFunctor::Param4 P4,
			                       typename ParentFunctor::Param5 P5 )
			{
				return FFun( P1, P2, P3, P4, P5 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3,
			                       typename ParentFunctor::Param4 P4,
			                       typename ParentFunctor::Param5 P5,
			                       typename ParentFunctor::Param6 P6 )
			{
				return FFun( P1, P2, P3, P4, P5, P6 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3,
			                       typename ParentFunctor::Param4 P4,
			                       typename ParentFunctor::Param5 P5,
			                       typename ParentFunctor::Param6 P6,
			                       typename ParentFunctor::Param7 P7 )
			{
				return FFun( P1, P2, P3, P4, P5, P6, P7 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3,
			                       typename ParentFunctor::Param4 P4,
			                       typename ParentFunctor::Param5 P5,
			                       typename ParentFunctor::Param6 P6,
			                       typename ParentFunctor::Param7 P7,
			                       typename ParentFunctor::Param8 P8 )
			{
				return FFun( P1, P2, P3, P4, P5, P6, P7, P8 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3,
			                       typename ParentFunctor::Param4 P4,
			                       typename ParentFunctor::Param5 P5,
			                       typename ParentFunctor::Param6 P6,
			                       typename ParentFunctor::Param7 P7,
			                       typename ParentFunctor::Param8 P8,
			                       typename ParentFunctor::Param9 P9 )
			{
				return FFun( P1, P2, P3, P4, P5, P6, P7, P8, P9 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3,
			                       typename ParentFunctor::Param4 P4,
			                       typename ParentFunctor::Param5 P5,
			                       typename ParentFunctor::Param6 P6,
			                       typename ParentFunctor::Param7 P7,
			                       typename ParentFunctor::Param8 P8,
			                       typename ParentFunctor::Param9 P9,
			                       typename ParentFunctor::Param10 P10 )
			{
				return FFun( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3,
			                       typename ParentFunctor::Param4 P4,
			                       typename ParentFunctor::Param5 P5,
			                       typename ParentFunctor::Param6 P6,
			                       typename ParentFunctor::Param7 P7,
			                       typename ParentFunctor::Param8 P8,
			                       typename ParentFunctor::Param9 P9,
			                       typename ParentFunctor::Param10 P10,
			                       typename ParentFunctor::Param11 P11 )
			{
				return FFun( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 );
			}
		private:
			Fun FFun;
		};
		//
		// For methods
		//
		template <class ParentFunctor, class Object, class Method> class clMethodHandler: public clFunctorImpl<typename ParentFunctor::ParentResultType, typename ParentFunctor::ParamList>
		{
		public:
			typedef typename ParentFunctor::ParentResultType ResultType;
			explicit clMethodHandler( const Object& Obj, const Method Meth ): FObject( Obj ), FMethod( Meth ) {};
			IMPLEMENT_CLONE_METHOD( clMethodHandler )
			virtual void* GetObjectPtr() const
			{
				return FObject;
			};
			virtual long long GetUID() const
			{
				// NOTE: this is not 64-bit safe
				union
				{
					Object Obj;
					void* UID;
				} Cast;
				Cast.Obj = FObject;
#ifdef OS_64BIT
				return reinterpret_cast<long long>( Cast.UID );
#else
				return reinterpret_cast<long>( Cast.UID );
#endif
			};
			ResultType operator()()
			{
				return ( ( *FObject ).*FMethod )();
			}
			ResultType operator()( typename ParentFunctor::Param1 P1 )
			{
				return ( ( *FObject ).*FMethod )( P1 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2 )
			{
				return ( ( *FObject ).*FMethod )( P1, P2 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3 )
			{
				return ( ( *FObject ).*FMethod )( P1, P2, P3 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3,
			                       typename ParentFunctor::Param4 P4 )
			{
				return ( ( *FObject ).*FMethod )( P1, P2, P3, P4 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3,
			                       typename ParentFunctor::Param4 P4,
			                       typename ParentFunctor::Param5 P5 )
			{
				return ( ( *FObject ).*FMethod )( P1, P2, P3, P4, P5 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3,
			                       typename ParentFunctor::Param4 P4,
			                       typename ParentFunctor::Param5 P5,
			                       typename ParentFunctor::Param6 P6 )
			{
				return ( ( *FObject ).*FMethod )( P1, P2, P3, P4, P5, P6 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3,
			                       typename ParentFunctor::Param4 P4,
			                       typename ParentFunctor::Param5 P5,
			                       typename ParentFunctor::Param6 P6,
			                       typename ParentFunctor::Param7 P7 )
			{
				return ( ( *FObject ).*FMethod )( P1, P2, P3, P4, P5, P6, P7 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3,
			                       typename ParentFunctor::Param4 P4,
			                       typename ParentFunctor::Param5 P5,
			                       typename ParentFunctor::Param6 P6,
			                       typename ParentFunctor::Param7 P7,
			                       typename ParentFunctor::Param8 P8 )
			{
				return ( ( *FObject ).*FMethod )( P1, P2, P3, P4, P5, P6, P7, P8 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3,
			                       typename ParentFunctor::Param4 P4,
			                       typename ParentFunctor::Param5 P5,
			                       typename ParentFunctor::Param6 P6,
			                       typename ParentFunctor::Param7 P7,
			                       typename ParentFunctor::Param8 P8,
			                       typename ParentFunctor::Param9 P9 )
			{
				return ( ( *FObject ).*FMethod )( P1, P2, P3, P4, P5, P6, P7, P8, P9 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3,
			                       typename ParentFunctor::Param4 P4,
			                       typename ParentFunctor::Param5 P5,
			                       typename ParentFunctor::Param6 P6,
			                       typename ParentFunctor::Param7 P7,
			                       typename ParentFunctor::Param8 P8,
			                       typename ParentFunctor::Param9 P9,
			                       typename ParentFunctor::Param10 P10 )
			{
				return ( ( *FObject ).*FMethod )( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 );
			}
			ResultType operator()( typename ParentFunctor::Param1 P1,
			                       typename ParentFunctor::Param2 P2,
			                       typename ParentFunctor::Param3 P3,
			                       typename ParentFunctor::Param4 P4,
			                       typename ParentFunctor::Param5 P5,
			                       typename ParentFunctor::Param6 P6,
			                       typename ParentFunctor::Param7 P7,
			                       typename ParentFunctor::Param8 P8,
			                       typename ParentFunctor::Param9 P9,
			                       typename ParentFunctor::Param10 P10,
			                       typename ParentFunctor::Param11 P11 )
			{
				return ( ( *FObject ).*FMethod )( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 );
			}
		private:
			Object    FObject;
			Method    FMethod;
		};

		//////////////////////////
		//                      //
		//        Binders       //
		//                      //
		//////////////////////////

		// for functions
		template <class ResultType>
		inline clFunctor<ResultType, TYPELIST_0()> Bind( ResultType( *FuncPtr )() )
		{
			return clFunctor<ResultType, TYPELIST_0()>( FuncPtr );
		}
		template <class ResultType, class P1>
		inline clFunctor<ResultType, TYPELIST_1( P1 )> Bind( ResultType( *FuncPtr )( P1 ) )
		{
			return clFunctor<ResultType, TYPELIST_1( P1 )>( FuncPtr );
		}
		template <class ResultType, class P1, class P2>
		inline clFunctor<ResultType, TYPELIST_2( P1, P2 )> Bind( ResultType( *FuncPtr )( P1, P2 ) )
		{
			return clFunctor<ResultType, TYPELIST_2( P1, P2 )>( FuncPtr );
		}
		template <class ResultType, class P1, class P2, class P3>
		inline clFunctor<ResultType, TYPELIST_3( P1, P2, P3 )> Bind( ResultType( *FuncPtr )( P1, P2, P3 ) )
		{
			return clFunctor<ResultType, TYPELIST_3( P1, P2, P3 )>( FuncPtr );
		}
		template <class ResultType, class P1, class P2, class P3, class P4>
		inline clFunctor<ResultType, TYPELIST_4( P1, P2, P3, P4 )> Bind( ResultType( *FuncPtr )( P1, P2, P3, P4 ) )
		{
			return clFunctor<ResultType, TYPELIST_4( P1, P2, P3, P4 )>( FuncPtr );
		}
		template <class ResultType, class P1, class P2, class P3, class P4, class P5>
		inline clFunctor<ResultType, TYPELIST_5( P1, P2, P3, P4, P5 )> Bind( ResultType( *FuncPtr )( P1, P2, P3, P4, P5 ) )
		{
			return clFunctor<ResultType, TYPELIST_5( P1, P2, P3, P4, P5 )>( FuncPtr );
		}
		template <class ResultType, class P1, class P2, class P3, class P4, class P5, class P6>
		inline clFunctor<ResultType, TYPELIST_6( P1, P2, P3, P4, P5, P6 )> Bind( ResultType( *FuncPtr )( P1, P2, P3, P4, P5, P6 ) )
		{
			return clFunctor<ResultType, TYPELIST_6( P1, P2, P3, P4, P5, P6 )>( FuncPtr );
		}
		template <class ResultType, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		inline clFunctor<ResultType, TYPELIST_7( P1, P2, P3, P4, P5, P6, P7 )> Bind( ResultType( *FuncPtr )( P1, P2, P3, P4, P5, P6, P7 ) )
		{
			return clFunctor<ResultType, TYPELIST_7( P1, P2, P3, P4, P5, P6, P7 )>( FuncPtr );
		}
		template <class ResultType, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
		inline clFunctor<ResultType, TYPELIST_8( P1, P2, P3, P4, P5, P6, P7, P8 )> Bind( ResultType( *FuncPtr )( P1, P2, P3, P4, P5, P6, P7, P8 ) )
		{
			return clFunctor<ResultType, TYPELIST_8( P1, P2, P3, P4, P5, P6, P7, P8 )>( FuncPtr );
		}
		template <class ResultType, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
		inline clFunctor<ResultType, TYPELIST_9( P1, P2, P3, P4, P5, P6, P7, P8, P9 )> Bind( ResultType( *FuncPtr )( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) )
		{
			return clFunctor<ResultType, TYPELIST_9( P1, P2, P3, P4, P5, P6, P7, P8, P9 )>( FuncPtr );
		}
		template <class ResultType, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
		inline clFunctor<ResultType, TYPELIST_10( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 )> Bind( ResultType( *FuncPtr )( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ) )
		{
			return clFunctor<ResultType, TYPELIST_10( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 )>( FuncPtr );
		}
		template <class ResultType, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11>
		inline clFunctor<ResultType, TYPELIST_11( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 )> Bind( ResultType( *FuncPtr )( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 ) )
		{
			return clFunctor<ResultType, TYPELIST_11( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 )>( FuncPtr );
		}
		// for methods

		template <class ResultType, class ClassType, class ObjectType>
		inline clFunctor<ResultType, TYPELIST_0()> Bind( ResultType( ClassType::*MethodPtr )(), const ObjectType& Object )
		{
			return clFunctor<ResultType, TYPELIST_0()>( Object, MethodPtr );
		}
		template <class ResultType, class ClassType, class ObjectType, class P1>
		inline clFunctor<ResultType, TYPELIST_1( P1 )> Bind( ResultType( ClassType::*MethodPtr )( P1 ), const ObjectType& Object )
		{
			return clFunctor<ResultType, TYPELIST_1( P1 )>( Object, MethodPtr );
		}
		template <class ResultType, class ClassType, class ObjectType, class P1, class P2>
		inline clFunctor<ResultType, TYPELIST_2( P1, P2 )> Bind( ResultType( ClassType::*MethodPtr )( P1, P2 ), const ObjectType& Object )
		{
			return clFunctor<ResultType, TYPELIST_2( P1, P2 )>( Object, MethodPtr );
		}
		template <class ResultType, class ClassType, class ObjectType, class P1, class P2, class P3>
		inline clFunctor<ResultType, TYPELIST_3( P1, P2, P3 )> Bind( ResultType( ClassType::*MethodPtr )( P1, P2, P3 ), const ObjectType& Object )
		{
			return clFunctor<ResultType, TYPELIST_3( P1, P2, P3 )>( Object, MethodPtr );
		}
		template <class ResultType, class ClassType, class ObjectType, class P1, class P2, class P3, class P4>
		inline clFunctor<ResultType, TYPELIST_4( P1, P2, P3, P4 )> Bind( ResultType( ClassType::*MethodPtr )( P1, P2, P3, P4 ), const ObjectType& Object )
		{
			return clFunctor<ResultType, TYPELIST_4( P1, P2, P3, P4 )>( Object, MethodPtr );
		}
		template <class ResultType, class ClassType, class ObjectType, class P1, class P2, class P3, class P4, class P5>
		inline clFunctor<ResultType, TYPELIST_5( P1, P2, P3, P4, P5 )> Bind( ResultType( ClassType::*MethodPtr )( P1, P2, P3, P4, P5 ), const ObjectType& Object )
		{
			return clFunctor<ResultType, TYPELIST_5( P1, P2, P3, P4, P5 )>( Object, MethodPtr );
		}
		template <class ResultType, class ClassType, class ObjectType, class P1, class P2, class P3, class P4, class P5, class P6>
		inline clFunctor<ResultType, TYPELIST_6( P1, P2, P3, P4, P5, P6 )> Bind( ResultType( ClassType::*MethodPtr )( P1, P2, P3, P4, P5, P6 ), const ObjectType& Object )
		{
			return clFunctor<ResultType, TYPELIST_6( P1, P2, P3, P4, P5, P6 )>( Object, MethodPtr );
		}
		template <class ResultType, class ClassType, class ObjectType, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
		inline clFunctor<ResultType, TYPELIST_7( P1, P2, P3, P4, P5, P6, P7 )> Bind( ResultType( ClassType::*MethodPtr )( P1, P2, P3, P4, P5, P6, P7 ), const ObjectType& Object )
		{
			return clFunctor<ResultType, TYPELIST_7( P1, P2, P3, P4, P5, P6, P7 )>( Object, MethodPtr );
		}
		template <class ResultType, class ClassType, class ObjectType, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
		inline clFunctor<ResultType, TYPELIST_8( P1, P2, P3, P4, P5, P6, P7, P8 )> Bind( ResultType( ClassType::*MethodPtr )( P1, P2, P3, P4, P5, P6, P7, P8 ), const ObjectType& Object )
		{
			return clFunctor<ResultType, TYPELIST_8( P1, P2, P3, P4, P5, P6, P7, P8 )>( Object, MethodPtr );
		}
		template <class ResultType, class ClassType, class ObjectType, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
		inline clFunctor<ResultType, TYPELIST_9( P1, P2, P3, P4, P5, P6, P7, P8, P9 )> Bind( ResultType( ClassType::*MethodPtr )( P1, P2, P3, P4, P5, P6, P7, P8, P9 ), const ObjectType& Object )
		{
			return clFunctor<ResultType, TYPELIST_9( P1, P2, P3, P4, P5, P6, P7, P8, P9 )>( Object, MethodPtr );
		}
		template <class ResultType, class ClassType, class ObjectType, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
		inline clFunctor<ResultType, TYPELIST_10( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 )> Bind( ResultType( ClassType::*MethodPtr )( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ), const ObjectType& Object )
		{
			return clFunctor<ResultType, TYPELIST_10( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 )>( Object, MethodPtr );
		}
		template <class ResultType, class ClassType, class ObjectType, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10, class P11>
		inline clFunctor<ResultType, TYPELIST_11( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 )> Bind( ResultType( ClassType::*MethodPtr )( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 ), const ObjectType& Object )
		{
			return clFunctor<ResultType, TYPELIST_11( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 )>( Object, MethodPtr );
		}
	}
}

#endif

/*
 * 29/07/2010
     BIND()
 * 23/01/2005
     IsEqual()
 * 17/01/2005
     Simple binding semantics added
 * 16/01/2005
     It's here
*/
