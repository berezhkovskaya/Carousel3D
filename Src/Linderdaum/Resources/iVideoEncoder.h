/*
   (Part of Linderdaum Engine)
   Version 0.5.89
   (15/02/2009)
   (C) Sergey Kosarevsky, 2009
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _iVideoEncoder_
#define _iVideoEncoder_

#include "Platform.h"
#include "Core/iObject.h"

/// Video stream encoder interface
class scriptfinal netexportable iVideoEncoder: public iObject
{
public:
	iVideoEncoder() {};
	//
	// iVideoEncoder
	//
	virtual void      AddVideoFrame( void* Frame ) = 0;
	virtual bool      CreateStream( const LString& FileName, int Width, int Height, int FrameRate ) = 0;
};

#endif

/*
 * 15/02/2009
     It's here
*/
