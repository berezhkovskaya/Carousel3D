/**
 * \file Input.h
 * \brief Input device and devices list
 * \version 0.6.06
 * \date 04/01/2012
 * \author Viktor Latypov, 2010-2012
 * \author Sergey Kosarevsky, 2010-2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __iInputDevice__h__included__
#define __iInputDevice__h__included__

#include "Core/iObject.h"

#include "Utils/Viewport.h"

#ifdef OS_WINDOWS
#include <windows.h>
#undef min
#undef max
#endif

#include <map>

class clCVar;

#if defined(OS_WINDOWS)
typedef HWND LWindowHandle;
#elif defined(OS_ANDROID)
typedef void* LWindowHandle;
#elif defined(OS_LINUX)
//#include <X11/X.h>
//typedef Window LWindowHandle;
#endif


//  ( 16 bytes )
#pragma pack(push, 1)
struct GCC_PACK( 1 ) sKeyInfo
{
   char Name[16];
};
#pragma pack(pop)

//  ( 28 bytes )
#pragma pack(push, 1)
struct GCC_PACK( 1 ) sAxisInfo
{
   char Name[16];

   float MinValue;
   float MaxValue;

   float DeadZone;
   float Saturation;
};
#pragma pack(pop)

#define MAX_INFO_PACKET_SIZE 4096

#define MAX_AXES_IN_PACKET (16)

#define MAX_KEYS_IN_PACKET (128)

#define MAX_KEYS_IN_DEVICE (1024)

/// sizeof(InfoPacket) = 128 * 16 + 32 * 16 = 2048 + 512 + 64 + 4 * 8 = 320 + 2048 = 2368

/// Partial information about the input device (4Kb packet)
#pragma pack(push, 1)
struct GCC_PACK( 1 ) sInputDeviceInfo
{
   char ShortDeviceName[64];

   Lint NumPackets;
   Lint PacketIndex;

   /// Number of keys in this device
   Lint NumKeys;
   Lint NumAxes;

   /// starting key in this packet
   Lint StartKey;
   /// last key in this packet
   Lint EndKey;

   /// starting axis in this packet
   Lint StartAxis;
   /// last axis in the packet
   Lint EndAxis;

   sKeyInfo    Keys[MAX_KEYS_IN_PACKET];
   sAxisInfo   Axes[MAX_AXES_IN_PACKET];
};
#pragma pack(pop)

#define MAX_PRESSED_KEYS (16)
#define MAX_AXES_IN_DEVICE (16)

/// Packed internal device state
#pragma pack(push, 1)
struct GCC_PACK( 1 ) sInputDeviceState
{
   /// number of transfered key states
   int   NumPressedKeys;

   /// indices of pressed keys
   int   PressedKeys[MAX_PRESSED_KEYS];

   /// value for each axis
   float AxisValue[MAX_AXES_IN_DEVICE];
};
#pragma pack(pop)

/**
   Basic class for Keyboard, mouse, joystick/gamepad representation
   Has axes and keys.
   Supports polling and asynchronous update. Provides API for setting the key/axis values
**/
class scriptfinal netexportable iInputDevice : public iObject
{
public:
	iInputDevice() : FLastError( 0 ),
		FLastErrorMsg( "" ) {}

	virtual ~iInputDevice() {}

	NET_EXPORTABLE()

	/// Run specific system setup for this device (available in DInput and winapi)
	virtual void      RunSystemSetup() {}; // e.g., lpdiKeyboard->RunControlPanel(NULL, 0); for DirectInput

	/// Can we set up this device using system tools
	virtual bool      SystemSetupAvailable() const { return false; };

	/// Human-readable device name  [maybe it is a GetObjectID ?]
	virtual LString   GetName() const = 0;

	/// Explicit device polling
	virtual bool      Poll() = 0;

	/// Last error code access
	virtual int       GetLastError() const { return FLastError; }

	/// Human-readable error message
	virtual LString   GetLastErrorMsg() const { return FLastErrorMsg; }

