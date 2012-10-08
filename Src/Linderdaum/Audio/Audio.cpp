/**
 * \file Audio.cpp
 * \brief Classes for audio subsystem
 * \version 0.5.99e
 * \date 04/06/2011
 * \author Sergey Kosarevsky, 2005-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Audio/Audio.h"

#include "Core/Console.h"
#include "Core/CVars.h"
#include "Core/Linker.h"

#include "Math/LMath.h"

// two classes for data streaming
#include "WAV.h"

#if L_AUDIO_USE_OGG
#	include "OGG.h"
#else
#	undef OGG_DYNAMIC_LINK
#endif

#include "MOD.h"

/// Waveform loading/management

iWaveform::iWaveform()
	: FWaveDataProvider( NULL ),
	  FStream( NULL )
{
}

iWaveform::~iWaveform()
{
	delete( FStream );
	delete( FWaveDataProvider );

	FWaveDataProvider = NULL;
}

LString iWaveform::GetCachingDir() const
{
	return Env->Console->GetVarValueStr( "Cache.WaveformsCachingDir", "Cache/Textures" );
}

bool iWaveform::LoadFromFile( const LString& FileName )
{
	this->SetFileName( FileName );

	LString Ext;
	Env->FileSystem->SplitPath( FileName, NULL, NULL, NULL, &Ext );
	LStr::ToUpper( &Ext );

	/*
	   NOTE(Vinil):
	   After so many years we finally came to this "lame" hard-coded solution...
	   Anyways, audio is not the main purpose of the engine
	   and Wav/Ogg loading code is only for those
	   who do not wish to pay for FMOD
	*/
	iWaveDataProvider* Provider = NULL;

	if ( FStream ) { delete( FStream ); }

	// open stream
	FStream = Env->FileSystem->CreateFileReader( GetFileName() );

	// some error while opening file
	if ( !FStream )
	{
		return false;
	}

	// select data provider based on file extension
	if ( Ext == ".WAV" )
	{
		Provider = new clWaveDataProvider_WAV( Env, FStream );
	}
	else if ( Ext == ".OGG" )
	{
#if L_AUDIO_USE_OGG
		Provider = new clWaveDataProvider_OGG( Env, FStream );
#endif
	}
	else if ( Ext == ".MOD" || Ext == ".IT" || Ext == ".S3M" || Ext == ".XM" )
	{
		Provider = new clModPlugProvider( Env, FStream );
	}

	// if we have the loader
	if ( Provider )
	{
		// initialize the data provider
		Provider->Env = Env;
		Provider->AfterConstruction();

		// attach it here
		this->AttachWaveDataProvider( Provider );

		// finish loading
		return true;
	}

	// Unsupported format
	return false;
}

void iWaveform::AttachWaveDataProvider( iWaveDataProvider* WaveDataProvider )
{
	if ( FWaveDataProvider != WaveDataProvider ) { delete( FWaveDataProvider ); }

	FWaveDataProvider = WaveDataProvider;
}

iWaveDataProvider* iWaveform::CreateWaveDataProvider() const
{
	return FWaveDataProvider->Clone();
}

iWaveDataProvider* iWaveform::GetWaveDataProvider() const
{
	return FWaveDataProvider;
}

/// Basic 'MUTE' soundsystem

iAudioSubSystem::iAudioSubSystem() : FModule( NULL )
{
}

void iAudioSubSystem::AfterConstruction()
{
	Env->Console->RegisterCommand( "PlayModule",   Utils::Bind( &iAudioSubSystem::PlayModuleC,   this ) );
	Env->Console->RegisterCommand( "StopModule",   Utils::Bind( &iAudioSubSystem::StopModuleC,   this ) );
	Env->Console->RegisterCommand( "ModuleVolume", Utils::Bind( &iAudioSubSystem::ModuleVolumeC, this ) );
	Env->Console->RegisterCommand( "ModuleFadeOut", Utils::Bind( &iAudioSubSystem::ModuleFadeOutC, this ) );
	Env->Console->RegisterCommand( "ModuleFadeIn",  Utils::Bind( &iAudioSubSystem::ModuleFadeInC, this ) );
	Env->Console->RegisterCommand( "Announce",     Utils::Bind( &iAudioSubSystem::AnnounceC,     this ) );
	Env->Console->RegisterCommand( "StopAll",      Utils::Bind( &iAudioSubSystem::StopAllC,      this ) );

	Env->Connect( L_EVENT_TIMER, BIND( &iAudioSubSystem::Event_TIMER ) );

	FModuleVolume = Env->Console->GetVarDefault( "Audio.ModuleVolume", "1.0" );
}

iAudioSubSystem::~iAudioSubSystem()
{
	Env->Disconnect( L_EVENT_TIMER, BIND( &iAudioSubSystem::Event_TIMER ) );

	Env->Console->UnRegisterCommand( "PlayModule", Utils::Bind( &iAudioSubSystem::PlayModuleC, this ) );
	Env->Console->UnRegisterCommand( "StopModule", Utils::Bind( &iAudioSubSystem::StopModuleC, this ) );
	Env->Console->UnRegisterCommand( "ModuleVolume", Utils::Bind( &iAudioSubSystem::ModuleVolumeC, this ) );
	Env->Console->UnRegisterCommand( "ModuleFadeOut", Utils::Bind( &iAudioSubSystem::ModuleFadeOutC, this ) );
	Env->Console->UnRegisterCommand( "ModuleFadeIn",  Utils::Bind( &iAudioSubSystem::ModuleFadeInC, this ) );
	Env->Console->UnRegisterCommand( "Announce",   Utils::Bind( &iAudioSubSystem::AnnounceC,   this ) );
	Env->Console->UnRegisterCommand( "StopAll",    Utils::Bind( &iAudioSubSystem::StopAllC,    this ) );

	delete( FModule );

	FModule = NULL;
}

