/**
 * \file GUIManager.cpp
 * \brief Internal GUI manager
 * \version 0.5.91
 * \date 07/07/2007-2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "GUIManager.h"
#include "GUIDesktop.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/CVars.h"
#include "Core/Linker.h"
#include "Core/Console.h"
#include "Core/Logger.h"
#include "Core/VFS/FileSystem.h"
#include "Core/VFS/iIStream.h"
#include "Core/VFS/iOStream.h"
#include "Utils/Viewport.h"
#include "Geometry/GeomServ.h"
#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"

// for utility constructor implementation
#include "GUI/ComCtl/I_InputLine.h"
#include "GUI/ComCtl/I_StaticWidget.h"
#include "GUI/ComCtl/I_Slider.h"
#include "GUI/ComCtl/I_PushButton.h"
#include "GUI/ComCtl/I_CheckBox.h"
#include "GUI/ComCtl/I_ColorDial.h"
#include "GUI/ComCtl/I_FSGraph.h"
#include "GUI/ComCtl/I_MtxView.h"
#include "GUI/ComCtl/I_SceneView.h"
#include "GUI/ComCtl/I_Page.h"
#include "GUI/ComCtl/I_TransitionPage.h"

#include "LKeys.h"
#include "LColors.h"

#include <algorithm>

clGUIManager::clGUIManager()
	: FGUISoundEnabled( NULL ),
	  FGUIVisible( true ),
	  FDesktop( NULL ),
	  FRecheckMouse( true ),
	  FMousePosition( 0, 0 ),
	  FCursorForm( CFORM_NOCURSOR ),
	  FMouseIdleTime( 0 ),
	  FMouseMovementDelay( 0 ),
	  FWindowBuffer( NULL ),
	  FActiveView( NULL ),
	  FCapturedView( NULL ),
	  FViewBeenSized( NULL ),
	  FViewBeenDragged( NULL ),
	  FTopmostView( NULL ),
	  FActivePage( NULL )
{
}

iRenderTarget* clGUIManager::GetWindowBuffer() const
{
	if ( !FWindowBuffer )
	{
		// create offscreen buffer for this window (width and height is according to maximized size)
		int Width  = Env->Viewport->GetWidth();
		int Height = Env->Viewport->GetHeight();

		int MaxWidth  = 1600;
		int MaxHeight = 1200;

		FWindowBuffer = Env->Renderer->CreateRenderTarget( std::min( Width, MaxWidth ), std::min( Height, MaxHeight ), 0, 8, false, 1 );
	}

	return FWindowBuffer;
}

iVertexArray* clGUIManager::GetDefaultRect() const
{
	return FDefaultRect;
}

iVertexArray* clGUIManager::GetDefaultBevelRect() const
{
	return FDefaultBevelRect;
}

iVertexArray* clGUIManager::GetDefaultTriangle() const
{
	return FDefaultTriangle;
}

bool clGUIManager::IsGUISoundEnabled() const
{
	return FGUISoundEnabled->GetBool();
}

void clGUIManager::SetGUISoundEnabled( bool Enabled )
{
	FGUISoundEnabled->SetBool( Enabled );
}

void clGUIManager::AfterConstruction()
{
	guard();

	FGUISoundEnabled = Env->Console->GetVar( "GUI.SoundEnabled" );
	FGUISoundEnabled->SetBool( true );

	InitLookAndFeel();

	FDefaultRect = Env->Renderer->AllocateEmptyVA();
	FDefaultBevelRect = Env->Renderer->AllocateEmptyVA();
	FDefaultTriangle = Env->Renderer->AllocateEmptyVA();

	FDefaultRect->SetVertexAttribs( clGeomServ::CreateRect2D( 0, 0, 1, 1, 0, false, 0 ) );
	FDefaultBevelRect->SetVertexAttribs( clGeomServ::CreateBevelRect2D( 0, 0, 1, 1, 0, GetDefaultButtonBevelRadius() ) );
	FDefaultTriangle->SetVertexAttribs( clGeomServ::CreateTriangle2D( 0, 0, 1, 1, 0 ) );

	Env->Console->RegisterCommand( "AddGUI",        Utils::Bind( &clGUIManager::AddGUIC, this ) );
	Env->Console->RegisterCommand( "AddGUITo",      Utils::Bind( &clGUIManager::AddGUIToC, this ) );
	Env->Console->RegisterCommand( "SaveGUI",       Utils::Bind( &clGUIManager::SaveGUIC, this ) );
	Env->Console->RegisterCommand( "ResetGUI",      Utils::Bind( &clGUIManager::ResetGUIC, this ) );
	Env->Console->RegisterCommand( "CloseView",     Utils::Bind( &clGUIManager::CloseViewC, this ) );
	Env->Console->RegisterCommand( "ToggleGUI",     Utils::Bind( &clGUIManager::ToggleGUIC, this ) );
	Env->Console->RegisterCommand( "SetActivePage", Utils::Bind( &clGUIManager::SetActivePageC, this ) );

	Env->Connect( L_EVENT_TIMER,       BIND( &clGUIManager::Event_TIMER    ) );
	Env->Connect( L_EVENT_POSTINIT,    BIND( &clGUIManager::Event_POSTINIT ) );

	Env->ConnectWithPriority( L_EVENT_DRAWOVERLAY, BIND( &clGUIManager::Event_DRAWOVERLAY ), 254 );
	Env->ConnectWithPriority( L_EVENT_KEY,         BIND( &clGUIManager::Event_KEY         ),  -1 );

	unguard();
}

clGUIManager::~clGUIManager()
{
	delete( FDefaultRect );
	delete( FDefaultBevelRect );
	delete( FDefaultTriangle );

	Env->DisconnectObject( this );

	ResetGUIC( "" );

	delete( FDesktop );

	Env->Console->UnRegisterCommand( "AddGUI",    Utils::Bind( &clGUIManager::AddGUIC, this ) );
	Env->Console->UnRegisterCommand( "AddGUITo",  Utils::Bind( &clGUIManager::AddGUIToC, this ) );
	Env->Console->UnRegisterCommand( "SaveGUI",   Utils::Bind( &clGUIManager::SaveGUIC, this ) );
	Env->Console->UnRegisterCommand( "ResetGUI",  Utils::Bind( &clGUIManager::ResetGUIC, this ) );
	Env->Console->UnRegisterCommand( "CloseView", Utils::Bind( &clGUIManager::CloseViewC, this ) );
	Env->Console->UnRegisterCommand( "ToggleGUI", Utils::Bind( &clGUIManager::ToggleGUIC, this ) );
	Env->Console->UnRegisterCommand( "SetActivePage", Utils::Bind( &clGUIManager::SetActivePageC, this ) );

	// release buffer
	delete( FWindowBuffer );
}

bool clGUIManager::IsTouchScreen() const
{
#if defined( OS_WINDOWS )
	return false;
#elif defined( OS_ANDROID )
	return true;
#else
	// return something
#endif
}

void clGUIManager::HideGUI()
{
	FGUIVisible = false;
}

void clGUIManager::ShowGUI()
{
	FGUIVisible = true;
}

void clGUIManager::ToggleGUIC( const LString& Param )
{
	FGUIVisible = !FGUIVisible;
}

void clGUIManager::AddGUIC( const LString& Param )
{
	guard( "%s", Param.c_str() );

	LString FileName = LStr::GetToken( Param, 1 );

	if ( FileName.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: AddGUI <filename>" );
		return;
	}

	if ( !Env->FileSystem->FileExists( FileName ) )
	{
		Env->Console->DisplayError( "GUI file '" + FileName + "' not found" );
		return;
	}

	LoadGUIFile( FileName, NULL );

	unguard();
}

void clGUIManager::SetActivePageC( const LString& Param )
{
	guard();

	LString PageName       = LStr::GetToken( Param, 1 );
	LString TransitionName = LStr::GetToken( Param, 2 );

	if ( PageName.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: SetActivePage <page name> [<transition page>]" );
		return;
	}

	if ( TransitionName.empty() )
	{
		SetActivePageID( PageName );
	}
	else
	{
		SetActivePageTransitionID( TransitionName, PageName );
	}

	unguard();
}

void clGUIManager::AddGUITo( const LString& FileName, iGUIView* To )
{
	LoadGUIFile( FileName, To );
}

void clGUIManager::DeleteGUIFrom( const LString& ViewID, iGUIView* From )
{
	iGUIView* View = From->FindSubViewByID( ViewID );

	if ( View ) { View->GetParentView()->RemoveView( View ); }
}

void clGUIManager::AddGUIToC( const LString& Param )
{
	guard( "%s", Param.c_str() );

	LString ViewID   = LStr::GetToken( Param, 1 );
	LString FileName = LStr::GetToken( Param, 2 );

	if ( FileName.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: AddGUITo <View> <filename>" );
		return;
	}

	iGUIView* TheView = FindViewByID( ViewID );

	LoadGUIFile( FileName, TheView );

	unguard();
}

void clGUIManager::SaveGUIC( const LString& Param )
{
	guard( "%s", Param.c_str() );

	LString FileName = LStr::GetToken( Param, 1 );

	if ( FileName.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: SaveGUI <filename>" );
		return;
	}

	SaveGUIFile( FileName );

	unguard();
}

void clGUIManager::ResetGUIC( const LString& Param )
{
	guard( "%s", Param.c_str() );

	DropActiveViews( FMousePosition );
	FDesktop->RemoveAllChildViews();

	FDockableViewsList.clear();
	FClosingViewsList.clear();
	FGlobalViewsList.clear();

	unguard();
}

void clGUIManager::CloseViewC( const LString& Param )
{
	LString ViewID = LStr::GetToken( Param, 1 );

	if ( ViewID.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: CloseView <view ID>" );
		return;
	}

	LArray<iGUIView*> ViewsToClose;

	FDesktop->FindAllSubViewsByID( ViewID, &ViewsToClose );

	if ( ViewsToClose.empty() )
	{
		Env->Console->DisplayError( "View " + ViewID + " not found" );
		return;
	}

	while ( !ViewsToClose.empty() )
	{
		if ( LStr::GetToken( Param, 2 ) == "DISSOLVE" )
		{
			ViewsToClose.back()->SetRegionState( REGION_DISSOLVING );
		}
		else
		{
			if ( iGUIView* Parent = ViewsToClose.back()->GetParentView() ) { Parent->RemoveView( ViewsToClose.back() ); }
		}

		ViewsToClose.pop_back();
	}

	DropActiveViews( FMousePosition );
}

void clGUIManager::LoadGUIFile( const LString& FileName, iGUIView* To )
{
	guard( "%s", FileName.c_str() );

	std::vector<iGUIView*> ViewsToAdd;

	iIStream* Stream = Env->FileSystem->CreateFileReader( FileName );

	bool GUILoaded = Env->Linker->LoadTypedObjectsListFromStream<iGUIView>( Stream, &ViewsToAdd );

	if ( !GUILoaded )
	{
		Env->Console->DisplayError( "Invalid GUI file: " + FileName );

		return;
	}

	DropActiveViews( FMousePosition );

	( To == NULL ? FDesktop : To )->AddViews( ViewsToAdd );

	// NOTE: this code could produce hidden problems later while binding to GUI
	/*
	   if ( !Env->FileSystem->FileExists(FileName) )
	   {
	      Env->Console->DisplayError( FileName + " does not exist on mounted filesystem" );
	      Env->Logger->Log("Warning! " + FileName + " does not exist on mounted filesystem");

	      return;
	   }
	*/
