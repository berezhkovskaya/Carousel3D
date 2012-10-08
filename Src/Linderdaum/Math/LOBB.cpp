#include "LOBB.h"

#include "LAABB.h"

/**
   Move all the "front" vertices/triangles to the "left" side of an array
   and "back" vertices to the "right" side of an array

    [ v1, v2, ... ,  vn, v(n+1), ...]
    -> Left               Right <-

   Run two pointers: from the beginning and from the end.
   If the vertex on the left is from the "right" side of the plane, then find first
   "left" vertex on the right side of an array and swap these vertices.

   Points here come in triples (one triple for each triangle, this is guaranteed by the caller)
*/
static inline int SortArrayByPlane( const LPlane& Plane, int NumPoints, LVector3* Points )
{
	const int PTS_IN_TRIANGLE = 3;

	// Start and stop indices
	int Left  = 0;
	int Right = NumPoints - PTS_IN_TRIANGLE;

	LVector3 Tmp;
	int i;

	while ( Left < Right )
	{
		LClassify LeftClass = Plane.ClassifyPoint( Points[Left] );

		if ( LeftClass == IN_FRONT )
		{
			// ok, just shift
			Left += PTS_IN_TRIANGLE;
		}
		else
		{
			// should swap, check points from the right side
			while ( Left < Right )
			{
				LClassify RightClass = Plane.ClassifyPoint( Points[Right] );

				if ( RightClass == IN_BACK )
				{
					// ok, shift
					Right -= PTS_IN_TRIANGLE;
				}
				else
				{
					// swap with left value
					for ( i = 0 ; i < 3 ; i++ )
					{
						Tmp = Points[Right + i];
						Points[Right + i] = Points[Left + i];
						Points[Left + i] = Tmp;
					}

					// finally, advance the pointers
					Right -= PTS_IN_TRIANGLE;
					Left  += PTS_IN_TRIANGLE;

					// and break from the inner while
					break;
				}
			}
		}
	}

	return ( Left > Right ) ? Right : Left;
}

/// Simple but slow per-point algorithm: move the points on to the Left and Right arrays, according to ClassifyPoint() result
// Remark: this is incorrect, we must build the box by triangles, not by points
static inline void SplitArrayByPlane( const LPlane& Plane, const LArray<LVector3>& Source, LArray<LVector3>& Left, LArray<LVector3>& Right )
{
	for ( LArray<LVector3>::const_iterator v = Source.begin() ; v != Source.end() ; v++ )
	{
		// make a right-unbalanced tree
		( ( Plane.ClassifyPoint( *v ) == IN_FRONT ) ? Left : Right ) .push_back( *v );
	}
}


/// Recursively calculate the tree
Lint32 CalculateOBBTree( int ParentIdx, int Depth, int MaxDepth, LOBBTree* Tree, const LArray<LVector3>& InVertices )
{
	if ( Depth > MaxDepth ) { return -1; }

	if ( InVertices.size() < 4 ) { return -1; }

	// calculate bounding box
	LBox Box( static_cast<int>( InVertices.size() ), InVertices.begin() );

	// add the node
	Lint32 NodeIdx = Tree->AddNode( Box );

	// initialize node
	Tree->FNodes[NodeIdx].FDepth = Depth;

	// select split axis and construct the split plane
	Lint32 AxisIdx = Box.FExtent.GetMaximumComponentIndex();

	Tree->FNodes[NodeIdx].FSplitAxis = AxisIdx;

	LPlane SplitPlane( Box.FCenter, Box.FAxis[AxisIdx] );

	Tree->FNodes[NodeIdx].FSplitPlane = SplitPlane;

	// split InVertices by plane. TODO: avoid dynamic allocation, use some "more explicit" memory management
	LArray<LVector3> Left;
	LArray<LVector3> Right;

	SplitArrayByPlane( SplitPlane, InVertices, Left, Right );

	// create child nodes
	Tree->FNodes[NodeIdx].FLeft  = CalculateOBBTree( NodeIdx, Depth + 1, MaxDepth, Tree, Left );
	Tree->FNodes[NodeIdx].FRight = CalculateOBBTree( NodeIdx, Depth + 1, MaxDepth, Tree, Right );

	// assign parent index to the created node
	Tree->FNodes[NodeIdx].FParent = ParentIdx;

	return NodeIdx;
}

/// Recursively calculate the tree. In-place vertex resoting, no memory allocations
Lint32 CalculateOBBTree_NoCopy( int ParentIdx, int Depth, int MaxDepth, LOBBTree* Tree, int NumVertices, LVector3* InVertices )
{
	if ( Depth > MaxDepth ) { return -1; }

	if ( NumVertices < 4 ) { return -1; }

	// calculate bounding box
	LBox Box;

	if ( Tree->FAligned )
	{
		LAABoundingBox AABB;
		AABB.FromPoints( NumVertices, InVertices );

		Box = AABB.ToBox();
	}
	else
	{
		Box.FromPoints( NumVertices, InVertices );
	}

	// add the node
	Lint32 NodeIdx = Tree->AddNode( Box );

	// initialize node
	Tree->FNodes[NodeIdx].FDepth = Depth;

	// select split axis and construct the split plane
	Lint32 AxisIdx = Box.FExtent.GetMaximumComponentIndex();

	Tree->FNodes[NodeIdx].FSplitAxis = AxisIdx;

	LPlane SplitPlane( Box.FCenter, Box.FAxis[AxisIdx] );

	Tree->FNodes[NodeIdx].FSplitPlane = SplitPlane;

	int MidPoint = SortArrayByPlane( SplitPlane, NumVertices, InVertices );

	// create child nodes
	Tree->FNodes[NodeIdx].FLeft  = CalculateOBBTree_NoCopy( NodeIdx, Depth + 1, MaxDepth, Tree, MidPoint, InVertices );
	Tree->FNodes[NodeIdx].FRight = CalculateOBBTree_NoCopy( NodeIdx, Depth + 1, MaxDepth, Tree, NumVertices - MidPoint, InVertices + MidPoint );

	// assign parent index to the created node
	Tree->FNodes[NodeIdx].FParent = ParentIdx;

	return NodeIdx;
}

void LOBBTree::BuildFromPoints( int NumPts, const LVector3* Points, int MaxDepth )
{
	LArray<LVector3> Source( Points, Points + NumPts );

	CalculateOBBTree( -1, 0, MaxDepth, this, Source );
}

/*
 * 08/01/2011
     It's here
*/
