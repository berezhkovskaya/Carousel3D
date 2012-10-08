/**
 * \file CVars.cpp
 * \brief OLEVariant-like untyped variable
 * \version 0.5.98
 * \date 01/12/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "CVars.h"
#include "Environment.h"
#include "Core/Console.h"
#include "Math/LMathStrings.h"

const LVector2& clCVar::GetVector2() const
{
	return FVector2;
}

const LVector3& clCVar::GetVector3() const
{
	return FVector3;
}

const LVector4& clCVar::GetVector4() const
{
	return FVector4;
}

const LMatrix3& clCVar::GetMatrix3() const
{
	FMatrix3 = FMatrix4.ExtractMatrix3();
	return FMatrix3;
}

const LMatrix4& clCVar::GetMatrix4() const
{
	return FMatrix4;
}

void clCVar::SetString( const LString& S )
{
	FString  = S;
	FInt     = LStr::ToInt( S );
	FDouble  = LStr::ToDouble( S );
	FBoolean = LStr::ToBool( S );
	FVector2  = LVector2( static_cast<float>( FDouble ) );
	FVector3  = LVector3( static_cast<float>( FDouble ) );
	FVector4  = LStr::StrToVec4( S );
	FMatrix4  = LMatrix4();
	SendSync( L_EVENT_CHANGED, LEventArgs( this ), false );
}

void clCVar::SetInt( const int L )
{
	FString   = LStr::ToStr( L );
	FInt      = L;
	FDouble   = static_cast<double>( L );
	FBoolean  = L > 0;
	FVector2  = LVector2( static_cast<float>( FDouble ) );
	FVector3  = LVector3( static_cast<float>( FDouble ) );
	FVector4  = LVector4( static_cast<float>( FDouble ) );
	FMatrix4  = LMatrix4( static_cast<float>( FDouble ) );
	SendSync( L_EVENT_CHANGED, LEventArgs( this ), false );
}

void clCVar::SetDouble( const double D )
{
	FString  = LStr::ToStr( D );
	FInt     = static_cast<int>( D );
	FDouble  = D;
	FBoolean = FInt > 0;
	FVector2  = LVector2( static_cast<float>( FDouble ) );
	FVector3  = LVector3( static_cast<float>( FDouble ) );
	FVector4  = LVector4( static_cast<float>( FDouble ) );
	FMatrix4  = LMatrix4( static_cast<float>( FDouble ) );
	SendSync( L_EVENT_CHANGED, LEventArgs( this ), false );
}


void clCVar::SetFloat( const float F )
{
	FString  = LStr::ToStr( F );
	FInt     = static_cast<int>( F );
	FDouble  = F;
	FBoolean = FInt > 0;
	FVector2  = LVector2( F );
	FVector3  = LVector3( F );
	FVector4  = LVector4( F );
	FMatrix4  = LMatrix4( F );
	SendSync( L_EVENT_CHANGED, LEventArgs( this ), false );
}

void clCVar::SetBool( const bool B )
{
	FString  = B ? "TRUE" : "FALSE";
	FInt     = B;
	FDouble  = B;
	FBoolean = B;
	FVector2  = LVector2( B );
	FVector3  = LVector3( B );
	FVector4  = LVector4( B );
	FMatrix4  = LMatrix4( B );
	SendSync( L_EVENT_CHANGED, LEventArgs( this ), false );
}

void clCVar::SetVector2( const LVector2& Vec )
{
	FString  = LStr::Vec2ToStr( Vec, ' ' );
	FInt     = static_cast<int>( Vec.X );
	FDouble  = Vec.X;
	FBoolean = Vec.X > 0;
	FVector2  = Vec;
	FVector3  = LVector3( Vec.X, Vec.Y, 0.0f );
	FVector4  = LVector4( Vec.X, Vec.Y, 0.0f, 0.0f );
	FMatrix4  = LMatrix4();
	SendSync( L_EVENT_CHANGED, LEventArgs( this ), false );
}

void clCVar::SetVector3( const LVector3& Vec )
{
	FString  = LStr::Vec3ToStr( Vec, ' ' );
	FInt     = static_cast<int>( Vec.X );
	FDouble  = Vec.X;
	FBoolean = Vec.X > 0;
	FVector2  = Vec.ToVector2();
	FVector3  = Vec;
	FVector4  = LVector4( Vec );
	FMatrix4  = LMatrix4();
	SendSync( L_EVENT_CHANGED, LEventArgs( this ), false );
}

void clCVar::SetVector4( const LVector4& Vec )
{
	FString  = LStr::Vec4ToStr( Vec, ' ' );
	FInt     = static_cast<int>( Vec.X );
	FDouble  = Vec.X;
	FBoolean = Vec.X > 0;
	FVector2  = LVector2( Vec.X, Vec.Y );
	FVector3  = Vec.ToVector3();
	FVector4  = Vec;
	FMatrix4  = LMatrix4();
	SendSync( L_EVENT_CHANGED, LEventArgs( this ), false );
}

void clCVar::SetMatrix3( const LMatrix3& Mat )
{
	FMatrix4.SetSubMatrix( Mat );
	SetMatrix4( FMatrix4 );
	SendSync( L_EVENT_CHANGED, LEventArgs( this ), false );
}

void clCVar::SetMatrix4( const LMatrix4& Mat )
{
	FString  = LStr::Mat4ToStr( Mat, ' ' );
	FInt     = static_cast<int>( Mat[0][0] );
	FDouble  = Mat[0][0];
	FBoolean = Mat[0][0] > 0;
	FVector2  = LVector2( Mat[0].X, Mat[0].Y );
	FVector3  = Mat[0].ToVector3();
	FVector4  = Mat[0];
	FMatrix4  = Mat;
	SendSync( L_EVENT_CHANGED, LEventArgs( this ), false );
}

void clCVar::AddInt( const int Val )
{
	FInt      = FInt + Val;
	FString   = LStr::ToStr( FInt );
	FDouble   = static_cast<double>( FInt );
	FBoolean  = FInt > 0;
	FVector2  = LVector2( static_cast<float>( FDouble ) );
	FVector3  = LVector3( static_cast<float>( FDouble ) );
	FVector4  = LVector4( static_cast<float>( FDouble ) );
	FMatrix4  = LMatrix4( static_cast<float>( FDouble ) );
	SendSync( L_EVENT_CHANGED, LEventArgs( this ), false );
}

clCVar* LCVar::GetCVar() const
{
	if ( !FCVar )
	{
		FCVar = FDefaultValue.empty() ? ( *Env )->Console->GetVar( FCVarName ) : ( *Env )->Console->GetVarDefault( FCVarName, FDefaultValue );
	}

	return FCVar;
}

/*
 * 01/12/2010
     LCVar
 * 23/02/2007
     SetFloat()
     GetFloat()
 * 17/02/2007
     Untyped vars
 * 10/02/2005
     It's here
*/
