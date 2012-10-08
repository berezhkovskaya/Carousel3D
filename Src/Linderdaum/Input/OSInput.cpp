//#ifdef _WIN32

/// This code is based entirely on the work of Jason Brooks (taken from codeguru.com)
/// DirectInput (dfDIJoy2 structure) initialization taken from MinGW/W32API (lib/dinput.c)

// DIJoystick.cpp: implementation of the CDIJoystick class.
//
// Written and Developed by Jason Brooks
// (C) 2000 Jason Brooks
//
// You may use this code freely, however a mention in your credits
// would be nice.
//
// For more information, Bug Reports and so on I can be contacted :-
//
// E-Mail:   DirectInput@muckypaws.com
//
// Web:      www.muckypaws.com
// ICQ:      9609400
//

#include "Platform.h"

#include "OSInput.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using std::cout;
using std::endl;

#ifdef L_USE_DIRECTINPUT

#define CINTERFACE
#define COBJMACROS

#include <windows.h>
#include "ExtLibs/DInput/dinput.h"

#undef CINTERFACE
#undef COBJMACROS

#include "Environment.h"
#include "Utils/Library.h"

/// Convert HRESULT ot Error Text
LString GetDIError( HRESULT error )
{
#define DI_STRING(Err, Msg) case Err : return LString(#Err) + LString(" : ") + LString(Msg); break;

	switch ( error )
	{
			DI_STRING( E_PENDING, "Data Not Available.\n" )
			DI_STRING( E_HANDLE, "The HWND parameter is not a valid top-level window that belongs to the process.\n" )
			DI_STRING( DIERR_UNSUPPORTED, "The function called is not supported at this time. This value is equal to the E_NOTIMPL standard COM return value.\n" )
			DI_STRING( DIERR_READONLY, "The specified property cannot be changed. This value is equal to the E_ACCESSDENIED standard COM return value.\n" )
			DI_STRING( DIERR_OUTOFMEMORY, "The DirectInput subsystem could not allocate sufficient memory to complete the call. This value is equal to the E_OUTOFMEMORY standard COM return value.\n" )
			DI_STRING( DIERR_OLDDIRECTINPUTVERSION, "The application requires a newer version of DirectInput.\n" )
			DI_STRING( DIERR_OBJECTNOTFOUND, "The requested object does not exist.\n" )
			DI_STRING( DIERR_NOTINITIALIZED, "This object has not been initialized.\n" )
			DI_STRING( DIERR_NOTEXCLUSIVEACQUIRED, "The operation cannot be performed unless the device is acquired in DISCL_EXCLUSIVE mode.\n" )
			DI_STRING( DIERR_NOTDOWNLOADED, "The effect is not downloaded.\n" )
			DI_STRING( DIERR_NOTBUFFERED, "The device is not buffered. Set the DIPROP_BUFFERSIZE property to enable buffering.\n" )
			DI_STRING( DIERR_NOTACQUIRED, "The operation cannot be performed unless the device is acquired.\n" )
			DI_STRING( DIERR_NOINTERFACE, "The specified interface is not supported by the object. This value is equal to the E_NOINTERFACE standard COM return value.\n" )
			DI_STRING( DIERR_NOAGGREGATION, "This object does not support aggregation.\n" )
			DI_STRING( DIERR_MOREDATA, "Not all the requested information fit into the buffer.\n" )
			DI_STRING( DIERR_INVALIDPARAM, "An invalid parameter was passed to the returning function, or the object was not in a state that permitted the function to be called. This value is equal to the E_INVALIDARG standard COM return value.\n" )
			DI_STRING( DIERR_INPUTLOST, "Access to the input device has been lost. It must be reacquired.\n" )
			DI_STRING( DIERR_INCOMPLETEEFFECT, "The effect could not be downloaded because essential information is missing. For example, no axes have been associated with the effect, or no type-specific information has been supplied.\n" )
			DI_STRING( DIERR_GENERIC, "An undetermined error occurred inside the DirectInput subsystem. This value is equal to the E_FAIL standard COM return value.\n" )
			DI_STRING( DIERR_HASEFFECTS, "The device cannot be reinitialized because there are still effects attached to it.\n" )
			DI_STRING( DIERR_EFFECTPLAYING, "The parameters were updated in memory but were not downloaded to the device because the device does not support updating an effect while it is still playing.\n" )
			DI_STRING( DIERR_DEVICENOTREG, "The device or device instance is not registered with DirectInput. This value is equal to the REGDB_E_CLASSNOTREG standard COM return value.\n" )
			DI_STRING( DIERR_DEVICEFULL, "The device is full.\n" )
			DI_STRING( DIERR_BETADIRECTINPUTVERSION, "The application was written for an unsupported prerelease version of DirectInput.\n" )
			DI_STRING( DIERR_BADDRIVERVER, "The object could not be created due to an incompatible driver version or mismatched or incomplete driver components.\n" )
			DI_STRING( DIERR_ALREADYINITIALIZED, "This object is already initialized\n" )
			DI_STRING( DIERR_ACQUIRED, "The operation cannot be performed while the device is acquired.\n" )
			DI_STRING( DI_TRUNCATEDANDRESTARTED, "Equal to DI_EFFECTRESTARTED | DI_TRUNCATED\n" )
			DI_STRING( DI_TRUNCATED, "The parameters of the effect were successfully updated, but some of them were beyond the capabilities of the device and were truncated to the nearest supported value.\n" )
			DI_STRING( DI_PROPNOEFFECT, "The change in device properties had no effect. This value is equal to the S_FALSE standard COM return value.\n" )
			DI_STRING( DI_POLLEDDEVICE, "The device is a polled device. As a result, device buffering does not collect any data and event notifications is not signaled until the IDirectInputDevice7::Poll method is called.\n" )
			DI_STRING( DI_OK, "The operation completed successfully. This value is equal to the S_OK standard COM return value.\n" )
			DI_STRING( DI_EFFECTRESTARTED, "The effect was stopped, the parameters were updated, and the effect was restarted.\n" )
			DI_STRING( DI_DOWNLOADSKIPPED, "The parameters of the effect were successfully updated, but the effect could not be downloaded because the associated device was not acquired in exclusive mode.\n" )
// DI_STRING(DIERR_HANDLEEXISTS, "The device already has an event notification associated with it. This value is equal to the E_ACCESSDENIED standard COM return value.\n")
// DI_STRING(DIERR_OTHERAPPHASPRIO, "Another application has a higher priority level, preventing this call from succeeding. This value is equal to the E_ACCESSDENIED standard COM return value. This error can be returned when an application has only foreground access to a device but is attempting to acquire the device while in the background. ")
// DI_STRING(DIERR_NOTFOUND, "The requested object does not exist.\n")
// DI_STRING(DI_NOTATTACHED, "The device exists but is not currently attached. This value is equal to the S_FALSE standard COM return value.\n")
// DI_STRING(DI_NOEFFECT, "The operation had no effect. This value is equal to the S_FALSE standard COM return value.\n")
// DI_STRING(DI_BUFFEROVERFLOW, "The device buffer overflowed and some input was lost. This value is equal to the S_FALSE standard COM return value.\n")
#ifdef _MSC_VER
// No such codes in MinGW ?
			DI_STRING( DIERR_UNPLUGGED, "The operation could not be completed because the device is not plugged in.\n" )
			DI_STRING( DIERR_REPORTFULL, "More information was requested to be sent than can be sent to the device.\n" )
#endif

		default:
			return LString( "Unknown Error Code.\n" );
	}

#undef DI_STRING
}

