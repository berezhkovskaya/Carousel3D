/**
 * \file Joints.h
 * \brief Skeleton joints
 * \version 0.5.82
 * \date 20/12/2007
 * \author Sergey Kosarevsky, 2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef  _Joints_
#define  _Joints_

#include "LString.h"
#include "Utils/LArray.h"
#include "Math/LVector.h"
#include "Math/LQuaternion.h"

class iIStream;
class iOStream;

/// Skeletal joint weight
struct sWeight
{
	int         FJointIndex;
	/// X,Y,Z - position, W - weight
	LVector4    FPosWeight;
};

/// Skeletal animation joint
struct sJoint
{
public:
	LString        FName;
	int            FParentIndex;
	LVector3       FPosition;
	LQuaternion    FOrientation;
	//
	LVector3 Transform( const LVector3& Vec ) const
	{
		return FOrientation.RotateVector( Vec ) + FPosition;
	}
	LVector3 InvTransform ( const LVector3& Vec ) const
	{
		LQuaternion  C( FOrientation );
		return C.Conjugate().RotateVector( Vec ) - FPosition;
	}
};

/// Skeletal animation skeleton
class clJointsSet
{
public:
	clJointsSet(): FJoints( 0 ) {};
	//
	// clJointsSet
	//
	void    SetMaxJoints( int Size )
	{
		FJoints.resize( Size );
	}
	int     GetJointsCount() const
	{
		return static_cast<int>( FJoints.size() );
	}
	void    SetJoint( int Index, const sJoint& Joint )
	{
		FJoints[ Index ] = Joint;
	}
	void    AddJoint( const sJoint& Joint )
	{
		FJoints.push_back( Joint );
	}
	sJoint&    GetJoint( int Index )
	{
		return FJoints[ Index ];
	}
	const sJoint&    GetJoint( int Index ) const
	{
		return FJoints[ Index ];
	}
	void    TransformJoints()
	{
		for ( int i = 0; i < GetJointsCount(); ++i )
		{
			sJoint& J = FJoints[ i ];

			if ( J.FParentIndex > -1 )
			{
				sJoint& P = FJoints[ J.FParentIndex ];

				J.FPosition    = P.FPosition + P.FOrientation.RotateVector ( J.FPosition );
				J.FOrientation = P.FOrientation * J.FOrientation;
			}
		}
	}
	void    SaveToStream( iOStream* Stream ) const;
	void    LoadFromStream( iIStream* Stream );
private:
	LArray<sJoint>    FJoints;
};

#endif

/*
 * 20/12/2007
     It's here
*/
