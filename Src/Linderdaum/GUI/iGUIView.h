/**
 * \file iGUIView.h
 * \brief GUI view
 * \version 0.6.06
 * \date 21/01/2012
 * \author Sergey Kosarevsky, 2005-2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iGUIView_
#define _iGUIView_

#include "iGUIRegion.h"

#include <vector>

class clCVar;

/// Windows docking type in the internal GUI
enum LDockingType
{
	DOCKING_MOVE,
	DOCKING_RESIZE
};

/// Base class for all visible GUI elements
class netexportable scriptfinal iGUIView: public iGUIRegion, public iGUIViewRenderer
{
public:
	typedef std::vector<iGUIView*> clViewsList;
public:
	iGUIView();
	virtual ~iGUIView();
	//
	// iObject interface
	//
	NET_EXPORTABLE();
	SERIALIZABLE_CLASS();

	virtual bool    EndLoad();

	//
	// iGUIRegion interface
	//
	virtual void        SetRegionState( LRegionState State );
	virtual void        MoveRel( const LVector2& Delta );
	virtual void        SetSize( const float Width, const float Height );
	virtual float       GetViewOpacity() const;

	//
	// iGUIViewRenderer interface
	//
	virtual void        PreRender() {};
	virtual void        PostRender() {};

	//
	// iGUIView
	//
	virtual void        AddView( iGUIView* View );
	virtual void        AddViews( const clViewsList& Views );
	virtual void        RemoveView( iGUIView* View );
	virtual bool        IsMouseOver() const { return FMouseOver; };
	virtual LVector2    GetMousePos() const { return FMousePosLocal; };
	virtual bool        IsViewKeyboardSelectable() const { return FKeyboardSelectable; };
	virtual bool        IsPage() const { return false; };
	virtual bool        IsViewDockable() const { return false; };
	virtual bool        IsViewDraggable() const { return false; };
	virtual bool        IsViewSizeable() const { return false; };
	virtual bool        IsViewDissolvable() const { return true; };
	virtual bool        IsViewDraggableAt( const LVector2& Pnt ) const
	{
		return ( IsViewDraggable() && ContainsCaption( Pnt ) && !ContainsCloseButton( Pnt ) );
	};
	virtual bool        IsViewSizeableAt( const LVector2& Pnt ) const
	{
		return ( IsViewSizeable() && ContainsResizeCorner( Pnt ) );
	};
	virtual bool        IsTopmost() const;
	virtual bool        IsPageActive() const;
	virtual void        DragTo( const LVector2& Pnt );
	virtual void        ResizeCornerTo( const LVector2& Pnt );
	virtual bool        CanFocus() const { return true; };
	virtual bool        IsBeenDragged() const { return FBeenDragged; };
	virtual bool        IsBeenSized() const { return FBeenSized; };
	virtual float       SuggestMinimalWidth() const;
	virtual float       SuggestMinimalHeight() const;
	virtual bool        IsMinimized() const;
	virtual bool        IsMaximized() const;
	virtual void        Minimize();
	virtual void        Maximize();
	virtual float       GetToolTipTime() const;
	virtual LString     GetToolTipText() const;
	virtual void        SetToolTipTime( float Time ) { FToolTipTime = Time; };
	virtual void        SetToolTipText( const LString& Text ) { FToolTipText = Text; };
	virtual void        ExecuteCommandsStack();
	virtual void        ExecuteCommandsStackFadeIn();
	virtual void        DispatchSpecialKey( const int Key, const bool KeyState, const LVector2& Pnt );
	virtual clCVar*     GetConsoleVariable() const { return FConsoleVariable; };
	virtual LString     GetConsoleCommand() const;
	virtual LString     GetConsoleCommandFadeIn() const;
	virtual iGUIView*   GetParentView() const { return FParentView; };
	virtual iGUIView*   FindSubViewByID( const LString& ID );
	virtual void        FindAllSubViewsByID( const LString& ID, LArray<iGUIView*>* Views );
	virtual size_t      GetTotalSubViews() const { return FChildViews.size(); };
	virtual iGUIView*   GetSubView( size_t i ) const { return ( i >= FChildViews.size() ) ? NULL : FChildViews[i]; };
	virtual void        SetSubView( size_t i, iGUIView* View ) { /* dummy for .NET export */ }
	//
	virtual void        SetVisible( bool Visible );
	virtual bool        IsVisible() const { return FVisible; };
	virtual void        SetInteractive( bool Interactive ) { FInteractive = Interactive; };
	virtual bool        IsInteractive() const { return FInteractive; };
	virtual void        SetOnTopPriority( int OnTop );
	virtual int         GetOnTopPriority() const;
	virtual void        RenderHierarchy();
	virtual void        RenderTopmost();
	virtual void        RemoveAllChildViews();
	virtual iGUIView*   FindView( const LVector2& ScreenPnt );
	virtual void        ToFront( iGUIView* View );
	virtual void        BindConsoleCommandS( const LString& Command );  // LV: the 'S' postfix is added to avoid conflicts with property name
	virtual void        BindConsoleCommandFadeInS( const LString& Command );  // LV: the 'S' postfix is added to avoid conflicts with property name
	virtual void        BindConsoleVariableS( const LString& VarName ); // LV: the 'S' postfix is added to avoid conflicts with property name
	virtual LString     GetConsoleVariableName() const;
	virtual LVector4    GetDefaultColor() const { return FDefaultColor; };
	virtual void        SetDefaultColor( const LVector4& Color ) { FDefaultColor = Color; };
	virtual void        SetDefaultColorHex( const LString& Color );
	virtual LString     GetDefaultColorHex() const;
	virtual iFontProperties* GetFont() const;

	virtual LString     GetCommand( int i ) const { return FCommandsStack[i]; }
	virtual void        SetCommand( int i, const LString& Str ) { FCommandsStack[i] = Str; }

	/* Property(Name="OnTop",                Type=int,    Setter=SetOnTopPriority, Validator=<default>, Getter=GetOnTopPriority       ) */
	/* Property(Name="BindConsoleCommand",   Type=string, Setter=BindConsoleCommandS,                   Getter=GetConsoleCommand      ) */
	/* Property(Name="BindConsoleCommandFadeIn", Type=string, Setter=BindConsoleCommandFadeInS,         Getter=GetConsoleCommandFadeIn) */
	/* Property(Name="BindConsoleVariable",  Type=string, Setter=BindConsoleVariableS,                  Getter=GetConsoleVariableName ) */
	/* Property(Name="Visible",              Type=bool,   Setter=SetVisible,       Validator=<default>, Getter=IsVisible              ) */
	/* Property(Name="Interactive",          Type=bool,   Setter=SetInteractive,   Validator=<default>, Getter=IsInteractive          ) */
	/* Property(Name="ToolTip",              Type=string, Setter=SetToolTipText,                        Getter=GetToolTipText         ) */
	/* Property(Name="ToolTipTime",          Type=float,  Setter=SetToolTipTime,   Validator=<default>, Getter=GetToolTipTime         ) */
	/* Property(Name="Color",                Type=vec4,   Setter=SetDefaultColor,                       Getter=GetDefaultColor        ) */
	/* Property(Name="ColorHex",             Type=string, Setter=SetDefaultColorHex,                    Getter=GetDefaultColorHex     ) */
	// new property for the console command array. FCommandsStack must become public at the time... Or we must provide the complete size/resize/get_item/set_item set of functions
	/* Property(Name="ConsoleCommand",        Type=string,   IndexType=int, FieldName=FCommandsStack, NetIndexedGetter=GetCommand, NetIndexedSetter=SetCommand  ) */
	/* Property(Name="ConsoleCommandFadeIn",  Type=string,   IndexType=int, FieldName=FCommandsStackFadeIn, NetIndexedGetter=GetCommand, NetIndexedSetter=SetCommand  ) */
	/* Property(Name="ChildViews",     Type=iGUIView, IndexType=size_t, FieldName=FChildViews,    NetIndexedGetter=GetSubView, NetIndexedSetter=SetSubView  ) */
	/* Property( Name="MinimalOpacity",       Type=float,   FieldName=FMinimalOpacity            ) */
	/* Property( Name="MaximalOpacity",       Type=float,   FieldName=FMaximalOpacity            ) */
	/* Property( Name="OpacityDeltaPlus",     Type=float,   FieldName=FOpacityDeltaPlus          ) */
	/* Property( Name="OpacityDeltaMinus",    Type=float,   FieldName=FOpacityDeltaMinus         ) */
	/* Property( Name="OpacityDeltaDissolve", Type=float,   FieldName=FOpacityDeltaDissolve      ) */
	/* Property( Name="Font",               Type=iFontProperties,  FieldName=FFontProperties    ) */
	/* Property( Name="KeyboardSelectable",   Type=bool,    FieldName=FKeyboardSelectable        ) */
	float            FMinimalOpacity;
	float            FMaximalOpacity;
	float            FOpacityDeltaPlus;
	float            FOpacityDeltaMinus;
	float            FOpacityDeltaDissolve;
	iFontProperties* FFontProperties;
	bool             FKeyboardSelectable;
