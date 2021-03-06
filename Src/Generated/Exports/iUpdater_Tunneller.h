/**
 * \file iUpdater_Tunneller.h
 * \brief Tunneller for class: iUpdater
 * \version 0.6.06
 * \date 16/01/2012
 * \author Sergey Kosarevsky, 2005-2011
 * \author Viktor Latypov, 2007-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

//                             Base class: iObject  (public)

#ifndef _iUpdater_Tunneller_
#define _iUpdater_Tunneller_

#include "Linderdaum/Scene/iUpdater.h"

#if !defined( _DISABLE_TUNNELLERS_ )

class iUpdater_Tunneller: public iUpdater
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
   // iUpdater interface
   //
   virtual void    Update(float P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Update" ) )
      {
         clParametersList Params;

         ParameterType<float>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Update", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) FATAL_MSG("Abstract method called: iUpdater::Update()")

         return;
      }

      FATAL_MSG("Abstract method called: iUpdater::Update()")
   }
   virtual void    UpdateInPause(float P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "UpdateInPause" ) )
      {
         clParametersList Params;

         ParameterType<float>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "UpdateInPause", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iUpdater::UpdateInPause(P0);

         return;
      }

      iUpdater::UpdateInPause(P0);
   }
   virtual void    Attach(int P0, clScene* P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Attach" ) )
      {
         clParametersList Params;

         ParameterType<int>::Type Param0;
         ParameterType<clScene*>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Attach", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iUpdater::Attach(P0, P1);

         return;
      }

      iUpdater::Attach(P0, P1);
   }
   virtual bool    IsIndex(int P0) const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsIndex" ) )
      {
         clParametersList Params;

         ParameterType<int>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "IsIndex", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iUpdater::IsIndex(P0);

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iUpdater::IsIndex(P0);
   }
};

#endif // _DISABLE_TUNNELLERS_

#endif

/*
 * 16/01/2012
     Autogenerated via LinderScript Database Compiler 0.9.24
*/
