/**
 * \file Audio_OpenAL.cpp
 * \brief OpenAL audio system
 * \version 0.5.73
 * \date 14/03/2007
 * \author Sergey Kosarevsky, 2005-2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Engine.h"
#include "Audio/Audio_OpenAL.h"

#include "Audio/OGG.h"

#include "Core/Linker.h"
#include "Core/Console.h"
#include "Core/Logger.h"
#include "Core/VFS/FileSystem.h"

#include "Resources/ResourcesManager.h"

#include <algorithm>
#include <functional>

const int BUFFER_SIZE = 2 * 262144;

#if L_AUDIO_USE_OPENAL
#	include "Generated/LAL/LAL.h"
#endif

clAudioSource_OpenAL::clAudioSource_OpenAL()
#if L_AUDIO_USE_OPENAL
	: FSourceID( 0 ),
	  FFormat( 0 ),
	  FBuffersCount( 0 ),
	  FWaveDataProvider( NULL ),
	  FLooping( false )
#endif
{
}

void clAudioSource_OpenAL::AfterConstruction()
{
#if L_AUDIO_USE_OPENAL
	alGenSources( 1, &FSourceID );

	alSourcef( FSourceID, AL_PITCH,   1.0 );
	alSourcef( FSourceID, AL_GAIN,    1.0 );
	alSourcei( FSourceID, AL_LOOPING, 0   );

	alSourcef( FSourceID, AL_REFERENCE_DISTANCE, 0.0 );
#endif
	dynamic_cast<clAudioSubSystem_OpenAL*>( Env->Audio )->RegisterSource( this );
}

clAudioSource_OpenAL::~clAudioSource_OpenAL()
{
#if L_AUDIO_USE_OPENAL
	//alSourceStop( FSourceID );
	Stop();

	DetachProvider();

	alDeleteSources( 1, &FSourceID );
	alDeleteBuffers( FBuffersCount, &FBufferID[0] );
#endif
	dynamic_cast<clAudioSubSystem_OpenAL*>( Env->Audio )->UnRegisterSource( this );
}

void clAudioSource_OpenAL::DetachProvider()
{
#if L_AUDIO_USE_OPENAL

	if ( FWaveDataProvider )
	{
		if ( FWaveDataProvider->IsStreaming() ) { delete( FWaveDataProvider ); }

		FWaveDataProvider = NULL;
	}

#endif
}

void clAudioSource_OpenAL::SetPosition( const LVector3& Position )
{
#if L_AUDIO_USE_OPENAL
	alSourcef( FSourceID, AL_REFERENCE_DISTANCE, 1.0 );

	alSourcefv( FSourceID, AL_POSITION, Position.ToFloatPtr() );
#endif
}

void clAudioSource_OpenAL::SetVelocity( const LVector3& Velocity )
{
#if L_AUDIO_USE_OPENAL
	alSourcef( FSourceID, AL_REFERENCE_DISTANCE, 1.0 );

	alSourcefv( FSourceID, AL_VELOCITY, Velocity.ToFloatPtr() );
#endif
}

void clAudioSource_OpenAL::SetDirection( const LVector3& Direction )
{
#if L_AUDIO_USE_OPENAL
	alSourcef( FSourceID, AL_REFERENCE_DISTANCE, 1.0 );

	alSourcefv( FSourceID, AL_DIRECTION, Direction.ToFloatPtr() );
#endif
}

void clAudioSource_OpenAL::SetVolumeNoFade( float Volume )
{
	iAudioSource::SetVolumeNoFade( Volume );

#if L_AUDIO_USE_OPENAL
	alSourcef( FSourceID, AL_GAIN, Volume );
#endif
}

void clAudioSource_OpenAL::SetVolume( float Volume )
{
	iAudioSource::SetVolume( Volume );

#if L_AUDIO_USE_OPENAL
	alSourcef( FSourceID, AL_GAIN, Volume );
#endif
}

void clAudioSource_OpenAL::SetPitch( float Pitch )
{
#if L_AUDIO_USE_OPENAL
	alSourcef( FSourceID, AL_PITCH, Pitch );
#endif
}

void clAudioSource_OpenAL::BindWaveform( iWaveform* Waveform )
{
#if L_AUDIO_USE_OPENAL
	DetachProvider();

	if ( !Waveform->GetWaveDataProvider() )
	{
		return;
	}

	FWaveDataProvider = Waveform->GetWaveDataProvider();

	if ( FWaveDataProvider->IsStreaming() )
	{
		// create separated data provider for streaming sounds
		FWaveDataProvider = Waveform->CreateWaveDataProvider();
	}

	sWaveDataFormat WaveDataFormat = FWaveDataProvider->GetWaveDataFormat();

	switch ( WaveDataFormat.FBitsPerSample )
	{
		case 8:

			switch ( WaveDataFormat.FChannels )
			{
				case 1:
					FFormat = AL_FORMAT_MONO8;
					break;
				case 2:
					FFormat = AL_FORMAT_STEREO8;
					break;
			}

			break;
		case 16:

			switch ( WaveDataFormat.FChannels )
			{
				case 1:
					FFormat = AL_FORMAT_MONO16;
					break;
				case 2:
					FFormat = AL_FORMAT_STEREO16;
					break;
			}

			break;
	}

	if ( FWaveDataProvider->IsStreaming() )
	{
		FBuffersCount = 2;

		alGenBuffers( FBuffersCount, &FBufferID[0] );
	}
	else
	{
		FBuffersCount = 1;

		alGenBuffers( FBuffersCount, &FBufferID[0] );

		alBufferData( FBufferID[0],
		              FFormat,
		              FWaveDataProvider->GetWaveData(),
		              FWaveDataProvider->GetWaveDataSize(),
		              FWaveDataProvider->GetWaveDataFormat().FSamplesPerSec );

		alSourcei( FSourceID, AL_BUFFER, FBufferID[0] );
	}

#endif
}

int clAudioSource_OpenAL::StreamBuffer( Luint BufferID, int Size )
{
#if L_AUDIO_USE_OPENAL
	int ActualSize = FWaveDataProvider->StreamWaveData( Size );

	alBufferData( BufferID,
	              FFormat,
	              FWaveDataProvider->GetWaveData(),
	              FWaveDataProvider->GetWaveDataSize(),
	              FWaveDataProvider->GetWaveDataFormat().FSamplesPerSec );

	return ActualSize;
#else
	return 0;
#endif
}

void clAudioSource_OpenAL::Play()
{
#if L_AUDIO_USE_OPENAL

//   if ( IsPlaying() ) alSourceStop( FSourceID );
	if ( IsPlaying() ) { return; }

	Lint State;

	alGetSourcei( FSourceID, AL_SOURCE_STATE, &State );

	if ( !FWaveDataProvider )
	{
		return;
	}

	//FATAL( , "No data provider is attached to the audio source" );

	if ( State != AL_PAUSED && FWaveDataProvider->IsStreaming() )
	{
		UnqueueAll();

		StreamBuffer( FBufferID[0], BUFFER_SIZE );
		StreamBuffer( FBufferID[1], BUFFER_SIZE );

		alSourceQueueBuffers( FSourceID, 2, &FBufferID[0] );

		dynamic_cast<clAudioSubSystem_OpenAL*>( Env->Audio )->RegisterSource( this );
	}

	alSourcePlay( FSourceID );
#endif
}

void clAudioSource_OpenAL::UnqueueAll()
{
#if L_AUDIO_USE_OPENAL
	Lint Queued;

	alGetSourcei( FSourceID, AL_BUFFERS_QUEUED, &Queued );

	if ( Queued > 0 )
	{
		alSourceUnqueueBuffers( FSourceID, Queued, &FBufferID[0] );
	}

#endif
}

void clAudioSource_OpenAL::Stop()
{
#if L_AUDIO_USE_OPENAL
//   if ( !IsPlaying() ) return;

	alSourceStop( FSourceID );

	if ( FWaveDataProvider )
	{
		FWaveDataProvider->Seek( 0 );

		if ( FWaveDataProvider->IsStreaming() )
		{
			dynamic_cast<clAudioSubSystem_OpenAL*>( Env->Audio )->UnRegisterSource( this );

			UnqueueAll();
		}
	}

#endif
}

void clAudioSource_OpenAL::Pause()
{
#if L_AUDIO_USE_OPENAL
	alSourcePause( FSourceID );

	UnqueueAll();
//   Stop();
#endif
}

void clAudioSource_OpenAL::LoopSound( const bool Loop )
{
#if L_AUDIO_USE_OPENAL

	if ( FWaveDataProvider && !FWaveDataProvider->IsStreaming() )
	{
		alSourcei( FSourceID, AL_LOOPING, Loop );
	}

	FLooping = Loop;
#endif
}

bool clAudioSource_OpenAL::IsPlaying() const
{
#if L_AUDIO_USE_OPENAL
	Lint State;

	alGetSourcei( FSourceID, AL_SOURCE_STATE, &State );

	return State == AL_PLAYING;
#else
	return false;
#endif
}

void clAudioSource_OpenAL::Update( float DeltaSeconds )
{
#if L_AUDIO_USE_OPENAL

	if ( !FWaveDataProvider ) { return; }

	if ( !IsPlaying() ) { return; }

	if ( FWaveDataProvider->IsStreaming() )
	{
		Lint Processed;

		alGetSourcei( FSourceID, AL_BUFFERS_PROCESSED, &Processed );

		while ( Processed-- )
		{
			Luint BufID;

			alSourceUnqueueBuffers( FSourceID, 1, &BufID );

			StreamBuffer( BufID, BUFFER_SIZE );

			if ( FWaveDataProvider->IsEOF() )
			{
				// FIXME: crash
				/*
				            if ( !FLooping )
				            {
				               UnqueueAll();
				               break;
				            }
				*/
				if ( FLooping ) { FWaveDataProvider->Seek( 0 ); }

				StreamBuffer( BufID, BUFFER_SIZE );
			}

			alSourceQueueBuffers( FSourceID, 1, &BufID );
		}
	}

