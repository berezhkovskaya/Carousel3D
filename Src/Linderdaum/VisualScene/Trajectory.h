/**
 * \file Trajectory.h
 * \brief Trajectory
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __Trajectory__h__included__
#define __Trajectory__h__included__

#include "Core/iObject.h"

#include "Math/LQuaternion.h"

class LCurve;

class clRigidBodyTrajectory;

/// Single trajectory node (selectable item)
class scriptfinal netexportable clTrajectoryNode : public iObject
{
public:
	clTrajectoryNode() {}
	virtual ~clTrajectoryNode() {}

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	/** Property(Name=Position,    Type=vec3,  FieldName=FPosition)    */
	/** Property(Name=Orientation, Type=quat,  FieldName=FOrientation) */
	/** Property(Name=Timestamp,   Type=float, FieldName=FTimestamp)   */
public:
	LVector3    FPosition;
	LQuaternion FOrientation;
	float       FTimestamp;

	// link to parent object
	clRigidBodyTrajectory* FTrajectory;
};

/// Coordinates and orientations tied to time
class netexportable scriptfinal clRigidBodyTrajectory : public iObject
{
public:
	clRigidBodyTrajectory() {}
	virtual ~clRigidBodyTrajectory() { ClearNodes( true ); }

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	/// assign links to child nodes
	virtual bool EndLoad();

	/// Transform each trajectory node by a given matrix
	scriptmethod void Transform( const LMatrix4& Mtx );

	/// Construct trajectory from curve
	void FromCurve( const LCurve& CC, bool UseOrientations, int NumPoints );

	/// Calculate orientations from LookAt matrix. Useful for orbiting around static objects
	void AdjustOrientationsToLookAt( const LVector3& Target, const LVector3& Up );

	/// Create an orbit around object. Orbit center is in (Target + Elevation * Up)
	void CreateOrbitingAroundObject( float Elevation, float OrbitRadius, const LVector3& Target, const LVector3& Up );

	/// Construct trajectory from an array
	void FromArray( int NumPoints, LVector3* Points, LQuaternion* Orientations );

	/// Render trajectory with orientations
	scriptmethod void RenderDebug( const LMatrix4& Proj, const LMatrix4& View, bool UseOrientations );

	/// Find the node in sorted list
	scriptmethod int FindNodeForT( float T );

	/// Find closest node for the given time value
	int FindNodeFast( float T, int from, int to ) const;

	/// Return the next node. If Loop is true, then we also use that
	scriptmethod int   GetNextNode( int ThisNode );

	/// Get the time to next node
	scriptmethod float GetNextIntervalForNode( int Node );

	scriptmethod void  InsertNodeAfter( int After, clTrajectoryNode* Node ) { FNodes.insert( FNodes.begin() + After, Node ); }
	scriptmethod void  DeleteNode( int Idx ) { FNodes.erase( FNodes.begin() + Idx ); }

	/// Sort nodes by 'timestamps'
	scriptmethod void SortByTime();

	scriptmethod int  GetNodeCount() const { return static_cast<int>( FNodes.size() ); }

	scriptmethod clTrajectoryNode* GetNode( int i ) { return FNodes[i]; }
	scriptmethod void              SetNode( int i, clTrajectoryNode* NN ) { FNodes[i] = NN; }

	/// Calculate position for the time
	scriptmethod LVector3 GetPositionForTime( float T );
	/// Calculate orientation for the time
	scriptmethod LQuaternion GetOrientationForTime( float T );

	/** Property(Name=Nodes, Type=clTrajectoryNode, IndexType=int, FieldName=FNodes, NetIndexedGetter=GetNode, NetIndexedSetter=SetNode)   */

	/** Property(Name=Loop,  Type=bool, FieldName=FLoop) */

	/// Clear all trajectory nodes with optional deletion
	scriptmethod void ClearNodes( bool Delete )
	{
		if ( Delete )
		{
			for ( size_t i = 0 ; i < FNodes.size() ; i++ )
			{
				delete FNodes[i];
			}
		}

		FNodes.clear();
	}

public:
	/// Is it an endless loop
	bool FLoop;

	/// List of trajectory nodes
	std::vector<clTrajectoryNode*> FNodes;

	/// Render a single node with a line to next node
	void RenderDebugNode( size_t i, const LMatrix4& Proj, const LMatrix4& View, bool UseOrientations, bool DrawSegment );

	/// Render the interpolated point for the given time
	void RenderDebugTime( float T, const LMatrix4& Proj, const LMatrix4& View, bool UseOrientations );
};

#endif

/*
 * 09/12/2010
     Log section added
*/
