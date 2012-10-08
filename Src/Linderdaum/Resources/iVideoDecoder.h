/*
   (Part of Linderdaum Engine)
   Version 0.5.84
   (21/11/2007)
   (C) Sergey Kosarevsky, 2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _iVideoDecoder_
#define _iVideoDecoder_

#include "Platform.h"
#include "Core/iObject.h"

/// Video stream decoder interface
class scriptfinal netexportable iVideoDecoder: public iObject
{
public:
	iVideoDecoder() {};
	//
	// iVideoDecoder
	//
	virtual int       GetMaxFrames() const = 0;
	virtual void*     ExtractFrame( int FrameNum ) = 0;
	virtual float     GetFrameSeconds() const = 0;
	virtual int       GetWidth() const = 0;
	virtual int       GetHeight() const = 0;
	virtual void      OpenStream( const LString& FileName ) = 0;
};

#endif

/*
 * 21/11/2007
     It's here
*/