#define DEFINE_GUID_Local(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) const GUID name = { l,w1,w2,{ b1,b2,b3,b4,b5,b6,b7,b8 } }

DEFINE_GUID_Local( LocalConstant_IID_IDirectInput8A, 0xBF798030, 0x483A, 0x4DA2, 0xAA, 0x99, 0x5D, 0x64, 0xED, 0x36, 0x97, 0x00 );

DEFINE_GUID_Local( Local_GUID_XAxis,   0xA36D02E0, 0xC9F3, 0x11CF, 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 );
DEFINE_GUID_Local( Local_GUID_YAxis,   0xA36D02E1, 0xC9F3, 0x11CF, 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 );
DEFINE_GUID_Local( Local_GUID_ZAxis,   0xA36D02E2, 0xC9F3, 0x11CF, 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 );
DEFINE_GUID_Local( Local_GUID_RxAxis,  0xA36D02F4, 0xC9F3, 0x11CF, 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 );
DEFINE_GUID_Local( Local_GUID_RyAxis,  0xA36D02F5, 0xC9F3, 0x11CF, 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 );
DEFINE_GUID_Local( Local_GUID_RzAxis,  0xA36D02E3, 0xC9F3, 0x11CF, 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 );
DEFINE_GUID_Local( Local_GUID_Slider,  0xA36D02E4, 0xC9F3, 0x11CF, 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 );

DEFINE_GUID_Local( Local_GUID_Button,  0xA36D02F0, 0xC9F3, 0x11CF, 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 );
DEFINE_GUID_Local( Local_GUID_Key,     0x55728220, 0xD33C, 0x11CF, 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 );

DEFINE_GUID_Local( Local_GUID_POV,     0xA36D02F2, 0xC9F3, 0x11CF, 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 );

DEFINE_GUID_Local( Local_GUID_Unknown, 0xA36D02F3, 0xC9F3, 0x11CF, 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 );

#define DIFMT_JoyAxis (DIDFT_ABSAXIS|DIDFT_RELAXIS|DIDFT_ANYINSTANCE|DIDFT_OPTIONAL)
#define DIFMT_OurButton (DIDFT_BUTTON|DIDFT_ANYINSTANCE|DIDFT_OPTIONAL)

/// Data format description for the gamepads.
TODO( "fill it in a cycle, avoid copy'n'paste" )

