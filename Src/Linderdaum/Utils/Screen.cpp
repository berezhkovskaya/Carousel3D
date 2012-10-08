/**
 * \file Screen.cpp
 * \brief Screen
 * \version 0.5.60
 * \date 17/12/2005
 * \author Sergey Kosarevsky, 2005
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifdef OS_WINDOWS
#include <windows.h>
#else
#include <stdio.h>
#endif

#include "Screen.h"
#include "Environment.h"
#include "Core/Logger.h"
#include "Math/LVector.h"
#include "Utils/Viewport.h"
#include "Utils/Exceptions.h"

bool clScreen::ShowMessageBox( const LString& Title, const LString& Text, LMBButton Buttons, bool Error )
{
#ifdef OS_WINDOWS
	UINT uType = MB_TASKMODAL;

	if ( Error                    )
	{
		uType |= MB_ICONERROR;
	}

	if ( Buttons == L_MB_OK       )
	{
		uType |= MB_OK;
	}

	if ( Buttons == L_MB_OKCANCEL )
	{
		uType |= MB_OKCANCEL;
	}

	int Result = MessageBoxA( 0, Text.c_str(), Title.c_str(), uType );
#else
	fprintf( stderr, Text.c_str() );
	FIXME( "come up with something reasonable" )
	int Result = 0;
#endif

	return Result != 0;
}

clScreen::clScreen()
{
	FFullscreen = false;
}

clScreen::~clScreen()
{
	ResetDisplayMode();
}

void clScreen::AfterConstruction()
{
	iObject::AfterConstruction();

	Env->Logger->Log( L_NOTICE, "Screen size: "+LStr::ToStr( GetScreenDiagonal(), 1 )+" inches" );
}

void clScreen::SetDisplayMode( int Width, int Height, int BitsPerPixel, int Frequency )
{
	guard( "%ix%i@%i %iHz", Width, Height, BitsPerPixel, Frequency );
#ifdef OS_WINDOWS
	FWidth = Width;
	FHeight = Height;
	FBitsPerPixel = BitsPerPixel;
	FFrequency = Frequency;

	DEVMODE dmScreenSettings;

	dmScreenSettings.dmSize             = sizeof( dmScreenSettings );
	dmScreenSettings.dmPelsWidth        = Width;
	dmScreenSettings.dmPelsHeight       = Height;
	dmScreenSettings.dmBitsPerPel       = BitsPerPixel;
	dmScreenSettings.dmDisplayFrequency = Frequency;
	dmScreenSettings.dmFields           = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	if ( Frequency > 0 )
	{
		dmScreenSettings.dmFields |= DM_DISPLAYFREQUENCY;
	}

	if ( ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN | CDS_SET_PRIMARY ) != DISP_CHANGE_SUCCESSFUL )
	{
		Env->Logger->LogP( L_NOTICE, "%ix%ix%i@%iHz", Width, Height, BitsPerPixel, Frequency );
		// try dual monitor configuration
		Width *= 2;
		dmScreenSettings.dmPelsWidth = Width;

		if ( ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN  | CDS_SET_PRIMARY ) != DISP_CHANGE_SUCCESSFUL )
		{
			Env->Logger->LogP( L_WARNING, "%ix%ix%i@%iHz", Width, Height, BitsPerPixel, Frequency );
			Env->Logger->Log( L_WARNING, "The Requested Fullscreen Mode Is Not Supported By Your Video Card." );
			// restore original width
			Width /= 2;
			dmScreenSettings.dmPelsWidth = Width;
		}
	}

#endif // OS_WINDOWS
	Env->Logger->Log( L_NOTICE, "Display mode changed" );

	FFullscreen = true;

	unguard();
}

void clScreen::ResetDisplayMode()
{
	if ( !FFullscreen )
	{
		return;
	}

#ifdef OS_WINDOWS
	ChangeDisplaySettings( NULL, 0 );
#endif // OS_WINDOWS
	FFullscreen = false;
}

void clScreen::RestoreDisplayMode()
{
	if ( FFullscreen )
	{
		return;
	}

	SetDisplayMode( FWidth, FHeight, FBitsPerPixel, FFrequency );
}

int clScreen::GetScreenWidth() const
{
#if defined OS_WINDOWS
	DEVMODE dm;

	EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &dm );

	return dm.dmPelsWidth;
#elif defined OS_ANDROID
	return sEnvironment::FAndroidViewportSize.FWidth;
#else
	FIXME( "Not implemented clScreen::GetScreenWidth()" );
	return -1;
#endif
}

int clScreen::GetScreenHeight() const
{
#if defined OS_WINDOWS
	DEVMODE dm;

	EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &dm );

	return dm.dmPelsHeight;
#elif defined OS_ANDROID
	return sEnvironment::FAndroidViewportSize.FHeight;
#else
	FIXME( "Not implemented clScreen::GetScreenHeight()" );
	return -1;
#endif
}

LVector2i clScreen::GetScreenDPI() const
{
#if defined OS_WINDOWS
	HDC dc = ::GetDC(NULL);

	const int xDPI = ::GetDeviceCaps( dc, LOGPIXELSX );
	const int yDPI = ::GetDeviceCaps( dc, LOGPIXELSY );

	::ReleaseDC(NULL, dc);

	return LVector2i( xDPI, yDPI );
#elif defined OS_ANDROID
	return sEnvironment::FAndroidViewportSize.FDPI;
#else
	FIXME( "Not implemented clScreen::GetScreenDPI()" );
	return -1;
#endif
}

float clScreen::GetScreenDiagonal() const
{             
	LVector2 DPI( GetScreenDPI() );

	int W = GetScreenWidth();
	int H = GetScreenHeight();

	double sX = W;
	double sY = H;

	float dDPI = DPI.Length();

	return float( sqrt( sX * sX + sY * sY ) ) / dDPI;
}

clViewport* clScreen::OpenViewport( const LString& WindowClassName,
                                    LWindowHandle ViewportHandle,
                                    const LString& Title,
                                    int Width,
                                    int Height,
                                    bool Fullscreen,
                                    bool RegWindowClass,
                                    bool DisplayWindow,
                                    bool TakeoverContext )
{
	clViewport* Viewport = new clViewport( WindowClassName,
	                                       ViewportHandle,
	                                       Title,
	                                       Width,
	                                       Height,
	                                       Fullscreen,
	                                       RegWindowClass,
	                                       DisplayWindow,
	                                       TakeoverContext );

	Viewport->Env = Env;
	Viewport->AfterConstruction();

	return Viewport;
}

clViewport* clScreen::TakeoverViewport( LWindowHandle ViewportHandle )
{
	return OpenViewport( "", ViewportHandle, "", 0, 0, false, false, false, true );
}

/*
 * 12/06/2005
     More logging information in SetDisplayMode()
 * 07/02/2005
     Mouse wheel & extra mouse buttons support
 * 05/02/2005
     Bug fixed in MainWndProc() in WA_ACTIVATE processing
 * 18/01/2005
     CDS_SET_PRIMARY flag used
 * 06/01/2005
     Correct Alt+Tab support in fullscreen mode
 * 07/12/2004
     SetDisplayMode()
     ResetDisplayMode();
 * 05/12/2004
     It's here
*/
