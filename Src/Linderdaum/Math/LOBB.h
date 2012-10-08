#pragma once

#ifndef _LOBBTree_included__H__
#define _LOBBTree_included__H__

#include "Math/LPlane.h"
#include "Math/LBox.h"
#include "Utils/LArray.h"

/**
   \brief Single node of the OBB tree. No pointers, only indices

   size = sizeof(Box) + 5 * sizeof(int32) = 15 * sizeof(float) + 20 = 80 bytes

   Remark: We can use Lint16 as node indices, single byte for the depth
   and a byte for split axis. This can cut down memory requirements,
   but also ruin the memory alignment.

   FSplitPlane takes extra 16 bytes
*/
struct sOBBNode
{
	/// The box for this node
	LBox   FBox;

	/// Stored for debug purposes
	LPlane FSplitPlane;

	/// Index of split axis
	Lint32 FSplitAxis;

	/// Cached depth of the node for complexity analisys
	Lint32 FDepth;

	/// Left child or -1 for NULL
	Lint32 FLeft;

	/// Right child or -1 for NULL
	Lint32 FRight;

	/// Index of the Parent or -1 if this is the root node
	Lint32 FParent;

	sOBBNode() {}
	sOBBNode( const LBox& B ): FBox( B ) {}
};

/**
   Oriented bounding box tree represented as a linear list of nodes
*/
class LOBBTree
{
public:
	LOBBTree() {}

	LOBBTree( int NumPts, const LVector3* Points, int MaxDepth ) { BuildFromPoints( NumPts, Points, MaxDepth ); }

	/// Add and return the index of a newly created node
	inline Lint32 AddNode( const LBox& ABox )
	{
		FNodes.push_back( sOBBNode( ABox ) );
		return ( static_cast<Lint32>( FNodes.size() ) - 1 );
	}

	/// Clear the tree
	inline void Clear() { FNodes.clear(); }

	/// Build the tree from vertex list
	void BuildFromPoints( int NumPts, const LVector3* Points, int MaxDepth );

public:
	/// Is it axis-aligned
	bool FAligned;

	/// Growing list of nodes. Root is the Zero node
	LArray<sOBBNode> FNodes;
};

Lint32 CalculateOBBTree_NoCopy( int ParentIdx, int Depth, int MaxDepth, LOBBTree* Tree, int NumVertices, LVector3* InVertices );

#endif

/*
 * 08/01/2011
     It's here
*/