//   DropActiveViews( FMousePosition );

//   Env->Linker->CreateHierarchyFromFile( FileName, ( To == NULL ? FDesktop : To ), false, NULL );
	/*
	   iObject* Object == NULL;

	   if ( Env->Linker->LoadObjectFromFile( FileName, &Object ) )
	   {
	      (To == NULL ? FDesktop : To)->ddSubObject( Object );
	   }
	*/
	delete( Stream );

	unguard();
}

void clGUIManager::SaveGUIFile( const LString& FileName )
{
	guard( "%s", FileName.c_str() );

	DropActiveViews( FMousePosition );

	Env->Linker->SaveObjectToFile( FileName, FDesktop );

	unguard();
}

void clGUIManager::Event_KEY( LEvent Event, const LEventArgs& Args )
{
	int Key      = static_cast<int>( Args.FFloatArg );
	int KeyState = Args.FBoolArg;

	FRecheckMouse = true;

	if ( Env->GUI->IsTouchScreen() )
	{
		CheckMouse();
//		FMousePosition = Env->Viewport->GetMouseCursorInfo().GetPoint();
	}

	if ( !KeyState )
	{
		if ( Key == LK_LBUTTON )
		{
			// first, let's check if we are dragging something
			DropDragging( FMousePosition );
			// and if we are resizeing something
			DropResizeing( FMousePosition );
		}

		// always send released keys to console
		Env->Console->ExecuteBinding( Key, KeyState );
	}

	iGUIView* DestView = FActiveView;

	// check captured events
	if ( FCapturedView )
	{
		DestView = FCapturedView;
	}

	// try keyboard selection
	if ( !DestView && !FKeyboardSelectableViewsList.empty() )
	{
		if ( Key == LK_UP || Key == LK_DOWN )
		{
			// FIXME: todo
		}
	}

	if ( FActivePage )
	{
		FActivePage->DispatchSpecialKey( Key, KeyState, FMousePosition );
		FActivePage->Event_Key( FActivePage, Key, KeyState );
	}

	if ( DestView && DestView->IsInteractive() )
	{
		if ( DestView->GetParentView() )
		{
			// view received focus
			if ( KeyState )
			{
				if ( !DestView->IsTopmost() &&
				     !DestView->GetParentView()->IsTopmost() ) { SetTopmostView( NULL ); }

				if ( DestView->CanFocus() ) { DestView->GetParentView()->ToFront( DestView ); }
			}
		}

		DestView->DispatchSpecialKey( Key, KeyState, FMousePosition );

		// pass key through, if this View want's to
		FIXME( "implement" )

		if ( DestView->Event_Key( DestView, Key, KeyState )
		     /* !DestView.IsKeyFiltered(Key) */ )
		{
			Env->Console->ExecuteBinding( Key, KeyState );
		}
	}
	else
	{
		Env->Console->ExecuteBinding( Key, KeyState );
	}
}

