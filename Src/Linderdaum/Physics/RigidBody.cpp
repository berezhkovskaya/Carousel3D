/**
 * \file RigidBody.cpp
 * \brief Physical rigid body
 * \version 0.5.91
 * \date 12/06/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "RigidBody.h"

#include "Math/LTransform.h"
#include "Math/LGeomUtils.h"

#include "Environment.h"
#include "Core/Linker.h"

LMatrix3 InertiaTensor_SolidSphere( float Mass, float Radius )
{
	float Inertia = ( 2.0f / 5.0f ) * Mass * Radius * Radius;
	return LMatrix3( vec3( Inertia, 0.0f, 0.0f ), vec3( 0.0f, Inertia, 0.0f ), vec3( 0.0f, 0.0f, Inertia ) );
}

LMatrix3 InertiaTensor_HollowSphere( float Mass, float Radius )
{
	float Inertia = ( 2.0f / 3.0f ) * Mass * Radius * Radius;
	return LMatrix3( vec3( Inertia, 0.0f, 0.0f ), vec3( 0.0f, Inertia, 0.0f ), vec3( 0.0f, 0.0f, Inertia ) );
}

LMatrix3 InertiaTensor_Cuboid( float Mass, const LVector3& Extents )
{
	float Denom = Mass / 12.0f;
	float Ix = Extents[1] * Extents[1] + Extents[2] * Extents[2];
	float Iy = Extents[0] * Extents[0] + Extents[2] * Extents[2];
	float Iz = Extents[1] * Extents[1] + Extents[0] * Extents[0];

	return LMatrix3( vec3( Denom * Ix, 0.0f, 0.0f ), vec3( 0.0f, Denom * Iy, 0.0f ), vec3( 0.0f, 0.0f, Denom * Iz ) );
}

LMatrix3 InertiaTensor_CylinderXY( float Mass, float Radius, float Height )
{
	float R2 = Radius * Radius;
	float H2 = Height * Height;

	float Ixy = ( Mass / 12.0f ) * ( 3.0f * R2 + H2 );

	return LMatrix3( vec3( Ixy, 0.0f, 0.0f ), vec3( 0.0f, Ixy, 0.0f ), vec3( 0.0f, 0.0f, Mass * R2 / 2.0f ) );
}

clCoordinateFrame::clCoordinateFrame() : FTransform( LMatrix4::Identity() ),
	FMtxOrientation( LMatrix4::Identity() ),
	FPosition( 0, 0, 0 ), FOrientation( 0, 0, 0, 1 )

{
}

clCoordinateFrame::~clCoordinateFrame()
{
}

LMatrix4 clCoordinateFrame::GetMtxOrientation() const
{
	return FMtxOrientation;
}

LMatrix3 clCoordinateFrame::GetMtxOrientation3() const
{
	return FMtxOrientation.ToMatrix3();
}

/*
void clCoordinateFrame::SetQuatOrientation(const LQuaternion& Q)
{
   SetMtxOrientation(Q.ToMatrix3());
}
*/
void clCoordinateFrame::SetMtxOrientation( const LMatrix3& Rot )
{
	for ( int i = 0; i < 3 ; i++ )
	{
		for ( int j = 0 ; j < 3 ; j++ )
		{
			FMtxOrientation[i][j] = Rot[i][j];
		}
	}
}

LQuaternion clCoordinateFrame::GetQuatOrientation() const
{
	/*
	   LQuaternion q;
	   q.FromMatrix3(FTransform.ExtractMatrix3());

	   return q;
	*/
	return FOrientation;
}

LMatrix4 clCoordinateFrame::GetTransformation() const
{
	return FTransform;
}

void clCoordinateFrame::SetTransformation( const LMatrix4& Mtx )
{
	FTransform = Mtx;

	DecomposeTransformation( FTransform, FPosition, FMtxOrientation );

	FOrientation.FromMatrix3( FMtxOrientation.ToMatrix3() );
}

void clCoordinateFrame::SetEulerAnglesV( LEulerAngleSystem Sys, const LVector3& Angles )
{
	SetEulerAngles( Sys, Angles.X, Angles.Y, Angles.Z );
}

void clCoordinateFrame::SetEulerAngles( LEulerAngleSystem Sys, float T1, float T2, float T3 )
{
	AnglesToMatrix( Sys, FMtxOrientation, T1, T2, T3 );

	SetTransformation( ComposeTransformation( FPosition, FMtxOrientation ) );
}

