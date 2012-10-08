/**
 * \file LTransform.h
 * \brief Transformation
 * \version 0.5.91
 * \date 22/08/2010
 * \author Viktor Latypov, 2009-2010
 * \author Setgey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __Transform__h__included__
#define __Transform__h__included__

#include "LQuaternion.h"

/**
 INAR-encoding for Euler angles system
 (index - negation - alternate - reversal)

 i n a r  Sys.  i n a r  Sys.  i n a r  Sys.  i n a r  Sys.
(1,0,0,0) xzxs (1,0,1,0) xzys (1,1,0,0) xyxs (1,1,1,0) xyzs
(2,0,0,0) yxys (2,0,1,0) yxzs (2,1,0,0) yzys (2,1,1,0) yzxs
(3,0,0,0) zyzs (3,0,1,0) zyxs (3,1,0,0) zxzs (3,1,1,0) zxys
(1,0,0,1) xzxr (1,0,1,1) yzxr (1,1,0,1) xyxr (1,1,1,1) zyxr
(2,0,0,1) yxyr (2,0,1,1) zxyr (2,1,0,1) yzyr (2,1,1,1) xzyr
(3,0,0,1) zyzr (3,0,1,1) xyzr (3,1,0,1) zxzr (3,1,1,1) yxzr
*/

enum LEulerAngleSystem
{
	Euler_xzxs = 0, Euler_xzys, Euler_xyxs, Euler_xyzs,
	Euler_yxys, Euler_yxzs, Euler_yzys, Euler_yzxs,
	Euler_zyzs, Euler_zyxs, Euler_zxzs, Euler_zxys,
	Euler_xzxr, Euler_yzxr, Euler_xyxr, Euler_zyxr,
	Euler_yxyr, Euler_zxyr, Euler_yzyr, Euler_xzyr,
	Euler_zyzr, Euler_xyzr, Euler_zxzr, Euler_yxzr
};

/// Utility class representing the 3D transformation
class LTransform
{
public:
	LTransform();
	LTransform( const LVector3& pos, const LQuaternion& quat );
	LTransform( const LMatrix4& mtx4 );

	void SetPositionAndAngles( const LVector3& Pos, float AngleX, float AngleY, float AngleZ );
	void SetPositionAndAngles( const LVector3& Pos, const LVector3& Angles );
	void SetPositionAndAxisAngle( const LVector3& Pos, const LVector3& Axis, float Angle );
	void SetPositionAndOrientation( const LVector3& pos, const LQuaternion& quat );
	void SetPosMatrixAndAxisAngle( const LMatrix4& Pos, const LVector3& Axis, float Angle );
	void SetMatrix4( const LMatrix4& Mtx4 )
	{
		FMatrix = Mtx4;
	};

	const LMatrix4& GetMatrix4() const
	{
		return FMatrix;
	};
	void GetPositionAndOrientation( LVector3& Pos, LQuaternion& Q ) const;

	void LookAt( const LVector3& From, const LVector3& To, const LVector3& Up );

	/// Lerp + SLerp between O1 and O2 for t in [0,1]
	void Interpolate( const LTransform& O1, const LTransform& O2, float t );

	scriptmethod void              SetAngleSystem( LEulerAngleSystem AS )
	{
		FAngleSystem = AS;
	}
	scriptmethod LEulerAngleSystem GetAngleSystem() const
	{
		return FAngleSystem;
	}

	void     SetAngles( const LVector3& Angles );
	LVector3 GetAngles() const;

	void     SetAngleTriple( float T1, float T2, float T3 );
	void     GetAngleTriple( float& T1, float& T2, float& T3 ) const;

	virtual void     SetPosition( const LVector3& P );
	virtual LVector3 GetPosition() const;
public:
	LMatrix4 FMatrix;

	/// Currently used 3-angle system for orientation
	LEulerAngleSystem FAngleSystem;
};

scriptexport( Math ) void DecomposeTransformation( const LMatrix4& T, LVector3& Pos, LMatrix4& Rot );
scriptexport( Math ) void DecomposeCameraTransformation( const LMatrix4& T, LVector3& Pos, LMatrix4& Rot );

scriptexport( Math ) LMatrix4 ComposeTransformation( const LVector3& Pos, const LMatrix4& Rot );
scriptexport( Math ) LMatrix4 ComposeCameraTransformation( const LVector3& Pos, const LMatrix4& Rot );

/**
   Calculate three Euler angles from orientation in a given axis system (ZXZ etc.)
*/
void MatrixToAngles( LEulerAngleSystem Sys, const LMatrix3& M, float& T1, float& T2, float& T3 );

/**
   Calculate orientation from three Euler angles in a given axis system (ZXZ etc.)
*/
void AnglesToMatrix( LEulerAngleSystem Sys, LMatrix4& M, float T1, float T2, float T3 );

#endif

/*
 * 01/03/2010
     Log section added
*/
