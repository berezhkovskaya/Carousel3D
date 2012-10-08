/**
 * \file iGUIView.cpp
 * \brief GUI view
 * \version 0.6.06
 * \date 21/12/2012
 * \author Sergey Kosarevsky, 2005-2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "iGUIView.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/Logger.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "GUI/GUIManager.h"
#include "Utils/Viewport.h"
#include "Renderer/iRenderContext.h"
#include "Math/LMathStrings.h"
#include "LKeys.h"
#include "LColors.h"

#include <algorithm>

iGUIView::iGUIView():
	FDestructorCalled( false ),
	FParentView( NULL ),
	FMouseOver( false ),
	FBeenDragged( false ),
	FBeenSized( false ),
	FDragDelta(),
	FMousePosLocal(),
	FDefaultColor( LC_White ),
	FToolTipTime( 0.2f ),
	FToolTipText( "" ),
	FConsoleVariable( NULL ),
	FVisible( true ),
	FInteractive( true ),
	FOnTopPriority( 0 ),
	FChildViews( 0 ),
	FFontProperties( NULL ),
	FKeyboardSelectable( false ),
	//
	FMinimalOpacity( 1.0f ),
	FMaximalOpacity( 1.0f ),
	FOpacityDeltaPlus( 0.3f ),
	FOpacityDeltaMinus( 0.15f ),
	FOpacityDeltaDissolve( 3.0f )
{
}

iGUIView::~iGUIView()
{
	if ( FFontProperties ) { delete( FFontProperties ); }

	Env->Logger->Log( L_PARANOID, "Deleting view: " + LStr::ToStrHex( ( size_t )this ) );

	FDestructorCalled = true;

	if ( FParentView )
	{
		if ( !FParentView->IsDestructorCalled() ) { FParentView->RemoveView( this ); }

		FParentView = NULL;
	}

	Env->GUI->UnRegisterView( this );

	RemoveAllChildViews();
}

void iGUIView::SetDefaultColorHex( const LString& Color )
{
	FDefaultColor = LColors::ToColor( LStr::StrToHex(Color) ); 
}

LString iGUIView::GetDefaultColorHex() const
{
	return LStr::Vec4ToStr( FDefaultColor );
}

bool iGUIView::EndLoad()
{
	Env->GUI->RegisterView( this );

	RefreshParents();

	// precenter all child views
	for ( size_t i = 0; i != GetTotalSubViews(); i++ )
	{
		iGUIView* View = GetSubView( i );

		if ( !View->IsInLocalCoords() ) { continue; }

		View->Precenter( this );

		LVector2 Pnt = LVector2( View->GetX1(), View->GetY1() );

		Pnt = Local2Screen( Pnt );

		View->MoveTo( Pnt );
	}

	if ( GetTotalSubViews() > 0 )
	{
		// put last to the front
		ToFront( GetSubView( GetTotalSubViews() - 1 ) );
	}

	return iGUIRegion::EndLoad();
}


void iGUIView::RefreshParents()
{
	for ( size_t i = 0; i != GetTotalSubViews(); i++ )
	{
		iGUIView* View = GetSubView( i );
		View->SetParentView( this );
		View->RefreshParents();
	}
}

iFontProperties* iGUIView::GetFont() const
{
	if ( FFontProperties )
	{
		return FFontProperties;
	}

	if ( GetParentView() )
	{
		return GetParentView()->GetFont();
	}

	return NULL;
}

void iGUIView::RemoveAllChildViews()
{
	// copy childs list because views detach themself from parent on deletion
	clViewsList Childs = FChildViews;

	Utils::DeallocateAll( Childs );

	// clear the original list (should be empty by now)
	FChildViews.clear();
}

LString iGUIView::GetConsoleVariableName() const
{
	return ( FConsoleVariable ) ? FConsoleVariable->GetObjectID() : "";
}

void iGUIView::SetOnTopPriority( int OnTop )
{
	FOnTopPriority = OnTop;
}

int iGUIView::GetOnTopPriority() const
{
	return FOnTopPriority;
}

float iGUIView::GetToolTipTime() const
{
	return FToolTipTime;
}

LString iGUIView::GetToolTipText() const
{
	return FToolTipText;
}

void iGUIView::ExecuteCommandsStack()
{
	for ( std::vector<LString>::const_iterator i = FCommandsStack.begin(); i != FCommandsStack.end(); ++i )
	{
		Env->Console->QueryCommand( *i );
	}
}

void iGUIView::ExecuteCommandsStackFadeIn()
{
	for ( std::vector<LString>::const_iterator i = FCommandsStackFadeIn.begin(); i != FCommandsStackFadeIn.end(); ++i )
	{
		Env->Console->QueryCommand( *i );
	}
}

void iGUIView::Event_Minimized( iGUIResponder* Source )
{
	iGUIRegion::Event_Minimized( Source );

	SetRegionState( REGION_MINIMIZED );
}

void iGUIView::Event_Maximized( iGUIResponder* Source )
{
	iGUIRegion::Event_Maximized( Source );

	SetRegionState( REGION_MAXIMIZED );
}

void iGUIView::Event_Shrinked( iGUIResponder* Source )
{
	iGUIRegion::Event_Shrinked( Source );

	SetRegionState( REGION_SHRINKED );
}

void iGUIView::Event_Dissolved( iGUIResponder* Source )
{
	iGUIRegion::Event_Dissolved( Source );

	SetVisible( false );
	SetInteractive( false );

	SetRegionState( REGION_DISSOLVED );
}

void iGUIView::Event_MouseInside( iGUIResponder* Source, const LVector2& Pnt )
{
	iGUIRegion::Event_MouseInside( Source, Pnt );

	FMousePosLocal = Screen2Local( Pnt );
	FMouseOver     = true;
}

void iGUIView::Event_MouseOutside( iGUIResponder* Source, const LVector2& Pnt )
{
	iGUIRegion::Event_MouseOutside( Source, Pnt );

	FMousePosLocal = Screen2Local( Pnt );
	FMouseOver     = false;
}

void iGUIView::Event_MouseRight( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState )
{
	iGUIRegion::Event_MouseRight( Source, Pnt, KeyState );
}

void iGUIView::Event_Timer( iGUIResponder* Source, float DeltaTime )
{
	using std::min;

	static const float FontHeight   = Env->GUI->GetStringHeight();
	static const float TIME_QUANTUM = Env->GetTimeQuantum();
	static const float Speed        = min( TIME_QUANTUM / DeltaTime, 0.99f );

	switch ( GetRegionState() )
	{
		case REGION_DISSOLVED:
			Env->GUI->RequestClose( this );
			break;

		case REGION_SHRINKED:
			ExecuteCommandsStack();
			Env->GUI->RequestClose( this );
			break;

		case REGION_DISSOLVING:

			if ( GetOpacity() > 0.0f )
			{
				SetOpacity( GetOpacity() - DeltaTime * FOpacityDeltaDissolve );
			}
			else
			{
				Event_Dissolved( Source );
			}

			break;

		case REGION_SHRINKING:

			if ( GetWidth() > FontHeight )
			{
				SetWidth( Speed * 0.75f * GetWidth() );
			}
			else
			{
				SetWidth( FontHeight );
				Event_Shrinked( Source );
			}

			break;

		case REGION_MINIMIZING:

			if ( GetHeight() > Env->GUI->GetDefaultCaptionHeight() )
			{
				SetHeight( Speed * 0.75f * GetHeight() );
			}
			else
			{
				SetHeight( Env->GUI->GetDefaultCaptionHeight() );
				Event_Minimized( Source );
			}

			break;

		case REGION_MAXIMIZING:

			if ( GetHeight() < GetMaximizedHeight() )
			{
				SetHeight( Speed * 0.75f * GetHeight() + Speed * 0.25f * GetMaximizedHeight() + Speed * 0.01f );
			}
			else
			{
				SetHeight( GetMaximizedHeight() );
				Event_Maximized( Source );
			}

			break;
	}

	switch ( GetRegionState() )
	{
		case REGION_MINIMIZING:
		case REGION_MAXIMIZING:
		case REGION_SHRINKING:
			Env->GUI->RecheckMouse();
	}

	if ( GetRegionState() != REGION_DISSOLVING )
	{
		const sMouseCursorInfo MCI = Env->Viewport->GetMouseCursorInfo();

		bool MouseOver = Contains( LVector2( MCI.FMouseX, MCI.FMouseY ) );

		SetOpacity( GetOpacity() + DeltaTime * ( MouseOver ? FOpacityDeltaPlus : -FOpacityDeltaMinus ) );

		if ( GetOpacity() > FMaximalOpacity ) { SetOpacity( FMaximalOpacity ); ExecuteCommandsStackFadeIn(); }

		if ( GetOpacity() < FMinimalOpacity ) { SetOpacity( FMinimalOpacity ); }
	}

	iGUIRegion::Event_Timer( Source, DeltaTime );
}

void iGUIView::Event_StartDragging( iGUIResponder* Source, const LVector2& Pnt )
{
	iGUIRegion::Event_StartDragging( Source, Pnt );

	FBeenDragged = true;

	FDragDelta = LVector2( GetX1(), GetY1() ) - Pnt;
}

void iGUIView::Event_StopDragging( iGUIResponder* Source, const LVector2& Pnt )
{
	iGUIRegion::Event_StopDragging( Source, Pnt );

	FBeenDragged = false;
}

void iGUIView::Event_StartResize( iGUIResponder* Source, const LVector2& Pnt )
{
	iGUIRegion::Event_StartResize( Source, Pnt );

	FBeenSized = true;

	FDragDelta = LVector2( GetX2(), GetY2() ) - Pnt;
}

void iGUIView::Event_StopResize( iGUIResponder* Source, const LVector2& Pnt )
{
	iGUIRegion::Event_StopResize( Source, Pnt );

	FBeenSized = false;
}

void iGUIView::DragTo( const LVector2& Pnt )
{
	LVector2 Coord = Pnt + FDragDelta;

	if ( IsViewDockable() )
	{
		ProcessDocking( DOCKING_MOVE, &Coord.X, &Coord.Y );
	}

	MoveTo( Coord );
}

void iGUIView::ResizeCornerTo( const LVector2& Pnt )
{
	float SizeX = Pnt.X + FDragDelta.X - GetX1();
	float SizeY = Pnt.Y + FDragDelta.Y - GetY1();

	if ( IsViewDockable() )
	{
		ProcessDocking( DOCKING_RESIZE, &SizeX, &SizeY );
	}

	SetSize( SizeX, SizeY );
}

void iGUIView::ProcessDocking( LDockingType DockingType,
                               float* CoordX, float* CoordY )
{
	clGUIManager::clViewsList* List = Env->GUI->GetDockableViewsList();

	for ( clGUIManager::clViewsList::const_iterator i = List->begin(); i != List->end(); ++i )
	{
		iGUIView* View = ( *i );

		if ( View != this )
		{
			switch ( DockingType )
			{
				case DOCKING_MOVE:
					View->DockCoordsToThisRegion( CoordX, CoordY, GetWidth(), GetHeight() );
					break;
				case DOCKING_RESIZE:
					View->DockSizeToThisRegion( GetX1(), GetY1(), CoordX, CoordY );
					break;
				default:
					FATAL_MSG( "Invalid docking type" );
			}
		}
	}
}

void iGUIView::MoveRel( const LVector2& Delta )
{
	iGUIRegion::MoveRel( Delta );

	// move all childs
	for ( size_t i = 0; i != GetTotalSubViews(); ++i )
	{
		GetSubView( i )->MoveRel( Delta );
	}
}

void iGUIView::SetSize( const float Width, const float Height )
{
	using std::max;

	float NewWidth  = max( SuggestMinimalWidth(),  Width  );
	float NewHeight = max( SuggestMinimalHeight(), Height );

	iGUIRegion::SetSize( NewWidth, NewHeight );
}

float iGUIView::SuggestMinimalWidth() const
{
	return Env->GUI->GetDefaultResizeCornerWidth();
}

float iGUIView::SuggestMinimalHeight() const
{
	return Env->GUI->GetDefaultResizeCornerHeight();
}

void iGUIView::SetRegionState( LRegionState State )
{
	iGUIRegion::SetRegionState( State );

	if ( State == REGION_DISSOLVING || State == REGION_DISSOLVED )
	{
		SetInteractive( false );

		for ( size_t i = 0; i != GetTotalSubViews(); i++ )
		{
			FChildViews[i]->SetRegionState( State );
		}

		Env->GUI->RecheckMouse();
	}
}

bool iGUIView::IsMinimized() const
{
	return GetRegionState() == REGION_MINIMIZED;
}

bool iGUIView::IsMaximized() const
{
	return GetRegionState() == REGION_MAXIMIZED;
}

bool iGUIView::IsTopmost() const
{
	return Env->GUI->IsViewTopmost( this );
}

bool iGUIView::IsPageActive() const
{
	return false;
}

void iGUIView::Minimize()
{
	IsViewSizeable() ? SetRegionState( REGION_MINIMIZING ) : Event_Minimized( this );
}

void iGUIView::Maximize()
{
	IsViewSizeable() ? SetRegionState( REGION_MAXIMIZING ) : Event_Maximized( this );
}

void iGUIView::DispatchSpecialKey( const int Key,
                                   const bool KeyState,
                                   const LVector2& Pnt )
{
	switch ( Key )
	{
		case LK_RBUTTONDB:
			Event_RDoubleClick( this, Pnt );
			break;
		case LK_LBUTTONDB:
			Event_LDoubleClick( this, Pnt );
			break;
		case LK_LBUTTON:

			if ( KeyState )
			{
				if ( IsViewSizeableAt( Pnt ) )
				{
					Env->GUI->Callback_SizeView( this );
					Event_StartResize( this, Pnt );
				}
				else if ( IsViewDraggableAt( Pnt ) )
				{
					Env->GUI->Callback_DragView( this );
					Event_StartDragging( this, Pnt );
				}
				else
				{
					Event_MouseLeft( this, Pnt, KeyState );
				}
			}
			else
			{
				Event_MouseLeft( this, Pnt, KeyState );
			}

			break;
		case LK_RBUTTON:
			Event_MouseRight( this, Pnt, KeyState );
			break;
	}
}

void iGUIView::RenderHierarchy()
{
	if ( IsTopmost() ) { return; }

	RenderTopmost();
}

void iGUIView::RenderTopmost()
{
	if ( !IsVisible() )
	{
		return;
	}

	PreRender();

	for ( size_t i = 0; i != GetTotalSubViews(); ++i )
	{
		GetSubView( i )->RenderHierarchy();
	}

	PostRender();
}

iGUIView* iGUIView::FindView( const LVector2& ScreenPnt )
{
	guard();

	if ( !IsVisible() ) { return NULL; }

	if ( !Contains( ScreenPnt ) ) { return NULL; }

	// allow content-independent parrent dragging
	if ( IsViewDraggable() && ContainsCaption( ScreenPnt ) ) { return this; }

	// allow content-independent resizing
	if ( IsViewSizeable()  && ContainsResizeCorner( ScreenPnt ) ) { return this; }

	iGUIView* Found = NULL;

	for ( size_t i = 0; i != GetTotalSubViews(); ++i )
	{
		iGUIView* SubView = GetSubView( i );

		if ( SubView->IsVisible() )
		{
			if ( SubView->Contains( ScreenPnt ) )
			{
				Found = SubView->FindView( ScreenPnt );
			}
		}
	}

	return Found ? Found : this;

	unguard();
}

void iGUIView::ToFront( iGUIView* View )
{
	guard();

	clViewsList::iterator i = std::find( FChildViews.begin(), FChildViews.end(), View );

	if ( i == FChildViews.end() )
	{
		ERROR_MSG( "Unable to find View in sub objects" );

		return;
	}

	iGUIView* View = dynamic_cast<iGUIView*>( *i );

	FChildViews.erase( i );

	// insert, preserving "on top" priority
	clViewsList::iterator InsertPos;

	for ( InsertPos = FChildViews.begin(); InsertPos != FChildViews.end(); ++InsertPos )
	{
		if ( ( *InsertPos )->GetOnTopPriority() > View->GetOnTopPriority() ) { break; }
	}

	FChildViews.insert( InsertPos, View );

	// traverse up to root
	iGUIView* ParentView = GetParentView();

	if ( ParentView ) { ParentView->ToFront( this ); }

	unguard();
}

void iGUIView::AddView( iGUIView* View )
{
	if ( std::find( FChildViews.begin(), FChildViews.end(), View ) == FChildViews.end() )
	{
		FChildViews.push_back( View );
	}

	Env->GUI->RegisterView( View );

	View->SetParentView( this );
	View->Precenter( this );

	LVector2 Pnt = LVector2( View->GetX1(), View->GetY1() );

	Pnt = Local2Screen( Pnt );

	View->MoveTo( Pnt );

//   ToFront( View );
}

void iGUIView::AddViews( const clViewsList& Views )
{
	for ( size_t i = 0; i != Views.size(); i++ )
	{
		AddView( Views[i] );
	}
}

void iGUIView::RemoveView( iGUIView* View )
{
	if ( !View->IsDestructorCalled() ) { Env->GUI->UnRegisterView( View ); }

	std::vector<iGUIView*>::iterator i = std::find( FChildViews.begin(), FChildViews.end(), View );

	if ( i != FChildViews.end() ) { FChildViews.erase( i ); }
}

iGUIView* iGUIView::FindSubViewByID( const LString& ID )
{
	if ( GetObjectID() == ID ) { return this; }

	for ( size_t i = 0; i != FChildViews.size(); ++i )
	{
		if ( FChildViews[i]->GetObjectID() == ID ) { return FChildViews[i]; }
	}

	// depth search
	for ( size_t i = 0; i != FChildViews.size(); ++i )
	{
		iGUIView* View = FChildViews[i]->FindSubViewByID( ID );

		if ( View ) { return View; }
	}

	return NULL;
}

void iGUIView::FindAllSubViewsByID( const LString& ID, LArray<iGUIView*>* Views )
{
	if ( GetObjectID() == ID ) { Views->push_back( this ); }

	for ( size_t i = 0; i != FChildViews.size(); ++i )
	{
		FChildViews[i]->FindAllSubViewsByID( ID, Views );
	}
}

void iGUIView::BindConsoleCommandS( const LString& Command )
{
	FCommandsStack.push_back( Command );
}

void iGUIView::BindConsoleCommandFadeInS( const LString& Command )
{
	FCommandsStackFadeIn.push_back( Command );
}

LString iGUIView::GetConsoleCommand() const
{
	LString Result;

	for ( std::vector<LString>::const_iterator i = FCommandsStack.begin(); i != FCommandsStack.end(); ++i )
	{
		Result += "; " + *i;
	}

	return Result;
}

LString iGUIView::GetConsoleCommandFadeIn() const
{
	LString Result;

	for ( std::vector<LString>::const_iterator i = FCommandsStackFadeIn.begin(); i != FCommandsStackFadeIn.end(); ++i )
	{
		Result += "; " + *i;
	}

	return Result;
}

void iGUIView::BindConsoleVariableS( const LString& VarName )
{
	if ( !VarName.empty() ) { FConsoleVariable = Env->Console->GetVar( VarName ); }
	else { FConsoleVariable = NULL; }
}

void iGUIView::SetVisible( bool Visible )
{
	if ( FVisible != Visible ) { Env->GUI->RecheckMouse(); }

	FVisible = Visible;
}

float iGUIView::GetViewOpacity() const
{
	iGUIView* Parent = GetParentView();

	return Parent ? GetOpacity() * Parent->GetViewOpacity() : GetOpacity();
}

/*
 * 20/06/2007
     ClearContextMenu()
     AddContextMenuItem()
     GetContextMenuItemsCount()
     GetContextMenuItem()
 * 13/04/2007
     Merged with iGUIAtom
 * 22/03/2007
     Generic binding to console variables
 * 25/02/2007
     Fixed speed of minimizing/maximizing
 * 18/06/2005
     Visibility setup
 * 17/05/2005
     IsTopmost()
 * 01/05/2005
     Bug fixed in ExecuteCommandsStack()
 * 05/02/2005
     Event_Timer()
 * 30/01/2005
     It's here
*/
