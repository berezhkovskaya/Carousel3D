/*
   Type-to-string conversions
   (defined to avoid using ToString() methods which require #include<string> )
   (Part of Linderdaum Engine)
   Version 0.5.89a
   (23/03/2009)
   (C) Latypov Viktor, 2007-2009
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "LMathStrings.h"

namespace LStr
{
	LString Mat3ToStr( const LMatrix3& m, const char Separator )
	{
		return "(" + Vec3ToStr( m[0], Separator ) + ")(" + Vec3ToStr( m[1], Separator ) + ")(" + Vec3ToStr( m[2], Separator ) + ")";
	}

	LString Mat4ToStr( const LMatrix4& m, const char Separator )
	{
//		return "(" + Vec4ToStr( m[0], Separator ) + ")(" + Vec4ToStr( m[1], Separator ) + ")(" + Vec4ToStr( m[2], Separator ) + ")(" + Vec4ToStr( m[3], Separator ) + ")";
		LString Res;

		for ( int i = 0 ; i < 16 ; i++ )
		{
			Res += LStr::ToStr( m.ToFloatPtr()[i] );

			if ( i < 15 ) { Res += Separator; } //= LStr::ToFloat( LStr::GetToken( Str, i + 1 ) );
		}

		return Res;
	}

	LString Vec2ToStr( const LVector2& v, const char Separator )
	{
		return LStr::ToStrEpsilon( v.X ) + Separator + LStr::ToStrEpsilon( v.Y );
	}

	LString Vec3ToStr( const LVector3& v, const char Separator )
	{
		return LStr::ToStrEpsilon( v.X ) + Separator + LStr::ToStrEpsilon( v.Y ) + Separator + LStr::ToStrEpsilon( v.Z );
	}

	LString Vec4ToStr( const LVector4& v, const char Separator )
	{
		return LStr::ToStrEpsilon( v.X ) + Separator + LStr::ToStrEpsilon( v.Y ) + Separator + LStr::ToStrEpsilon( v.Z ) + Separator + LStr::ToStrEpsilon( v.W );
	}

	LVector3 StrToVec3( const LString& Str )
	{
		LVector3 Result;

		for ( int i = 0 ; i < 3 ; i++ )
		{
			Result[i] = LStr::ToFloat( LStr::GetToken( Str, i + 1 ) );
		}

		return Result;
	}

	LVector2 StrToVec2( const LString& Str )
	{
		LVector2 Result;

		for ( int i = 0 ; i < 2 ; i++ )
		{
			Result[i] = LStr::ToFloat( LStr::GetToken( Str, i + 1 ) );
		}

		return Result;
	}

	LVector4 StrToVec4( const LString& Str )
	{
		LVector4 Result;

		// convert from hexadecimal value, #6683a3 / #6683a300
		if ( Str.length() >= 7 && Str[0] == '#' )
		{
			LString SubStr = Str.substr( 1, Str.length() - 1 );
			uint64_t Color = StrToHex( SubStr );
			Result = ::Linderdaum::LColors::ToColor( Color );
		}
		else
			for ( int i = 0 ; i < 4 ; i++ )
			{
				Result[i] = LStr::ToFloat( LStr::GetToken( Str, i + 1 ) );
			}

		return Result;
	}

	LMatrix3 StrToMtx3( const LString& Str )
	{
		LMatrix3 Result;

		for ( int i = 0 ; i < 9 ; i++ )
		{
			Result.ToFloatPtr()[i] = LStr::ToFloat( LStr::GetToken( Str, i + 1 ) );
		}

		return Result;
	}

	LMatrix4 StrToMtx4( const LString& Str )
	{
		LMatrix4 Result;

		for ( int i = 0 ; i < 16 ; i++ )
		{
			Result.ToFloatPtr()[i] = LStr::ToFloat( LStr::GetToken( Str, i + 1 ) );
		}

		return Result;
	}

	LAABoundingBox StrToAABB( const LString& Str )
	{
		LAABoundingBox Box;

		for ( int i = 0 ; i < 3 ; i++ )
		{
			Box.FMin[i] = LStr::ToFloat( LStr::GetToken( Str, i + 0 ) );
			Box.FMax[i] = LStr::ToFloat( LStr::GetToken( Str, i + 3 ) );
		}

		return Box;
	}

	LString        AABBToStr( const LAABoundingBox& Box )
	{
		return Vec3ToStr( Box.FMin ) + LString( " " ) + Vec3ToStr( Box.FMax );
	}

	const LString AngularSystemNames[] =
	{
		LString( "xzxs" ), LString( "xzys" ), LString( "xyxs" ), LString( "xyzs" ),
		LString( "yxys" ), LString( "yxzs" ), LString( "yzys" ), LString( "yzxs" ),
		LString( "zyzs" ), LString( "zyxs" ), LString( "zxzs" ), LString( "zxys" ),
		LString( "xzxr" ), LString( "yzxr" ), LString( "xyxr" ), LString( "zyxr" ),
		LString( "yxyr" ), LString( "zxyr" ), LString( "yzyr" ), LString( "xzyr" ),
		LString( "zyzr" ), LString( "xyzr" ), LString( "zxzr" ), LString( "yxzr" )
	};

	LEulerAngleSystem StrToAngleSystem( const LString& Str )
	{
		for ( int i = 0 ; i < 24 ; i++ ) if ( Str == AngularSystemNames[i] )
			{
				return ( ( LEulerAngleSystem )( i ) );
			}

		return Euler_xzxs;
	}

	LString AngleSystemToStr( LEulerAngleSystem AS )
	{
		return AngularSystemNames[( int )( AS )];
	}

	const LString ProjectionName_Orthographic = "Orthographic";
	const LString ProjectionName_Perspective = "Perspective";
	const LString ProjectionName_Error = "Error";

	LString ProjectionTypeToStr( LProjectionType PT )
	{
		if ( PT == PROJECTION_ORTHOGRAPHIC )
		{
			return ProjectionName_Orthographic;
		}

		if ( PT == PROJECTION_PERSPECTIVE )
		{
			return ProjectionName_Perspective;
		}

		return ProjectionName_Error;
	}

	LProjectionType StrToProjectionType( const LString& Str )
	{
		if ( LStr::GetUpper( Str ) == LStr::GetUpper( ProjectionName_Orthographic ) )
		{
			return PROJECTION_ORTHOGRAPHIC;
		}

		if ( LStr::GetUpper( Str ) == LStr::GetUpper( ProjectionName_Perspective ) )
		{
			return PROJECTION_PERSPECTIVE;
		}

		return PROJECTION_ERROR;
	}

}; // namespace LStr

/*
 * 28/03/2009
     StrToMtx/Vec
 * 11/06/2007
     Log section added
*/
