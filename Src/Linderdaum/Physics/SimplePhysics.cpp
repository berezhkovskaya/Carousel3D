/**
 * \file SimplePhysics.cpp
 * \brief Simple physics
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "SimplePhysics.h"

#include "Environment.h"
#include "Core/Linker.h"

#include "Collider.h"
#include "CollisionShape.h"

#include "LColors.h"
#include "Renderer/Canvas.h"

#include "Math/LTransform.h"

SimplePhysics::SimplePhysics() : FBodies(), FCollider( NULL )
{
}

SimplePhysics::~SimplePhysics()
{
	Clear( true );
	FCollider->DisposeObject();
}

/**
   Stupid ugly double-dispatch down the CollisionShape hierarchy

   FIXME: This IS slow, we have to switch to table-driven implementation with no downcasting
*/
bool SimplePhysics::CollideBodies( clRigidBody* Body1, clRigidBody* Body2, Linderdaum::Math::sCollisionInfo* Info )
{
	LCollisionShape* Sh1 = Body1->FCollisionShape;
	LCollisionShape* Sh2 = Body2->FCollisionShape;

	if ( !Sh1 || !Sh2 ) { return false; }

	LCollisionShape_Box* Box1 = dynamic_cast<LCollisionShape_Box*>( Sh1 );
	LCollisionShape_Box* Box2 = dynamic_cast<LCollisionShape_Box*>( Sh2 );

	LCollisionShape_Sphere* Sphere1 = dynamic_cast<LCollisionShape_Sphere*>( Sh1 );
	LCollisionShape_Sphere* Sphere2 = dynamic_cast<LCollisionShape_Sphere*>( Sh2 );

	LCollisionShape_Plane* Plane1 = dynamic_cast<LCollisionShape_Plane*>( Sh1 );
	LCollisionShape_Plane* Plane2 = dynamic_cast<LCollisionShape_Plane*>( Sh2 );

	if ( Plane1 && Plane2 ) { return false; }

	if ( Box1 && Plane2 )
	{
		return Math::Collision_BoxToPlane( Box1->FBox, LPlane( Plane2->FEquation ), Info );
	}

	if ( Box2 && Plane1 )
	{
		if ( Math::Collision_BoxToPlane( Box2->FBox, LPlane( Plane1->FEquation ), Info ) )
		{
			Info->Swap();
			return true;
		}

		return false;
	}

	if ( Sphere1 && Plane2 )
	{
		LVector3 Pt;
		float Depth;
		LVector3 N;

		if ( Math::Collision_SphereToPlane( LSphere( Sphere1->FPosition, Sphere1->FRadius ), LPlane( Plane2->FEquation ), &Pt, &N, &Depth ) )
		{
			Info->FNumPoints = 1;
			Info->FPoints[0] = Info->FPoints1[0] = Info->FPoints2[0] = Pt;
			Info->FPenetration[0] = Depth;
			Info->N1 = N;
			Info->N2 = -N;
		}

		return false;
	}

	if ( Sphere2 && Plane1 )
	{
		LVector3 Pt;
		float Depth;
		LVector3 N;

		if ( Math::Collision_SphereToPlane( LSphere( Sphere2->FPosition, Sphere2->FRadius ), LPlane( Plane1->FEquation ), &Pt, &N, &Depth ) )
		{
			Info->FNumPoints = 1;
			Info->FPoints[0] = Info->FPoints1[0] = Info->FPoints2[0] = Pt;
			Info->FPenetration[0] = Depth;
			Info->N1 = -N;
			Info->N2 = N;
		}

		return false;
	}

	if ( Box1 && Box2 )
	{
		return Math::Collision_BoxToBox( Box1->FBox, Box2->FBox, Info );
	}

	if ( Box1 && Sphere2 )
	{
		if ( Math::Collision_SphereToBox( LSphere( Sphere2->FPosition, Sphere2->FRadius ), Box1->FBox, Info ) )
		{
			Info->Swap();
			return true;
		}
		else
		{
			return false;
		}
	}

	if ( Box2 && Sphere1 )
	{
		return Math::Collision_SphereToBox( LSphere( Sphere1->FPosition, Sphere1->FRadius ), Box2->FBox, Info );
	}

	if ( Sphere1 && Sphere2 )
	{
		LVector3 Pt;
		float Depth;
		LVector3 N;

		if ( Math::Collision_SphereToSphere( LSphere( Sphere1->FPosition, Sphere1->FRadius ), LSphere( Sphere2->FPosition, Sphere2->FRadius ), &Pt, &N, &Depth ) )
		{
			Info->FNumPoints = 1;
			Info->FPoints[0] = Info->FPoints1[0] = Info->FPoints2[0] = Pt;
			Info->FPenetration[0] = Depth;
			Info->N1 = N;
			Info->N2 = -N;
		}

		return false;
	}

	return false;
}

void SimplePhysics::AfterConstruction()
{
	FCollider = Env->Linker->Instantiate( "LCollider" );
}

/// Infinite static plane
scriptmethod clRigidBody* SimplePhysics::CreatePlane( const LVector3& Point, const LVector3& Normal )
{
	/// 1. Create body
	clRigidBody* Body = Env->Linker->Instantiate( "clRigidBody" );

	Body->MakeZeroParams();

	/// 2. Assign mass properties
	Body->SetMass( -1.0f );

	/// 3. Create collision geometry
	LCollisionShape* Geometry = FCollider->CreateCollisionPlane( LPlane( Point, Normal ).GetEquation() );
	Geometry->SetOwner( Body );

	Body->SetCollisionShape( Geometry );

	return Body;
}

