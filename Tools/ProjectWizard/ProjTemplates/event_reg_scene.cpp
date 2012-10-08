	// set projection
	Projection = Math::Perspective( 45.0f, Env->Viewport->GetAspectRatio(), 0.4f, 2000.0f );

	// receive events from the Engine
	Camera = Env->Linker->Instantiate( "clGameCamera" );
	Camera->GetCamera().SetPosition( LVector3(0,-10,10) );
	CONNECTOBJ( L_EVENT_TIMER, &clGameCamera::Event_TIMER, Camera );

	Env->Connect( L_EVENT_DRAWOVERLAY, Utils::Bind( &DrawOverlay ) );
	Env->Connect( L_EVENT_TIMER,       Utils::Bind( &Update      ) );

	// create scene
	Scene = Env->Linker->Instantiate("clScene");

	// add geom object to the scene
	int ID = Scene->AddGeom( Env->Resources->CreateIcosahedron( 3.0f, LVector3(0) ) );

	// set material
	clMaterial* Mtl = Env->Resources->CreateMaterial();
	Mtl->SetPropertyValue( "DiffuseColor", "1.0 0.0 0.0 0" );
	Mtl->SetPropertyValue( "CastShadow",   "false" );
	Scene->SetMtl( ID, Mtl );

	// set position of the object
	Scene->SetLocalTransform( ID, LMatrix4::GetTranslateMatrix( LVector3(  0.0f, 0.0f, 0.0f ) ) );
