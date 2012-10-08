/**
 * \file iActor_Tunneller.h
 * \brief Tunneller for class: iActor
 * \version 0.6.06
 * \date 16/01/2012
 * \author Sergey Kosarevsky, 2005-2011
 * \author Viktor Latypov, 2007-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

//                             Base class: iObject  (public)

#ifndef _iActor_Tunneller_
#define _iActor_Tunneller_

#include "Linderdaum/World/iActor.h"

#if !defined( _DISABLE_TUNNELLERS_ )

class iActor_Tunneller: public iActor
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
   // iActor interface
   //
   virtual clWorld*    GetWorld() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetWorld" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetWorld", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iActor::GetWorld();

         return *(TypeTraits< clWorld* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iActor::GetWorld();
   }
   virtual clScene*    GetScene() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetScene" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetScene", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iActor::GetScene();

         return *(TypeTraits< clScene* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iActor::GetScene();
   }
   virtual void    LeaveWorld()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "LeaveWorld" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "LeaveWorld", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iActor::LeaveWorld();

         return;
      }

      iActor::LeaveWorld();
   }
   virtual void    UpdateActor(float P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "UpdateActor" ) )
      {
         clParametersList Params;

         ParameterType<float>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "UpdateActor", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) FATAL_MSG("Abstract method called: iActor::UpdateActor()")

         return;
      }

      FATAL_MSG("Abstract method called: iActor::UpdateActor()")
   }
   virtual void    RenderOverlay(const LMatrix4& P0, const LMatrix4& P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "RenderOverlay" ) )
      {
         clParametersList Params;

         ParameterType<const LMatrix4&>::Type Param0;
         ParameterType<const LMatrix4&>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "RenderOverlay", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iActor::RenderOverlay(P0, P1);

         return;
      }

      iActor::RenderOverlay(P0, P1);
   }
   virtual bool    IsUpdatedInPause() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsUpdatedInPause" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsUpdatedInPause", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iActor::IsUpdatedInPause();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iActor::IsUpdatedInPause();
   }
   virtual bool    IsPersistent() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsPersistent" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsPersistent", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iActor::IsPersistent();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iActor::IsPersistent();
   }
   virtual int    GetOrder() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetOrder" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetOrder", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iActor::GetOrder();

         return *(TypeTraits< int >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iActor::GetOrder();
   }
   virtual void    SetOrder(int P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetOrder" ) )
      {
         clParametersList Params;

         ParameterType<int>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetOrder", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iActor::SetOrder(P0);

         return;
      }

      iActor::SetOrder(P0);
   }
   virtual void    SetTimeProfile(double P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetTimeProfile" ) )
      {
         clParametersList Params;

         ParameterType<double>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetTimeProfile", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iActor::SetTimeProfile(P0);

         return;
      }

      iActor::SetTimeProfile(P0);
   }
   virtual double    GetTimeProfile() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetTimeProfile" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetTimeProfile", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iActor::GetTimeProfile();

         return *(TypeTraits< double >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iActor::GetTimeProfile();
   }
   virtual void    Actor_EnteredWorld(clWorld* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Actor_EnteredWorld" ) )
      {
         clParametersList Params;

         ParameterType<clWorld*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Actor_EnteredWorld", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iActor::Actor_EnteredWorld(P0);

         return;
      }

      iActor::Actor_EnteredWorld(P0);
   }
   virtual void    Actor_LeavedWorld()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Actor_LeavedWorld" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "Actor_LeavedWorld", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iActor::Actor_LeavedWorld();

         return;
      }

      iActor::Actor_LeavedWorld();
   }
   virtual void    Actor_PostBeginPlay()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Actor_PostBeginPlay" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "Actor_PostBeginPlay", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iActor::Actor_PostBeginPlay();

         return;
      }

      iActor::Actor_PostBeginPlay();
   }
   virtual void    Actor_SomeoneEnteredWorld(iActor* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Actor_SomeoneEnteredWorld" ) )
      {
         clParametersList Params;

         ParameterType<iActor*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Actor_SomeoneEnteredWorld", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iActor::Actor_SomeoneEnteredWorld(P0);

         return;
      }

      iActor::Actor_SomeoneEnteredWorld(P0);
   }
   virtual void    Actor_SomeoneLeavedWorld(iActor* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Actor_SomeoneLeavedWorld" ) )
      {
         clParametersList Params;

         ParameterType<iActor*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Actor_SomeoneLeavedWorld", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iActor::Actor_SomeoneLeavedWorld(P0);

         return;
      }

      iActor::Actor_SomeoneLeavedWorld(P0);
   }
   virtual void    Actor_Event(const LString& P0, bool P1, const LString& P2)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Actor_Event" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;
         ParameterType<bool>::Type Param1;
         ParameterType<const LString&>::Type Param2;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );
         Param2.ReadValue( &P2 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );
         Params.push_back( &Param2 );

         bool MethodCalled = iObject::CallMethod( "Actor_Event", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iActor::Actor_Event(P0, P1, P2);

         return;
      }

      iActor::Actor_Event(P0, P1, P2);
   }
   virtual void    SendEvent(const LString& P0, bool P1) const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SendEvent" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;
         ParameterType<bool>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "SendEvent", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iActor::SendEvent(P0, P1);

         return;
      }

      iActor::SendEvent(P0, P1);
   }
   virtual bool    ReceiveSelfEvents() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "ReceiveSelfEvents" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "ReceiveSelfEvents", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iActor::ReceiveSelfEvents();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iActor::ReceiveSelfEvents();
   }
};

#endif // _DISABLE_TUNNELLERS_

#endif

/*
 * 16/01/2012
     Autogenerated via LinderScript Database Compiler 0.9.24
*/
