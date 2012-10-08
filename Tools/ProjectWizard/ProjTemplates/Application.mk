#
APP_OPTIM := release
APP_PLATFORM := android-7
APP_STL := gnustl_static
APP_CPPFLAGS += -frtti 
APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -DANDROID
# native activity in android-9
# APP_CPPFLAGS += -DANDROID9
APP_ABI := armeabi-v7a