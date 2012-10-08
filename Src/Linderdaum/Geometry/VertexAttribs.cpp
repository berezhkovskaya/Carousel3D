/**
 * \file VertexAttribs.cpp
 * \brief Host-side vertex arrays
 * \version 0.6.00
 * \date 11/01/2011
 * \author Sergey Kosarevsky, 2010-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "VertexAttribs.h"

#include "Math/LMath.h"
#include "Math/LGeomUtils.h"

#include "Images/Bitmap.h"
#include "Geometry/VAMender.h"

clVertexAttribs::clVertexAttribs()
	: FPrimitiveType( L_PT_TRIANGLE ),
	  FActiveVertexCount( 0 ),
	  FDynamic( false ),
	  FIndices( 0 ),
	  FVertices( 0 ),
	  FTexCoords( 0 ),
	  FNormals( 0 ),
	  FBinormals( 0 ),
	  FTangents( 0 ),
	  FColors( 0 ),
	  FBlendingCoefs( 0 ),
	  FSkeletonFrames( 0 ),
	  FBoundingBoxes( 0 ),
	  FWeights( 0 ),
//
	  FMotionBlur( false ),
	  FCastShadow( true ),
	  FReceiveShadow( true ),
// private
	  FCurrentTexCoords(),
	  FCurrentNormal(),
     FCurrentColor( LC_White ),
	  FVTBits( 0 ),
	  FStreams( L_VS_TOTAL_ATTRIBS ),
	  FMorphTargetsBitmap( NULL )
{
}

clVertexAttribs::clVertexAttribs( size_t Vertices,
                                  int VertexTypeBits )
	: FPrimitiveType( L_PT_TRIANGLE ),
	  FActiveVertexCount( Vertices ),
	  FDynamic( false ),
	  FIndices( 0 ),
	  FVertices( Vertices ),
	  FTexCoords( 0 ),
	  FNormals( 0 ),
	  FBinormals( 0 ),
	  FTangents( 0 ),
	  FColors( 0 ),
	  FBlendingCoefs( 0 ),
	  FSkeletonFrames( 0 ),
	  FBoundingBoxes( 0 ),
	  FWeights( 0 ),
// private
	  FCurrentTexCoords(),
	  FCurrentNormal(),
     FCurrentColor( LC_White ),
	  FVTBits( VertexTypeBits ),
	  FStreams( L_VS_TOTAL_ATTRIBS )
{
	int VTB = VertexTypeBits;

	using Linderdaum::Math::IsMaskSet;

	if ( IsMaskSet( VTB, L_TEXCOORDS_BIT   ) )
	{
		FTexCoords.FStream.resize( Vertices );
	}

	if ( IsMaskSet( VTB, L_NORMALS_BIT     ) )
	{
		FNormals.FStream.resize( Vertices );
	}

	if ( IsMaskSet( VTB, L_BINORMALS_BIT   ) )
	{
		FBinormals.FStream.resize( Vertices );
	}

	if ( IsMaskSet( VTB, L_TANGENTS_BIT    ) )
	{
		FTangents.FStream.resize( Vertices );
	}

	if ( IsMaskSet( VTB, L_COLORS_BIT      ) )
	{
		FColors.FStream.resize( Vertices );
	}

	if ( IsMaskSet( VTB, L_BONES_BIT       ) )
	{
		FBlendingCoefs.FStream.resize( Vertices );
	}
}

clVertexAttribs::~clVertexAttribs()
{
}

void clVertexAttribs::LoadFromStream( iIStream* Stream )
{
	Stream->BlockRead( &FPrimitiveType, sizeof( FPrimitiveType ) );
	Stream->BlockRead( &FDynamic, sizeof( FDynamic ) );
	Stream->BlockRead( &FActiveVertexCount, sizeof( FActiveVertexCount ) );

	FIndices.LoadFromStream( Stream );
	FVertices.LoadFromStream( Stream );
	FTexCoords.LoadFromStream( Stream );
	FNormals.LoadFromStream( Stream );
	FBinormals.LoadFromStream( Stream );
	FTangents.LoadFromStream( Stream );
	FColors.LoadFromStream( Stream );
	FBlendingCoefs.LoadFromStream( Stream );
	FSkeletonFrames.LoadFromStreamArray( Stream );
	FBoundingBoxes.LoadFromStream( Stream );
	FWeights.LoadFromStream( Stream );

	Stream->BlockRead( &FMotionBlur,    sizeof( FMotionBlur    ) );
	Stream->BlockRead( &FCastShadow,    sizeof( FCastShadow    ) );
	Stream->BlockRead( &FReceiveShadow, sizeof( FReceiveShadow ) );

	Stream->BlockRead( &FCurrentTexCoords, sizeof( FCurrentTexCoords ) );
	Stream->BlockRead( &FCurrentNormal,    sizeof( FCurrentNormal ) );
	Stream->BlockRead( &FCurrentColor,     sizeof( FCurrentColor ) );
	Stream->BlockRead( &FVTBits,           sizeof( FVTBits ) );
}

void clVertexAttribs::SaveToStream( iOStream* Stream ) const
{
	Stream->BlockWrite( &FPrimitiveType, sizeof( FPrimitiveType ) );
	Stream->BlockWrite( &FDynamic, sizeof( FDynamic ) );
	Stream->BlockWrite( &FActiveVertexCount, sizeof( FActiveVertexCount ) );

	FIndices.SaveToStream( Stream );
	FVertices.SaveToStream( Stream );
	FTexCoords.SaveToStream( Stream );
	FNormals.SaveToStream( Stream );
	FBinormals.SaveToStream( Stream );
	FTangents.SaveToStream( Stream );
	FColors.SaveToStream( Stream );
	FBlendingCoefs.SaveToStream( Stream );
	FSkeletonFrames.SaveToStreamArray( Stream );
	FBoundingBoxes.SaveToStream( Stream );
	FWeights.SaveToStream( Stream );

	Stream->BlockWrite( &FMotionBlur,    sizeof( FMotionBlur    ) );
	Stream->BlockWrite( &FCastShadow,    sizeof( FCastShadow    ) );
	Stream->BlockWrite( &FReceiveShadow, sizeof( FReceiveShadow ) );

	Stream->BlockWrite( &FCurrentTexCoords, sizeof( FCurrentTexCoords ) );
	Stream->BlockWrite( &FCurrentNormal,    sizeof( FCurrentNormal ) );
	Stream->BlockWrite( &FCurrentColor,     sizeof( FCurrentColor ) );
	Stream->BlockWrite( &FVTBits,           sizeof( FVTBits ) );
}

const std::vector<const void*>& clVertexAttribs::EnumerateVertexStreams() const
{
	FStreams[ L_VS_VERTEX   ] = FVertices.GetPtr();
	FStreams[ L_VS_TEXCOORD ] = FTexCoords.GetPtr();
	FStreams[ L_VS_NORMAL   ] = FNormals.GetPtr();
	FStreams[ L_VS_TANGENT  ] = FTangents.GetPtr();
	FStreams[ L_VS_BINORMAL ] = FBinormals.GetPtr();
	FStreams[ L_VS_COLORS   ] = FColors.GetPtr();
	FStreams[ L_VS_BONES    ] = FBlendingCoefs.GetPtr();
	FStreams[ L_VS_AUX0     ] = NULL;

	return FStreams;
}

void clVertexAttribs::AddSkeletonFrame( const clJointsSet& FrameJoints )
{
	FSkeletonFrames.FStream.push_back( FrameJoints );
	FBoundingBoxes.FStream.push_back( LAABoundingBox() );
}

const clJointsSet& clVertexAttribs::GetSkeletonFrame( size_t i ) const
{
	return FSkeletonFrames.FStream[ i ];
}

size_t clVertexAttribs::GetSkeletonFramesCount() const
{
	return FSkeletonFrames.size();
}

void clVertexAttribs::GenerateTexCoordsFromVertices( const LVector3& Coefs )
{
	if ( !FTexCoords.GetPtr() )
	{
		FTexCoords.FStream.resize( FVertices.size() );

		FVTBits |= L_TEXCOORDS_BIT;
	}

	LVector4* TexCoords = FTexCoords.GetPtr();
	LVector3* Coords    = FVertices.GetPtr();

	// update texcoords
	for ( size_t i = 0; i != FVertices.size(); i++ )
	{
		TexCoords[i] = LVector4( Coords[i] * Coefs, 0.0f );
	}
}

void clVertexAttribs::CreateFromIndexedPoints( int NumPoints, LVector3* Points, LVector3* Normals, LVector4* TexCoords )
{
	Restart( L_PT_TRIANGLE, NumPoints, ( TexCoords ? L_TEXCOORDS_BIT : 0 ) | ( Normals ? L_NORMALS_BIT : 0 ) );

	for ( int i = 0 ; i < NumPoints ; i++ )
	{
		SetNormalV( Normals[i] );
		SetTexCoord( TexCoords[i].X, TexCoords[i].Y, TexCoords[i].Z );

		EmitVertexV( Points[i], -1, -1 );
	}
}

void clVertexAttribs::AddPolygon( const LPoly& Poly )
{
	int Indices[Math::MAX_VERTS_IN_POLY];
	LVector3 TmpVtx[Math::MAX_VERTS_IN_POLY];

	SetNormalV( Poly.GetNormal() );

	int Num = Poly.Triangulate( Indices );

	for ( int i = 0 ; i < Num ; i++ )
	{
		for ( int k = 0 ; k < 3 ; k++ )
		{
			LVector3 Vtx = Poly.FVertices[ Indices[i * 3 + k] ];

			float TexU = Poly.FTexV1.Dot( Vtx );
			float TexV = Poly.FTexV2.Dot( Vtx );

			SetTexCoord( TexU, TexV, 0.0f );

			EmitVertexV( Vtx, -1, -1 );
		}
	}
}

void clVertexAttribs::AddPolyList( const LPolyList& PolyList )
{
	size_t NumPts = PolyList.FPolys.size() * Math::MAX_VERTS_IN_POLY;

	LArray<int> Indices( NumPts );
	LArray<LVector3> Pts( NumPts );
	LArray<LVector3> Norms( NumPts );
	LArray<LVector4> Texs( NumPts );

	int NumOutPoints = 0;

	int Num = PolyList.TriangulateIndexed( Pts.GetPtr( 0 ), NumOutPoints, Indices.GetPtr( 0 ), -1.0f, Norms.GetPtr( 0 ), Texs.GetPtr( 0 ) );

//	int  TriangulateIndexed(v3* Points, int& NumPoints, int* Indices, float Thresh, vec3* Normals, vec4* OutTexCoords);
	for ( int i = 0 ; i < Num * 3 ; i++ )
	{
		LVector3 P = Pts[i];
		SetNormalV( Norms[i] );
		SetTexCoord( Texs[i].X, Texs[i].Y, Texs[i].Z );
		EmitVertexV( P, -1, -1 );
	}
}

void clVertexAttribs::CreateFromPolyList( const LPolyList& PolyList )
{
	Restart( L_PT_TRIANGLE, PolyList.FPolys.size() * Math::MAX_VERTS_IN_POLY, L_TEXCOORDS_BIT | L_NORMALS_BIT );
	AddPolyList( PolyList );
}

void clVertexAttribs::CreateFromPolygon( const LPoly& Poly )
{
	Restart( L_PT_TRIANGLE, Poly.FNumVertices * 3, L_TEXCOORDS_BIT | L_NORMALS_BIT );

	AddPolygon( Poly );
}

clVertexAttribs* clVertexAttribs::CloneVA() const
{
	clVertexAttribs* NewVA = new clVertexAttribs();

	NewVA->MergeWith( this );

	return NewVA;
}

bool clVertexAttribs::MergeWith( const clVertexAttribs* Other )
{
	/// No topology change allowed
	if ( Other->FPrimitiveType != this->FPrimitiveType )
	{
		return false;
	}

	/// This is dangerous, but required for merging something with an empty array
	if ( Other->GetVertexBits() != this->FVTBits )
	{
		this->FVTBits = Other->FVTBits;
	}

	/// Copy each attribute stream
	FVertices.MergeWith( Other->FVertices );

	using Linderdaum::Math::IsMaskSet;

	if ( IsMaskSet( FVTBits, L_TEXCOORDS_BIT   ) ) { FTexCoords.MergeWith( Other->FTexCoords ); }

	if ( IsMaskSet( FVTBits, L_NORMALS_BIT     ) ) { FNormals.MergeWith( Other->FNormals ); }
	if ( IsMaskSet( FVTBits, L_BINORMALS_BIT   ) ) FBinormals.MergeWith(Other->FBinormals);
	if ( IsMaskSet( FVTBits, L_TANGENTS_BIT    ) ) FTangents.MergeWith(Other->FTangents);
   if ( IsMaskSet( FVTBits, L_COLORS_BIT      ) ) FColors.MergeWith(Other->FColors);

	if ( IsMaskSet( FVTBits, L_BONES_BIT       ) ) { FBlendingCoefs.MergeWith( Other->FBlendingCoefs ); }

	size_t OldCount = FIndices.size();
	FIndices.MergeWith( Other->FIndices );

	// add offsets to each index

	for ( size_t i = 0 ; i < Other->FIndices.size() ; i++ )
	{
		FIndices[i + OldCount] += ( Luint )OldCount;
	}

	FActiveVertexCount += Other->FActiveVertexCount;

	return true;
}

void clVertexAttribs::Scale( float Coef )
{
	Transform( LMatrix4::GetDiagonalMatrixV( LVector4( Coef, Coef, Coef, 1.0f ) ) );
}

void clVertexAttribs::Translate( const LVector3& Vec )
{
	Transform( LMatrix4::GetTranslateMatrix( Vec ) );
}

void clVertexAttribs::Transform( const LMatrix4& Mtx )
{
	LVector3* Vertices = FVertices.GetPtr();

	size_t VertexCount = FVertices.size();

	/// TODO: use pointers without [i]
	for ( size_t i = 0; i != VertexCount; i++ )
	{
		Vertices[i] = Mtx * Vertices[i];
	}
}

void clVertexAttribs::TransformTexCoords( const LMatrix4& Mtx )
{
	LVector4* Coords = FTexCoords.GetPtr();
	size_t VertexCount = FVertices.size();

	/// TODO: use pointers without [i]
	for ( size_t i = 0; i != VertexCount; i++ )
	{
		Coords[i] = Mtx * Coords[i];
	}
}

void clVertexAttribs::NormalizeVertices()
{
	LVector3* Vertices = FVertices.GetPtr();
	size_t VertexCount = FVertices.size();

	for ( size_t i = 0; i != VertexCount; i++ )
	{
		Vertices[i] = Vertices[i].GetNormalized();
	}
}

LVector3 clVertexAttribs::TranslateToCenter()
{
	LVector3 CM;

	size_t VertexCount = FVertices.size();

	if ( VertexCount == 0 ) { return CM; }

	LVector3* Vertices = FVertices.GetPtr();

	for ( size_t i = 0; i != VertexCount; i++ ) { CM += Vertices[i]; }

	CM /= static_cast<float>( VertexCount );

	this->Translate( -CM );

	return CM;
}

void clVertexAttribs::BuildOBBTree( LOBBTree& Tree, int MaxDepth )
{
	LArray<LVector3> Tmp( GetNumTriangles() * 3 );

	CopyVerticesToArray( Tmp );

	CalculateOBBTree_NoCopy( -1, 0, MaxDepth, &Tree, static_cast<int>( Tmp.size() ), Tmp.begin() );
}

LAABoundingBox clVertexAttribs::CalculateBoundingBox() const
{
	LAABoundingBox BBox;

	BBox.FromPoints( static_cast<int>( FVertices.size() ), FVertices.GetPtr() );

	return BBox;
}

LSphere clVertexAttribs::CalculateBoundingSphere() const
{
	return LSphere( ( int )FVertices.size(), FVertices.GetPtr() );
}

void clVertexAttribs::SetKeyframe( sEnvironment* Env, int KeyframeNum, int NextKeyframeNum, float LerpCoef )
{
	if ( FWeights.size() > 0 )
	{
		int Last = static_cast<int>( FSkeletonFrames.size() ) - 1;

		if ( KeyframeNum > Last ) { KeyframeNum = Last; }

		if ( NextKeyframeNum > Last ) { NextKeyframeNum = Last; }

		CalcPointsFromBitmap( Env, KeyframeNum, NextKeyframeNum, LerpCoef );
	}
}

clBitmap* clVertexAttribs::PackSkeletonsToBitmap( sEnvironment* Env ) const
{
	if ( !FSkeletonFrames.size() ) { return NULL; }

	/*
	  Texture layout:

	  line0: vec4(WeightPosW)   x NumWeights
	  line1: vec4(WeightPosIdx) x NumWeights
	  line2: vec4(JointQuat)    x NumJoints
	  line3: vec4(JointPos)     x NumJoints
	  ...
	*/

	// prepare texture metrics
	int NumWeights = ( int )FWeights.size();
	int NumJoints  = ( int )FSkeletonFrames[0].GetJointsCount();
	int NumFrames  = ( int )FSkeletonFrames.size();

	using std::max;
	int TextureWidth  = max( NumWeights, NumJoints );
	int TextureHeight = NumFrames * 4;

