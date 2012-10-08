/**
 * \file AnimSequence.h
 * \brief Single named animation sequence
 * \version 0.6.02
 * \date 22/09/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clAnimSequence_
#define _clAnimSequence_

#include "Platform.h"
#include "Resources/iResource.h"

struct sAnimSequence
{
	sAnimSequence(): FName( "" ),
		FFirstFrame( 0 ),
		FFrames( 1.0f ),
		FLoopingFrames( 0 ),
		FFPS( 20.0f ) {};
	sAnimSequence( const LString& Name,
	               int FirstFrame,
	               int Frames,
	               int LoopingFrames,
	               int FPS ): FName( Name ),
		FFirstFrame( static_cast<float>( FirstFrame ) ),
		FFrames( static_cast<float>( Frames ) ),
		FLoopingFrames( static_cast<float>( LoopingFrames ) ),
		FFPS( static_cast<float>( FPS ) ) {};
	sAnimSequence( const LString& Name,
	               float FirstFrame,
	               float Frames,
	               float LoopingFrames,
	               float FPS ): FName( Name ),
		FFirstFrame( FirstFrame ),
		FFrames( Frames ),
		FLoopingFrames( LoopingFrames ),
		FFPS( FPS ) {};
	LStringBuffer FName;
	float         FFirstFrame;
	float         FFrames;
	float         FLoopingFrames;
	float         FFPS;
	LVector4 ToVector4() const { return LVector4( FFirstFrame, FFrames, FLoopingFrames, FFPS ); };
};

#endif

/*
 * 22/09/2011
     It's here
*/
