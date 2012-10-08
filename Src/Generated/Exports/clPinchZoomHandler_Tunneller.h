/**
 * \file clPinchZoomHandler_Tunneller.h
 * \brief Tunneller for class: clPinchZoomHandler
 * \version 0.6.06
 * \date 16/01/2012
 * \author Sergey Kosarevsky, 2005-2011
 * \author Viktor Latypov, 2007-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

//                             Base class: clGestureHandler  (public)

#ifndef _clPinchZoomHandler_Tunneller_
#define _clPinchZoomHandler_Tunneller_

#include "Linderdaum/Input/Gestures.h"

#if !defined( _DISABLE_TUNNELLERS_ )

class clPinchZoomHandler_Tunneller: public clPinchZoomHandler
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
   // clPinchZoomHandler interface
   //
   virtual bool    IsPinchZoomValid() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsPinchZoomValid" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsPinchZoomValid", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clPinchZoomHandler::IsPinchZoomValid();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clPinchZoomHandler::IsPinchZoomValid();
   }
   virtual float    GetPinchZoomFactor() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetPinchZoomFactor" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetPinchZoomFactor", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clPinchZoomHandler::GetPinchZoomFactor();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clPinchZoomHandler::GetPinchZoomFactor();
   }
   virtual float    GetPinchInitialDistance() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetPinchInitialDistance" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetPinchInitialDistance", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clPinchZoomHandler::GetPinchInitialDistance();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clPinchZoomHandler::GetPinchInitialDistance();
   }
   virtual LVector2    GetPinchInitialPoint1() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetPinchInitialPoint1" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetPinchInitialPoint1", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clPinchZoomHandler::GetPinchInitialPoint1();

         return *(TypeTraits< LVector2 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clPinchZoomHandler::GetPinchInitialPoint1();
   }
   virtual LVector2    GetPinchInitialPoint2() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetPinchInitialPoint2" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetPinchInitialPoint2", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clPinchZoomHandler::GetPinchInitialPoint2();

         return *(TypeTraits< LVector2 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clPinchZoomHandler::GetPinchInitialPoint2();
   }
   virtual LVector2    GetPinchInitialCenter() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetPinchInitialCenter" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetPinchInitialCenter", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clPinchZoomHandler::GetPinchInitialCenter();

         return *(TypeTraits< LVector2 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clPinchZoomHandler::GetPinchInitialCenter();
   }
   virtual LVector2    GetPinchCurrentPoint1() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetPinchCurrentPoint1" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetPinchCurrentPoint1", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clPinchZoomHandler::GetPinchCurrentPoint1();

         return *(TypeTraits< LVector2 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clPinchZoomHandler::GetPinchCurrentPoint1();
   }
   virtual LVector2    GetPinchCurrentPoint2() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetPinchCurrentPoint2" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetPinchCurrentPoint2", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clPinchZoomHandler::GetPinchCurrentPoint2();

         return *(TypeTraits< LVector2 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clPinchZoomHandler::GetPinchCurrentPoint2();
   }
   virtual LVector2    GetPinchCurrentCenter() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetPinchCurrentCenter" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetPinchCurrentCenter", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clPinchZoomHandler::GetPinchCurrentCenter();

         return *(TypeTraits< LVector2 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clPinchZoomHandler::GetPinchCurrentCenter();
   }
   virtual LMatrix4    GetPinchZoomMatrix() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetPinchZoomMatrix" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetPinchZoomMatrix", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clPinchZoomHandler::GetPinchZoomMatrix();

         return *(TypeTraits< LMatrix4 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clPinchZoomHandler::GetPinchZoomMatrix();
   }
   virtual LMatrix4    GetPinchRotationMatrix() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetPinchRotationMatrix" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetPinchRotationMatrix", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clPinchZoomHandler::GetPinchRotationMatrix();

         return *(TypeTraits< LMatrix4 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clPinchZoomHandler::GetPinchRotationMatrix();
   }
   virtual LMatrix4    GetPinchTranslationMatrix() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetPinchTranslationMatrix" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetPinchTranslationMatrix", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clPinchZoomHandler::GetPinchTranslationMatrix();

         return *(TypeTraits< LMatrix4 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clPinchZoomHandler::GetPinchTranslationMatrix();
   }
   virtual LMatrix4    GetPinchCombinedMatrix() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetPinchCombinedMatrix" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetPinchCombinedMatrix", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clPinchZoomHandler::GetPinchCombinedMatrix();

         return *(TypeTraits< LMatrix4 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clPinchZoomHandler::GetPinchCombinedMatrix();
   }
};

#endif // _DISABLE_TUNNELLERS_

#endif

/*
 * 16/01/2012
     Autogenerated via LinderScript Database Compiler 0.9.24
*/
