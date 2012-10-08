/**
 * \file clCompositeBehaviour_Tunneller.h
 * \brief Tunneller for class: clCompositeBehaviour
 * \version 0.6.06
 * \date 16/01/2012
 * \author Sergey Kosarevsky, 2005-2011
 * \author Viktor Latypov, 2007-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

//                             Base class: clBehaviour  (public)

#ifndef _clCompositeBehaviour_Tunneller_
#define _clCompositeBehaviour_Tunneller_

#include "Linderdaum/World/AI/Behaviour.h"

#if !defined( _DISABLE_TUNNELLERS_ )

class clCompositeBehaviour_Tunneller: public clCompositeBehaviour
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
   // clBehaviour interface
   //
   virtual void    DumpDotFile(const LString& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "DumpDotFile" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "DumpDotFile", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clBehaviour::DumpDotFile(P0);

         return;
      }

      clBehaviour::DumpDotFile(P0);
   }
   virtual void    SetParent(clBehaviour* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetParent" ) )
      {
         clParametersList Params;

         ParameterType<clBehaviour*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetParent", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clBehaviour::SetParent(P0);

         return;
      }

      clBehaviour::SetParent(P0);
   }
   virtual clBehaviour*    GetParent() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetParent" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetParent", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clBehaviour::GetParent();

         return *(TypeTraits< clBehaviour* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clBehaviour::GetParent();
   }
   virtual void    SetWeight(float P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetWeight" ) )
      {
         clParametersList Params;

         ParameterType<float>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetWeight", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clBehaviour::SetWeight(P0);

         return;
      }

      clBehaviour::SetWeight(P0);
   }
   virtual float    GetWeight() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetWeight" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetWeight", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clBehaviour::GetWeight();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clBehaviour::GetWeight();
   }
   virtual void    DumpDot(iOStream* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "DumpDot" ) )
      {
         clParametersList Params;

         ParameterType<iOStream*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "DumpDot", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clCompositeBehaviour::DumpDot(P0);

         return;
      }

      clCompositeBehaviour::DumpDot(P0);
   }
   virtual eRetCode    Update(float P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Update" ) )
      {
         clParametersList Params;

         ParameterType<float>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Update", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clBehaviour::Update(P0);

         return *(TypeTraits< eRetCode >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clBehaviour::Update(P0);
   }
   //
   // clCompositeBehaviour interface
   //
   virtual void    AddBehaviour(clBehaviour* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "AddBehaviour" ) )
      {
         clParametersList Params;

         ParameterType<clBehaviour*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "AddBehaviour", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clCompositeBehaviour::AddBehaviour(P0);

         return;
      }

      clCompositeBehaviour::AddBehaviour(P0);
   }
   virtual void    ClearBehaviours(bool P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "ClearBehaviours" ) )
      {
         clParametersList Params;

         ParameterType<bool>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "ClearBehaviours", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clCompositeBehaviour::ClearBehaviours(P0);

         return;
      }

      clCompositeBehaviour::ClearBehaviours(P0);
   }
   virtual size_t    GetNumBehaviours() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetNumBehaviours" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetNumBehaviours", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clCompositeBehaviour::GetNumBehaviours();

         return *(TypeTraits< size_t >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clCompositeBehaviour::GetNumBehaviours();
   }
   virtual clBehaviour*    GetBehaviour(int P0) const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetBehaviour" ) )
      {
         clParametersList Params;

         ParameterType<int>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "GetBehaviour", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clCompositeBehaviour::GetBehaviour(P0);

         return *(TypeTraits< clBehaviour* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clCompositeBehaviour::GetBehaviour(P0);
   }
   virtual void    SetBehaviour(int P0, clBehaviour* P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetBehaviour" ) )
      {
         clParametersList Params;

         ParameterType<int>::Type Param0;
         ParameterType<clBehaviour*>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "SetBehaviour", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clCompositeBehaviour::SetBehaviour(P0, P1);

         return;
      }

      clCompositeBehaviour::SetBehaviour(P0, P1);
   }
};

#endif // _DISABLE_TUNNELLERS_

#endif

/*
 * 16/01/2012
     Autogenerated via LinderScript Database Compiler 0.9.24
*/