	#pragma region Device initialization
	/// Initialize device
	virtual bool      Start() = 0;

	/// Is the device started and active
	virtual bool      IsStarted() const = 0;

	/// Deinitialize device
	virtual bool      Stop()  = 0;
	#pragma endregion

	#pragma region Axis handling
	/// number of axes for mouses/gamepads/joysticks
	virtual int       GetNumAxes() const = 0;

	/// Internal conversion of the axis value to [Min,Max] range
	virtual bool      SetAxisRange( int AxisIdx, float MinVal, float MaxVal ) = 0;

	/// Set dead zone (minimum acceptable value) for the axis
	virtual bool      SetAxisDeadZone( int AxisIdx, float DeadZone ) = 0;

	/// Set saturation value (high-cutoff, after which the axis is considered being at full swing)
	virtual bool      SetAxisSaturation( int AxisIdx, float Saturation ) = 0;

	/// get current axis value
	virtual float     GetAxisValue( int AxisIdx ) const = 0;

	/// shortcut to GetAxisValue() with 3-axis devices
	virtual LVector3  GetValues() const { return LVector3( 0 ); };

	/// Human-readable axis name
	virtual LString   GetAxisName( int AxisIdx ) const = 0;

	/// Index for the axis name
	virtual int       GetAxisIndex( const LString& AxisName ) const
	{
		int num = GetNumAxes();

		for ( int i = 0 ; i < num ; i++ )
			if ( AxisName == GetAxisName( i ) ) { return i; }

		return -1;
	}

	/// External override for axis values
	virtual void      SetAxisValue( int AxisIdx, float Val ) = 0;
	#pragma endregion

	#pragma region Key/button handling
	/// Human-readable key/button name
	virtual LString   GetKeyName( int KeyIdx ) const = 0;

	/// Index for this key
	virtual int       GetKeyIndex( const LString& KeyName ) const
	{
		int num = GetNumKeys();

		for ( int i = 0 ; i < num ; i++ )
			if ( KeyName == GetKeyName( i ) ) { return i; }

		return -1;
	}

	/// Check if the key/button is pressed
	virtual bool      IsPressed( int KeyIdx ) const = 0;

	/// Number of buttons for gamepads/mouses/keyboards
	virtual int       GetNumKeys() const = 0;

	/// External override for key states
	virtual void      SetKeyState( int KeyIdx, bool Pressed ) = 0;
	#pragma endregion

	virtual bool      IsAxisMoved( int Idx ) const
	{
		if ( Idx < 0 || Idx >= GetNumAxes() ) { return false; }

		return ( fabs( GetAxisValue( Idx ) ) > 0.01f );
	}

	virtual bool      IsMovedAny() const
	{
		for ( int j = 0 ; j < GetNumAxes() ; j++ )
		{
			if ( IsAxisMoved( j ) ) { return true; }
		}

		return false;
	}

	virtual bool      IsPressedAny() const
	{
		for ( int j = 0 ; j < GetNumKeys() ; j++ )
		{
			if ( IsPressed( j ) ) { return true; }
		}

		return false;
	}

	#pragma region Force feedback
	/// Force feedback effects for FF-enabled devices
	#pragma endregion

	#pragma region Network data transfer

	/// Pack internal device state to the network packet
	virtual void PackDeviceState( sInputDeviceState* State ) const
	{
		State->NumPressedKeys = 0;

		// pack pressed keys
		int nk = GetNumKeys();

		for ( int i = 0 ; i < nk ; i++ )
		{
			if ( this->IsPressed( i ) )
			{
				if ( State->NumPressedKeys >= MAX_PRESSED_KEYS ) { break; }

				State->PressedKeys[State->NumPressedKeys] = i;

				State->NumPressedKeys++;
			}
		}

		// pack all axes
		int na = GetNumAxes();

		for ( int i = 0 ; i < na ; i++ ) { State->AxisValue[i] = GetAxisValue( i ); }
	}