//	TextureWidth  = Linderdaum::Math::GetNextPowerOf2( TextureWidth );
//	TextureHeight = Linderdaum::Math::GetNextPowerOf2( TextureHeight );

	clBitmap* Bitmap = clBitmap::CreateBitmap( Env, TextureWidth, TextureHeight, 1, L_BITMAP_FLOAT32_RGBA, L_TEXTURE_2D );

	for ( int Frame = 0; Frame != NumFrames; Frame++ )
	{
		int MorphTargetVStart = Frame * 4;

		// pack weights
		for ( int Weight = 0; Weight != NumWeights; Weight++ )
		{
			Bitmap->SetPixel( Weight, MorphTargetVStart + 0, 0, FWeights[Weight].FPosWeight );
			Bitmap->SetPixel( Weight, MorphTargetVStart + 1, 0, LVector4( ( float )FWeights[Weight].FJointIndex ) );
		}

		// pack joints
		for ( int Joint = 0; Joint != NumJoints; Joint++ )
		{
			sJoint J = FSkeletonFrames[Frame].GetJoint( Joint );

			LVector4 Quat = J.FOrientation.ToVector4();
			LVector4 Pos  = LVector4( J.FPosition );
			Bitmap->SetPixel( Joint, MorphTargetVStart + 2, 0, Quat );
			Bitmap->SetPixel( Joint, MorphTargetVStart + 3, 0, Pos  );
		}
	}

	return Bitmap;
}