LVector3 clCoordinateFrame::GetEulerAnglesV( LEulerAngleSystem Sys ) const
{
	LVector3 Res;

	GetEulerAngles( Sys, Res.X, Res.Y, Res.Z );

	return Res;
}

void clCoordinateFrame::GetEulerAngles( LEulerAngleSystem Sys, float& T1, float& T2, float& T3 ) const
{
	MatrixToAngles( Sys, FMtxOrientation.ExtractMatrix3(), T1, T2, T3 );
}

void clCoordinateFrame::SetPosition( const LVector3& Pos )
{
	/// alter current transformation matrix
	FTransform[3] = LVector4( Pos );
	FTransform[3][3] = 1.0f;

	SetTransformation( FTransform );
}

LVector3 clCoordinateFrame::GetPosition() const
{
	return FTransform[3].ToVector3();
}


///////////////////////////////////////

clRigidBody::clRigidBody() : FCollisionShape( NULL )
{
}

clRigidBody::~clRigidBody()
{
}

/// Apply an impulse caused by an impact with some static body
void clRigidBody::ResolveImpact_Single( const LVector3& Cp1, const LVector3& N2 )
{
	// CP1 - contact point in body reference frame

	LMatrix3 Or = FMtxOrientation.ExtractMatrix3();
	LMatrix3 OrInv = Or.GetTransposed();

	LMatrix3 tmpInertiaInv = Or * FInvInertiaTensor * OrInv;

	float  ImpulseNumerator   = -( 1.0f/*+GlobalRestitutionFactor*/ ) * ( GetVelocityAtPoint( Cp1 ).Dot( N2 ) );
	LVector3 VV = tmpInertiaInv * ( Cp1.Cross( N2 ) );
	float ImpulseDenominator = ( 1.0f / FMass ) + ( VV.Cross( Cp1 ).Dot( N2 ) );

	LVector3 Impulse = ( ImpulseNumerator / ImpulseDenominator ) * N2;

	// compute the instant velocity update
	FVelocity += Impulse / FMass;
	FAngularVelocity += tmpInertiaInv * ( Cp1.Cross( Impulse ) );
}

/// Apply impulses to the bodies involved in a fast non-resting contact
void clRigidBody::ResolveImpact_Double( const LVector3& Cp1, const LVector3& Cp2, const LVector3& N1, const LVector3& N2, clRigidBody* Body1, clRigidBody* Body2 )
{
	LMatrix3 Or1 = Body1->FMtxOrientation.ExtractMatrix3();
	LMatrix3 Or2 = Body2->FMtxOrientation.ExtractMatrix3();
	LMatrix3 Or1Inv = Or1.GetTransposed();
	LMatrix3 Or2Inv = Or2.GetTransposed();

	LMatrix3 tmpInertiaInv1 = Or1 * Body1->FInvInertiaTensor * Or1Inv;
	LMatrix3 tmpInertiaInv2 = Or2 * Body2->FInvInertiaTensor * Or2Inv;

	LVector3 VV1 = tmpInertiaInv1 * ( Cp1.Cross( N2 ) );
	LVector3 VV2 = tmpInertiaInv2 * ( Cp2.Cross( N1 ) );

	float T1 = 1.0f / Body1->FMass;
	float T2 = VV1.Cross( Cp1 ).Dot( N2 );
	float T3 = 1.0f / Body2->FMass;
	float T4 = VV2.Cross( Cp2 ).Dot( N1 );

	LVector3 RelativeVelocity = Body2->GetVelocityAtPoint( Cp2 ) - Body1->GetVelocityAtPoint( Cp1 );

	float ImpulseNumerator = RelativeVelocity.Dot( N2 ); // or n1 ?
	float ImpulseDenominator = ( T1 + T2 + T3 + T4 );

	LVector3 Impulse = ( ImpulseNumerator / ImpulseDenominator ) * N2;

	Body1->FVelocity += Impulse * T1;
	Body2->FVelocity += Impulse * T3;

	Body1->FAngularVelocity += tmpInertiaInv1 * ( Cp1.Cross( Impulse ) );
	Body2->FAngularVelocity += tmpInertiaInv2 * ( Cp2.Cross( Impulse ) );
}

/// Euler's formula
LVector3 clRigidBody::GetVelocityAtPoint( const LVector3& p ) const
{
	return FVelocity + FAngularVelocity.Cross( p );
}

void clRigidBody::ClearAccumulators()
{
	FForceAccumulator  = vec3( 0, 0, 0 );
	FTorqueAccumulator = vec3( 0, 0, 0 );
}

