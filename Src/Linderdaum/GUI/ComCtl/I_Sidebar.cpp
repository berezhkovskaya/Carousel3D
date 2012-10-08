/*
   (Part of Linderdaum Engine)
   Version 0.5.73
   (05/04/2007)
   (C) Sergey Kosarevsky, 2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_Sidebar.h"

#include "Environment.h"
#include "GUI/GUIManager.h"
#include "Utils/Viewport.h"
#include "Renderer/iRenderContext.h"

#include "LColors.h"

clGUISidebar::clGUISidebar()
{
	FHiddenX = 0;
	FHiddenY = 0;
}

clGUISidebar::~clGUISidebar()
{
}

void clGUISidebar::PreRender()
{
	clGUIDialogWindow::PreRender();

	FIXME( "implement" )
}

void clGUISidebar::PostRender()
{
	clGUIDialogWindow::PostRender();
}

void clGUISidebar::Event_Registered( iGUIResponder* Source )
{
	clGUIDialogWindow::Event_Registered( Source );

	FTargetX = GetX1();
	FTargetY = GetY1();

	MoveTo( LVector2( FHiddenX, FHiddenY ) );
}

void clGUISidebar::Event_MouseInside( iGUIResponder* Source, const LVector2& Pnt )
{
	clGUIDialogWindow::Event_MouseInside( Source, Pnt );
}

void clGUISidebar::Event_MouseOutside( iGUIResponder* Source, const LVector2& Pnt )
{
	clGUIDialogWindow::Event_MouseOutside( Source, Pnt );
}

void clGUISidebar::Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState )
{
	clGUIDialogWindow::Event_MouseLeft( Source, Pnt, KeyState );
}

void clGUISidebar::Event_Timer( iGUIResponder* Source, float DeltaTime )
{
	clGUIDialogWindow::Event_Timer( Source, DeltaTime );

	LVector2 Dist;

	const sMouseCursorInfo MCI = Env->Viewport->GetMouseCursorInfo();

	if ( Contains( LVector2( MCI.FMouseX, MCI.FMouseY ) ) )
	{
		Dist = LVector2( FTargetX - GetX1(), FTargetY - GetY1() );
	}
	else
	{
		Dist = LVector2( FHiddenX - GetX1(), FHiddenY - GetY1() );
	}

	float Speed = 0.05f;

	LVector2 Delta( 0, 0 );

	if ( fabs( Dist.X ) > 0 )
	{
		Delta.X = fabs( Dist.X ) / Dist.X;
	}

	if ( fabs( Dist.Y ) > 0 )
	{
		Delta.Y = fabs( Dist.Y ) / Dist.Y;
	}

	Delta = Delta * Speed;

	if ( fabs( Delta.X ) > fabs( Dist.X ) )
	{
		Delta.X = Dist.X;
	}

	if ( fabs( Delta.Y ) > fabs( Dist.Y ) )
	{
		Delta.Y = Dist.Y;
	}

	if ( fabs( Delta.X  ) + fabs( Delta.Y ) > 0 )
	{
		MoveRel( Delta );
	}

	Env->GUI->RecheckMouse();
}

/*
 * 05/04/2007
     SaveToXLMLStream()
 * 23/03/2007
     Improved repainting performance
 * 18/03/2007
     It's here
*/
