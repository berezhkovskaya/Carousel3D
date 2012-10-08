/**
 * \file MD5.h
 * \brief MD5 meshes loader
 * \version 0.5.84
 * \date 27/11/2007
 * \author Sergey Kosarevsky, 2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _MD5Loader_
#define _MD5Loader_

#include "Geometry/Mesh.h"
#include "Geometry/VAMender.h"
#include "Renderer/iVertexArray.h"
#include "Geometry/Joints.h"
#include "Math/LQuaternion.h"
#include "Math/LAABB.h"

/// MD5 vertex description
struct sMD5Vertex
{
	LVector2    FTexCoord;
	int         FWeightIndex;
	int         FWeightCount;
};

/// MD5 indexed triangle
struct sMD5Triangle
{
	int   FIndex[3];
};

/// MD5 generic item
struct sMD5HierarchyItem
{
	LString    FName;
	int        FParent;
	int        FFlags;
	int        FStartIndex;
};

/// MD5 skeleton joint
struct sMD5BaseFrameJoint
{
	LVector3       FPos;
	LQuaternion    FOrient;
};

/// MD5 mesh (collection of vertices, triangles, weights and points)
struct sMD5Mesh
{
	LString                      FMaterialName;
	std::vector<sMD5Vertex>      FVertices;
	std::vector<sMD5Triangle>    FTris;
	std::vector<sWeight>         FWeights;
	std::vector<LVector3>        FPoints;
	int                          FMaxJointsPerVertex;
	//
	void    SetVertex( int Index, const LVector2& Tex, int WeightIndex, int WeightCount )
	{
		FVertices[Index].FTexCoord    = Tex;
		FVertices[Index].FWeightIndex = WeightIndex;
		FVertices[Index].FWeightCount = WeightCount;
	}
	void    SetTri( int Index, int V1, int V2, int V3 )
	{
		FTris[Index].FIndex[0] = V1;
		FTris[Index].FIndex[1] = V2;
		FTris[Index].FIndex[2] = V3;
	}
	void    SetWeight( int Index, int Joint, float Bias, const LVector3& Weight )
	{
		FWeights[Index].FJointIndex = Joint;
		FWeights[Index].FPosWeight = LVector4( Weight, Bias );
	}
	void    CalcPoints( clJointsSet* Joints );
	void    ComputeMaxJointsPerVertex();
};

/// Loader of MD5 animated meshes
class scriptfinal clMD5Loader: public iMeshLoader
{
public:
	clMD5Loader();
	//
	// iMeshLoader interface
	//
	virtual bool         LoadMesh( iIStream* IStream, clMesh* Resource );
private:
	iIStream*    FStream;
	// MD5mesh params
	clJointsSet  FJoints;
	std::vector<sMD5Mesh>   FMeshes;
	// MD5anim params
	int          FNumFrames;
	int          FNumAnimJoints;
	int          FFrameRate;
	int          FNumAnimatedComponents;
	std::vector<LAABoundingBox>        FAnimBBoxes;
	std::vector<sMD5HierarchyItem>     FHierarchy;
	std::vector<sMD5BaseFrameJoint>    FBaseFrame;
	std::vector<float*>                FFrames;
	std::vector<clJointsSet>           FFrameJoints;
private:
	void     ResetJoints( clJointsSet* Joints );
	void     SetFrame( clJointsSet* Joints, int N );
private:
	void     MD5_ReadHeader();
	void     MD5_ReadJoints();
	sMD5Mesh MD5_ReadMesh();
	void     MD5_ReadAnimHeader();
	void     MD5_ReadAnimHierarchy();
	void     MD5_ReadAnimBounds();
	void     MD5_ReadAnimBaseFrame();
	void     MD5_ReadAnimFrame( int no );

	/// New method for animation loading: loads a number of .md5anim files and master .anim file
	void     MD5_ReadAnimations( const LString& BaseFileName );
};


#endif

/*
 * 27/11/2007
     It's here
*/