/// Create solid sphere
clRigidBody* SimplePhysics::CreateSolidSphere( float Mass, float Radius )
{
	/// 1. Create body
	clRigidBody* Body = Env->Linker->Instantiate( "clRigidBody" );

	Body->MakeZeroParams();

	/// 2. Assign mass properties
	Body->SetMass( Mass );
	Body->SetInertiaTensor( InertiaTensor_SolidSphere( Mass, Radius ) );

	/// 3. Create collision geometry
	LCollisionShape* Geometry = FCollider->CreateCollisionSphere( vec3( 0, 0, 0 ), Radius );
	Geometry->SetOwner( Body );

	Body->SetCollisionShape( Geometry );

	return Body;
}

/// Create thin shell with a spherical shape
clRigidBody* SimplePhysics::CreateHollowSphere( float Mass, float Radius )
{
	/// 1. Create body
	clRigidBody* Body = Env->Linker->Instantiate( "clRigidBody" );

	Body->MakeZeroParams();

	/// 2. Assign mass properties
	Body->SetMass( Mass );
	Body->SetInertiaTensor( InertiaTensor_HollowSphere( Mass, Radius ) );

	/// 3. Create collision geometry
	LCollisionShape* Geometry = FCollider->CreateCollisionSphere( vec3( 0, 0, 0 ), Radius );
	Geometry->SetOwner( Body );

	Body->SetCollisionShape( Geometry );

	return Body;
}

/// Solid zero-centered cuboid with specified extents
clRigidBody* SimplePhysics::CreateSolidCuboid( float Mass, const LVector3& Extents )
{
	/// 1. Create body
	clRigidBody* Body = Env->Linker->Instantiate( "clRigidBody" );

	Body->MakeZeroParams();

	/// 2. Assign mass properties
	Body->SetMass( Mass );
	Body->SetInertiaTensor( InertiaTensor_Cuboid( Mass, Extents ) );

	/// 3. Create collision geometry
	LCollisionShape* Geometry = FCollider->CreateCollisionBox( LMatrix4::Identity(), Extents );
	Geometry->SetOwner( Body );

	Body->SetCollisionShape( Geometry );

	return Body;
}

/// Solid cylinder with Z-symmetry
clRigidBody* SimplePhysics::CreateCylinderXY( float Mass, float Radius, float Height )
{
	/// 1. Create body
	clRigidBody* Body = Env->Linker->Instantiate( "clRigidBody" );

	Body->MakeZeroParams();

	/// 2. Assign mass properties
	Body->SetMass( Mass );
	Body->SetInertiaTensor( InertiaTensor_CylinderXY( Mass, Radius, Height ) );

	/// 3. Create collision geometry (NOT YET, TODO)
	Body->SetCollisionShape( NULL );

	return Body;
}

/// Render some debug geometry using internal Canvas
void SimplePhysics::DebugRender( const LMatrix4& Proj, const LMatrix4& View )
{
	for ( size_t i = 0 ; i < FBodies.size() ; i++ )
	{
//		DrawRigidBody( Env->Renderer->GetCanvas(), Proj, View, FBodies[i], false, false, true);
	}

	/*
	LVector4 AxisColors[] = { LC_Red, LC_Green, LC_Blue };

	for ( size_t i = 0 ; i < FBodies.size() ; i++ )
	{
	   clRigidBody* B = FBodies[i];

	   /// 1. render orientation as a coord frame (three arrows)
	   LVector3 Pos;
	   mtx4 Rot;

	   DecomposeTransformation( B->GetTransformation(), Pos, Rot );

	   float Radius = 0.4f;

	   for ( int j = 0 ; j < 3 ; j++ )
	   {
	      Env->Renderer->GetCanvas()->Arrow3D( Proj, View, Pos, Pos + Rot[j].ToVector3() * Radius, 0.02f, AxisColors[j], LC_Yellow );
	   }

	   /// 2. render collision shape proxy
	   if (LCollisionShape* Sh = B->GetCollisionShape())
	   {
	      Sh->DebugRender( Proj, View );
	   }
	}
	*/
}

/// Add one more body to simulation pool
int  SimplePhysics::AddBody( clRigidBody* Body )
{
	FBodies.push_back( Body );

	return GetNumBodies() - 1;
}

/// Remove rigid body from simulation
void SimplePhysics::RemoveBody( int idx )
{
	size_t Sz = FBodies.size();

	if ( Sz > 0 ) { FBodies[idx] = FBodies[Sz-1]; }

	FBodies.pop_back();
}

/// Clear the list of bodies (with optional item deletion)
void SimplePhysics::Clear( bool ToDelete )
{
	if ( ToDelete )
	{
		for ( size_t i = 0 ; i < FBodies.size() ; i++ )
		{
			delete FBodies[i];
		}
	}

	FBodies.clear();
}

void SimplePhysics::Update( float DT ) { InternalUpdate( DT ); }

void SimplePhysics::InternalUpdate( float DT )
{
	UpdateOnlyBodies( DT );

	CheckForCollisions();

	ResolveCollisions();
}

void SimplePhysics::UpdateOnlyBodies( float DT )
{
	for ( size_t i = 0 ; i < FBodies.size() ; i++ )
	{
		FBodies[i]->Update( DT );
	}
}

void SimplePhysics::CheckForCollisions()
{
//	FCollider->BruteForceIntersect()
	FCollider->DetectBroadPhaseCollisions();
}

void SimplePhysics::ResolveCollisions()
{
}

/*
 * 09/12/2010
     Log section added
*/