DIOBJECTDATAFORMAT c_rgodfDIJoy2[] =
{
	{&Local_GUID_XAxis, 0x0,  DIFMT_JoyAxis, DIDOI_ASPECTPOSITION},
	{&Local_GUID_YAxis, 0x4,  DIFMT_JoyAxis, DIDOI_ASPECTPOSITION},
	{&Local_GUID_ZAxis, 0x8,  DIFMT_JoyAxis, DIDOI_ASPECTPOSITION},
	{&Local_GUID_RxAxis, 0xc, DIFMT_JoyAxis, DIDOI_ASPECTPOSITION},
	{&Local_GUID_RyAxis, 0x10, DIFMT_JoyAxis, DIDOI_ASPECTPOSITION},
	{&Local_GUID_RzAxis, 0x14, DIFMT_JoyAxis, DIDOI_ASPECTPOSITION},
	{&Local_GUID_Slider, 0x18, DIFMT_JoyAxis, DIDOI_ASPECTPOSITION},
	{&Local_GUID_Slider, 0x1c, DIFMT_JoyAxis, DIDOI_ASPECTPOSITION},
	{&Local_GUID_POV, 0x20, DIDFT_POV | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0x0},
	{&Local_GUID_POV, 0x24, DIDFT_POV | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0x0},
	{&Local_GUID_POV, 0x28, DIDFT_POV | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0x0},
	{&Local_GUID_POV, 0x2c, DIDFT_POV | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0x0},
	{NULL, 0x30, DIFMT_OurButton, 0x0}, {NULL, 0x31, DIFMT_OurButton, 0x0}, {NULL, 0x32, DIFMT_OurButton, 0x0}, {NULL, 0x33, DIFMT_OurButton, 0x0},
	{NULL, 0x34, DIFMT_OurButton, 0x0}, {NULL, 0x35, DIFMT_OurButton, 0x0}, {NULL, 0x36, DIFMT_OurButton, 0x0}, {NULL, 0x37, DIFMT_OurButton, 0x0},
	{NULL, 0x38, DIFMT_OurButton, 0x0}, {NULL, 0x39, DIFMT_OurButton, 0x0}, {NULL, 0x3a, DIFMT_OurButton, 0x0}, {NULL, 0x3b, DIFMT_OurButton, 0x0},
	{NULL, 0x3c, DIFMT_OurButton, 0x0}, {NULL, 0x3d, DIFMT_OurButton, 0x0}, {NULL, 0x3e, DIFMT_OurButton, 0x0}, {NULL, 0x3f, DIFMT_OurButton, 0x0},
	{NULL, 0x40, DIFMT_OurButton, 0x0}, {NULL, 0x41, DIFMT_OurButton, 0x0}, {NULL, 0x42, DIFMT_OurButton, 0x0}, {NULL, 0x43, DIFMT_OurButton, 0x0},
	{NULL, 0x44, DIFMT_OurButton, 0x0}, {NULL, 0x45, DIFMT_OurButton, 0x0}, {NULL, 0x46, DIFMT_OurButton, 0x0}, {NULL, 0x47, DIFMT_OurButton, 0x0},
	{NULL, 0x48, DIFMT_OurButton, 0x0}, {NULL, 0x49, DIFMT_OurButton, 0x0}, {NULL, 0x4a, DIFMT_OurButton, 0x0}, {NULL, 0x4b, DIFMT_OurButton, 0x0},
	{NULL, 0x4c, DIFMT_OurButton, 0x0}, {NULL, 0x4d, DIFMT_OurButton, 0x0}, {NULL, 0x4e, DIFMT_OurButton, 0x0}, {NULL, 0x4f, DIFMT_OurButton, 0x0},
	{NULL, 0x50, DIFMT_OurButton, 0x0}, {NULL, 0x51, DIFMT_OurButton, 0x0}, {NULL, 0x52, DIFMT_OurButton, 0x0}, {NULL, 0x53, DIFMT_OurButton, 0x0},
	{NULL, 0x54, DIFMT_OurButton, 0x0}, {NULL, 0x55, DIFMT_OurButton, 0x0}, {NULL, 0x56, DIFMT_OurButton, 0x0}, {NULL, 0x57, DIFMT_OurButton, 0x0},
	{NULL, 0x58, DIFMT_OurButton, 0x0}, {NULL, 0x59, DIFMT_OurButton, 0x0}, {NULL, 0x5a, DIFMT_OurButton, 0x0}, {NULL, 0x5b, DIFMT_OurButton, 0x0},
	{NULL, 0x5c, DIFMT_OurButton, 0x0}, {NULL, 0x5d, DIFMT_OurButton, 0x0}, {NULL, 0x5e, DIFMT_OurButton, 0x0}, {NULL, 0x5f, DIFMT_OurButton, 0x0},
	{NULL, 0x60, DIFMT_OurButton, 0x0}, {NULL, 0x61, DIFMT_OurButton, 0x0}, {NULL, 0x62, DIFMT_OurButton, 0x0}, {NULL, 0x63, DIFMT_OurButton, 0x0},
	{NULL, 0x64, DIFMT_OurButton, 0x0}, {NULL, 0x65, DIFMT_OurButton, 0x0}, {NULL, 0x66, DIFMT_OurButton, 0x0}, {NULL, 0x67, DIFMT_OurButton, 0x0},
	{NULL, 0x68, DIFMT_OurButton, 0x0}, {NULL, 0x69, DIFMT_OurButton, 0x0}, {NULL, 0x6a, DIFMT_OurButton, 0x0}, {NULL, 0x6b, DIFMT_OurButton, 0x0},
	{NULL, 0x6c, DIFMT_OurButton, 0x0}, {NULL, 0x6d, DIFMT_OurButton, 0x0}, {NULL, 0x6e, DIFMT_OurButton, 0x0}, {NULL, 0x6f, DIFMT_OurButton, 0x0},
	{NULL, 0x70, DIFMT_OurButton, 0x0}, {NULL, 0x71, DIFMT_OurButton, 0x0}, {NULL, 0x72, DIFMT_OurButton, 0x0}, {NULL, 0x73, DIFMT_OurButton, 0x0},
	{NULL, 0x74, DIFMT_OurButton, 0x0}, {NULL, 0x75, DIFMT_OurButton, 0x0}, {NULL, 0x76, DIFMT_OurButton, 0x0}, {NULL, 0x77, DIFMT_OurButton, 0x0},
	{NULL, 0x78, DIFMT_OurButton, 0x0}, {NULL, 0x79, DIFMT_OurButton, 0x0}, {NULL, 0x7a, DIFMT_OurButton, 0x0}, {NULL, 0x7b, DIFMT_OurButton, 0x0},
	{NULL, 0x7c, DIFMT_OurButton, 0x0}, {NULL, 0x7d, DIFMT_OurButton, 0x0}, {NULL, 0x7e, DIFMT_OurButton, 0x0}, {NULL, 0x7f, DIFMT_OurButton, 0x0},
	{NULL, 0x80, DIFMT_OurButton, 0x0}, {NULL, 0x81, DIFMT_OurButton, 0x0}, {NULL, 0x82, DIFMT_OurButton, 0x0}, {NULL, 0x83, DIFMT_OurButton, 0x0},
	{NULL, 0x84, DIFMT_OurButton, 0x0}, {NULL, 0x85, DIFMT_OurButton, 0x0}, {NULL, 0x86, DIFMT_OurButton, 0x0}, {NULL, 0x87, DIFMT_OurButton, 0x0},
	{NULL, 0x88, DIFMT_OurButton, 0x0}, {NULL, 0x89, DIFMT_OurButton, 0x0}, {NULL, 0x8a, DIFMT_OurButton, 0x0}, {NULL, 0x8b, DIFMT_OurButton, 0x0},
	{NULL, 0x8c, DIFMT_OurButton, 0x0}, {NULL, 0x8d, DIFMT_OurButton, 0x0}, {NULL, 0x8e, DIFMT_OurButton, 0x0}, {NULL, 0x8f, DIFMT_OurButton, 0x0},
	{NULL, 0x90, DIFMT_OurButton, 0x0}, {NULL, 0x91, DIFMT_OurButton, 0x0}, {NULL, 0x92, DIFMT_OurButton, 0x0}, {NULL, 0x93, DIFMT_OurButton, 0x0},
	{NULL, 0x94, DIFMT_OurButton, 0x0}, {NULL, 0x95, DIFMT_OurButton, 0x0}, {NULL, 0x96, DIFMT_OurButton, 0x0}, {NULL, 0x97, DIFMT_OurButton, 0x0},
	{NULL, 0x98, DIFMT_OurButton, 0x0}, {NULL, 0x99, DIFMT_OurButton, 0x0}, {NULL, 0x9a, DIFMT_OurButton, 0x0}, {NULL, 0x9b, DIFMT_OurButton, 0x0},
	{NULL, 0x9c, DIFMT_OurButton, 0x0}, {NULL, 0x9d, DIFMT_OurButton, 0x0}, {NULL, 0x9e, DIFMT_OurButton, 0x0}, {NULL, 0x9f, DIFMT_OurButton, 0x0},
	{NULL, 0xa0, DIFMT_OurButton, 0x0}, {NULL, 0xa1, DIFMT_OurButton, 0x0}, {NULL, 0xa2, DIFMT_OurButton, 0x0}, {NULL, 0xa3, DIFMT_OurButton, 0x0},
	{NULL, 0xa4, DIFMT_OurButton, 0x0}, {NULL, 0xa5, DIFMT_OurButton, 0x0}, {NULL, 0xa6, DIFMT_OurButton, 0x0}, {NULL, 0xa7, DIFMT_OurButton, 0x0},
	{NULL, 0xa8, DIFMT_OurButton, 0x0}, {NULL, 0xa9, DIFMT_OurButton, 0x0}, {NULL, 0xaa, DIFMT_OurButton, 0x0}, {NULL, 0xab, DIFMT_OurButton, 0x0},
	{NULL, 0xac, DIFMT_OurButton, 0x0}, {NULL, 0xad, DIFMT_OurButton, 0x0}, {NULL, 0xae, DIFMT_OurButton, 0x0}, {NULL, 0xaf, DIFMT_OurButton, 0x0},
	{&Local_GUID_XAxis, 0xb0, DIFMT_JoyAxis, DIDOI_ASPECTVELOCITY},
	{&Local_GUID_YAxis, 0xb4, DIFMT_JoyAxis, DIDOI_ASPECTVELOCITY},
	{&Local_GUID_ZAxis, 0xb8, DIFMT_JoyAxis, DIDOI_ASPECTVELOCITY},
	{&Local_GUID_RxAxis, 0xbc, DIFMT_JoyAxis, DIDOI_ASPECTVELOCITY},
	{&Local_GUID_RyAxis, 0xc0, DIFMT_JoyAxis, DIDOI_ASPECTVELOCITY},
	{&Local_GUID_RzAxis, 0xc4, DIFMT_JoyAxis, DIDOI_ASPECTVELOCITY},
	{&Local_GUID_Slider, 0x18, DIFMT_JoyAxis, DIDOI_ASPECTVELOCITY},
	{&Local_GUID_Slider, 0x1c, DIFMT_JoyAxis, DIDOI_ASPECTVELOCITY},
	{&Local_GUID_XAxis, 0xd0, DIFMT_JoyAxis, DIDOI_ASPECTACCEL},
	{&Local_GUID_YAxis, 0xd4, DIFMT_JoyAxis, DIDOI_ASPECTACCEL},
	{&Local_GUID_ZAxis, 0xd8, DIFMT_JoyAxis, DIDOI_ASPECTACCEL},
	{&Local_GUID_RxAxis, 0xdc, DIFMT_JoyAxis, DIDOI_ASPECTACCEL},
	{&Local_GUID_RyAxis, 0xe0, DIFMT_JoyAxis, DIDOI_ASPECTACCEL},
	{&Local_GUID_RzAxis, 0xe4, DIFMT_JoyAxis, DIDOI_ASPECTACCEL},
	{&Local_GUID_Slider, 0x18, DIFMT_JoyAxis, DIDOI_ASPECTACCEL},
	{&Local_GUID_Slider, 0x1c, DIFMT_JoyAxis, DIDOI_ASPECTACCEL},
	{&Local_GUID_XAxis, 0xf0, DIFMT_JoyAxis, DIDOI_ASPECTFORCE},
	{&Local_GUID_YAxis, 0xf4, DIFMT_JoyAxis, DIDOI_ASPECTFORCE},
	{&Local_GUID_ZAxis, 0xf8, DIFMT_JoyAxis, DIDOI_ASPECTFORCE},
	{&Local_GUID_RxAxis, 0xfc, DIFMT_JoyAxis, DIDOI_ASPECTFORCE},
	{&Local_GUID_RyAxis, 0x100, DIFMT_JoyAxis, DIDOI_ASPECTFORCE},
	{&Local_GUID_RzAxis, 0x104, DIFMT_JoyAxis, DIDOI_ASPECTFORCE},
	{&Local_GUID_Slider, 0x18, DIFMT_JoyAxis, DIDOI_ASPECTFORCE},
	{&Local_GUID_Slider, 0x1c, DIFMT_JoyAxis, DIDOI_ASPECTFORCE}
};

