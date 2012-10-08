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

#ifndef __LMathStrings__h__included__
#define __LMathStrings__h__included__

#include "Platform.h"
#include "LString.h"
#include "LString.h"
#include "Math/LMatrix.h"
#include "Math/LTransform.h"

#include "Math/LAABB.h"
#include "Math/LProjection.h"

namespace LStr
{

	LString Mat3ToStr( const LMatrix3& m, const char Separator = ' ' );
	LString Mat4ToStr( const LMatrix4& m, const char Separator = ' ' );
	LString Vec2ToStr( const LVector2& v, const char Separator = ' ' );
	LString Vec3ToStr( const LVector3& v, const char Separator = ' ' );
	LString Vec4ToStr( const LVector4& v, const char Separator = ' ' );

	inline LString QuatToStr( const LQuaternion& q, const char Serparator = ' ' )
	{
		return Vec4ToStr( q.ToVector4(), Serparator );
	}

	LVector2 StrToVec2( const LString& Str );
	LVector3 StrToVec3( const LString& Str );
	LVector4 StrToVec4( const LString& Str );
	LMatrix3 StrToMtx3( const LString& Str );
	LMatrix4 StrToMtx4( const LString& Str );

	inline LQuaternion StrToQuat( const LString& Str )
	{
		return LQuaternion( StrToVec4( Str ) );
	}

	LAABoundingBox StrToAABB( const LString& Str );
	LString        AABBToStr( const LAABoundingBox& Box );

	inline LTransform StrToTransform( const LString& Str )
	{
		return LTransform( StrToMtx4( Str ) );
	}
	inline LString TransformToStr( const LTransform& Transform )
	{
		Mat4ToStr( Transform.FMatrix );
	}

	LEulerAngleSystem StrToAngleSystem( const LString& Str );
	LString AngleSystemToStr( LEulerAngleSystem AS );

	LString ProjectionTypeToStr( LProjectionType PT );
	LProjectionType StrToProjectionType( const LString& Str );

	LVector4 StrToColor( const LString& Str );
	LString ColorToStr( const LVector4& Col );

}; // namespace LStr

namespace Linderdaum
{
	/// Transformation of colors between different representation formats
	namespace LColors
	{
		inline LVector4 BGRA8toRGBAvec4( const Luint32* Color )
		{
			Lubyte* BC = ( Lubyte* )Color;
			return LVector4( static_cast<float>( BC[2] ) / 255.0f,
			                 static_cast<float>( BC[1] ) / 255.0f,
			                 static_cast<float>( BC[0] ) / 255.0f,
			                 static_cast<float>( BC[3] ) / 255.0f );
		};

		inline LVector4 RGBA8toRGBAvec4( const Luint32* Color )
		{
			Lubyte* BC = ( Lubyte* )Color;
			return LVector4( static_cast<float>( BC[0] ) / 255.0f,
			                 static_cast<float>( BC[1] ) / 255.0f,
			                 static_cast<float>( BC[2] ) / 255.0f,
			                 static_cast<float>( BC[3] ) / 255.0f );
		};

		/// helper to convert hexadecimal RGB/RGBA-colors (like 0x6683a3) into vec4, alpha is set to 1.0 for values 0xFFFFFF and lower
		inline LVector4 ToColor( uint64_t Color )
		{
			return ( Color > 0xFFFFFF ) ? LVector4( float( ( Color >> 24 ) & 0xFF ) / 255.0f,
			                                        float( ( Color >> 16 ) & 0xFF ) / 255.0f,
			                                        float( ( Color >> 8  ) & 0xFF ) / 255.0f,
			                                        float( ( Color >> 0  ) & 0xFF ) / 255.0f ) :
			       LVector4( float( ( Color >> 16 ) & 0xFF ) / 255.0f,
			                 float( ( Color >> 8  ) & 0xFF ) / 255.0f,
			                 float( ( Color >> 0  ) & 0xFF ) / 255.0f, 1.0f );
		}
	}; // LColors

}; // Linderdaum

#endif

/*
 * 11/06/2007
     Log section added
*/