void clVertexAttribs::PrecalculateBoundingBoxes()
{
	size_t Size = FSkeletonFrames.size();

	if ( Size > 0 )
	{
		FBoundingBoxes.resize( Size );

		for ( size_t Frame = 0; Frame != Size; Frame++ )
		{
			// generate vertieces for this frame
			CalcPoints( &FSkeletonFrames[Frame], &FSkeletonFrames[Frame], 0.0f );

			// store bounding box for every frame
			FBoundingBoxes[Frame] = CalculateBoundingBox();
		}
	}
	else
	{
		FBoundingBoxes.resize( 1 );
		FBoundingBoxes[0] = CalculateBoundingBox();
	}
}

LAABoundingBox clVertexAttribs::GetBoundingBox( size_t Keyframe )
{
	if ( !FBoundingBoxes.size() )
	{
		PrecalculateBoundingBoxes();
	}

	if ( Keyframe > FBoundingBoxes.size() ) { return LAABoundingBox(); }

	return FBoundingBoxes[Keyframe];
}

LAABoundingBox clVertexAttribs::GetBoundingBoxInterpolated( size_t Keyframe1, size_t Keyframe2, float Blend )
{
	if ( !FBoundingBoxes.size() )
	{
		PrecalculateBoundingBoxes();
	}

	if ( Keyframe1 >= FBoundingBoxes.size() ) { return LAABoundingBox(); }

	if ( Keyframe2 >= FBoundingBoxes.size() ) { return LAABoundingBox(); }

	LAABoundingBox Box;

	Box.Lerp( FBoundingBoxes[Keyframe1], FBoundingBoxes[Keyframe2], Blend );

	return Box;
}

