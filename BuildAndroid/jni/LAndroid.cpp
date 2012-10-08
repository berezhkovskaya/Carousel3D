#include <jni.h>
#include <errno.h>

//#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <android/log.h>

#include "Linderdaum.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "LEngine-JNI", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "LEngine-JNI", __VA_ARGS__))

/// forward declaration of entry point, defined in user's application
APPLICATION_ENTRY_POINT;

/// statics
LString sEnvironment::FCacheDir;
LString sEnvironment::FExternalDir;
LString sEnvironment::FAPKFullName;
LString sEnvironment::FLocaleName = "en";
LString sEnvironment::FKeyValue;
LString sEnvironment::FCommandLine;
sEnvironment::sAndroid_ViewportSize sEnvironment::FAndroidViewportSize;
LVector3 iInputDeviceList::FLinearAcceleration(0.0f);
LVector3 iInputDeviceList::FVelocity(0.0f);
float iInputDeviceList::FParameters[NUM_GPS_PARAMETERS];

sEnvironment* Android_Env = NULL;

/// buffered keyboard input
std::vector< std::pair<LEvent,LEventArgs> > Events;
clMutex                                     EventsMutex;

const size_t MaxEvents = 1024;

/// can be used from the game code to send async events before environment is deployed
void Android_SendAsyncEvent( const LEvent& Event, const LEventArgs& Args )
{
	if ( Events.size() > MaxEvents ) return;

	Events.push_back( std::pair<LEvent,LEventArgs>( Event, Args ) );
}

/// C-runtime patch
extern "C"
{
	// used deep inside FreeImage
	void* lfind( const void * key, const void * base, size_t num, size_t width, int (*fncomparison)(const void *, const void * ) )
	{
		char* Ptr = (char*)base;
	
		for ( size_t i = 0; i != num; i++, Ptr+=width )
		{
			if ( fncomparison( key, Ptr ) == 0 ) return Ptr;
		}

		return NULL;
	}

	// used in libcompress
	int fseeko64(FILE *stream, off64_t offset, int whence)
	{
		return fseek( stream, int( offset & 0xFFFFFFFF ), whence );
	}

	// used in libcompress
	off64_t ftello64(FILE *stream)
	{
		return ftell( stream );
	}
} // extern C

LString ConvertJString(JNIEnv* env, jstring str)
{
	if ( !str ) LString();

	const jsize len = env->GetStringUTFLength(str);
	const char* strChars = env->GetStringUTFChars(str, (jboolean *)0);

	LString Result(strChars, len);

	env->ReleaseStringUTFChars(str, strChars);

	return Result;
}

