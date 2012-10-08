#ifndef __LWinInput__h__included__
#define __LWinInput__h__included__

#include "Platform.h"

class clLibrary;

/**
  DirectInput is always used (because it is linked dynamically) under Win32.
*/
#if defined(OS_WINDOWS)
#define L_USE_DIRECTINPUT
#else
#undef L_USE_DIRECTINPUT
#endif

#if defined(L_USE_DIRECTINPUT)

#define DIRECTINPUT_VERSION 0x0800

#define CINTERFACE
#define COBJMACROS

#include <windows.h>
#include "ExtLibs/DInput/dinput.h"

#undef CINTERFACE
#undef COBJMACROS

/// factory function prototype for dynamic loading of dinput8.dll
typedef HRESULT ( WINAPI* DirectInput8Create_func )( HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, void* punkOther );

#endif // L_USE_DIRECTINPUT

#define MAXJOYBUTTONS (256)

#include "Input/Input.h"

class LDirectInputJoystick;

/// WinAPI devices
class scriptfinal LWinDeviceList : public iInputDeviceList
{
public:
	LWinDeviceList();
	virtual ~LWinDeviceList();

	virtual void            AfterConstruction();

	/// Build a complete list of available devices
	virtual bool            ReenumerateDevices();

public:
	bool EnumerateSystemDevices();
	bool EnumerateDirectInputDevices();

	/// DirectInput stuff

	/// Pass the HWND to which the joystick will be attached.
	virtual void SetWindowHandle( LWindowHandle Wnd );

	/// Create the device for i-th enumerated device GUID
	iInputDevice* CreateDeviceForIdx( int Idx );

	/// Run the Control Panel
	void RunControlPanel( void );

private:
	/// Initialise Direct Input 8
	bool Initialise( void );

	/// Has Direct Input Been Initialised?
	bool m_Initialised;

	/// Close down and de-allocate any memory assigned to this object.
	void Shutdown( void );

	/// Start Enumeration of Attached Joystick Devices
	bool Enumerate_Joysticks( void );

	/// ReInitialise DI8 (Not Implemented)
	bool ReInitialise( void );

#ifdef L_USE_DIRECTINPUT
	/// Set the current GUID for joystick device.
	LDirectInputJoystick* CreateDeviceFor( const GUID* pguid );

	/// list of Attached Joystick Devices
	std::vector<LPCDIDEVICEINSTANCE> m_DIJoystickList;

	/// Add DI8 Device Info for Enumerated Device
	bool AddDeviceInfo( LPCDIDEVICEINSTANCE lpddi );

	/// Callback procedure for Enumerating Attached Joystick Devices
	noexport static BOOL CALLBACK EnumDevicesProc( LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef );

	/// Wait Cursor Handle when Enumerating Feedback Effects
	HCURSOR m_hCursorWait;

	/// This modules Instance
	HINSTANCE m_hInstance;

	/// Pointer to Direct Input 8
	IDirectInput8A*  m_lpDI;

	/// The only function we export from dinput8.dll
	DirectInput8Create_func DirectInput8Create_Function;

	/// dinput8.dll handle
	clLibrary* hDInputDll;
#endif
};

#endif

/*
 * 11/08/2010
     Dynamic linking with dinput8.dll
     Code cleanup
 * 11/06/2010
     Log section added
*/
