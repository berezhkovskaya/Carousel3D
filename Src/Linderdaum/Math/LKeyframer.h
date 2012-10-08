/**
 * \file LKeyframer.h
 * \brief Keyframes calculations
 * \version 0.6.00
 * \date 26/12/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LKeyframer_
#define _LKeyframer_

#include <math.h>

#include "LVector.h"

/// Keyframes calculations
class LKeyframer
{
public:
	LVector4 FKeyframe;  // ( Keyframe,   NextKeyrame, LerpCoef,   CurrentNumberOfIterations )
	LVector4 FAnimation; // ( FirstFrame, LastFrame,   Loopframes, FPS    )
public:
	LKeyframer(): FKeyframe( 0.0f ), FAnimation( 0.0f ) {};
	LKeyframer( int FirstFrame,
	            int LastFrame,
	            float FPS ): FKeyframe( static_cast<float>( FirstFrame ), static_cast<float>( FirstFrame ), 0.0f, 0.0f ),
		FAnimation( static_cast<float>( FirstFrame ), static_cast<float>( LastFrame ), static_cast<float>( LastFrame ), FPS ) {};
	LKeyframer( float FirstFrame,
	            float LastFrame,
	            float FPS ): FKeyframe( FirstFrame, FirstFrame, 0.0f, 0.0f ),
		FAnimation( FirstFrame, LastFrame, LastFrame, FPS ) {};
	LKeyframer( int FirstFrame,
	            int LastFrame,
	            int LoopFrames,
	            float FPS ): FKeyframe( static_cast<float>( FirstFrame ), static_cast<float>( FirstFrame ), 0.0f, 0.0f ),
		FAnimation( static_cast<float>( FirstFrame ), static_cast<float>( LastFrame ), static_cast<float>( LoopFrames ), FPS ) {};
	LKeyframer( float FirstFrame,
	            float LastFrame,
	            float LoopFrames,
	            float FPS ): FKeyframe( FirstFrame, FirstFrame, 0.0f, 0.0f ),
		FAnimation( FirstFrame, LastFrame, LoopFrames, FPS ) {};

	//
	// LKeyframer
	//
	inline bool               operator==( const LKeyframer& K ) const { return ( FKeyframe.ToVector3() == K.FKeyframe.ToVector3() ) && ( FAnimation == K.FAnimation ); };
	inline bool               operator!=( const LKeyframer& K ) const { return ( FKeyframe.ToVector3() != K.FKeyframe.ToVector3() ) || ( FAnimation != K.FAnimation ); };
	inline const float*       ToFloatPtr() const { return FKeyframe.ToFloatPtr(); };
	inline float*             ToFloatPtr() { return FKeyframe.ToFloatPtr(); };
	inline const LVector4&    GetKeyframeVec() const { return FKeyframe; };
	inline LVector4&          GetKeyframeVec() { return FKeyframe; };
	inline const LVector4&    GetAnimationVec() const { return FAnimation; };
	inline LVector4&          GetAnimationVec() { return FAnimation; };
	inline void               SetKeyframe( float ThisFrame,   float NextFrame, float Lerp ) { FKeyframe = LVector4( ThisFrame, NextFrame, Lerp, 0.0f ); };
	inline void               SetFPS( float FPS ) { FAnimation.W = FPS; };
	inline int                GetKeyframe() const { return static_cast<int>( FKeyframe[0] ); };
	inline int                GetNextKeyframe() const { return static_cast<int>( FKeyframe[1] ); };
	inline float              GetKeyframeLerp() const { return FKeyframe[2]; };

	/// make keyframe animation step, return 'true' if the end of animation was passed
	inline bool               Update( float DeltaTime )
	{
		float FirstFrame = FAnimation[0];
		float LastFrame  = FAnimation[1];
		float LoopFrames = FAnimation[2];
		float FPS        = FAnimation[3];

		float ThisFrame  = FKeyframe[0];
		float NextFrame  = FKeyframe[1];
		float Lerp       = FKeyframe[2];
		float Iterations = FKeyframe[3];

		float AdvanceFrames = FPS * DeltaTime;

		ThisFrame += Lerp + AdvanceFrames;

		Lerp = modff( ThisFrame, &ThisFrame );

		bool Result = false;

		if ( ThisFrame >= LastFrame ) { ThisFrame = LastFrame - LoopFrames; Result = true; Iterations++; }

		NextFrame = ThisFrame + 1.0f;

		if ( NextFrame >= LastFrame ) { NextFrame = LastFrame - LoopFrames; }

		if ( ThisFrame < 0 )
		{
			ThisFrame = FirstFrame;
		}

		if ( NextFrame < 0 )
		{
			NextFrame = FirstFrame;
		}

		FKeyframe = LVector4( ThisFrame, NextFrame, Lerp, Iterations );

		return Result;
	}

	/// advance animation by the number of frames, return 'true' if the end of animation was passed
	inline bool               AdvanceFrames( int AdvanceFrames )
	{
		float FirstFrame = FAnimation[0];
		float LastFrame  = FAnimation[1];
		float LoopFrames = FAnimation[2];

		// use AdvanceFrames without Lerp
		float ThisFrame  = FKeyframe[0] + AdvanceFrames;
		float NextFrame  = ThisFrame + 1.0f;
		float Iterations = FKeyframe[3];

		bool Result = false;

		if ( ThisFrame > LastFrame ) { ThisFrame = LastFrame - LoopFrames; Result = true; Iterations++; }

		if ( NextFrame > LastFrame ) { NextFrame = LastFrame - LoopFrames; }

		if ( ThisFrame < 0 ) { ThisFrame = FirstFrame; }

		if ( NextFrame < 0 ) { NextFrame = FirstFrame; }

		FKeyframe = LVector4( ThisFrame, NextFrame, 0.0f, Iterations );

		return Result;
	}
};


#endif

/*
 * 26/12/2010
     Calculate number of iterations of animation
 * 29/09/2010
     It's here
*/