void clVertexAttribs::CalcPoints( clJointsSet* Joints1, clJointsSet* Joints2, float Blend )
{
	LVector3* Vertices      = &FVertices.FStream[0];
	LVector2* BlendingCoefs = &FBlendingCoefs.FStream[0];

	for ( size_t i = 0; i != FBlendingCoefs.size(); i++ )
	{
		int WeightIndex = ( int )BlendingCoefs[ i ].X;
		int WeightCount = ( int )BlendingCoefs[ i ].Y;

		LVector3 Pos( 0, 0, 0 );

		for ( int k = 0; k != WeightCount; ++k )
		{
			sWeight& Weight = FWeights.FStream[ WeightIndex + k ];
			sJoint&  Joint1  = Joints1->GetJoint( Weight.FJointIndex );
			sJoint&  Joint2  = Joints2->GetJoint( Weight.FJointIndex );

			sJoint Joint = Joint1;
			Joint.FOrientation.SLERP( Joint1.FOrientation, Joint2.FOrientation, Blend );
			Joint.FPosition.Lerp( Joint1.FPosition, Joint2.FPosition, Blend );

			Pos += Joint.Transform( Weight.FPosWeight.ToVector3() ) * Weight.FPosWeight.W;
		}

		Vertices[i] = Pos;
	}
}

