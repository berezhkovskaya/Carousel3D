/**
 * \file Audio_OpenAL.h
 * \brief OpenAL audio subsystem
 * \version 0.5.99
 * \date 04/06/2011
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clAudioSubSystem_OpenAL_
#define _clAudioSubSystem_OpenAL_

#include "Platform.h"
#include "Audio/Audio.h"
#include "Utils/Thread.h"

#if L_AUDIO_USE_OPENAL
#	include "Generated/LAL/LAL.h"
#endif // L_AUDIO_USE_OPENAL

/// Updates all OpenAL sources in a separate thread
class scriptfinal clAudioThread: public iThread
{
public:
	clAudioThread(): FActive( true ) {};
	//
	// iThread interface
	//
	virtual void Run();
	//
	// clAudioThread
	//
	virtual void    RegisterSource( iAudioSource* Source );
	virtual void    UnRegisterSource( iAudioSource* Source );
	virtual void    StopAll();
	virtual void    PauseAll();
	virtual void    ResumeAll();
	virtual void    ToggleAll();
	virtual bool    IsActive() const { return FActive; }
	virtual void    Activate( bool Activated );
private:
	inline void     InitOpenAL();
	inline void     ShutdownOpenAL();
	typedef std::vector<iAudioSource*> clSourcesList;
private:
	clSourcesList    FActiveSources;
	clMutex          FMutex;
	bool             FActive;
#if L_AUDIO_USE_OPENAL
	ALCdevice*     FDevice;
	ALCcontext*    FContext;
#endif // L_AUDIO_USE_OPENAL
};

/// OpenAL audio subsystem
class scriptfinal clAudioSubSystem_OpenAL: public iAudioSubSystem
{
public:
	clAudioSubSystem_OpenAL();
	virtual ~clAudioSubSystem_OpenAL();

	//
	// iObject interface
	//
	virtual void             AfterConstruction();
	//
	// iAudioSubSystem interface
	//
	virtual iAudioSource*    CreateSourceFromFile( const LString& FileName ) const;
	virtual iAudioSource*    CreateSource() const;
	virtual void             RegisterSource( iAudioSource* Source );
	virtual void             UnRegisterSource( iAudioSource* Source );
	virtual void             SetListenerPosition( const LVector3& Position );
	virtual void             SetListenerVelocity( const LVector3& Velocity );
	virtual void             SetListenerOrientation( const LVector3& Forward, const LVector3& Up );
	virtual void             SetListenerVolume( float Volume );
	virtual void             StopAll();
	virtual void             PauseAll();
	virtual void             ResumeAll();
	virtual void             ToggleAll();
	virtual bool             IsActive() const;
	FWD_EVENT_HANDLER( Event_ACTIVATE );
private:
#if L_AUDIO_USE_OPENAL
	ALCdevice*     FDevice;
	ALCcontext*    FContext;
#endif // L_AUDIO_USE_OPENAL
	clAudioThread* FAudioThread;
protected:
	virtual void    StopAllC( const LString& Param );
};

/// OpenAL audio source
class scriptfinal clAudioSource_OpenAL: public iAudioSource
{
public:
	clAudioSource_OpenAL();
	virtual ~clAudioSource_OpenAL();
	//
	// iAudioSource interface
	//
	virtual void    AfterConstruction();
	//
	// iAudioSource interface
	//
	virtual void    Play();
	virtual void    Stop();
	virtual void    Pause();
	virtual void    LoopSound( const bool Loop );
	virtual bool    IsPlaying() const;
	virtual void    Update( float DeltaSeconds );
	virtual void    SetPosition( const LVector3& Position );
	virtual void    SetVelocity( const LVector3& Velocity );
	virtual void    SetDirection( const LVector3& Direction );
	virtual void    SetVolume( float Volume );
	virtual void    SetVolumeNoFade( float Volume );
	virtual void    SetPitch( float Pitch );
	virtual void    BindWaveform( iWaveform* Waveform );
private:
	int    StreamBuffer( Luint BufferID, int Size );
	void   UnqueueAll();
	void   DetachProvider();
private:
#if L_AUDIO_USE_OPENAL
	Luint                 FSourceID;
	Luint                 FFormat;
	Luint                 FBufferID[2];
	int                   FBuffersCount;
	iWaveDataProvider*    FWaveDataProvider;
	bool                  FLooping;
#endif
};

#endif

/*
 * 25/07/2010
     Dynamic linking refactored in GL-style
 * 14/05/2009
     Dynamic linking with OpenAL32.dll
 * 06/04/2009
     Mutex stuff removed
 * 14/03/2007
     SetListenerVolume()
 * 07/03/2007
     Announce3D()
 * 08/10/2006
     SetListenerPosition()
     SetListenerVelocity()
     SetListenerOrientation()
 * 18/02/2005
     It's here
*/