	/// Form device information packet
	virtual void PackDeviceInfo( sInputDeviceInfo* Info, int PacketIndex ) const
	{
		int NumPackets = GetNumKeys() / MAX_KEYS_IN_PACKET;

		if ( NumPackets < 1 ) { NumPackets = 1; }

		// 1. pack common info

		LString AName = GetName();

		if ( AName.length() > 63 ) { AName = AName.substr( 0, 63 ); }

		strcpy( reinterpret_cast<char*>( &Info->ShortDeviceName[0] ), AName.c_str() );

		Info->NumPackets = NumPackets;
		Info->PacketIndex = PacketIndex;

		Info->NumKeys = GetNumKeys();
		Info->NumAxes = GetNumAxes();

		Info->StartKey = PacketIndex * MAX_KEYS_IN_PACKET;
		Info->EndKey = std::min( Info->StartKey + MAX_KEYS_IN_PACKET - 1, GetNumKeys() - 1 );

		Info->StartAxis = 0;
		Info->EndAxis = GetNumAxes() - 1;

		// 2. pack all axes
		for ( int i = 0 ; i < GetNumAxes(); i++ )
		{
			LString AxisName = GetAxisName( i );

			if ( AxisName.length() > 15 ) { AxisName = AxisName.substr( 0, 15 ); }

			strcpy( reinterpret_cast<char*>( &Info->Axes[i].Name[0] ), AxisName.c_str() );

			TODO( "pack other values" )
		}

		// 3. pack keys
		for ( int i = Info->StartKey ; i <= Info->EndKey ; i++ )
		{
			LString KeyName = GetKeyName( i );

			if ( KeyName.length() > 15 ) { KeyName = KeyName.substr( 0, 15 ); }

			strcpy( reinterpret_cast<char*>( &Info->Keys[i].Name[0] ), KeyName.c_str() );
		}
	}

	/// Read device state from a network packet
	virtual void UnpackDeviceState( sInputDeviceState* State )
	{
		TODO( "make this faster using specific ClearState()" )

		int nk = GetNumKeys();

		for ( int i = 0 ; i < nk ; i++ ) { SetKeyState( i, false ); }

		if ( State->NumPressedKeys < 0 ) { return; }

		for ( int i = 0 ; i < State->NumPressedKeys ; i++ )
		{
			SetKeyState( State->PressedKeys[i], true );
		}

		// pack all axes
		int na = GetNumAxes();

		for ( int i = 0 ; i < na ; i++ ) { SetAxisValue( i, State->AxisValue[i] ); }
	}

	#pragma endregion

protected:
	int     FLastError;
	LString FLastErrorMsg;
};

/// Yet another 'manager' ? Enumerates the devices (using OS API or something like DirectInput)
class netexportable scriptfinal iInputDeviceList : public iObject
{
public:
	iInputDeviceList(): FLinearAccelerometer( NULL ), FGyroscope( NULL ), FGPS( NULL ) {}
	virtual ~iInputDeviceList();

	virtual void AfterConstruction();

	NET_EXPORTABLE()

	/// Set external window handle which accepts the events
	virtual void            SetWindowHandle( LWindowHandle Wnd );

	/// Get number of available devices
	virtual int             GetNumDevices() const
	{
		return static_cast<int>( FDevices.size() );
	}

	/// Get device with the specified index
	virtual iInputDevice*   GetDevice( int Idx ) const
	{
		if ( Idx < static_cast<int>( FDevices.size() ) && Idx >= 0 )
		{
			return FDevices[Idx];
		}

		return NULL;
	}

	/// Build a complete list of available devices
	virtual bool            ReenumerateDevices() = 0;

	/// Find the device by name
	virtual int  GetDeviceIndex( const LString& DeviceName );

	/// Get the default mouse
	virtual iInputDevice*   GetStdMouse() const;

	/**
	   Get the internal handle for default keyboard

	   If there are more than one keyboard we can easily combine them
	   into one device and return the key state for this "Virtual" device
	*/
	virtual iInputDevice*   GetStdKeyboard() const;