clBitmap* clVertexAttribs::GetMorphTargetsBitmap( sEnvironment* Env ) const
{
	if ( !FMorphTargetsBitmap )
	{
		FMorphTargetsBitmap = PackSkeletonsToBitmap( Env );
	}

	return FMorphTargetsBitmap;
}

void clVertexAttribs::CalcPointsFromBitmap( sEnvironment* Env, int Frame1, int Frame2, float Blend )
{
	if ( !FMorphTargetsBitmap )
	{
		FMorphTargetsBitmap = PackSkeletonsToBitmap( Env );
	}

	LVector3* Vertices      = &FVertices.FStream[0];
	LVector2* BlendingCoefs = &FBlendingCoefs.FStream[0];

	int VStart1 = Frame1 * 4;
	int VStart2 = Frame2 * 4;

	sJoint Joint;
	sJoint Joint1;
	sJoint Joint2;

	for ( size_t i = 0; i != FBlendingCoefs.size(); i++ )
	{
		int WeightIndex = ( int )BlendingCoefs[ i ].X;
		int WeightCount = ( int )BlendingCoefs[ i ].Y;

		LVector3 Pos( 0, 0, 0 );

		for ( int k = 0; k != WeightCount; ++k )
		{
			LVector4 PosWeight  = FMorphTargetsBitmap->GetPixel( WeightIndex + k, VStart1 + 0, 0 );
			LVector4 JointIndex = FMorphTargetsBitmap->GetPixel( WeightIndex + k, VStart1 + 1, 0 );

			Joint1.FOrientation = LQuaternion( FMorphTargetsBitmap->GetPixel( ( int )JointIndex.X, VStart1 + 2, 0 ) );
			Joint1.FPosition    =              FMorphTargetsBitmap->GetPixel( ( int )JointIndex.X, VStart1 + 3, 0 ).ToVector3();

			Joint2.FOrientation = LQuaternion( FMorphTargetsBitmap->GetPixel( ( int )JointIndex.X, VStart2 + 2, 0 ) );
			Joint2.FPosition    = FMorphTargetsBitmap->GetPixel( ( int )JointIndex.X, VStart2 + 3, 0 ).ToVector3();

			Joint.FOrientation.SLERP( Joint1.FOrientation, Joint2.FOrientation, Blend );
			Joint.FPosition.Lerp( Joint1.FPosition, Joint2.FPosition, Blend );

			Pos += Joint.Transform( PosWeight.ToVector3() ) * PosWeight.W;
		}

		Vertices[i] = Pos;
	}
}

