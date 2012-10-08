/**
 * \file VAMender.h
 * \brief Vertex array mender (optimizer)
 * \version 0.5.74
 * \date 20/05/2007
 * \author Sergey Kosarevsky, 2005
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clVAMender_
#define _clVAMender_

#include "Geometry/VertexAttribs.h"
#include "Math/LVector.h"

class clJointsSet;

struct sFaceNormal
{
	LVector3    FFaceNormal;
	LVector3    FVertexNormal[3];
};

class netexportable clVAMender
{
public:
	clVAMender();

	/// Set all internal buffers to initial state to allow mender reuse
	void Reset();

	//
	// clVAMender
	//
	scriptmethod void ReserveVertices( int NumVtx )
	{
		FVertices.resize( NumVtx );
		FWeightIndex.resize( NumVtx );
		FWeightCount.resize( NumVtx );
	}

	scriptmethod void ReserveNormals( int NumNormals ) { FNormals.resize( NumNormals ); }
	scriptmethod void ReserveTangents( int NumTangents ) { FTangents.resize( NumTangents ); }
	scriptmethod void ReserveBinormals( int NumBinorms ) { FBinormals.resize( NumBinorms ); }
	scriptmethod void ReserveFNormals( int NumNormals ) { FFacesNormals.resize( NumNormals ); }
	scriptmethod void ReserveFaces( int NumFaces )
	{
		FFaces.resize( NumFaces );
		FFacesSubMaterial.resize( NumFaces );
		FFacesSmoothingGroup.resize( NumFaces );
	}
	scriptmethod void ReserveTFaces( int NumTFaces ) { FTextureFaces.resize( NumTFaces ); }
	scriptmethod void ReserveTVertices( int NumTVtx ) { FTextureVertices.resize( NumTVtx ); }

	scriptmethod void             AddJointsFrame( clJointsSet* FrameJoints );
	scriptmethod void             EmitWeight( const sWeight& Weight );
	scriptmethod void             EmitVertex( int VertexIndex, const LVector3& V, int WeightIndex, int WeightCount );

	/// Add vector to the normal. If there is no such normal it is added
	scriptmethod void             AddToNormal( int VertexIndex, const LVector3& VN );
	scriptmethod void             EmitNormal( int VertexIndex, const LVector3& VN );
	scriptmethod void             EmitTangent( int VertexIndex, const LVector3& VT );
	scriptmethod void             EmitBinormal( int VertexIndex, const LVector3& VB );

	/// Normalize each vertex normal. Usefull after multiple AddToNormal() calls
	scriptmethod void             Normalize();

	scriptmethod void             EmitFace( int FaceIndex, int A, int B, int C, int SubMaterialIndex, int SmoothingGroup );
	scriptmethod void             EmitFaceNormal( int FaceIndex, const sFaceNormal& VN );
	scriptmethod void             EmitTextureVertex( int TextureVertexIndex, const LVector3& VT );
	scriptmethod void             EmitTextureFace( int FaceIndex, int A, int B, int C );
	scriptmethod void             ReplaceAllSubMaterials( int SubMaterialIndex );
	scriptmethod clVertexAttribs* CreateVertexAttribs( int ForSubMaterial );

	scriptmethod LString          GetLastError() const;
private:
	LString FLastError;
private:
	// methods
	void    CalculateTBN( clVertexAttribs* CurrentVA, int ForSubMaterial );
	void    BuildSGNMap( int ForSubMaterial );
private:
	// types
	typedef std::vector<LVector3>    clVectorsList;
	typedef std::vector<int>         clIntsList;
private:
	// fields
	clVectorsList       FVertices;
	std::vector<int>    FWeightIndex;
	std::vector<int>    FWeightCount;
	std::vector<sWeight>FWeights;
	std::vector<clJointsSet*>     FJointsFrames;
	clVectorsList       FNormals;
	clVectorsList       FTangents;
	clVectorsList       FBinormals;
	clVectorsList       FTextureVertices;
	clVectorsList       FTextureFaces;
	clVectorsList       FFaces;
	std::vector<sFaceNormal>    FFacesNormals;
	std::vector<int>    FFacesSubMaterial;
	std::vector<int>    FFacesSmoothingGroup;

#pragma region Smoothing groups management
	/// indexed by output vertex
	std::vector<int>           FVerticesSmoothingGroup;
	/// smooth group normals
	std::vector<clVectorsList> FSGN;
	/// indexed by input vertex
	std::vector<clIntsList>    FSplitMap;
	/// indexed by input vertex index
	std::vector<int> FSplitVertexToSGVertex_SGNum;
	/// indexed by input vertex index
	std::vector<int> FSplitVertexToSGVertex_VIndex;
#pragma endregion
};

#endif

/*
 * 20/05/2007
     EmitKeyframe()
 * 06/03/2005
     It's here
*/