#endif

	iAudioSource::Update( DeltaSeconds );
}


void clAudioThread::Run()
{
	Env->Logger->SetCurrentThreadName( "Audio" );

	guard();

#if L_AUDIO_USE_OPENAL
	LAL::clALExtRetriever* OpenAL = new LAL::clALExtRetriever;
	FATAL( !OpenAL->Reload(), "Unable to initialize OpenAL" );

	// now initialize OpenAL
	InitOpenAL();
#endif

	double Seconds = Env->GetSeconds();

	while ( !IsPendingExit() )
	{
		float DeltaSeconds = static_cast<float>( Env->GetSeconds() - Seconds );

		{
			LMutex Lock( &FMutex );

			// update sources
			std::for_each( FActiveSources.begin(), FActiveSources.end(), std::bind2nd( std::mem_fun( &iAudioSource::Update ), DeltaSeconds ) );
		}

		Seconds = Env->GetSeconds();

		Env->ReleaseTimeslice( 100 );
	}

	Env->Logger->Log( L_NOTICE, "Deallocating sources" );

	{
		LMutex Lock( &FMutex );

		while ( !FActiveSources.empty() )
		{
			delete( FActiveSources.back() );
		}
	}

#if L_AUDIO_USE_OPENAL
	ShutdownOpenAL();

	delete( OpenAL );
#endif

	Env->Logger->Log( L_NOTICE, "Audio thread exited" );

	unguard();
}

