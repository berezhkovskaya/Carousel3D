#include "Environment.h"
#include "Utils/Library.h"
#include "MOD.h"

/// Export code and variables

void InitModPlug();

#if !defined(L_MODPLUG_STATIC_LINK)

ModPlug_Load_func ModPlug_Load_P;
ModPlug_Unload_func ModPlug_Unload_P;
ModPlug_Read_func ModPlug_Read_P;
ModPlug_GetName_func ModPlug_GetName_P;
ModPlug_GetLength_func ModPlug_GetLength_P;
ModPlug_Seek_func ModPlug_Seek_P;
ModPlug_GetSettings_func ModPlug_GetSettings_P;
ModPlug_SetSettings_func ModPlug_SetSettings_P;
ModPlug_GetMasterVolume_func ModPlug_GetMasterVolume_P;
ModPlug_SetMasterVolume_func ModPlug_SetMasterVolume_P;

ModPlug_GetPlayingChannels_func ModPlug_GetPlayingChannels_P;

ModPlug_SeekOrder_func ModPlug_SeekOrder_P;
ModPlug_GetModuleType_func ModPlug_GetModuleType_P;
ModPlug_GetMessage_func ModPlug_GetMessage_P;

ModPlug_NumInstruments_func ModPlug_NumInstruments_P;
ModPlug_NumSamples_func     ModPlug_NumSamples_P;
ModPlug_NumPatterns_func    ModPlug_NumPatterns_P;
ModPlug_NumChannels_func    ModPlug_NumChannels_P;
ModPlug_SampleName_func     ModPlug_SampleName_P;
ModPlug_InstrumentName_func ModPlug_InstrumentName_P;

clLibrary* g_ModPlugLibrary = NULL;

int ModInitialized = 0;

bool LoadModPlug( sEnvironment* Env )
{
	ModInitialized++;

	if ( ModInitialized > 1 ) { return true; }

#if defined( OS_WINDOWS )
	g_ModPlugLibrary = Env->LoadDynamicLibrary( "modplug.dll" );
#elif defined( OS_APPLE )
	g_ModPlugLibrary = Env->LoadDynamicLibrary( "libmodplug.dylib" );
#else
	g_ModPlugLibrary = Env->LoadDynamicLibrary( "libmodplug.so" );
#endif

	if ( !g_ModPlugLibrary ) { return false; }

#define GetModProc(Name) \
   Name##_P = (Name##_func)g_ModPlugLibrary->GetProcAddress(#Name); \
   if(!(Name##_P)) return false;

	GetModProc( ModPlug_Load );
	GetModProc( ModPlug_Unload );
	GetModProc( ModPlug_Read );
	GetModProc( ModPlug_GetName );
	GetModProc( ModPlug_GetLength );
	GetModProc( ModPlug_Seek );
	GetModProc( ModPlug_GetSettings );
	GetModProc( ModPlug_SetSettings );
	GetModProc( ModPlug_GetMasterVolume );
	GetModProc( ModPlug_SetMasterVolume );

	GetModProc( ModPlug_GetPlayingChannels );

	GetModProc( ModPlug_SeekOrder );
	GetModProc( ModPlug_GetModuleType );
	GetModProc( ModPlug_GetMessage );

	GetModProc( ModPlug_NumInstruments );
	GetModProc( ModPlug_NumSamples );
	GetModProc( ModPlug_NumPatterns );
	GetModProc( ModPlug_NumChannels );
	GetModProc( ModPlug_SampleName );
	GetModProc( ModPlug_InstrumentName );

	InitModPlug();

	return true;
}

bool UnloadModPlug()
{
	if ( !g_ModPlugLibrary ) { return true; }

	if ( ModInitialized <= 0 ) { return true; }

	ModInitialized--;

	if ( ModInitialized > 0 ) { return true; }

	delete( g_ModPlugLibrary );

	g_ModPlugLibrary = NULL;

	return true;
}

#else

// static linking does nothing

bool LoadModPlug( sEnvironment* Env ) { InitModPlug(); return true; }
bool UnloadModPlug() { return true; }

#endif // L_MODPLUG_STATIC_LINK - end of dynamic linking code

/// Default modplug options
void InitModPlug()
{
	ModPlug_Settings Settings;
	ModPlug_GetSettings_P( &Settings );

	// all "basic settings" are set before ModPlug_Load.
	Settings.mResamplingMode = MODPLUG_RESAMPLE_FIR; /* RESAMP */
	Settings.mChannels = 2;
	Settings.mBits = 16;
	Settings.mFrequency = 44100;
	Settings.mStereoSeparation = 128;
	Settings.mMaxMixChannels = 256;

	ModPlug_SetSettings_P( &Settings );
}
