#include "Math/LCurve.h"

#include "Math/LMath.h"
#include "Math/LGeomUtils.h"
#include "Math/LTransform.h"

#include <string.h>

/// Interpret an L-System string
void LCurve::InterpretLSystem( const char* L, float AngleDelta, float StepSize, float dt )
{
	ClearControlPoints();

	size_t len = strlen( L );

	LVector3 P( 0 );

	float Angle = 0.0f;
	LVector3 Dir( 1, 0, 0 );
	float t = 0.0f;

	for ( size_t i = 0 ; i < len ; i++ )
	{
		switch ( L[i] )
		{
			case 'F':
			case 'G':
			case 'L':
			case 'R':
			case 'A':
			case 'B':
				AddControlPoint( t, P );
				P += Dir * StepSize;
				break;
			case '-':
				Angle -= AngleDelta;
				break;
			case '+':
				Angle += AngleDelta;
				break;
		}

		float a = Linderdaum::Math::DegToRad( Angle );
		Dir = LVector3( cosf( a ), sinf( a ), 0.0f );
	}
}

/// Generate a helical curve
void LCurve::GenerateHelix( const LVector3& Start, const LVector3& Dir, float Radius, float T0, float TurnRate, float ShiftRate, int NumPoints )
{
	ClearControlPoints();

	const float dt = Linderdaum::Math::TWOPI / ( NumPoints - 1 );
	float t = T0;

	LVector3 V1, V2;
	BuildComplementaryBasis( Dir, V1, V2 );

	for ( int i = 0 ; i < NumPoints ; i++ )
	{
		LVector3 Pos = Start + V1 * cosf( t * TurnRate ) + V2 * sinf( t * TurnRate ) + ( t - T0 ) * ShiftRate * Dir;
		t += dt;
		AddControlPoint( t, Pos );
	}

	Recalculate();
}

/// Generate a circle on a plane (useful for trajectories)
void LCurve::GenerateCircle( const LVector3& Center, const LVector3& N, float Radius, int NumPoints )
{
	ClearControlPoints();

	LVector3 V1, V2;
	BuildComplementaryBasis( N, V1, V2 );

	LVector3 X = Radius * V1;
	LVector3 Y = Radius * V2;

	const float Delta = Linderdaum::Math::TWOPI / ( NumPoints - 1 );

	for ( int i = 0; i < NumPoints; ++i )
	{
		float Angle = Delta * i;

		LVector3 Point = Center + X * cosf( Angle ) + Y * sinf( Angle );

		AddControlPoint( Angle, Point );
	}

	Recalculate();
}

/// Generate a line with specified number of control points
void LCurve::GenerateStraightLine( const LVector3& Start, const LVector3& Dir, float Step, int NumPoints )
{
	ClearControlPoints();

	for ( int i = 0 ; i < NumPoints ; i++ )
	{
		AddControlPoint( ( float )i * Step, Start + ( float )i * Step * Dir );
	}

	Recalculate();
}

/// Add the curve to the end
void LCurve::Concatenate( const LCurve& Other )
{
	float MaxParam = GetMaximumParameter();

	for ( size_t k = 0 ; k < Other.P.size() ; k++ )
	{
		AddControlPoint( Other.T[k] + MaxParam, Other.P[k] );
	}

	Recalculate();
}

/// Subdivide each segment in NumSubDiv parts and output resulting points to the new curve
void LCurve::Refine( LCurve& Out, int NumSubDiv )
{
	if ( NumSubDiv <= 0 ) { return; }

	int N = static_cast<int>( T.size() );

	Out.ClearControlPoints();

	// 1. Generate new timestamps (T array)

	Out.T.resize( N * NumSubDiv );
	Out.P.resize( N * NumSubDiv );

	for ( int i = 0 ; i < N - 1 ; i++ )
	{
		float dt = ( T[i+1] - T[i] ) / static_cast<float>( NumSubDiv );
		float Timestamp = T[i];

		for ( int j = 0 ; j < NumSubDiv ; j++ )
		{
			Out.T[ i* NumSubDiv + j ] = Timestamp;

			Out.P[ i* NumSubDiv + j ] = GetPosition( Timestamp );

			Timestamp += dt;
		}
	}

	Recalculate();
}