/// The replacement for c_dfDIJoystick2 defined in dinput.lib
const DIDATAFORMAT our_c_dfDIJoystick2 = {sizeof( DIDATAFORMAT ), sizeof( DIOBJECTDATAFORMAT ), DIDF_ABSAXIS, 272, sizeof( c_rgodfDIJoy2 ) / sizeof( c_rgodfDIJoy2[0] ), c_rgodfDIJoy2};

/// Gamepad/Joystick device class constant
const DWORD LocalConstant_JoystickDeviceClass =
   DI8DEVCLASS_GAMECTRL
//    DIDEVTYPE_JOYSTICK // for DInput 7 and below
   ;

#endif // L_USE_DIRECTINPUT

class scriptfinal LDirectInputJoystick : public iInputDevice
{
public:
	LDirectInputJoystick();
	virtual ~LDirectInputJoystick();

	/// Run control panel for the devices. E.g., lpdiKeyboard->RunControlPanel(NULL, 0); for DirectInput
	virtual void      RunSystemSetup();

	virtual bool      SystemSetupAvailable() const { return true; }

	virtual LString   GetName() const;

	/// Update the device state.
	virtual bool      Poll();

	virtual bool      Start();
	virtual bool      IsStarted() const;
	virtual bool      Stop();

	virtual int       GetNumAxes() const;
	virtual bool      SetAxisRange( int AxisIdx, float MinVal, float MaxVal );
	virtual bool      SetAxisDeadZone( int AxisIdx, float DeadZone );
	virtual bool      SetAxisSaturation( int AxisIdx, float Saturation );
	virtual float     GetAxisValue( int AxisIdx ) const;
	virtual LString   GetAxisName( int AxisIdx ) const;
	virtual void      SetAxisValue( int AxisIdx, float Val ) {}

	virtual LString   GetKeyName( int KeyIdx ) const;
	virtual bool      IsPressed( int KeyIdx ) const;
	virtual int       GetNumKeys() const;
	virtual void      SetKeyState( int KeyIdx, bool Pressed ) {}

private:
	/// Initialise the Device Pointer
	bool InitDevice( void );

	/// Initialise the Joystick
	bool InitJoystick( void );

	/// Find out how many buttons the attached device has
	int HowManyButtons( void );

	/// Acquire/UnAcquire the Joystick
	bool Acquire( bool state );

	/// list of Button Names for selected Joystick
	std::vector<LString> m_DIButtonNames;

	// generic flags filled in PollDevice()
	bool m_JoyFire[MAXJOYBUTTONS];
	bool m_JoyFire1;

#ifdef L_USE_DIRECTINPUT
public:
	// internal setter
	void SetDInput( IDirectInput8A* TheDI )
	{
		m_lpDI = TheDI;
		m_lpDIDevice = NULL;
	}

	/// Pass the HWND to which the joystick will be attached.
	inline void SetWindowHandle( LWindowHandle hwnd ) { m_hwnd = hwnd; }

	/// Set the current GUID for joystick device.
	void SetPreferredDevice( const GUID* pguid );

	void SetDeviceInstance( LPDIDEVICEINSTANCE di ) { m_DeviceInstance = const_cast<LPDIDEVICEINSTANCE>( di ); }

private:

	/// Pointer to Direct Input
	IDirectInput8A* m_lpDI;

	/// Holds Joystick State Information
	DIJOYSTATE2   m_dijs;

	/// Current Joystick GUID
	GUID m_JoystickGUID;

	/// Pointer to Direct Input Device
	IDirectInputDevice8A* m_lpDIDevice;

