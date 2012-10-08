/**
 * \file VAMender.cpp
 * \brief Vertex array mender (optimizer)
 * \version 0.5.88
 * \date 26/03/2008
 * \author Sergey Kosarevsky, 2005-2008
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "VAMender.h"
#include "Engine.h"

#include "Math/LMath.h"
#include "Math/LVector.h"
#include "Math/LMathStrings.h"

TODO( "implement average normal calculation at the cut-vertex of several faces" )

clVAMender::clVAMender() : FLastError( "" )
{
	const int InitialReserve = 100000;

	FVertices.reserve( InitialReserve );
	FWeightIndex.reserve( InitialReserve );
	FWeightCount.reserve( InitialReserve );
	FWeights.reserve( InitialReserve );
	FJointsFrames.reserve( InitialReserve );
	FNormals.reserve( InitialReserve );
	FTangents.reserve( InitialReserve );
	FBinormals.reserve( InitialReserve );
	FTextureVertices.reserve( InitialReserve );
	FTextureFaces.reserve( InitialReserve );
	FFaces.reserve( InitialReserve );
	FFacesNormals.reserve( InitialReserve );
	FFacesSubMaterial.reserve( InitialReserve );
	FFacesSmoothingGroup.reserve( InitialReserve );
}

void clVAMender::Reset()
{
	FVertices.resize( 0 );
	FWeightIndex.resize( 0 );
	FWeightCount.resize( 0 );
	FWeights.resize( 0 );
	FJointsFrames.resize( 0 );
	FNormals.resize( 0 );
	FTangents.resize( 0 );
	FBinormals.resize( 0 );
	FTextureVertices.resize( 0 );
	FTextureFaces.resize( 0 );
	FFaces.resize( 0 );
	FFacesNormals.resize( 0 );
	FFacesSubMaterial.resize( 0 );
	FFacesSmoothingGroup.resize( 0 );
}


LString clVAMender::GetLastError() const
{
	return FLastError;
}

void clVAMender::AddJointsFrame( clJointsSet* FrameJoints )
{
	FJointsFrames.push_back( FrameJoints );
}

void clVAMender::EmitWeight( const sWeight& Weight )
{
	FWeights.push_back( Weight );
}

void clVAMender::EmitVertex( int VertexIndex, const LVector3& V, int WeightIndex, int WeightCount )
{
	size_t NewSize = static_cast<size_t>( VertexIndex ) + 1;

	if ( NewSize > FVertices.size() )
	{
//		FVertices.reserve( NewSize * 2 );
//		FWeightIndex.reserve( NewSize * 2 );
//		FWeightCount.reserve( NewSize *2 );

		FVertices.resize( NewSize );
		FWeightIndex.resize( NewSize );
		FWeightCount.resize( NewSize );
	}

	FVertices[VertexIndex] = V;
	FWeightIndex[VertexIndex] = WeightIndex;
	FWeightCount[VertexIndex] = WeightCount;
}

void clVAMender::AddToNormal( int VertexIndex, const LVector3& VN )
{
	size_t NewSize = static_cast<size_t>( VertexIndex ) + 1;

	if ( NewSize > FNormals.size() )
	{
		EmitNormal( VertexIndex, VN );
	}
	else
	{
		FNormals[VertexIndex] += VN;
	}
}

void clVAMender::Normalize()
{
	size_t Num = FNormals.size();

	for ( size_t i = 0 ; i < Num ; i++ )
	{
		FNormals[i].Normalize();
	}
}

void clVAMender::EmitNormal( int VertexIndex, const LVector3& VN )
{
	size_t NewSize = static_cast<size_t>( VertexIndex ) + 1;

	if ( NewSize > FNormals.size() )
	{
//		FNormals.reserve( NewSize * 2);
		FNormals.resize( NewSize );
	}

	FNormals[VertexIndex] = VN;
}

void clVAMender::EmitTangent( int VertexIndex, const LVector3& VT )
{
	size_t NewSize = static_cast<size_t>( VertexIndex ) + 1;

	if ( NewSize > FTangents.size() )
	{
		FTangents.resize( NewSize );
	}

	FTangents[VertexIndex] = VT;

}

void clVAMender::EmitBinormal( int VertexIndex, const LVector3& VB )
{
	size_t NewSize = static_cast<size_t>( VertexIndex ) + 1;

	if ( NewSize > FBinormals.size() )
	{
		FBinormals.resize( NewSize );
	}

	FBinormals[VertexIndex] = VB;
}


void clVAMender::EmitFace( int FaceIndex, int A, int B, int C, int SubMaterialIndex, int SmoothingGroup )
{
	size_t NewSize = static_cast<size_t>( FaceIndex ) + 1;

	if ( NewSize > FFaces.size() )
	{
//		FFaces.reserve( NewSize * 2 );
//		FFacesSubMaterial.reserve( NewSize * 2 );
//		FFacesSmoothingGroup.reserve( NewSize * 2 );

		FFaces.resize( NewSize );
		FFacesSubMaterial.resize( NewSize );
		FFacesSmoothingGroup.resize( NewSize );
	}

	FFaces[FaceIndex] = LVector3( static_cast<float>( A ),
	                              static_cast<float>( B ),
	                              static_cast<float>( C ) );

	FFacesSubMaterial[FaceIndex] = SubMaterialIndex;
	FFacesSmoothingGroup[FaceIndex] = SmoothingGroup + 1;
}

void clVAMender::EmitFaceNormal( int FaceIndex, const sFaceNormal& VN )
{
	size_t NewSize = static_cast<size_t>( FaceIndex ) + 1;

	if ( NewSize > FFacesNormals.size() )
	{
		FFacesNormals.resize( NewSize );
	}

	FFacesNormals[ FaceIndex ] = VN;
}

void clVAMender::EmitTextureVertex( int TextureVertexIndex, const LVector3& VT )
{
	size_t NewSize = static_cast<size_t>( TextureVertexIndex ) + 1;

	if ( NewSize > FTextureVertices.size() )
	{
//		FTextureVertices.reserve( NewSize * 2 );
		FTextureVertices.resize( NewSize );
	}

	FTextureVertices[TextureVertexIndex] = VT;
}

void clVAMender::EmitTextureFace( int FaceIndex, int A, int B, int C )
{
	size_t NewSize = static_cast<size_t>( FaceIndex ) + 1;

	if ( NewSize > FTextureFaces.size() )
	{
//		FTextureFaces.reserve( NewSize * 2 );
		FTextureFaces.resize( NewSize );
	}

	FTextureFaces[FaceIndex] = LVector3( static_cast<float>( A ),
	                                     static_cast<float>( B ),
	                                     static_cast<float>( C ) );
}

void clVAMender::ReplaceAllSubMaterials( int SubMaterialIndex )
{
	for ( size_t i = 0; i != FFaces.size(); ++i )
	{
		FFacesSubMaterial[i] = SubMaterialIndex;
	}
}

void clVAMender::CalculateTBN( clVertexAttribs* CurrentVA, int ForSubMaterial )
{
	if ( !FTextureFaces.size() )
	{
		return;
	}

	// split vertices
	//LVector3* Tangents  = CurrentVA->GetTangents();
	//LVector3* Binormals = CurrentVA->GetBinormals();
	bool CalcNormals = FNormals.empty();

	int Idx = 0;

	for ( size_t i = 0; i != FFaces.size(); ++i )
	{
		if ( FFacesSubMaterial[i] != ForSubMaterial )
		{
			continue;
		}

		int VertexIndex1  = static_cast<int>( FFaces[Idx][0] );
		int VertexIndex2  = static_cast<int>( FFaces[Idx][1] );
		int VertexIndex3  = static_cast<int>( FFaces[Idx][2] );
		int OutputVertexIndex1 = Idx * 3 + 0;
		int OutputVertexIndex2 = Idx * 3 + 1;
		int OutputVertexIndex3 = Idx * 3 + 2;

		int CurrentFaceSG = FFacesSmoothingGroup[ Idx ];

		LVector3 p1 = FVertices[VertexIndex1];
		LVector3 p2 = FVertices[VertexIndex2];
		LVector3 p3 = FVertices[VertexIndex3];

		int TextureIndex1 = static_cast<int>( FTextureFaces[Idx][0] );
		int TextureIndex2 = static_cast<int>( FTextureFaces[Idx][1] );
		int TextureIndex3 = static_cast<int>( FTextureFaces[Idx][2] );

		float u1 = FTextureVertices[TextureIndex1].X;
		float v1 = FTextureVertices[TextureIndex1].Y;
		float u2 = FTextureVertices[TextureIndex2].X;
		float v2 = FTextureVertices[TextureIndex2].Y;
		float u3 = FTextureVertices[TextureIndex3].X;
		float v3 = FTextureVertices[TextureIndex3].Y;

		float Tdenom = ( u2 - u1 ) * ( v3 - v1 ) - ( v2 - v1 ) * ( u3 - u1 );
		float Bdenom = ( v2 - v1 ) * ( u3 - u1 ) - ( u2 - u1 ) * ( v3 - v1 );

		LVector3 LocalBinormal = ( ( u3 - u1 ) * ( p2 - p1 ) - ( u2 - u1 ) * ( p3 - p1 ) ) / Bdenom;
		LVector3 LocalTangent  = ( ( v3 - v1 ) * ( p2 - p1 ) - ( v2 - v1 ) * ( p3 - p1 ) ) / Tdenom;

		if ( CalcNormals )
		{
//         LVector3* Normals = CurrentVA->GetNormals();

			LVector3 LocalNormal = ( p3 - p1 ).Cross( p2 - p1 );
//         LVector3 LocalNormal = LocalTangent.Cross(LocalBinormal);

			LocalNormal.Normalize();

			int Index;

			Index = FSplitVertexToSGVertex_VIndex[ OutputVertexIndex1 ];
			FSGN[ CurrentFaceSG ][ Index ] += LocalNormal;

			Index = FSplitVertexToSGVertex_VIndex[ OutputVertexIndex2 ];
			FSGN[ CurrentFaceSG ][ Index ] += LocalNormal;

			Index = FSplitVertexToSGVertex_VIndex[ OutputVertexIndex3 ];
			FSGN[ CurrentFaceSG ][ Index ] += LocalNormal;

//         Normals[OutputVertexIndex1] = LocalNormal;
//        Normals[OutputVertexIndex2] = LocalNormal;
//         Normals[OutputVertexIndex3] = LocalNormal;
		}

		Idx++;
	}

	LVector3* Normals = CurrentVA->FNormals.GetPtr();

	for ( size_t j = 0 ; j != CurrentVA->FVertices.size(); j++ )
	{
		if ( CalcNormals )
		{
			int SG    = FSplitVertexToSGVertex_SGNum[ j ];
			int Index = FSplitVertexToSGVertex_VIndex[ j ];

			Normals[j] = FSGN[ SG ][ Index ];
			Normals[j].Normalize();

//         Env->Logger->Log( L_DEBUG, "Dumping normal: Vertex: " +LStr::ToStr(j) + "(SG:"+LStr::ToStr(SG)+" Idx:"+LStr::ToStr(Index)+") Normal: " + Vec3ToString( Normals[j], ',' ) );
		}
	}
}

void clVAMender::BuildSGNMap( int ForSubMaterial )
{
	std::vector<int> AlreadyAddedToSG;      // indexed by input vertex index

	size_t MaxNumVtx = 3 * FFaces.size();

	FSplitVertexToSGVertex_SGNum.clear();
	FSplitVertexToSGVertex_SGNum.resize( MaxNumVtx  );
	FSplitVertexToSGVertex_VIndex.clear();
	FSplitVertexToSGVertex_VIndex.resize( MaxNumVtx );
	AlreadyAddedToSG.clear();
	AlreadyAddedToSG.resize( MaxNumVtx );

	// clear flags
	for ( size_t i = 0; i != MaxNumVtx ; ++i )
	{
		AlreadyAddedToSG[i] = 0;
	}

	FSGN.clear();
	FSplitMap.clear();

	int MaxSmoothingGroupIdx = 0;

	// prepare SGN data structure
	for ( size_t i = 0; i != FFaces.size(); ++i )
	{
		//if ( FFacesSubMaterial[i] != ForSubMaterial ) continue;

		if ( MaxSmoothingGroupIdx < FFacesSmoothingGroup[i] )
		{
			MaxSmoothingGroupIdx = FFacesSmoothingGroup[i];
		}

		while ( FFacesSmoothingGroup[i] >= static_cast<int>( FSGN.size() ) )
		{
			clVectorsList NormalsList;
			clIntsList    IntsList;

			IntsList.resize( FVertices.size() );

			FSGN.push_back( NormalsList );
			FSplitMap.push_back( IntsList );
		}
	}

//   Env->Logger->Log( L_DEBUG, "MaxSmoothingGroupIdx = "+LStr::ToStr(MaxSmoothingGroupIdx) );

	// create mapping for every SG
	int Idx = 0;

	for ( size_t i = 0; i != FFaces.size(); ++i )
	{
		// if ( FFacesSubMaterial[i] != ForSubMaterial ) continue;
		int SG = FFacesSmoothingGroup[i];

		for ( int Apex = 0; Apex != 3; ++Apex )
		{
			int VertexIndex = static_cast<int>( FFaces[i][Apex] );
			int OutputVertex = 3 * Idx + Apex;

			FSplitVertexToSGVertex_SGNum[ OutputVertex ] = SG;

			if ( !Math::IsBitSet( AlreadyAddedToSG[VertexIndex], SG ) )
			{
				// dump
//            Env->Logger->Log( L_DEBUG, "SGN: "+LStr::ToStr(SG)+" VertexIndex: "+LStr::ToStr(VertexIndex) );

				AlreadyAddedToSG[VertexIndex] = Math::SetBit( AlreadyAddedToSG[VertexIndex], SG );
				FSplitVertexToSGVertex_VIndex[ OutputVertex ] = ( int )FSGN[SG].size();
				FSplitMap[SG][VertexIndex] = ( int )FSGN[SG].size();
				FSGN[SG].push_back( LVector3( 0, 0, 0 ) ); // add empty normal
			}
			else
			{
				FSplitVertexToSGVertex_VIndex[ OutputVertex ] = FSplitMap[SG][ VertexIndex ];
			}
		}

		Idx++;
	}
}

clVertexAttribs* clVAMender::CreateVertexAttribs( int ForSubMaterial )
{
	if ( FFaces.size() == 0 )
	{
		FLastError = "Unable to create vertex array: FFaces.size() = 0";
		return NULL;
	}

//   FATAL( FFaces.size() == 0, "Unable to create vertex array: FFaces.size() = 0" );

	size_t SubMaterialFaces = 0;

	// calculate number of faces for this submaterial (immutable per frame)
	for ( size_t i = 0; i != FFaces.size(); ++i )
	{
		if ( FFacesSubMaterial[i] == ForSubMaterial )
		{
			SubMaterialFaces++;
		}
	}

	if ( !SubMaterialFaces )
	{
		return NULL;
	}

	clVertexAttribs* VA = clVertexAttribs::CreateEmpty(); // SubMaterialFaces * 3, L_TEXCOORDS_BIT | L_NORMALS_BIT );

	for ( size_t i = 0; i != FJointsFrames.size(); ++i )
	{
		VA->AddSkeletonFrame( *FJointsFrames[i] );
	}

	if ( FWeights.size() > 0 )
	{
		VA->FWeights.FStream.resize( FWeights.size() );

		sWeight* Weights = VA->FWeights.GetPtr();

		for ( size_t i = 0; i != FWeights.size(); ++i )
		{
			Weights[ i ] = FWeights[ i ];
		}
	}

	FVerticesSmoothingGroup.clear();
	FVerticesSmoothingGroup.reserve( SubMaterialFaces * 3 );

	VA->Restart( L_PT_TRIANGLE, SubMaterialFaces * 3, L_TEXCOORDS_BIT | L_NORMALS_BIT | L_BONES_BIT );

	for ( size_t i = 0; i != FFaces.size(); ++i )
	{
		if ( FFacesSubMaterial[i] != ForSubMaterial )
		{
			continue;
		}

		int CurrentSG = FFacesSmoothingGroup[i];

		// for each apex of the face
		for ( int Apex = 0; Apex != 3; ++Apex )
		{
			int VertexIndex  = static_cast<int>( FFaces[i][Apex] );

			FIXME( "think about this" )
			LVector3 Normal = LVector3( 0, 0, 0 );

			if ( FFacesNormals.size() > 0 )
			{
				Normal = FFacesNormals[i].FVertexNormal[Apex];
			}
			else if ( FNormals.size() > 0 )
			{
				Normal = FNormals[VertexIndex];
			}

			//       Normal = FNormals[VertexIndex];

			VA->SetNormalV( Normal );

			if ( FTextureFaces.size() > 0 )
			{
				int TextureIndex = static_cast<int>( FTextureFaces[i][Apex] );

				VA->SetTexCoord( FTextureVertices[TextureIndex].X, FTextureVertices[TextureIndex].Y, FTextureVertices[TextureIndex].Z );
			}

			VA->EmitVertexV( FVertices[VertexIndex], FWeightIndex[VertexIndex], FWeightCount[VertexIndex] );

			FVerticesSmoothingGroup.push_back( CurrentSG );
		}
	}

	if ( FNormals.empty() )
	{
		BuildSGNMap( ForSubMaterial );

		CalculateTBN( VA, ForSubMaterial );
	}

	return VA;
}

/*
 * 22/02/2010
     Mender is a lightweight object now
 * 26/03/2008
     Normals calculation respecting smoothing groups
 * 07/11/2007
     Separation of mesh by submaterials
 * 01/12/2005
     Optimization of mesh
 * 27/03/2005
     Fixed bug with resizing of lists
 * 06/03/2005
     It's here
*/
