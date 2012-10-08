/**
 * \file CollisionShape.cpp
 * \brief Collision shape
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "CollisionShape.h"
#include "RigidBody.h"

#include "Environment.h"

LCollisionShape::LCollisionShape()
{
}

LCollisionShape::~LCollisionShape()
{
}

iObject* LCollisionShape::GetOwner()
{
	if ( !FOwner )
	{
		// Find the owner in global objects graph
		if ( !FOwnerID.empty() )
		{
//			FOwner = Env->FindObjectByID(FOwnerID);
		}
	}

	return FOwner;
}

void LCollisionShape_Sphere::GetBodyParameters( clRigidBody* Body )
{
	FPosition = Body->GetPosition();
}

void LCollisionShape_Box::GetBodyParameters( clRigidBody* Body )
{
	FBox.FCenter = Body->GetPosition();
	FBox.FAxis = Body->GetMtxOrientation3();
}

void LCollisionShape_Box::CalculateExtents( const LVector3& Axis, float& Min, float& Max )
{
	// transform the corner points by FTransform, project them on the Axis and return max/min value
	LVector3 W( abs( FBox.FAxis[0].Dot( Axis ) ), abs( FBox.FAxis[1].Dot( Axis ) ), abs( FBox.FAxis[2].Dot( Axis ) ) );

	float d = FBox.FCenter.Dot( Axis );

	Min = d - ( W.Dot( FBox.FExtent ) );
	Max = d + ( W.Dot( FBox.FExtent ) );;
}

void LCollisionShape_Sphere::CalculateExtents( const LVector3& Axis, float& Min, float& Max )
{
	// project position to axis and add Radius

	float Projection = Axis.Dot( FPosition );

	Min = Projection - FRadius;
	Max = Projection + FRadius;
}

/*
 * 09/12/2010
     Log section added
*/