	/// Try to get a connected joystick
	virtual iInputDevice*   GetJoystick( int i ) const { return NULL; }

	/// Try to get a linear accelerometer
	virtual iInputDevice*   GetLinearAccelerometer() const;

	/// Try to get a gyroscope
	virtual iInputDevice*   GetGyroscope() const;

	/// Try to get a GPS
	virtual iInputDevice*   GetGPS() const;

	/// Last error code access
	virtual int       GetLastError() const { return FLastError; }

	/// Human-readable error message
	virtual LString   GetLastErrorMsg() const { return FLastErrorMsg; }

	/// Explicit device polling
	virtual void PollDevices();

	/*
	   /// API call for adding some 'non-standart' virtual devices (like network device)
	   virtual void            AddDevice(iInputDevice* Dev) = 0;

	   /// Remove specific device from list
	   virtual void            RemoveDevice(iInputDevice* Dev) = 0;

	   /// Deinitialize every device
	   virtual void            ClearDevices() = 0;

	   /// Register the listener for device events (e.g., a Console might want to listen for Keys)
	*/

	#pragma region Aliases an key/axis bindings

	/// Bind device key to the console variable
	scriptmethod void BindKey( int DeviceIdx, int KeyIdx, const LString& VarName );

	/// Bind device.axis value to the console variable
	scriptmethod void BindAxis( int DeviceIdx, int AxisIdx, const LString& VarName );

	/// Create an alias for the device. E.g., "Logitech Rumblepad 2 no.1" can be mapped to "Joy1"
	scriptmethod void SetDeviceAlias( const LString& SourceName, const LString& Alias );

	/// Remove alias
	scriptmethod void ClearAlias( const LString& Alias );

	/// Unbind device key
	scriptmethod void UnbindKey( int DeviceIdx, int KeyIdx );

	/// Unbind device.axis value to the console variable
	scriptmethod void UnbindAxis( int DeviceIdx, int AxisIdx );

	/// Binding update routine (update each bind variable in FKeyBindings and FAxisBindings)
	scriptmethod void UpdateBindVars();

	#pragma endregion

	#pragma region Force feedback
	scriptmethod void HapticFeedback( float Delay );
	scriptmethod void HapticFeedback_Tap();
	scriptmethod void HapticFeedback_LongPress();
	#pragma endregion

	#pragma region Console command implementers
	void BindKeyC( const LString& Params );
	void UnbindKeyC( const LString& Params );
	void BindAxisC( const LString& Params );
	void UnbindAxisC( const LString& Params );
	void AddDeviceAliasC( const LString& Params );
	void RemoveDeviceAliasC( const LString& Params );
	#pragma endregion

protected:

	/// The index of an axis or a key is the pair (DeviceIdx, Key/AxisIdx)
	typedef std::pair<int, int> LKeyAxisIdx;

	/// List of bindings
	typedef std::map<LKeyAxisIdx, clCVar*>  LBindList;

	/// Device aliases
	std::map<LString, LString> FAliases;

	/// Key bindings
	LBindList FKeyBindings;

	/// Axis bindings
	LBindList FAxisBindings;

protected:
	int     FLastError;
	LString FLastErrorMsg;

protected:
	LWindowHandle FHandle;

	/// Find device by name
	int FindDevice( const LString& DeviceName );

	std::vector<iInputDevice*> FDevices;

protected:
	void MyDebugString( const LString& Str );

public:
	static const int NUM_GPS_PARAMETERS = 6;

#if defined( OS_ANDROID )
	/// from accelerometer
	static LVector3 FLinearAcceleration;
	/// from gyroscope
	static LVector3 FVelocity;

	static float FParameters[NUM_GPS_PARAMETERS];
#endif

private:
	mutable iInputDevice* FLinearAccelerometer;
	mutable iInputDevice* FGyroscope;
	mutable iInputDevice* FGPS;
};

