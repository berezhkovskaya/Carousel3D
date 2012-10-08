#ifndef __ScreenJoystick_included_
#define __ScreenJoystick_included_

#include "Input/Input.h"

#include "Math/LMath.h"

class clImage;
class clBitmap;

const int MAX_TOUCH_CONTACTS = 10;

/**
   \brief The description of a button for the on-screen joystick

   Defines the color of this button in a bitmask

   Derived from iObject to allow easy serialization
*/
class scriptfinal netexportable clBitmapButton: public iObject
{
public:
	clBitmapButton() {}
	virtual ~clBitmapButton() {}

	/** Property(Name=Color, Type=vec4, FieldName=FColor) */
	/** Property(Name=Index, Type=int,  FieldName=FIndex) */

public:
	/// Color in the mask for this button
	vec4 FColor;

	/// Button index in the virtual on-screen device
	int FIndex;
};

/**
   \brief The description of an axis for the on-screen joystick

   AxisX and AxisY specify the axis indices

   Derived from iObject to allow easy serialization
*/
class scriptfinal netexportable clBitmapAxis: public iObject
{
public:
	clBitmapAxis(): FAxis1( -1 ), FAxis2( -1 ), FRadius( 0.0f ), FPosition( 0, 0 ) {}
	virtual ~clBitmapAxis() {}

	scriptmethod bool HasAxis( int idx ) const { return ( FAxis1 == idx ) || ( FAxis2 == idx ); }

public:
	/** Property(Name=Position, Type=vec2,  FieldName=FPosition) */
	/** Property(Name=Radius,   Type=float, FieldName=FRadius) */

	/** Property(Name=Axis1,    Type=int,   FieldName=FAxis1) */
	/** Property(Name=Axis2,    Type=int,   FieldName=FAxis2) */

	/** Property(Name=Color,    Type=vec4,  FieldName=FColor) */

	/// Radius of the UI for these axes controller
	float FRadius;

	/// The center of the element
	vec2 FPosition;

	/// Index of the axis1
	int FAxis1;

	/// Index of the axis2
	int FAxis2;

	/// Color of the axis in the mask
	vec4 FColor;
};

class scriptfinal netexportable clScreenJoystick: public iInputDevice
{
public:
	clScreenJoystick(): FActive( false ), FMaskImage( NULL ), FDisplayImage( NULL ) {}

	/// Deallocate axis/button info
	virtual ~clScreenJoystick()
	{
		ClearButtons();
		ClearAxes();

		Stop();
	}

	/// ScreenJoystick-specific: rendering
	virtual void Render();

	/// Allocate button and axis state arrays
	virtual bool EndLoad()
	{
		int MaxAxisIdx = -1;
		int MaxKeyIdx = -1;

		/// Conversion of AxisIdx to clBitmapAxis pointer
		for ( size_t i = 0 ; i != FAxisDesc.size() ; i++ )
		{
			if ( FAxisDesc[i]->FAxis1 > MaxAxisIdx ) { MaxAxisIdx = FAxisDesc[i]->FAxis1; }

			if ( FAxisDesc[i]->FAxis2 > MaxAxisIdx ) { MaxAxisIdx = FAxisDesc[i]->FAxis2; }
		}

		for ( size_t i = 0 ; i != FButtonDesc.size() ; i++ )
		{
			if ( FButtonDesc[i]->FIndex > MaxKeyIdx ) { MaxKeyIdx = FButtonDesc[i]->FIndex; }
		}

		MaxKeyIdx = ::Math::Clamp( MaxKeyIdx + 1, 0, MAX_KEYS_IN_DEVICE );
		MaxAxisIdx = ::Math::Clamp( MaxAxisIdx + 1, 0, MAX_AXES_IN_DEVICE );

		FKeyState.resize( ( size_t )MaxKeyIdx );
		FKeyValue.resize( ( size_t )MaxKeyIdx );

		if ( FKeyState.size() > 0 )
		{
			memset( &FKeyState[0], 0, FKeyState.size() * sizeof( sKeyInfo ) );

//			memset(&(FKeyValue[0]), 0, FKeyState.size() * sizeof(bool));
			for ( size_t j = 0 ; j < FKeyValue.size() ; j++ ) { FKeyValue[j] = false; }
		}

		FAxisState.resize( ( size_t )MaxAxisIdx );
		FAxisValue.resize( ( size_t )MaxAxisIdx );

		if ( FAxisState.size() > 0 )
		{
			memset( &FAxisState[0], 0, FAxisState.size() * sizeof( sAxisInfo ) );
			memset( &FAxisValue[0], 0, FAxisState.size() * sizeof( float ) );
		}

		return iObject::EndLoad();
	}

	/*
	   script_method int GetBitmapAxisForIdx() const

	   /// Convert key index to bitmap
	   script_method int GetBitmapButtonForIdx() const
	*/

	virtual LString   GetName() const { return "On-Screen joystick"; }

	/// Explicit device polling (this type is event-driven)
	virtual bool      Poll() { return true; }