template <class T> int FindSegmentFastA( const LArray<T>& Array, float Val )
{
	int from = 0;
	int to = static_cast<int>( Array.size() ) - 1;

	/// perform binary search in the array
	while ( from < to )
	{
		float fromVal = Array[from];
		float toVal   = Array[to];

		int mid = ( from + to ) / 2;

		float midVal = Array[mid];

		if ( to == from + 1 )
		{
			if ( Val < toVal && Val >= fromVal ) { return from; }
		}

		if ( Val < midVal ) { to   = mid - 1; }

		if ( Val > midVal ) { from = mid + 1; }
	}

	/// not found
	return -1;
}

/// Find the segment in array where the Val resides
template <class T> int FindSegmentFast( const LArray<T>& Array, float Val )
{
	int N = static_cast<int>( Array.size() );

	if ( Val <= Array[0] )
	{
		return -1;
	}

	for ( int i = 0 ; i < N - 1 ; i++ )
	{
		if ( Val > Array[i] && Val <= Array[i + 1] )
		{
			return i;
		}
	}

	return N - 1;
}

/// Shift all values from 'From' to the end by ShiftN elements
template<class T> void ShiftArray( LArray<T>& In, int From, int ShiftN )
{
	int Num = static_cast<int>( In.size() );

	if ( From < 0 || From >= Num ) { return; }

	if ( ShiftN > 0 )
	{
		In.resize( Num + ShiftN );

		for ( int i = Num - ShiftN ; i >= From ; i-- )
		{
			In[ i + ShiftN ] = In[i];
		}
	}
	else
	{
		for ( int i = From ; i < Num + ShiftN ; i-- )
		{
			In[ i ] = In[i - ShiftN];
		}

//		In.resize( Num + ShiftN );
	}
}

/// Add a control point, maintaining the T argument order
void LCurve::AddControlPoint( float t, const LVector3& Pos )
{
	T.push_back( t );
	P.push_back( Pos );
	/*

	int To = -1;

	if (T.size() > 0)
	{
	   To = FindSegmentFast( T, t);

	   if (To == -1)
	      if (t < T[0])
	         To = 0;
	}

	if (To == -1)
	{
	   T.push_back( t );
	   P.push_back( Pos );
	}
	else
	{
	   /// Shift arrays
	   ShiftArray(T, To, 1);
	   ShiftArray(P, To, 1);

	   T[To] = t;
	   P[To] = Pos;
	}
	*/
}

/// Remove specifed point
void LCurve::RemoveControlPoint( int i )
{
	ShiftArray( T, i, -1 );
	ShiftArray( P, i, -1 );
}

void LCurve::TransformPoints( const LMatrix4& Mtx )
{
	for ( size_t i = 0 ; i < P.size() ; i++ ) { P[i] = Mtx * P[i]; }
}

void LCurve::CopyTo( LCurve* Out ) const
{
	Out->P.resize( this->P.size() );
	Out->T.resize( this->T.size() );

	for ( size_t i = 0 ; i < P.size() ; i++ )
	{
		Out->P[i] = P[i];
		Out->T[i] = T[i];
	}

	Out->Recalculate();
}

/// Shift curve points along the path
void LCurve::TranslateAlongPath( const LCurve& Path, float FromT, float ToT )
{
	LMatrix4 Mtx;
	Path.GetTranstitionMatrix( FromT, ToT, &Mtx );
	TransformPoints( Mtx );
	Recalculate();
}

