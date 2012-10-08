#include "ScreenJoystick.h"

#include "Environment.h"
#include "Resources/ResourcesManager.h"
#include "Images/Image.h"
#include "Images/Bitmap.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/Canvas.h"

#include "Math/LMathStrings.h"

clBitmapButton* clScreenJoystick::GetButtonForColor( const vec4& Color ) const
{
	for ( size_t k = 0 ; k < FButtonDesc.size(); k++ )
	{
		if ( ( FButtonDesc[k]->FColor - Color ).Length() < 0.1f ) { return FButtonDesc[k]; }
	}

	return NULL;
}

clBitmapAxis* clScreenJoystick::GetAxisForColor( const vec4& Color ) const
{
	for ( size_t k = 0 ; k < FAxisDesc.size(); k++ )
	{
		if ( ( FAxisDesc[k]->FColor - Color ).Length() < 0.1f ) { return FAxisDesc[k]; }
	}

	return NULL;
}

vec4 clScreenJoystick::GetColorAtPoint( const vec2& Pt ) const
{
	if ( !FMaskBitmap )
	{
		FMaskBitmap = FMaskImage->GetCurrentBitmap();
		FMaskW = ( float )FMaskBitmap->GetWidth();
		FMaskH = ( float )FMaskBitmap->GetHeight();
	}

	if ( !FMaskBitmap ) { return vec4( -1 ); }

	float x = Pt.X * FMaskW;
	float y = Pt.Y * FMaskH;
	return vec4( FMaskBitmap->GetPixel( ( int )x, ( int )y, 0 ).ToVector3(), 1.0f );
}

void clScreenJoystick::Render()
{
	if ( !FDisplayImage ) { return; }

	// We avoid calling flush()
	Env->Renderer->GetCanvas()->TexturedRect( 0, 0, 1, 1, FDisplayImage->GetTexture(), NULL, LC_White );
}

bool clScreenJoystick::Start()
{
	if ( FActive ) { return false; }

	CONNECTOBJ( L_EVENT_MOTION, &clScreenJoystick::Event_MOTION, this );

	FActive = true;

	ResetPushes();

	return true;
}

void clScreenJoystick::ResetPushes()
{
	memset( &FPushedAxis[0], 0, sizeof( clBitmapAxis* ) * MAX_TOUCH_CONTACTS );
	memset( &FPushedButtons[0], 0, sizeof( clBitmapButton* ) * MAX_TOUCH_CONTACTS );
}

bool clScreenJoystick::Stop()
{
	if ( !FActive ) { return false; }

	Env->DisconnectObject( this );

	FActive = false;

	return true;
}

bool clScreenJoystick::IsPressed( int KeyIdx ) const
{
	if ( KeyIdx < 0 || KeyIdx >= ( int )FKeyValue.size() ) { return false; }

	return FKeyValue[KeyIdx];
}

void clScreenJoystick::SetKeyState( int KeyIdx, bool Pressed )
{
	if ( KeyIdx < 0 || KeyIdx >= ( int )FKeyValue.size() ) { return; }

	FKeyValue[KeyIdx] = Pressed;
}

float clScreenJoystick::GetAxisValue( int AxisIdx ) const
{
	if ( ( AxisIdx < 0 ) || AxisIdx >= ( int )FAxisValue.size() ) { return 0.0f; }

	return FAxisValue[AxisIdx];
}

void clScreenJoystick::SetAxisValue( int AxisIdx, float Val )
{
	if ( ( AxisIdx < 0 ) || AxisIdx >= ( int )FAxisValue.size() ) { return; }

	FAxisValue[AxisIdx] = Val;
}

void clScreenJoystick::ReadAxis( clBitmapAxis* Axis, const vec2& Pos )
{
	if ( Axis )
	{
		// read axis value based on center and touch point
		int a1 = Axis->FAxis1;
		int a2 = Axis->FAxis2;

		float v1 = (  ( Axis->FPosition - Pos ).X / Axis->FRadius ) * 10000.0f;
		float v2 = ( -( Axis->FPosition - Pos ).Y / Axis->FRadius ) * 10000.0f;

		this->SetAxisValue( a1, v1 );
		this->SetAxisValue( a2, v2 );
	}
}

void clScreenJoystick::Event_MOTION( LEvent Event, const LEventArgs& Args )
{
	// unpack the motion data
	int ContactID = ( int )Args.FPtrArg;
	LVector2 Pos( Args.FFloatArg, Args.FFloatArg1 );
	bool Pressed = Args.FBoolArg;

	LMotionFlag Flag = ( LMotionFlag )( ( int )Args.FObjArg );

	if ( Flag == L_MOTION_START ) { return; }

	if ( Flag == L_MOTION_END ) { return; }

	if ( ContactID < 0 ) { return; }

	/// clear all previous presses/axis slides
//	if(Flag == L_MOTION_MOVE || Flag == L_MOTION_UP)
	{
		// check if this ContactID was already pressed
		clBitmapButton* PrevButton = FPushedButtons[ContactID];
		clBitmapAxis* PrevAxis = FPushedAxis[ContactID];

		if ( PrevButton )
		{
			this->SetKeyState( PrevButton->FIndex, false );
			FPushedButtons[ContactID] = NULL;
		}

		if ( PrevAxis )
		{
			if ( Pressed )
			{
				if ( PrevAxis->FAxis1 > 0 ) { this->SetAxisValue( PrevAxis->FAxis1, 0.0f ); }

				if ( PrevAxis->FAxis2 > 0 ) { this->SetAxisValue( PrevAxis->FAxis2, 0.0f ); }
			}

			FPushedAxis[ContactID] = NULL;
		}
	}

	if ( Flag == L_MOTION_DOWN || Flag == L_MOTION_MOVE )
	{
		vec4 Color = GetColorAtPoint( Pos );
		clBitmapButton* Button = GetButtonForColor( Color );

		if ( Button )
		{
			// touchdown, set the key
			int Idx = Button->FIndex;

			this->SetKeyState( Idx, true );

			// and store the initial button/color to track movement later
			FPushedButtons[ContactID] = Button;
		}
		else
		{
			clBitmapAxis* Axis = GetAxisForColor( Color );

			if ( Axis )
			{
				if ( Pressed )
				{
					this->ReadAxis( Axis, Pos );
					FPushedAxis[ContactID] = Axis;
				}
			}
		}
	}
}

void clScreenJoystick::SetMaskImageName( const LString& MF )
{
	FMaskBitmap = NULL;
	FMaskImage = Env->Resources->LoadImg( MF, L_TEXTURE_2D );
}

void clScreenJoystick::SetDisplayImageName( const LString& MF )
{
	FDisplayImage = Env->Resources->LoadImg( MF, L_TEXTURE_2D );
}

LString clScreenJoystick::GetMaskImageName() const { return FMaskImage ? FMaskImage->GetFileName() : LString( "" ); }

LString clScreenJoystick::GetDisplayImageName() const { return FDisplayImage ? FDisplayImage->GetFileName() : LString( "" ); }
