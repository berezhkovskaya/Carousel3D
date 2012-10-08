/**
 * \file clGUITransitionPage_Tunneller.h
 * \brief Tunneller for class: clGUITransitionPage
 * \version 0.6.06
 * \date 16/01/2012
 * \author Sergey Kosarevsky, 2005-2011
 * \author Viktor Latypov, 2007-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

//                             Base class: clGUIPage  (public)

#ifndef _clGUITransitionPage_Tunneller_
#define _clGUITransitionPage_Tunneller_

#include "Linderdaum/GUI/ComCtl/I_TransitionPage.h"

#if !defined( _DISABLE_TUNNELLERS_ )

class clGUITransitionPage_Tunneller: public clGUITransitionPage
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
   // iGestureResponder interface
   //
   virtual void    Event_PointerChanged(int P0, const LVector2& P1, bool P2)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_PointerChanged" ) )
      {
         clParametersList Params;

         ParameterType<int>::Type Param0;
         ParameterType<const LVector2&>::Type Param1;
         ParameterType<bool>::Type Param2;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );
         Param2.ReadValue( &P2 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );
         Params.push_back( &Param2 );

         bool MethodCalled = iObject::CallMethod( "Event_PointerChanged", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::Event_PointerChanged(P0, P1, P2);

         return;
      }

      iGUIResponder::Event_PointerChanged(P0, P1, P2);
   }
   virtual void    Event_PointerMoved(int P0, const LVector2& P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_PointerMoved" ) )
      {
         clParametersList Params;

         ParameterType<int>::Type Param0;
         ParameterType<const LVector2&>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Event_PointerMoved", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::Event_PointerMoved(P0, P1);

         return;
      }

      iGUIResponder::Event_PointerMoved(P0, P1);
   }
   virtual void    Event_Fling(LVector2 P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_Fling" ) )
      {
         clParametersList Params;

         ParameterType<LVector2>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Event_Fling", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::Event_Fling(P0);

         return;
      }

      iGUIResponder::Event_Fling(P0);
   }
   virtual void    Event_Fling2(const sTouchPoint& P0, const sTouchPoint& P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_Fling2" ) )
      {
         clParametersList Params;

         ParameterType<const sTouchPoint&>::Type Param0;
         ParameterType<const sTouchPoint&>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Event_Fling2", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGestureResponder::Event_Fling2(P0, P1);

         return;
      }

      iGestureResponder::Event_Fling2(P0, P1);
   }
   virtual void    Event_Drag(LVector2 P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_Drag" ) )
      {
         clParametersList Params;

         ParameterType<LVector2>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Event_Drag", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::Event_Drag(P0);

         return;
      }

      iGUIResponder::Event_Drag(P0);
   }
   virtual void    Event_Drag2(const sTouchPoint& P0, const sTouchPoint& P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_Drag2" ) )
      {
         clParametersList Params;

         ParameterType<const sTouchPoint&>::Type Param0;
         ParameterType<const sTouchPoint&>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Event_Drag2", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGestureResponder::Event_Drag2(P0, P1);

         return;
      }

      iGestureResponder::Event_Drag2(P0, P1);
   }
   //
   // iGUIResponder interface
   //
   virtual void    SetResponder(iGUIResponder* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetResponder" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetResponder", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::SetResponder(P0);

         return;
      }

      iGUIResponder::SetResponder(P0);
   }
   virtual bool    Event_Key(iGUIResponder* P0, const int P1, const bool P2)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_Key" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;
         ParameterType<const int>::Type Param1;
         ParameterType<const bool>::Type Param2;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );
         Param2.ReadValue( &P2 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );
         Params.push_back( &Param2 );

         bool MethodCalled = iObject::CallMethod( "Event_Key", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGUIPage::Event_Key(P0, P1, P2);

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGUIPage::Event_Key(P0, P1, P2);
   }
   virtual void    Event_ViewportDetached(iGUIResponder* P0, clViewport* P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_ViewportDetached" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;
         ParameterType<clViewport*>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Event_ViewportDetached", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::Event_ViewportDetached(P0, P1);

         return;
      }

      iGUIResponder::Event_ViewportDetached(P0, P1);
   }
   virtual void    Event_Registered(iGUIResponder* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_Registered" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Event_Registered", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::Event_Registered(P0);

         return;
      }

      iGUIResponder::Event_Registered(P0);
   }
   virtual void    Event_UnRegistered(iGUIResponder* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_UnRegistered" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Event_UnRegistered", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::Event_UnRegistered(P0);

         return;
      }

      iGUIResponder::Event_UnRegistered(P0);
   }
   virtual void    Event_Timer(iGUIResponder* P0, float P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_Timer" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;
         ParameterType<float>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Event_Timer", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clGUITransitionPage::Event_Timer(P0, P1);

         return;
      }

      clGUITransitionPage::Event_Timer(P0, P1);
   }
   virtual void    Event_Minimized(iGUIResponder* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_Minimized" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Event_Minimized", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::Event_Minimized(P0);

         return;
      }

      iGUIView::Event_Minimized(P0);
   }
   virtual void    Event_Maximized(iGUIResponder* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_Maximized" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Event_Maximized", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::Event_Maximized(P0);

         return;
      }

      iGUIView::Event_Maximized(P0);
   }
   virtual void    Event_Shrinked(iGUIResponder* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_Shrinked" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Event_Shrinked", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::Event_Shrinked(P0);

         return;
      }

      iGUIView::Event_Shrinked(P0);
   }
   virtual void    Event_Dissolved(iGUIResponder* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_Dissolved" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Event_Dissolved", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::Event_Dissolved(P0);

         return;
      }

      iGUIView::Event_Dissolved(P0);
   }
   virtual void    Event_CaptureChanged(iGUIResponder* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_CaptureChanged" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Event_CaptureChanged", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::Event_CaptureChanged(P0);

         return;
      }

      iGUIResponder::Event_CaptureChanged(P0);
   }
   virtual void    Event_MouseInside(iGUIResponder* P0, const LVector2& P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_MouseInside" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;
         ParameterType<const LVector2&>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Event_MouseInside", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::Event_MouseInside(P0, P1);

         return;
      }

      iGUIView::Event_MouseInside(P0, P1);
   }
   virtual void    Event_MouseOutside(iGUIResponder* P0, const LVector2& P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_MouseOutside" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;
         ParameterType<const LVector2&>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Event_MouseOutside", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::Event_MouseOutside(P0, P1);

         return;
      }

      iGUIView::Event_MouseOutside(P0, P1);
   }
   virtual void    Event_MouseLeft(iGUIResponder* P0, const LVector2& P1, const bool P2)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_MouseLeft" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;
         ParameterType<const LVector2&>::Type Param1;
         ParameterType<const bool>::Type Param2;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );
         Param2.ReadValue( &P2 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );
         Params.push_back( &Param2 );

         bool MethodCalled = iObject::CallMethod( "Event_MouseLeft", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::Event_MouseLeft(P0, P1, P2);

         return;
      }

      iGUIResponder::Event_MouseLeft(P0, P1, P2);
   }
   virtual void    Event_MouseRight(iGUIResponder* P0, const LVector2& P1, const bool P2)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_MouseRight" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;
         ParameterType<const LVector2&>::Type Param1;
         ParameterType<const bool>::Type Param2;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );
         Param2.ReadValue( &P2 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );
         Params.push_back( &Param2 );

         bool MethodCalled = iObject::CallMethod( "Event_MouseRight", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::Event_MouseRight(P0, P1, P2);

         return;
      }

      iGUIView::Event_MouseRight(P0, P1, P2);
   }
   virtual void    Event_LDoubleClick(iGUIResponder* P0, const LVector2& P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_LDoubleClick" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;
         ParameterType<const LVector2&>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Event_LDoubleClick", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::Event_LDoubleClick(P0, P1);

         return;
      }

      iGUIResponder::Event_LDoubleClick(P0, P1);
   }
   virtual void    Event_RDoubleClick(iGUIResponder* P0, const LVector2& P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_RDoubleClick" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;
         ParameterType<const LVector2&>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Event_RDoubleClick", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::Event_RDoubleClick(P0, P1);

         return;
      }

      iGUIResponder::Event_RDoubleClick(P0, P1);
   }
   virtual void    Event_Scroll(iGUIResponder* P0, const LGUIDirection P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_Scroll" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;
         ParameterType<const LGUIDirection>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Event_Scroll", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::Event_Scroll(P0, P1);

         return;
      }

      iGUIResponder::Event_Scroll(P0, P1);
   }
   virtual void    Event_StartDragging(iGUIResponder* P0, const LVector2& P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_StartDragging" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;
         ParameterType<const LVector2&>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Event_StartDragging", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::Event_StartDragging(P0, P1);

         return;
      }

      iGUIView::Event_StartDragging(P0, P1);
   }
   virtual void    Event_StopDragging(iGUIResponder* P0, const LVector2& P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_StopDragging" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;
         ParameterType<const LVector2&>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Event_StopDragging", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::Event_StopDragging(P0, P1);

         return;
      }

      iGUIView::Event_StopDragging(P0, P1);
   }
   virtual void    Event_StartResize(iGUIResponder* P0, const LVector2& P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_StartResize" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;
         ParameterType<const LVector2&>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Event_StartResize", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::Event_StartResize(P0, P1);

         return;
      }

      iGUIView::Event_StartResize(P0, P1);
   }
   virtual void    Event_StopResize(iGUIResponder* P0, const LVector2& P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_StopResize" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;
         ParameterType<const LVector2&>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Event_StopResize", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::Event_StopResize(P0, P1);

         return;
      }

      iGUIView::Event_StopResize(P0, P1);
   }
   virtual void    Event_EnterTopmost(iGUIResponder* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_EnterTopmost" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Event_EnterTopmost", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::Event_EnterTopmost(P0);

         return;
      }

      iGUIResponder::Event_EnterTopmost(P0);
   }
   virtual void    Event_LeaveTopmost(iGUIResponder* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_LeaveTopmost" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Event_LeaveTopmost", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::Event_LeaveTopmost(P0);

         return;
      }

      iGUIResponder::Event_LeaveTopmost(P0);
   }
   virtual void    Event_PageActive(iGUIResponder* P0, clGUIPage* P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_PageActive" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;
         ParameterType<clGUIPage*>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Event_PageActive", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clGUITransitionPage::Event_PageActive(P0, P1);

         return;
      }

      clGUITransitionPage::Event_PageActive(P0, P1);
   }
   virtual void    Event_PageInactive(iGUIResponder* P0, clGUIPage* P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_PageInactive" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;
         ParameterType<clGUIPage*>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "Event_PageInactive", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clGUITransitionPage::Event_PageInactive(P0, P1);

         return;
      }

      clGUITransitionPage::Event_PageInactive(P0, P1);
   }
   virtual void    Event_PageAnimationActive(iGUIResponder* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_PageAnimationActive" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Event_PageAnimationActive", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::Event_PageAnimationActive(P0);

         return;
      }

      iGUIResponder::Event_PageAnimationActive(P0);
   }
   virtual void    Event_PageAnimationInactive(iGUIResponder* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Event_PageAnimationInactive" ) )
      {
         clParametersList Params;

         ParameterType<iGUIResponder*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Event_PageAnimationInactive", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIResponder::Event_PageAnimationInactive(P0);

         return;
      }

      iGUIResponder::Event_PageAnimationInactive(P0);
   }
   //
   // iGUIRegion interface
   //
   virtual bool    IsInLocalCoords() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsInLocalCoords" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsInLocalCoords", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::IsInLocalCoords();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::IsInLocalCoords();
   }
   virtual float    GetLifeTime() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetLifeTime" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetLifeTime", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::GetLifeTime();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::GetLifeTime();
   }
   virtual void    SetRegionState(LRegionState P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetRegionState" ) )
      {
         clParametersList Params;

         ParameterType<LRegionState>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetRegionState", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::SetRegionState(P0);

         return;
      }

      iGUIView::SetRegionState(P0);
   }
   virtual LRegionState    GetRegionState() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetRegionState" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetRegionState", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::GetRegionState();

         return *(TypeTraits< LRegionState >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::GetRegionState();
   }
   virtual void    OutTextXY(const float P0, const float P1, const LString& P2, const LVector4& P3)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "OutTextXY" ) )
      {
         clParametersList Params;

         ParameterType<const float>::Type Param0;
         ParameterType<const float>::Type Param1;
         ParameterType<const LString&>::Type Param2;
         ParameterType<const LVector4&>::Type Param3;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );
         Param2.ReadValue( &P2 );
         Param3.ReadValue( &P3 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );
         Params.push_back( &Param2 );
         Params.push_back( &Param3 );

         bool MethodCalled = iObject::CallMethod( "OutTextXY", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIRegion::OutTextXY(P0, P1, P2, P3);

         return;
      }

      iGUIRegion::OutTextXY(P0, P1, P2, P3);
   }
   virtual LVector2    Screen2Local(const LVector2& P0) const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Screen2Local" ) )
      {
         clParametersList Params;

         ParameterType<const LVector2&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Screen2Local", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::Screen2Local(P0);

         return *(TypeTraits< LVector2 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::Screen2Local(P0);
   }
   virtual LVector2    Local2Screen(const LVector2& P0) const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Local2Screen" ) )
      {
         clParametersList Params;

         ParameterType<const LVector2&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Local2Screen", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::Local2Screen(P0);

         return *(TypeTraits< LVector2 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::Local2Screen(P0);
   }
   virtual bool    Contains(const LVector2& P0) const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Contains" ) )
      {
         clParametersList Params;

         ParameterType<const LVector2&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Contains", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::Contains(P0);

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::Contains(P0);
   }
   virtual bool    ContainsCaption(const LVector2& P0) const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "ContainsCaption" ) )
      {
         clParametersList Params;

         ParameterType<const LVector2&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "ContainsCaption", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGUIPage::ContainsCaption(P0);

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGUIPage::ContainsCaption(P0);
   }
   virtual bool    ContainsResizeCorner(const LVector2& P0) const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "ContainsResizeCorner" ) )
      {
         clParametersList Params;

         ParameterType<const LVector2&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "ContainsResizeCorner", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGUIPage::ContainsResizeCorner(P0);

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGUIPage::ContainsResizeCorner(P0);
   }
   virtual bool    ContainsCloseButton(const LVector2& P0) const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "ContainsCloseButton" ) )
      {
         clParametersList Params;

         ParameterType<const LVector2&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "ContainsCloseButton", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGUIPage::ContainsCloseButton(P0);

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGUIPage::ContainsCloseButton(P0);
   }
   virtual void    MoveTo(const LVector2& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "MoveTo" ) )
      {
         clParametersList Params;

         ParameterType<const LVector2&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "MoveTo", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIRegion::MoveTo(P0);

         return;
      }

      iGUIRegion::MoveTo(P0);
   }
   virtual void    MoveRel(const LVector2& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "MoveRel" ) )
      {
         clParametersList Params;

         ParameterType<const LVector2&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "MoveRel", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::MoveRel(P0);

         return;
      }

      iGUIView::MoveRel(P0);
   }
   virtual void    SetSize(const float P0, const float P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetSize" ) )
      {
         clParametersList Params;

         ParameterType<const float>::Type Param0;
         ParameterType<const float>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "SetSize", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::SetSize(P0, P1);

         return;
      }

      iGUIView::SetSize(P0, P1);
   }
   virtual float    CenterTextHorizontal(const LString& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "CenterTextHorizontal" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "CenterTextHorizontal", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::CenterTextHorizontal(P0);

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::CenterTextHorizontal(P0);
   }
   virtual float    CenterTextVertical()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "CenterTextVertical" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "CenterTextVertical", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::CenterTextVertical();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::CenterTextVertical();
   }
   virtual float    GetMaximizedHeight() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetMaximizedHeight" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetMaximizedHeight", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::GetMaximizedHeight();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::GetMaximizedHeight();
   }
   virtual void    SetMaximizedHeight(float P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetMaximizedHeight" ) )
      {
         clParametersList Params;

         ParameterType<float>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetMaximizedHeight", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIRegion::SetMaximizedHeight(P0);

         return;
      }

      iGUIRegion::SetMaximizedHeight(P0);
   }
   virtual float    GetWidth() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetWidth" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetWidth", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::GetWidth();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::GetWidth();
   }
   virtual float    GetHeight() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetHeight" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetHeight", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::GetHeight();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::GetHeight();
   }
   virtual float    GetX1() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetX1" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetX1", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::GetX1();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::GetX1();
   }
   virtual float    GetY1() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetY1" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetY1", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::GetY1();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::GetY1();
   }
   virtual float    GetX2() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetX2" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetX2", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::GetX2();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::GetX2();
   }
   virtual float    GetY2() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetY2" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetY2", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::GetY2();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::GetY2();
   }
   virtual LVector4    GetUniformsVec() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetUniformsVec" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetUniformsVec", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::GetUniformsVec();

         return *(TypeTraits< LVector4 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::GetUniformsVec();
   }
   virtual void    UpdateUniforms(clRenderState* P0) const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "UpdateUniforms" ) )
      {
         clParametersList Params;

         ParameterType<clRenderState*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "UpdateUniforms", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIRegion::UpdateUniforms(P0);

         return;
      }

      iGUIRegion::UpdateUniforms(P0);
   }
   virtual void    UpdateUniformsV(clRenderState* P0, const LVector4& P1) const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "UpdateUniformsV" ) )
      {
         clParametersList Params;

         ParameterType<clRenderState*>::Type Param0;
         ParameterType<const LVector4&>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "UpdateUniformsV", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIRegion::UpdateUniformsV(P0, P1);

         return;
      }

      iGUIRegion::UpdateUniformsV(P0, P1);
   }
   virtual void    SetWidth(float P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetWidth" ) )
      {
         clParametersList Params;

         ParameterType<float>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetWidth", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIRegion::SetWidth(P0);

         return;
      }

      iGUIRegion::SetWidth(P0);
   }
   virtual void    SetHeight(float P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetHeight" ) )
      {
         clParametersList Params;

         ParameterType<float>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetHeight", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIRegion::SetHeight(P0);

         return;
      }

      iGUIRegion::SetHeight(P0);
   }
   virtual LString    GetTitle() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetTitle" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetTitle", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::GetTitle();

         return *(TypeTraits< LString >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::GetTitle();
   }
   virtual void    SetTitle(const LString& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetTitle" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetTitle", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIRegion::SetTitle(P0);

         return;
      }

      iGUIRegion::SetTitle(P0);
   }
   virtual void    SetCoords(float P0, float P1, float P2, float P3)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetCoords" ) )
      {
         clParametersList Params;

         ParameterType<float>::Type Param0;
         ParameterType<float>::Type Param1;
         ParameterType<float>::Type Param2;
         ParameterType<float>::Type Param3;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );
         Param2.ReadValue( &P2 );
         Param3.ReadValue( &P3 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );
         Params.push_back( &Param2 );
         Params.push_back( &Param3 );

         bool MethodCalled = iObject::CallMethod( "SetCoords", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIRegion::SetCoords(P0, P1, P2, P3);

         return;
      }

      iGUIRegion::SetCoords(P0, P1, P2, P3);
   }
   virtual void    SetCoordsV(const LVector4& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetCoordsV" ) )
      {
         clParametersList Params;

         ParameterType<const LVector4&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetCoordsV", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIRegion::SetCoordsV(P0);

         return;
      }

      iGUIRegion::SetCoordsV(P0);
   }
   virtual void    ParseCoords(const LString& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "ParseCoords" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "ParseCoords", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIRegion::ParseCoords(P0);

         return;
      }

      iGUIRegion::ParseCoords(P0);
   }
   virtual LString    GetInitialCoords() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetInitialCoords" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetInitialCoords", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::GetInitialCoords();

         return *(TypeTraits< LString >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::GetInitialCoords();
   }
   virtual LVector4    GetCoords() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetCoords" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetCoords", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::GetCoords();

         return *(TypeTraits< LVector4 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::GetCoords();
   }
   virtual void    SetOpacity(float P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetOpacity" ) )
      {
         clParametersList Params;

         ParameterType<float>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetOpacity", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIRegion::SetOpacity(P0);

         return;
      }

      iGUIRegion::SetOpacity(P0);
   }
   virtual float    GetOpacity() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetOpacity" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetOpacity", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::GetOpacity();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::GetOpacity();
   }
   virtual void    SetTextOffsets(const LVector4& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetTextOffsets" ) )
      {
         clParametersList Params;

         ParameterType<const LVector4&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetTextOffsets", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIRegion::SetTextOffsets(P0);

         return;
      }

      iGUIRegion::SetTextOffsets(P0);
   }
   virtual LVector4    GetTextOffsets()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetTextOffsets" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetTextOffsets", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIRegion::GetTextOffsets();

         return *(TypeTraits< LVector4 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIRegion::GetTextOffsets();
   }
   virtual iFontProperties*    GetFont() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetFont" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetFont", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::GetFont();

         return *(TypeTraits< iFontProperties* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::GetFont();
   }
protected:
   virtual void    Precenter(iGUIRegion* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Precenter" ) )
      {
         clParametersList Params;

         ParameterType<iGUIRegion*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "Precenter", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIRegion::Precenter(P0);

         return;
      }

      iGUIRegion::Precenter(P0);
   }
   virtual void    DockCoordsToThisRegion(float* P0, float* P1, const float P2, const float P3)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "DockCoordsToThisRegion" ) )
      {
         clParametersList Params;

         ParameterType<float*>::Type Param0;
         ParameterType<float*>::Type Param1;
         ParameterType<const float>::Type Param2;
         ParameterType<const float>::Type Param3;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );
         Param2.ReadValue( &P2 );
         Param3.ReadValue( &P3 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );
         Params.push_back( &Param2 );
         Params.push_back( &Param3 );

         bool MethodCalled = iObject::CallMethod( "DockCoordsToThisRegion", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIRegion::DockCoordsToThisRegion(P0, P1, P2, P3);

         return;
      }

      iGUIRegion::DockCoordsToThisRegion(P0, P1, P2, P3);
   }
   virtual void    DockSizeToThisRegion(const float P0, const float P1, float* P2, float* P3)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "DockSizeToThisRegion" ) )
      {
         clParametersList Params;

         ParameterType<const float>::Type Param0;
         ParameterType<const float>::Type Param1;
         ParameterType<float*>::Type Param2;
         ParameterType<float*>::Type Param3;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );
         Param2.ReadValue( &P2 );
         Param3.ReadValue( &P3 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );
         Params.push_back( &Param2 );
         Params.push_back( &Param3 );

         bool MethodCalled = iObject::CallMethod( "DockSizeToThisRegion", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIRegion::DockSizeToThisRegion(P0, P1, P2, P3);

         return;
      }

      iGUIRegion::DockSizeToThisRegion(P0, P1, P2, P3);
   }
public:
   //
   // iGUIViewRenderer interface
   //
   virtual void    PreRender()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "PreRender" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "PreRender", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clGUITransitionPage::PreRender();

         return;
      }

      clGUITransitionPage::PreRender();
   }
   virtual void    PostRender()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "PostRender" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "PostRender", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::PostRender();

         return;
      }

      iGUIView::PostRender();
   }
   virtual void    SetRenderer(iGUIViewRenderer* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetRenderer" ) )
      {
         clParametersList Params;

         ParameterType<iGUIViewRenderer*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetRenderer", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIViewRenderer::SetRenderer(P0);

         return;
      }

      iGUIViewRenderer::SetRenderer(P0);
   }
   virtual iGUIViewRenderer*    GetRenderer() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetRenderer" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetRenderer", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIViewRenderer::GetRenderer();

         return *(TypeTraits< iGUIViewRenderer* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIViewRenderer::GetRenderer();
   }
   //
   // iGUIView interface
   //
   virtual float    GetViewOpacity() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetViewOpacity" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetViewOpacity", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::GetViewOpacity();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::GetViewOpacity();
   }
   virtual void    AddView(iGUIView* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "AddView" ) )
      {
         clParametersList Params;

         ParameterType<iGUIView*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "AddView", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::AddView(P0);

         return;
      }

      iGUIView::AddView(P0);
   }
   virtual void    AddViews(const clViewsList& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "AddViews" ) )
      {
         clParametersList Params;

         ParameterType<const clViewsList&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "AddViews", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::AddViews(P0);

         return;
      }

      iGUIView::AddViews(P0);
   }
   virtual void    RemoveView(iGUIView* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "RemoveView" ) )
      {
         clParametersList Params;

         ParameterType<iGUIView*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "RemoveView", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::RemoveView(P0);

         return;
      }

      iGUIView::RemoveView(P0);
   }
   virtual bool    IsMouseOver() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsMouseOver" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsMouseOver", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::IsMouseOver();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::IsMouseOver();
   }
   virtual LVector2    GetMousePos() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetMousePos" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetMousePos", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::GetMousePos();

         return *(TypeTraits< LVector2 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::GetMousePos();
   }
   virtual bool    IsViewKeyboardSelectable() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsViewKeyboardSelectable" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsViewKeyboardSelectable", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::IsViewKeyboardSelectable();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::IsViewKeyboardSelectable();
   }
   virtual bool    IsPage() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsPage" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsPage", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGUIPage::IsPage();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGUIPage::IsPage();
   }
   virtual bool    IsViewDockable() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsViewDockable" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsViewDockable", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGUIPage::IsViewDockable();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGUIPage::IsViewDockable();
   }
   virtual bool    IsViewDraggable() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsViewDraggable" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsViewDraggable", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGUIPage::IsViewDraggable();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGUIPage::IsViewDraggable();
   }
   virtual bool    IsViewSizeable() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsViewSizeable" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsViewSizeable", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGUIPage::IsViewSizeable();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGUIPage::IsViewSizeable();
   }
   virtual bool    IsViewDissolvable() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsViewDissolvable" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsViewDissolvable", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::IsViewDissolvable();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::IsViewDissolvable();
   }
   virtual bool    IsViewDraggableAt(const LVector2& P0) const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsViewDraggableAt" ) )
      {
         clParametersList Params;

         ParameterType<const LVector2&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "IsViewDraggableAt", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::IsViewDraggableAt(P0);

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::IsViewDraggableAt(P0);
   }
   virtual bool    IsViewSizeableAt(const LVector2& P0) const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsViewSizeableAt" ) )
      {
         clParametersList Params;

         ParameterType<const LVector2&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "IsViewSizeableAt", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::IsViewSizeableAt(P0);

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::IsViewSizeableAt(P0);
   }
   virtual bool    IsTopmost() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsTopmost" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsTopmost", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::IsTopmost();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::IsTopmost();
   }
   virtual bool    IsPageActive() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsPageActive" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsPageActive", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGUIPage::IsPageActive();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGUIPage::IsPageActive();
   }
   virtual void    DragTo(const LVector2& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "DragTo" ) )
      {
         clParametersList Params;

         ParameterType<const LVector2&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "DragTo", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::DragTo(P0);

         return;
      }

      iGUIView::DragTo(P0);
   }
   virtual void    ResizeCornerTo(const LVector2& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "ResizeCornerTo" ) )
      {
         clParametersList Params;

         ParameterType<const LVector2&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "ResizeCornerTo", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::ResizeCornerTo(P0);

         return;
      }

      iGUIView::ResizeCornerTo(P0);
   }
   virtual bool    CanFocus() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "CanFocus" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "CanFocus", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::CanFocus();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::CanFocus();
   }
   virtual bool    IsBeenDragged() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsBeenDragged" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsBeenDragged", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::IsBeenDragged();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::IsBeenDragged();
   }
   virtual bool    IsBeenSized() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsBeenSized" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsBeenSized", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::IsBeenSized();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::IsBeenSized();
   }
   virtual float    SuggestMinimalWidth() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SuggestMinimalWidth" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "SuggestMinimalWidth", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::SuggestMinimalWidth();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::SuggestMinimalWidth();
   }
   virtual float    SuggestMinimalHeight() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SuggestMinimalHeight" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "SuggestMinimalHeight", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::SuggestMinimalHeight();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::SuggestMinimalHeight();
   }
   virtual bool    IsMinimized() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsMinimized" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsMinimized", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::IsMinimized();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::IsMinimized();
   }
   virtual bool    IsMaximized() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsMaximized" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsMaximized", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::IsMaximized();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::IsMaximized();
   }
   virtual void    Minimize()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Minimize" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "Minimize", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::Minimize();

         return;
      }

      iGUIView::Minimize();
   }
   virtual void    Maximize()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "Maximize" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "Maximize", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::Maximize();

         return;
      }

      iGUIView::Maximize();
   }
   virtual float    GetToolTipTime() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetToolTipTime" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetToolTipTime", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::GetToolTipTime();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::GetToolTipTime();
   }
   virtual LString    GetToolTipText() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetToolTipText" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetToolTipText", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::GetToolTipText();

         return *(TypeTraits< LString >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::GetToolTipText();
   }
   virtual void    SetToolTipTime(float P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetToolTipTime" ) )
      {
         clParametersList Params;

         ParameterType<float>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetToolTipTime", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::SetToolTipTime(P0);

         return;
      }

      iGUIView::SetToolTipTime(P0);
   }
   virtual void    SetToolTipText(const LString& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetToolTipText" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetToolTipText", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::SetToolTipText(P0);

         return;
      }

      iGUIView::SetToolTipText(P0);
   }
   virtual void    ExecuteCommandsStack()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "ExecuteCommandsStack" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "ExecuteCommandsStack", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::ExecuteCommandsStack();

         return;
      }

      iGUIView::ExecuteCommandsStack();
   }
   virtual void    ExecuteCommandsStackFadeIn()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "ExecuteCommandsStackFadeIn" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "ExecuteCommandsStackFadeIn", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::ExecuteCommandsStackFadeIn();

         return;
      }

      iGUIView::ExecuteCommandsStackFadeIn();
   }
   virtual void    DispatchSpecialKey(const int P0, const bool P1, const LVector2& P2)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "DispatchSpecialKey" ) )
      {
         clParametersList Params;

         ParameterType<const int>::Type Param0;
         ParameterType<const bool>::Type Param1;
         ParameterType<const LVector2&>::Type Param2;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );
         Param2.ReadValue( &P2 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );
         Params.push_back( &Param2 );

         bool MethodCalled = iObject::CallMethod( "DispatchSpecialKey", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::DispatchSpecialKey(P0, P1, P2);

         return;
      }

      iGUIView::DispatchSpecialKey(P0, P1, P2);
   }
   virtual clCVar*    GetConsoleVariable() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetConsoleVariable" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetConsoleVariable", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::GetConsoleVariable();

         return *(TypeTraits< clCVar* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::GetConsoleVariable();
   }
   virtual LString    GetConsoleCommand() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetConsoleCommand" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetConsoleCommand", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::GetConsoleCommand();

         return *(TypeTraits< LString >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::GetConsoleCommand();
   }
   virtual LString    GetConsoleCommandFadeIn() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetConsoleCommandFadeIn" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetConsoleCommandFadeIn", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::GetConsoleCommandFadeIn();

         return *(TypeTraits< LString >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::GetConsoleCommandFadeIn();
   }
   virtual iGUIView*    GetParentView() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetParentView" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetParentView", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::GetParentView();

         return *(TypeTraits< iGUIView* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::GetParentView();
   }
   virtual iGUIView*    FindSubViewByID(const LString& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "FindSubViewByID" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "FindSubViewByID", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::FindSubViewByID(P0);

         return *(TypeTraits< iGUIView* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::FindSubViewByID(P0);
   }
   virtual void    FindAllSubViewsByID(const LString& P0, LArray<iGUIView*>* P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "FindAllSubViewsByID" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;
         ParameterType<LArray<iGUIView*>*>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "FindAllSubViewsByID", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::FindAllSubViewsByID(P0, P1);

         return;
      }

      iGUIView::FindAllSubViewsByID(P0, P1);
   }
   virtual size_t    GetTotalSubViews() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetTotalSubViews" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetTotalSubViews", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::GetTotalSubViews();

         return *(TypeTraits< size_t >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::GetTotalSubViews();
   }
   virtual iGUIView*    GetSubView(size_t P0) const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetSubView" ) )
      {
         clParametersList Params;

         ParameterType<size_t>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "GetSubView", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::GetSubView(P0);

         return *(TypeTraits< iGUIView* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::GetSubView(P0);
   }
   virtual void    SetSubView(size_t P0, iGUIView* P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetSubView" ) )
      {
         clParametersList Params;

         ParameterType<size_t>::Type Param0;
         ParameterType<iGUIView*>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "SetSubView", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::SetSubView(P0, P1);

         return;
      }

      iGUIView::SetSubView(P0, P1);
   }
   virtual void    SetVisible(bool P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetVisible" ) )
      {
         clParametersList Params;

         ParameterType<bool>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetVisible", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::SetVisible(P0);

         return;
      }

      iGUIView::SetVisible(P0);
   }
   virtual bool    IsVisible() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsVisible" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsVisible", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::IsVisible();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::IsVisible();
   }
   virtual void    SetInteractive(bool P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetInteractive" ) )
      {
         clParametersList Params;

         ParameterType<bool>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetInteractive", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::SetInteractive(P0);

         return;
      }

      iGUIView::SetInteractive(P0);
   }
   virtual bool    IsInteractive() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "IsInteractive" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "IsInteractive", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::IsInteractive();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::IsInteractive();
   }
   virtual void    SetOnTopPriority(int P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetOnTopPriority" ) )
      {
         clParametersList Params;

         ParameterType<int>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetOnTopPriority", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::SetOnTopPriority(P0);

         return;
      }

      iGUIView::SetOnTopPriority(P0);
   }
   virtual int    GetOnTopPriority() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetOnTopPriority" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetOnTopPriority", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::GetOnTopPriority();

         return *(TypeTraits< int >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::GetOnTopPriority();
   }
   virtual void    RenderHierarchy()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "RenderHierarchy" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "RenderHierarchy", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clGUIPage::RenderHierarchy();

         return;
      }

      clGUIPage::RenderHierarchy();
   }
   virtual void    RenderTopmost()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "RenderTopmost" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "RenderTopmost", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::RenderTopmost();

         return;
      }

      iGUIView::RenderTopmost();
   }
   virtual void    RemoveAllChildViews()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "RemoveAllChildViews" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "RemoveAllChildViews", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::RemoveAllChildViews();

         return;
      }

      iGUIView::RemoveAllChildViews();
   }
   virtual iGUIView*    FindView(const LVector2& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "FindView" ) )
      {
         clParametersList Params;

         ParameterType<const LVector2&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "FindView", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::FindView(P0);

         return *(TypeTraits< iGUIView* >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::FindView(P0);
   }
   virtual void    ToFront(iGUIView* P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "ToFront" ) )
      {
         clParametersList Params;

         ParameterType<iGUIView*>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "ToFront", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::ToFront(P0);

         return;
      }

      iGUIView::ToFront(P0);
   }
   virtual void    BindConsoleCommandS(const LString& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "BindConsoleCommandS" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "BindConsoleCommandS", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::BindConsoleCommandS(P0);

         return;
      }

      iGUIView::BindConsoleCommandS(P0);
   }
   virtual void    BindConsoleCommandFadeInS(const LString& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "BindConsoleCommandFadeInS" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "BindConsoleCommandFadeInS", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::BindConsoleCommandFadeInS(P0);

         return;
      }

      iGUIView::BindConsoleCommandFadeInS(P0);
   }
   virtual void    BindConsoleVariableS(const LString& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "BindConsoleVariableS" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "BindConsoleVariableS", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::BindConsoleVariableS(P0);

         return;
      }

      iGUIView::BindConsoleVariableS(P0);
   }
   virtual LString    GetConsoleVariableName() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetConsoleVariableName" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetConsoleVariableName", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::GetConsoleVariableName();

         return *(TypeTraits< LString >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::GetConsoleVariableName();
   }
   virtual LVector4    GetDefaultColor() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetDefaultColor" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetDefaultColor", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::GetDefaultColor();

         return *(TypeTraits< LVector4 >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::GetDefaultColor();
   }
   virtual void    SetDefaultColor(const LVector4& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetDefaultColor" ) )
      {
         clParametersList Params;

         ParameterType<const LVector4&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetDefaultColor", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::SetDefaultColor(P0);

         return;
      }

      iGUIView::SetDefaultColor(P0);
   }
   virtual void    SetDefaultColorHex(const LString& P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetDefaultColorHex" ) )
      {
         clParametersList Params;

         ParameterType<const LString&>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetDefaultColorHex", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::SetDefaultColorHex(P0);

         return;
      }

      iGUIView::SetDefaultColorHex(P0);
   }
   virtual LString    GetDefaultColorHex() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetDefaultColorHex" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetDefaultColorHex", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::GetDefaultColorHex();

         return *(TypeTraits< LString >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::GetDefaultColorHex();
   }
   virtual LString    GetCommand(int P0) const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetCommand" ) )
      {
         clParametersList Params;

         ParameterType<int>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "GetCommand", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return iGUIView::GetCommand(P0);

         return *(TypeTraits< LString >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return iGUIView::GetCommand(P0);
   }
   virtual void    SetCommand(int P0, const LString& P1)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetCommand" ) )
      {
         clParametersList Params;

         ParameterType<int>::Type Param0;
         ParameterType<const LString&>::Type Param1;

         Param0.ReadValue( &P0 );
         Param1.ReadValue( &P1 );

         Params.push_back( &Param0 );
         Params.push_back( &Param1 );

         bool MethodCalled = iObject::CallMethod( "SetCommand", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) iGUIView::SetCommand(P0, P1);

         return;
      }

      iGUIView::SetCommand(P0, P1);
   }
   //
   // clGUIPage interface
   //
   virtual bool    HasFallback()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "HasFallback" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "HasFallback", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGUIPage::HasFallback();

         return *(TypeTraits< bool >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGUIPage::HasFallback();
   }
   //
   // clGUITransitionPage interface
   //
   virtual void    StartAnimation()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "StartAnimation" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "StartAnimation", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clGUITransitionPage::StartAnimation();

         return;
      }

      clGUITransitionPage::StartAnimation();
   }
   virtual void    FinishAnimation()
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "FinishAnimation" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "FinishAnimation", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clGUITransitionPage::FinishAnimation();

         return;
      }

      clGUITransitionPage::FinishAnimation();
   }
   virtual void    SetTransitionPercent(float P0)
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "SetTransitionPercent" ) )
      {
         clParametersList Params;

         ParameterType<float>::Type Param0;

         Param0.ReadValue( &P0 );

         Params.push_back( &Param0 );

         bool MethodCalled = iObject::CallMethod( "SetTransitionPercent", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) clGUITransitionPage::SetTransitionPercent(P0);

         return;
      }

      clGUITransitionPage::SetTransitionPercent(P0);
   }
   virtual float    GetTransitionPercent() const
   {
      if ( !iObject::FInheritedCall && iObject::IsMethodOverriden( "GetTransitionPercent" ) )
      {
         clParametersList Params;

         bool MethodCalled = iObject::CallMethod( "GetTransitionPercent", Params, iObject::FInheritedCall );

         if ( !MethodCalled ) return clGUITransitionPage::GetTransitionPercent();

         return *(TypeTraits< float >::ReferredType*)iObject::GetReturnValue()->GetNativeBlock();
      }

      return clGUITransitionPage::GetTransitionPercent();
   }
};

#endif // _DISABLE_TUNNELLERS_

#endif

/*
 * 16/01/2012
     Autogenerated via LinderScript Database Compiler 0.9.24
*/
