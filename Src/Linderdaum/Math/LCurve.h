#ifndef __LCurve__included__
#define __LCurve__included__

#include "Utils/LArray.h"

#include "Math/LMatrix.h"

/**
   \brief 3D curve

   The order of points is automatically maintained

   Interpolation is embedded in this class, no separation because all we need from this curve
   is the "Refine" function which outputs a number of additional nodes
**/
class LCurve
{
public:
	LCurve() {}
	virtual ~LCurve() {}

#pragma region Control points management

	/// Get specified control point
	LVector3 GetControlPoint( int i ) const { return P[i]; }

	/// Get the number of control points
	int GetNumControlPoints() const { return static_cast<int>( P.size() ); }

	/// Add a control point, maintaining the T argument order
	virtual void AddControlPoint( float t, const LVector3& Pos );

	/// Remove specifed point
	void RemoveControlPoint( int i );

	/// Clear the control points and associated arrays
	void ClearControlPoints() { P.clear(); T.clear(); }

	/// Transform each point with a given matrix
	void TransformPoints( const LMatrix4& Mtx );

	/// Determine minimum parameter value for this curve
	float GetMinimumParameter() const { return T.empty() ? 0.0f : T[0]; }

	/// Determine maximum parameter value for this curve
	float GetMaximumParameter() const { return T.empty() ? 0.0f : T[ T.size() - 1]; }

	/// Multiple timestamps by coefficient
	void ScaleTime( float Scale ) { for ( size_t kk = 0 ; kk < T.size() ; kk++ ) { T[kk] *= Scale; } }

	/// Shift timestamps by constant
	void ShiftTime( float Shift ) { for ( size_t kk = 0 ; kk < T.size() ; kk++ ) { T[kk] += Shift; } }

	/// Reverse the curve
	void Reverse() { for ( int kk = 0 ; kk < ( int )P.size() / 2 ; kk++ ) { SwapPointPositions( kk, ( int )P.size() - kk ); } }

	/// Swap P1 and P2
	void SwapPointPositions( int p1, int p2 ) { if ( p1 == p2 ) { return; } LVector3 Tmp = P[p1]; P[p1] = P[p2]; P[p2] = Tmp; }

#pragma endregion

#pragma region Positions and tangents

	/// Form a canonical Frenet's frame (tangent, normal, binormal) for a given parameter
	LMatrix3 GetCanonicalFrame( float t ) const;

	/// Get the position at a given point
	virtual LVector3 GetPosition( float t ) const = 0;

	/// Calculate first derivative (tangent vector) at a given point
	virtual LVector3 GetDerivative( float t ) const = 0;

	/// Calculate second derivative at a given point
	virtual LVector3 GetSecondDerivative( float t ) const = 0;

	/// Calculate a matrix of the shift-along-the-path transform in (t,n,b) basis
	scriptmethod void GetTranstitionMatrix( float FromT, float ToT, LMatrix4* Out ) const;

#pragma endregion

#pragma region Geometric operations

	/// Subdivide each segment in NumSubDiv parts and output resulting points to the new curve
	void Refine( LCurve& Out, int NumSubDiv );

	/// Generate a circle on a plane (useful for trajectories)
	void GenerateCircle( const LVector3& Center, const LVector3& N, float Radius, int NumPoints );

	/// Generate a line with specified number of control points
	void GenerateStraightLine( const LVector3& Start, const LVector3& Dir, float Step, int NumPoints );

	/// Generate a helical curve
	void GenerateHelix( const LVector3& Start, const LVector3& Dir, float Radius, float T0, float TurnRate, float ShiftRate, int NumPoints );

	/// Interpret an L-System string
	void InterpretLSystem( const char* L, float AngleDelta, float StepSize, float dt );

	/// Add the curve to the end
	void Concatenate( const LCurve& Other );

	/// Make a copy of curve segment
	void Extract( LCurve* Out, float FromT, float ToT, int NumPoints ) const;

	/// Shift curve points along the path
	void TranslateAlongPath( const LCurve& Path, float FromT, float ToT );

	/// Copy curve control points
	void CopyTo( LCurve* Out ) const;

#pragma endregion

	/// Rebuild internal coefficients
	virtual void Recalculate() = 0;

public:
	/// Temporal variables
	LArray<float> T;

	/// List of control points
	LArray<LVector3>  P;
};

/// Simple piecewise linear curve
class LPiecewiseLinearCurve : public LCurve
{
public:
	LPiecewiseLinearCurve() {}
	virtual ~LPiecewiseLinearCurve() {}

	/// Get the position at a given point
	virtual LVector3 GetPosition( float t ) const;

	/// Calculate first derivative (tangent vector) at a given point
	virtual LVector3 GetDerivative( float t ) const;

	/// Calculate second derivative at a given point
	virtual LVector3 GetSecondDerivative( float t ) const;

	/// Rebuild internal coefficients
	virtual void Recalculate();

public:
	/// k*x+b (k coefficient)
	LArray<LVector3> K;

	/// k*x+b (b coefficient)
	LArray<LVector3> B;
};

/**
   \brief Constrained cubic spline

   Extension to 3D of the http://www.korf.co.uk/spline.pdf
*/
class LSpline : public LCurve
{
public:
	LSpline() {}
	virtual ~LSpline() {}

	/// Get the position at a given point
	virtual LVector3 GetPosition( float t ) const;

	/// Calculate first derivative (tangent vector) at a given point
	virtual LVector3 GetDerivative( float t ) const;

	/// Calculate second derivative at a given point
	virtual LVector3 GetSecondDerivative( float t ) const;

	/// Recalculate interpolating coefficients for each segment
	virtual void Recalculate();

public:
	/// (X,Y,Z,W) quad represents (a_i, b_i, c_i, d_i) coefficients for each segment
	LArray<LVector3> ACoeff, BCoeff, CCoeff, DCoeff;

private:
	/// Inverse distances between points
	LArray<float> dT;

	/// Array with precalculated f'_i(t_i) values
	LArray<LVector3> dF;
};

#endif

/*
 * 09/01/2011
     Reimplemented from former Curve3D
     Added splines
*/
