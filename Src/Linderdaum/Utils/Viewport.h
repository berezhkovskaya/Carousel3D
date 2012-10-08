/**
 * \file Viewport.h
 * \brief Viewport and engine's window
 * \version 0.5.91
 * \date 19/08/2010
 * \author Sergey Kosarevsky, 2004-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clViewport_
#define _clViewport_

#include "Platform.h"

#if defined(OS_WINDOWS)
#include <windows.h>
#undef min
#undef max
typedef HDC  LDeviceHandle;
typedef HWND LWindowHandle;
#elif defined(OS_ANDROID)

#if defined( ANDROID9 )
// this is only for android-9 target with NativeActivity support
#include <EGL/egl.h>
typedef EGLDisplay  LDeviceHandle;
typedef EGLSurface  LWindowHandle;
#else
typedef void* LDeviceHandle;
typedef void* LWindowHandle;
#endif // ANDROID9

#elif defined(OS_LINUX)
#include <X11/Xlib.h>
typedef Display* LDeviceHandle;
typedef Window*  LWindowHandle;
#endif

#include "Core/iObject.h"
#include "Math/LVector.h"

class clCVar;
class clVirtualTrackball;

class clBitmap;

struct sMouseCursorInfo
{
	sMouseCursorInfo(): FMouseX( 0 ),
		FMouseY( 0 ),
		FOldMouseX( 0 ),
		FOldMouseY( 0 ),
		FMouseMoved( false ) {};
	float FMouseX;
	float FMouseY;

	float FOldMouseX;
	float FOldMouseY;

	bool FMouseMoved;

	inline LVector2 GetPoint() const { return LVector2( FMouseX, FMouseY ); };
	inline LVector2 GetOldPoint() const { return LVector2( FOldMouseX, FOldMouseY ); };
};

class scriptfinal clViewport: public iObject
{
public:
	clViewport( const LString& WindowClassName,
	            LWindowHandle ViewportHandle,
	            const LString& Title,
	            int Width,
	            int Height,
	            bool Fullscreen,
	            bool RegWindowClass,
	            bool DisplayWindow,
	            bool TakeoverContext );
	virtual ~clViewport();

	NET_EXPORTABLE()

	//
	// iObject interface
	//
	virtual void                AfterConstruction();
	//
	// clViewport interface
	//
	virtual const sMouseCursorInfo& GetMouseCursorInfo() const;
	virtual int                 GetWidth() const;
	virtual int                 GetHeight() const;
	/// width/height ratio
	virtual float               GetAspectRatio() const
	{
		return static_cast<float>( FWidth ) / static_cast<float>( FHeight );
	};
	virtual int         GetTop() const
	{
		return FTop;
	};
	virtual int         GetLeft() const
	{
		return FLeft;
	};
	virtual bool        IsLandscape() const { return GetWidth() >= GetHeight(); };
	virtual bool        IsPortrait() const { return GetWidth() < GetHeight(); };
	virtual void        SetViewportTitle( const LString& Title );
	virtual void        SetMousePosition( float X, float Y );
	virtual LVector2    GetMousePosition() const;
	virtual void        MoveMouse( int X, int Y );
	/// transform X, Y screen position into a normalized engine screen position
	virtual LVector2    TransformPointerPosition( int X, int Y );
	virtual void        ShowOSMouseCursor();
	virtual void        HideOSMouseCursor();
	virtual void        UpdateTrackball( clVirtualTrackball* Trackball, float Speed, bool Keypressed ) const;
	virtual LVector2    MousePositionIntToFloat( int X, int Y ) const;
	virtual void        SetViewportSize( int W, int H );

#pragma region Bitmap blitting and primitive drawing
	virtual void    BlitBitmap( int X, int Y, clBitmap* Bmp );
	virtual void    DrawLine( int X1, int Y1, int X2, int Y2 );
	virtual void    DrawText( int X, int Y, const LString& Text );
	virtual void    Clear( const LVector4& ClearColor );
#pragma endregion

#pragma region Properties
	/* Property(Description="Mouse",       Category="State",    Type=vec2,  Name=Mouse,       Getter=GetMousePosition) */
	/* Property(Description="Width",       Category="Geometry", Type=int,   Name=Width,       Getter=GetWidth ) */
	/* Property(Description="Height",      Category="Geometry", Type=int,   Name=Height,      Getter=GetHeight ) */
	/* Property(Description="Top",         Category="Geometry", Type=int,   Name=Top,         Getter=GetTop ) */
	/* Property(Description="Left",        Category="Geometry", Type=int,   Name=Left,        Getter=GetWidth ) */
	/* Property(Description="AspectRatio", Category="Geometry", Type=float, Name=AspectRatio, Getter=GetAspectRatio ) */