	/// Create A Device Pointer for a GUID
	bool CreateDevice( GUID* guid );
	/*
	   // Used for Force Feed Back, Not Yet Implemented
	   LPDIRECTINPUTEFFECT  m_lpTriggerEffect;
	   LPDIRECTINPUTEFFECT  m_lpStickyEffect;
	   LPDIRECTINPUTEFFECT  m_lpResistEffect;
	   bool m_TriggerOK;
	   /// GUID For Force Feed Back Device?
	   GUID m_TriggerGuid;
	*/

	/// Is device Force Feedback Compatible?
	BOOL m_FFAvailable;

	/// HWND to which Direct Input will be attached to.
	HWND m_hwnd;

	LPCDIDEVICEINSTANCE m_DeviceInstance;
#endif

	void MyDebugString( const LString& Str );
};

void LDirectInputJoystick::MyDebugString( const LString& Str )
{
	FLastErrorMsg = Str;

	cout << Str;
	cout.flush();
}

void LDirectInputJoystick::RunSystemSetup()
{
#ifdef L_USE_DIRECTINPUT
	IDirectInputDevice8_RunControlPanel( m_lpDIDevice, NULL, 0 );
#endif
}

LString LDirectInputJoystick::GetName() const
{
#ifdef L_USE_DIRECTINPUT

	if ( m_DeviceInstance != NULL )
	{
		return LString( m_DeviceInstance->tszInstanceName );
	}

#endif
	return LString( "" );
}

bool LDirectInputJoystick::Start()
{
	// button enumeration automatically takes care of init
	/*int tmp = */HowManyButtons();

	return true;
}

bool LDirectInputJoystick::IsStarted() const
{
#ifdef L_USE_DIRECTINPUT
	return ( m_lpDIDevice != NULL );
#else
	return false;
#endif
}

bool LDirectInputJoystick::Stop()
{
#ifdef L_USE_DIRECTINPUT

	if ( m_lpDIDevice )
	{
		// Always unacquire device before calling Release().
		try
		{
			Acquire( false );

			HRESULT hr = IDirectInputDevice8_Release( m_lpDIDevice );

			if ( FAILED( hr ) )
			{
				MyDebugString( GetDIError( hr ) );
				return false;
			}
			else
			{
				m_lpDIDevice = NULL;
			}

			return true;
		}
		catch ( ... )
		{
			MyDebugString( "Failed to Release Pointer in CDIJoystick::Shutdown\n" );
		}
	}

#endif

	return false;
}

float LDirectInputJoystick::GetAxisValue( int AxisIdx ) const
{
#ifdef L_USE_DIRECTINPUT

	if ( AxisIdx == 0 ) { return static_cast<float>( m_dijs.lX ); }

	if ( AxisIdx == 1 ) { return static_cast<float>( m_dijs.lY ); }

	if ( AxisIdx == 2 ) { return static_cast<float>( m_dijs.lZ ); }

	if ( AxisIdx == 3 ) { return static_cast<float>( m_dijs.lRz ); }

#endif
	return 0.0f;
}

LString LDirectInputJoystick::GetAxisName( int AxisIdx ) const
{
	if ( AxisIdx == 0 ) { return LString( "X Axis" ); }

	if ( AxisIdx == 1 ) { return LString( "Y Axis" ); }

	if ( AxisIdx == 2 ) { return LString( "Z Axis" ); }

	if ( AxisIdx == 3 ) { return LString( "Z Roll Axis" ); }

	return LString( "" );
}

LString LDirectInputJoystick::GetKeyName( int KeyIdx ) const
{
	return m_DIButtonNames[KeyIdx];
}

bool LDirectInputJoystick::IsPressed( int KeyIdx ) const
{
	return ( KeyIdx >= 0 && KeyIdx < GetNumKeys() ) ? m_JoyFire[KeyIdx] : false;
}

int LDirectInputJoystick::GetNumKeys() const
{
	return static_cast<int>( m_DIButtonNames.size() );
}

//////// DirectInput specific code

#define BUFFERSIZE 16

// Set the maxmimum range to which we'll gauge the swing
#define JOYMAX 10000
#define JOYMIN -10000

/*             Y
               ^
               |
X   -10,000 <--*--> +10,000
               |
               v
*/

// Dead zone is the amount of sway the joystick can have before we start registering movement
// In this case 20%

#define JOYDEAD 2000

// The Saturation Point Is Where the Joystick is deemed to be at Full Swing, in this case 95%
#define JOYSAT  9500

int LDirectInputJoystick::GetNumAxes() const
{
	// x,y
	// z + z-roll
	return 4;
	TODO( "use DInput to determine this" )
}

bool LDirectInputJoystick::SetAxisRange( int AxisIdx, float MinVal, float MaxVal )
{
#ifdef L_USE_DIRECTINPUT

	if ( m_lpDIDevice == NULL )
	{
		return false;
	}

	// Set range.
	// Note: range, deadzone, and saturation are being set for the
	// entire device. This could have unwanted effects on
	// sliders, dials, etc.

	DIPROPRANGE diprg;

	diprg.diph.dwSize       = sizeof( diprg );
	diprg.diph.dwHeaderSize = sizeof( diprg.diph );
	diprg.diph.dwObj        = 0;
	diprg.diph.dwHow        = DIPH_DEVICE;
	diprg.lMin              = static_cast<int>( MinVal ); /*JOYMIN; */
	diprg.lMax              = static_cast<int>( MaxVal ); /*JOYMAX; */

	return !FAILED( IDirectInputDevice8_SetProperty( m_lpDIDevice, DIPROP_RANGE, &diprg.diph ) );

#else
	return false;
#endif
}

bool LDirectInputJoystick::SetAxisDeadZone( int AxisIdx, float DeadZone )
{
#ifdef L_USE_DIRECTINPUT

	if ( m_lpDIDevice == NULL )
	{
		return false;
	}

	// Set deadzone.
	DIPROPDWORD dipdw;

	dipdw.diph.dwSize       = sizeof( dipdw );
	dipdw.diph.dwHeaderSize = sizeof( dipdw.diph );
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = static_cast<int>( DeadZone ); /*JOYDEAD*/;

	return !FAILED( IDirectInputDevice8_SetProperty( m_lpDIDevice, DIPROP_DEADZONE, &dipdw.diph ) );

#else
	return false;
#endif
}

bool LDirectInputJoystick::SetAxisSaturation( int AxisIdx, float Saturation )
{
#ifdef L_USE_DIRECTINPUT

	if ( m_lpDIDevice == NULL )
	{
		return false;
	}

	// Set saturation.
	DIPROPDWORD dipdw;

	dipdw.diph.dwSize       = sizeof( dipdw );
	dipdw.diph.dwHeaderSize = sizeof( dipdw.diph );
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = JOYSAT;

	return !FAILED( IDirectInputDevice8_SetProperty( m_lpDIDevice, DIPROP_SATURATION, &dipdw.diph ) );

#else
	return false;
#endif
}