void clAudioThread::Activate( bool Activated )
{
	guard();

	if ( Activated ) { ResumeAll(); }
	else { PauseAll(); }

	unguard();
}

void clAudioThread::ToggleAll()
{
	if ( FActive ) { PauseAll(); }
	else { ResumeAll(); }
}

void clAudioThread::RegisterSource( iAudioSource* Source )
{
	guard();

	Env->Logger->Log( L_NOTICE, "Registering audio source" );

	LMutex Lock( &FMutex );

	clSourcesList::iterator i = std::find( FActiveSources.begin(), FActiveSources.end(), Source );

	if ( i != FActiveSources.end() ) { return; }

	FActiveSources.push_back( Source );

	unguard();
}

void clAudioThread::UnRegisterSource( iAudioSource* Source )
{
	guard();

	Env->Logger->Log( L_NOTICE, "Unregistering audio source" );

	LMutex Lock( &FMutex );

	clSourcesList::iterator i = std::find( FActiveSources.begin(), FActiveSources.end(), Source );

	if ( i != FActiveSources.end() ) { FActiveSources.erase( i ); }

	unguard();
}

void clAudioThread::StopAll()
{
	FMutex.Lock();

	std::vector<iAudioSource*> Sources = FActiveSources;

	FMutex.Unlock();

	for ( size_t i = 0 ; i != Sources.size(); i++ )
	{
		Sources[i]->Stop();
	}
}

