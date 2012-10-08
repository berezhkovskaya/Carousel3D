#ifndef _LJNI_
#define _LJNI_

#include "Platform.h"

#if defined( OS_ANDROID )

extern "C"
{
	void Android_OpenURL( const char* URL );
	void Android_SetWallpaper( const char* ImageFileName );
	void Android_OpenImageDialog();

#pragma region Sensors
	void Android_EnableLinearAccelerometer( bool Active );
	void Android_EnableGyroscope( bool Active );
	void Android_EnableGPS( bool Active );
#pragma endregion

#pragma region Force feedback
	void Android_HapticFeedback( float Delay );
	void Android_HapticFeedback_Tap();
	void Android_HapticFeedback_LongPress();
#pragma endregion
};

#endif // OS_ANDROID

#endif
