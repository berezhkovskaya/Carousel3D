/**
 * \file clFilterPostprocessor_Tunneller.h
 * \brief Tunneller for class: clFilterPostprocessor
 * \version 0.6.06
 * \date 16/01/2012
 * \author Sergey Kosarevsky, 2005-2011
 * \author Viktor Latypov, 2007-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

//                             Base class: iPostprocessor  (public)

#ifndef _clFilterPostprocessor_Tunneller_
#define _clFilterPostprocessor_Tunneller_

#include "Linderdaum/Scene/Postprocess/Filter.h"

#if !defined( _DISABLE_TUNNELLERS_ )

class clFilterPostprocessor_Tunneller: public clFilterPostprocessor
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
   // iPostprocessor interface
   //
   virtual bool    Render(iRenderTarget* P0, iRenderTarget* P1, iRenderTarget* P2)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Render" ) )
      {
         clParametersList Params;

         ParameterType<iRenderTarget*>::Type Param0;
         ParameterType<iRenderTarget*>::Type Param1;
         ParameterType<iRenderTarget*>::Type Param2;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );
         Param2.ReadValue( &P2 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );
         Params.push_back( &Param2 );

         bool MethodCalled = iObject::CallMethod( "Render", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clFilterPostprocessor::Render(P0, P1, P2);

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clFilterPostprocessor::Render(P0, P1, P2);
   }
   virtual bool    IsEnabled() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsEnabled" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsEnabled", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iPostprocessor::IsEnabled();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iPostprocessor::IsEnabled();
   }
   virtual void    SetEnableVarName(const LString& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetEnableVarName" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetEnableVarName", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iPostprocessor::SetEnableVarName(P0);

         return;
      }

      iPostprocessor::SetEnableVarName(P0);
   }
   virtual LString    GetEnableVarName() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetEnableVarName" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetEnableVarName", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iPostprocessor::GetEnableVarName();

         return *(TypeTraits< LString >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iPostprocessor::GetEnableVarName();
   }
   virtual clCVar*    GetEnableVar() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetEnableVar" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetEnableVar", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iPostprocessor::GetEnableVar();

         return *(TypeTraits< clCVar* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iPostprocessor::GetEnableVar();
   }
   //
   // clFilterPostprocessor interface
   //
   virtual void    SetFilterName(const LString& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetFilterName" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetFilterName", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clFilterPostprocessor::SetFilterName(P0);

         return;
      }

      clFilterPostprocessor::SetFilterName(P0);
   }
   virtual LString    GetFilterName() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetFilterName" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetFilterName", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clFilterPostprocessor::GetFilterName();

         return *(TypeTraits< LString >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clFilterPostprocessor::GetFilterName();
   }
};

#endif // _DISABLE_TUNNELLERS_

#endif

/*
 * 16/01/2012
     Autogenerated via LinderScript Database Compiler 0.9.24
*/
