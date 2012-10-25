/**
 * \file StainedGlass.cpp
 * \brief StainedGlass
 * \version 1.0.01
 * \date 11/10/2012
 */

#include "Linderdaum.h"

sEnvironment* Env = NULL;

LMatrix4              Projection;
LMatrix4              Trans;
LMatrix4              Trans1;

LMatrix3              rotation;
LMatrix3              rotationXY;
LMatrix3              rotationYZ;
LMatrix3              rotationYZ0;
LMatrix3              inv;
LVector3              p00;
LVector3              p10;
LVector3              p11;
LVector3              p01;
LVector3              Light = LVector3(0.f, 5.f, 0.f);
LVector2              MousePosition;
LVector2              startPos;
bool                  MousePressedL = false;
float                 currentXAngle = 0;
float                 addXAngle;
float                 addYAngle;
float                 alp;
float                 h;
float                 ralp;
float                 roalp;
float                 startRotYAlp = Math::PI / 3;
float                 currentYAngle = startRotYAlp;
LVector4              plane = LVector4(0.f, cos(Math::PI / 2 - startRotYAlp), sin(Math::PI / 2 - startRotYAlp), 1.1f);
LVector4              plane0 = LVector4(0.f, cos(Math::PI / 2 - startRotYAlp), sin(Math::PI / 2 - startRotYAlp), 0.f);

//float                 inf = 1.2;
float                 r = 0.9f;
int                   n = 6;
bool                  newMousePressedL;

clVertexAttribs*      clVA;

iVertexArray*      iiVA;
iRenderTarget* iRT;
clRenderState* rendState;
clRenderState* rendStateFinal;

void drawRect3D_1(LVector3 p00, LVector3 p10, LVector3 p11, LVector3 p01, LVector3 n, clRenderState* State ) {
	
	if (clVA == NULL) {
		clVA = clVertexAttribs::CreateEmpty();
	}
	clVertexAttribs* VA = clVA;	

	VA->Restart( L_PT_TRIANGLE_STRIP, 4, L_TEXCOORDS_BIT | L_NORMALS_BIT );
	
	VA->SetNormalV( n );

	VA->SetTexCoord( 0, 0, 0 );
	VA->EmitVertexV( p00, -1, -1 );

	VA->SetTexCoord( 0, 1, 0 );
	VA->EmitVertexV( p01, -1, -1 );

	VA->SetTexCoord( 1, 0, 0 );
	VA->EmitVertexV( p10, -1, -1 );

	VA->SetTexCoord( 1, 1, 0 );
	VA->EmitVertexV( p11, -1, -1 );

	State->GetShaderProgram()->BindUniforms();	
	State->GetShaderProgram()->SetUniformNameVec3Array( "u_Normal", 1, n);

	if (iiVA == NULL) {
		iiVA = Env->Renderer->AllocateEmptyVA();
		iiVA->SetVertexAttribs(VA);
	}
	iiVA->CommitChanges();
	Env->Renderer->AddBuffer( iiVA, State, 1, false );
}

void drawRect3D(LVector3 p00, LVector3 p10, LVector3 p11, LVector3 p01, clRenderState* State ) {
	LVector3 n = LVector3((p10 - p00).Cross(p01 - p00)); 
	n.Normalize();
	float e = 0.001f;
	drawRect3D_1(p00 + n * e, p10 + n * e, p11 + n * e, p01 + n * e,  n, State);
	drawRect3D_1(p00 - n * e, p10 - n * e, p11 - n * e, p01 - n * e, -n, State);
}

LVector3 getSymm(LVector3 p, LVector4 plane) {
	float dst = LVector4(p, 1.f).Dot(plane);
	return p - 2 * dst * plane.ToVector3();	
}
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
    p00 = getSymm(p00, plane);
	p10 = getSymm(p10, plane);
	p11 = getSymm(p11, plane);
	p01 = getSymm(p01, plane);
}
void drawCarcas(LVector3& p00, LVector3& p10, LVector3& p11, LVector3& p01, LVector4 cl) {
    Env->Renderer->GetCanvas()->Line3D(p00, p10, cl);
    Env->Renderer->GetCanvas()->Line3D(p10, p11, cl);
    Env->Renderer->GetCanvas()->Line3D(p11, p01, cl);
    Env->Renderer->GetCanvas()->Line3D(p01, p00, cl);
}
void drawAxis() {
    LVector3 p1 = LVector3(0.f, 0.f, 100.f);
    LVector3 p2 = LVector3(0.f, 0.f, -100.f);
    p1 = rotation * p1;
    p2 = rotation * p2;
    Env->Renderer->GetCanvas()->Line3D(p1, p2, LC_Red);
}
void drawSymmetricAxis() {
    LVector3 p1 = LVector3(0.f, 0.f, 100.f);
    LVector3 p2 = LVector3(0.f, 0.f, -100.f);
    p1 = rotation * p1;
    p2 = rotation * p2;
    p1 = getSymm(p1, plane);
	p2 = getSymm(p2, plane);
    Env->Renderer->GetCanvas()->Line3D(p1, p2, LC_Blue);
}