void clRigidBody::ApplyPureTorque( const LVector3& T )
{
	FTorqueAccumulator += T;
}

void clRigidBody::ApplyPureForce( const LVector3& F )
{
	FForceAccumulator += F;
}

void clRigidBody::ApplyForce( const LVector3& F, const LVector3& P )
{
	FForceAccumulator += F;
	FTorqueAccumulator += F.Cross( P );
}

void clRigidBody::RestrictDynamicsToPlane( const LPlane& P )
{
	// position
	SetPosition( P.ProjectPoint( GetPosition() ) );

	// orientation remains the same

	// force is projected
	FForceAccumulator = P.ProjectPoint( FForceAccumulator );

	// torque is aligned with the plane's normal
	FTorqueAccumulator = P.ProjectToNormal( FTorqueAccumulator );

	// linear velocity is projected to the plane
	FVelocity = P.ProjectPoint( FVelocity );

	// angular velocity is aligned with the plane's normal
	FAngularVelocity = P.ProjectToNormal( FAngularVelocity );
}

void clRigidBody::MakeEulerStep( float dt )
{
	if ( FMass <= Math::EPSILON ) { return; }

	FVelocity += FForceAccumulator * ( dt / FMass );

	LVector3 InertiaTerm = ( FInvInertiaTensor * FAngularVelocity ).Cross( ( FInertiaTensor * FAngularVelocity ) );
	LVector3 TorqueTerm = FInvInertiaTensor * FTorqueAccumulator;

	FAngularVelocity += ( InertiaTerm + TorqueTerm ) * dt;
}

void clRigidBody::Update( float dt )
{
	// update position and orientation using current velocity values

	// 1. Translate
	FPosition += FVelocity * dt;

	// 2. Rotate in local reference frame
	LMatrix3 Or = GetMtxOrientation().ExtractMatrix3();

	float theta = FAngularVelocity.Length();
	LVector3 Dir = FAngularVelocity / theta;
	theta = theta * dt;

	LQuaternion dq( Dir * sin( theta / 2.0f ), cos( theta / 2.0f ) );

	LQuaternion NewQ = dq * FOrientation /*GetQuatOrientation()*/;

//   SetMtxOrientation(NewQ.ToMatrix3());
	FOrientation = NewQ;

	/*
	   LMatrix3 OmegaMtx = VectorStar(FAngularVelocity);
	   LMatrix3 OrDelta = OmegaMtx * Or;

	   Or = Or + OrDelta;

	   // 3, Fix orientation (due to the rounding errors)
	   Or.Orthonormalize();

	   SetMtxOrientation(Or);
	   */
//   SetQuatOrientation(NewQ);

	FTransform = ComposeTransformation( FPosition, LMatrix4( FOrientation.ToMatrix3() ) );
}

void clRigidBody::SetInertiaTensor( const LMatrix3& Inertia )
{
	FInertiaTensor = Inertia;
	FInvInertiaTensor = Inertia.GetInversed();
}

clRigidBody* clRigidBody::Clone()
{
	clRigidBody* New = Env->Linker->Instantiate( "clRigidBody" );

	New->SetInertiaTensor( FInertiaTensor );
	New->FMass = FMass;

	New->SetTransformation( FTransform );

	New->FVelocity = FVelocity;
	New->FAngularVelocity = FAngularVelocity;

	New->FForceAccumulator = FForceAccumulator;
	New->FTorqueAccumulator = FTorqueAccumulator;

	return New;
}

void clRigidBody::MakeZeroParams()
{
	SetTransformation( LMatrix4::Identity() );

	FMass = 1.0f;
	SetInertiaTensor( LMatrix3::Identity() );

	FVelocity = vec3( 0, 0, 0 );
	FAngularVelocity = vec3( 0, 0, 0 );

	ClearAccumulators();
}

LVector3 clRigidBody::GetAccelerationAtPoint( const LVector3& p ) const
{
	TODO( "implement" )
	return LVector3( 0, 0, 0 );
}

LVector3 clRigidBody::GetInstantRotationAxis() const
{
	LVector3 OmegaA, OmegaB;
	BuildComplementaryBasis( FAngularVelocity, OmegaA, OmegaB );

	float A = FVelocity.Dot( OmegaB ) * OmegaA.SqrLength();
	float B = FVelocity.Dot( OmegaA ) * OmegaB.SqrLength();

	LVector3 Res = A * OmegaA + B * OmegaB;

	return Res;
}

/*
 * 12/06/2010
     Log section added
*/
