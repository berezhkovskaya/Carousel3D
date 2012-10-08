/**
 * \file LGraph.h
 * \brief Oriented geometric graph
 * \version 0.5.91
 * \date 16/06/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __LGraph__h__included__
#define __LGraph__h__included__

#include "Core/iObject.h"

// vectors, matrices, quats
#include "Math/LQuaternion.h"

#include "Utils/LArray.h"

class clMaterial;
class clGeom;

class clRigidBodyTrajectory;

/**
   \brief Oriented geometric graph. Vertices may also contain some iObjects (for each edge and vertex)

   This might be a geometric cell complex with n-cells. However, we can just inherit it to produce the CellComplex by adding the higher-dimensional cells

   (Vertex,Edge0-Edge1) lists and Items are properties
*/
class scriptfinal netexportable clGraph : public iObject
{
public:
	clGraph();
	virtual ~clGraph();

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

#pragma region Graph construction

	/// Create a graph for 4-connected 2D grid
	scriptmethod   void        From2DGrid4( const LVector3& Center, const LVector3& V1, const LVector3& V2, float SizeX, float SizeY, int Nx, int Ny );

	/// Create a graph for 8-connected 2D grid
	scriptmethod   void        From2DGrid8( const LVector3& Center, const LVector3& V1, const LVector3& V2, float SizeX, float SizeY, int Nx, int Ny );

	/// Create 4 or 8-connected 2D grid
	scriptmethod   void        From2DGrid( const LVector3& Center, const LVector3& V1, const LVector3& V2, float SizeX, float SizeY, int Nx, int Ny, int Conn );

#pragma endregion

	/// Create a beads-and-sticks model for the graph. Arrows at the end denote the orientation, if any
	/// UseCubes tells to generate oriented cubes instead of spheres
	scriptmethod   clGeom*     CreateGraphGeometry( float BeadRadius, float TubeRadius, float EdgeArrowSize, bool UseCubes, clMaterial* BeadMaterial, clMaterial* StickMaterial, clMaterial* EdgeArrowMaterial );

#pragma region Item management

	scriptmethod   void        SetEdgeItem( int i, iObject* AItem ) { FEdgeItems[i] = AItem; }
	scriptmethod   iObject*    GetEdgeItem( int i ) const { return FEdgeItems[i]; }

	scriptmethod   void        SetVertexItem( int i, iObject* AItem ) { FVertexItems[i] = AItem; }
	scriptmethod   iObject*    GetVertexItem( int i ) const { return FVertexItems[i]; }

	scriptmethod   void        SetVertex( int i, const LVector3& Vtx ) { FVertices[i] = Vtx; }
	scriptmethod   LVector3    GetVertex( int i ) const { return FVertices[i]; }

	scriptmethod   void        SetEdge0( int i, int ee ) { FEdge0[i] = ee;   }
	scriptmethod   int         GetEdge0( int i ) const   { return FEdge0[i]; }

	scriptmethod   void        SetEdge1( int i, int ee ) { FEdge1[i] = ee;   }
	scriptmethod   int         GetEdge1( int i ) const   { return FEdge1[i]; }

	scriptmethod   void        SetLocalOrientation( int i, const LQuaternion& Q ) { FLocalOrientations[i] = Q; }
	scriptmethod   LQuaternion GetLocalOrientation( int i ) const { return FLocalOrientations[i]; }

	/// Remove every item
	scriptmethod   void        ClearGraph();

	/// Remove specific edge
	scriptmethod   void        RemoveEdge( size_t Idx );

	/// Add an edge
	scriptmethod   void        AddEdge( int From, int To ) { FEdge0.push_back( From ); FEdge1.push_back( To ); }

	/// Remove specific vertex and all adjacent edges
	scriptmethod   void        RemoveVertex( int Idx );

	/// Collect adjacent items
	void GetAdjacent( int x, LArray<int>& Adj );

#pragma endregion

#pragma region Graph-theoretic algorithms

	/// Check if there is a path from Start to Stop
	scriptmethod   bool        PathExists( int Start, int Stop );

	/// Get a sequential list of vertices for Start to Stop, if one exists
	scriptmethod   bool        FindPath( int Start, int Stop, LArray<int>& Path );

#pragma endregion

#pragma region Geometric stuff

	/// Build a 3D trajectory for a given path
	scriptmethod    void        CurveForPath( const LArray<int>& Path, clRigidBodyTrajectory* Out );

	/// Check if something is selected and return selected vertex/edge index
	static noexport bool        PickGraphItem( clGeom* Graph, const LMatrix4& Projection, const LMatrix4& View, float X, float Y, int* VertexNum, int* EdgeStart, int* EdgeStop );

#pragma endregion

public:
	// TODO : LSDC must handle std::vector<> indexed properties automatically (and generate Length property ?) - to avoid writing IndexedGetter/IndexedSetter

	/** Property(Category="Topology and Geometry", Description="Graph vertices",             Name=Vertices,          Type=vec3, IndexType=int, FieldName=FVertices, NetIndexedGetter=GetVertex, NetIndexedSetter=SetVertex) */
	/** Property(Category="Topology and Geometry", Description="Graph vertiex orientations", Name=LocalOrientations, Type=quat, IndexType=int, FieldName=FLocalOrientations, NetIndexedGetter=GetLocalOrientation, NetIndexedSetter=SetLocalOrientation) */
	/** Property(Category="Topology and Geometry", Description="Edge starting points",       Name=Edge0,             Type=int,  IndexType=int, FieldName=FEdge0, NetIndexedGetter=GetEdge0, NetIndexedSetter=SetEdge0) */
	/** Property(Category="Topology and Geometry", Description="Edge starting points",       Name=Edge1,             Type=int,  IndexType=int, FieldName=FEdge1, NetIndexedGetter=GetEdge1, NetIndexedSetter=SetEdge1) */

	/** Property(Category="Topology and Geometry", Description="Oriented flag", Name=Oriented, Type=bool, FieldName=FOriented) */

	/** Property(Category="Content", Description="Items stored at the vertices", Name=VertexItems, Type=iObject, IndexType=int, FieldName=FVertexItems, NetIndexedGetter=GetVertexItem, NetIndexedSetter=SetVertexItem) */
	/** Property(Category="Content", Description="Items stored at the edges",    Name=EdgeItems,   Type=iObject, IndexType=int, FieldName=FEdgeItems,   NetIndexedGetter=GetEdgeItem,   NetIndexedSetter=SetEdgeItem) */

	/// Local orientation at the vertex
	LArray<LQuaternion>      FLocalOrientations;

	/// 0-cells (vertices of the graph)
	LArray<LVector3>         FVertices;

	/// list of "source points"
	LArray<int>              FEdge0;

	/// list of "end points"
	LArray<int>              FEdge1;

	/// Items, stored at the vertices
	LArray<iObject*>         FVertexItems;

	/// Items, stored at the edges
	LArray<iObject*>         FEdgeItems;

	/// Is it oriented or not
	bool                     FOriented;
};

#endif

/*
 * 26/09/2010
     Serialization fixed
*/
