/**
 * \file LGraph.cpp
 * \brief Oriented geometric graph
 * \version 0.5.91
 * \date 16/06/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "LGraph.h"

#include "Resources/ResourcesManager.h"

#include "Math/LTransform.h"
#include "Math/LGeomUtils.h"

#include "Environment.h"
#include "Scene/Material.h"

#include "Geometry/Geom.h"
#include "Geometry/GeomServ.h"
#include "Geometry/Mesh.h"
#include "Geometry/VertexAttribs.h"

#include "VisualScene/Trajectory.h"

clGraph::clGraph()
{
}

clGraph::~clGraph()
{
}

void clGraph::RemoveEdge( size_t Idx )
{
	FEdge0.EraseNoShift( Idx );
	FEdge1.EraseNoShift( Idx );
}

void clGraph::RemoveVertex( int Idx )
{
	size_t Ptr = 0;
	int LastIdx = static_cast<int>( FEdge0.size() ) - 1;

	while ( Ptr < FEdge0.size() )
	{
		if ( FEdge0[Ptr] == Idx || FEdge1[Ptr] == Idx )
		{
			RemoveEdge( Ptr );
		}

		Ptr++;
	}

	FVertices.EraseNoShift( Idx );

	// remap the edges
	for ( size_t P = 0 ; P < FEdge0.size() ; P++ )
	{
		if ( FEdge0[P] == LastIdx ) { FEdge0[P] = Idx; }

		if ( FEdge1[P] == LastIdx ) { FEdge1[P] = Idx; }
	}
}

void clGraph::ClearGraph()
{
	FLocalOrientations.clear();
	FVertices.clear();
	FEdge0.clear();
	FEdge1.clear();

	FVertexItems.clear();
	FEdgeItems.clear();
	// TODO: decide what to do with vertex/edge items
}

// (Re)generate representing geometry (beads and sticks) for the graph/curve
clGeom* clGraph::CreateGraphGeometry( float BeadRadius, float TubeRadius, float EdgeArrowSize, bool UseCubes, clMaterial* BeadMaterial, clMaterial* StickMaterial, clMaterial* EdgeArrowMaterial )
{
	clMesh* Mesh = Env->Resources->CreateMesh();

	int VtxMaterialID = Mesh->AddMaterial( BeadMaterial->GetMaterialDesc() );

	// 1. points(vertices) are spheres
	for ( size_t i = 0 ; i < FVertices.size() ; i++ )
	{
//    int VtxGeomID = static_cast<int>(i);

		clVertexAttribs* VtxVA;

		LVector3 BoxMin( -BeadRadius, -BeadRadius, -BeadRadius );
		LVector3 BoxMax( +BeadRadius, +BeadRadius, +BeadRadius );

		// name it for picking
		LString VtxName = LString( "Vertex " ) + LStr::ToStr( i );

		if ( UseCubes )
		{

			FIXME( "hack, resources should also create a VA ? Or it is a geomserv ?" )
			// clGeomServ::CreateAxisAlignedBox(BoxMin, BoxMax, false);
			VtxVA = Env->Resources->CreateBox( BoxMin, BoxMax )->GetCurrentMesh()->GetRigid( 0 );
		}
		else
		{
			// it does not work...
//       VtxVA = Env->Resources->CreateSphere();
			VtxVA = Env->Resources->CreateBox( BoxMin, BoxMax )->GetCurrentMesh()->GetRigid( 0 );
		}

		LVector3 VtxPos = FVertices[i];

		LMatrix4 VtxTransform = LMatrix4::GetTranslateMatrix( VtxPos );

		if ( UseCubes )
		{
			LQuaternion VtxOrientation = FLocalOrientations[i];

			// orient the cube at point, if needed
			VtxTransform = ComposeTransformation( VtxPos, LMatrix4( VtxOrientation.ToMatrix3() ) ); // add rotation from LocalOrientation
		}

		Mesh->AddVertexAttribs( VtxName, VtxVA, VtxTransform, -1, VtxMaterialID );
	}

	// 2. render arrows for oriented edges
	int ArrowMaterialID = -1;

	if ( FOriented && EdgeArrowMaterial )
	{
		ArrowMaterialID = Mesh->AddMaterial( EdgeArrowMaterial->GetMaterialDesc() );
	}

// map< pair<int,int> > ProcessedEdges;

	int EdgeMaterialID = Mesh->AddMaterial( StickMaterial->GetMaterialDesc() );

	// 3. render lines from point to point ("tubes"), avoiding duplicate generation for non-oriented edges
	for ( size_t i = 0; i < FEdge0.size() ; i++ )
	{
		int v1 = FEdge0[i];
		int v2 = FEdge1[i];

		LVector3 v1pos = FVertices[v1];
		LVector3 v2pos = FVertices[v2];

		LVector3 Dir = ( v2pos - v1pos );
		LVector3 DirN = Dir.GetNormalized();
		LVector3 EndPointDir = -Dir.Length() * DirN * ( EdgeArrowSize );

		LVector3 Offset = DirN * EdgeArrowSize;

		clVertexAttribs* EdgeVA = clGeomServ::CreateTube( 10, v1pos + Offset, Dir - Offset, TubeRadius, false );

		// name the edge for picking
		LString EdgeName = LString( "Edge " ) + LStr::ToStr( v1 ) + LString( " " ) + LStr::ToStr( v2 );

		LMatrix4 EdgeTransform = LMatrix4::Identity();

		Mesh->AddVertexAttribs( EdgeName, EdgeVA, EdgeTransform, -1, EdgeMaterialID );

		FIXME( "avoid duplicates, use processed edges list" )

		// add arrow, if required
		if ( FOriented )
		{
			clVertexAttribs* ArrowVA = clGeomServ::CreateTube( 10, v2pos - Offset, -EndPointDir, TubeRadius * 2.0f , true );

			LString ArrowName = LString( "Arrow_" ) + EdgeName;

			LMatrix4 ArrowTransform = LMatrix4::Identity();

			Mesh->AddVertexAttribs( ArrowName, ArrowVA, ArrowTransform, -1, ArrowMaterialID );
		}
	}

	clGeom* Geom = Env->Resources->CreateGeom();
	Geom->SetMesh( Mesh );

	return Geom;
}

/// Check if there is a path from Start to Stop
bool clGraph::PathExists( int Start, int Stop )
{
	LArray<int> Tmp;
	return FindPath( Start, Stop, Tmp );
}

void AddUnique( LArray<int>& Dest, int E )
{
	for ( size_t j = 0 ; j < Dest.size() ; j++ )
	{
		if ( Dest[j] == E ) { return; }
	}

	Dest.push_back( E );
}

void clGraph::GetAdjacent( int x, LArray<int>& Adj )
{
	Adj.clear();

	for ( size_t j = 0 ; j < FEdge0.size() ; j++ )
	{
		if ( x == FEdge0[j] && x != FEdge1[j] ) { AddUnique( Adj, FEdge1[j] ); }

		if ( x == FEdge1[j] && x != FEdge0[j] ) { AddUnique( Adj, FEdge0[j] ); }
	}
}

/// Get a sequential list of vertices for Start to Stop, if one exists
bool clGraph::FindPath( int Start, int Stop, LArray<int>& Path )
{
	Path.clear();

	/// Try one: BFS

	LArray<bool> Visited( FVertices.size() );
	bool* v = Visited.begin();

	for ( size_t p = 0 ; p < FVertices.size() ; p++ ) { *v++ = false; }

	/// stack for visited nodes
	LArray<int> Q;

	Path.push_back( Start );
	Path.push_back( Start ); // twic, because first is popped

	Q.push_back( Start );
	Visited[Start] = true;

	LArray<int> Adj;

	while ( !Q.empty() )
	{
		int x = Q.back();

		if ( x == Stop ) { return true; }

		Q.pop_back();
		Path.pop_back();

		GetAdjacent( x, Adj );

		int N = static_cast<int>( Adj.size() );

		for ( int i = 0 ; i < N ; i++ )
		{
			int v = Adj[i];

			if ( !Visited[v] )
			{
				Q.push_back( v );
				Visited[v] = true;
				Path.push_back( v );
			}
		}
	}

	return false;
}

/// Build a 3D-curve for a given path
void clGraph::CurveForPath( const LArray<int>& Path, clRigidBodyTrajectory* Out )
{
	/// We assume that the path exists, so we just add the nodes
	LArray<LVector3> Nodes;
	LArray<LQuaternion> Orientations;

	for ( size_t i = 0 ; i < Path.size() ; i++ )
	{
		Nodes.push_back( FVertices[Path[i]] );

		if ( FLocalOrientations.size() > 0 )
		{
			Orientations.push_back( FLocalOrientations[Path[i]] );
		}
	}

	LQuaternion* OrPtr = FLocalOrientations.empty() ? NULL : Orientations.begin();

	Out->FromArray( static_cast<int>( Nodes.size() ), Nodes.begin(), OrPtr );

	Out->FLoop = false;
}

bool clGraph::PickGraphItem( clGeom* Graph, const LMatrix4& Projection, const LMatrix4& View, float X, float Y, int* VertexNum, int* EdgeStart, int* EdgeStop )
{
	LVector3 SrcPoint;
	LVector3 Dir;
	MouseCoordsToWorldPointAndRay( Projection , View, X, Y, SrcPoint, Dir );

	clMesh* M = Graph->GetCurrentMesh();

	LVector3 isect;
	int Rigid;

	*EdgeStart = *EdgeStop = *VertexNum = -1;

	if ( M->IntersectWithRayIdx( SrcPoint, Dir, isect, Rigid ) )
	{
		LString Name = M->GetRigidName( Rigid );

		if ( LStr::StartsWith( Name, "Vertex" ) )
		{
			*VertexNum = LStr::ToInt( LStr::GetToken( Name, 2 ) );

			return true;
		}
		else if ( LStr::StartsWith( Name, "Edge" ) )
		{
			// edge found
			*EdgeStart = LStr::ToInt( LStr::GetToken( Name, 2 ) );
			*EdgeStop = LStr::ToInt( LStr::GetToken( Name, 3 ) );

			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

// (i,j) to linear index
inline int NodeForPair( int i, int j, int Nx, int Ny )
{
	( void )Ny;

	if ( i < 0   || j < 0 ) { return -1; }

	if ( i >= Nx || j >= Ny ) { return -1; }

	return Nx * i + j;
}

void clGraph::From2DGrid( const LVector3& Center, const LVector3& V1, const LVector3& V2, float SizeX, float SizeY, int Nx, int Ny, int Conn )
{
	ClearGraph();

	LVector3 N = V1.Cross( V2 );

	// common orientation for all nodes
	LQuaternion Orientation;
	Orientation.FromAxisAngle( N, 0.0f );

	float CellX = SizeX / Nx;
	float CellY = SizeY / Ny;

	FOriented = false;

	FLocalOrientations.resize( Nx * Ny );
	FVertices.resize( Nx * Ny );

	for ( int i = 0 ; i < Nx ; i++ )
	{
		for ( int j = 0 ; j < Ny ; j++ )
		{
			int ThisNode = NodeForPair( i, j, Nx, Ny );

			FLocalOrientations[ThisNode] = Orientation;

			FVertices[ThisNode] = Center + V1 * static_cast<float>( i - Nx / 2 ) * CellX + V2 * static_cast<float>( j - Ny / 2 ) * CellY;

			// add 4-connected edges
			int West  = NodeForPair( i - 1, j, Nx, Ny );
			int East  = NodeForPair( i + 1, j, Nx, Ny );
			int North = NodeForPair( i, j - 1, Nx, Ny );
			int South = NodeForPair( i, j - 1, Nx, Ny );

			if ( West > -1 )  { FEdge0.push_back( ThisNode ); FEdge1.push_back( West );  }

			if ( East > -1 )  { FEdge0.push_back( ThisNode ); FEdge1.push_back( East );  }

			if ( North > -1 ) { FEdge0.push_back( ThisNode ); FEdge1.push_back( North ); }

			if ( South > -1 ) { FEdge0.push_back( ThisNode ); FEdge1.push_back( South ); }

			if ( Conn > 4 )
			{
				int NW = NodeForPair( i - 1, j - 1, Nx, Ny );
				int NE = NodeForPair( i + 1, j - 1, Nx, Ny );
				int SW = NodeForPair( i - 1, j + 1, Nx, Ny );
				int SE = NodeForPair( i + 1, j + 1, Nx, Ny );

				if ( NW > -1 ) { FEdge0.push_back( ThisNode ); FEdge1.push_back( NW ); }

				if ( NE > -1 ) { FEdge0.push_back( ThisNode ); FEdge1.push_back( NE ); }

				if ( SW > -1 ) { FEdge0.push_back( ThisNode ); FEdge1.push_back( SW ); }

				if ( SE > -1 ) { FEdge0.push_back( ThisNode ); FEdge1.push_back( SE ); }
			}
		}
	}
}

void clGraph::From2DGrid4( const LVector3& Center, const LVector3& V1, const LVector3& V2, float SizeX, float SizeY, int Nx, int Ny )
{
	From2DGrid ( Center, V1, V2, SizeX, SizeY, Nx, Ny, 4 );
}

void clGraph::From2DGrid8( const LVector3& Center, const LVector3& V1, const LVector3& V2, float SizeX, float SizeY, int Nx, int Ny )
{
	From2DGrid ( Center, V1, V2, SizeX, SizeY, Nx, Ny, 8 );
}


/*
 * 08/10/2010
     Graph from grid generation
     Some graph-theoretic operations defined
     Picking function PickGraphItem()
 * 26/07/2010
     It's here
*/