clVertexAttribs* clVertexAttribs::CreateEmpty()
{
	return new clVertexAttribs();
}

clVertexAttribs* clVertexAttribs::Create( size_t Vertices, int VertexTypeBits )
{
	return new clVertexAttribs( Vertices, VertexTypeBits );
}

clVertexAttribs* clVertexAttribs::CreateFromTriangles( const std::vector<int>& Indices, const std::vector<LVector3>& Vertices, int VertexTypeBits )
{
	clVertexAttribs* VA = CreateEmpty();

	VA->Restart( L_PT_TRIANGLE, Indices.size(), VertexTypeBits );

	for ( size_t i = 0; i != Indices.size(); i++ )
	{
		int Index  = Indices[i];
		LVector3 V = Vertices[ Index ];

		// current triangle first index
		size_t SIndex    = ( i / 3 ) * 3;
		LVector3 D1 = Vertices[ Indices[SIndex+0] ] - Vertices[ Indices[SIndex+1] ];
		LVector3 D2 = Vertices[ Indices[SIndex+1] ] - Vertices[ Indices[SIndex+2] ];
		VA->SetNormalV( ( D1.Cross( D2 ) ).GetNormalized() );

		// calculate spherical texcoords
		float R = V.Length();
		float Theta = ( R > 0.0f ) ? acos( V.Z / R ) : 0.0f;
		float Phi   = atan2( V.Y, V.X );
		VA->SetTexCoord( Theta / Math::PI, Phi / Math::PI2, 0 );

		VA->EmitVertexV( V, -1, -1 );
	}

	return VA;
}

void clVertexAttribs::SetIndices16( size_t Count, const Lushort* Indices )
{
	FIndices.FStream.resize( Count );

	if ( Indices )
	{
		// convert to unsigned int
		const Lushort* From = Indices;
		Luint*         To = &FIndices.FStream[0];

		for ( size_t i = 0; i != Count; i++ )
		{
			( *To++ ) = ( *From++ );
		}
	}
}

void clVertexAttribs::SetIndices32( size_t Count, const Luint*   Indices )
{
	FIndices.FStream.resize( Count );

	if ( Indices )
	{
		memcpy( &FIndices.FStream[0], Indices, sizeof( Luint ) * Count );
	}
}

void clVertexAttribs::SetTexCoord( float U, float V, float W )
{
	FCurrentTexCoords = LVector4( U, V, W, 0.0f );
}

void clVertexAttribs::SetTexCoordV2( const LVector2& V )
{
	FCurrentTexCoords = LVector4( V );
}

void clVertexAttribs::SetTexCoordV3( const LVector3& V )
{
	FCurrentTexCoords = LVector4( V );
}

void clVertexAttribs::SetTexCoordV4( const LVector4& V )
{
	FCurrentTexCoords = V;
}

void clVertexAttribs::SetNormalV( const LVector3& Vec )
{
	FCurrentNormal = Vec;
}

void clVertexAttribs::SetColorV( const LVector4& Vec )
{
	FCurrentColor = Vec;
}