/**
   Implementation of the externally updated keyboard device
*/
class scriptfinal clKeyboardDevice : public iInputDevice
{
public:
	clKeyboardDevice() {}

	virtual ~clKeyboardDevice() {}

	/// Human-readable device name
	virtual LString   GetName() const { return "Keyboard"; }

	/* device polling is not performed */
	virtual bool      Poll() { return true; }

	/* device initialization is not required */
	virtual bool      Start() { return true; }
	virtual bool      IsStarted() const { return true; }
	virtual bool      Stop() { return true; }

	/* This device does not support any kind of axis */
	virtual int       GetNumAxes() const { return 0; }
	virtual bool      SetAxisRange( int AxisIdx, float MinVal, float MaxVal ) { return false; }
	virtual bool      SetAxisDeadZone( int AxisIdx, float DeadZone ) { return false; }
	virtual bool      SetAxisSaturation( int AxisIdx, float Saturation ) { return false; }
	virtual float     GetAxisValue( int AxisIdx ) const { return 0.0f; }
	virtual LString   GetAxisName( int AxisIdx ) const { return ""; }
	virtual void      SetAxisValue( int AxisIdx, float Val ) {}

	/// Human-readable key/button name
	virtual LString   GetKeyName( int KeyIdx ) const
	{
		/*
		   if (KeyNames > 255 || KeyIdx < 0) return LString("");
		   return Linderdaum::Utils::KeyNames[KeyIdx];
		*/
		return LString( "" );
	}

	/// check if the key/button is pressed
	virtual bool      IsPressed( int KeyIdx ) const { return FKeys[KeyIdx]; }

	/// Number of buttons for gamepads/mouses/keyboards
	virtual int       GetNumKeys() const { return 256; }

	/// External override for key states
	virtual void      SetKeyState( int KeyIdx, bool Pressed ) { FKeys[KeyIdx] = Pressed; }

private:
	bool FKeys[256];
};

/**
   Implementation of the externally updated mouse device
*/
class scriptfinal clMouseDevice : public iInputDevice
{
public:
	clMouseDevice() {};

	virtual ~clMouseDevice() {};

	/// Human-readable device name
	virtual LString   GetName() const { return "Mouse"; }

	/* device polling is not performed */
	virtual bool      Poll() { return true; }

	/* device initialization is not required */
	virtual bool      Start() { return true; }
	virtual bool      IsStarted() const { return true; }
	virtual bool      Stop() { return true; }

	/* This device does supports X and Y axis, Z axis might be the Wheel */
	virtual int       GetNumAxes() const { return 2; }
	virtual bool      SetAxisRange( int AxisIdx, float MinVal, float MaxVal ) { return false; }
	virtual bool      SetAxisDeadZone( int AxisIdx, float DeadZone ) { return false; }
	virtual bool      SetAxisSaturation( int AxisIdx, float Saturation ) { return false; }

	virtual float     GetAxisValue( int AxisIdx ) const
	{
		if ( AxisIdx == 0 ) { return FMouseX; }

		if ( AxisIdx == 1 ) { return FMouseY; }

		return 0.0f;
	}

	virtual LString   GetAxisName( int AxisIdx ) const
	{
		if ( AxisIdx == 0 ) { return "MouseX"; }

		if ( AxisIdx == 1 ) { return "MouseY"; }

		return LString( "" );
	}

	virtual void      SetAxisValue( int AxisIdx, float Val )
	{
		if ( AxisIdx == 0 ) { FMouseX = Val; }

		if ( AxisIdx == 1 ) { FMouseY = Val; }
	}

	/// Human-readable key/button name
	virtual LString   GetKeyName( int KeyIdx ) const
	{
		if ( KeyIdx == 0 ) { return "MouseLeft"; }

		if ( KeyIdx == 1 ) { return "MouseRight"; }

		if ( KeyIdx == 2 ) { return "MouseMiddle"; }

		return LString( "" );
	}