iWaveform* iAudioSubSystem::CreateWaveform() const
{
	return Env->Linker->Instantiate( "iWaveform" );
}

iAudioSource* iAudioSubSystem::CreateSourceFromFile( const LString& FileName ) const
{
	return CreateSource();
}

iAudioSource* iAudioSubSystem::CreateSource() const
{
	return Env->Linker->Instantiate( "iAudioSource" );
}

void iAudioSubSystem::Event_TIMER( LEvent Event, const LEventArgs& Args )
{
	if ( FModule )
	{
		FModule->SetFadeVolume( FModuleVolume->GetFloat() );
	}

	// just deallocate one-by-one
	if ( FAnnouncers.size() > 0 )
	{
		clAnnouncer* Announcer = FAnnouncers.front();

		if ( Announcer->IsFinished() )
		{
			FAnnouncers.pop_front();
			delete( Announcer );
		}
	}
}

iAudioSource* iAudioSubSystem::Announce( const LString& FileName )
{
	if ( !Env->FileSystem->FileExists( FileName ) )
	{
		Env->Console->DisplayError( "Unable to find sound: " + FileName );

		return NULL;
	}

	FAnnouncers.push_back( new clAnnouncer( FileName, NULL ) );
	FAnnouncers.back()->Env = Env;
	FAnnouncers.back()->AfterConstruction();

	return FAnnouncers.back()->GetSource();
}

iAudioSource* iAudioSubSystem::Announce3D( iAudioSource* Source )
{
	FAnnouncers.push_back( new clAnnouncer( "", Source ) );
	FAnnouncers.back()->Env = Env;
	FAnnouncers.back()->AfterConstruction();

	return Source;
}

void iAudioSubSystem::AnnounceC( const LString& Param )
{
	if ( Param.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: ANNOUNCE <sound file> [cvar name]" );
		return;
	}

	bool Play = true;

	LString FileName = LStr::GetToken( Param, 1 );
	LString CVarName = LStr::GetToken( Param, 2 );

	if ( !CVarName.empty() )
	{
		clCVar* CVar = Env->Console->GetVar( CVarName );

		Play = CVar->GetBool();
	}

	if ( Play ) { Announce( FileName ); }
}

void iAudioSubSystem::PlayModuleC( const LString& Param )
{
	if ( Param.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: PLAYMODULE <sound file>" );
		return;
	}

	delete( FModule );

	FModule = CreateSourceFromFile( Param );

	FModule->Play();
	FModule->LoopSound( true );
//	FModule->SetVolume( 1.0f );
//	FModule->SetFadeVolume( 1.0f );
}

void iAudioSubSystem::ModuleVolumeC( const LString& Param )
{
	if ( Param.empty() && LStr::IsCorrectDouble( Param ) )
	{
		Env->Console->DisplayInfoTip( "Usage: MODULEVOLUME <volume 0.0 to 1.0>" );
		return;
	}

	FModuleVolume->SetFloat( LStr::ToFloat( Param ) );

	if ( FModule )
	{
		FModule->SetVolume( LStr::ToFloat( Param ) );
	}
}

void iAudioSubSystem::ModuleFadeOutC( const LString& Param )
{
	FModuleVolume->SetFloat( 0.0f );
}

void iAudioSubSystem::ModuleFadeInC( const LString& Param )
{
	if ( Param.empty() && LStr::IsCorrectDouble( Param ) )
	{
		Env->Console->DisplayInfoTip( "Usage: ModuleFadeIn <volume 0.0 to 1.0>" );
		return;
	}

	FModuleVolume->SetFloat( LStr::ToFloat( Param ) );
}

void iAudioSubSystem::StopModuleC( const LString& Param )
{
	delete( FModule );

	FModule = NULL;
}

/// Dummy sound source

void iAudioSource::Update( float DeltaSeconds )
{
	if ( FVolume != FFadeVolume )
	{
		float DeltaVolume = FFadeVolume - FVolume;

		float NewVolume = GetVolume() + 0.5f * Math::fsign( DeltaVolume ) * DeltaSeconds;

		if ( fabs( DeltaVolume ) < DeltaSeconds ) { NewVolume = FFadeVolume; }

		SetVolumeNoFade( NewVolume );
	}
}

/*
 * 23/10/2010
     SetFadeVolume()
     GetFadeVolume()
     GetVolume()
 * 08/10/2010
     ModuleVolumeC()
 * 14/03/2007
     SetVolume()
     SetPitch()
 * 08/10/2006
     SetPosition()
     SetVelocity()
     SetDirection()
 * 06/01/2006
     CreateWaveDataProvider()
 * 24/12/2005
     AttachWaveDataProvider()
     GetWaveDataProvider()
 * 11/01/2005
     It's here
*/
