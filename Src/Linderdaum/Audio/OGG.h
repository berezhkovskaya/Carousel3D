/**
 * \file OGG.inl
 * \brief OGG library interface
 * \version 0.5.89a
 * \date 16/05/2009
 * \author Viktor Latypov, 2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __OGG_inl__
#define __OGG_inl__

#include "Platform.h"

#if L_AUDIO_USE_OGG
#	include "ExtLibs/Vorbis/codec.h"
#	include "ExtLibs/Vorbis/vorbisfile.h"
#endif // L_AUDIO_USE_OGG

#include "Utils/Library.h"
#include "Audio/Audio.h"

// we support both dynamic and static_ linking
#define OGG_DYNAMIC_LINK 1

#if defined( OS_ANDROID )
#  undef OGG_DYNAMIC_LINK
#endif // OS_ANDROID


#if !OGG_DYNAMIC_LINK
// static link
#  define OGG_ov_clear           ov_clear
#  define OGG_ov_open_callbacks  ov_open_callbacks
#  define OGG_ov_time_seek       ov_time_seek
#  define OGG_ov_info            ov_info
#  define OGG_ov_comment         ov_comment
#  define OGG_ov_read            ov_read
#endif

#if L_AUDIO_USE_OGG
/*
   If dynamic linking to libVorbis is enabled then we define function prototypes
   Unfortunately, these defines are not porvided in LibVorbis itself
*/
#if OGG_DYNAMIC_LINK
typedef int  ( __cdecl* ov_clear_func )( OggVorbis_File* vf );
typedef int  ( __cdecl* ov_open_callbacks_func )( void* datasource, OggVorbis_File* vf, char* initial, long ibytes, ov_callbacks callbacks );
typedef int  ( __cdecl* ov_time_seek_func )( OggVorbis_File* vf, double pos );
typedef long ( __cdecl* ov_read_func )( OggVorbis_File* vf, char* buffer, int length, int bigendianp, int word, int sgned, int* bitstream );

typedef vorbis_info *( __cdecl* ov_info_func )( OggVorbis_File* vf, int link );
typedef vorbis_comment *( __cdecl* ov_comment_func )( OggVorbis_File* vf, int link );

extern clLibrary* FOGGLibrary;

extern ov_clear_func          OGG_ov_clear;
extern ov_open_callbacks_func OGG_ov_open_callbacks;
extern ov_time_seek_func      OGG_ov_time_seek;
extern ov_info_func           OGG_ov_info;
extern ov_comment_func        OGG_ov_comment;
extern ov_read_func           OGG_ov_read;
#endif // OGG_DYNAMIC_LINK

#endif // L_AUDIO_USE_OGG

class iIStream;

