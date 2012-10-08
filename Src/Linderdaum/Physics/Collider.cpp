/**
 * \file Collider.cpp
 * \brief Collider
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Collider.h"

#include "Environment.h"
#include "Core/Linker.h"
#include "CollisionShape.h"
#include "CollisionIntervals.h"

LCollider::LCollider()
{
}

LCollider::~LCollider()
{
}

LCollisionShape_Sphere* LCollider::CreateCollisionSphere( const LVector3& Pos, float Rad ) const
{
	LCollisionShape_Sphere* Result = Env->Linker->Instantiate( "LCollisionShape_Sphere" );

	Result->FPosition = Pos;
	Result->FRadius   = Rad;

	return Result;
}

LCollisionShape_Box* LCollider::CreateCollisionBox   ( const LMatrix4& Transform, const LVector3& Extents ) const
{
	LCollisionShape_Box* Result = Env->Linker->Instantiate( "LCollisionShape_Box" );

	Result->FBox.FAxis = Transform.ExtractMatrix3();
	Result->FBox.FCenter = Transform[3].ToVector3(); // or vec3(Transform[0][3], [1][3], [2][3])
	Result->FBox.FExtent = Extents;

	return Result;
}

LCollisionShape_Plane* LCollider::CreateCollisionPlane ( const LVector4& Eq ) const
{
	LCollisionShape_Plane* Result = Env->Linker->Instantiate( "LCollisionShape_Plane" );

	Result->FEquation = Eq;

	return Result;
}

void LCollider::ClearCallbacks()
{
	memset( &FDualCallback[0][0], 0, sizeof( LBroadPhaseDualCollisionCallback* ) * MAX_COLLISION_CLASSES * MAX_COLLISION_CLASSES );
	memset( &FSingleCallback[0][0], 0, sizeof( LBroadPhaseSingleCollisionCallback* ) * MAX_COLLISION_CLASSES * MAX_COLLISION_CLASSES );
}

void LCollider::SetDualCollisionCallback( int Class1, int Class2 , LBroadPhaseDualCollisionCallback* Callback )
{
	if ( Class1 < 0 || Class2 < 0 ) { return; }

	if ( Class1 >= MAX_COLLISION_CLASSES || Class2 >= MAX_COLLISION_CLASSES ) { return; }

	FDualCallback[Class1][Class2] = Callback;
}

void LCollider::SetSingleCollisionCallback( int Class1, int Class2 , LBroadPhaseSingleCollisionCallback* Callback )
{
	if ( Class1 < 0 || Class2 < 0 ) { return; }

	if ( Class1 >= MAX_COLLISION_CLASSES || Class2 >= MAX_COLLISION_CLASSES ) { return; }

	FSingleCallback[Class1][Class2] = Callback;
}


void LCollider::AddObject( LCollisionShape* Shape )
{
	// Add Shape to each list
}

void LCollider::RemoveObject( LCollisionShape* Shape )
{
	// Remove shape from each list
}

void LCollider::DetectBroadPhaseCollisions()
{
	// 1. Check each AxisList

	// 2. Select intersecting objects from each list
}

void LCollider::ClearCollisionFlags()
{
	for ( int k = 0 ; k < 3 ; k++ )
		for ( size_t i = 0 ; i < FIntervalList[k]->FIntervals.size() ; i++ )
		{
			FIntervalList[k]->FIntervals[i]->FCollisionState = CT_UNDEFINED;
		}
}

void LCollider::BruteForceIntersect()
{
	// 1. Check each axis list
	for ( int k = 0 ; k < 3 ; k++ )
	{
		LAxisList* Axis = FIntervalList[k];

		// do N*(N-1)/2 checks
		for ( size_t i = 0 ; i < Axis->FIntervals.size() ; i++ )
		{
			for ( size_t j = i + 1 ; j < Axis->FIntervals.size() ; j++ )
			{

			}
		}
	}
}

/*
 * 09/12/2010
     Log section added
*/