LDirectInputJoystick::LDirectInputJoystick()
{
#ifdef L_USE_DIRECTINPUT
	m_DeviceInstance = NULL;
	m_lpDI           = NULL;
	m_lpDIDevice     = NULL;
#endif
}

#ifdef L_USE_DIRECTINPUT
///  Create the Joystick Device Using GUID Passed
bool LDirectInputJoystick::CreateDevice( GUID* guid )
{
	HRESULT hr = 0; // NULL;

	Stop();

	// Check to see that Direct Input has been created and initialised.
	if ( !m_lpDI )
	{
		MyDebugString( "Failed to Create DI interface to device in LDIJoystick::CreateDevice(GUID *guid) m_lpDI not initialised.\n" );
		return false;
	}

	// Attempt to create the device based on the GUID passed to this routine
	hr = IDirectInput8_CreateDevice( m_lpDI, *guid, &m_lpDIDevice, NULL );

	if ( FAILED( hr ) )
	{
		MyDebugString( "Failed to Create DI 8 interface to device in LDIJoystick::CreateDevice(GUID *guid)\n" );
		return false;
	}

	// We must have been successful at this point.
	// Therefore copy the GUID to this members instance for future reference.
	memcpy( &m_JoystickGUID, guid, sizeof( GUID ) );

	return true;
}
#endif

// Return How Many Buttons The Attached Device Has Installed
// When giving the player an option of which joystick to use
// You may wish to evaluate the buttons available per attached device.
// Returns the number of buttons for the currently selected device.
int LDirectInputJoystick::HowManyButtons()
{
#ifdef L_USE_DIRECTINPUT
	DIDEVICEOBJECTINSTANCE didoi;
	DWORD x;
	DWORD dwOfs;
	int count = 0;
	HRESULT hr = 0;

	m_DIButtonNames.clear();

	//if (m_lpDIDevice)
	if ( InitDevice() )
	{
		ZeroMemory( &didoi, sizeof( DIDEVICEOBJECTINSTANCE ) );
		didoi.dwSize = sizeof( didoi );

		for ( x = 0; x < 32; x++ )
		{
			dwOfs = DIJOFS_BUTTON( x );

			hr = IDirectInputDevice8_GetObjectInfo( m_lpDIDevice, &didoi, dwOfs, DIPH_BYOFFSET );

			if ( SUCCEEDED( hr ) )
			{
				count++;

				// Add the button name to the Pointer List for future reference.
				m_DIButtonNames.push_back( LString( didoi.tszName ) );
			}
			else
			{
				MyDebugString( GetDIError( hr ) );
			}
		}
	}

	return count;  // How many buttons did we find?
#else
	return 0;
#endif
}

bool LDirectInputJoystick::InitJoystick()
{
	for ( int i = 0 ; i < GetNumAxes() ; i++ )
	{
		if ( !SetAxisRange( i, JOYMIN, JOYMAX ) ) { return false; }

		if ( !SetAxisDeadZone( i, JOYDEAD ) ) { return false; }

		if ( !SetAxisSaturation( i, JOYSAT ) ) { return false; }
	}

#ifdef L_USE_DIRECTINPUT
	// Find out if force feedback available.
	DIDEVCAPS didc;
	didc.dwSize = sizeof( DIDEVCAPS );

	if ( FAILED( IDirectInputDevice8_GetCapabilities( m_lpDIDevice, &didc ) ) )
	{
		return false;
	}

	m_FFAvailable = ( didc.dwFlags & DIDC_FORCEFEEDBACK );

	// If it's a force feedback stick, turn off autocenter so it doesn't
	// get in the way of our effects.
	if ( m_FFAvailable )
	{
		DIPROPDWORD DIPropAutoCenter;

		DIPropAutoCenter.diph.dwSize = sizeof( DIPropAutoCenter );
		DIPropAutoCenter.diph.dwHeaderSize = sizeof( DIPROPHEADER );
		DIPropAutoCenter.diph.dwObj = 0;
		DIPropAutoCenter.diph.dwHow = DIPH_DEVICE;
		DIPropAutoCenter.dwData = 0;

		IDirectInputDevice8_SetProperty( m_lpDIDevice, DIPROP_AUTOCENTER, &DIPropAutoCenter.diph );
	}

#endif
	return true;
}

// Initialise The Joystick Device
bool LDirectInputJoystick::InitDevice()
{
#ifdef L_USE_DIRECTINPUT
	HRESULT hr;
	DIDEVICEINSTANCE    diDeviceInstance;

	// Just a precaution when Enumerating Devices and button Names if you create
	// An options Dialog before creating your main gaming window.
	// Then simply use the desktop window, temporarily!
	if ( !m_hwnd )
	{
		m_hwnd =::GetDesktopWindow();
	}

	Stop();

	// Create game device and set IDirectInputDevice7 interface in m_lpDIDevice.
	if ( !CreateDevice( &m_JoystickGUID ) )
	{
		MyDebugString( "Failed to create device in CDIJoystick::InitDevice()\n" );
		return false;
	}

	// Find out what type it is and set data format accordingly.

	diDeviceInstance.dwSize = sizeof( DIDEVICEINSTANCE );

	hr = IDirectInputDevice8_GetDeviceInfo( m_lpDIDevice, &diDeviceInstance );

	if ( FAILED( hr ) )
	{
		MyDebugString( "Failed to obtain device info in CDIJoystick::InitDevice()\n" );
		MyDebugString( GetDIError( hr ) );
		return false;
	}

	// Set the data format to be a Joystick
	hr = IDirectInputDevice8_SetDataFormat( m_lpDIDevice, &our_c_dfDIJoystick2 );

	if ( FAILED( hr ) )
	{
		MyDebugString( "Failed to create device in CDIJoystick::InitDevice()\n" );
		MyDebugString( GetDIError( hr ) );
		return false;
	}

	// Set cooperative level.
	DWORD cl, cl1;
	cl = DISCL_EXCLUSIVE;

	// Set foreground level for release version, but use background level
	// for debugging so we don't keep losing the device when switching to
	// a debug window.

	//cl1 = DISCL_FOREGROUND;
	cl1 = DISCL_BACKGROUND;

	// now set the co-operation level.
	hr = IDirectInputDevice8_SetCooperativeLevel( m_lpDIDevice, m_hwnd, cl | cl1 );

	if ( FAILED( hr ) )
	{
		MyDebugString( "Failed to set game device cooperative level.\n" );
		MyDebugString( GetDIError( hr ) );
		return false;
	}

	// Set up the data buffer.
	DIPROPDWORD dipdw =
	{
		// The header.
		{
			sizeof( DIPROPDWORD ),      // diph.dwSize
			sizeof( DIPROPHEADER ),     // diph.dwHeaderSize
			0,                          // diph.dwObj
			DIPH_DEVICE,                // diph.dwHow
		},
		// Number of elements in data buffer.
		BUFFERSIZE              // dwData
	};

	hr = IDirectInputDevice8_SetProperty( m_lpDIDevice, DIPROP_BUFFERSIZE, &dipdw.diph );

	if ( FAILED( hr ) )
	{
		MyDebugString( "Failed to set up Data Buffer property.\n" );
		MyDebugString( GetDIError( hr ) );
		return false;
	}

	// Resest the Force Feedback Flag
	m_FFAvailable = false;

	// Try and initialise the Joystick!
	if ( !InitJoystick() )
	{
		return false;
	}

	// In case you were wondering, this is not a good time to acquire the
	// device. For one thing, we can't acquire in foreground mode here
	// because the options dialog may be open. We'll acquire it when the
	// main window is activated.
#endif

	return true;
}

