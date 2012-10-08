/**
 * \file Audio_FMOD.h
 * \brief FMOD audio subsystem
 * \version 0.5.89a
 * \date 15/05/2009
 * \author Sergey Kosarevsky, 2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clAudioSource_FMOD_
#define _clAudioSource_FMOD_

#include "Audio/Audio.h"

#if L_AUDIO_USE_FMOD
#	include "ExtLibs/FMOD/fmod.hpp"
#endif

class iIStream;

/// FMOD audio library
namespace FMOD
{
	class System;
};

/// FMOD audio source
class scriptfinal clAudioSource_FMOD: public iAudioSource
{
public:
	clAudioSource_FMOD();
	virtual ~clAudioSource_FMOD();
	//
	// iAudioSource interface
	//
	virtual void    Play();
	virtual void    Stop();
	virtual void    Pause();
	virtual void    LoopSound( const bool Loop );
	virtual bool    IsPlaying() const;
	virtual void    BindWaveform( iWaveform* Waveform );
	//
	// clAudioSource_FMOD
	//
	virtual void    SetSystem( FMOD::System* System )
	{
#if L_AUDIO_USE_FMOD
		FSystem = System;
#endif
	};
private:
#if L_AUDIO_USE_FMOD
	FMOD::System*    FSystem;
	FMOD::Sound*     FSource;
	FMOD::Channel*   FChannel;
#endif

	iIStream*        FStream;
};

/// FMOD audio subsystem
class scriptfinal clAudioSubSystem_FMOD: public iAudioSubSystem
{
public:
	clAudioSubSystem_FMOD();
	virtual ~clAudioSubSystem_FMOD();
	//
	// iObject interface
	//
	virtual void             AfterConstruction();
	//
	// iAudioSubSystem interface
	//
	virtual iAudioSource*    CreateSourceFromFile( const LString& FileName ) const;
	virtual iAudioSource*    CreateSource() const;
private:
#if L_AUDIO_USE_FMOD
	inline void       InitFMOD();
	inline void       ShutdownFMOD();
	inline LString    GetFMODVersion() const;
	inline void       CheckError( FMOD_RESULT Result ) const;
	FMOD::System*     FSystem;
#endif
};

#endif

/*
 * 05/01/2009
     It's here
*/