void LCurve::GetTranstitionMatrix( float FromT, float ToT, LMatrix4* Out ) const
{
	LVector3 DestPos = GetPosition( ToT );
	LVector3 SrcPos = GetPosition( FromT );
	LMatrix3 Src = GetCanonicalFrame( FromT );
	LMatrix3 Dest = GetCanonicalFrame( ToT );
	LMatrix3 SrcInv = Src.GetTransposed();
//	*Out = LMatrix4::GetTranslateMatrix(DestPos) * LMatrix4(Dest) * LMatrix4(SrcInv) * LMatrix4::GetTranslateMatrix(SrcPos);
//	*Out = LMatrix4::GetTranslateMatrix(DestPos) * LMatrix4(Dest) * LMatrix4(SrcInv) * LMatrix4::GetTranslateMatrix(-SrcPos);
	*Out = LMatrix4::GetTranslateMatrix( -SrcPos ) * LMatrix4( SrcInv ) * LMatrix4( Dest ) * LMatrix4::GetTranslateMatrix( DestPos );
}

/// Make a copy of curve segment
void LCurve::Extract( LCurve* Out, float FromT, float ToT, int NumPoints ) const
{
	float dt = ( ToT - FromT ) / ( float )( NumPoints - 1 );

	Out->ClearControlPoints();

	for ( int i = 0 ; i < NumPoints ; i++ )
	{
		float t = FromT + ( float )i * dt;
		Out->AddControlPoint( t, GetPosition( t ) );
	}
}


/// Form a canonical Frenet's frame (tangent, normal, binormal) for a given parameter
LMatrix3 LCurve::GetCanonicalFrame( float t ) const
{
	LVector3 R   = GetPosition( t );

	LMatrix3 Frame;

	Frame[0] = GetDerivative( t ).GetNormalized();
	Frame[1] = GetSecondDerivative( t ) ;

	if ( Frame[1].Length() > Linderdaum::Math::EPSILON )
	{
		Frame[1].Normalize();

		Frame[1] -= Frame[0].Dot( Frame[1] ) * Frame[0];

		Frame[2] = Frame[1].Cross( Frame[0] );
	}
	else
	{
		// linear curve or just a degenerate point
		BuildComplementaryBasis( Frame[0], Frame[1], Frame[2] );
	}

	for ( int i = 0 ; i < 3 ; i++ ) { Frame[i].Normalize(); }

	return Frame;
}

////// Linear interpolation

void LPiecewiseLinearCurve::Recalculate()
{
	int N = static_cast<int>( T.size() );

	K.resize( N );
	B.resize( N );

	for ( int i = 0 ; i < N - 1 ; i++ )
	{
		K[i] = ( P[i+1] - P[i] ) / ( T[i+1] - T[i] );
		B[i] =   P[i];
	}
}

LVector3 LPiecewiseLinearCurve::GetPosition( float t ) const
{
	int N = static_cast<int>( T.size() );

	int idx = FindSegmentFast( T, t );

	if ( idx < 0 ) { return P[0]; }

	if ( idx >= N - 1 ) { return P[N-1]; }

	return K[idx] * ( t - T[idx] ) + B[idx];
}

LVector3 LPiecewiseLinearCurve::GetDerivative( float t ) const
{
	int N = static_cast<int>( T.size() );

	int idx = FindSegmentFast( T, t );

	if ( idx < 0 ) { return K[0]; }

	if ( idx >= N - 1 ) { return K[N-1]; }

	return K[idx];
}

LVector3 LPiecewiseLinearCurve::GetSecondDerivative( float t ) const
{
	return LVector3( 0, 0, 0 );
}


////// Spline implementation

LVector3 LSpline::GetPosition( float t ) const
{
	int N = static_cast<int>( T.size() );

	int idx = FindSegmentFast( T, t );

	if ( idx < 0 ) { return P[0]; }

	if ( idx >= N - 1 ) { return P[N-1]; }

	return ACoeff[idx + 1] + t * ( BCoeff[idx + 1] + t * ( CCoeff[idx + 1] + t * DCoeff[idx + 1] ) );
}

