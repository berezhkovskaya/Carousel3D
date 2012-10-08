/**
 * \file Viewport.cpp
 * \brief Viewport and engine's window
 * \version 0.5.91
 * \date 19/08/2010
 * \author Sergey Kosarevsky, 2004-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */


#include "Viewport.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/CVars.h"
#include "Core/Console.h"
#include "Core/Logger.h"
#include "Utils/Screen.h"
#include "Utils/Library.h"
#include "Renderer/iRenderContext.h"
#include "LKeys.h"
#include "Math/LMath.h"
#include "Math/Trackball.h"

#include "Images/Bitmap.h"

#if defined(OS_LINUX) && !defined(OS_ANDROID)
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <GL/glx.h>
#endif

#if defined(OS_WINDOWS)
#include "windowsx.h"
WNDPROC FOriginalWndProc = NULL;
#endif

clViewport::clViewport( const LString& WindowClassName,
                        LWindowHandle ViewportHandle,
                        const LString& Title,
                        int Width,
                        int Height,
                        bool Fullscreen,
                        bool RegWindowClass,
                        bool DisplayWindow,
                        bool TakeoverContext )
	: FWindowHandle( 0 ),
	  FWindowClassName( WindowClassName ),
	  FTitle( Title ),
	  FWidth( Width ),
	  FHeight( Height ),
	  FTop( 0 ),
	  FLeft( 0 ),
	  FFullscreen( Fullscreen ),
	  FRegisterWindowClass( RegWindowClass ),
	  FDisplayWindow( DisplayWindow ),
	  FTakeoverContext( TakeoverContext )
{
	FWindowClassName = LString( ENGINE_NAME ) + " " + LString( ENGINE_VERSION );

#ifdef OS_WINDOWS
	FExternalWindowHandle = ViewportHandle;

	if ( FTakeoverContext )
	{
		FWindowHandle = ViewportHandle;
	}

#endif

#if defined( OS_ANDROID ) && !defined( ANDROID9 )
	ResizeViewport( sEnvironment::FAndroidViewportSize.FWidth, sEnvironment::FAndroidViewportSize.FHeight );
#endif
}

void clViewport::EVENT_HANDLER( Event_SURFACE_CHANGED )
{
#if defined( OS_ANDROID )
	ResizeViewport( sEnvironment::FAndroidViewportSize.FWidth, sEnvironment::FAndroidViewportSize.FHeight );
#endif
}

#ifdef OS_WINDOWS
void clViewport::SetHighDPIAware() const
{
	guard();

	clLibrary* Lib = Env->LoadDynamicLibrary( "user32.dll" );

	typedef BOOL ( *SetProcessDPIAwareFunc )();

	SetProcessDPIAwareFunc SetDPIAware = ( SetProcessDPIAwareFunc )Lib->GetProcAddress( "SetProcessDPIAware" );

	if ( SetDPIAware )
	{
		SetDPIAware();
	}

	delete( Lib );

	unguard();
}

void clViewport::StoreAndDisableStickyKeys()
{
	guard();

	FStickyKeys.cbSize  = sizeof( FStickyKeys );
	FStickyKeys.dwFlags = 0;

	// save current value
	SystemParametersInfo( SPI_GETSTICKYKEYS, sizeof( FStickyKeys ), &FStickyKeys, 0 );

	STICKYKEYS StickyKeys = FStickyKeys;

	StickyKeys.dwFlags = 0;
	StickyKeys.dwFlags &= ~SKF_STICKYKEYSON;
	StickyKeys.dwFlags &= ~SKF_HOTKEYACTIVE;
	StickyKeys.dwFlags &= ~SKF_HOTKEYSOUND;
	StickyKeys.dwFlags &= ~SKF_AVAILABLE;

	// reset sticky keys
	SystemParametersInfo( SPI_SETSTICKYKEYS, sizeof( StickyKeys ), &StickyKeys, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE  );

	unguard();
}

void clViewport::RestoreStickyKeys() const
{
	guard();

	// restore sticky keys
	SystemParametersInfo( SPI_SETSTICKYKEYS, sizeof( FStickyKeys ), const_cast<STICKYKEYS*>( &FStickyKeys ), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE );

	unguard();
}
#endif