void clAudioThread::PauseAll()
{
	FMutex.Lock();

	std::vector<iAudioSource*> Sources = FActiveSources;

	FMutex.Unlock();

	for ( size_t i = 0 ; i != Sources.size(); i++ )
	{
		Sources[i]->Pause();
	}

	FActive = false;
}

void clAudioThread::ResumeAll()
{
	FMutex.Lock();

	std::vector<iAudioSource*> Sources = FActiveSources;

	FMutex.Unlock();

	for ( size_t i = 0 ; i != Sources.size(); i++ )
	{
		Sources[i]->Play();
	}

	FActive = true;
}

clAudioSubSystem_OpenAL::clAudioSubSystem_OpenAL()
#if L_AUDIO_USE_OPENAL
	: FDevice( NULL ),
	  FContext( NULL )
#endif
{
}

void clAudioSubSystem_OpenAL::AfterConstruction()
{
#if L_AUDIO_USE_OPENAL
	iAudioSubSystem::AfterConstruction();

	// start thread
	FAudioThread = new clAudioThread();
	FAudioThread->Start( Env, iThread::Priority_Low );

	Env->Connect( L_EVENT_ACTIVATE, BIND( &clAudioSubSystem_OpenAL::Event_ACTIVATE ) );
#endif
}

void clAudioThread::InitOpenAL()
{
#if L_AUDIO_USE_OPENAL
	guard();

//	Env->Logger->Log( L_LOG, "Initializing OpenAL..." );

	LString DeviceName = Env->Console->GetVarValueStr( "Audio.AudioDeviceName", "" );

	FDevice = alcOpenDevice( DeviceName.empty() ? NULL : DeviceName.c_str() );

	FATAL( !FDevice, "Unable to open OpenAL device" );

	FContext = alcCreateContext( FDevice, NULL );

	FATAL( !FContext, "Unable to create OpenAL context" );

	FATAL( !alcMakeContextCurrent( FContext ), "Unable to select OpenAL context" );

	Env->Logger->LogP( L_LOG, "OpenAL (%s, %s, %s), extensions : %s", alGetString( AL_VERSION ), alGetString( AL_VENDOR ), alGetString( AL_RENDERER ), alGetString( AL_EXTENSIONS ) );
//	Env->Logger->Log( L_LOG, "OpenAL vendor    : " + LString(  ) );
//	Env->Logger->Log( L_LOG, "OpenAL renderer  : " + LString( alGetString( AL_RENDERER ) ) );
//	Env->Logger->Log( L_LOG, "OpenAL extensions: " );
	/*
	   {
	      LString Extensions;
	      Extensions.assign( alGetString( AL_EXTENSIONS ) );
	      LStr::ReplaceAll( &Extensions, ' ', '\n' );
	      Env->Logger->Log( L_LOG, '\n' + Extensions );
	   }
	*/
	unguard();
#endif
}