extern "C"
{

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumJNILib_SetLocaleName(JNIEnv * env, jobject obj, jstring jLocaleName)
{
	sEnvironment::FLocaleName = ConvertJString(env, jLocaleName);
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumJNILib_SetAPKName(JNIEnv * env, jobject obj, jstring jAPKName)
{
	sEnvironment::FAPKFullName = ConvertJString(env, jAPKName);
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumJNILib_SetKey(JNIEnv * env, jobject obj, jstring jKeyValue)
{
	sEnvironment::FKeyValue = ConvertJString(env, jKeyValue);
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumJNILib_SetCommandLine(JNIEnv * env, jobject obj, jstring jCommandLine)
{
	sEnvironment::FCommandLine = ConvertJString(env, jCommandLine);

	LOGI( "SetCommandLine(): %s", sEnvironment::FCommandLine.c_str() );
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumJNILib_SendSync(JNIEnv * env, jobject obj, int Event)
{
	if ( !Android_Env ) return;


	Android_Env->SendSync( static_cast<LEvent>( Event ), LEventArgs(), false );
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumJNILib_SendAsync(JNIEnv * env, jobject obj, int Event)
{
	LMutex Lock( &EventsMutex );

	if ( Events.size() > MaxEvents ) return;

	Events.push_back( std::pair<LEvent,LEventArgs>( LEvent( Event ), LEventArgs() ) );
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumJNILib_SetDirs(JNIEnv * env, jobject obj, jstring jCacheDir, jstring jExternalDir)
{
	LOGI( "LinderdaumJNILib_SetDirs" );

	sEnvironment::FCacheDir    = ConvertJString(env, jCacheDir);
	sEnvironment::FExternalDir = ConvertJString(env, jExternalDir);

	LOGI( "CacheDir    = %s", sEnvironment::FCacheDir.c_str() );
	LOGI( "ExternalDir = %s", sEnvironment::FExternalDir.c_str() );
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumJNILib_SurfaceChanged(JNIEnv * env, jobject obj, int Width, int Height, int xDPI, int yDPI)
{
	LOGI( "LinderdaumJNILib_SurfaceChanged" );

	sEnvironment::FAndroidViewportSize.FWidth  = Width;
	sEnvironment::FAndroidViewportSize.FHeight = Height;
	sEnvironment::FAndroidViewportSize.FDPI    = LVector2i( xDPI, yDPI );

	LOGI( "%i x %i", Width, Height );

	if ( Android_Env )
	{
		LMutex Lock( &EventsMutex );

		if ( Events.size() > MaxEvents ) return;

		Events.push_back( std::pair<LEvent,LEventArgs>( L_EVENT_SURFACE_CHANGED, LEventArgs() ) );
	}
}

LString ImageFileName = "";

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumEngineActivity_SendSelectedImage(JNIEnv * env, jobject obj, jstring jImageFileName)
{
	LOGI( "LinderdaumEngineActivity_SendSelectedImage" );

	ImageFileName = ConvertJString( env, jImageFileName );

	LOGI( "ImageFileName=%s", ImageFileName.c_str() );

	LMutex Lock( &EventsMutex );

	if ( Events.size() > MaxEvents ) return;

	Events.push_back( std::pair<LEvent,LEventArgs>( L_EVENT_FILE_SELECTED, LEventArgs( &ImageFileName ) ) );
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumEngineActivity_SendKey(JNIEnv * env, jobject obj, int keycode, bool pressed)
{
	if ( !Android_Env ) return;

	LMutex Lock( &EventsMutex );

	Android_Env->Logger->LogP( L_DEBUG, "Enquing keypress. Key: %i, Pressed: %i", keycode, pressed );

	if ( Events.size() > MaxEvents ) return;

	Events.push_back( std::pair<LEvent,LEventArgs>( L_EVENT_KEY, LEventArgs( keycode, pressed ) ) );
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumEngineActivity_SendMotion(JNIEnv * env, jobject obj, int PointerID, int x, int y, bool pressed, int Flag)
{                        
	if ( !Android_Env ) return;

	LMutex Lock( &EventsMutex );

	Android_Env->Logger->LogP( L_DEBUG, "Enquing motion. PointerID: %i, x: %i y: %i pressed: %i", PointerID, x, y, pressed );

	if ( Events.size() > MaxEvents ) return;

	LVector2 Pos = Android_Env->Viewport->TransformPointerPosition( x, y );

	LEventArgs Args( PointerID, Pos.x, Pos.y, pressed );
	Args.FObjArg = (iObject*)Flag;

	Events.push_back( std::pair<LEvent,LEventArgs>( L_EVENT_MOTION, Args ) );
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumEngineActivity_MoveMouse(JNIEnv * env, jobject obj, int x, int y)
{
	if ( !Android_Env ) return;

	Android_Env->Logger->LogP( L_DEBUG, "Mouse moved to: %i, %i", x, y );
	Android_Env->Viewport->MoveMouse( x, y );
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumEngineActivity_SendOptionsMenu( JNIEnv * env, jobject obj, int MenuID )
{
	if ( !Android_Env ) return;

	Android_Env->Logger->LogP( L_DEBUG, "Enquing menu. MenuID: %i", MenuID );

	LMutex Lock( &EventsMutex );

	if ( Events.size() > MaxEvents ) return;

	Events.push_back( std::pair<LEvent,LEventArgs>( L_EVENT_MENU, LEventArgs( MenuID ) ) );
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumEngineActivity_ExitEngine(JNIEnv * env, jobject obj)
{
	if ( !Android_Env ) return;

	Android_Env->Logger->LogP( L_DEBUG, "Requesting exit..." );
	Android_Env->RequestExit();
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumEngineActivity_NativePause(JNIEnv * env, jobject obj)
{
	if ( !Android_Env ) return;

	Android_Env->Logger->LogP( L_DEBUG, "LinderdaumEngineActivity_NativePause(): Paused" );

	Android_Env->SendSync( L_EVENT_ACTIVATE, LEventArgs( false ), false );
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumEngineActivity_NativeResume(JNIEnv * env, jobject obj)
{
	if ( !Android_Env ) return;

	Android_Env->Logger->LogP( L_DEBUG, "LinderdaumEngineActivity_NativeResume(): Resumed" );

	Android_Env->SendSync( L_EVENT_ACTIVATE, LEventArgs( true ), false );
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumSensors_UpdateLinearAcceleration(JNIEnv * env, jobject obj, float ax, float ay, float az)
{
	iInputDeviceList::FLinearAcceleration = LVector3( ax, ay, az );
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumSensors_UpdateVelocity(JNIEnv * env, jobject obj, float vx, float vy, float vz)
{
	iInputDeviceList::FVelocity = LVector3( vx, vy, vz );
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumSensors_UpdateGPS(JNIEnv * env, jobject obj, float Altitude, float Latitude, float Longitude, float Speed, float Bearing, float Time)
{
	iInputDeviceList::FParameters[ clGPSDevice::GPS_ALTITUDE ] = Altitude;
	iInputDeviceList::FParameters[ clGPSDevice::GPS_LATITUDE ] = Latitude;
	iInputDeviceList::FParameters[ clGPSDevice::GPS_LONGITUDE] = Longitude;
	iInputDeviceList::FParameters[ clGPSDevice::GPS_SPEED    ] = Speed;
	iInputDeviceList::FParameters[ clGPSDevice::GPS_BEARING  ] = Bearing;
	iInputDeviceList::FParameters[ clGPSDevice::GPS_TIME     ] = Time;
}

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumJNILib_SurfaceCreated(JNIEnv * env, jobject obj)
{
	// not yet initialized - nothing to refresh
	if ( !Android_Env ) return;

	LMutex Lock( &EventsMutex );

	Events.push_back( std::pair<LEvent,LEventArgs>( L_EVENT_SURFACE_DETACHED, LEventArgs( false ) ) );
	Events.push_back( std::pair<LEvent,LEventArgs>( L_EVENT_SURFACE_ATTACHED, LEventArgs( true  ) ) );
}

// Set to "1" in case of emergency. Use GLDebug_LoadStaticProgramObject() and GLDebug_RenderTriangle() to debug.
#define ANDROID_GLDEBUG 0

#if ANDROID_GLDEBUG
#	include "LAndroid_GLDebug.h"
#endif

JNIEXPORT void JNICALL Java_com_linderdaum_engine_LinderdaumJNILib_step(JNIEnv * env, jobject obj)
{
#if ANDROID_GLDEBUG
	glClearColor( 0.2f, 0.0f, 0.0f, 0.0f );
#endif
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

////
#if ANDROID_GLDEBUG
	GLDebug_LoadStaticProgramObject();
#endif
////

	if ( !Android_Env )
	{
		// application entry point
		Android_Env = android_main( 0, NULL );

		Android_Env->Logger->LogP( L_DEBUG, "Key value = %s", sEnvironment::FKeyValue.c_str() );
	}

	if ( Android_Env )
	{
		// process all enqued keys, unlock the mutex before proceeding
		{
			LMutex Lock( &EventsMutex );

			for ( size_t i = 0; i != Events.size(); i++ )
			{
				Android_Env->SendSync( Events[i].first, Events[i].second, false );
			}

			Events.clear();
		}

		if ( !Android_Env->IsRequestingExit() )
		{
			Android_Env->GenerateTicks();
		}
		else
		{
			Android_Env->Logger->LogP( L_DEBUG, "Exiting from LinderdaumJNILib_step()" );

			LMutex Lock( &EventsMutex );

			Android_Env = NULL;

			// do shutdown logic
			LEngineApplicationShutdown();

			// just kill the app
			exit(0);
		}
	}

///
#if ANDROID_GLDEBUG
	GLDebug_RenderTriangle();
#endif
///
}

} // extern C
