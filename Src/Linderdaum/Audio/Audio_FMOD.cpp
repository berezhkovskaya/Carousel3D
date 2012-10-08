/**
 * \file Audio_FMOD.cpp
 * \brief FMOD audio system
 * \version 0.5.89
 * \date 05/01/2009
 * \author Sergey Kosarevsky, 2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Audio_FMOD.h"

#include "Environment.h"

#include "Core/Linker.h"
#include "Core/Logger.h"
#include "Core/Console.h"
#include "Core/VFS/FileSystem.h"
#include "Core/VFS/iIStream.h"

#include "Resources/ResourcesManager.h"

#include <string.h>

clAudioSource_FMOD::clAudioSource_FMOD(): FStream( NULL )
#if L_AUDIO_USE_FMOD
	,
	FSystem( NULL ),
	FSource( NULL ),
	FChannel( NULL )
#endif

{
}

clAudioSource_FMOD::~clAudioSource_FMOD()
{
	Stop();

	delete( FStream );
}

void clAudioSource_FMOD::BindWaveform( iWaveform* Waveform )
{
	delete( FStream );

	FStream = Env->FileSystem->CreateFileReader( Waveform->GetFileName() );

#if L_AUDIO_USE_FMOD
	FMOD_CREATESOUNDEXINFO ExInfo;

	memset( &ExInfo, 0, sizeof( FMOD_CREATESOUNDEXINFO ) );

	ExInfo.cbsize = sizeof( FMOD_CREATESOUNDEXINFO );
	ExInfo.length = FStream->GetFileSize();

	FSystem->createSound( ( const char* )FStream->MapStream(),
	                      FMOD_HARDWARE | FMOD_OPENMEMORY | FMOD_LOOP_NORMAL,
	                      &ExInfo,
	                      &FSource );
#endif
}

void clAudioSource_FMOD::Play()
{
#if L_AUDIO_USE_FMOD
	bool Paused;

	FChannel->getPaused( &Paused );

	if ( Paused )
	{
		FChannel->setPaused( false );
	}
	else
	{
		FSystem->playSound( FMOD_CHANNEL_FREE, FSource, false, &FChannel );
		FChannel->setLoopCount( 0 );
	}

#endif
}

void clAudioSource_FMOD::Stop()
{
#if L_AUDIO_USE_FMOD
	FChannel->stop();
#endif
}

void clAudioSource_FMOD::Pause()
{
#if L_AUDIO_USE_FMOD
	FChannel->setPaused( true );
#endif
}

void clAudioSource_FMOD::LoopSound( const bool Loop )
{
#if L_AUDIO_USE_FMOD
	FChannel->setLoopCount( Loop ? -1 : 0 );
#endif
}

bool clAudioSource_FMOD::IsPlaying() const
{
	bool Playing = false;

#if L_AUDIO_USE_FMOD
	FChannel->isPlaying( &Playing );
#endif

	return Playing;
}


#if L_AUDIO_USE_FMOD
#	include "ExtLibs/FMOD/fmod_errors.h"
#endif

clAudioSubSystem_FMOD::clAudioSubSystem_FMOD()
#if L_AUDIO_USE_FMOD
	: FSystem( NULL )
#endif
{
}

void clAudioSubSystem_FMOD::AfterConstruction()
{
	guard();

	iAudioSubSystem::AfterConstruction();

#if L_AUDIO_USE_FMOD
	InitFMOD();
#endif

	unguard();
}

#if L_AUDIO_USE_FMOD
void clAudioSubSystem_FMOD::CheckError( FMOD_RESULT Result ) const
{
	FATAL( Result != FMOD_OK, "FMOD error: " + LString( FMOD_ErrorString( Result ) ) );
}

void clAudioSubSystem_FMOD::InitFMOD()
{
	guard();

	CheckError( FMOD::System_Create( &FSystem ) );

	CheckError( FSystem->setSpeakerMode( FMOD_SPEAKERMODE_STEREO ) );
	CheckError( FSystem->init( 100, FMOD_INIT_NORMAL, 0 ) );

	Env->Logger->Log( L_LOG, "FMOD version   : " + GetFMODVersion() );

	unguard();
}

void clAudioSubSystem_FMOD::ShutdownFMOD()
{
	guard();

	CheckError( FSystem->close() );
	CheckError( FSystem->release() );

	unguard();
}

LString clAudioSubSystem_FMOD::GetFMODVersion() const
{
	unsigned int Version = 0;

	CheckError( FSystem->getVersion( &Version ) );

	return LStr::ToStr( Version );
}
#endif

clAudioSubSystem_FMOD::~clAudioSubSystem_FMOD()
{
	StopAllC( "" );

	Utils::DeallocateAll( FAnnouncers );

	if ( FModule != NULL ) { delete( FModule ); }

	FModule = NULL;

#if L_AUDIO_USE_FMOD
	ShutdownFMOD();
#endif
}

iAudioSource* clAudioSubSystem_FMOD::CreateSourceFromFile( const LString& FileName ) const
{
	iAudioSource* Source = CreateSource();

	Source->BindWaveform( Env->Resources->LoadWaveform( FileName ) );

	return Source;
}

iAudioSource* clAudioSubSystem_FMOD::CreateSource() const
{
	clAudioSource_FMOD* Source = Env->Linker->Instantiate( "clAudioSource_FMOD" );

#if L_AUDIO_USE_FMOD
	Source->SetSystem( FSystem );
#endif

	return Source;
}

/*
 * 15/05/2009
     L_AUDIO_USE_FMOD switch
 * 05/01/2009
     It's here
*/
