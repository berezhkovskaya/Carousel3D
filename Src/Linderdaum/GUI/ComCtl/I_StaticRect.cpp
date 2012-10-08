/**
 * \file I_StaticRect.cpp
 * \brief Container do draw a colored rect
 * \version 0.6.06
 * \date 21/01/2012
 * \author Sergey Kosarevsky, 2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "I_StaticRect.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/CVars.h"
#include "GUI/GUIManager.h"

#include "LColors.h"

clGUIStaticRect::clGUIStaticRect()
{
	SetDefaultColor( LC_White );
}

void clGUIStaticRect::PreRender()
{
	iGUIView::PreRender();

	LVector4 Color( GetDefaultColor() );
	Color.W = GetViewOpacity();

	Env->Renderer->GetCanvas()->Rect( GetX1(), GetY1(), GetX2(), GetY2(), Color );
}

/*
 * 21/01/2012
     It's here
*/