#pragma endregion

#pragma region OS-specific
	/// Dispatch OS events
	virtual bool    DispatchEvents();

	/// Swap rendering buffers
	virtual void    SwapBuffers();

	virtual LDeviceHandle    GetDevice() const
	{
		return FDeviceHandle;
	};
	virtual LWindowHandle    GetWindow() const
	{
		return FWindowHandle;
	};
#pragma endregion

	FWD_EVENT_HANDLER( Event_SURFACE_CHANGED );
private:
	void RecreateViewport();

	/// set parameters for already existing viewport
	void ReinitializeViewport();

	// parameters are not used in Windows
	void ResizeViewport( int Width, int Height );

private:
	/// HDC (WinAPI) or Display* (Xlib)
	LDeviceHandle    FDeviceHandle;

	/// HWND (WinAPI) or Window* (Xlib)
	LWindowHandle    FWindowHandle;

#ifdef OS_WINDOWS
private:
	static LRESULT CALLBACK     MainWndProc( HWND, UINT, WPARAM, LPARAM );
	void RegisterWindowClass();
	void UnRegisterWindowClass();
	/// High DPI - awareness
	void SetHighDPIAware() const;
	/// Disable "sticky" keys in Windows
	void StoreAndDisableStickyKeys();
	void RestoreStickyKeys() const;
private:
	LWindowHandle    FExternalWindowHandle;
	STICKYKEYS       FStickyKeys;
#elif defined OS_ANDROID
private:
	// TODO
#elif defined OS_LINUX
private:
	/// X11 display (should be in clScreen ?)
	Display*    FDisplay;

	/// the graphic context
	GC          FWindowGC;

	/// X-image used for output to the screen
	XImage*     FTmpImage;

	/// window
	Window*     FWindow;

#pragma region Used to hide themouse cursor
	Cursor    FLeftArrowCursor;
	Cursor    FInvisibleCursor;
	Pixmap    FBitmapEmptyData;
	XColor    FBlackColor;
#pragma endregion
#endif // OS_LINUX

private:
	LString          FWindowClassName;
	LString          FTitle;
	int              FWidth;
	int              FHeight;
	int              FTop;
	int              FLeft;
	bool             FFullscreen;
	bool             FRegisterWindowClass;
	bool             FDisplayWindow;
	bool             FTakeoverContext;
	sMouseCursorInfo FMouseCursorInfo;
	float            FScaleFactorX;
	float            FScaleFactorY;
	clCVar*          FMouseX;
	clCVar*          FMouseY;
};

#endif

/*
 * 19/08/2010
     Initial Xlib code
 * 03/06/2010
     Bitmap blitting
 * 20/04/2009
     MousePositionIntToFloat()
 * 18/04/2009
     Merged with iViewport
 * 15/04/2009
     Merged with clViewportProxy
 * 07/04/2009
     ShowOSMouseCursor()
     HideOSMouseCursor()
 * 26/02/2007
     SetViewportTitleC()
 * 14/01/2007
     SetViewportTitle()
 * 13/01/2005
     GetMouseCursorInfo()
 * 04/12/2004
     It's here
*/
