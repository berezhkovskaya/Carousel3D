/**
 * \file Trajectory.cpp
 * \brief Trajectory
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Trajectory.h"

#include "LColors.h"
#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"
#include "Environment.h"
#include "Core/Linker.h"

#include "Math/LTransform.h"
#include "Math/LCurve.h"

void clRigidBodyTrajectory::AdjustOrientationsToLookAt( const LVector3& Target, const LVector3& Up )
{
	for ( size_t i = 0 ; i < FNodes.size() ; i++ )
	{
		LMatrix4 LookAtMtx = Math::LookAt( FNodes[i]->FPosition, Target, Up );

		LQuaternion q;
		q.FromMatrix3( LookAtMtx.ExtractMatrix3() );
		FNodes[i]->FOrientation = q;
	}
}

void clRigidBodyTrajectory::CreateOrbitingAroundObject( float Elevation, float OrbitRadius, const LVector3& Target, const LVector3& Up )
{
	LSpline Curve;
	Curve.GenerateCircle( Target + Elevation * Up, Up, OrbitRadius, 128 );
	FromCurve( Curve, false, 128 );

	// finally, adjust transforms
	AdjustOrientationsToLookAt( Target, Up );

	FLoop = true;
}

void clRigidBodyTrajectory::Transform( const LMatrix4& Mtx )
{
	LVector3 Pos;
	LMatrix4 Rot;
	DecomposeTransformation( Mtx, Pos, Rot );
	LQuaternion Q;
	Q.FromMatrix3( Rot.ExtractMatrix3() );

	for ( size_t i = 0 ; i < FNodes.size() ; i++ )
	{
		FNodes[i]->FPosition = Mtx * FNodes[i]->FPosition;
		FNodes[i]->FOrientation = Q * FNodes[i]->FOrientation;
	}
}

/// Construct trajectory from an array
void clRigidBodyTrajectory::FromArray( int NumPoints, LVector3* Points, LQuaternion* Orientations )
{
	guard();

	ClearNodes( true );

	for ( int i = 0 ; i < NumPoints ; i++ )
	{
		clTrajectoryNode* Node = Env->Linker->Instantiate( "clTrajectoryNode" );

		Node->FPosition = Points[i];

		if ( Orientations )
		{
			Node->FOrientation = Orientations[i];
		}

		// evenly-spaced
		Node->FTimestamp = static_cast<float>( i );

		Node->FTrajectory = this;

		FNodes.push_back( Node );
	}

	unguard();
}

void clRigidBodyTrajectory::SortByTime()
{
	clTrajectoryNode* TmpNode;

	int Sz = static_cast<int>( FNodes.size() );

	for ( int i = 0 ; i < Sz ; i++ )
		for ( int j = i + 1 ; j < Sz ; j++ )
			if ( FNodes[j]->FTimestamp < FNodes[i]->FTimestamp )
			{
				TmpNode = FNodes[j];
				FNodes[j] = FNodes[i];
				FNodes[i] = TmpNode;
			}
}

float clRigidBodyTrajectory::GetNextIntervalForNode( int Node )
{
	int Next = Node + 1;

	if ( Next >= static_cast<int>( FNodes.size() ) )
	{
		return 0.0f;
	}

	return FNodes[Next]->FTimestamp - FNodes[Node]->FTimestamp;
}

int clRigidBodyTrajectory::GetNextNode( int ThisNode )
{
	int NextNode = ThisNode + 1;

	if ( NextNode >= static_cast<int>( FNodes.size() ) )
	{
		NextNode = FLoop ? 0 : ThisNode;
	}

	return NextNode;
}

bool clRigidBodyTrajectory::EndLoad()
{
	for ( size_t i = 0 ; i < FNodes.size() ; i++ )
	{
		FNodes[i]->FTrajectory = this;
	}

	SortByTime();

	return true;
}

LVector3 clRigidBodyTrajectory::GetPositionForTime( float T )
{
	int node = FindNodeForT( T );

	if ( node < 0 ) { node = 0; }

	int next = GetNextNode( node );

	float IntervalLen = GetNextIntervalForNode( node );

	float F = 0.0f;

	if ( IntervalLen > 0.0f )
	{
		F = ( T - FNodes[node]->FTimestamp ) / IntervalLen;
	}

	LVector3 Pos;
	Pos.Lerp( FNodes[node]->FPosition, FNodes[next]->FPosition, F );

	return Pos;
}

LQuaternion clRigidBodyTrajectory::GetOrientationForTime( float T )
{
	int node = FindNodeForT( T );

	if ( node < 0 ) { node = 0; }

	int next = GetNextNode( node );

	float IntervalLen = GetNextIntervalForNode( node );

	float F = 0.0f;

	if ( IntervalLen > 0.0f )
	{
		F = ( T - FNodes[node]->FTimestamp ) / IntervalLen;
	}

	LQuaternion Q;
	Q.SLERP( FNodes[node]->FOrientation, FNodes[next]->FOrientation, F );

	return Q;
}

int clRigidBodyTrajectory::FindNodeForT( float T )
{
	return FindNodeFast( T, 0, 0 );
	/*
	   if ( FNodes.empty() ) { return -1; }

	   float RealT = T;

	   float MaxTime = FNodes[FNodes.size()-1]->FTimestamp;

	   if ( T > FNodes[FNodes.size()-1]->FTimestamp )
	   {
	      if ( FLoop )
	      {
	         RealT -= static_cast<float>( static_cast<int>( T / MaxTime ) ) * MaxTime;
	      }
	   }

	   return FindNodeFast( RealT, 0, GetNodeCount() - 1 );
	*/
}

