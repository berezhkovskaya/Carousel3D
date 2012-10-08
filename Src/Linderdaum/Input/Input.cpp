/**
 * \file Input.cpp
 * \brief Input device and devices list
 * \version 0.6.06
 * \date 04/01/2012
 * \author Viktor Latypov, 2010-2012
 * \author Sergey Kosarevsky, 2010-2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Input.h"

#include "Environment.h"
#include "Core/CVars.h"
#include "Core/Console.h"
#include "Core/Linker.h"
#include "Core/Logger.h"
#include "Utils/LJNI.h"

void iInputDeviceList::PollDevices()
{
	int num = static_cast<int>( FDevices.size() );

	for ( int i = 0 ; i < num ; i++ )
	{
		FDevices[i]->Poll();
	}
}

iInputDeviceList::~iInputDeviceList()
{
	Env->Console->UnRegisterCommand( "BindKey",           Utils::Bind( &iInputDeviceList::BindKeyC,    this ) );
	Env->Console->UnRegisterCommand( "BindAxis",          Utils::Bind( &iInputDeviceList::BindAxisC,   this ) );
	Env->Console->UnRegisterCommand( "UnbindKey",         Utils::Bind( &iInputDeviceList::UnbindKeyC,  this ) );
	Env->Console->UnRegisterCommand( "UnbindAxis",        Utils::Bind( &iInputDeviceList::UnbindAxisC, this ) );
	Env->Console->UnRegisterCommand( "AddDeviceAlias",    Utils::Bind( &iInputDeviceList::AddDeviceAliasC,    this ) );
	Env->Console->UnRegisterCommand( "RemoveDeviceAlias", Utils::Bind( &iInputDeviceList::RemoveDeviceAliasC, this ) );

	delete( FLinearAccelerometer );
}

void iInputDeviceList::HapticFeedback( float Delay )
{
#if defined( OS_ANDROID )
	Android_HapticFeedback( Delay );
#endif
}

void iInputDeviceList::HapticFeedback_Tap()
{
#if defined( OS_ANDROID )
	Android_HapticFeedback_Tap();
#endif
}

void iInputDeviceList::HapticFeedback_LongPress()
{
#if defined( OS_ANDROID )
	Android_HapticFeedback_LongPress();
#endif
}

void iInputDeviceList::AfterConstruction()
{
	iObject::AfterConstruction();

	Env->Console->RegisterCommand( "BindKey",           Utils::Bind( &iInputDeviceList::BindKeyC,    this ) );
	Env->Console->RegisterCommand( "BindAxis",          Utils::Bind( &iInputDeviceList::BindAxisC,   this ) );
	Env->Console->RegisterCommand( "UnbindKey",         Utils::Bind( &iInputDeviceList::UnbindKeyC,  this ) );
	Env->Console->RegisterCommand( "UnbindAxis",        Utils::Bind( &iInputDeviceList::UnbindAxisC, this ) );
	Env->Console->RegisterCommand( "AddDeviceAlias",    Utils::Bind( &iInputDeviceList::AddDeviceAliasC,    this ) );
	Env->Console->RegisterCommand( "RemoveDeviceAlias", Utils::Bind( &iInputDeviceList::RemoveDeviceAliasC, this ) );
}

void iInputDeviceList::UpdateBindVars()
{
	for ( LBindList::iterator i = FKeyBindings.begin() ; i != FKeyBindings.end() ; i++ )
	{
		LKeyAxisIdx idx = i->first;
		iInputDevice* Dev = FDevices[idx.first];

		// set boolean flag for the keypress
		i->second->SetBool( Dev->IsPressed( idx.second ) );
	}

	for ( LBindList::iterator i = FAxisBindings.begin() ; i != FAxisBindings.end() ; i++ )
	{
		LKeyAxisIdx idx = i->first;
		iInputDevice* Dev = FDevices[idx.first];

		// set boolean flag for the keypress
		i->second->SetFloat( Dev->GetAxisValue( idx.second ) );
	}
}

void iInputDeviceList::BindKey( int DeviceIdx, int KeyIdx, const LString& VarName )
{
	if ( DeviceIdx < 0 || KeyIdx < 0 || VarName.empty() ) { return; }

	LKeyAxisIdx idx( DeviceIdx, KeyIdx );
	clCVar* Var = Env->Console->GetVar( VarName );
	FKeyBindings.insert( std::make_pair( idx, Var ) );
}

void iInputDeviceList::BindAxis( int DeviceIdx, int AxisIdx, const LString& VarName )
{
	if ( DeviceIdx < 0 || AxisIdx < 0 || VarName.empty() ) { return; }

	LKeyAxisIdx idx( DeviceIdx, AxisIdx );
	clCVar* Var = Env->Console->GetVar( VarName );
	FAxisBindings.insert( std::make_pair( idx, Var ) );
}

void iInputDeviceList::UnbindKey( int DeviceIdx, int KeyIdx )
{
	if ( DeviceIdx < 0 || KeyIdx < 0 ) { return; }

	LKeyAxisIdx idx( DeviceIdx, KeyIdx );

	FKeyBindings.erase( idx );
}

void iInputDeviceList::UnbindAxis( int DeviceIdx, int AxisIdx )
{
	if ( DeviceIdx < 0 || AxisIdx < 0 ) { return; }

	LKeyAxisIdx idx( DeviceIdx, AxisIdx );

	FAxisBindings.erase( idx );
}

void iInputDeviceList::ClearAlias( const LString& Alias )
{
	FAliases.erase( Alias );
}

void iInputDeviceList::SetDeviceAlias( const LString& SourceName, const LString& Alias )
{
	FAliases[Alias] = SourceName;
}

int iInputDeviceList::FindDevice( const LString& DeviceName )
{
	for ( size_t i = 0 ; i < FDevices.size() ; i++ )
	{
		if ( FDevices[i]->GetName() == DeviceName )
		{
			return static_cast<int>( i );
		}
	}

	return -1;
}

int iInputDeviceList::GetDeviceIndex( const LString& DeviceName )
{
	// 1. Try aliases
	if ( FAliases.count( DeviceName ) > 0 )
	{
		return FindDevice( FAliases[DeviceName] );
	}

	// 2. Return direct search result
	return FindDevice( DeviceName );
}

void iInputDeviceList::SetWindowHandle( LWindowHandle Wnd )
{
	FHandle = Wnd;
}

void iInputDeviceList::MyDebugString( const LString& Str )
{
	FLastErrorMsg = Str;
}

iInputDevice* iInputDeviceList::GetStdMouse() const
{
	return NULL;
}

iInputDevice* iInputDeviceList::GetStdKeyboard() const
{
	return NULL;
}

iInputDevice* iInputDeviceList::GetLinearAccelerometer() const
{
	if ( !FLinearAccelerometer ) { FLinearAccelerometer = Env->Linker->Instantiate( "clLinearAccelerometerDevice" ); }

	return FLinearAccelerometer;
}

iInputDevice* iInputDeviceList::GetGPS() const
{
	if ( !FGPS ) { FGPS = Env->Linker->Instantiate( "clGPSDevice" ); }

	return FGPS;
}

iInputDevice* iInputDeviceList::GetGyroscope() const
{
	if ( !FGyroscope ) { FGyroscope = Env->Linker->Instantiate( "clGyroscopeDevice" ); }

	return FGyroscope;
}

void iInputDeviceList::BindKeyC( const LString& Params )
{
	LString DeviceName = LStr::GetToken( Params, 1 );
	LString Key = LStr::GetToken( Params, 2 );
	LString Var = LStr::GetToken( Params, 3 );

	int DevIdx = FindDevice( DeviceName );

	if ( DevIdx < 0 )
	{
		// print error ?
		Env->Logger->Log( L_WARNING, LString( "Unable to bind: " ) + Params );

		return;
	}

	int KeyIdx = FDevices[DevIdx]->GetKeyIndex( Key );

	BindKey( DevIdx, KeyIdx, Var );
}

void iInputDeviceList::UnbindKeyC( const LString& Params )
{
	LString DeviceName = LStr::GetToken( Params, 1 );
	LString Key = LStr::GetToken( Params, 2 );

	int DevIdx = FindDevice( DeviceName );

	if ( DevIdx < 0 )
	{
		// print error ?
		Env->Logger->Log( L_WARNING, LString( "Unable to bind: " ) + Params );

		return;
	}

	int KeyIdx = FDevices[DevIdx]->GetKeyIndex( Key );

	UnbindKey( DevIdx, KeyIdx );
}

void iInputDeviceList::BindAxisC( const LString& Params )
{
	LString DeviceName = LStr::GetToken( Params, 1 );
	LString Axis = LStr::GetToken( Params, 2 );
	LString Var = LStr::GetToken( Params, 3 );

	int DevIdx = FindDevice( DeviceName );

	if ( DevIdx < 0 )
	{
		// print error ?
		Env->Logger->Log( L_WARNING, LString( "Unable to bind axis: " ) + Params );

		return;
	}

	int AxisIdx = FDevices[DevIdx]->GetAxisIndex( Axis );

	BindAxis( DevIdx, AxisIdx, Var );
}

void iInputDeviceList::UnbindAxisC( const LString& Params )
{
	LString DeviceName = LStr::GetToken( Params, 1 );
	LString Axis = LStr::GetToken( Params, 2 );

	int DevIdx = FindDevice( DeviceName );

	if ( DevIdx < 0 )
	{
		// print error ?
		Env->Logger->Log( L_WARNING, LString( "Unable to unbind axis: " ) + Params );

		return;
	}

	int AxisIdx = FDevices[DevIdx]->GetAxisIndex( Axis );

	UnbindAxis( DevIdx, AxisIdx );
}

void iInputDeviceList::AddDeviceAliasC( const LString& Params )
{
	LString AliasName = LStr::GetToken( Params, 1 );
	LString DeviceName = LStr::GetToken( Params, 2 );

	SetDeviceAlias( AliasName, DeviceName );
}

void iInputDeviceList::RemoveDeviceAliasC( const LString& Params )
{
	ClearAlias( LStr::GetToken( Params, 1 ) );
}

bool clLinearAccelerometerDevice::Start()
{
#if defined( OS_ANDROID )
	Android_EnableLinearAccelerometer( true );
#endif

	FStarted = true;

	return true;
}

bool clLinearAccelerometerDevice::Stop()
{
#if defined( OS_ANDROID )
	Android_EnableLinearAccelerometer( false );
#endif

	FStarted = false;

	return true;
}

bool clLinearAccelerometerDevice::Poll()
{
#if defined( OS_ANDROID )
	FAcceleration = iInputDeviceList::FLinearAcceleration;
#endif

	return true;
}

bool clGyroscopeDevice::Start()
{
#if defined( OS_ANDROID )
	Android_EnableGyroscope( true );
#endif

	FStarted = true;

	return true;
}

bool clGyroscopeDevice::Stop()
{
#if defined( OS_ANDROID )
	Android_EnableGyroscope( false );
#endif

	FStarted = false;

	return true;
}

bool clGyroscopeDevice::Poll()
{
#if defined( OS_ANDROID )
	FVelocity = iInputDeviceList::FVelocity;
#endif

	return true;
}

bool clGPSDevice::Start()
{
#if defined( OS_ANDROID )
	Android_EnableGPS( true );
#endif

	FStarted = true;

	return true;
}

bool clGPSDevice::Stop()
{
#if defined( OS_ANDROID )
	Android_EnableGPS( false );
#endif

	FStarted = false;

	return true;
}

/*
 * 04/01/2012
     GPS
 * 03/11/2011
     Linear accelerometer
 * 11/06/2010
     Log section added
*/
