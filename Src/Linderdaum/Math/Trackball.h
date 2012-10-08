/**
 * \file Trackball.h
 * \brief Virtual trackball
 * \version 0.5.89
 * \date 27/11/2008
 * \author Sergey Kosarevsky, 2008
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _VirtualTrackball_
#define _VirtualTrackball_

#include "Platform.h"
#include "Math/LMath.h"

/// Virtual trackball for user interaction with rotations
class netexportable clVirtualTrackball
{
public:
	clVirtualTrackball(): FCurrentPoint( 0.0f ),
		FPrevPoint( 0.0f )
	{
		FRotation.IdentityMatrix();
		FRotationDelta.IdentityMatrix();
	};

	/**
	   Pick initial mouse point
	**/
	virtual void StartDragging( LVector2 ScreenPoint )
	{
		FRotation = FRotation * FRotationDelta;
		FCurrentPoint = ProjectOnSphere( ScreenPoint );
		FPrevPoint    = FCurrentPoint;
		FRotationDelta.IdentityMatrix();
	}

	/**
	   Get rotation matrix for new mouse point
	**/
	virtual LMatrix4 DragTo( LVector2 ScreenPoint, float Speed )
	{
		FCurrentPoint = ProjectOnSphere( ScreenPoint );

		LVector3 Direction = FCurrentPoint - FPrevPoint;

		LMatrix4 RotMatrix;

		RotMatrix.IdentityMatrix();

		float Shift = Direction.Length();

		if ( Shift > ::Linderdaum::Math::EPSILON )
		{
			LVector3 Axis = FPrevPoint.Cross( FCurrentPoint );

			RotMatrix.RotateMatrixAxis( Shift * Speed, Axis );
		}

		FRotationDelta = RotMatrix;

		return RotMatrix;
	}

	LMatrix4& GetRotationDelta()
	{
		return FRotationDelta;
	};

	/**
	   Get current rotation matrix
	**/
	virtual LMatrix4 GetRotationMatrix() const
	{
		return FRotation * FRotationDelta;
	}

	static clVirtualTrackball* Create()
	{
		return new clVirtualTrackball();
	}

private:
	LVector3 ProjectOnSphere( LVector2 ScreenPoint )
	{
		LVector3 Proj;

		// convert to -1.0...1.0 range
		Proj.X = 2.0f * ScreenPoint.X - 1.0f;
		Proj.Y = -( 2.0f * ScreenPoint.Y - 1.0f );
		Proj.Z = 0.0f;

		float Length = Proj.Length();

		Length = ( Length < 1.0f ) ? Length : 1.0f;

		Proj.Z = sqrtf( 1.001f - Length * Length );
		Proj.Normalize();

		return Proj;
	}
	LVector3   FCurrentPoint;
	LVector3   FPrevPoint;
	LMatrix4   FRotation;
	LMatrix4   FRotationDelta;
};

#endif

/*
 * 27/11/2008
     It's here
*/
