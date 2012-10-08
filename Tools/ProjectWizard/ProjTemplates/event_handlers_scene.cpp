
LMatrix4              Projection;
clVirtualTrackball    Trackball;
clGameCamera*         Camera = NULL;
clScene*              Scene  = NULL;

void DrawOverlay(LEvent Event, const LEventArgs& Args)
{
	// update camera transform & projection
	Scene->SetCameraTransform( Trackball.GetRotationMatrix() * Camera->GetCamera().GetModelViewMatrix() );
	Scene->SetCameraProjection( Projection );

	// render scene
	Scene->SetUseOffscreenBuffer( false, false );
	Scene->RenderForward();

	// update trackball
	bool MousePressedL = Env->Console->IsKeyPressed( LK_LBUTTON );

	Env->Viewport->UpdateTrackball( &Trackball, 10.0f, MousePressedL );
}

void Update( LEvent Event, const LEventArgs& Args )
{
	// use Args.FFloatArg for DeltaTime (in seconds)
}