void clVertexAttribs::Restart( LPrimitiveType Primitive, size_t ReserveVertices, int VertexTypeBits )
{
	FCurrentColor     = LC_White;
	FCurrentNormal    = LVector3( 0, 0, 0 );
	FCurrentTexCoords = LVector4( 0, 0, 0, 0 );
	FVTBits           = VertexTypeBits;

	FPrimitiveType = Primitive;

	FActiveVertexCount = 0;

	FVertices.FStream.resize( ReserveVertices );

	using Linderdaum::Math::IsMaskSet;

	if ( IsMaskSet( FVTBits, L_TEXCOORDS_BIT   ) )
	{
		FTexCoords.FStream.resize( ReserveVertices );
	}

	if ( IsMaskSet( FVTBits, L_NORMALS_BIT     ) )
	{
		FNormals.FStream.resize( ReserveVertices );
	}

	if ( IsMaskSet( FVTBits, L_BINORMALS_BIT   ) )
	{
		FBinormals.FStream.resize( ReserveVertices );
	}

	if ( IsMaskSet( FVTBits, L_TANGENTS_BIT    ) )
	{
		FTangents.FStream.resize( ReserveVertices );
	}

	if ( IsMaskSet( FVTBits, L_COLORS_BIT     ) )
	{
		FColors.FStream.resize( ReserveVertices );
	}

	if ( IsMaskSet( FVTBits, L_BONES_BIT ) )
	{
		FBlendingCoefs.FStream.resize( ReserveVertices );
	}

	/*
	   if (Vertices)
	   {
	      FATAL( !VA, "Vertex array is NULL");
	      FATAL( VA->FVertices.GetSize() != Vertices,
	             "Vertex array should have "+LStr::ToStr(Vertices)+" vertices. "+Message);
	   }

	   FVertices = FVertexArray->FVertices.GetPtr();
	   FBlendingCoefs = FVertexArray->FBlendingCoefs.GetPtr();

	   FATAL( !FVertices, "VertexArray->GetVertices() is NULL");
	   FATAL( FVertexArray->FPrimitiveType != Primitive, "Wrong primitive type in vertex array");

	   FNormals = FVertexArray->FNormals.GetPtr();
	   FTexCoords = FVertexArray->FTexCoords.GetPtr();
	*/
}

void clVertexAttribs::EmitVertex( float X, float Y, float Z, int WeightStart, int WeightCount )
{
	EmitVertexV( LVector3( X, Y, Z ), WeightStart, WeightCount );
}

void clVertexAttribs::EmitVertexV( LVector3 Vec, int WeightStart, int WeightCount )
{
	FVertices.FStream[ FActiveVertexCount ] = Vec;

	using Linderdaum::Math::IsMaskSet;

	if ( IsMaskSet( FVTBits, L_TEXCOORDS_BIT   ) )
	{
		FTexCoords.FStream[ FActiveVertexCount ] = FCurrentTexCoords;
	}

	if ( IsMaskSet( FVTBits, L_NORMALS_BIT     ) )
	{
		FNormals.FStream[ FActiveVertexCount ] = FCurrentNormal;
	}

	if ( IsMaskSet( FVTBits, L_COLORS_BIT     ) )
	{
		FColors.FStream[ FActiveVertexCount ] = FCurrentColor;
	}

//   if ( IsMaskSet( FVTBits, L_BINORMALS_BIT   ) ) FBinormals.FStream.push_back();
//   if ( IsMaskSet( FVTBits, L_TANGENTS_BIT    ) ) FTangents.FStream.push_back(;
//   if ( IsMaskSet( FVTBits, L_COLORS_BIT      ) ) FColors.FStream.push_back(;
	if ( IsMaskSet( FVTBits, L_BONES_BIT       ) )
	{
		FBlendingCoefs.FStream[ FActiveVertexCount ] = LVector2( WeightStart, WeightCount );
	}

	FActiveVertexCount++;
}

int clVertexAttribs::GetNumTriangles() const
{
	int NumVtx = static_cast<int>( FVertices.size() );

	switch ( FPrimitiveType )
	{
		case L_PT_POINT:
			return 0;
		case L_PT_LINE:
			return 0;
		case L_PT_LINE_STRIP:
			return 0;
		case L_PT_TRIANGLE:
			return NumVtx / 3;
		case L_PT_TRIANGLE_STRIP:
			return NumVtx / 2;
	}

	return 0;
}

void clVertexAttribs::GetTriangle( int idx, LVector3& V1, LVector3& V2, LVector3& V3 ) const
{
	int mult = 0;

	if ( FPrimitiveType == L_PT_TRIANGLE )
	{
		mult = 3;
	}
	else if ( FPrimitiveType == L_PT_TRIANGLE_STRIP )
	{
		mult = 2;
	}
	else
	{
		return;
	}

	V1 = FVertices.GetPtr()[idx * mult + 0];
	V2 = FVertices.GetPtr()[idx * mult + 1];
	V3 = FVertices.GetPtr()[idx * mult + 2];
}

