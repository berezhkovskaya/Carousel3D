/**
 * \file OGG.cpp
 * \brief OGG library interface
 * \version 0.6.00
 * \date 21/01/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "OGG.h"
#include "Environment.h"
#include "Utils/Exceptions.h"
#include "Core/VFS/iIStream.h"

#if !OGG_DYNAMIC_LINK
// static link
#define OGG_ov_clear           ov_clear
#define OGG_ov_open_callbacks  ov_open_callbacks
#define OGG_ov_time_seek       ov_time_seek
#define OGG_ov_info            ov_info
#define OGG_ov_comment         ov_comment
#define OGG_ov_read            ov_read
#endif // !OGG_DYNAMIC_LINK

#if L_AUDIO_USE_OGG
/*
   If dynamic linking to libVorbis is enabled then we define function prototypes
   Unfortunately, these defines are not porvided in LibVorbis itself
*/
#if OGG_DYNAMIC_LINK
clLibrary* FOGGLibrary = NULL;

ov_clear_func          OGG_ov_clear = NULL;
ov_open_callbacks_func OGG_ov_open_callbacks = NULL;
ov_time_seek_func      OGG_ov_time_seek = NULL;
ov_info_func           OGG_ov_info = NULL;
ov_comment_func        OGG_ov_comment = NULL;
ov_read_func           OGG_ov_read = NULL;

void LinkToOGG( sEnvironment* Env )
{
	// exit if the OGG library is already loaded
	if ( FOGGLibrary ) { return; }

	// _d.dll for DEBUG
	FOGGLibrary = Env->LoadDynamicLibrary( "vorbisfile.dll" );

	FATAL( !FOGGLibrary, "Unable to load in vorbisfile.dll" );

	OGG_ov_read           = ( ov_read_func )FOGGLibrary->GetProcAddress( "ov_read" );
	OGG_ov_info           = ( ov_info_func )FOGGLibrary->GetProcAddress( "ov_info" );
	OGG_ov_comment        = ( ov_comment_func )FOGGLibrary->GetProcAddress( "ov_comment" );
	OGG_ov_time_seek      = ( ov_time_seek_func )FOGGLibrary->GetProcAddress( "ov_time_seek" );
	OGG_ov_open_callbacks = ( ov_open_callbacks_func )FOGGLibrary->GetProcAddress( "ov_open_callbacks" );
	OGG_ov_clear          = ( ov_clear_func )FOGGLibrary->GetProcAddress( "ov_clear" );

	FATAL( !OGG_ov_read, "Unable to link to ov_read in vorbisfile.dll" );
	FATAL( !OGG_ov_info, "Unable to link to ov_info in vorbisfile.dll" );
	FATAL( !OGG_ov_open_callbacks, "Unable to link to ov_open_callbacks in vorbisfile.dll" );
}
#endif // OGG_DYNAMIC_LINK

#endif // L_AUDIO_USE_OGG

void clWaveDataProvider_OGG::ShutdownOGG()
{
#if L_AUDIO_USE_OGG && defined( OGG_DYNAMIC_LINK )
	delete( FOGGLibrary );

	/// Allow reinitalization
	FOGGLibrary = NULL;
#endif // L_AUDIO_USE_OGG && OGG_DYNAMIC_LINK
}

clWaveDataProvider_OGG::clWaveDataProvider_OGG( sEnvironment* E, iIStream* IStream )
{
	Env = E;

#if L_AUDIO_USE_OGG && defined( OGG_DYNAMIC_LINK )
	// try to relink to vorbisfile.dll
	LinkToOGG( Env );
#endif // L_AUDIO_USE_OGG

	FWaveDataSize = static_cast<Lsizei>( IStream->GetFileSize() );
	FOGGRawData = new Lubyte[ FWaveDataSize ];
	FOGGRawPosition = 0;
	FEndOfFile = FOGGRawPosition >= FWaveDataSize;

	IStream->BlockRead( FOGGRawData, IStream->GetFileSize() );

#if L_AUDIO_USE_OGG
	LoadOGG();
#endif // L_AUDIO_USE_OGG
};

clWaveDataProvider_OGG::clWaveDataProvider_OGG( const clWaveDataProvider_OGG* Provider )
{
	Env = Provider->Env;
	FWaveDataSize = Provider->FWaveDataSize;
	FOGGRawData = new Lubyte[ FWaveDataSize ];
	FOGGRawPosition = 0;
	FEndOfFile = Provider->FEndOfFile;

	memcpy( FOGGRawData, Provider->FOGGRawData, FWaveDataSize );

	LoadOGG();
}

void clWaveDataProvider_OGG::LoadOGG()
{
#if L_AUDIO_USE_OGG
	ov_callbacks CallBacks;

	CallBacks.read_func  = OGG_ReadFunc;
	CallBacks.seek_func  = OGG_SeekFunc;
	CallBacks.close_func = OGG_CloseFunc;
	CallBacks.tell_func  = OGG_TellFunc;

	vorbis_comment*   VorbisComment;
	vorbis_info*      VorbisInfo;

	if ( OGG_ov_open_callbacks( this, &FVorbisFile, NULL, -1, CallBacks ) < 0 )
	{
		FATAL_MSG( "Invalid OGG file" );
	}

	VorbisComment = OGG_ov_comment( &FVorbisFile, -1 );
	VorbisInfo    = OGG_ov_info ( &FVorbisFile, -1 );

	FWaveDataFormat.FChannels      = VorbisInfo->channels;
	FWaveDataFormat.FSamplesPerSec = VorbisInfo->rate;
	FWaveDataFormat.FBitsPerSample = 16;
#endif // L_AUDIO_USE_OGG
}

/*
 * 21/01/2011
     It's here
*/
