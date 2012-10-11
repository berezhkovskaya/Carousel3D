/**
 * \file StainedGlass.cpp
 * \brief StainedGlass
 * \version 1.0.01
 * \date 11/10/2012
 */

#include "Linderdaum.h"
//#include <cmath>
//using namespace std;
sEnvironment* Env = NULL;

LMatrix4              Projection;
LMatrix4  		      Trans;
clVirtualTrackball    Trackball;

void DrawCarousel(int n, float r) {	
	float alp = 2 * Math::PI / (3 * n);
	float h = 800.f / 480 * r * sin(alp) * 2;
	bool MousePressedL = Env->Console->IsKeyPressed( LK_LBUTTON );
    Env->Viewport->UpdateTrackball( &Trackball, 10.0f, MousePressedL );


	Projection = Math::Perspective( 45.0f, Env->Viewport->GetAspectRatio(), 0.4f, 2000.0f );
	Trans = Trackball.GetRotationMatrix() * LMatrix4::GetTranslateMatrix( LVector3( 0.0f, 0.0f, -5.0f ) );
	Env->Renderer->GetCanvas()->SetMatrices(Projection, Trans);

	LString file = "img0.jpg";
	clImage* Img = Env->Resources->LoadImg(file, L_TEXTURE_2D);		
	
	for (int i = 0; i < n; i++) {
		float alpStart	= i * 3 * alp;
		float alpEnd	= (i * 3 + 2) * alp;
		LVector3 p00 = LVector3(r * cos(alpStart), r * sin(alpStart), -h / 2);
		LVector3 p10 = LVector3(r * cos(alpStart), r * sin(alpStart), h / 2);
		LVector3 p11 = LVector3(r * cos(alpEnd), r * sin(alpEnd), h / 2);
		LVector3 p01 = LVector3(r * cos(alpEnd), r * sin(alpEnd), -h / 2);
		Env->Renderer->GetCanvas()->Line3D(p00, p10, LC_Green);
		Env->Renderer->GetCanvas()->Line3D(p10, p11, LC_Green);
		Env->Renderer->GetCanvas()->Line3D(p11, p01, LC_Green);
		Env->Renderer->GetCanvas()->Line3D(p01, p00, LC_Green);
	}    	

	Env->Renderer->GetCanvas()->Flush();

	for (int i = 0; i < n; i++) {
		float alpStart	= i * 3 * alp;
		float alpEnd	= (i * 3 + 2) * alp;
		LVector3 p00 = LVector3(r * cos(alpStart), r * sin(alpStart), -h / 2);
		LVector3 p10 = LVector3(r * cos(alpStart), r * sin(alpStart), h / 2);
		LVector3 p11 = LVector3(r * cos(alpEnd), r * sin(alpEnd), h / 2);
		LVector3 p01 = LVector3(r * cos(alpEnd), r * sin(alpEnd), -h / 2);
		Env->Renderer->GetCanvas()->TexturedRect3D(p00, p10, p11, p01, Img->GetTexture(), NULL, true);
		//		Env->Renderer->GetCanvas()->TexturedRect3D(p00, p10, p11, p01, NULL, Env->Resources->LoadSP("shader.sp", ""), false);
	}    	

	Env->Renderer->GetCanvas()->Flush();
}

void DrawOverlay( LEvent Event, const LEventArgs& Args )
{
	DrawCarousel(6, 0.9f);

	Env->Renderer->GetCanvas()->Flush();
}

APPLICATION_ENTRY_POINT
{
	Env = new sEnvironment;

	Env->DeployDefaultEnvironment( NULL, "..\\..\\CommonMedia" );

	Env->Connect( L_EVENT_DRAWOVERLAY, Utils::Bind( &DrawOverlay ) );

	Env->RunApplication( DEFAULT_CONSOLE_AUTOEXEC );

	APPLICATION_EXIT_POINT( Env );
}

APPLICATION_SHUTDOWN
{
};

/*
 * 08/10/2012
     Created with Linderdaum ProjectWizard
*/
