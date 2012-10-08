/**
 * \file iTestApplet_Tunneller.h
 * \brief Tunneller for class: iTestApplet
 * \version 0.6.06
 * \date 16/01/2012
 * \author Sergey Kosarevsky, 2005-2011
 * \author Viktor Latypov, 2007-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

//                             Base class: iApplet  (public)

#ifndef _iTestApplet_Tunneller_
#define _iTestApplet_Tunneller_

#include "Linderdaum/UnitTests/Tests/Classes_Test_8.h"

#if !defined( _DISABLE_TUNNELLERS_ )

class iTestApplet_Tunneller: public iTestApplet
{
public:
   //
   // iObject interface
   //
      /*
            The interface of this class was marked as "native final".
            LSDC bypassed generation of stubs for it - only pure virtual methods are generated.
      */
   //
   // iTestApplet interface
   //
   virtual int&    TestReturnIntRefByRef(int& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "TestReturnIntRefByRef" ) )
      {
         clParametersList Params;

         ParameterType<int&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "TestReturnIntRefByRef", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) FATAL_MSG("Abstract method called: iTestApplet::TestReturnIntRefByRef()")

         return *(TypeTraits< int& >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      FATAL_MSG("Abstract method called: iTestApplet::TestReturnIntRefByRef()")

      return *(TypeTraits< int& >::ReferredType*)(0);
   }
   virtual int    TestReturnInt(int P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "TestReturnInt" ) )
      {
         clParametersList Params;

         ParameterType<int>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "TestReturnInt", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) FATAL_MSG("Abstract method called: iTestApplet::TestReturnInt()")

         return *(TypeTraits< int >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      FATAL_MSG("Abstract method called: iTestApplet::TestReturnInt()")

      return *(TypeTraits< int >::ReferredType*)(0);
   }
   virtual int&    TestReturnIntRef(int P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "TestReturnIntRef" ) )
      {
         clParametersList Params;

         ParameterType<int>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "TestReturnIntRef", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) FATAL_MSG("Abstract method called: iTestApplet::TestReturnIntRef()")

         return *(TypeTraits< int& >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      FATAL_MSG("Abstract method called: iTestApplet::TestReturnIntRef()")

      return *(TypeTraits< int& >::ReferredType*)(0);
   }
   virtual LString    TestReturnString(int P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "TestReturnString" ) )
      {
         clParametersList Params;

         ParameterType<int>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "TestReturnString", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) FATAL_MSG("Abstract method called: iTestApplet::TestReturnString()")

         return *(TypeTraits< LString >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      FATAL_MSG("Abstract method called: iTestApplet::TestReturnString()")

      return *(TypeTraits< LString >::ReferredType*)(0);
   }
   virtual LString&    TestReturnStringRef(int P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "TestReturnStringRef" ) )
      {
         clParametersList Params;

         ParameterType<int>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "TestReturnStringRef", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) FATAL_MSG("Abstract method called: iTestApplet::TestReturnStringRef()")

         return *(TypeTraits< LString& >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      FATAL_MSG("Abstract method called: iTestApplet::TestReturnStringRef()")

      return *(TypeTraits< LString& >::ReferredType*)(0);
   }
   virtual LString*    TestReturnStringPtr(int P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "TestReturnStringPtr" ) )
      {
         clParametersList Params;

         ParameterType<int>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "TestReturnStringPtr", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) FATAL_MSG("Abstract method called: iTestApplet::TestReturnStringPtr()")

         return *(TypeTraits< LString* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      FATAL_MSG("Abstract method called: iTestApplet::TestReturnStringPtr()")

      return *(TypeTraits< LString* >::ReferredType*)(0);
   }
   virtual clTestClient*    TestCreateClient()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "TestCreateClient" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "TestCreateClient", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) FATAL_MSG("Abstract method called: iTestApplet::TestCreateClient()")

         return *(TypeTraits< clTestClient* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      FATAL_MSG("Abstract method called: iTestApplet::TestCreateClient()")

      return *(TypeTraits< clTestClient* >::ReferredType*)(0);
   }
   virtual void    TestSaySomething(clTestClient* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "TestSaySomething" ) )
      {
         clParametersList Params;

         ParameterType<clTestClient*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "TestSaySomething", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) FATAL_MSG("Abstract method called: iTestApplet::TestSaySomething()")

         return;
      }

      FATAL_MSG("Abstract method called: iTestApplet::TestSaySomething()")
   }
   virtual LString    TestReturnStringArg(const LString& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "TestReturnStringArg" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "TestReturnStringArg", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) FATAL_MSG("Abstract method called: iTestApplet::TestReturnStringArg()")

         return *(TypeTraits< LString >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      FATAL_MSG("Abstract method called: iTestApplet::TestReturnStringArg()")

      return *(TypeTraits< LString >::ReferredType*)(0);
   }
   virtual bool    TestString_ByValue(LString P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "TestString_ByValue" ) )
      {
         clParametersList Params;

         ParameterType<LString>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "TestString_ByValue", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) FATAL_MSG("Abstract method called: iTestApplet::TestString_ByValue()")

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      FATAL_MSG("Abstract method called: iTestApplet::TestString_ByValue()")

      return *(TypeTraits< bool >::ReferredType*)(0);
   }
   virtual bool    TestString_ByRef(LString& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "TestString_ByRef" ) )
      {
         clParametersList Params;

         ParameterType<LString&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "TestString_ByRef", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) FATAL_MSG("Abstract method called: iTestApplet::TestString_ByRef()")

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      FATAL_MSG("Abstract method called: iTestApplet::TestString_ByRef()")

      return *(TypeTraits< bool >::ReferredType*)(0);
   }
   virtual bool    TestString_ByConstRef(const LString& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "TestString_ByConstRef" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "TestString_ByConstRef", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) FATAL_MSG("Abstract method called: iTestApplet::TestString_ByConstRef()")

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      FATAL_MSG("Abstract method called: iTestApplet::TestString_ByConstRef()")

      return *(TypeTraits< bool >::ReferredType*)(0);
   }
   virtual bool    TestString_ByPtr(LString* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "TestString_ByPtr" ) )
      {
         clParametersList Params;

         ParameterType<LString*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "TestString_ByPtr", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) FATAL_MSG("Abstract method called: iTestApplet::TestString_ByPtr()")

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      FATAL_MSG("Abstract method called: iTestApplet::TestString_ByPtr()")

      return *(TypeTraits< bool >::ReferredType*)(0);
   }
   virtual LString    TestState()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "TestState" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "TestState", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) FATAL_MSG("Abstract method called: iTestApplet::TestState()")

         return *(TypeTraits< LString >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      FATAL_MSG("Abstract method called: iTestApplet::TestState()")

      return *(TypeTraits< LString >::ReferredType*)(0);
   }
};

#endif // _DISABLE_TUNNELLERS_

#endif

/*
 * 16/01/2012
     Autogenerated via LinderScript Database Compiler 0.9.24
*/
