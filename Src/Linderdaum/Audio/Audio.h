/**
 * \file Audio.h
 * \brief Classes for audio subsystem
 * \version 0.5.93
 * \date 23/10/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __LAudio__h__included__
#define __LAudio__h__included__

#include "Environment.h"

#include "Math/LVector.h"
#include "Math/LMath.h"

#include "Resources/iResource.h"

class iWaveform;
class clAnnouncer;

class iIStream;
class iWaveDataProvider;

/// Generic holder of waveform data for audiosubsytem
class scriptfinal netexportable iWaveform: public iResource
{
public:
	iWaveform();
	virtual ~iWaveform();
	//
	// iCacheable interface
	//
	virtual LString    GetCachingDir() const;
	//
	// iWaveform
	//
	virtual void                   AttachWaveDataProvider( iWaveDataProvider* WaveDataProvider );
	virtual iWaveDataProvider*     CreateWaveDataProvider() const;
	virtual iWaveDataProvider*     GetWaveDataProvider() const;
protected:
	virtual bool                   LoadFromFile( const LString& FileName );
private:
	iWaveDataProvider*    FWaveDataProvider;
	iIStream*             FStream;
};

/// Format of waveform data
struct sWaveDataFormat
{
	sWaveDataFormat(): FChannels( 0 ),
		FSamplesPerSec( 0 ),
		FBitsPerSample( 0 ) {};
	int    FChannels;
	int    FSamplesPerSec;
	int    FBitsPerSample;
};

/// Provider of waveform data for streaming
class scriptfinal netexportable iWaveDataProvider: public iObject
{
public:
	iWaveDataProvider() {};
	//
	// iWaveDataProvider
	//
	virtual iWaveDataProvider*     Clone() const = 0;
	virtual bool                   IsStreaming() const = 0;
	virtual bool                   IsEOF() const = 0;
	virtual sWaveDataFormat        GetWaveDataFormat() const = 0;
	virtual Lubyte*                GetWaveData() = 0;
	virtual Lsizei                 GetWaveDataSize() const = 0;
	virtual int                    StreamWaveData( int Size ) = 0;
	virtual void                   Seek( float Time ) = 0;
};

/// Audio source interface, also directly used in silent mode
class scriptfinal netexportable iAudioSource: public iObject
{
public:
	iAudioSource(): FVolume( 1.0f ),
		FFadeVolume( 1.0f ) {};
	//
	// iAudioSource
	//
	virtual void    Play() {};
	virtual void    Stop() {};
	virtual void    Pause() {};
	virtual void    LoopSound( const bool Loop ) {};
	virtual bool    IsPlaying() const { return false; };
	virtual void    Update( float DeltaSeconds );
	virtual void    SetPosition( const LVector3& Position ) {};
	virtual void    SetVelocity( const LVector3& Velocity ) {};
	virtual void    SetDirection( const LVector3& Direction ) {};

	/// Set desired fade volume level
	virtual void    SetFadeVolume( float Volume ) { FFadeVolume = Volume; }

	/// Get current desired fade volume level
	virtual float   GetFadeVolume() const { return FFadeVolume; }

	/// Set current volume and reset fading effect
	virtual void    SetVolume( float Volume ) { FVolume = Volume; FFadeVolume = Volume; };

	/// Set current volume (fade will continue to the desired level)
	virtual void    SetVolumeNoFade( float Volume ) { FVolume = Volume; };

	/// Get current volume
	virtual float   GetVolume() const { return FVolume; };

	virtual void    SetPitch( float Pitch ) {};
	virtual void    BindWaveform( iWaveform* Waveform ) {};

	/// Get current waveform for adjustment
	virtual iWaveform* GetWaveform() const { return NULL; }
private:
	float    FVolume;
	float    FFadeVolume;
};

/// Audio interface with dummy implementation of silent mode
class scriptfinal netexportable iAudioSubSystem: public iObject
{
public:
	iAudioSubSystem();
	virtual ~iAudioSubSystem();

	//
	// iObject interface
	//
	virtual void    AfterConstruction();
	//
	// iAudioSubSystem
	//
	virtual iAudioSource*    Announce( const LString& FileName );
	virtual iAudioSource*    Announce3D( iAudioSource* Source );
	virtual iWaveform*       CreateWaveform() const;
	virtual iAudioSource*    CreateSourceFromFile( const LString& FileName ) const;
	virtual iAudioSource*    CreateSource() const;
	virtual void             SetListenerPosition( const LVector3& Position ) {};
	virtual void             SetListenerVelocity( const LVector3& Velocity ) {};
	virtual void             SetListenerOrientation( const LVector3& Forward, const LVector3& Up ) {};
	virtual void             SetListenerVolume( float Volume ) {};
	virtual void             StopAll() {};
	virtual void             PauseAll() {};
	virtual void             ResumeAll() {};
	virtual void             ToggleAll() {};
	virtual bool             IsActive() const { return true; };
	// events
	FWD_EVENT_HANDLER( Event_TIMER );
private:
	typedef std::list<clAnnouncer*> clAnnouncersList;
protected:
	clAnnouncersList    FAnnouncers;
	iAudioSource*       FModule;
	clCVar*             FModuleVolume;
protected:
	// commands handlers
	virtual void    PlayModuleC( const LString& Param );
	virtual void    StopModuleC( const LString& Param );
	virtual void    ModuleVolumeC( const LString& Param );
	virtual void    ModuleFadeOutC( const LString& Param );
	virtual void    ModuleFadeInC( const LString& Param );
	virtual void    AnnounceC( const LString& Param );
	virtual void    StopAllC( const LString& Param ) {};
};

/// Automatic lifetime management for a sound (play once and remove). Used internally
class scriptfinal clAnnouncer: public iObject
{
public:
	clAnnouncer( const LString& FileName,
	             iAudioSource* Source ): FFileName( FileName ),
		FAudioSource( Source ) {};
	virtual ~clAnnouncer()
	{
		delete( FAudioSource );
	}

	virtual void    AfterConstruction()
	{
		if ( !FAudioSource )
		{
			FAudioSource = Env->Audio->CreateSourceFromFile( FFileName );
		}

		FAudioSource->Play();
	}

	virtual bool    IsFinished() const
	{
		return !FAudioSource->IsPlaying();
	}

	virtual iAudioSource*   GetSource() const { return FAudioSource; };
private:
	LString          FFileName;
	iAudioSource*    FAudioSource;
};

/// Streaming monotone wave generator
class scriptfinal clToneGenerator : public iWaveDataProvider
{
public:
	clToneGenerator(): FDecodingBufferUsed( 100000 ), FDecodingBuffer( 100000 )
	{
		LocalFormat.FChannels = 2;
		LocalFormat.FSamplesPerSec = FSignalFreq = 44100;
		LocalFormat.FBitsPerSample = 16;

		FAmplitude = 350.0f;
		FFrequency = 440.0f;
	}

	virtual ~clToneGenerator() {}
	//
	// iWaveDataProvider
	//
	virtual iWaveDataProvider*     Clone() const { return new clToneGenerator(); }
	virtual bool                   IsStreaming() const { return true; }
	virtual bool                   IsEOF() const { return true; }

	virtual sWaveDataFormat        GetWaveDataFormat() const
	{
		return LocalFormat;
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
			LastOffset = 0;
		}

		for ( int i = 0 ; i < Size / 4 ; i++ )
		{

			if ( FSignal )
			{
				float t = ( 2.0f * Math::PI * FFrequency * ( i + LastOffset ) ) / static_cast<float>( FSignalFreq );

				float _val = FAmplitude * sin( t );

				short VV = static_cast<short>( _val );

				FDecodingBuffer[i * 4 + 0] = VV & 0xFF; //static_cast<char>(_val);
				FDecodingBuffer[i * 4 + 1] = VV >> 8;
				FDecodingBuffer[i * 4 + 2] = VV & 0xFF; //static_cast<char>(_val);
				FDecodingBuffer[i * 4 + 3] = VV >> 8;
			}
			else
			{
				for ( int j = 0 ; j < 4 ; j++ ) { FDecodingBuffer[i * 4 + j] = 0x0; }
			}
		}

		LastOffset += Size / 2;
		LastOffset %= FSignalFreq;

		FDecodingBufferUsed = Size;
		return Size;
	}

	virtual void                   Seek( float Time ) {}

	int  FSignalFreq;
	bool FSignal;

	float FFrequency;
	float FAmplitude;

private:
	int LastOffset;

	sWaveDataFormat LocalFormat;
	std::vector<char>      FDecodingBuffer;
	int                    FDecodingBufferUsed;
};


#endif