void drawPlaneSquare(LVector4 plane, clRenderState* rendState) {
	LVector3 n = plane.ToVector3();
	LVector3 v1 = LVector3(1.f, 0.f, 0.f);
	LVector3 v2 = n.Cross(v1);
	float r = 1.f;	
	LVector3 p0 = -n * plane.W;

	p00 = p0 + v1 * r + v2 * r;
	p01 = p0 + v1 * r - v2 * r;
	p11 = p0 - v1 * r - v2 * r;
	p10 = p0 - v1 * r + v2 * r;

	Env->Renderer->GetCanvas()->Rect3DShader(p00, p10, p11, p01, rendState);
    Env->Renderer->GetCanvas()->Flush();
}
void RenderScene(LMatrix4 Projection, LMatrix4 Trans) {
	Env->Renderer->GetCanvas()->SetMatrices(Projection, Trans);

    /*iShaderProgram* SP = Env->Resources->LoadSP("shader1_1.sp", "");
    SP->BindUniforms();
    SP->SetUniformNameMat4Array( "ProjectionMatrix", 1, Projection );
    SP->SetUniformNameMat4Array( "ModelViewMatrix",  1, Trans );
     */   
     


    /*for (int i = 0; i < n; i++) {       
        getVertexes(i, p00, p10, p11, p01);
        getSymmetricVertexes(p00, p10, p11, p01);
        drawCarcas(p00, p10, p11, p01, LC_Green);
    }       

    Env->Renderer->GetCanvas()->Flush();*/
    //drawSymmetricAxis();

    /*for (int i = 0; i < n; i++) {       
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
    drawPlaneSquare(plane, rendState);
    */

  /*  for (int i = 0; i < n; i++) {       
        getVertexes(i, p00, p10, p11, p01);
        drawCarcas(p00, p10, p11, p01, LC_Green);
    }       

    Env->Renderer->GetCanvas()->Flush();*/
    //drawAxis();
    rendState->GetShaderProgram()->BindUniforms();
    rendState->GetShaderProgram()->SetUniformNameVec3Array( "u_Light", 1, Light);
    rendState->GetShaderProgram()->SetUniformNameMat4Array( "ProjectionMatrix", 1, Projection );
    rendState->GetShaderProgram()->SetUniformNameMat4Array( "ModelViewMatrix",  1, Trans );

    for (int i = 0; i < n; i++) {       
        getVertexes(i, p00, p10, p11, p01);
        
        rendState->GetShaderProgram()->BindUniforms();
        rendState->GetShaderProgram()->SetUniformNameVec4Array("u_Color", 1, vec4( 1.f * i / n, 0.f, 1 - 1.f * i / n , 1.f ));
        drawRect3D(p00, p10, p11, p01, rendState);
//        Env->Renderer->GetCanvas()->TexturedRect3D(p00, p10, p11, p01, NULL, SP, true);
    }       

    //drawRect3D(LVector3(-1.f, 0.f, 0.f), LVector3(-1.f, 10.f, 0.f), LVector3(-1.f, 10.f, 10.f), LVector3(-1.f, 0.f, 10.f), rendState);

    Env->Renderer->GetCanvas()->Flush();    
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
    ralp = currentYAngle + addYAngle;
    roalp = currentXAngle + addXAngle;
    //ralp = 0;
    //roalp = 0;
    rotationXY = LMatrix3(LVector3(cos(roalp), -sin(roalp), 0.f), 
                          LVector3(sin(roalp), cos(roalp),  0.f),
                          LVector3(0.f,        0.f,         1.f));
    rotationYZ = LMatrix3(LVector3(1.f, 0.f,       0.f), 
                          LVector3(0.f, cos(ralp), -sin(ralp)),
                          LVector3(0.f, sin(ralp), cos(ralp)));
    rotation = rotationXY * rotationYZ;

    MousePressedL = newMousePressedL;

    Projection = Math::Perspective( 45.0f, Env->Viewport->GetAspectRatio(), 0.4f, 2000.0f );
    Trans = LMatrix4::GetTranslateMatrix( LVector3( 0.0f, 0.0f, -5.0f ) );
    Trans1 =  Math::LookAt( getSymm(LVector3( 0.0f, 0.0f, -5.0f ), plane) , getSymm(LVector3( 0.0f, 0.0f, 0.f ), plane),  getSymm(LVector3( 0.0f, 1.f, 0.f ), plane0));
    
    if (iRT == NULL) {
	    iRT = Env->Renderer->CreateRenderTarget( 512, 512, 1, 8, true, 1);
	}
    iRT->Bind(0);

    Env->Renderer->GetCanvas()->SetMatrices(Projection, Trans1);
    RenderScene(Projection, Trans1);        
    iRT->UnBind();

    iTexture* tx = iRT->GetColorTexture(0);

    rendStateFinal->GetShaderProgram()->BindUniforms();
    rendStateFinal->GetShaderProgram()->SetUniformNameMat4Array( "ProjectionMatrix", 1, Projection );
    rendStateFinal->GetShaderProgram()->SetUniformNameMat4Array( "ModelViewMatrix",  1, Trans );
    rendStateFinal->SetTexture(1, tx, false);
    drawPlaneSquare(plane, rendStateFinal);
    Env->Renderer->GetCanvas()->Flush();

    RenderScene(Projection, Trans);
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

	 rendState = Env->Resources->LoadShader("shader3.shader");
    rendStateFinal = Env->Resources->LoadShader("shader2.shader");

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