void clGUIManager::Event_DRAWOVERLAY( LEvent Event, const LEventArgs& Args )
{
	if ( !FGUIVisible )
	{
		return;
	}

	// render
	FDesktop->RenderHierarchy();

	if ( FTopmostView )
	{
		FTopmostView->RenderTopmost();
	}

	RenderToolTip();

	FDesktop->RenderCursor();
}

void clGUIManager::Event_TIMER( LEvent Event, const LEventArgs& Args )
{
	guard( "%f", Args.FFloatArg );

	FMouseIdleTime += Args.FFloatArg;
	FMouseMovementDelay += Args.FFloatArg;

	CheckMouse();

	// ping everything
	for ( clViewsList::const_iterator i = FGlobalViewsList.begin(); i != FGlobalViewsList.end(); ++i )
	{
		if ( !( *i )->IsVisible() ) { continue; }

		if ( ( *i )->IsPage() )
		{
			bool Active = ( *i )->IsPageActive();

			if ( !Active ) { continue; }
		}

		( *i )->Event_Timer( *i, Args.FFloatArg );
	}

	if ( FClosingViewsList.size() > 0 )
	{
		PurgeViews();
	}

	unguard();
}

void clGUIManager::Event_POSTINIT( LEvent Event, const LEventArgs& Args )
{
	guard();

	if ( !Env->Linker->LoadObjectFromFile( "interface/desktop.gui", reinterpret_cast<iObject**>( &FDesktop ) ) )
	{
		FDesktop = Env->Linker->Instantiate( "clGUIDesktop" );
	}

	unguard();
}

void clGUIManager::RequestClose( iGUIView* View )
{
	FClosingViewsList.push_back( View );
}

void clGUIManager::RecheckMouse()
{
	FRecheckMouse = true;
}

void clGUIManager::PurgeViews()
{
	guard();

	DropActiveViews( FMousePosition );

	for ( clViewsList::iterator i = FClosingViewsList.begin(); i != FClosingViewsList.end(); ++i )
	{
		iGUIView* View = ( *i )->GetParentView();

		if ( View ) { View->RemoveView( *i ); }

		delete( *i );
	}

	FClosingViewsList.clear();

	FRecheckMouse = true;

	unguard();
}

void clGUIManager::RenderToolTip() const
{
	if ( !FActiveView )
	{
		return;
	}

	if ( FMouseIdleTime < FActiveView->GetToolTipTime() )
	{
		return;
	}

	if ( FActiveView->GetToolTipText().empty() )
	{
		return;
	}

	Env->Renderer->GetCanvas()->TextStr( FMousePosition.X,
	                                     FMousePosition.Y,
	                                     FActiveView->GetToolTipText(),
	                                     LVector4( 0, 1, 0, 0 ), NULL );
}