LVector3 LSpline::GetDerivative( float t ) const
{
	int N = static_cast<int>( T.size() );

	int idx = FindSegmentFast( T, t );

	if ( idx < 0 ) { return dF[0]; }

	if ( idx >= N - 1 ) { return dF[N-1]; }

	return BCoeff[idx + 1] + t * ( 2.0f * CCoeff[idx + 1] + 3.0f * t * DCoeff[idx + 1] );
}

/// Calculate second derivative at a given point
LVector3 LSpline::GetSecondDerivative( float t ) const
{
	int N = static_cast<int>( T.size() );

	int idx = FindSegmentFast( T, t );

	if ( idx < 0 || idx >= N - 1 ) { return vec3( 0, 0, 0 ); }

	return 2.0f * CCoeff[idx + 1] + 6.0f * t * DCoeff[idx + 1];
}

/// Calculate 2 * ( 1/Prev + 1/Next )^-1 for each coordinate
inline LVector3 CombineSlope( const LVector3& Prev, const LVector3& Next )
{
	float Mult;
	LVector3 Res;

	for ( int i = 0 ; i < 3 ; i++ )
	{
		Mult = Prev[i] * Next[i];

		/// The slope for this coordinate does not change direction and is not zero
		Res[i] = ( Mult > Linderdaum::Math::EPSILON ) ? ( 2.0f / ( ( 1.0f / Prev[i] ) + ( 1.0f / Next[i] ) ) ) : 0.0f;
	}

	return Res;
}

void LSpline::Recalculate()
{
	int Num = static_cast<int>( P.size() );
	ACoeff.resize( Num );
	BCoeff.resize( Num );
	CCoeff.resize( Num );
	DCoeff.resize( Num );

	/// Resize internal temporary buffers
	dT.resize( Num );
	dF.resize( Num );

	/// Recalculate T distances
	for ( int i = 1 ; i < Num ; i++ )
	{
		dT[i] = 1.0f / ( T[i] - T[i-1] );
	}

	/// Precalculate derivative estimates
	for ( int i = 1 ; i < Num - 1 ; i++ )
	{
		dF[i] = CombineSlope( ( P[i+1] - P[i] ) * dT[i+1], ( P[i] - P[i-1] ) * dT[i] );
	}

	/// Boundary points for derivatives are a special case
	dF[0]     = ( 1.5f * ( P[1]     - P[0]     ) * dT[1]     ) - 0.5f * dF[1];
	dF[Num-1] = ( 1.5f * ( P[Num-1] - P[Num-2] ) * dT[Num-1] ) - 0.5f * dF[Num-2];

	float dt, ti, ti1;
	LVector3  dP, d2F, d2F1;

	const float One6 = ( 1.0f / 6.0f );

	/// Finally, calculate the coefficients for each segment
	for ( int i = 1 ; i < Num ; i++ )
	{
		dt  = dT[i];
		ti  = T[i];
		ti1 = T[i-1];

		dP  = 6.0f * ( P[i] - P[i-1] ) * dt;

		d2F1 = ( -2.0f * ( dF[i]   + 2.0f * dF[i-1] ) + dP ) * dt;
		d2F  = ( 2.0f * ( dF[i-1] + 2.0f * dF[i]  ) - dP ) * dt;

		// d
		DCoeff[i] = ( d2F - d2F1 ) * dt * One6;

		// c
		CCoeff[i] = ( T[i] * d2F1 - T[i-1] * d2F ) * 0.5f * dt;

		// b
		BCoeff[i] = One6 * dP - CCoeff[i] * ( ti + ti1 ) - DCoeff[i] * ( ti * ti + ti * ti1 + ti1 * ti1 );

		// a
		ACoeff[i] = P[i-1] - ( ( DCoeff[i] * T[i-1] + CCoeff[i] ) * T[i-1] + BCoeff[i] ) * T[i-1];
	}
}

/*
 * 08/01/2011
     It's here
*/
