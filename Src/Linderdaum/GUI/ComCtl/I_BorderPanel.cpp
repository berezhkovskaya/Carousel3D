/**
 * \file I_BorderPanel.cpp
 * \brief Panel with border
 * \version 0.5.60
 * \date 17/12/2005
 * \author Sergey Kosarevsky, 2005
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "I_BorderPanel.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/Linker.h"
#include "Resources/ResourcesManager.h"

void clGUIBorderPanel::Event_Registered( iGUIResponder* Source )
{
	clGUIPanel::Event_Registered( Source );

	SetShader( Env->Resources->LoadShader( "interface/shaders/panel.shader" ) );
	//
	iGUIView* LeftV   = Env->Linker->Instantiate( "clGUIVerticalSplitter" );
	iGUIView* RightV  = Env->Linker->Instantiate( "clGUIVerticalSplitter" );
	iGUIView* TopV    = Env->Linker->Instantiate( "clGUIHorizontalSplitter" );
	iGUIView* BottomV = Env->Linker->Instantiate( "clGUIHorizontalSplitter" );

	FIXME( "Pascal legacy... " )

	/// dimensions are calculated in 1024x768 screen space
	const float LeftPos      = 3.0f / 1024.0f;
	const float TopPos       = 5.0f / 768.0f;
	const float HorSize      = GetWidth() - 4.0f / 1024.0f;
	const float VerSize      = GetHeight() - 7.0f / 768.0f;
	const float Thickness    = 2.0f / 1024.0f;
	const float DblThickness = 3.0f / 768.0f;

	const float LeftPosOffs  = LeftPos + 1.0f / 1024.0f;
	const float TopPosOffs   = TopPos - 1.0f / 768.0f;

	LeftV->SetCoords(
	   LeftPos,
	   TopPos,
	   Thickness,
	   VerSize
	);
	RightV->SetCoords(
	   HorSize,
	   TopPos,
	   Thickness,
	   VerSize
	);
	TopV->SetCoords(
	   LeftPosOffs,
	   TopPosOffs,
	   HorSize - DblThickness,
	   Thickness
	);
	BottomV->SetCoords(
	   LeftPosOffs,
	   VerSize + DblThickness,
	   HorSize - DblThickness,
	   DblThickness
	);

	AddView( TopV );
	AddView( BottomV );
	AddView( LeftV );
	AddView( RightV );
};

/*
 * 12/02/2005
     It's here
*/