void clGUIManager::RegisterView( iGUIView* View )
{
	if ( std::find( FGlobalViewsList.begin(), FGlobalViewsList.end(), View ) == FGlobalViewsList.end() )
	{
		FGlobalViewsList.push_back( View );

		if ( View->IsViewDockable() )
		{
			FDockableViewsList.push_back( View );
		}

		if ( View->IsViewKeyboardSelectable() )
		{
			FKeyboardSelectableViewsList.push_back( View );
		}

		View->Event_Registered( View );
	}
}

void clGUIManager::UnRegisterView( iGUIView* View )
{
	guard();

	Env->Logger->Log( L_PARANOID, "Unreg view: " + LStr::ToStrHex( ( size_t )View ) );

	FGlobalViewsList.remove( View );

	if ( View->IsViewDockable() )
	{
		FDockableViewsList.remove( View );
	}

	if ( View->IsViewKeyboardSelectable() )
	{
		FKeyboardSelectableViewsList.remove( View );
	}

	View->Event_UnRegistered( View );

	unguard();
}

void clGUIManager::CheckMouse()
{
	iGUIView* ViewFound = NULL;

	// update cursor coordinates
	const sMouseCursorInfo& MCI = Env->Viewport->GetMouseCursorInfo();

	FMousePosition = MCI.GetPoint();

	if ( MCI.FMouseMoved || FRecheckMouse && FGUIVisible )
	{
		FRecheckMouse = false;

		// check active page first
		if ( FActivePage )
		{
			ViewFound = FActivePage->FindView( FMousePosition );
		}

		// check topmost view first
		if ( !ViewFound && FTopmostView )
		{
			ViewFound = FTopmostView->FindView( FMousePosition );
		}

		// check everything else
		if ( !ViewFound )
		{
			ViewFound = FDesktop->FindView( FMousePosition );
		}

		// check captured events
		if ( FCapturedView )
		{
			ViewFound = FCapturedView;
		}

		if ( ViewFound && !FViewBeenSized )
		{
			ViewFound->Event_MouseInside( ViewFound, FMousePosition );
		}

		if ( ( FActiveView ) &&
		     ( FActiveView != ViewFound ) )
		{
			FActiveView->Event_MouseOutside( FActiveView, FMousePosition );
		}

		FActiveView = ViewFound;
	}

	if ( MCI.FMouseMoved )
	{
		FMouseIdleTime = 0;

		const float MOUSE_QUANTUM = 0.04f; // sec

		if ( FMouseMovementDelay > MOUSE_QUANTUM )
		{
			FMouseMovementDelay = 0;

			if ( FViewBeenDragged )
			{
				FViewBeenDragged->DragTo( FMousePosition );
			}

			if ( FViewBeenSized )
			{
				FViewBeenSized->ResizeCornerTo( FMousePosition );
			}
		}
	}
}

void clGUIManager::SetTopmostView( iGUIView* View )
{
	if ( FTopmostView ) { FTopmostView->Event_LeaveTopmost( FTopmostView ); }

	FTopmostView = View;

	if ( View ) { View->Event_EnterTopmost( View ); }
}

void clGUIManager::SetTopmostViewID( const LString& ID )
{
	SetTopmostView( FindViewByID( ID ) );
}

void clGUIManager::SetActivePage( clGUIPage* Page )
{
	if ( Page == FActivePage ) { return; }

	SetTopmostView( Page );

	if ( FActivePage )
	{
		FActivePage->Event_PageInactive( FActivePage, Page );
	}

	clGUIPage* PrevPage = FActivePage;

	FActivePage = Page;

	if ( FActivePage )
	{
		FActivePage->Event_PageActive( FActivePage, PrevPage );
	}
}

void clGUIManager::SetActivePageID( const LString& ID )
{
	SetActivePage( FindView<clGUIPage>( ID ) );
}

void clGUIManager::SetActivePageTransition( clGUITransitionPage* TransitionPage, const LString& TargetPageID )
{
	if ( FActivePage && FActivePage->GetObjectID() == TargetPageID ) { return; }

	if ( TransitionPage )
	{
		TransitionPage->FPageToID = TargetPageID;

		SetActivePage( TransitionPage );
	}
	else
	{
		SetActivePageID( TargetPageID );
	}
}

void clGUIManager::SetActivePageTransitionID( const LString& TransitionPageID, const LString& TargetPageID )
{
	SetActivePageTransition( FindView<clGUITransitionPage>( TransitionPageID ), TargetPageID );
}

void clGUIManager::SetCursorForm( LCursorForm CursorForm )
{
	FCursorForm = CursorForm;
}

LCursorForm clGUIManager::GetCursorForm() const
{
	return FCursorForm;
}

void clGUIManager::SetCapture( iGUIView* View )
{
	if ( FCapturedView )
	{
		FCapturedView->Event_CaptureChanged( FCapturedView );
	}

	FCapturedView = View;
}

void clGUIManager::ReleaseCapture()
{
	if ( FCapturedView )
	{
		FCapturedView->Event_CaptureChanged( FCapturedView );
	}

	FCapturedView = NULL;
}

iGUIView* clGUIManager::FindViewByID( const LString& ID ) const
{
	return FDesktop->FindSubViewByID( ID );
}

void clGUIManager::ShowView( const LString& ID, const bool Visible ) const
{
	iGUIView* View = FindViewByID( ID );

	if ( View ) { View->SetVisible( Visible ); }
}

