#ifndef __FlowSelector_h__included__
#define __FlowSelector_h__included__

#include "Core/iObject.h"
#include "Math/LCurve.h"

/**
   \brief Flow-motion selector

   This class only implements the geometrical routines to calculate each image's position.
   Coverflow control with dynamic image/texture loading and gesture support is a whole different story.

   Created using four direction curves (points of each image move on these curves).

   Coverflow requires similar arctangent-like curves. Carousel is just the same - simple another guiding curves (circles).

   Remarks:
      - QuadCoords and RebuildCurves methods are declared virtual to allow overrides for some special effects
*/
class scriptfinal clFlowSelector: public iObject
{
public:
	clFlowSelector():
		FNumPoints( 100 ), FPointStep( 0.2f ), FControlExp( 0.001f ),
		FHeight( 4.0f ), FElevation( 2.0f ), FSlope( 0.3f ), FPeakOffset( 3.0f ),
		FFlowExp( 0.01f ), FFlowMult( 4.0f )
	{
	}

	/// Transformation for the Quad
	mtx4 GetQuadTransform( float t_center, float TSize ) const
	{
		vec3 Pts[4];
		QuadCoords( Pts, t_center, TSize );

		mtx4 QuadTranslate = mtx4::GetTranslateMatrix( ( Pts[0] + Pts[1] + Pts[2] + Pts[3] ) / 4.0f );
		mtx4 QuadRotate = mtx4::Identity();

		vec3 axis[3] = { ( Pts[2] - Pts[1] ).GetNormalized(), ( Pts[1] - Pts[0] ).GetNormalized(), vec3( 0 ) };
		axis[2] = axis[1].Cross( axis[0] );

		for ( int l = 0 ; l < 3 ; l++ )
			for ( int k = 0 ; k < 3 ; k++ )
			{
				QuadRotate[l][k] = axis[l][k];
			}

		return ( QuadRotate * QuadTranslate );
	}

	/// Calculate coordinates for the Quad in a given position
	virtual void QuadCoords( vec3* Pts, float t_center, float TSize ) const
	{
		float _Ofs[] = { TSize / 2, -TSize / 2, -TSize / 2, TSize / 2 };

		for ( int i = 0 ; i < 4 ; i++ ) { Pts[i] = FBaseCurve[i].GetPosition( t_center - _Ofs[i] ); }
	}

	#pragma region Fine-tuning properties

	/** Property(Description="Number of control points on the base curves", Name=NumPoints, Type=int, FieldName=FNumPoints) */

	/** Property(Description="Height of the images", Name=ImageHeight, Type=float, FieldName=FHeight) */
	/** Property(Description="Elevation of the flow curve", Name=Elevation, Type=float, FieldName=FElevation) */
	/** Property(Description="Slope of the flow curve", Name=Slope, Type=float, FieldName=FSlope) */
	/** Property(Description="Symmetric offset", Name=PeakOffset, Type=float, FieldName=FPeakOffset) */

	/** Property(Description="Elevation curve amplitude", Name=ElevationAmplitude, Type=float, FieldName=FFlowMult) */
	/** Property(Description="Elevation curve slope parameter", Name=ElevationSlope, Type=float, FieldName=FFlowExp) */

	/// Number of control points on the flow curve
	int FNumPoints;

	/// Height of the image (distnace between lower and upper curves)
	float FHeight;

	/// "Thickness" of the curve
	float FElevation;

	/// Slopiness of the curve
	float FSlope;

	/// Symmetric displacement of curve peaks
	float FPeakOffset;

	/// Exponential falloff coefficient
	float FFlowMult;

	/// Exponential coefficient (0.01 is OK)
	float FFlowExp;

	/// Number of units between sequential control points
	float FPointStep;

	/// Tweaking parameter for the quad points speed. Used internally
	float FControlExp;

	#pragma endregion

	/// Trajectory control points for each base curve
	LSpline FBaseCurve[4];

public:
	virtual void RebuildCurves()
	{
		float _Y[] = { FHeight, FHeight, 0, 0 };// { FHeight/2.0f, FHeight/2.0f, -FHeight/2.0f, -FHeight/2.0f };
		float _Ofs[] = { FPeakOffset, -FPeakOffset, -FPeakOffset, FPeakOffset };
		float _Coef[] = { FSlope, -FSlope, -FSlope, FSlope };

		for ( int i = 0 ; i < 4 ; i++ )
		{
			FBaseCurve[i].ClearControlPoints();

			for ( int j = -FNumPoints / 2 ; j < FNumPoints / 2 + 1 ; j++ )
			{
				float t = ( float )j * FPointStep;
				/// Displacement is possible here. Morphing between two sets of curves is also possible
				FBaseCurve[i].AddControlPoint( t * expf( FControlExp * t * t ), BaseCurvePoint( t, _Y[i], FElevation, _Coef[i], -_Ofs[i] ) );
			}

			FBaseCurve[i].Recalculate();
		}
	}

private:
	/// Base curve for the point of image quad
	vec3 BaseCurvePoint( float t, float _Y, float CElevation, float CSlope, float CPeakOffset ) const
	{
		float P = CSlope * ( t + CPeakOffset );

		// Simple arctangent multiplied by exp(-x^2). Much better
		float Mult = FFlowMult * exp( -FFlowExp * P * P );
		return vec3( t, Mult * CElevation * atanf( P ) / ( Math::PI ), _Y );

		// Witch of Agnesi, works strange
//		return vec3(t, _Y, CElevation / (1.0f + P * P));
		// Circle parametrization would give a carousel-like appearance. We just got to make periodic curves.
//		return vec3(CElevation * sin(t + CPeakOffset), _Y, CElevation * cos(t + CPeakOffset));
	}
};

#endif // if FlowSelector_included

/*
 * 08/12/2011
     No more magic constants, no rendering code
 * 06/12/2011
     First version
*/
