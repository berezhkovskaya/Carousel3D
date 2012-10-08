/**
 * \file clFlingHandler_Tunneller.h
 * \brief Tunneller for class: clFlingHandler
 * \version 0.6.06
 * \date 16/01/2012
 * \author Sergey Kosarevsky, 2005-2011
 * \author Viktor Latypov, 2007-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

//                             Base class: clGestureHandler  (public)

#ifndef _clFlingHandler_Tunneller_
#define _clFlingHandler_Tunneller_

#include "Linderdaum/Input/Gestures.h"

#if !defined( _DISABLE_TUNNELLERS_ )

class clFlingHandler_Tunneller: public clFlingHandler
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

         if ( !MethodCalled ) clGestureHandler::UpdateGesture();

         return;
      }

      clGestureHandler::UpdateGesture();
   }
public:
   //
   // clFlingHandler interface
   //
   virtual void    Reset()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Reset" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "Reset", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clFlingHandler::Reset();

         return;
      }

      clFlingHandler::Reset();
   }
   virtual void    SetIncrementalRotationMode(const vec2& P0, const vec2& P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetIncrementalRotationMode" ) )
      {
         clParametersList Params;

         ParameterType<const vec2&>::Type Param0;
         ParameterType<const vec2&>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "SetIncrementalRotationMode", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clFlingHandler::SetIncrementalRotationMode(P0, P1);

         return;
      }

      clFlingHandler::SetIncrementalRotationMode(P0, P1);
   }
   virtual void    SetStrokeMode()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetStrokeMode" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "SetStrokeMode", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clFlingHandler::SetStrokeMode();

         return;
      }

      clFlingHandler::SetStrokeMode();
   }
   virtual void    SetValue(const vec2& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetValue" ) )
      {
         clParametersList Params;

         ParameterType<const vec2&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetValue", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clFlingHandler::SetValue(P0);

         return;
      }

      clFlingHandler::SetValue(P0);
   }
   virtual vec2    GetValue() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetValue" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetValue", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clFlingHandler::GetValue();

         return *(TypeTraits< vec2 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clFlingHandler::GetValue();
   }
   virtual void    SetActive(bool P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetActive" ) )
      {
         clParametersList Params;

         ParameterType<bool>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetActive", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clFlingHandler::SetActive(P0);

         return;
      }

      clFlingHandler::SetActive(P0);
   }
   virtual bool    IsActive() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsActive" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsActive", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clFlingHandler::IsActive();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clFlingHandler::IsActive();
   }
};

#endif // _DISABLE_TUNNELLERS_

#endif

/*
 * 16/01/2012
     Autogenerated via LinderScript Database Compiler 0.9.24
*/
