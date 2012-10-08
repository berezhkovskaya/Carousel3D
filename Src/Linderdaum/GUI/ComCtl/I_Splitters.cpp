/*
   (Part of Linderdaum Engine)
   Version 0.05.74
   (29/05/2007)
   (C) Sergey Kosarevsky, 2005-2007
   (C) Viktor Latypov, 2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_Splitters.h"

#include "Engine.h"
#include "Environment.h"
#include "GUI/GUIManager.h"
#include "Resources/ResourcesManager.h"

void clGUISplitter::Event_Registered( iGUIResponder* Source )
{
	clGUIPanel::Event_Registered( Source );

	SetShader( Env->Resources->LoadShader( FSplitterShaderName ) );

	FGeometry = Env->GUI->GetDefaultRect();
}

/*
 * 29/05/2007
     Refactored solution
 * 12/02/2005
     It's here
*/
