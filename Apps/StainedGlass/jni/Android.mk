#
# Part of the Linderdaum Engine
#

LOCAL_PATH := $(call my-dir)

# bringing in libraries

include $(CLEAR_VARS)
LOCAL_MODULE    := libFreeImage
LOCAL_SRC_FILES := ../../../BuildAndroid/jni/$(TARGET_ARCH_ABI)/libFreeImage.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libOGG
LOCAL_SRC_FILES := ../../../BuildAndroid/jni/$(TARGET_ARCH_ABI)/libOGG.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libVorbis
LOCAL_SRC_FILES := ../../../BuildAndroid/jni/$(TARGET_ARCH_ABI)/libVorbis.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libFreeType
LOCAL_SRC_FILES := ../../../BuildAndroid/jni/$(TARGET_ARCH_ABI)/libFreeType.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libOpenAL
LOCAL_SRC_FILES := ../../../BuildAndroid/jni/$(TARGET_ARCH_ABI)/libOpenAL.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libModPlug
LOCAL_SRC_FILES := ../../../BuildAndroid/jni/$(TARGET_ARCH_ABI)/libModPlug.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libLinderdaumEngineCore
LOCAL_SRC_FILES := ../../../BuildAndroid/obj/local/$(TARGET_ARCH_ABI)/libLinderdaumEngineCore.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := LinderdaumEngine

# bringing in Engine stub

LOCAL_SRC_FILES := ../../../BuildAndroid/jni/LAndroid.cpp ../../../BuildAndroid/jni/LJNI.c

# using Engine include folders

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../../Src \
	$(LOCAL_PATH)/../../../Src \
	$(LOCAL_PATH)/../../../Src/ExtLibs \
	$(LOCAL_PATH)/../../../Src/ExtLibs/CL \
	$(LOCAL_PATH)/../../../Src/ExtLibs/DInput \
	$(LOCAL_PATH)/../../../Src/ExtLibs/FMOD \
	$(LOCAL_PATH)/../../../Src/ExtLibs/GL \
	$(LOCAL_PATH)/../../../Src/ExtLibs/OpenAL \
	$(LOCAL_PATH)/../../../Src/ExtLibs/RSC \
	$(LOCAL_PATH)/../../../Src/ExtLibs/Vorbis \
	$(LOCAL_PATH)/../../../Src/ExtLibs/Vorbis/ogg \
	$(LOCAL_PATH)/../../../Src/Generated \
	$(LOCAL_PATH)/../../../Src/Generated/Exports \
	$(LOCAL_PATH)/../../../Src/Generated/Globals \
	$(LOCAL_PATH)/../../../Src/Generated/LAL \
	$(LOCAL_PATH)/../../../Src/Generated/LCL \
	$(LOCAL_PATH)/../../../Src/Generated/LGL \
	$(LOCAL_PATH)/../../../Src/Generated/Serialization \
	$(LOCAL_PATH)/../../../Src/Generated/VM \
	$(LOCAL_PATH)/../../../Src/Linderdaum \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Audio \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Audio/FMOD \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Audio/OpenAL \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Core \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Core/RTTI \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Core/Script \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Core/VFS \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Core/VFS/ML \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Core/VFS/ML/pugi \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Core/VFS \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Core/VM \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Geometry \
	$(LOCAL_PATH)/../../../Src/Linderdaum/GUI \
	$(LOCAL_PATH)/../../../Src/Linderdaum/GUI/ComCtl \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Images \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Input \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Math \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Network \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Physics \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Renderer \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Renderer/CL \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Renderer/DX11 \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Renderer/GL \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Renderer/Soft \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Resources \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Scene \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Scene/Heightmaps \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Scene/Particles \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Scene/Postprocess \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Scene/Terrain \
	$(LOCAL_PATH)/../../../Src/Linderdaum/UnitTests \
	$(LOCAL_PATH)/../../../Src/Linderdaum/UnitTests/Tests \
	$(LOCAL_PATH)/../../../Src/Linderdaum/Utils \
	$(LOCAL_PATH)/../../../Src/Linderdaum/VisualScene \
	$(LOCAL_PATH)/../../../Src/Linderdaum/World \
	$(LOCAL_PATH)/../../../Src/Linderdaum/World/AI

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../C:/LinderdaumSDK/Src \
	$(LOCAL_PATH)/../C:/LinderdaumSDK/Src/Linderdaum \
	$(LOCAL_PATH)/../C:/LinderdaumSDK/Src/ExtLibs \
	$(LOCAL_PATH)/../Src \
	$(LOCAL_PATH)/../Src/com \
	$(LOCAL_PATH)/../Src/com/linderdaum \
	$(LOCAL_PATH)/../Src/com/linderdaum/engine \

LOCAL_SRC_FILES += \
	../Src/StainedGlass.cpp \

#

GLOBAL_CFLAGS   := -mfpu=vfp -mfloat-abi=softfp -O3 -Werror -DANDROID -DDISABLE_IMPORTGL -D_DISABLE_TUNNELLERS_ -D_DISABLE_METHODS_ -DDISABLE_GUARD_UNGUARD_MECHANISM -isystem $(SYSROOT)/usr/include/ 

LOCAL_CFLAGS := $(GLOBAL_CFLAGS)

LOCAL_LDLIBS    += -llog -lGLESv2
#LOCAL_LDLIBS    += -llog -landroid -lEGL -lGLESv2
LOCAL_STATIC_LIBRARIES += LinderdaumEngineCore
LOCAL_STATIC_LIBRARIES += FreeImage
LOCAL_STATIC_LIBRARIES += FreeType
LOCAL_STATIC_LIBRARIES += Vorbis
LOCAL_STATIC_LIBRARIES += OGG
LOCAL_STATIC_LIBRARIES += OpenAL
LOCAL_STATIC_LIBRARIES += ModPlug
#LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)

#-------------
# http://stackoverflow.com/questions/4765465/android-ndk-two-static-libraries-and-linking
#
