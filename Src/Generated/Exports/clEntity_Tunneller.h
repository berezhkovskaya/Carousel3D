/**
 * \file clEntity_Tunneller.h
 * \brief Tunneller for class: clEntity
 * \version 0.6.06
 * \date 16/01/2012
 * \author Sergey Kosarevsky, 2005-2011
 * \author Viktor Latypov, 2007-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

//                             Base class: iActor  (public)

#ifndef _clEntity_Tunneller_
#define _clEntity_Tunneller_

#include "Linderdaum/World/Entity.h"

#if !defined( _DISABLE_TUNNELLERS_ )

class clEntity_Tunneller: public clEntity
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

         if ( !MethodCalled ) clEntity::UpdateActor(P0);

         return;
      }

      clEntity::UpdateActor(P0);
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

         if ( !MethodCalled ) clEntity::Actor_EnteredWorld(P0);

         return;
      }

      clEntity::Actor_EnteredWorld(P0);
   }
   virtual void    Actor_LeavedWorld()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Actor_LeavedWorld" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "Actor_LeavedWorld", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clEntity::Actor_LeavedWorld();

         return;
      }

      clEntity::Actor_LeavedWorld();
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
   //
   // clEntity interface
   //
   virtual int    GetGeomHandle() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetGeomHandle" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetGeomHandle", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clEntity::GetGeomHandle();

         return *(TypeTraits< int >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clEntity::GetGeomHandle();
   }
   virtual int    GetParentGeomHandle() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetParentGeomHandle" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetParentGeomHandle", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clEntity::GetParentGeomHandle();

         return *(TypeTraits< int >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clEntity::GetParentGeomHandle();
   }
   virtual iActor*    GetParentActor() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetParentActor" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetParentActor", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clEntity::GetParentActor();

         return *(TypeTraits< iActor* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clEntity::GetParentActor();
   }
   virtual LString    GetMeshName() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetMeshName" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetMeshName", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clEntity::GetMeshName();

         return *(TypeTraits< LString >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clEntity::GetMeshName();
   }
   virtual void    SetMeshName(const LString& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetMeshName" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetMeshName", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clEntity::SetMeshName(P0);

         return;
      }

      clEntity::SetMeshName(P0);
   }
   virtual float    GetMeshHeight() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetMeshHeight" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetMeshHeight", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clEntity::GetMeshHeight();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clEntity::GetMeshHeight();
   }
   virtual void    SetMeshHeight(float P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetMeshHeight" ) )
      {
         clParametersList Params;

         ParameterType<float>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetMeshHeight", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clEntity::SetMeshHeight(P0);

         return;
      }

      clEntity::SetMeshHeight(P0);
   }
   virtual clGeom*    GetGeom() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetGeom" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetGeom", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clEntity::GetGeom();

         return *(TypeTraits< clGeom* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clEntity::GetGeom();
   }
   virtual void    SetGeom(clGeom* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetGeom" ) )
      {
         clParametersList Params;

         ParameterType<clGeom*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetGeom", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clEntity::SetGeom(P0);

         return;
      }

      clEntity::SetGeom(P0);
   }
   virtual LVector3    GetPosition() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetPosition" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetPosition", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clEntity::GetPosition();

         return *(TypeTraits< LVector3 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clEntity::GetPosition();
   }
   virtual void    SetPosition(const LVector3& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetPosition" ) )
      {
         clParametersList Params;

         ParameterType<const LVector3&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetPosition", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clEntity::SetPosition(P0);

         return;
      }

      clEntity::SetPosition(P0);
   }
   virtual LTransform&    GetTransform()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetTransform" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetTransform", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clEntity::GetTransform();

         return *(TypeTraits< LTransform& >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clEntity::GetTransform();
   }
   virtual const LTransform&    GetTransformConst() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetTransformConst" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetTransformConst", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clEntity::GetTransformConst();

         return *(TypeTraits< const LTransform& >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clEntity::GetTransformConst();
   }
   virtual bool    GetCenterMesh() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetCenterMesh" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetCenterMesh", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clEntity::GetCenterMesh();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clEntity::GetCenterMesh();
   }
   virtual void    SetCenterMesh(bool P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetCenterMesh" ) )
      {
         clParametersList Params;

         ParameterType<bool>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetCenterMesh", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clEntity::SetCenterMesh(P0);

         return;
      }

      clEntity::SetCenterMesh(P0);
   }
   virtual void    Entity_BecomeVisible()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Entity_BecomeVisible" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "Entity_BecomeVisible", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clEntity::Entity_BecomeVisible();

         return;
      }

      clEntity::Entity_BecomeVisible();
   }
   virtual void    Entity_BecomeInvisible()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Entity_BecomeInvisible" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "Entity_BecomeInvisible", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clEntity::Entity_BecomeInvisible();

         return;
      }

      clEntity::Entity_BecomeInvisible();
   }
};

#endif // _DISABLE_TUNNELLERS_

#endif

/*
 * 16/01/2012
     Autogenerated via LinderScript Database Compiler 0.9.24
*/
