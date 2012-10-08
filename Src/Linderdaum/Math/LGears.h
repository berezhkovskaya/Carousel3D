/**
 * \file LGears.h
 * \brief
 * \version 0.6.00
 * \date 27/01/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LGears_
#define _LGears_

#include "Platform.h"
#include "Math/LMath.h"
#include "Math/LVector.h"

namespace Linderdaum
{
	/// Linderdaum Math Library
	namespace Math
	{
		inline LVector2 GetInvolute( float R, float Alpha )
		{
			return R * LVector2( cosf( Alpha ) + Alpha * sinf( Alpha ), sinf( Alpha ) - Alpha * cosf( Alpha ) );
		}

		inline LVector2 GetRotatedInvolute( float BaseRadius, float InvAngle, float Angle )
		{
			float Sin1 = sin( Angle );
			float Cos1 = cos( Angle );

			LVector2 P1 = GetInvolute( BaseRadius, InvAngle );
			LVector2 R1( P1.X * Cos1 - P1.Y * Sin1, P1.X * Sin1 + P1.Y * Cos1 );

			return R1;
		}

		inline void GetGearParams( float Module, int NumberOfTeeth, float ProfileShift, float* BaseDiam, float* RootDiam, float* OutsideDiam, float* WorkPitchDiam )
		{
			///// ISO Involute Gear

			/// standard params for equal spur gears
			const float PressureAngle   = 20.0f * Math::DTOR;
			const float Z1              = static_cast<float>( NumberOfTeeth );
			const float Z2              = Z1;
			const float X1              = ProfileShift;
			const float X2              = ProfileShift;
			const float AlphaW          = 2 * tan( PressureAngle ) * ( X1 + X2 ) / ( Z1 + Z2 ) + tan( PressureAngle ) - PressureAngle;
			const float PitchDiameter   = NumberOfTeeth * Module;                        // d
			// const float CenterDistance  = PitchDiameter / 2.0f + ProfileShift * Module;  // Ax
			const float BaseDiameter    = PitchDiameter * cos( PressureAngle );          // db
			const float WorkPitchD      = BaseDiameter / cos( AlphaW );
			const float Addendum        = Module * ( 1.0f + ProfileShift );
			const float WholeDepth      = 2.25f * Module;
			const float OutsideDiameter = PitchDiameter + 2.0f * Addendum;
			const float RootDiameter    = OutsideDiameter - 2.0f * WholeDepth;

			if ( BaseDiam ) { *BaseDiam = BaseDiameter; }

			if ( RootDiam ) { *RootDiam = RootDiameter; }

			if ( OutsideDiam ) { *OutsideDiam = OutsideDiameter; }

			if ( WorkPitchDiam ) { *WorkPitchDiam = WorkPitchD; }
		}
	}
}

#endif

/*
 * 27/01/2011
     It's here
*/