	/// Initialize device
	virtual bool      Start();

	/// Is the device started and active
	virtual bool      IsStarted() const { return FActive; }

	/// Deinitialize device
	virtual bool      Stop();

	#pragma region Axes are not supported yet (dummy implementation)

	/// number of axes for mouses/gamepads/joysticks
	virtual int       GetNumAxes() const { return ( int )FAxisState.size(); }

	/// Internal conversion of the axis value to [Min,Max] range
	virtual bool      SetAxisRange( int AxisIdx, float MinVal, float MaxVal ) { return false; }

	/// Set dead zone (minimum acceptable value) for the axis
	virtual bool      SetAxisDeadZone( int AxisIdx, float DeadZone ) { return false; }

	/// Set saturation value (high-cutoff, after which the axis is considered being at full swing)
	virtual bool      SetAxisSaturation( int AxisIdx, float Saturation ) { return false; }

	/// get current axis value
	virtual float     GetAxisValue( int AxisIdx ) const;

	/// Human-readable axis name
	virtual LString   GetAxisName( int AxisIdx ) const
	{
		return "";
		/* FAxisDesc.find_axis(AxisIdx)->GetAxisName();*/
	}

	/// External override for axis values
	virtual void      SetAxisValue( int AxisIdx, float Val );

	#pragma endregion

	/// Human-readable key/button name
	virtual LString   GetKeyName( int KeyIdx ) const { return FButtonDesc[KeyIdx]->GetObjectID(); }

	/// Check if the key/button is pressed
	virtual bool      IsPressed( int KeyIdx ) const;

	/// Number of buttons for gamepads/mouses/keyboards
	virtual int       GetNumKeys() const { return ( int )FButtonDesc.size(); }

	/// External override for key states
	virtual void      SetKeyState( int KeyIdx, bool Pressed );

	/// Event handler for the taps
	virtual void Event_MOTION( LEvent Event, const LEventArgs& Args );

public:
	/// A little serialization stuff

	/** Property(Name=Buttons, Type=clBitmapButton, IndexType=int, FieldName=FButtonDesc, NetIndexedSetter=SetButtonDesc, NetIndexedGetter=GetButtonDesc) */
	/** Property(Name=Axes,    Type=clBitmapAxis,   IndexType=int, FieldName=FAxisDesc, NetIndexedSetter=SetAxisDesc, NetIndexedGetter=GetAxisDesc) */

	/** Property(Name=MaskImageName,    Type=string, Getter=GetMaskImageName, Setter=SetMaskImageName) */
	/** Property(Name=DisplayImageName, Type=string, Getter=GetDisplayImageName, Setter=SetDisplayImageName) */

	scriptmethod clBitmapButton* GetButtonDesc( int idx ) const { return FButtonDesc[idx]; }
	scriptmethod clBitmapAxis*   GetAxisDesc( int idx ) const   { return FAxisDesc[idx]; }

	scriptmethod void SetButtonDesc( int idx, clBitmapButton* Val ) { FButtonDesc[idx] = Val; }
	scriptmethod void SetAxisDesc( int idx, clBitmapAxis* Val ) { FAxisDesc[idx] = Val; }

	scriptmethod clBitmapButton* GetButtonForColor( const vec4& Color ) const;
	scriptmethod clBitmapAxis* GetAxisForColor( const vec4& Color ) const;

public:
	scriptmethod LString GetMaskImageName() const;
	scriptmethod void SetMaskImageName( const LString& MF );

	scriptmethod LString GetDisplayImageName() const;
	scriptmethod void SetDisplayImageName( const LString& MF );

	void ClearButtons()
	{
		for ( size_t i = 0 ; i != FButtonDesc.size(); i++ ) { FButtonDesc[i]->DisposeObject(); }

		FButtonDesc.clear();
		FKeyState.clear();
	}

	void ClearAxes()
	{
		for ( size_t i = 0 ; i != FAxisDesc.size(); i++ ) { FAxisDesc[i]->DisposeObject(); }

		FAxisDesc.clear();
		FAxisState.clear();
	}

public:

	/// Currently pushed buttons
	clBitmapButton* FPushedButtons[MAX_TOUCH_CONTACTS];

	/// Current axes being sled
	clBitmapAxis*   FPushedAxis[MAX_TOUCH_CONTACTS];

	void ResetPushes();
	void ReadAxis( clBitmapAxis* Axis, const vec2& Pos );

	/// Buttons
	std::vector<clBitmapButton*> FButtonDesc;

	/// Axes
	std::vector<clBitmapAxis*> FAxisDesc;

	std::vector<sAxisInfo> FAxisState;
	std::vector<float> FAxisValue;

	std::vector<sKeyInfo> FKeyState;
	std::vector<bool> FKeyValue;

private:
	bool FActive;

	clImage* FMaskImage;
	clImage* FDisplayImage;

	vec4 GetColorAtPoint( const vec2& Pt ) const;

	mutable clBitmap* FMaskBitmap;
	mutable float FMaskW;
	mutable float FMaskH;
};

#endif // SJoy_included