void clVertexAttribs::SplitTriangles()
{
	if ( FPrimitiveType != L_PT_TRIANGLE ) { return; }

	clVertexAttribs* VA = clVertexAttribs::CreateEmpty();

	VA->Restart( L_PT_TRIANGLE, GetNumTriangles() * 3 * 4, FVTBits );

	// TODO: implement
	for ( int i = 0; i != GetNumTriangles(); i++ )
	{
		LVector3 V1, V2, V3;

		GetTriangle( i, V1, V2, V3 );

		VA->SetNormalV( V1 );
		VA->SetTexCoord( 1, 1, 0 );

		VA->EmitVertexV( V1, -1, -1 );
		VA->EmitVertexV( V2, -1, -1 );
		VA->EmitVertexV( V3, -1, -1 );
	}

	delete( VA );
}

void clVertexAttribs::CopyVerticesToArray( LArray<LVector3>& Out ) const
{
	LVector3 V1, V2, V3;

	int Sz = GetNumTriangles();
	Out.reserve( Sz * 3 );

	for ( int i = 0 ; i < Sz ; i++ )
	{
		GetTriangle( i, V1, V2, V3 );

		Out[i * 3 + 0] = V1;
		Out[i * 3 + 1] = V2;
		Out[i * 3 + 2] = V3;
	}
}

bool clVertexAttribs::IntersectWithRay( const LVector3& P, const LVector3& A, LVector3& isect ) const
{
	int TriangleIdx;
	return IntersectWithRayAndFindTriangle( P, A, isect, TriangleIdx );
}

// return closest point on the mesh and find triangle index (or -1 if no intersection)
bool clVertexAttribs::IntersectWithRayAndFindTriangle( const LVector3& P, const LVector3& A, LVector3& isect, int& TriangleIdx ) const
{
	int NumTris = GetNumTriangles();

	float Dist = Linderdaum::Math::INFINITY;
	bool Intersection = false;

	LVector3 result;
	LVector3 V1, V2, V3;

	TriangleIdx = -1;

	for ( int i = 0 ; i < NumTris ; i++ )
	{
		GetTriangle( i, V1, V2, V3 );

		if ( IntersectRayToTriangle( P, A, V1, V2, V3, result ) )
		{
			Intersection = true;

			float NewDist = ( result - P ).Length();

			if ( NewDist < Dist )
			{
				Dist = NewDist;
				isect = result;

				TriangleIdx = i;
			}
		}
	}

	return Intersection;
}

void clVertexAttribs::MapPlanar( const LVector3& Start, const LVector3& V1, const LVector3& V2 )
{
	for ( size_t i = 0 ; i < FVertices.FStream.size() ; i++ )
	{
		LVector3 V = FVertices[i] - Start;
		float TexU = V.Dot( V1 );
		float TexV = V.Dot( V2 );

		FTexCoords[i] = vec4( TexU, TexV, 1.0f, 0.0f );
	}
}

void clVertexAttribs::MapSpherical( const LVector3& Center, float Radius, float PhiOffset, float ThetaOffset )
{
	for ( size_t i = 0 ; i < FVertices.FStream.size() ; i++ )
	{
		LVector3 V = FVertices[i] - Center;

		float r = ( Radius > 0.0f ) ? Radius : V.Length();

		float TexV = ( acosf( V.Z / r ) - ThetaOffset ) / Math::PI;

		float sv = acosf( V.X / ( r * sinf( Math::PI * TexV ) ) );
		float TexU = ( ( ( ( V.Y < 0 ) ? Math::PI : 0.0f ) + sv ) - PhiOffset ) / Math::TWOPI;

		FTexCoords.FStream[i] = vec4( TexU, TexV, 1.0f, 0.0f );
	}
}

void clVertexAttribs::MapCylindrical( const LVector3& Base, float Radius, float ThetaOffset )
{
	for ( size_t i = 0 ; i < FVertices.FStream.size() ; i++ )
	{
		LVector3 V = FVertices[i];

		float r = ( Radius > 0.0f ) ? Radius : sqrtf( V.X * V.X + V.Y * V.Y );

		float TexU = ( Math::Vec2Angle( V.X / r, V.Y / r ) - Math::RadToDeg( ThetaOffset ) ) / 360.0f;
		float TexV = V.Z;

		FTexCoords.FStream[i] = vec4( TexU, TexV, 1.0f, 0.0f );
	}
}

/*
 * 11/01/2011
     SplitTriangles()
     CreateFromTriangles()
 * 22/08/2010
     Fixed caching of skeleton frames
 * 01/08/2010
     PrecalculateBoundingBoxes()
 * 08/02/2010
     It's here
*/