int clRigidBodyTrajectory::FindNodeFast( float T, int from, int to ) const
{
	int N = static_cast<int>( FNodes.size() );

	if ( T <= FNodes[0]->FTimestamp )
	{
		return -1;
	}

	for ( int i = 0 ; i < N - 1 ; i++ )
	{
		if ( T > FNodes[i]->FTimestamp && T <= FNodes[i + 1]->FTimestamp )
		{
			return i;
		}
	}

	return N - 1;

	/*
	   if ( from >= to ) { return to; }

	   float fromT = FNodes[from]->FTimestamp;
	   float toT = FNodes[to]->FTimestamp;

	   int mid = ( from + to ) / 2;

	   float midT = FNodes[mid]->FTimestamp;

	   if ( to == from + 1 )
	   {
	      if ( T < toT && T >= fromT ) { return from; }
	   }

	   if ( T < midT ) { return FindNodeFast( T, from, mid - 1 ); }

	   if ( T > midT ) { return FindNodeFast( T, mid + 1, to ); }

	   return mid;
	*/
}

const float Sz = 0.1f;

void clRigidBodyTrajectory::RenderDebugTime( float T, const LMatrix4& Proj, const LMatrix4& View, bool UseOrientations )
{
	clCanvas* C = Env->Renderer->GetCanvas();
	LVector3 Pos = GetPositionForTime( T );

	C->SetMatrices( Proj, View );

	// 1. draw sphere
	C->Sphere( LSphere( Pos, Sz ), LC_White );

	// 2. draw orientation
	if ( UseOrientations )
	{
		LMatrix3 Axis = GetOrientationForTime( T ).ToMatrix3();

		C->Line3D( Pos, Pos + 2.0f * Sz * Axis[0], LC_Red );
		C->Line3D( Pos, Pos + 2.0f * Sz * Axis[1], LC_Green );
		C->Line3D( Pos, Pos + 2.0f * Sz * Axis[2], LC_Blue );
	}
}

void clRigidBodyTrajectory::RenderDebugNode( size_t i, const LMatrix4& Proj, const LMatrix4& View, bool UseOrientations, bool DrawSegment )
{
	RenderDebugTime( FNodes[i]->FTimestamp, Proj, View, UseOrientations );

	clCanvas* C = Env->Renderer->GetCanvas();
	LVector3 Pos = FNodes[i]->FPosition;

	// line to the next node
	if ( DrawSegment )
	{
		LVector3 Pos2 = FNodes[( i + 1 ) % ( FNodes.size() )]->FPosition;
		C->SetMatrices( Proj, View );
		C->Line3D( Pos, Pos2, LC_Yellow );
	}
}

void clRigidBodyTrajectory::RenderDebug( const LMatrix4& Proj, const LMatrix4& View, bool UseOrientations )
{
	size_t Size = FNodes.size();

	if ( FNodes.size() < 2 ) { return; }

	// draw coord frame at each node
	for ( size_t i = 0 ; i != Size - 1; i++ )
	{
		RenderDebugNode( i, Proj, View, UseOrientations, true );
	}

	// draw last node
	RenderDebugNode( Size - 1, Proj, View, UseOrientations, FLoop );
}

void clRigidBodyTrajectory::FromCurve( const LCurve& CC, bool UseOrientations, int NumPoints )
{
	/// We assume that the path exists, so we just add the nodes
	LArray<LVector3> Nodes;
	LArray<LQuaternion> Orientations;

	float MinT = CC.GetMinimumParameter();
	float MaxT = CC.GetMaximumParameter();

	float dt = ( MaxT - MinT ) / static_cast<float>( NumPoints - 1 );
	float T = 0.0f;

	for ( int i = 0 ; i < NumPoints ; i++ )
	{
		LVector3 Pos = CC.GetPosition( T );
		Nodes.push_back( Pos );

		if ( UseOrientations )
		{
			LQuaternion q;
			q.FromMatrix3( CC.GetCanonicalFrame( T ) );
			Orientations.push_back( q );
		}

		T += dt;
	}

	LQuaternion* OrPtr = UseOrientations ? Orientations.begin() : NULL;

	this->FromArray( static_cast<int>( Nodes.size() ), Nodes.begin(), OrPtr );

	this->FLoop = false;
}


/*
 * 09/12/2010
     Log section added
*/