//////////////////////////////////////
//                                  //
//       Look'n'feel tweaking       //
//                                  //
//////////////////////////////////////

#define CVARF( VarName, Value ) VarName = Env->Console->GetVarDefaultFloat( #VarName, (Value) );

void clGUIManager::InitLookAndFeel()
{
	CVARF( DefaultCaptionHeight, 0.0f );
	CVARF( DefaultCaptionLedge, 0.0f );
	CVARF( DefaultCaptionRadius, 0.0f );
	CVARF( DefaultResizeCornerWidth, 0.0f );
	CVARF( DefaultResizeCornerHeight, 0.0f );
	CVARF( DefaultCloseButtonWidth, 0.0f );
	CVARF( DefaultCloseButtonHeight_Delta1, 0.0f );
	CVARF( DefaultCloseButtonHeight_Delta2, 0.0f );
	CVARF( DefaultVerticalTitlePosition, 0.0f );
	CVARF( DefaultButtonBevelRadius, 0.0f );
	CVARF( DefaultPanelBevelRadius, 0.0f );
	CVARF( DefaultDockingDistance, 0.0f );
	CVARF( DefaultWindowBorderWidth, 0.0f );
	CVARF( DefaultWindowBorderHeight, 0.0f );
	CVARF( DefaultButtonVerticalTitleOffset,     -1.9f * GetStringHeight() );
	CVARF( DefaultCheckBoxWidth, 0.0f );
	CVARF( DefaultCheckBoxHorizontalTitleOffset, 25.0f / 1024.0f           );
	CVARF( DefaultCheckBoxHeight_Delta1, 0.0f );
	CVARF( DefaultCheckBoxHeight_Delta2, 0.0f );
	CVARF( DefaultGaugeXOffset, 0.0f );
	CVARF( DefaultGaugeYOffset, 0.0f );
	CVARF( DefaultGaugeTextYOffset, 0.0f );
}

float clGUIManager::GetDefaultCaptionHeight() const
{
	static const float CaptionHeight = GetStringHeight() + 6.0f / 768.0f;

	return CaptionHeight;
}

float clGUIManager::GetDefaultCaptionLedge() const
{
	static const float CaptionLedge = 5.0f / 1024.0f;

	return CaptionLedge;
}

float clGUIManager::GetDefaultCaptionRadius() const
{
	static const float CaptionRadius = 20.0f / 1024.0f;

	return CaptionRadius;
}

float clGUIManager::GetDefaultResizeCornerWidth() const
{
	static const float ResizeCornerWidth = 16.0f / 1024.0f;

	return ResizeCornerWidth;
}

float clGUIManager::GetDefaultResizeCornerHeight() const
{
	static const float ResizeCornerHeight = 16.0f / 768.0f;

	return ResizeCornerHeight;
}

float clGUIManager::GetDefaultCloseButtonWidth() const
{
	static const float CloseButtonWidth = 16.0f / 1024.0f;

	return CloseButtonWidth;
}

float clGUIManager::GetDefaultCloseButtonHeight_Delta1() const
{
	static const float HDelta1 = 2.0f / 768.0f;

	return HDelta1;
}

float clGUIManager::GetDefaultCloseButtonHeight_Delta2() const
{
	static const float HDelta2 = GetDefaultCloseButtonHeight_Delta1() + 16.0f / 768.0f;

	return HDelta2;
}

float clGUIManager::GetDefaultVerticalTitlePosition() const
{
	static const float TitlePosition = - 24.0f / 768.0f;

	return TitlePosition;
}

float clGUIManager::GetDefaultButtonBevelRadius() const
{
	static const float BevelRadius = 0.2f * GetDefaultCaptionRadius();

	return BevelRadius;
}

float clGUIManager::GetDefaultPanelBevelRadius() const
{
	static const float BevelRadius = 4.0f / 1024.0f;

	return BevelRadius;
}

float clGUIManager::GetDefaultDockingDistance() const
{
	static const float DockingDistance = 7.0f / 768.0f;

	return DockingDistance;
}

float clGUIManager::GetDefaultWindowBorderWidth() const
{
	static const float BorderWidth = 8.0f / 1024.0f;

	return BorderWidth;
}

float clGUIManager::GetDefaultWindowBorderHeight() const
{
	static const float BorderHeight = 8.0f / 768.0f;

	return BorderHeight;
}

float clGUIManager::GetDefaultButtonVerticalTitleOffset() const
{
	return DefaultButtonVerticalTitleOffset->GetFloat();
}

float clGUIManager::GetDefaultCheckBoxWidth() const
{
	static const float CheckBoxWidth = 16.0f / 1024.0f;

	return CheckBoxWidth;
}

float clGUIManager::GetDefaultCheckBoxHorizontalTitleOffset() const
{
	return DefaultCheckBoxHorizontalTitleOffset->GetFloat();
}

float clGUIManager::GetDefaultCheckBoxHeight_Delta1() const
{
	static const float HDelta1 = GetStringHeight() - 16.0f / 768.0f - 2.0f / 768.0f;

	return HDelta1;
}

float clGUIManager::GetDefaultCheckBoxHeight_Delta2() const
{
	static const float HDelta2 = GetStringHeight() - 2.0f / 768.0f;

	return HDelta2;
}

float clGUIManager::GetDefaultGaugeXOffset() const
{
	static const float OffsetX = 1.3f * GetDefaultPanelBevelRadius();

	return OffsetX;
}

float clGUIManager::GetDefaultGaugeYOffset() const
{
	static const float OffsetY = 1.1f * GetDefaultPanelBevelRadius();

	return OffsetY;
}

float clGUIManager::GetDefaultGaugeTextYOffset() const
{
	return -0.025f;
}