/// .OGG file data provider for audio streaming
class scriptfinal clWaveDataProvider_OGG: public iWaveDataProvider
{
public:
	static void ShutdownOGG();
public:
	clWaveDataProvider_OGG( sEnvironment* E, iIStream* IStream );
	explicit clWaveDataProvider_OGG( const clWaveDataProvider_OGG* Provider );
	void LoadOGG();
	virtual ~clWaveDataProvider_OGG()
	{
#if L_AUDIO_USE_OGG
		OGG_ov_clear( &FVorbisFile );
#endif

		delete[] FOGGRawData;
	}
	//
	// iWaveDataProvider interface
	//
	virtual iWaveDataProvider*     Clone() const
	{
		iWaveDataProvider* Provider = new clWaveDataProvider_OGG( this );

		Provider->Env = Env;
		Provider->AfterConstruction();

		return Provider;
	}
	virtual bool                   IsStreaming() const
	{
		return true;
	}
	virtual bool                   IsEOF() const
	{
		return FEndOfFile;
	}
	virtual sWaveDataFormat        GetWaveDataFormat() const
	{
		return FWaveDataFormat;
	}
	virtual Lubyte*                GetWaveData()
	{
		return reinterpret_cast<Lubyte*>( &FDecodingBuffer[0] );
	}
	virtual Lsizei                 GetWaveDataSize() const
	{
		return FDecodingBufferUsed;
	}
	virtual int                    StreamWaveData( int Size )
	{
		if ( Size > static_cast<int>( FDecodingBuffer.size() ) )
		{
			FDecodingBuffer.resize( Size );
		}

		long BytesRead = 0;

#if L_AUDIO_USE_OGG

		while ( BytesRead < Size )
		{
			long Ret = OGG_ov_read( &FVorbisFile,
			                        &FDecodingBuffer[0] + BytesRead,
			                        Size - BytesRead,
			                        0, // LITTLE_ENDIAN,
			                        FWaveDataFormat.FBitsPerSample >> 3,
			                        1,
			                        &FOGGCurrentSection );

			if ( Ret == 0 )
			{
				FEndOfFile = true;
				break;
			}

			if ( Ret < 0 )
			{
				// error in bitstream
			}
			else
			{
				BytesRead += Ret;
			}
		}

#endif // L_AUDIO_USE_OGG

		FDecodingBufferUsed = BytesRead;

		return FDecodingBufferUsed;
	}
	virtual void    Seek( float Time )
	{
		FEndOfFile = false;
#if L_AUDIO_USE_OGG
		OGG_ov_time_seek( &FVorbisFile, Time );
#endif // L_AUDIO_USE_OGG
	}
private:
#if L_AUDIO_USE_OGG
	static size_t OGG_ReadFunc( void* Ptr, size_t Size, size_t NMemB, void* DataSource )
	{
		clWaveDataProvider_OGG* OGG = static_cast<clWaveDataProvider_OGG*>( DataSource );

		Lsizei BytesRead = OGG->FWaveDataSize - OGG->FOGGRawPosition;
		Lsizei BytesSize = static_cast<Lsizei>( Size * NMemB );

		if ( BytesSize < BytesRead )
		{
			BytesRead = BytesSize;
		}

		memcpy( Ptr, OGG->FOGGRawData + OGG->FOGGRawPosition, BytesRead );

		OGG->FOGGRawPosition += BytesRead;

		return BytesRead;
	}
	static int OGG_SeekFunc( void* DataSource, ogg_int64_t Offset, int Whence )
	{
		clWaveDataProvider_OGG* OGG = static_cast<clWaveDataProvider_OGG*>( DataSource );

		if ( Whence == SEEK_SET )
		{
			OGG->FOGGRawPosition = static_cast<Lsizei>( Offset );
		}
		else if ( Whence == SEEK_CUR )
		{
			OGG->FOGGRawPosition += static_cast<Lsizei>( Offset );
		}
		else if ( Whence == SEEK_END )
		{
			OGG->FOGGRawPosition = OGG->FWaveDataSize + static_cast<Lsizei>( Offset );
		}

		if ( OGG->FOGGRawPosition > OGG->FWaveDataSize )
		{
			OGG->FOGGRawPosition = OGG->FWaveDataSize;
		}

		if ( OGG->FOGGRawPosition < 0                  )
		{
			OGG->FOGGRawPosition = 0;
		}

		return OGG->FOGGRawPosition;
	}
	static int OGG_CloseFunc( void* DataSource )
	{
		return 0;
	}
	static long OGG_TellFunc( void* DataSource )
	{
		return static_cast<clWaveDataProvider_OGG*>( DataSource )->FOGGRawPosition;
	}
#endif // L_AUDIO_USE_OGG
private:
#if L_AUDIO_USE_OGG
	OggVorbis_File         FVorbisFile;
#endif // L_AUDIO_USE_OGG
	sWaveDataFormat        FWaveDataFormat;
	Lsizei                 FWaveDataSize;
	Lubyte*                FOGGRawData;
	Lsizei                 FOGGRawPosition;
	int                    FOGGCurrentSection;
	std::vector<char>      FDecodingBuffer;
	int                    FDecodingBufferUsed;
	bool                   FEndOfFile;
};

//#endif

#endif

/*
 * 16/05/2009
     It's here
*/
