#ifndef __Mod__h__included__
#define __Mod__h__included__

#include "ExtLibs/ModPlug/modplug.h"

#include "Audio/Audio.h"
#include "Core/VFS/iIStream.h"

#undef L_MODPLUG_STATIC_LINK

#if defined(ANDROID)

#  define L_MODPLUG_STATIC_LINK

#endif

#if defined(L_MODPLUG_STATIC_LINK)

/// Static link on this platfrom, so we just redefine the function names (use "decapitalized" ones)

#define ModPlug_Load_P          ModPlug_Load
#define ModPlug_Unload_P        ModPlug_Unload
#define ModPlug_Read_P          ModPlug_Read
#define ModPlug_GetName_P       ModPlug_GetName
#define ModPlug_GetLength_P     ModPlug_GetLength
#define ModPlug_Seek_P          ModPlug_Seek
#define ModPlug_GetSettings_P   ModPlug_GetSettings
#define ModPlug_SetSettings_P   ModPlug_SetSettings
#define ModPlug_GetMasterVolume_P ModPlug_GetMasterVolume
#define ModPlug_SetMasterVolume_P ModPlug_SetMasterVolume

#define ModPlug_GetPlayingChannels_P ModPlug_GetPlayingChannels

#define ModPlug_SeekOrder_P      ModPlug_SeekOrder
#define ModPlug_GetModuleType_P  ModPlug_GetModuleType
#define ModPlug_GetMessage_P     ModPlug_GetMessage

#define ModPlug_NumInstruments_P ModPlug_NumInstruments
#define ModPlug_NumSamples_P     ModPlug_NumSamples
#define ModPlug_NumPatterns_P    ModPlug_NumPatterns
#define ModPlug_NumChannels_P    ModPlug_NumChannels
#define ModPlug_SampleName_P     ModPlug_SampleName
#define ModPlug_InstrumentName_P ModPlug_InstrumentName

#else

//////// Function prototypes

typedef ModPlugFile* ( *ModPlug_Load_func )( const void* data, int size );
typedef void ( *ModPlug_Unload_func )( ModPlugFile* file );
typedef int  ( *ModPlug_Read_func )( ModPlugFile* file, void* buffer, int size );
typedef const char* ( *ModPlug_GetName_func )( ModPlugFile* file );
typedef int  ( *ModPlug_GetLength_func )( ModPlugFile* file );
typedef void ( *ModPlug_Seek_func )( ModPlugFile* file, int millisecond );
typedef void ( *ModPlug_GetSettings_func )( ModPlug_Settings* settings );
typedef void ( *ModPlug_SetSettings_func )( const ModPlug_Settings* settings );
typedef unsigned int ( *ModPlug_GetMasterVolume_func )( ModPlugFile* file ) ;
typedef void ( *ModPlug_SetMasterVolume_func )( ModPlugFile* file, unsigned int cvol ) ;

typedef int ( *ModPlug_GetPlayingChannels_func )( ModPlugFile* file );

typedef void  ( *ModPlug_SeekOrder_func )( ModPlugFile* file, int order );
typedef int   ( *ModPlug_GetModuleType_func )( ModPlugFile* file );
typedef char* ( *ModPlug_GetMessage_func )( ModPlugFile* file );

typedef unsigned int ( *ModPlug_NumInstruments_func )( ModPlugFile* file );
typedef unsigned int ( *ModPlug_NumSamples_func )( ModPlugFile* file );
typedef unsigned int ( *ModPlug_NumPatterns_func )( ModPlugFile* file );
typedef unsigned int ( *ModPlug_NumChannels_func )( ModPlugFile* file );
typedef unsigned int ( *ModPlug_SampleName_func )( ModPlugFile* file, unsigned int qual, char* buff );
typedef unsigned int ( *ModPlug_InstrumentName_func )( ModPlugFile* file, unsigned int qual, char* buff );

typedef void ( *ModPlug_InitMixerCallback_func )( ModPlugFile* file, ModPlugMixerProc proc ) ;
typedef void ( *ModPlug_UnloadMixerCallback_func )( ModPlugFile* file ) ;

//////// end of function prototypes

extern ModPlug_Load_func ModPlug_Load_P;
extern ModPlug_Unload_func ModPlug_Unload_P;
extern ModPlug_Read_func ModPlug_Read_P;
extern ModPlug_GetName_func ModPlug_GetName_P;
extern ModPlug_GetLength_func ModPlug_GetLength_P;
extern ModPlug_Seek_func ModPlug_Seek_P;
extern ModPlug_GetSettings_func ModPlug_GetSettings_P;
extern ModPlug_SetSettings_func ModPlug_SetSettings_P;
extern ModPlug_GetMasterVolume_func ModPlug_GetMasterVolume_P;
extern ModPlug_SetMasterVolume_func ModPlug_SetMasterVolume_P;

