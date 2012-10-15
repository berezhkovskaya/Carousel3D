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
LMatrix4              Trans;
LMatrix3              rotation;
LMatrix3              rotationXY;
LMatrix3              rotationYZ;
LMatrix3              rotationYZ0;
LMatrix3              inv;
LVector3              p00;
LVector3              p10;
LVector3              p11;
LVector3              p01;
LVector2              MousePosition;
LVector2              startPos;
bool                  MousePressedL = false;
float                 currentXAngle = 0;
float                 addXAngle;
float                 currentYAngle = Math::PI / 3;
float                 addYAngle;
float                 alp;
float                 h;
float                 ralp;
float                 roalp;
float                 planeY = -1.5;
float                 inf = 1.2;
float                 r = 0.9f;
int                   n = 6;
bool                  newMousePressedL;

void getVertexes(int i, LVector3& p00, LVector3& p10, LVector3& p11, LVector3& p01) {
    float alpStart  = i * 3 * alp;
    float alpEnd    = (i * 3 + 2) * alp;
    p00 = LVector3(r * cos(alpStart), r * sin(alpStart), -h / 2);
    p10 = LVector3(r * cos(alpStart), r * sin(alpStart), h / 2);
    p11 = LVector3(r * cos(alpEnd),   r * sin(alpEnd),   h / 2);
    p01 = LVector3(r * cos(alpEnd),   r * sin(alpEnd),   -h / 2);
    p00 = rotation * p00;
    p10 = rotation * p10;
    p11 = rotation * p11;
    p01 = rotation * p01;
}
void getSymmetricVertexes(LVector3& p00, LVector3& p10, LVector3& p11, LVector3& p01) {
    p00 = inv * p00;
    p10 = inv * p10;
    p11 = inv * p11;
    p01 = inv * p01;
    p00[1] = 2 * planeY - p00[1];
    p10[1] = 2 * planeY - p10[1];
    p11[1] = 2 * planeY - p11[1];
    p01[1] = 2 * planeY - p01[1];
    p00 = rotationYZ0 * p00;
    p10 = rotationYZ0 * p10;
    p11 = rotationYZ0 * p11;
    p01 = rotationYZ0 * p01;
}
void drawCarcas(LVector3& p00, LVector3& p10, LVector3& p11, LVector3& p01, LVector4 cl) {
    Env->Renderer->GetCanvas()->Line3D(p00, p10, cl);
    Env->Renderer->GetCanvas()->Line3D(p10, p11, cl);
    Env->Renderer->GetCanvas()->Line3D(p11, p01, cl);
    Env->Renderer->GetCanvas()->Line3D(p01, p00, cl);
}
void DrawCarousel() {   
    alp = 2 * Math::PI / (3 * n);
    h = 800.f / 480 * r * sin(alp) * 2;
    newMousePressedL = Env->Console->IsKeyPressed( LK_LBUTTON );
    MousePosition = Env->Viewport->GetMousePosition();    
    if (!MousePressedL && newMousePressedL) {
        startPos = MousePosition;
    }
    if (MousePressedL && !newMousePressedL) {
        currentXAngle += addXAngle;
        currentYAngle += addYAngle;
    }
    addXAngle = 0;
    addYAngle = 0;

    if (newMousePressedL) {
        addXAngle = -asin(MousePosition[0]) + asin(startPos[0]);
        addYAngle = -asin(MousePosition[1]) + asin(startPos[1]);
    }
    //Env->Viewport->UpdateTrackball( &Trackball, 10.0f, MousePressedL );
    ralp = currentYAngle + addYAngle;
    roalp = currentXAngle + addXAngle;
   // ralp = 0;
    //roalp = 0;
    rotationXY = LMatrix3(LVector3(cos(roalp), -sin(roalp), 0.f), 
                          LVector3(sin(roalp), cos(roalp),  0.f),
                          LVector3(0.f,        0.f,         1.f));
    rotationYZ = LMatrix3(LVector3(1.f, 0.f,       0.f), 
                          LVector3(0.f, cos(ralp), -sin(ralp)),
                          LVector3(0.f, sin(ralp), cos(ralp)));
    rotation = rotationXY *  rotationYZ;

    MousePressedL = newMousePressedL;

    Projection = Math::Perspective( 45.0f, Env->Viewport->GetAspectRatio(), 0.4f, 2000.0f );
    Trans = /*Trackball.GetRotationMatrix() * */LMatrix4::GetTranslateMatrix( LVector3( 0.0f, 0.0f, -5.0f ) );
    Env->Renderer->GetCanvas()->SetMatrices(Projection, Trans);

    iShaderProgram* SP = Env->Resources->LoadSP("shader1_1.sp", "");
    SP->BindUniforms();
    SP->SetUniformNameMat4Array( "ProjectionMatrix", 1, Projection );
    SP->SetUniformNameMat4Array( "ModelViewMatrix",  1, Trans );
        
    ralp = -Math::PI / 3;
    //ralp = 0;
    rotationYZ0 = LMatrix3(LVector3(1.f, 0.f,       0.f), 
                           LVector3(0.f, cos(ralp), -sin(ralp)),
                           LVector3(0.f, sin(ralp), cos(ralp)));
    inv = rotationYZ0.GetInversed();
    


    /*for (int i = 0; i < n; i++) {       
        getVertexes(i, p00, p10, p11, p01);
        getSymmetricVertexes(p00, p10, p11, p01);
        drawCarcas(p00, p10, p11, p01, LC_Green);
    }       

    Env->Renderer->GetCanvas()->Flush();*/

    for (int i = 0; i < n; i++) {       
        getVertexes(i, p00, p10, p11, p01);
        getSymmetricVertexes(p00, p10, p11, p01);
        SP->BindUniforms();
        SP->SetUniformNameVec4Array("u_Color", 1, vec4( 1.f * i / n, 0.f, 1 - 1.f * i / n , 1.f ));         
        Env->Renderer->GetCanvas()->TexturedRect3D(p00, p10, p11, p01, NULL, SP, true);
    }       

    Env->Renderer->GetCanvas()->Flush();

    clRenderState* rendState = Env->Resources->LoadShader("shader2_2.sp");
    rendState->GetShaderProgram()->BindUniforms();
    rendState->GetShaderProgram()->SetUniformNameMat4Array( "ProjectionMatrix", 1, Projection );
    rendState->GetShaderProgram()->SetUniformNameMat4Array( "ModelViewMatrix",  1, Trans );
    Env->Renderer->GetCanvas()->Rect3DShader(rotationYZ0 * LVector3(-inf, planeY, -inf), 
                                             rotationYZ0 * LVector3(-inf, planeY, inf),
                                             rotationYZ0 * LVector3(inf,  planeY, inf),
                                             rotationYZ0 * LVector3(inf,  planeY, -inf),
                                             rendState);
    Env->Renderer->GetCanvas()->Flush();

  /*  for (int i = 0; i < n; i++) {       
        getVertexes(i, p00, p10, p11, p01);
        drawCarcas(p00, p10, p11, p01, LC_Green);
    }       

    Env->Renderer->GetCanvas()->Flush();*/

    for (int i = 0; i < n; i++) {       
        getVertexes(i, p00, p10, p11, p01);
        SP->BindUniforms();
        SP->SetUniformNameVec4Array("u_Color", 1, vec4( 1.f * i / n, 0.f, 1 - 1.f * i / n , 1.f ));         
        Env->Renderer->GetCanvas()->TexturedRect3D(p00, p10, p11, p01, NULL, SP, true);
    }       

    Env->Renderer->GetCanvas()->Flush();    
}

void DrawOverlay( LEvent Event, const LEventArgs& Args )
{
    DrawCarousel();

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