float clGUIManager::GetStringWidth( const LString& Str, iFontProperties* Font ) const
{
	/*
	  Transform string width to normalized device coordinates (0..1)
	*/

// static clCVar* FontW = Env->Console->GetVar( "Font.Width" );
//	return 0.5f * Str.length() * FontW->GetFloat();
	if ( Font )
	{
		if ( Font->IsFixedFont() )
		{
			return 0.5f * Str.length() * dynamic_cast<clFixedFontProperties*>( Font )->FFontWidth;
		}
		else
		{
			// TODO: implement for FreeTypeFont
			return 0;
		}
	}

	return 0.5f * Str.length() * Env->Renderer->GetCanvas()->GetDefaultFont()->FFontWidth;
}

float clGUIManager::GetStringHeight() const
{
	/*
	  Transform font height to normalized device coordinates (0..1)
	*/
	static const float NDC_COEFF = 1.0f / 768.0f;
	static const Luint FontHeight = 16;

	return FontHeight * NDC_COEFF;
}

static size_t ScanFromRight( const LString Str, size_t Pos, char Ch )
{
	if ( Str.empty() )
	{
		return 0;
	}

	size_t CurrentPos = ( Pos >= Str.length() ) ? Str.length() - 1 : Pos;

	if ( CurrentPos > 1 && Str[ CurrentPos-1 ] == Ch )
	{
		while ( CurrentPos > 1 && Str[ CurrentPos-1 ] == Ch )
		{
			CurrentPos--;
		}
	}

	while ( CurrentPos > 1 && Str[ CurrentPos-1 ] != Ch )
	{
		CurrentPos--;
	}

	if ( CurrentPos == 1 && Str[0] != Ch )
	{
		CurrentPos = 0;
	}

	return CurrentPos;
}

static size_t ScanFromLeft( const LString Str, size_t Pos, char Ch )
{
	if ( Str.empty() )
	{
		return 0;
	}

	size_t Len = Str.length() - 1;

	size_t CurrentPos = ( Pos > Len ) ? Len : Pos;

	while ( CurrentPos < Len && Str[ CurrentPos ] != Ch )
	{
		CurrentPos++;
	}

	if ( CurrentPos < Len && Str[ CurrentPos ] == Ch )
	{
		while ( CurrentPos < Len && Str[ CurrentPos ] == Ch )
		{
			CurrentPos++;
		}
	}

	if ( CurrentPos == Len && Str[Len] != Ch )
	{
		CurrentPos = Len + 1;
	}

	return CurrentPos;
}

static void DeleteNextWord( LString& Str, size_t& Pos )
{
	size_t NewCursorPos = ScanFromLeft( Str, Pos, ' ' );

	if ( NewCursorPos > Pos )
	{
		size_t DeletedLen = NewCursorPos - Pos;

		Str.erase( Pos, DeletedLen );

		Pos = NewCursorPos - DeletedLen;
	}
}

static void DeletePrevWord( LString& Str, size_t& Pos )
{
	size_t NewCursorPos = ScanFromRight( Str, Pos, ' ' );

	if ( NewCursorPos < Pos )
	{
		Str.erase( NewCursorPos, Pos - NewCursorPos );

		Pos = NewCursorPos;
	}
}

static void PasteText( LString& Str, const LString& ClipText, size_t& Pos, float MaxScrLength )
{
	Str.insert( Pos, ClipText );
	Pos += ClipText.length();
}

void clGUIManager::ProcessInputLineKey( const int Key, LString& TextBuffer, size_t& CursorPos, float MaxScrLength ) const
{
	switch ( Key )
	{
		case LK_ESCAPE:

			if ( !TextBuffer.empty() )
			{
				TextBuffer.clear();
				CursorPos = 0;
			};

			break;

		case LK_HOME:
			CursorPos = 0;

			break;

		case LK_END:
			CursorPos = TextBuffer.length();

			break;

		case LK_BACK:
			if ( CursorPos > 0 )
			{
				if ( Env->Console->IsKeyPressed( LK_CONTROL ) )
				{
					DeletePrevWord( TextBuffer, CursorPos );
				}
				else
				{
					TextBuffer.erase( CursorPos - 1, 1 );
					CursorPos--;
				}
			}

			break;
		case LK_DELETE:

			if ( Env->Console->IsKeyPressed( LK_CONTROL ) )
			{
				DeleteNextWord( TextBuffer, CursorPos );
			}
			else
			{
				if ( CursorPos < static_cast<int>( TextBuffer.length() ) )
				{
					TextBuffer.erase( CursorPos, 1 );
				}
			}

			break;
		case LK_LEFT:

			if ( Env->Console->IsKeyPressed( LK_CONTROL ) )
			{
				CursorPos = ScanFromRight( TextBuffer, CursorPos, ' ' );
			}
			else if ( CursorPos > 0 )
			{
				CursorPos--;
			}

			break;
		case LK_RIGHT:

			if ( Env->Console->IsKeyPressed( LK_CONTROL ) )
			{
				CursorPos = ScanFromLeft( TextBuffer, CursorPos, ' ' );
			}
			else if ( CursorPos < static_cast<int>( TextBuffer.length() ) )
			{
				CursorPos++;
			}

			break;
		case LK_UP:
			TextBuffer = Env->Console->GetCommandsHistory()->MoveBack();
			CursorPos = TextBuffer.length();
			break;
		case LK_DOWN:
			TextBuffer = Env->Console->GetCommandsHistory()->MoveForward();
			CursorPos = TextBuffer.length();
			break;
			// Ctrl+C, Ctrl+Ins
		case LK_V:

			if ( Env->Console->IsKeyPressed( LK_CONTROL ) )
			{
				PasteText( TextBuffer, Env->GetClipboardText(), CursorPos, MaxScrLength );
			}

			break;
		case LK_C:

			if ( Env->Console->IsKeyPressed( LK_CONTROL ) )
			{
				Env->SetClipboardText( TextBuffer );
			}

			break;
		case LK_INSERT:

			if ( Env->Console->IsKeyPressed( LK_SHIFT  ) )
			{
				PasteText( TextBuffer, Env->GetClipboardText(), CursorPos, MaxScrLength );
			}
			else if ( Env->Console->IsKeyPressed( LK_CONTROL ) )
			{
				Env->SetClipboardText( TextBuffer );
			}

			break;
	}

	while ( Env->GUI->GetStringWidth( TextBuffer, NULL ) >= MaxScrLength )
	{
		LStr::pop_back( &TextBuffer );
	}

	if ( CursorPos > TextBuffer.length() )
	{
		CursorPos = TextBuffer.length();
	}
}