	/// check if the key/button is pressed
	virtual bool      IsPressed( int KeyIdx ) const
	{
		if ( KeyIdx == 0 ) { return FLeftButton; }

		if ( KeyIdx == 1 ) { return FRightButton; }

		if ( KeyIdx == 2 ) { return FMidButton; }

		return false;
	}

	/// Number of buttons for gamepads/mouses/keyboards
	virtual int       GetNumKeys() const { return 3; }

	/// External override for key states
	virtual void      SetKeyState( int KeyIdx, bool Pressed )
	{
		if ( KeyIdx == 0 ) { FLeftButton  = Pressed; }

		if ( KeyIdx == 1 ) { FRightButton = Pressed; }

		if ( KeyIdx == 2 ) { FMidButton   = Pressed; }
	}

protected:
	float FMouseX;
	float FMouseY;

	bool  FLeftButton;
	bool  FRightButton;
	bool  FMidButton;
};

class scriptfinal clLinearAccelerometerDevice : public iInputDevice
{
public:
	clLinearAccelerometerDevice(): FStarted( false ), FAcceleration( 0.0f ) {};

	virtual ~clLinearAccelerometerDevice() {};

	/// Human-readable device name
	virtual LString   GetName() const { return "LinearAccelerometer"; }

	virtual bool      Poll();

	/* device initialization is not required */
	virtual bool      Start();
	virtual bool      IsStarted() const { return FStarted; }
	virtual bool      Stop();

	virtual int       GetNumAxes() const { return 3; }
	virtual bool      SetAxisRange( int AxisIdx, float MinVal, float MaxVal ) { return false; }
	virtual bool      SetAxisDeadZone( int AxisIdx, float DeadZone ) { return false; }
	virtual bool      SetAxisSaturation( int AxisIdx, float Saturation ) { return false; }

	virtual LVector3  GetValues() const { return FAcceleration; };

	virtual float     GetAxisValue( int AxisIdx ) const
	{
		return FAcceleration[AxisIdx];
	}

	virtual LString   GetAxisName( int AxisIdx ) const
	{
		if ( AxisIdx == 0 ) { return "aX"; }

		if ( AxisIdx == 1 ) { return "aY"; }

		if ( AxisIdx == 2 ) { return "aZ"; }

		return LString( "" );
	}

	virtual void      SetAxisValue( int AxisIdx, float Val )
	{
		FAcceleration[AxisIdx] = Val;
	}

	/// Human-readable key/button name
	virtual LString   GetKeyName( int KeyIdx ) const
	{
		return LString( "" );
	}

	/// check if the key/button is pressed
	virtual bool      IsPressed( int KeyIdx ) const
	{
		return false;
	}

	/// Number of buttons for gamepads/mouses/keyboards
	virtual int       GetNumKeys() const { return 0; }

	/// External override for key states
	virtual void      SetKeyState( int KeyIdx, bool Pressed ) {}

private:
	bool     FStarted;
	LVector3 FAcceleration;
};

class scriptfinal clGyroscopeDevice : public iInputDevice
{
public:
	clGyroscopeDevice(): FStarted( false ), FVelocity( 0.0f ) {};

	virtual ~clGyroscopeDevice() {};

	/// Human-readable device name
	virtual LString   GetName() const { return "Gyroscope"; }

	virtual bool      Poll();

	/* device initialization is not required */
	virtual bool      Start();
	virtual bool      IsStarted() const { return FStarted; }
	virtual bool      Stop();

	virtual int       GetNumAxes() const { return 3; }
	virtual bool      SetAxisRange( int AxisIdx, float MinVal, float MaxVal ) { return false; }
	virtual bool      SetAxisDeadZone( int AxisIdx, float DeadZone ) { return false; }
	virtual bool      SetAxisSaturation( int AxisIdx, float Saturation ) { return false; }

	virtual LVector3  GetValues() const { return FVelocity; };

	virtual float     GetAxisValue( int AxisIdx ) const
	{
		return FVelocity[AxisIdx];
	}