void clViewport::AfterConstruction()
{
#ifdef OS_WINDOWS
	DWORD dwVersion = 0;
	DWORD dwMajorVersion = 0;

	dwVersion = GetVersion();

	// Get the Windows version.
	dwMajorVersion = ( DWORD )( LOBYTE( LOWORD( dwVersion ) ) );

	if ( dwMajorVersion > 5 )
	{
		/// Anything below WinXP without SP3 does not support High DPI, so we do not enable it on WinXP at all.
		SetHighDPIAware();
	}

	StoreAndDisableStickyKeys();

	if ( FTakeoverContext )
	{
		ResizeViewport( 0, 0 );
	}
	else
#endif
	{
#ifdef OS_WINDOWS

		if ( FRegisterWindowClass )
		{
			RegisterWindowClass();
		}

		if ( FExternalWindowHandle )
		{
			RECT Rect;

			GetWindowRect( FExternalWindowHandle, &Rect );

			FWidth  = Rect.right  - Rect.left;
			FHeight = Rect.bottom - Rect.top;
		}

#endif
		RecreateViewport();
	}

	ReinitializeViewport();

	FMouseX = Env->Console->GetVar( "Viewport.MouseX" );
	FMouseY = Env->Console->GetVar( "Viewport.MouseY" );

	Env->Connect( L_EVENT_SURFACE_CHANGED, BIND( &clViewport::Event_SURFACE_CHANGED ) );
}

// purely windows stuff
#ifdef OS_WINDOWS
void clViewport::RegisterWindowClass()
{
	guard();

	WNDCLASS WindowClass;

	if ( GetClassInfo( GetModuleHandle( NULL ), FWindowClassName.c_str(), &WindowClass ) )
	{
		return;
	}

	WindowClass.style = CS_DBLCLKS;
	WindowClass.lpfnWndProc = MainWndProc;
	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = 4 * sizeof( LONG_PTR );
	WindowClass.hInstance = GetModuleHandle( NULL );
	WindowClass.hIcon = LoadIcon( GetModuleHandle( NULL ), IDI_APPLICATION );
	WindowClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	WindowClass.hbrBackground = 0;
	WindowClass.lpszMenuName = NULL;
	WindowClass.lpszClassName = FWindowClassName.c_str();

	FATAL( !RegisterClass( &WindowClass ), "Unable to register window class" );

	unguard();
}

void clViewport::UnRegisterWindowClass()
{
	RestoreStickyKeys();

	if ( !UnregisterClass( FWindowClassName.c_str(), GetModuleHandle( NULL ) ) )
	{
		// if we are running multiple instances of the engine the UnregisterClass() will fail - just ignore that error
		int ErrorCode = static_cast<int>( GetLastError() );

		FATAL( ErrorCode != ERROR_CLASS_HAS_WINDOWS, "Unable to unregister window class: " + LStr::ToStr( ErrorCode ) );
	}
}
#endif

void clViewport::SetViewportSize( int W, int H )
{
#if defined OS_WINDOWS
	SetWindowPos( FWindowHandle, NULL, 0, 0, W, H, 0 );
#elif defined OS_ANDROID
	TODO( "implement" )
#elif defined OS_LINUX
	XResizeWindow( FDisplay, *FWindowHandle, W, H );
#endif

	ResizeViewport( W, H );
}

