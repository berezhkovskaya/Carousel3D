/**
 * \file Animation.h
 * \brief Animation resources
 * \version 0.6.02
 * \date 22/09/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clAnimation_
#define _clAnimation_

#include "Platform.h"
#include "Resources/iResource.h"
#include "AnimSequence.h"
#include "VertexAttribs.h"

class scriptfinal clAnimSet: public iObject
{
public:
	scriptmethod void             AddAnimSequence( const sAnimSequence& Anim );
	scriptmethod sAnimSequence    GetAnimSequenceByName( const LString& Name ) const;
	/// FirstFrame, Frames, LoopingFrames, FPS

	scriptmethod sAnimSequence    GetAnimSequence( size_t i ) const;
	scriptmethod LString          GetAnimSequenceName( size_t i ) const;
	scriptmethod LVector4         GetAnimSequenceParams( size_t i ) const;
	scriptmethod size_t           GetNumAnimSequences() const;

	scriptmethod void             LoadAnimSet( const LString& FileName );
private:
	/// Collection of named animations
	sAttribStream<sAnimSequence>   FAnimations;
};

class scriptfinal clAnimation: public iResource
{
public:
	clAnimation();
	virtual ~clAnimation();

	NET_EXPORTABLE()

	//
	// iObject interface
	//
	virtual void    AfterConstruction();
	//
	// iResource interface
	//
	virtual LString GetCachingDir() const;
	virtual void    CacheTo( const LString& FileName );
	virtual bool    CacheFrom( const LString& FileName );
	//
	// clAnimation
	//
	virtual clAnimSet* GetCurrentAnimSet() const;
	virtual clAnimSet* GetDefaultAnimSet() const;

	virtual void SetAnimSet( clAnimSet* AnimSet );

	virtual clAnimation* CloneAnimation() const;
protected:
	virtual bool    LoadFromFile( const LString& FileName );
private:
	clAnimSet*    FDefaultAnimSet;
	clAnimSet*    FCurrentAnimSet;
	clMutex       FMutex;
};

#endif

/*
 * 22/09/2011
     It's here
*/
