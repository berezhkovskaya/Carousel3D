/**
 * \file Joints.cpp
 * \brief Skeleton joints
 * \version 0.5.82
 * \date 15/02/2008
 * \author Sergey Kosarevsky, 2008
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Joints.h"
#include "Core/VFS/iIStream.h"
#include "Core/VFS/iOStream.h"

void clJointsSet::SaveToStream( iOStream* Stream ) const
{
	size_t JointsCount = FJoints.size();

	Stream->BlockWrite( &JointsCount, sizeof( JointsCount ) );

	for ( size_t i = 0; i != JointsCount; ++i )
	{
		Stream->WriteLine( FJoints[i].FName );
		Stream->BlockWrite( &FJoints[i].FParentIndex, sizeof( FJoints[i].FParentIndex ) );
		Stream->BlockWrite( &FJoints[i].FPosition,    sizeof( FJoints[i].FPosition ) );
		Stream->BlockWrite( &FJoints[i].FOrientation, sizeof( FJoints[i].FOrientation ) );
	}
}

void clJointsSet::LoadFromStream( iIStream* Stream )
{
	FJoints.clear();

	size_t JointsCount = 0;

	Stream->BlockRead( &JointsCount, sizeof( JointsCount ) );

	FJoints.reserve( JointsCount );

	for ( size_t i = 0; i != JointsCount; ++i )
	{
		sJoint Joint;

		Joint.FName = Stream->ReadLine();

		Stream->BlockRead( &Joint.FParentIndex, sizeof( Joint.FParentIndex ) );
		Stream->BlockRead( &Joint.FPosition,    sizeof( Joint.FPosition ) );
		Stream->BlockRead( &Joint.FOrientation, sizeof( Joint.FOrientation ) );

		FJoints.push_back( Joint );
	}
}

/*
 * 15/02/2008
     It's here
*/