void clAudioSubSystem_OpenAL::StopAllC( const LString& Param )
{
	iAudioSubSystem::StopAllC( Param );

	FAudioThread->StopAll();
}

void clAudioSubSystem_OpenAL::StopAll()
{
	FAudioThread->StopAll();
}

void clAudioSubSystem_OpenAL::PauseAll()
{
	FAudioThread->PauseAll();
}

void clAudioSubSystem_OpenAL::ResumeAll()
{
	FAudioThread->ResumeAll();
}

void clAudioSubSystem_OpenAL::ToggleAll()
{
	FAudioThread->ToggleAll();
}

bool clAudioSubSystem_OpenAL::IsActive() const
{
	return FAudioThread->IsActive();
}

void clAudioSubSystem_OpenAL::Event_ACTIVATE( LEvent Envent, const LEventArgs& Args )
{
	FAudioThread->Activate( Args.FBoolArg );
}

clAudioSubSystem_OpenAL::~clAudioSubSystem_OpenAL()
{
	Env->Disconnect( L_EVENT_ACTIVATE, BIND( &clAudioSubSystem_OpenAL::Event_ACTIVATE ) );

	StopAllC( "" );

	Utils::DeallocateAll( FAnnouncers );

	FModule = NULL;

	FAudioThread->Exit( true );
	delete( FAudioThread );
}

void clAudioThread::ShutdownOpenAL()
{
#if L_AUDIO_USE_OPENAL
	guard();

	alcDestroyContext( FContext );
	alcCloseDevice( FDevice );

	unguard();
#endif
}

void clAudioSubSystem_OpenAL::SetListenerPosition( const LVector3& Position )
{
#if L_AUDIO_USE_OPENAL
	alListenerfv( AL_POSITION, Position.ToFloatPtr() );
#endif
}

void clAudioSubSystem_OpenAL::SetListenerVelocity( const LVector3& Velocity )
{
#if L_AUDIO_USE_OPENAL
	alListenerfv( AL_VELOCITY, Velocity.ToFloatPtr() );
#endif
}

void clAudioSubSystem_OpenAL::SetListenerOrientation( const LVector3& Forward, const LVector3& Up )
{
#if L_AUDIO_USE_OPENAL
	float Orientation[6] = { Forward.X, Forward.Y, Forward.Z,
	                         Up.X,      Up.Y,      Up.Z
	                       };

	alListenerfv( AL_ORIENTATION, Orientation );
#endif
}

void clAudioSubSystem_OpenAL::SetListenerVolume( float Volume )
{
#if L_AUDIO_USE_OPENAL
	alListenerf( AL_GAIN, Volume );
#endif
}

iAudioSource* clAudioSubSystem_OpenAL::CreateSourceFromFile( const LString& FileName ) const
{
	iAudioSource* Source = CreateSource();

#if L_AUDIO_USE_OPENAL
	Source->BindWaveform( Env->Resources->LoadWaveform( FileName ) );
#endif

	return Source;
}

iAudioSource* clAudioSubSystem_OpenAL::CreateSource() const
{
#if L_AUDIO_USE_OPENAL
	return Env->Linker->Instantiate( "clAudioSource_OpenAL" );
#else
	return NULL;
#endif
}

void clAudioSubSystem_OpenAL::RegisterSource( iAudioSource* Source )
{
	FAudioThread->RegisterSource( Source );
}

void clAudioSubSystem_OpenAL::UnRegisterSource( iAudioSource* Source )
{
	FAudioThread->UnRegisterSource( Source );
}

/*
 * 14/03/2007
     SetVolume()
     SetPitch()
 * 08/10/2006
     SetPosition()
     SetVelocity()
     SetDirection()
 * 25/12/2005
     Fixed bugs with stream looping
 * 24/12/2005
     Better streaming
 * 18/02/2005
     It's here
*/
