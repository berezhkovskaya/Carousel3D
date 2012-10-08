/**
 * \file WAV.inl
 * \brief .WAV loader
 * \version 0.5.93
 * \date 28/09/2010
 * \author Viktor Latypov, 2009
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __WAV_inl__
#define __WAV_inl__

#include "Core/Logger.h"
#include "Audio/Audio.h"

/// .WAV file data provider for audio streaming
class scriptfinal clWaveDataProvider_WAV: public iWaveDataProvider
{
public:
	clWaveDataProvider_WAV( sEnvironment* Env, iIStream* IStream )
	{
		sWAVHeader Header;

		FATAL( sizeof( Header ) > IStream->GetFileSize(), "Invalid WAV header" );

		IStream->BlockRead( &Header, sizeof( Header ) );

		FATAL( ( Header.RIFF[0] != 'R' ) ||
		       ( Header.RIFF[1] != 'I' ) ||
		       ( Header.RIFF[2] != 'F' ) ||
		       ( Header.RIFF[3] != 'F' ), "Can only read RIFF files" );

		const unsigned short FORMAT_PCM = 1;

		FATAL( Header.FormatTag != FORMAT_PCM, "Can only read PCM wave files" );

		Env->Logger->Log( L_NOTICE, "Channels       : " + LStr::ToStr( Header.Channels       ) );
		Env->Logger->Log( L_NOTICE, "Bits per sample: " + LStr::ToStr( Header.nBitsperSample ) );
		Env->Logger->Log( L_NOTICE, "Sample rate    : " + LStr::ToStr( ( int )Header.SampleRate     ) );

		FSample = ( Lubyte* )malloc( Header.DataSize );

		FATAL( Header.DataSize > IStream->GetFileSize() - sizeof( Header ), "Invalid WAV file" );

		IStream->BlockRead( FSample, Header.DataSize );

		FWaveDataFormat.FChannels      = Header.Channels;
		FWaveDataFormat.FSamplesPerSec = Header.SampleRate;
		FWaveDataFormat.FBitsPerSample = Header.nBitsperSample;

		FWaveDataSize = Header.DataSize;
	}
	explicit clWaveDataProvider_WAV( const clWaveDataProvider_WAV* Provider )
	{
		FSample = ( Lubyte* )malloc( Provider->FWaveDataSize );

		memcpy( FSample, Provider->FSample, Provider->FWaveDataSize );

		FWaveDataFormat = Provider->FWaveDataFormat;
		FWaveDataSize   = Provider->FWaveDataSize;
	}
	virtual ~clWaveDataProvider_WAV()
	{
		free( FSample );
	}
	//
	// iWaveDataProvider interface
	//
	virtual iWaveDataProvider*     Clone() const
	{
		iWaveDataProvider* Provider = new clWaveDataProvider_WAV( this );

		Provider->Env = Env;
		Provider->AfterConstruction();

		return Provider;
	}
	virtual bool                   IsStreaming() const
	{
		return false;
	}
	virtual bool                   IsEOF() const
	{
		return true;
	}
	virtual sWaveDataFormat        GetWaveDataFormat() const
	{
		return FWaveDataFormat;
	}
	virtual Lubyte*                GetWaveData()
	{
		return FSample;
	}
	virtual Lsizei                 GetWaveDataSize() const
	{
		return FWaveDataSize;
	}
	virtual int     StreamWaveData( int Size )
	{
		return 0;
	}
	virtual void    Seek( float Time )
	{
		// nothing to do
	}
private:
	Lubyte*            FSample;
	sWaveDataFormat    FWaveDataFormat;
	Lsizei             FWaveDataSize;
private:
#pragma pack(push, 1)
	struct GCC_PACK( 1 ) sWAVHeader
	{
	   unsigned char    RIFF[4];
	   unsigned long    Size;            // forget this
	   unsigned char    WAVE[4];
	   unsigned char    FMT[4];
	   unsigned long    SizeFmt;
	   unsigned short   FormatTag;       // 1=PCM
	   unsigned short   Channels;        // 1=Mono, 2=Stereo
	   unsigned long    SampleRate;
	   unsigned long    AvgBytesPerSec;  // usually same as Sp.Rate
	   unsigned short   nBlockAlign;     // too lazy to look it up
	   unsigned short   nBitsperSample;  // 8 or 16
	   unsigned char    dat_[4];         // forget this
	   unsigned long    DataSize;        // size of the sample data
	};
#pragma pack(pop)
};

#endif

/*
 * 28/09/2010
     Added WAV header dumping
 * 16/05/2009
     It's here
*/
