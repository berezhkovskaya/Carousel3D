#include <stdlib.h>
#include <jni.h>

/// global pointer to Java VM (also shared with OpenAL implementation)
JavaVM* javaVM = NULL;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	javaVM = vm;
	return JNI_VERSION_1_2;
}

JNIEnv* GetEnv()
{
	JNIEnv* env = NULL;
	if (javaVM) (*javaVM)->GetEnv( javaVM, (void**)&env, JNI_VERSION_1_2 );
	return env;
}

jmethodID FindJavaStaticMethod( JNIEnv* env, jclass* Class, jmethodID* Method, const char* ClassName, const char* MethodName, const char* MethodSignature )
{
	*Class  = (*env)->FindClass( env, ClassName );
	*Method = (*env)->GetStaticMethodID( env, *Class, MethodName, MethodSignature );
}

#define JAVA_ENTER() JNIEnv* env; JavaEnter( &env ); jclass Class; jmethodID Method;
#define JAVA_LEAVE() JavaLeave( env );

void JavaEnter( JNIEnv** env )
{
	*env = GetEnv();

	(**env)->PushLocalFrame(*env, 4);
}

void JavaLeave( JNIEnv* env )
{
	(*env)->PopLocalFrame( env, NULL );
}

void Android_OpenURL( const char* URL )
{
	JAVA_ENTER();

	jstring jstr = (*env)->NewStringUTF( env, URL );

	FindJavaStaticMethod( env, &Class, &Method, "com/linderdaum/engine/LinderdaumJNILib", "OpenURL", "(Ljava/lang/String;)V" );

	(*env)->CallStaticVoidMethod( env, Class, Method, jstr );
	(*env)->ReleaseStringUTFChars( env, jstr, URL );

	JAVA_LEAVE();
}

void Android_SetWallpaper( const char* ImageFileName )
{
	JAVA_ENTER();
	
	jstring jstr = (*env)->NewStringUTF( env, ImageFileName );

	FindJavaStaticMethod( env, &Class, &Method, "com/linderdaum/engine/LinderdaumJNILib", "SetWallpaper", "(Ljava/lang/String;)V" );

	(*env)->CallStaticVoidMethod( env, Class, Method, jstr );
	
//	(*env)->ReleaseStringUTFChars( env, jstr, ImageFileName );

	JAVA_LEAVE();
}

void Android_OpenImageDialog()
{
	JAVA_ENTER();

	FindJavaStaticMethod( env, &Class, &Method, "com/linderdaum/engine/LinderdaumJNILib", "OpenImageDialog", "()V" );

	(*env)->CallStaticVoidMethod( env, Class, Method );

	JAVA_LEAVE();
}

void Android_EnableLinearAccelerometer( int Active )
{
	JAVA_ENTER();

	FindJavaStaticMethod( env, &Class, &Method, "com/linderdaum/engine/LinderdaumJNILib", "EnableLinearAccelerometer", "(Z)V" );

	(*env)->CallStaticVoidMethod( env, Class, Method, Active );

	JAVA_LEAVE();
}

void Android_EnableGyroscope( int Active )
{
	JAVA_ENTER();

	FindJavaStaticMethod( env, &Class, &Method, "com/linderdaum/engine/LinderdaumJNILib", "EnableGyroscope", "(Z)V" );

	(*env)->CallStaticVoidMethod( env, Class, Method, Active );

	JAVA_LEAVE();
}

void Android_EnableGPS( int Active )
{
	JAVA_ENTER();

	FindJavaStaticMethod( env, &Class, &Method, "com/linderdaum/engine/LinderdaumJNILib", "EnableGPS", "(Z)V" );

	(*env)->CallStaticVoidMethod( env, Class, Method, Active );

	JAVA_LEAVE();
}

void Android_HapticFeedback( float Delay )
{
	JAVA_ENTER();

	FindJavaStaticMethod( env, &Class, &Method, "com/linderdaum/engine/LinderdaumJNILib", "HapticFeedback", "(I)V" );

	int MSec = (int)( Delay * 1000.0f );

	(*env)->CallStaticVoidMethod( env, Class, Method, MSec );

	JAVA_LEAVE();
}

void Android_HapticFeedback_Tap()
{
	JAVA_ENTER();

	FindJavaStaticMethod( env, &Class, &Method, "com/linderdaum/engine/LinderdaumJNILib", "HapticFeedback_Tap", "()V" );

	(*env)->CallStaticVoidMethod( env, Class, Method );

	JAVA_LEAVE();
}

void Android_HapticFeedback_LongPress()
{
	JAVA_ENTER();

	FindJavaStaticMethod( env, &Class, &Method, "com/linderdaum/engine/LinderdaumJNILib", "HapticFeedback_LongPress", "()V" );

	(*env)->CallStaticVoidMethod( env, Class, Method );

	JAVA_LEAVE();
}

#undef JAVA_ENTER
#undef JAVA_LEAVE