// purely windows stuff
#ifdef OS_WINDOWS
LRESULT CALLBACK clViewport::MainWndProc( HWND Window, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	// NOTE: check for this in WinUser.h
#undef WM_MOUSEWHEEL
#undef WM_XBUTTONDOWN
#undef WM_XBUTTONUP
	const UINT WM_MOUSEWHEEL  = 522; // WM_MOUSELAST+1;
	const UINT WM_XBUTTONDOWN = 523; // WM_MOUSELAST+2;
	const UINT WM_XBUTTONUP   = 524; // WM_MOUSELAST+3;

#ifndef XBUTTON1
	const UINT XBUTTON1 = 1;
#endif

#ifndef XBUTTON2
	const UINT XBUTTON2 = 2;
#endif

	sEnvironment* Env = reinterpret_cast<sEnvironment*>( ( LONG_PTR )GetWindowLongPtr( Window, GWLP_USERDATA ) );

	if ( Env )
	{
		switch ( Msg )
		{
			case WM_ACTIVATE:
			{
				bool Deactivated = wParam == WA_INACTIVE;

				if ( Deactivated )
				{
					Env->SendSync( L_EVENT_ACTIVATE, LEventArgs( false ), false );

					// this will switch all CVars binded to keys into false
					for ( int Key = 0; Key != 255; ++Key )
					{
						Env->SendSync( L_EVENT_KEY, LEventArgs( Key, false ), false );
					}
				}
				else
				{
					Env->SendSync( L_EVENT_ACTIVATE, LEventArgs( true ), false );

					SetForegroundWindow( Window );
				}

				// is a fullscreen window ?
				if ( !( GetWindowLong( Window, GWL_STYLE ) & WS_CAPTION ) )
				{
					int Active = LOWORD( wParam );

					if ( Active != WA_INACTIVE )
					{
						// firts, restore fullscreen mode native to this window
						Env->Screen->RestoreDisplayMode();
						// restore the window itself
						ShowWindow( Window, SW_RESTORE );
						SetForegroundWindow( Window );
					}

					if ( Active == WA_INACTIVE )
					{
						// first, iconify the window
						ShowWindow( Window, SW_MINIMIZE );
						// only after that reset display mode
						Env->Screen->ResetDisplayMode();
					}
				}

				break;
			}
			case WM_HOTKEY:
			{
				return FOriginalWndProc ? CallWindowProc( FOriginalWndProc, Window, Msg, wParam, lParam ) : 0;
			}
			case WM_MOUSEWHEEL:
			{
				if ( static_cast<int>( wParam ) > 0 )
				{
					Env->SendSync( L_EVENT_KEY, LEventArgs( LK_WHEELUP, true ), false );
				}
				else
				{
					Env->SendSync( L_EVENT_KEY, LEventArgs( LK_WHEELDOWN, true ), false );
				}

				break;
			}
			case WM_SYSCOMMAND:
			{
				if ( wParam == SC_SCREENSAVE )
				{
					return FOriginalWndProc ? CallWindowProc( FOriginalWndProc, Window, Msg, wParam, lParam ) : 0;
				}

				break;
			}
			case WM_SIZE:
			{
				clViewport* Viewport = Env->Viewport;

				dynamic_cast<clViewport*>( Viewport )->ResizeViewport( 0, 0 );
				Env->Renderer->RestoreViewport();

				Env->SendSync( L_EVENT_VIEWPORTRESIZED, LEventArgs(), false );

				break;
			}
			case WM_CLOSE:
			{
				PostQuitMessage( 0 );
				break;

			}
			case WM_DEVICECHANGE:
			{

				break;
			}
			case WM_MOUSEMOVE:
			{
				int X = GET_X_LPARAM( lParam );
				int Y = GET_Y_LPARAM( lParam );

				Env->Viewport->MoveMouse( X, Y );
/*
				Env->SendSync( L_EVENT_MOTION, LEventArgs( -1, 0, 0, false ), false );
				Env->SendSync( L_EVENT_MOTION, LEventArgs(  0, Pos.x, Pos.y, Env->Console->IsKeyPressed( LK_LBUTTON ) ), false );
				Env->SendSync( L_EVENT_MOTION, LEventArgs( -2, 0, 0, false ), false );
*/
				LVector2 Pos = Env->Viewport->GetMousePosition();
				LEventArgs Args( 0, Pos.x, Pos.y, false );
				Args.FObjArg = (iObject*)L_MOTION_MOVE;
				Args.FBoolArg = Env->Console->IsKeyPressed( LK_LBUTTON );

				// make the gestures' handlers happy				
				Env->SendSync( L_EVENT_MOTION, LEventArgs( -1, 0, 0, false ), false );
				Env->SendSync( L_EVENT_MOTION, Args, false );
				Env->SendSync( L_EVENT_MOTION, LEventArgs( -2, 0, 0, false ), false );
				break;
			}
			case WM_XBUTTONDOWN:
			{
				SetCapture( Window );

				int Key = LK_XBUTTON1;

				switch HIWORD( wParam )
				{
					case XBUTTON1:
						Key = LK_XBUTTON1;
						break;
					case XBUTTON2:
						Key = LK_XBUTTON2;
						break;
					default:
						Env->Logger->Log( L_WARNING, "Unknown wParam in WM_XBUTTONDOWN: " + LStr::ToStr( LOWORD( wParam ) ) );
				}

				Env->SendSync( L_EVENT_KEY, LEventArgs( Key, true ), false );

				return FOriginalWndProc ? CallWindowProc( FOriginalWndProc, Window, Msg, wParam, lParam ) : 1;
			}
			case WM_XBUTTONUP:
			{
				ReleaseCapture();

				int Key = LK_XBUTTON1;

				switch HIWORD( wParam )
				{
					case XBUTTON1:
						Key = LK_XBUTTON1;
						break;
					case XBUTTON2:
						Key = LK_XBUTTON2;
						break;
					default:
						Env->Logger->Log( L_WARNING, "Unknown wParam in WM_XBUTTONUP: " + LStr::ToStr( LOWORD( wParam ) ) );
				}

				Env->SendSync( L_EVENT_KEY, LEventArgs( Key, false ), false );

				return FOriginalWndProc ? CallWindowProc( FOriginalWndProc, Window, Msg, wParam, lParam ) : 1;
			}
			case WM_LBUTTONDOWN:
			{
				SetForegroundWindow( Window );
				SetFocus( Window );

				SetCapture( Window );

				Env->SendSync( L_EVENT_KEY, LEventArgs( LK_LBUTTON, true ), false );

				LVector2 Pos = Env->Viewport->GetMousePosition();
				LEventArgs Args( 0, Pos.x, Pos.y, false );
				Args.FObjArg = (iObject*)L_MOTION_DOWN;
				Args.FBoolArg = true;

				// make the gestures' handlers happy				
				Env->SendSync( L_EVENT_MOTION, LEventArgs( -1, 0, 0, false ), false );
				Env->SendSync( L_EVENT_MOTION, Args, false );
				Env->SendSync( L_EVENT_MOTION, LEventArgs( -2, 0, 0, false ), false );

				return FOriginalWndProc ? CallWindowProc( FOriginalWndProc, Window, Msg, wParam, lParam ) : 0;
			}
			case WM_LBUTTONUP:
			{
				ReleaseCapture();

				Env->SendSync( L_EVENT_KEY, LEventArgs( LK_LBUTTON, false ), false );

				LVector2 Pos = Env->Viewport->GetMousePosition();
				LEventArgs Args( 0, Pos.x, Pos.y, false );
				Args.FObjArg = (iObject*)L_MOTION_UP;
				Args.FBoolArg = false;

				// make the gestures' handlers happy
				Env->SendSync( L_EVENT_MOTION, LEventArgs( -1, 0, 0, false ), false );
				Env->SendSync( L_EVENT_MOTION, Args, false );
				Env->SendSync( L_EVENT_MOTION, LEventArgs( -2, 0, 0, false ), false );

				return FOriginalWndProc ? CallWindowProc( FOriginalWndProc, Window, Msg, wParam, lParam ) : 0;
			}
			case WM_RBUTTONDOWN:
			{
				SetCapture( Window );

				Env->SendSync( L_EVENT_KEY, LEventArgs( LK_RBUTTON, true ), false );

				return FOriginalWndProc ? CallWindowProc( FOriginalWndProc, Window, Msg, wParam, lParam ) : 0;
			}
			case WM_RBUTTONUP:
			{
				ReleaseCapture();

				Env->SendSync( L_EVENT_KEY, LEventArgs( LK_RBUTTON, false ), false );

				return FOriginalWndProc ? CallWindowProc( FOriginalWndProc, Window, Msg, wParam, lParam ) : 0;
			}
			case WM_MBUTTONDOWN:
			{
				SetCapture( Window );

				Env->SendSync( L_EVENT_KEY, LEventArgs( LK_MBUTTON, true ), false );

				return FOriginalWndProc ? CallWindowProc( FOriginalWndProc, Window, Msg, wParam, lParam ) : 0;
			}
			case WM_MBUTTONUP:
			{
				ReleaseCapture();

				Env->SendSync( L_EVENT_KEY, LEventArgs( LK_MBUTTON, false ), false );

				return FOriginalWndProc ? CallWindowProc( FOriginalWndProc, Window, Msg, wParam, lParam ) : 0;
			}
			case WM_LBUTTONDBLCLK:
			{
				Env->SendSync( L_EVENT_KEY, LEventArgs( LK_LBUTTONDB, true ), false );

				return FOriginalWndProc ? CallWindowProc( FOriginalWndProc, Window, Msg, wParam, lParam ) : 0;
			}
			case WM_RBUTTONDBLCLK:
			{
				Env->SendSync( L_EVENT_KEY, LEventArgs( LK_RBUTTONDB, true ), false );

				return FOriginalWndProc ? CallWindowProc( FOriginalWndProc, Window, Msg, wParam, lParam ) : 0;
			}
			case WM_SYSKEYUP:
			case WM_KEYUP:
			{
				Env->SendSync( L_EVENT_KEY, LEventArgs( static_cast<int>( wParam ), false ), false );

				return FOriginalWndProc ? CallWindowProc( FOriginalWndProc, Window, Msg, wParam, lParam ) : 0;
			}
			case WM_SYSKEYDOWN:
			{
				if ( wParam == 13 )
				{
					TODO( "toggle fullscreen here" )
				}
			}
			case WM_KEYDOWN:
			{
				Env->SendSync( L_EVENT_KEY, LEventArgs( static_cast<int>( wParam ), true ), false );

				return FOriginalWndProc ? CallWindowProc( FOriginalWndProc, Window, Msg, wParam, lParam ) : 0;
			}
		}
	}

	return FOriginalWndProc ? CallWindowProc( FOriginalWndProc, Window, Msg, wParam, lParam ) : DefWindowProc( Window, Msg, wParam, lParam );
}
#endif