////// Utility functions for quick'n'dirty binding of handlers/editors to the internal state

/// Of course, .NET's property grid replaces this stuff, but anyways it is nice

clGUIPushButton* clGUIManager::AddButtonCmd( const LVector2& Pos, const LVector2& Size, const LString& Title, const LString& Command )
{
	clGUIPushButton* Button = Env->Linker->Instantiate( "clGUIPushButton" );

	Button->SetTitle( Title );
	Button->SetCoords( Pos.X, Pos.Y, Size.X, Size.Y );
	Button->SetVisible( true );
	Button->BindConsoleCommandS( Command );

	GetDesktop()->AddView( Button );

	return Button;
}

/// shit, I know...
int Counter_Callback = 0;

clGUIPushButton* clGUIManager::AddButtonCallback( const LVector2& Pos, const LVector2& Size, const LString& Title, clConsole::clConsoleProc Callback )
{
	LString CmdName = "UNIVERSALBUTTONCALLBACKCMD_" + LStr::ToStr( Counter_Callback++ );

	Env->Console->RegisterCommand( CmdName, Callback );

	return AddButtonCmd( Pos, Size, Title, CmdName );
}

clGUISlider* clGUIManager::AddSlider( const LVector2& Pos, const LVector2& Size, float Min, float Max, const LString& ToolTip, const LString& VarName )
{
	clGUISlider* Slider = Env->Linker->Instantiate( "clGUISlider" );

	clCVar* TheVar = Env->Console->GetVar( VarName );
	float InitVal = TheVar->GetFloat();

	Slider->SetCoords( Pos.X, Pos.Y, Size.X, Size.Y );
	Slider->FMinValue = Min;
	Slider->FMaxValue = Max;
	Slider->SetToolTipText( ToolTip );
	Slider->BindConsoleVariableS ( VarName );
	Slider->SetVisible( true );

	GetDesktop()->AddView( Slider );

	Slider->SetCurrentValue( InitVal );
	// Restore variable's value. It is broken occasionaly
	TheVar->SetFloat( InitVal );

	return Slider;
}

clGUIStaticWidget* clGUIManager::AddStatic( const LVector2& Pos, const LVector2& Size, const LString& VarName )
{
	clGUIStaticWidget* Static = Env->Linker->Instantiate( "clGUIStaticWidget" );

	Static->SetCoords( Pos.X, Pos.Y, Size.X, Size.Y );
	Static->BindConsoleVariableS ( VarName );
	Static->SetVisible( true );

	GetDesktop()->AddView( Static );

	return Static;
}

clGUIInputLine* clGUIManager::AddEditBox( const LVector2& Pos, const LVector2& Size, const LString& VarName )
{
	clGUIInputLine* InputLine = Env->Linker->Instantiate( "clGUIInputLine" );

	clCVar* TheVar = Env->Console->GetVar( VarName );
	LString InitVal = TheVar->GetString();

	InputLine->SetCoords( Pos.X, Pos.Y, Size.X, Size.Y );
	InputLine->BindConsoleVariableS ( VarName );
	InputLine->SetVisible( true );

	GetDesktop()->AddView( InputLine );

	InputLine->SetTitle( InitVal );
	// Restore variable's value. It is broken occasionaly
	TheVar->SetString( InitVal );

	return InputLine;
}

clGUICheckBox* clGUIManager::AddCheckBox( const LVector2& Pos, const LVector2& Size, const LString& Title, const LString& VarName )
{
	clGUICheckBox* Check = Env->Linker->Instantiate( "clGUICheckBox" );

	clCVar* TheVar = Env->Console->GetVar( VarName );
	bool InitVal = TheVar->GetBool();

	Check->SetCoords( Pos.X, Pos.Y, Size.X, Size.Y );
	Check->SetTitle( Title );
	Check->BindConsoleVariableS ( VarName );
	Check->SetVisible( true );

	GetDesktop()->AddView( Check );

	InitVal ? Check->Check() : Check->UnCheck();
	// Restore variable's value. It is broken occasionaly
	TheVar->SetBool( InitVal );

	return Check;
}

clGUIStaticWidget* clGUIManager::AddPropertyStatic ( const LVector2& Pos, const LVector2& Size, iObject* Obj, const LString& PropName )
{
	LString IntVarName = "INTVARNAME_BINDER_" + LStr::ToStr( Counter_Callback++ );

	Obj->BindPropertyToCVar( PropName, IntVarName );

	return AddStatic( Pos, Size, IntVarName );
}

