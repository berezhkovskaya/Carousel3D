/**
 * \file Screen.h
 * \brief Screen
 * \version 0.5.89
 * \date 26/02/2009
 * \author Sergey Kosarevsky, 2005-2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _Screen_
#define _Screen_

#include "Engine.h"

#include "Core/iObject.h"
#include "Utils/Viewport.h"

class clViewport;

enum LMBButton { L_MB_OK, L_MB_OKCANCEL };

/// Screen management
class scriptfinal clScreen: public iObject
{
public:
	clScreen();
	~clScreen();

	NET_EXPORTABLE()

	//
	// iObject interface
	//
	virtual void          AfterConstruction();

	//
	// clScreen
	//
	virtual void          SetDisplayMode( int Width, int Height, int BitsPerPixel, int Frequency );
	virtual void          ResetDisplayMode();
	virtual void          RestoreDisplayMode();
	virtual int           GetScreenWidth() const;
	virtual int           GetScreenHeight() const;
	virtual LVector2i     GetScreenDPI() const;
	// get the size (diagonal) of the screen in inches
	virtual float         GetScreenDiagonal() const;
	virtual clViewport*   OpenViewport( const LString& WindowClassName,
	                                    LWindowHandle ViewportHandle,
	                                    const LString& Title,
	                                    int Width,
	                                    int Height,
	                                    bool Fullscreen,
	                                    bool RegWindowClass,
	                                    bool DisplayWindow,
	                                    bool TakeoverContext );
	virtual clViewport*   TakeoverViewport( LWindowHandle ViewportHandle );
	virtual bool          IsFullscreen() const
	{
		return FFullscreen;
	};
	static bool           ShowMessageBox( const LString& Title, const LString& Text, LMBButton Buttons, bool Error );

	/* Property(Description="Width",       Category="Parameters", Type=int,   Name=Width,       Getter=GetScreenWidth ) */
	/* Property(Description="Height",      Category="Parameters", Type=int,   Name=Height,      Getter=GetScreenHeight ) */
private:
	bool    FFullscreen;
	int     FWidth, FHeight, FBitsPerPixel, FFrequency;
};

#endif

/*
 * 26/02/2009
     TakeoverViewport()
 * 05/12/2004
     It's here
*/