public:
	virtual void    Event_Minimized( iGUIResponder* Source );
	virtual void    Event_Maximized( iGUIResponder* Source );
	virtual void    Event_Shrinked( iGUIResponder* Source );
	virtual void    Event_Dissolved( iGUIResponder* Source );
	virtual void    Event_MouseInside( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_MouseOutside( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_MouseRight( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState );
	virtual void    Event_Timer( iGUIResponder* Source, float DeltaTime );
	virtual void    Event_StartDragging( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_StopDragging( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_StartResize( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_StopResize( iGUIResponder* Source, const LVector2& Pnt );
private:
	void    SetParentView( iGUIView* View ) { FParentView = View; };
	void    RefreshParents();
	void    ProcessDocking( LDockingType DockingType, float* CoordX, float* CoordY );
	bool    IsDestructorCalled() const { return FDestructorCalled; };
public:
	std::vector<LString>      FCommandsStack;
	std::vector<LString>      FCommandsStackFadeIn;
	clViewsList               FChildViews;
private:
	bool      FDestructorCalled;
	iGUIView* FParentView;
	bool      FVisible;
	bool      FInteractive;
	int       FOnTopPriority;
	LString   FID;
	bool      FMouseOver;
	bool      FBeenDragged;
	bool      FBeenSized;
	LVector2  FDragDelta;
	LVector2  FMousePosLocal;
	LVector4  FDefaultColor;
	float     FToolTipTime;
	LString   FToolTipText;
	clCVar*   FConsoleVariable;
};

#endif

/*
 * 25/10/2010
     SetDefaultTextColor()
     GetDefaultTextColor()
 * 04/10/2010
     FChildViews property
 * 29/05/2009
     Context menu removed
 * 20/06/2007
     ClearContextMenu()
     AddContextMenuItem()
     GetContextMenuItemsCount()
     GetContextMenuItem()
 * 14/04/2007
     GetObjectID() from iObject used instead of GetID()
 * 05/04/2007
     Some experimental code for hierarchy added
 * 14/11/2006
     SetToolTipTime()
     SetToolTipText()
     GetToolTipTime()
     GetToolTipText()
 * 09/04/2005
     GetID()
 * 04/03/2005
     DispatchSpecialKey()
 * 06/02/2005
     IsViewDockable()
 * 30/01/2005
     It's here
*/
