#include "Linderdaum.h"

sEnvironment* Env = NULL;

LMatrix4              Projection;
clVirtualTrackball    Trackball;
clGameCamera*         Camera = NULL;
clScene*              Scene  = NULL;

void DrawOverlay(LEvent Event, const LEventArgs& Args)
{
    // обновим камеру
    LMatrix4 Trans( Trackball.GetRotationMatrix() * Camera->GetCamera().GetModelViewMatrix() );
    Scene->SetCameraTransform( Trans );
    Scene->SetCameraProjection( Projection );

    // отрисуем сцену
    Scene->SetUseOffscreenBuffer( false, false );
    Scene->RenderForward();

    // обновим виртуальный трэкбол
    bool MousePressedL = Env->Console->IsKeyPressed( LK_LBUTTON );

    Env->Viewport->UpdateTrackball( &Trackball, 10.0f, MousePressedL );
}

void Update( LEvent Event, const LEventArgs& Args )
{
    // в Args.FFloatArg хранится DeltaTime в миллисекундах
}

APPLICATION_ENTRY_POINT
{
	std::vector<LString> CommandLine;
    EXTRACT_COMMAND_LINE(CommandLine);

    Env = new sEnvironment();

    // CommonMedia используется только на PC, на Андроиде она будет хитро упакована в ресурсы
	Env->DeployDefaultEnvironment(&CommandLine, "..\\..\\CommonMedia" );
    Env->FileSystem->Mount("GameData");

    Projection = Math::Perspective( 45.0f, Env->Viewport->GetAspectRatio(), 0.4f, 2000.0f );

    // создадим камеру и подпишем её на события
    Camera = Env->Linker->Instantiate( "clGameCamera" );
    Camera->GetCamera().SetPosition( LVector3(0,-10,10) );
    CONNECTOBJ( L_EVENT_TIMER, &clGameCamera::Event_TIMER, Camera );

    Env->Connect( L_EVENT_DRAWOVERLAY, Utils::Bind( &DrawOverlay ) );
    Env->Connect( L_EVENT_TIMER,       Utils::Bind( &Update      ) );

    // создадим сцену
    Scene = Env->Linker->Instantiate("clScene");

    // и добавим в неё меш
    int ID = Scene->AddGeom( Env->Resources->CreateIcosahedron( 3.0f, LVector3(0) ) );

    // установим ему материал
    clMaterial* Mtl = Env->Resources->CreateMaterial();
    Mtl->SetPropertyValue( "DiffuseColor", "1.0 0.0 0.0 0" );
    Mtl->SetPropertyValue( "CastShadow",   "false" );
    Scene->SetMtl( ID, Mtl );

    // установим положение
    Scene->SetLocalTransform( ID, LMatrix4::GetTranslateMatrix( LVector3(  0.0f, 0.0f, 0.0f ) ) );

    Env->RunApplication( DEFAULT_CONSOLE_AUTOEXEC );

    APPLICATION_EXIT_POINT( Env );
}

APPLICATION_SHUTDOWN
{
}