	virtual LString   GetAxisName( int AxisIdx ) const
	{
		if ( AxisIdx == 0 ) { return "vX"; }

		if ( AxisIdx == 1 ) { return "vY"; }

		if ( AxisIdx == 2 ) { return "vZ"; }

		return LString( "" );
	}

	virtual void      SetAxisValue( int AxisIdx, float Val )
	{
		FVelocity[AxisIdx] = Val;
	}

	/// Human-readable key/button name
	virtual LString   GetKeyName( int KeyIdx ) const
	{
		return LString( "" );
	}

	/// check if the key/button is pressed
	virtual bool      IsPressed( int KeyIdx ) const
	{
		return false;
	}

	/// Number of buttons for gamepads/mouses/keyboards
	virtual int       GetNumKeys() const { return 0; }

	/// External override for key states
	virtual void      SetKeyState( int KeyIdx, bool Pressed ) {}

private:
	bool     FStarted;
	LVector3 FVelocity;
};

class scriptfinal clGPSDevice : public iInputDevice
{
public:
	static const int GPS_ALTITUDE  = 0;
	static const int GPS_LATITUDE  = 1;
	static const int GPS_LONGITUDE = 2;
	static const int GPS_SPEED     = 3;
	static const int GPS_BEARING   = 4;
	static const int GPS_TIME      = 5;
public:
	clGPSDevice(): FStarted( false ) {};

	virtual ~clGPSDevice() {};

	/// Human-readable device name
	virtual LString   GetName() const { return "GPS"; }

	virtual bool      Poll() { return true; };

	/* device initialization is not required */
	virtual bool      Start();
	virtual bool      IsStarted() const { return FStarted; }
	virtual bool      Stop();

	virtual int       GetNumAxes() const { return iInputDeviceList::NUM_GPS_PARAMETERS; }
	virtual bool      SetAxisRange( int AxisIdx, float MinVal, float MaxVal ) { return false; }
	virtual bool      SetAxisDeadZone( int AxisIdx, float DeadZone ) { return false; }
	virtual bool      SetAxisSaturation( int AxisIdx, float Saturation ) { return false; }

	virtual LVector3  GetValues() const
	{
#if defined( OS_ANDROID )
		return LVector3( iInputDeviceList::FParameters[0], iInputDeviceList::FParameters[1], iInputDeviceList::FParameters[2] );
#else
		return LVector3();
#endif
	};

	virtual float     GetAxisValue( int AxisIdx ) const
	{
#if defined( OS_ANDROID )
		return iInputDeviceList::FParameters[AxisIdx];
#else
		return 0.0f;
#endif
	}

	virtual LString   GetAxisName( int AxisIdx ) const
	{
		if ( AxisIdx == 0 ) { return "Altitude"; }

		if ( AxisIdx == 1 ) { return "Latitude"; }

		if ( AxisIdx == 2 ) { return "Longitude"; }

		if ( AxisIdx == 3 ) { return "Speed"; }

		if ( AxisIdx == 4 ) { return "Bearing"; }

		if ( AxisIdx == 5 ) { return "Time"; }

		return LString( "" );
	}

	virtual void      SetAxisValue( int AxisIdx, float Val )
	{
#if defined( OS_ANDROID )
		iInputDeviceList::FParameters[AxisIdx] = Val;
#endif
	}

	/// Human-readable key/button name
	virtual LString   GetKeyName( int KeyIdx ) const
	{
		return LString( "" );
	}

	/// check if the key/button is pressed
	virtual bool      IsPressed( int KeyIdx ) const
	{
		return false;
	}

	/// Number of buttons for gamepads/mouses/keyboards
	virtual int       GetNumKeys() const { return 0; }

	/// External override for key states
	virtual void      SetKeyState( int KeyIdx, bool Pressed ) {}

private:
	bool     FStarted;
};


#endif

/*
 * 04/01/2012
     GPS
 * 05/11/2011
     Gyroscope
 * 03/11/2011
     Linear accelerometer
 * 11/06/2010
     Log section added
*/