/// Either Acquire or Unacquire the Device
bool LDirectInputJoystick::Acquire( bool state )
{
#ifdef L_USE_DIRECTINPUT
	HRESULT hr;

	if ( !m_lpDIDevice )
	{
		MyDebugString( "Error in CDIJoystick::Acquire(bool state)\nDevice Has Not Been Created.\n" );
		return false;
	}

	if ( !state )  // Unacquire.
	{
		hr = IDirectInputDevice8_Unacquire( m_lpDIDevice );
	}
	else       // Acquire.
	{
		// This could take a while with FF.
		// ::SetCursor( m_hCursorWait );

		hr = IDirectInputDevice8_Acquire( m_lpDIDevice );

		if ( SUCCEEDED( hr ) )
		{
			// Initialize effects; ignore failure and just
			// pretend FF not there.
			if ( m_FFAvailable )      // First set when device initialized.
			{
				//m_FFAvailable = InitFFEffects();
			}
		}
	}

	if ( FAILED( hr ) )
	{
		MyDebugString( "Failed in CDIJoystick::Acquire(bool state)\n" );
		printf( "%x\n", hr );

		MyDebugString( GetDIError( hr ) );
		return false;
	}

#endif

	return true;
}

/// Update member variables to reflect the state of the device
bool LDirectInputJoystick::Poll()
{
#ifdef L_USE_DIRECTINPUT
	static int loopcount = 0;

	HRESULT            hr;
	//DIDEVICEOBJECTDATA rgdod[BUFFERSIZE];
	//DWORD              dwItems;

	ZeroMemory( &m_dijs, sizeof( m_dijs ) );

	// Has device been initialised ?
	if ( !m_lpDIDevice )
	{
		// Try and initialise device
		if ( !InitDevice() )
		{
			MyDebugString( "Failed To Initialise and Poll Joystick in CDIJoystick::PollDevice()\n" );
		}

		return false;
	}

	hr = IDirectInputDevice8_Poll( m_lpDIDevice );  // May be unnecessary but never hurts. // unsupported in MinGW ?

	if ( FAILED( hr ) )
	{
		MyDebugString( "Failed To Poll Joystick in CDIJoystick::PollDevice()\n" );
		MyDebugString( GetDIError( hr ) );
	}

	while ( 1 )
	{
		if ( loopcount > 20 )
		{
			return false;   // Infinite Loop Protection
		}

		hr = IDirectInputDevice8_GetDeviceState( m_lpDIDevice, sizeof( DIJOYSTATE2 ), &m_dijs );

		// The data stream was interrupted. Reacquire the device and try again.
		if ( hr == DIERR_INPUTLOST )
		{
			MyDebugString( "Failed To Obtain Immediate Device State in CDIJoystick::PollDevice()\n" );
			MyDebugString( GetDIError( hr ) );

			// Increment Infinite Loop Protection Counter and try again.
			loopcount++;

			// Try and acquire device and start again.
			if ( Acquire( true ) )
			{
				continue;
			}
		}

		// We can't get the device because it has not been acquired so try and acquire it.
		if ( hr == DIERR_NOTACQUIRED )
		{
			// Increment Infinite Loop Protection Counter.
			loopcount++;

			MyDebugString( "Device Not Acquired Trying Again immediate CDIJoystick::PollDevice()\n" );

			if ( !Acquire( true ) )
			{
				MyDebugString( "Unable to acquire Immediate Device in CDIJoystick::PollDevice() Quitting\n" );
				return false;
			}

			// Try and get buffered data if device is buffered!
			continue;
		}

		break;
	}

	if ( FAILED( hr ) )
	{
		MyDebugString( "Unable to obtain Immediate Data from Device in CDIJoystick::PollDevice() Quitting\n" );
		return false;
	}

	m_JoyFire1 = false;

#ifdef _DEBUG
	int firecount = 0;
#endif

	for ( int i = 0; i < sizeof( m_dijs.rgbButtons ); i++ )
	{
		if ( m_dijs.rgbButtons[i] & 0x80 )
		{
#ifdef _DEBUG
			firecount++;
#endif
			m_JoyFire[i] = true;
			m_JoyFire1 = true;

		}
		else
		{
			m_JoyFire[i] = false;
		}
	}

#ifdef _DEBUG

	if ( firecount > 0 )
	{
//    MyDebugString("Many Buttons Pressed\n");
	}

#endif
// MyDebugString("Ok\n");

#endif  // L_USE_DIRECTINPUT

	return true;
}

LDirectInputJoystick::~LDirectInputJoystick()
{
	if ( IsStarted() ) { Stop(); }
}

LWinDeviceList::LWinDeviceList()
{
#ifdef L_USE_DIRECTINPUT
	// Initialise Member Variables
// m_EnumerationStarted=false;
	m_Initialised = false;
	m_hInstance = GetModuleHandle( NULL );

	m_lpDI = NULL;

	hDInputDll = NULL;
	DirectInput8Create_Function = NULL;
#endif
}

void LWinDeviceList::AfterConstruction()
{
	iInputDeviceList::AfterConstruction();

	// Initialise Direct Input
	Initialise();

	// Start Enumeration of Attached Joysticks.
	Enumerate_Joysticks();
}

#ifdef L_USE_DIRECTINPUT

/// Add Enumerated Devices To A Link List Object. Continue Enumeration Of The Device
BOOL LWinDeviceList::EnumDevicesProc( LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef )
{
	LWinDeviceList* obj = ( LWinDeviceList* )( pvRef );
	obj->AddDeviceInfo( lpddi );

	return DIENUM_CONTINUE;
}

