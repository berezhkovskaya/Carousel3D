	// receive events from the Engine
	Env->Connect( L_EVENT_DRAWOVERLAY, Utils::Bind( &DrawOverlay ) );
	Env->Connect( L_EVENT_TIMER,       Utils::Bind( &Update      ) );