#if defined(OS_LINUX)

#if !defined(OS_ANDROID)
void clViewport::RecreateViewport()
{
	if ( FWindow )
	{
		XDestroyWindow( FDisplay, *FWindow );
	}

	if ( FFullscreen )
	{
	}
	else
	{
	}

	static char EmptyData[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	FBlackColor.red   = 0;
	FBlackColor.green = 0;
	FBlackColor.blue  = 0;

	FBitmapEmptyData = XCreateBitmapFromData( FDisplay, *FWindow, EmptyData, 8, 8 );

	FLeftArrowCursor = XCreateFontCursor( FDisplay, XC_left_ptr );
	FInvisibleCursor = XCreatePixmapCursor( FDisplay, FBitmapEmptyData, FBitmapEmptyData, &FBlackColor, &FBlackColor, 0, 0 );
}

void clViewport::ReinitializeViewport()
{
	// FIXME: implement
}

#else // OS_ANDROID

void clViewport::RecreateViewport()
{
	// FIXME: implement
}

void clViewport::ReinitializeViewport()
{
#ifdef ANDROID9
	// FIXME: implement
	FDeviceHandle = eglGetDisplay( EGL_DEFAULT_DISPLAY );
#endif // ANDROID9
}

#endif // OS_ANDROID

#endif // OS_LINUX

#ifdef OS_WINDOWS
void clViewport::RecreateViewport()
{
	if ( FWindowHandle )
	{
		SetWindowLongPtrA( FWindowHandle, GWLP_USERDATA, 0 );

		ReleaseDC( FWindowHandle, FDeviceHandle );
		DestroyWindow( FWindowHandle );
	}

	DWORD dwExStyle;
	DWORD dwStyle;

	int OriginX = 0;
	int OriginY = 0;

	// setup fullscreenness
	if ( FFullscreen )
	{
		dwExStyle = 0;
		dwStyle   = WS_POPUP;

		OriginX = 0;
		OriginY = 0;
	}
	else
	{
		dwExStyle = 0;
		dwStyle   = WS_OVERLAPPED | WS_BORDER | WS_CAPTION;

		const int SW = GetSystemMetrics( SM_CXFULLSCREEN );
		const int SH = GetSystemMetrics( SM_CYFULLSCREEN );

		if ( FHeight > SH )
		{
			FHeight = SH;
		}

		if ( FWidth  > SW )
		{
			FWidth  = SW;
		}

		OriginX = ( GetSystemMetrics( SM_CXFULLSCREEN ) - FWidth  ) / 2;
		OriginY = ( GetSystemMetrics( SM_CYFULLSCREEN ) - FHeight ) / 2;
	}

	if ( FExternalWindowHandle )
	{
		OriginX = 0;
		OriginY = 0;

		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE | WS_EX_TOPMOST;
		dwStyle   = WS_CHILDWINDOW | WS_BORDER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}

	if ( FDisplayWindow )
	{
		dwStyle |= WS_VISIBLE;
	}

	RECT Rect;

	Rect.left = 0;
	Rect.top = 0;
	Rect.right  = FWidth;
	Rect.bottom = FHeight;

	AdjustWindowRect( &Rect, dwStyle, false );

	const int WinWidth  = Rect.right  - Rect.left;
	const int WinHeight = Rect.bottom - Rect.top;

	FWindowHandle = CreateWindowEx( dwExStyle,
	                                FWindowClassName.c_str(),
	                                FTitle.c_str(),
	                                dwStyle,
	                                OriginX, OriginY,
	                                WinWidth, WinHeight,
	                                FExternalWindowHandle, 0,
	                                GetModuleHandle( NULL ),
	                                NULL );
}
#endif

// parameters are not used in Windows
void clViewport::ResizeViewport( int Width, int Height )
{
#if defined(OS_WINDOWS)
	RECT Rect;

	GetClientRect( FWindowHandle, &Rect );

	FWidth  = Rect.right  - Rect.left;
	FHeight = Rect.bottom - Rect.top;
	FTop  = Rect.top;
	FLeft = Rect.left;
#elif defined(OS_LINUX) && !defined(OS_ANDROID)
	Window* rootRet;
	int* xRet;
	int* yRet;
	unsigned int* widthRet, *heightRet;
	unsigned int* borderWidthRet;
	unsigned int* depthRet;

	XGetGeometry( FDisplay, *FWindow, rootRet, xRet, yRet, widthRet, heightRet, borderWidthRet, depthRet );
	FWidth  = Width;
	FHeight = Height;
	FLeft = static_cast<unsigned int>( *xRet ) + *borderWidthRet;
	FTop = static_cast<unsigned int>( *yRet ) + *borderWidthRet;
#elif defined( OS_ANDROID )
	FWidth  = Width;
	FHeight = Height;
#endif

	FScaleFactorX = 1.0f / FWidth;
	FScaleFactorY = 1.0f / FHeight;
}

#ifdef OS_WINDOWS
void clViewport::ReinitializeViewport()
{
	FATAL( !FWindowHandle, "Window Creation Error" );

	SetWindowLongPtrA( FWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( Env ) );

	FDeviceHandle = GetDC( FWindowHandle );

	if ( FTakeoverContext )
	{
		FOriginalWndProc = ( WNDPROC )SetWindowLongPtr( FWindowHandle, GWLP_WNDPROC, ( LONG_PTR )MainWndProc );
	}
	else
	{
		// bring to front
		if ( FDisplayWindow )
		{
			ShowWindow( FWindowHandle, SW_SHOW );
			UpdateWindow( FWindowHandle );
			SetFocus( FWindowHandle );
			SetForegroundWindow( FWindowHandle );
		}

		FScaleFactorX = 1.0f / FWidth;
		FScaleFactorY = 1.0f / FHeight;
	}
}
#endif

void clViewport::BlitBitmap( int X, int Y, clBitmap* Bmp )
{
	if ( Bmp->FBitmapParams.FBitmapFormat != L_BITMAP_BGR8 )
	{
		return;
	}

	int _W = Bmp->FBitmapParams.FWidth;
	int _H = Bmp->FBitmapParams.FHeight;

#ifdef OS_WINDOWS

	// 1. create bitmap
	HBITMAP _hBitmap = ::CreateCompatibleBitmap ( FDeviceHandle, _W, _H );

	// 1.1. fill bitmap information
	BITMAPINFO bi_dst;
	ZeroMemory( &bi_dst, sizeof( bi_dst ) );
	bi_dst.bmiHeader.biSize = sizeof( bi_dst.bmiHeader );
	bi_dst.bmiHeader.biWidth = _W;
	bi_dst.bmiHeader.biHeight = _H;
	bi_dst.bmiHeader.biPlanes = 1;
	bi_dst.bmiHeader.biBitCount = 24;
	bi_dst.bmiHeader.biCompression = BI_RGB;

	// 1.2. fill it with data
	/*int RetCode =*/
	SetDIBits( FDeviceHandle, _hBitmap, 0, _H, &( Bmp->FBitmapData[0] ), &bi_dst, DIB_RGB_COLORS );

	// 2. blit to the screen

	// 2.1. create temporary DC
	HDC _hTempDC = CreateCompatibleDC( FDeviceHandle );

	// 2.2. select source bitmap there
	SelectObject( _hTempDC, _hBitmap );

	// 2.3. blit temporary DC to our window
	/*bool Ret =*/
	BitBlt( FDeviceHandle, X, Y, _W, _H, _hTempDC, 0, 0, SRCCOPY );

	// 3. finish painting
	DeleteObject( _hBitmap );
	DeleteDC( _hTempDC );
#endif

#if defined(OS_LINUX) && !defined(OS_ANDROID)
	TODO( "check BitmapFormat and convert bits if necessary" )
	memcpy( ( void* )( FTmpImage->data ), Bmp->FBitmapData, Bmp->FBitmapParams.GetStorageSize() );

	// Present to window/screen
	XPutImage( FDisplay, *FWindow, FWindowGC, FTmpImage, 0, 0, X, Y, _W, _H );
#endif
}

inline int FloatTo0_255( float x )
{
	int a = static_cast<int>( x * 255.0f );

	if ( a < 0   )
	{
		a = 0;
	}

	if ( a > 255 )
	{
		a = 255;
	}

	return a;
}

void clViewport::Clear( const LVector4& ClearColor )
{
#ifdef OS_WINDOWS
	COLORREF Color = RGB( FloatTo0_255( ClearColor.X ),
	                      FloatTo0_255( ClearColor.Y ),
	                      FloatTo0_255( ClearColor.Z ) );

	COLORREF ColorOld = SetBkColor( FDeviceHandle, Color );

	RECT Rect;

	GetClientRect( FWindowHandle, &Rect );

	ExtTextOut( FDeviceHandle, 0, 0, ETO_OPAQUE, &Rect, NULL, 0, NULL );
	SetBkColor( FDeviceHandle, ColorOld );
#endif
}

void clViewport::DrawLine( int X1, int Y1, int X2, int Y2 )
{
#ifdef OS_WINDOWS
	MoveToEx( FDeviceHandle, X1, Y1, 0 );
	LineTo( FDeviceHandle, X2, Y2 );
#endif
}

void clViewport::DrawText( int X, int Y, const LString& Text )
{
#ifdef OS_WINDOWS
	TextOut( FDeviceHandle, X, Y, Text.c_str(), static_cast<int>( Text.length() ) );
#endif
}

clViewport::~clViewport()
{
	guard();

	Env->DisconnectObject( this );

#ifdef OS_WINDOWS
	Env->Logger->Log( L_DEBUG, "Resetting window user data..." );

	// reset responder
	SetWindowLongPtrA( FWindowHandle, GWLP_USERDATA, 0 );

	Env->Logger->Log( L_DEBUG, "Releasing DC..." );

	ReleaseDC( FWindowHandle, FDeviceHandle );

	if ( !FTakeoverContext )
	{
		Env->Logger->Log( L_DEBUG, "Destroying window..." );

		DestroyWindow( FWindowHandle );

		Env->Logger->Log( L_DEBUG, "Unregistering window class..." );

		if ( FRegisterWindowClass )
		{
			UnRegisterWindowClass();
		}
	}

#endif

#if defined(OS_LINUX) && !defined(OS_ANDROID)
	XFreeCursor( FDisplay, FLeftArrowCursor );
	XFreeCursor( FDisplay, FInvisibleCursor );

	XFreePixmap( FDisplay, FBitmapEmptyData );

	XDestroyWindow( FDisplay, *FWindow );
//   XFreeColormap( Disp, ColorMap );
	XCloseDisplay( FDisplay );
#endif

	unguard();

}

void clViewport::SetViewportTitle( const LString& Title )
{
#ifdef OS_WINDOWS
	SetWindowText( FWindowHandle, Title.c_str() );
#endif

#if defined(OS_LINUX) && !defined(OS_ANDROID)
	XSetStandardProperties( FDeviceHandle, *FWindowHandle, Title.c_str(), Title.c_str(), None, NULL, 0, NULL );
#endif
}

int clViewport::GetWidth() const
{
	return FWidth;
}

int clViewport::GetHeight() const
{
	return FHeight;
}

void clViewport::MoveMouse( int X, int Y )
{
	FMouseCursorInfo.FMouseMoved = true;

	FMouseCursorInfo.FOldMouseX = FMouseCursorInfo.FMouseX;
	FMouseCursorInfo.FOldMouseY = FMouseCursorInfo.FMouseY;

	FMouseCursorInfo.FMouseX = Math::Clamp( X * FScaleFactorX, 0.0f, 1.0f );
	FMouseCursorInfo.FMouseY = Math::Clamp( Y * FScaleFactorY, 0.0f, 1.0f );

	FMouseX->SetFloat( FMouseCursorInfo.FMouseX );
	FMouseY->SetFloat( FMouseCursorInfo.FMouseY );
}

LVector2 clViewport::TransformPointerPosition( int X, int Y )
{
	float fX = Math::Clamp( X * FScaleFactorX, 0.0f, 1.0f );
	float fY = Math::Clamp( Y * FScaleFactorY, 0.0f, 1.0f );

	return LVector2( fX, fY );
}

const sMouseCursorInfo& clViewport::GetMouseCursorInfo() const
{
	return FMouseCursorInfo;
}

void clViewport::SetMousePosition( float X, float Y )
{
#if defined(OS_WINDOWS)
	POINT Pnt;
	Pnt.x = static_cast<int>( X * FWidth );
	Pnt.y = static_cast<int>( Y * FHeight );
	ClientToScreen( FWindowHandle, &Pnt );
	SetCursorPos( Pnt.x, Pnt.y );
#elif defined(OS_LINUX) && !defined(OS_ANDROID)
	XWarpPointer( FDisplay, None, *FWindow, 0, 0, 0, 0, static_cast<int>( X * FWidth ), static_cast<int>( Y * FHeight ) );
#elif defined(OS_ANDROID)
	TODO( "Implement" );
#else
#error Target OS not recognized
#endif
}

LVector2 clViewport::GetMousePosition() const
{
	return LVector2( FMouseCursorInfo.FMouseX, FMouseCursorInfo.FMouseY );
}

void clViewport::ShowOSMouseCursor()
{
#ifdef OS_WINDOW
	ShowCursor( true );
#endif

#if defined(OS_LINUX) && !defined(OS_ANDROID)
	XDefineCursor( FDisplay, *FWindow, FLeftArrowCursor );
#endif
}

void clViewport::HideOSMouseCursor()
{
#ifdef OS_WINDOW
	ShowCursor( false );
#endif

#if defined(OS_LINUX) && !defined(OS_ANDROID)
	XDefineCursor( FDisplay, *FWindow, FInvisibleCursor );
#endif
}

void clViewport::UpdateTrackball( clVirtualTrackball* Trackball, float Speed, bool Keypressed ) const
{
	if ( Keypressed )
	{
		Trackball->DragTo( LVector2( FMouseCursorInfo.FMouseX, FMouseCursorInfo.FMouseY ), Speed );
	}
	else
	{
		Trackball->StartDragging( LVector2( FMouseCursorInfo.FMouseX, FMouseCursorInfo.FMouseY ) );
	}
}

LVector2 clViewport::MousePositionIntToFloat( int X, int Y ) const
{
	return LVector2( X * FScaleFactorX, Y * FScaleFactorY );
}

void clViewport::SwapBuffers()
{
#ifdef OS_WINDOWS
	::SwapBuffers( GetDevice() );
#endif

#if defined(OS_LINUX) && !defined(OS_ANDROID)
	glXSwapBuffers( GetDevice(), *( this->GetWindow() ) );
#endif

#if defined(OS_ANDROID) && defined( ANDROID9 )
	eglSwapBuffers( GetDevice(), this->GetWindow() );
#endif
}

// return true if everything went fine
// return false if the execution should be stopped
bool clViewport::DispatchEvents()
{
	/// OS-specific loop ///
#ifdef OS_WINDOWS
	MSG AMessage;

	while ( PeekMessage( &AMessage, 0, 0, 0, PM_NOREMOVE ) )
	{
		if ( !GetMessage( &AMessage, 0, 0, 0 ) )
		{
			return false;
		}

		TranslateMessage( &AMessage );
		DispatchMessage( &AMessage );
	}

#endif

#if defined(OS_LINUX) && !defined(OS_ANDROID)
	KeySym Key;
	XEvent Event;

	while ( XPending( FDisplay ) )
	{
		XNextEvent( FDisplay, &Event );

		switch ( Event.type )
		{
			case KeyPress:
				XLookupString( ( XKeyEvent* )&Event, NULL, 0, &Key, NULL );
				break;

			case ConfigureNotify:
				ResizeViewport( Event.xconfigure.width, Event.xconfigure.height );
				break;
		}
	}

	TODO( "implement" )
#endif

	return true;
}

/*
 * 19/08/2010
     Initial Xlib code
 * 20/04/2009
     MousePositionIntToFloat()
 * 07/04/2009
     ShowOSMouseCursor()
     HideOSMouseCursor()
 * 26/02/2007
     SetViewportTitleC()
 * 14/01/2007
     SetViewportTitle()
 * 13/11/2006
     ReinitializeViewport()
 * 02/11/2005
     NULL changed to 0 in ~clViewport() to make it GCC-compatible
 * 01/11/2005
     Fixed detachment of Responder from viewport on destruction
 * 07/04/2005
     Mouse cursor bug fixed
 * 07/02/2005
     Fixed adjustment of window's sizes to match desired width & height
 * 12/12/2004
     OriginX & OriginY selection, depending of fulscreenness
 * 05/12/2004
     First working version
 * 04/12/2004
     It's here
*/
