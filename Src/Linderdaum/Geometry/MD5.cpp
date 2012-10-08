/**
 * \file MD5.cpp
 * \brief MD5 meshes loader
 * \version 0.5.91
 * \date 27/03/2010
 * \author Sergey Kosarevsky, 2007-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "MD5.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/VFS/FileSystem.h"
#include "Core/Logger.h"
#include "Renderer/iRenderContext.h"
#include "Geometry/VertexAttribs.h"
#include "Resources/ResourcesManager.h"

#include "Math/LVector.h"
#include "Math/LMatrix.h"
#include "Math/LQuaternion.h"

// mesh
const LString MD5_Version     = "MD5VERSION";
const LString MD5_CommandLine = "COMMANDLINE";
const LString MD5_NumJoints   = "NUMJOINTS";
const LString MD5_NumMeshes   = "NUMMESHES";
const LString MD5_Mesh        = "MESH";

const LString MD5_Joints      = "JOINTS";

const LString MD5_Material    = "MATERIAL";
const LString MD5_NumVerts    = "NUMVERTS";
const LString MD5_Vert        = "VERT";
const LString MD5_NumTris     = "NUMTRIS";
const LString MD5_Tri         = "TRI";
const LString MD5_NumWeights  = "NUMWEIGHTS";
const LString MD5_Weight      = "WEIGHT";

// anim
const LString MD5_NumFrames             = "NUMFRAMES";
const LString MD5_FrameRate             = "FRAMERATE";
const LString MD5_NumAnimatedComponents = "NUMANIMATEDCOMPONENTS";

const LString MD5_Hierarchy   = "HIERARCHY";
const LString MD5_Bounds      = "BOUNDS";
const LString MD5_BaseFrame   = "BASEFRAME";
const LString MD5_Frame       = "FRAME";

void sMD5Mesh::CalcPoints( clJointsSet* Joints )
{
	FPoints.resize( FVertices.size() );

	for ( size_t i = 0; i != FVertices.size(); ++i )
	{
		sMD5Vertex& V = FVertices[i];

		LVector3 Pos( 0, 0, 0 );

		for ( int k = 0; k != V.FWeightCount; ++k )
		{
			sWeight& Weight = FWeights[ V.FWeightIndex + k ];
			sJoint&  Joint  = Joints->GetJoint( Weight.FJointIndex );

			Pos += Joint.Transform( Weight.FPosWeight.ToVector3() ) * Weight.FPosWeight.W;
		}

		FPoints[i] = LVector3( Pos.Y, Pos.X, Pos.Z );
	}
}

void sMD5Mesh::ComputeMaxJointsPerVertex()
{
	FMaxJointsPerVertex = 0;

	for ( size_t i = 0; i != FVertices.size(); ++i )
	{
		if ( FVertices[i].FWeightCount > FMaxJointsPerVertex )
		{
			FMaxJointsPerVertex = FVertices[i].FWeightCount;
		}
	}
}

sMD5Mesh clMD5Loader::MD5_ReadMesh()
{
	sMD5Mesh Mesh;

	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLine();
		LStr::TrimSpaces( &Line );

		LString Tag = LStr::GetUpper( LStr::GetToken( Line, 1 ) );

		if ( Tag.empty () )
		{
			continue;
		}

		if ( Tag == MD5_Material )
		{
			Mesh.FMaterialName = LStr::GetToken( Line, 2 );
		}
		else if ( Tag == MD5_NumVerts )
		{
			Mesh.FVertices.resize( LStr::ToInt( LStr::GetToken( Line, 2 ) ) );
		}
		else if ( Tag == MD5_Vert )
		{
			int    Index = LStr::ToInt( LStr::GetToken( Line, 2 ) );
			float U      = LStr::ToFloat( LStr::GetToken( Line, 4 ) );
			float V      = LStr::ToFloat( LStr::GetToken( Line, 5 ) );
			int    WeightIndex = LStr::ToInt( LStr::GetToken( Line, 7 ) );
			int    WeightCount = LStr::ToInt( LStr::GetToken( Line, 8 ) );

			Mesh.SetVertex ( Index, LVector2 ( U, V ), WeightIndex, WeightCount );
		}
		else if ( Tag == MD5_NumTris )
		{
			Mesh.FTris.resize( LStr::ToInt( LStr::GetToken( Line, 2 ) ) );
		}
		else if ( Tag == MD5_Tri )
		{
			int Index = LStr::ToInt( LStr::GetToken( Line, 2 ) );

			int V1 = LStr::ToInt( LStr::GetToken( Line, 3 ) );
			int V2 = LStr::ToInt( LStr::GetToken( Line, 4 ) );
			int V3 = LStr::ToInt( LStr::GetToken( Line, 5 ) );

			Mesh.SetTri( Index, V1, V2, V3 );
		}
		else if ( Tag == MD5_NumWeights )
		{
			Mesh.FWeights.resize( LStr::ToInt( LStr::GetToken( Line, 2 ) ) );
		}
		else if ( Tag == MD5_Weight )
		{
			int Index  = LStr::ToInt( LStr::GetToken( Line, 2 ) );
			int Joint  = LStr::ToInt( LStr::GetToken( Line, 3 ) );
			float Bias = LStr::ToFloat( LStr::GetToken( Line, 4 ) );

			float x = LStr::ToFloat( LStr::GetToken( Line, 6 ) );
			float y = LStr::ToFloat( LStr::GetToken( Line, 7 ) );
			float z = LStr::ToFloat( LStr::GetToken( Line, 8 ) );

			Mesh.SetWeight ( Index, Joint, Bias, LVector3 ( x, y, z ) );
		}
		else if ( Tag == "}" )
		{
			break;
		}
	}

	Mesh.ComputeMaxJointsPerVertex();

	return Mesh;
}

///////////////////////////////// Md5Model methods ///////////////////////////////////

void clMD5Loader::MD5_ReadJoints()
{
	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLine();

		if ( LStr::GetToken( Line, 1 ) == "}" )
		{
			return;
		}

		float X = LStr::ToFloat( LStr::GetToken( Line, 4 ) );
		float Y = LStr::ToFloat( LStr::GetToken( Line, 5 ) );
		float Z = LStr::ToFloat( LStr::GetToken( Line, 6 ) );

		float Xr = LStr::ToFloat( LStr::GetToken( Line,  9 ) );
		float Yr = LStr::ToFloat( LStr::GetToken( Line, 10 ) );
		float Zr = LStr::ToFloat( LStr::GetToken( Line, 11 ) );

		sJoint Joint;

		Joint.FName        = LStr::GetToken( Line, 1 );
		Joint.FParentIndex = LStr::ToInt( LStr::GetToken( Line, 2 ) );
		Joint.FPosition    = LVector3( X, Y, Z );
		Joint.FOrientation = LQuaternion( Xr, Yr, Zr, 0.0f );
		Joint.FOrientation.ReNormalizeW();

		FJoints.AddJoint( Joint );
	}
}


void clMD5Loader::MD5_ReadHeader()
{
	int meshNo = 0;

	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLine();
		LStr::TrimSpaces( &Line );

		LString Tag = LStr::GetUpper( LStr::GetToken( Line, 1 ) );

		if ( Tag.empty () )
		{
			continue;
		}

		if ( Tag == MD5_Version )
		{
			// do nothing
		}
		else if ( Tag == MD5_CommandLine )
		{
			// do nothing
		}
		else if ( Tag == MD5_NumJoints )
		{
			FJoints.SetMaxJoints( LStr::ToInt( LStr::GetToken( Line, 2 ) ) );
		}
		else if ( Tag == MD5_Joints )
		{
			MD5_ReadJoints ();
		}
		else if ( Tag == MD5_NumMeshes )
		{
			FMeshes.resize( LStr::ToInt( LStr::GetToken( Line, 2 ) ) );
		}
		else if ( Tag == MD5_Mesh )
		{
			FMeshes[meshNo++] = MD5_ReadMesh();
		}
	}
}

void clMD5Loader::MD5_ReadAnimHierarchy()
{
	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLine();

		LString Tag = LStr::GetToken( Line, 1 );

		if ( Tag.empty () )
		{
			continue;
		}

		if ( Tag == "}" )
		{
			return;
		}

		sMD5HierarchyItem Hierarchy;

		Hierarchy.FName       = Tag;
		Hierarchy.FParent     = LStr::ToInt( LStr::GetToken( Line, 2 ) );
		Hierarchy.FFlags      = LStr::ToInt( LStr::GetToken( Line, 3 ) );
		Hierarchy.FStartIndex = LStr::ToInt( LStr::GetToken( Line, 4 ) );

		FHierarchy.push_back( Hierarchy );
	}
}

void clMD5Loader::MD5_ReadAnimBounds()
{
	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLine();

		LString Tag = LStr::GetToken( Line, 1 );

		if ( Tag.empty () )
		{
			continue;
		}

		if ( Tag == "}" )
		{
			return;
		}

		LVector3 Min, Max;

		Min.X = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		Min.Y = LStr::ToFloat( LStr::GetToken( Line, 3 ) );
		Min.Z = LStr::ToFloat( LStr::GetToken( Line, 4 ) );

		Max.X = LStr::ToFloat( LStr::GetToken( Line, 7 ) );
		Max.Y = LStr::ToFloat( LStr::GetToken( Line, 8 ) );
		Max.Z = LStr::ToFloat( LStr::GetToken( Line, 9 ) );

		LAABoundingBox BBox( Min, Max );

		FAnimBBoxes.push_back( BBox );
	}
}

void clMD5Loader::MD5_ReadAnimBaseFrame()
{
	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLine();
		LStr::TrimSpaces( &Line );

		if ( Line == "}" )
		{
			return;
		}

		float X = LStr::ToFloat( LStr::GetToken( Line, 2 ) );
		float Y = LStr::ToFloat( LStr::GetToken( Line, 3 ) );
		float Z = LStr::ToFloat( LStr::GetToken( Line, 4 ) );

		float Xr = LStr::ToFloat( LStr::GetToken( Line, 7 ) );
		float Yr = LStr::ToFloat( LStr::GetToken( Line, 8 ) );
		float Zr = LStr::ToFloat( LStr::GetToken( Line, 9 ) );

		sMD5BaseFrameJoint BaseFrame;

		BaseFrame.FPos    = LVector3( X, Y, Z );
		BaseFrame.FOrient = LQuaternion( Xr, Yr, Zr, 0 );
		BaseFrame.FOrient.ReNormalizeW();

		FBaseFrame.push_back( BaseFrame );
	}
}

void clMD5Loader::MD5_ReadAnimFrame( int no )
{
	FFrames [no] = new float [ FNumAnimatedComponents ];

	int i = 0;

	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLine();

		LString cmd = LStr::GetToken( Line, 1 );

		if ( cmd.empty () )
		{
			break;
		}

		if ( cmd == "}" )
		{
			return;
		}

		for ( int k = 1; k != 7; ++k )
		{
			LString Param = LStr::GetToken( Line, k );

			if ( Param.empty() )
			{
				break;
			}

			FFrames [no][i++] = LStr::ToFloat( Param );
		}
	}
}

void clMD5Loader::ResetJoints( clJointsSet* Joints )
{
	for ( int i = 0; i != Joints->GetJointsCount(); ++i )
	{
		sJoint& Joint = Joints->GetJoint( i );

		Joint.FName        = FHierarchy[i].FName;
		Joint.FParentIndex = FHierarchy[i].FParent;
		Joint.FPosition    = FBaseFrame[i].FPos;
		Joint.FOrientation = FBaseFrame[i].FOrient;
	}
}

void clMD5Loader::MD5_ReadAnimHeader()
{
	while ( !FStream->Eof() )
	{
		LString Line = FStream->ReadLine();
		LString Tag = LStr::GetUpper( LStr::GetToken( Line, 1 ) );

		if ( Tag.empty () )
		{
			continue;
		}

		if ( Tag == MD5_Version )
		{
		}
		else if ( Tag == MD5_CommandLine )
		{
		}
		else if ( Tag == MD5_NumFrames )
		{
			FNumFrames = LStr::ToInt( LStr::GetToken( Line, 2 ) );
			FFrames.resize( FNumFrames );
		}
		else if ( Tag == MD5_NumJoints )
		{
			FNumAnimJoints = LStr::ToInt( LStr::GetToken( Line, 2 ) );
		}
		else if ( Tag == MD5_FrameRate )
		{
			FFrameRate = LStr::ToInt( LStr::GetToken( Line, 2 ) );
		}
		else if ( Tag == MD5_NumAnimatedComponents )
		{
			FNumAnimatedComponents = LStr::ToInt( LStr::GetToken( Line, 2 ) );
		}
		else if ( Tag == MD5_Hierarchy )
		{
			MD5_ReadAnimHierarchy();
		}
		else if ( Tag == MD5_Bounds )
		{
			MD5_ReadAnimBounds();
		}
		else if ( Tag == MD5_BaseFrame )
		{
			MD5_ReadAnimBaseFrame();
		}
		else if ( Tag == MD5_Frame )
		{
			MD5_ReadAnimFrame( LStr::ToInt( LStr::GetToken( Line, 2 ) ) );
		}
	}
}

void clMD5Loader::SetFrame( clJointsSet* Joints, int N )
{
	ResetJoints( Joints );

	for ( int i = 0; i != Joints->GetJointsCount(); ++i )
	{
		int Flags = FHierarchy[i].FFlags;
		int Pos   = FHierarchy[i].FStartIndex;

		sJoint& Joint = Joints->GetJoint( i );

		if ( Flags & 1  )
		{
			Joint.FPosition.X = FFrames[N][Pos++];
		}

		if ( Flags & 2  )
		{
			Joint.FPosition.Y = FFrames[N][Pos++];
		}

		if ( Flags & 4  )
		{
			Joint.FPosition.Z = FFrames[N][Pos++];
		}

		if ( Flags & 8  )
		{
			Joint.FOrientation.FVec.X = FFrames[N][Pos++];
		}

		if ( Flags & 16 )
		{
			Joint.FOrientation.FVec.Y = FFrames[N][Pos++];
		}

		if ( Flags & 32 )
		{
			Joint.FOrientation.FVec.Z = FFrames[N][Pos++];
		}

		Joint.FOrientation.ReNormalizeW();
	}

	Joints->TransformJoints();

	// compute points
// for ( size_t i = 0; i < FMeshes.size(); i++ )
//   {
//    FMeshes[i].CalcPoints ( Joints );
//   }
}

clMD5Loader::clMD5Loader()://mesh
	FJoints(),
	FMeshes(),
	// anim
	FNumFrames( 0 ),
	FNumAnimJoints( 0 ),
	FFrameRate( 0 ),
	FNumAnimatedComponents( 0 ),
	FAnimBBoxes(),
	FHierarchy(),
	FBaseFrame(),
	FFrames(),
	FFrameJoints()
{
}

bool clMD5Loader::LoadMesh( iIStream* IStream, clMesh* Resource )
{
	guard( "%s", IStream->GetFileName().c_str() );

	FStream = IStream;

	// load .md5mesh file
	MD5_ReadHeader();

	// load .md5anim file
//   LString AnimFileName = IStream->GetVirtualFileName() + ".md5anim";
	LString Drive;
	LString Dir;
	LString FileName;
	LString FileExt;

	Env->FileSystem->SplitPath( IStream->GetVirtualFileName(), &Drive, &Dir, &FileName, &FileExt );

	LString AnimFileName = Drive + Dir + FileName + ".md5anim";


	FStream = Env->FileSystem->CreateFileReader( AnimFileName );

	MD5_ReadAnimHeader();

	// calculate frame skeletons
	FFrameJoints.resize( FNumFrames );

	for ( int i = 0; i != FNumFrames; ++i )
	{
		FFrameJoints[i].SetMaxJoints( FNumAnimJoints );

		SetFrame( &FFrameJoints[i], i );
	}

	// compute points
	for ( size_t i = 0; i < FMeshes.size(); i++ )
	{
		FMeshes[i].CalcPoints( &FFrameJoints[0] );
	}

	delete( FStream );

	FStream = IStream;

	// generate geometry

//   MeshNode->SetAnimationSegment( 0, FNumFrames-1, FNumFrames-1, 20 );

	for ( size_t i = 0; i != FMeshes.size(); ++i )
	{
		FMeshes[i].FPoints.resize( FMeshes[i].FVertices.size() );

		clVAMender* Mender = Env->Renderer->CreateVAMender();

		// setup frame skeletons
		for ( int f = 0; f != FNumFrames; ++f )
		{
			Mender->AddJointsFrame( &FFrameJoints[f] );
		}

		// setup weights
		for ( size_t w = 0; w != FMeshes[i].FWeights.size(); ++w )
		{
			Mender->EmitWeight( FMeshes[i].FWeights[w] );
		}

		// emit vertices
		for ( size_t v = 0; v != FMeshes[i].FVertices.size(); ++v )
		{
			Mender->EmitVertex( static_cast<int>( v ), FMeshes[i].FPoints[v], FMeshes[i].FVertices[v].FWeightIndex, FMeshes[i].FVertices[v].FWeightCount  );
			Mender->EmitTextureVertex( static_cast<int>( v ), LVector3( FMeshes[i].FVertices[v].FTexCoord, 0.0f ) );
		}

		// emit faces
		for ( int j = 0; j != static_cast<int>( FMeshes[i].FTris.size() ); ++j )
		{
			int i1 = FMeshes[i].FTris[j].FIndex[0];
			int i2 = FMeshes[i].FTris[j].FIndex[1];
			int i3 = FMeshes[i].FTris[j].FIndex[2];

			Mender->EmitFace( j, i1, i2, i3, -1, 0 );
			Mender->EmitTextureFace( j, i1, i2, i3 );
		}

		/*
		      // setup vertices
		      for ( int j = 0; j != static_cast<int>( FMeshes[i].FTris.size() ); ++j )
		      {
		         int i1 = FMeshes[i].FTris[j].FIndex [0];
		         int i2 = FMeshes[i].FTris[j].FIndex [1];
		         int i3 = FMeshes[i].FTris[j].FIndex [2];

		         Mender->EmitTextureVertex( 3*j + 0, LVector3( FMeshes[i].FVertices[i1].FTexCoord.X, FMeshes[i].FVertices[i1].FTexCoord.Y, 0.0f )) ;
		         Mender->EmitTextureVertex( 3*j + 1, LVector3( FMeshes[i].FVertices[i2].FTexCoord.X, FMeshes[i].FVertices[i2].FTexCoord.Y, 0.0f )) ;
		         Mender->EmitTextureVertex( 3*j + 2, LVector3( FMeshes[i].FVertices[i3].FTexCoord.X, FMeshes[i].FVertices[i3].FTexCoord.Y, 0.0f )) ;

		         Mender->EmitTextureFace( j, 3*j + 0, 3*j + 1, 3*j + 2 );

		         LVector3& VPos1 = FMeshes[i].FPoints[i1];
		         LVector3& VPos2 = FMeshes[i].FPoints[i2];
		         LVector3& VPos3 = FMeshes[i].FPoints[i3];

		         sMD5Vertex& V1 = FMeshes[i].FVertices[i1];
		         sMD5Vertex& V2 = FMeshes[i].FVertices[i2];
		         sMD5Vertex& V3 = FMeshes[i].FVertices[i3];

		         Mender->EmitVertex( 3*j + 0, VPos1, V1.FWeightIndex, V1.FWeightCount  );
		         Mender->EmitVertex( 3*j + 1, VPos2, V2.FWeightIndex, V2.FWeightCount  );
		         Mender->EmitVertex( 3*j + 2, VPos3, V3.FWeightIndex, V3.FWeightCount  );

		         Mender->EmitFace( j, 3*j + 0, 3*j + 1, 3*j + 2, -1, 0 );
		      }
		*/
		clVertexAttribs* Attribs = Mender->CreateVertexAttribs( -1 );

		Attribs->SetKeyframe( Env, 0, 0, 0 );

		if ( Attribs->FActiveVertexCount )
		{
			int MaterialIdx = -1;

			if ( !FMeshes[i].FMaterialName.empty() )
			{
				// check local dir for material
				LString MatName = Env->FileSystem->FindFile( FMeshes[i].FMaterialName, FStream->GetVirtualFileName() );

				clMaterial* Mat = Env->Resources->LoadMaterial( MatName );

				MaterialIdx = Resource->AddMaterial( Mat->GetMaterialDesc() );
			}
			else
			{
				Env->Logger->Log( L_WARNING, "No material defined for mesh in file: " + IStream->GetFileName() );
			}

			Resource->AddVertexAttribs( "MD5::noname", Attribs, LMatrix4::Identity(), -1, MaterialIdx );
		}

		delete( Mender );
	}

	// load animations into mesh
	LString AnimFile = IStream->GetVirtualFileName() + ".anim";

	Resource->LoadAnimStates( AnimFile );
	Resource->PrecalculateBoundingBoxes();

	return true;

	unguard();
}

void clMD5Loader::MD5_ReadAnimations( const LString& BaseFileName )
{
	// 1. Enumerate every animation file in the direction with a given BaseFileName

	// 2. Read each animation file

	// 3. Read master animation track file
}

/*
 * 27/03/2010
     (In)Correct normals smoothing (using VAMender)
 * 28/11/2007
     Correctly reads .md5mesh file
 * 27/11/2007
     It's here
*/
