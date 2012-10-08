/**
 * \file clFlingHandler2_Tunneller.h
 * \brief Tunneller for class: clFlingHandler2
 * \version 0.6.06
 * \date 16/01/2012
 * \author Sergey Kosarevsky, 2005-2011
 * \author Viktor Latypov, 2007-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

//                             Base class: clGestureHandler  (public)

#ifndef _clFlingHandler2_Tunneller_
#define _clFlingHandler2_Tunneller_

#include "Linderdaum/Input/Gestures2.h"

#if !defined( _DISABLE_TUNNELLERS_ )

class clFlingHandler2_Tunneller: public clFlingHandler2
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
   // clGestureHandler interface
   //
   virtual bool    IsGestureValid() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsGestureValid" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsGestureValid", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGestureHandler::IsGestureValid();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGestureHandler::IsGestureValid();
   }
   virtual bool    IsDraggingValid() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsDraggingValid" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsDraggingValid", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGestureHandler::IsDraggingValid();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGestureHandler::IsDraggingValid();
   }
   virtual const LMotionData*    GetMotionData() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetMotionData" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetMotionData", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGestureHandler::GetMotionData();

         return *(TypeTraits< const LMotionData* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGestureHandler::GetMotionData();
   }
   virtual LVector2    GetPositionDelta() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetPositionDelta" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetPositionDelta", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGestureHandler::GetPositionDelta();

         return *(TypeTraits< LVector2 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGestureHandler::GetPositionDelta();
   }
   virtual LMatrix4    GetTranslationMatrix() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetTranslationMatrix" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetTranslationMatrix", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGestureHandler::GetTranslationMatrix();

         return *(TypeTraits< LMatrix4 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGestureHandler::GetTranslationMatrix();
   }
   virtual const sTouchPoint&    GetInitialTouchPoint() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetInitialTouchPoint" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetInitialTouchPoint", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGestureHandler::GetInitialTouchPoint();

         return *(TypeTraits< const sTouchPoint& >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGestureHandler::GetInitialTouchPoint();
   }
   virtual const sTouchPoint&    GetCurrentTouchPoint() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetCurrentTouchPoint" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetCurrentTouchPoint", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGestureHandler::GetCurrentTouchPoint();

         return *(TypeTraits< const sTouchPoint& >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGestureHandler::GetCurrentTouchPoint();
   }
   virtual void    AddListener(iGestureResponder* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "AddListener" ) )
      {
         clParametersList Params;

         ParameterType<iGestureResponder*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "AddListener", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clGestureHandler::AddListener(P0);

         return;
      }

      clGestureHandler::AddListener(P0);
   }
   virtual size_t    GetNumListeners() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetNumListeners" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetNumListeners", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGestureHandler::GetNumListeners();

         return *(TypeTraits< size_t >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGestureHandler::GetNumListeners();
   }
   virtual iGestureResponder*    GetListener(size_t P0) const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetListener" ) )
      {
         clParametersList Params;

         ParameterType<size_t>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "GetListener", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGestureHandler::GetListener(P0);

         return *(TypeTraits< iGestureResponder* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGestureHandler::GetListener(P0);
   }
protected:
   virtual void    UpdateGesture()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "UpdateGesture" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "UpdateGesture", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clFlingHandler2::UpdateGesture();

         return;
      }

      clFlingHandler2::UpdateGesture();
   }
   //
   // clFlingHandler2 interface
   //
};

#endif // _DISABLE_TUNNELLERS_

#endif

/*
 * 16/01/2012
     Autogenerated via LinderScript Database Compiler 0.9.24
*/