/**
// Add Device Info To The List Of Pointers Held in m_DIJoystickList
//
// return  true = Successfully Added
//        false = Failed To Add
*/
bool LWinDeviceList::AddDeviceInfo( LPCDIDEVICEINSTANCE lpddi )
{
	LPCDIDEVICEINSTANCE lpdi2 = new DIDEVICEINSTANCE;

	if ( lpdi2 )
	{
		memcpy( ( void* )lpdi2, lpddi, sizeof( DIDEVICEINSTANCE ) );

		m_DIJoystickList.push_back( lpdi2 );

		return true;
	}

	return false;
}

void LDirectInputJoystick::SetPreferredDevice( const GUID* pguid )
{
	memcpy( &m_JoystickGUID, pguid, sizeof( GUID ) );
}

LDirectInputJoystick* LWinDeviceList::CreateDeviceFor( const GUID* pguid )
{
	LDirectInputJoystick* J = new LDirectInputJoystick();

	J->SetDInput( this->m_lpDI );

	J->SetWindowHandle( FHandle );

	J->SetPreferredDevice( pguid );

	return J;
}
#endif

iInputDevice* LWinDeviceList::CreateDeviceForIdx( int Idx )
{
#ifdef L_USE_DIRECTINPUT
	LDirectInputJoystick* J = CreateDeviceFor( &m_DIJoystickList[Idx]->guidInstance );

	J->SetDeviceInstance( const_cast<LPDIDEVICEINSTANCE>( m_DIJoystickList[Idx] ) );

	return J;
#else
	return NULL;
#endif
}

/// Destroy The Direct Input Joystick Control and tidy up.
LWinDeviceList::~LWinDeviceList()
{
	Shutdown();
}

/// Initialise Direct Input
bool LWinDeviceList::Initialise()
{
#ifdef L_USE_DIRECTINPUT
	hDInputDll = Env->LoadDynamicLibrary( "dinput8.dll" );

	if ( !hDInputDll )
	{
		FLastErrorMsg = "Unable to load dinput8.dll";
		return false;
	}

	DirectInput8Create_Function = ( DirectInput8Create_func )( hDInputDll->GetProcAddress( "DirectInput8Create" ) );

	if ( !DirectInput8Create_Function )
	{
		FLastErrorMsg = "Unable to get DirectInput8Create() function pointer";
		return false;
	}

	HRESULT hr = DirectInput8Create_Function( m_hInstance, DIRECTINPUT_VERSION, LocalConstant_IID_IDirectInput8A, ( void** )&m_lpDI, NULL );

	if FAILED( hr )
	{
		// DirectInput not available; take appropriate action
		MyDebugString( "Failed To Initialise Direct Input 8 in CDIJoystick::Initialise\n" );
		MyDebugString( GetDIError( hr ) );

		Shutdown();
		return false;
	}

	// Successfully Created Direct Input 8 Object
	m_Initialised = true;
	return true;
#else
	return false;
#endif
}

void LWinDeviceList::Shutdown()
{
#ifdef L_USE_DIRECTINPUT

	// Remove Joystick Information
	if ( !m_DIJoystickList.empty() )
	{
		for ( size_t d = 0 ; d < m_DIJoystickList.size() ; d++ )
		{
			delete m_DIJoystickList[d];
		}

		m_DIJoystickList.clear();
	}

	// Shutdown Direct Input!
	if ( m_lpDI )
	{
		try
		{
			IDirectInput8_Release( m_lpDI );
			m_lpDI = NULL;
		}
		catch ( ... )
		{
			MyDebugString( "Failed to Release DI8 Pointer in CDIJoystick::Shutdown\n" );
		}
	}

	m_Initialised = false;

	// unload the dinput.dll
	if ( hDInputDll ) { delete hDInputDll; }

	hDInputDll = NULL;
#endif
}

/// Start the Enumeration Of Attached Joystick Devices.
bool LWinDeviceList::Enumerate_Joysticks()
{
#ifdef L_USE_DIRECTINPUT
	HRESULT hr = 0;

	if ( !m_Initialised )
	{
		Initialise();
	}

	if ( !m_lpDI ) // Has a Direct Input Interface Already Been Initialised?
	{
		hr = DirectInput8Create_Function( m_hInstance, DIRECTINPUT_VERSION, LocalConstant_IID_IDirectInput8A, ( void** )&m_lpDI, NULL );

		if FAILED( hr )
		{
			MyDebugString( "Error in LWinDeviceList::Enumerate_Joysticks()\n" );
			MyDebugString( GetDIError( hr ) );
			return false;
		}
	}

	DWORD DeviceClass = LocalConstant_JoystickDeviceClass;

	if ( m_lpDI )
	{
		hr = IDirectInput8_EnumDevices( m_lpDI, DeviceClass, EnumDevicesProc, this, DIEDFL_ATTACHEDONLY );
	}

	if FAILED( hr )
	{
		MyDebugString( "Error in LWinDeviceList::Enumerate_Joysticks()\n" );
		MyDebugString( GetDIError( hr ) );
		return false;
	}

// MyDebugString("Enumerating Joystick Devices\n");
	return true;
#else
	return false;
#endif
}

/// Set the HWND that will be used when Acquiring Devices!
void LWinDeviceList::SetWindowHandle( LWindowHandle Wnd )
{
	//Shutdown();

	iInputDeviceList::SetWindowHandle( Wnd );

#ifdef L_USE_DIRECTINPUT
	m_hInstance = GetModuleHandle( NULL );
#endif

	// Initialise Direct Input
	//Initialise();

	// Start Enumeration of Attached Joysticks.
	//Enumerate_Joysticks();
}

// Re-Initialise this object, used when changing HWND or Device
// Not yet implemented.
bool LWinDeviceList::ReInitialise()
{
// m_hInstance=GetModuleHandle(NULL);
// Enumerate_Joysticks();
	return true;
}

void LWinDeviceList::RunControlPanel()
{
#ifdef L_USE_DIRECTINPUT

	if ( !m_lpDI ) { return; }

	IDirectInput8_RunControlPanel( m_lpDI, FHandle, 0/*NULL*/ );
#endif
}

bool LWinDeviceList::ReenumerateDevices()
{
	if ( !EnumerateSystemDevices() ) { return false; }

	return EnumerateDirectInputDevices();
}

bool LWinDeviceList::EnumerateSystemDevices()
{
	// add standart keyboard and mouse
	return true;
}

bool LWinDeviceList::EnumerateDirectInputDevices()
{
#ifdef L_USE_DIRECTINPUT

	if ( !Enumerate_Joysticks() ) { return false; }

	// create device for each joystick available
	for ( size_t i = 0 ; i < m_DIJoystickList.size() ; i++ )
	{
		iInputDevice* Joy = CreateDeviceForIdx( static_cast<int>( i ) );

		if ( Joy == NULL ) { return false; }

		FDevices.push_back( Joy );
	}

#endif

	return true;
}

//#endif // _WIN32

/*
 * 11/06/2010
     Log section added
*/