extern ModPlug_GetPlayingChannels_func ModPlug_GetPlayingChannels_P;

extern ModPlug_SeekOrder_func ModPlug_SeekOrder_P;
extern ModPlug_GetModuleType_func ModPlug_GetModuleType_P;
extern ModPlug_GetMessage_func ModPlug_GetMessage_P;

extern ModPlug_NumInstruments_func ModPlug_NumInstruments_P;
extern ModPlug_NumSamples_func     ModPlug_NumSamples_P;
extern ModPlug_NumPatterns_func    ModPlug_NumPatterns_P;
extern ModPlug_NumChannels_func    ModPlug_NumChannels_P;
extern ModPlug_SampleName_func     ModPlug_SampleName_P;
extern ModPlug_InstrumentName_func ModPlug_InstrumentName_P;

// internal counter (how much times was the modplug initialized)
extern int ModInitialized;

#endif // not OS_ANDROID, dynamic linking code

bool LoadModPlug( sEnvironment* Env );
bool UnloadModPlug();

//// Now the WaveProvider itself

/**
   \brief MOD/XM/IT streamer based on ModPlug library

   File stream is managed outside this class.
   Even more, the iStream here is really unnecessary. All we need is a simple buffer/size parameters
*/
class scriptfinal clModPlugProvider: public iWaveDataProvider
{
public:
	clModPlugProvider( const clModPlugProvider* Other )
	{
		FLoop = Other->FLoop;
		FEof = Other->FEof;
		FModFile = NULL; // Other->FModFile;
		LocalFormat = Other->LocalFormat;

		Env = Other->Env;

		LoadModPlug( Env );

		FSrcStream = Other->FSrcStream;
		StartFile( FSrcStream );
	}

	clModPlugProvider( sEnvironment* E, iIStream* InStream )
	{
		Env = E;
		LoadModPlug( Env );
		SetDefaults();
		StartFile( InStream );
	}

	void SetDefaults()
	{
		FLoop = false;
		FEof = true;

		LocalFormat.FChannels = 2;
		LocalFormat.FSamplesPerSec = 44100;
		LocalFormat.FBitsPerSample = 16;
	}

	mutable ModPlugFile* FModFile;

	virtual void StartFile( iIStream* In ) { FSrcStream = In; FModFile = ModPlug_Load_P( ( const void* )In->MapStream(), ( int )In->GetFileSize() ); FEof = false; }

	virtual void Stop() { if ( FModFile ) { ModPlug_Unload_P( FModFile ); FModFile = NULL; } }

	virtual ~clModPlugProvider() { Stop(); UnloadModPlug(); }
	virtual iWaveDataProvider*     Clone() const { /*FModFile = NULL;*/ return new clModPlugProvider( this ); }

	virtual bool                   IsStreaming() const { return true; }
	virtual bool                   IsEOF() const { return FEof; }

	virtual sWaveDataFormat        GetWaveDataFormat() const { return LocalFormat; }

	virtual Lubyte* GetWaveData() { return reinterpret_cast<Lubyte*>( &FDecodingBuffer[0] ); }

	virtual Lsizei GetWaveDataSize() const { return FDecodingBufferUsed; }

	virtual int StreamWaveData( int Size )
	{
		if ( Size > static_cast<int>( FDecodingBuffer.size() ) ) { FDecodingBuffer.resize( Size ); }

		if ( FEof ) { return 0; }

		int BytesRead = 0;

		while ( BytesRead < Size )
		{
			int mlen = ModPlug_Read_P( FModFile, &FDecodingBuffer[0] + BytesRead, Size - BytesRead );

			if ( mlen > 0 )
			{
				BytesRead += mlen;
			}
			else if ( mlen == 0 )
			{
				FEof = true;

				if ( FLoop )
				{
					Seek( 0 ); // rewind to the beginning
					FEof = false;
					continue;
				}

				break;
			}
			else
			{
				Seek( 0 );
				FEof = true;
				break; // some error
			}
		}

		FDecodingBufferUsed = BytesRead; //Size;
		return BytesRead; // Size;
	}

	/// TODO : keep track of the current playing pos
	virtual void Seek( float Time )
	{
		// convert to msecs
		int ThePos = ( int )( Time * 1000.0f );
		ModPlug_Seek_P( FModFile, ThePos );
		FEof = false;
	}

	iIStream* FSrcStream;

	bool FLoop;

	bool FEof;

public:
//	int LastOffset;
	char TmpBuf[4096];

	sWaveDataFormat LocalFormat;
	std::vector<char> FDecodingBuffer;
	int               FDecodingBufferUsed;
};

#endif