clGUISlider* clGUIManager::AddPropertySlider( const LVector2& Pos, const LVector2& Size, float Min, float Max, const LString& ToolTip, iObject* Obj, const LString& PropName )
{
	LString IntVarName = "INTVARNAME_BINDER_" + LStr::ToStr( Counter_Callback++ );

	Obj->BindPropertyToCVar( PropName, IntVarName );

	return AddSlider( Pos, Size, Min, Max, ToolTip, IntVarName );
}

clGUIInputLine* clGUIManager::AddPropertyEdit( const LVector2& Pos, const LVector2& Size, iObject* Obj, const LString& PropName )
{
	LString IntVarName = "INTVARNAME_BINDER_" + LStr::ToStr( Counter_Callback++ );

	Obj->BindPropertyToCVar( PropName, IntVarName );

	return AddEditBox( Pos, Size, IntVarName );
}

clGUICheckBox* clGUIManager::AddPropertyCheck( const LVector2& Pos, const LVector2& Size, const LString& Title, iObject* Obj, const LString& PropName )
{
	LString IntVarName = "INTVARNAME_BINDER_" + LStr::ToStr( Counter_Callback++ );

	Obj->BindPropertyToCVar( PropName, IntVarName );

	return AddCheckBox( Pos, Size, Title, IntVarName );
}

clGUIColorDial* clGUIManager::AddPropColorDial ( const LVector2& Pos, const LVector2& Size, iObject* Obj, const LString& PropName )
{
	LString IntVarName = "INTVARNAME_BINDER_" + LStr::ToStr( Counter_Callback++ );

	Obj->BindPropertyToCVar( PropName, IntVarName );

	return AddColorDial( Pos, Size, IntVarName );
}

/// Add a full-screen graph attached to the variable
clGUIFullScreenGraph* clGUIManager::AddFSGraph( const LString& InVarName, const LString& VisibleVarName )
{
	clGUIFullScreenGraph* Graph = Env->Linker->Instantiate( "clGUIFullScreenGraph" );

	Graph->FReadVarName = InVarName;
	Graph->BindConsoleVariableS( VisibleVarName );
	Graph->SetRegionState( REGION_MAXIMIZED );
	Graph->SetVisible( true );
	Graph->SetCoords( 0.f, 0.f, 1.f, 1.f );

	GetDesktop()->AddView( Graph );

	return Graph;
}

clGUIColorDial* clGUIManager::AddColorDial( const LVector2& Pos, const LVector2& Size, const LString& OutVarName )
{
	clGUIColorDial* Panel = Env->Linker->Instantiate( "clGUIColorDial" );

	Panel->FOutVarName = OutVarName;
	Panel->SetRegionState( REGION_MINIMIZED );
	Panel->SetVisible( true );
	Panel->SetCoords( Pos.X, Pos.Y, Size.X, Size.Y );

	GetDesktop()->AddView( Panel );

	return Panel;
}

clGUIMtxView* clGUIManager::AddMtxView( const LVector2& Pos, const LVector2& Size, const LString& InVarName )
{
	clGUIMtxView* Panel = Env->Linker->Instantiate( "clGUIMtxView" );

	Panel->FInVariable = InVarName;
	Panel->SetRegionState( REGION_MINIMIZED );
	Panel->SetVisible( true );
	Panel->SetCoords( Pos.X, Pos.Y, Size.X, Size.Y );

	GetDesktop()->AddView( Panel );

	return Panel;
}

clGUISceneView* clGUIManager::AddSceneView( const LVector2& Pos, const LVector2& Size, const LMatrix4& Proj, const LMatrix4& View, clScene* Scene )
{
	clGUISceneView* Panel = Env->Linker->Instantiate( "clGUISceneView" );

	Panel->FProjection = Proj;
	Panel->FView = View;
	Panel->FScene = Scene;

	Panel->SetRegionState( REGION_MINIMIZED );
	Panel->SetVisible( true );
	Panel->SetCoords( Pos.X, Pos.Y, Size.X, Size.Y );

	GetDesktop()->AddView( Panel );

	return Panel;
}

clGUIVisualSceneView* clGUIManager::AddVisualSceneView( const LVector2& Pos, const LVector2& Size, clVisualScene* Sc, clVisualProjector* Prj, clRenderingTechnique* RT )
{
	clGUIVisualSceneView* Panel = Env->Linker->Instantiate( "clGUIVisualSceneView" );

	Panel->SetProjector( Prj );
	Panel->SetScene( Sc );
	Panel->SetRenderTechnique( RT );

	Panel->SetRegionState( REGION_MINIMIZED );
	Panel->SetVisible( true );
	Panel->SetCoords( Pos.X, Pos.Y, Size.X, Size.Y );

	GetDesktop()->AddView( Panel );

	return Panel;
}

/*
 * 09/01/2011
     Utility constructors. At last.
 * 09/07/2010
     Shift+Ins, Ctrl+C, Ctrl+V in ProcessInputLineKey()
 * 06/07/2010
     Improved Ctrl+Del, Ctrl+BackSpace in ProcessInputLineKey()
 * 05/07/2010
     ProcessInputLineKey()
 * 23/06/2007
     OpenContextMenu()
 * 22/06/2007
     ToggleGUIC()
 * 05/04/2007
     SaveGUIC()
 * 14/01/2007
     Activate()
     Deactivate()
 * 06/01/2007
     CloseViewC()
 * 14/11/2006
     Tool tips
     Mouse movement events are sent to view that has current capture
 * 06/05/2005
     ShowView()
 * 09/04/2005
     FindViewByID()
 * 04/03/2005
     Code clean-up in SendKey()
 * 11/02/2005
     Unique mask IDs generation for views
 * 09/02/2005
     Fixes in SendKey()
 * 03/02/2005
     Work in progress...
 * 27/01/2005
     AddGUIC()
 * 26/01/2005
     It's here
*/
