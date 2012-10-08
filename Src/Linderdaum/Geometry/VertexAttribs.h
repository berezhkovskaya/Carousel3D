/**
 * \file VertexAttribs.h
 * \brief Host-side vertex arrays
 * \version 0.6.00
 * \date 11/01/2011
 * \author Sergey Kosarevsky, 2010-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clVertexAttribs_
#define _clVertexAttribs_

#include "Platform.h"
#include "Utils/LArray.h"
#include "LString.h"

#include "Math/LAABB.h"
#include "Math/LSphere.h"
#include "Math/LVector.h"
#include "Math/LPoly.h"
#include "Math/LOBB.h"

#include "Geometry/Joints.h"
#include "Geometry/AttribStream.h"

#include "Core/VFS/iIStream.h"
#include "Core/VFS/iOStream.h"

class clBitmap;

const int L_TEXCOORDS_BIT   = 1;
const int L_NORMALS_BIT     = 2;
const int L_BINORMALS_BIT   = 4;
const int L_TANGENTS_BIT    = 8;
const int L_COLORS_BIT      = 16;
const int L_BONES_BIT       = 32;

const int NUM_MORPH_TARGETS = 2;

// vertex streams mapping (used as vertex shader inputs)
const int L_VS_VERTEX   = 0;  // vec3
const int L_VS_TEXCOORD = 1;  // vec4
const int L_VS_NORMAL   = 2;  // vec3
const int L_VS_TANGENT  = 3;  // vec3
const int L_VS_BINORMAL = 4;  // vec3
const int L_VS_COLORS   = 5;  // vec4
const int L_VS_BONES    = 6;  // vec2
const int L_VS_AUX0     = 7;  // vec4

const int L_VS_TOTAL_ATTRIBS = L_VS_AUX0 + 1;

/// number of float components in every stream
const int L_VS_VEC_COMPONENTS[ L_VS_TOTAL_ATTRIBS ] = { 3, 4, 3, 3, 3, 4, 2, 4 };

enum LPrimitiveType
{
	L_PT_POINT          = 0,
	L_PT_LINE           = 1,
	L_PT_LINE_STRIP     = 2,
	L_PT_TRIANGLE       = 3,
	L_PT_TRIANGLE_STRIP = 4
};

using Math::LPolyList;
using Math::LPoly;

/// Container for vertex attribs (think about it like a mesh without internal hierarchy)
class netexportable clVertexAttribs
{
public:
	clVertexAttribs();
	clVertexAttribs( size_t Vertices, int VertexTypeBits );
	virtual ~clVertexAttribs();

#pragma region Pack/unpack skeletal data into bitmap to allow GPU uploading
	virtual clBitmap*    GetMorphTargetsBitmap( sEnvironment* Env ) const;
#pragma endregion

#pragma region Caching in internal format
	virtual void    LoadFromStream( iIStream* Stream );
	virtual void    SaveToStream( iOStream* Stream ) const;
#pragma endregion

#pragma region Data allocation and access
	noexport void    SetIndices16( size_t Count, const Lushort* Indices );
	noexport void    SetIndices32( size_t Count, const Luint*   Indices );

	scriptmethod void    SetActiveVertexCount( size_t Count ) { FActiveVertexCount = Count; }
	scriptmethod size_t  GetActiveVertexCount() const { return FActiveVertexCount; }

	virtual void    SetKeyframe( sEnvironment* Env, int KeyframeNum, int NextKeyframeNum, float LerpCoef );

	virtual void                  AddSkeletonFrame( const clJointsSet& FrameJoints );
	virtual const clJointsSet&    GetSkeletonFrame( size_t i ) const;
	virtual size_t                GetSkeletonFramesCount() const;

	/// map all vertex attribs (vertices, texcoords, normals, etc) to L_VS_TOTAL_ATTRIBS vertex streams
	virtual const std::vector<const void*>&    EnumerateVertexStreams() const;
#pragma endregion

#pragma region Per-vertex generation
	scriptmethod void    Restart( LPrimitiveType Primitive, size_t ReserveVertices, int VertexTypeBits );
	scriptmethod void    EmitVertex( float X, float Y, float Z, int WeightStart, int WeightCount );
	scriptmethod void    EmitVertexV( LVector3 Vec, int WeightStart, int WeightCount );
	scriptmethod void    SetTexCoord( float U, float V, float W );
	scriptmethod void    SetTexCoordV2( const LVector2& V );
	scriptmethod void    SetTexCoordV3( const LVector3& V );
	scriptmethod void    SetTexCoordV4( const LVector4& V );
	scriptmethod void    SetNormalV( const LVector3& Vec );
	scriptmethod void    SetColorV( const LVector4& Vec );
	scriptmethod void    SplitTriangles();
#pragma endregion

#pragma region Data modification and regeneration

	/// Create VertexAttribs from the triangulated list of points
	noexport     void       CreateFromIndexedPoints( int NumPoints, LVector3* Points, LVector3* Normals, LVector4* TexCoords );

	/// Triangulate the polygon and add it to the VA
	scriptmethod void       AddPolygon( const LPoly& Poly );

	/// Triangulate poly list
	scriptmethod void       AddPolyList( const LPolyList& PolyList );

	/// Create VertexAttribs for the poly list
	scriptmethod void       CreateFromPolyList( const LPolyList& PolyList );

	/// Create VertexAttribs for the polygon
	scriptmethod void       CreateFromPolygon( const LPoly& Poly );

	/// Merge with another Vertex array
	scriptmethod bool       MergeWith( const clVertexAttribs* Other );

	/// Create a copy of VA
	scriptmethod clVertexAttribs* CloneVA() const;

#pragma region Per-vertex transformations

	/// Scale vertices
	scriptmethod void       Scale( float Coef );

	/// Shift vertices
	scriptmethod void       Translate( const LVector3& Vec );

	/// Apply transformation to each vertex
	scriptmethod void       Transform( const LMatrix4& Mtx );

	/// Normalize vertex coordinates (gaussian map) - map to unit sphere
	scriptmethod void       NormalizeVertices();

	/// LV: dunno what is does, no comments
	scriptmethod LVector3   TranslateToCenter();

	/// Transform texture coordinates with a given mtx4
	scriptmethod void       TransformTexCoords( const LMatrix4& Mtx );

#pragma endregion

#pragma region Bounding geometry

	/// Obvious
	scriptmethod LAABoundingBox  CalculateBoundingBox() const;

	/// Obvious
	scriptmethod LSphere         CalculateBoundingSphere() const;

	/// Precalculate bounding boxes for all frames
	scriptmethod void            PrecalculateBoundingBoxes();

	/// Get LABB for animated geometry, precalculation of per-frame LABBs is done if necessary
	scriptmethod LAABoundingBox  GetBoundingBox( size_t Keyframe );

	/// Caclulate interpolated LABB for animated geometry, precalculation of per-frame LABBs is done if necessary
	scriptmethod LAABoundingBox  GetBoundingBoxInterpolated( size_t Keyframe1, size_t Keyframe2, float Blend );

	void BuildOBBTree( LOBBTree& Tree, int MaxDepth );

#pragma endregion

#pragma region Texture mapping

	/// Calculate texture coordinates by projecting vertices to the (V1,V2) plane
	scriptmethod void MapPlanar( const LVector3& Start, const LVector3& V1, const LVector3& V2 );

	/**
	   \brief Calculate texture coordinates (TexU, TexV) using spherical mapping

	   x = center.x + r * sin(theta - thetaofs) * cos(phi - phiofs)
	   y = center.y + r * sin(theta - thetaofs) * sin(phi - phiofs)
	   z = center.z + r * cos(theta - thetaofs)

	   2 * Pi * TexU = Theta - ThetaOfs
	   Pi * TexV = Phi - PhiOfs

	   If Radius is <= 0 then Length(vertex) is used for normalization (r parameter)
	*/
	scriptmethod void MapSpherical( const LVector3& Center, float Radius, float PhiOffset, float ThetaOffset );

	/**
	   \brief Calculate texture coordinates (TexU, TexV) using cylindrical mapping

	   x = R * cos(theta - thetaofs)
	   y = R * sin(theta - thetaofs)
	   z = Z

	   2 * Pi * TexU = atan2( y / R, x / R )
	   TexV = Zcyl

	   If Radius is <= 0 then Length(vertex.x, vertex.y) is used for normalization (Rcyl parameter)
	*/
	scriptmethod void MapCylindrical( const LVector3& Base, float Radius, float ThetaOffset );

	/// LV: Some strange texcoord generation (even more, it is unused)
	scriptmethod void GenerateTexCoordsFromVertices( const LVector3& Coefs );

#pragma endregion

#pragma endregion

#pragma region Properties
	scriptmethod void    SetMotionBlur( bool MotionBlur ) { FMotionBlur = MotionBlur; };
	scriptmethod void    SetCastShadow( bool CastShadow ) { FCastShadow = CastShadow; };
	scriptmethod void    SetReceiveShadow( bool ReceiveShadow ) { FReceiveShadow = ReceiveShadow; };
	scriptmethod bool    GetMotionBlur() const { return FMotionBlur; };
	scriptmethod bool    GetCastShadow() const { return FCastShadow; };
	scriptmethod bool    GetReceiveShadow() const { return FReceiveShadow; };

	/// Get vertex attrib bit flags
	scriptmethod int     GetVertexBits() const { return FVTBits; }

#pragma endregion

#pragma region Triangle/Vertex iteration

	/// Determine the number of triangles in this vertex stream
	scriptmethod int     GetNumTriangles() const;

	/// Information about total vertex count
	scriptmethod int     GetNumVertices() const { return static_cast<int>( FVertices.size() ); }

	/// Get i-th triangle from array (handles internal indexing of triangles)
	scriptmethod void    GetTriangle( int idx, LVector3& V1, LVector3& V2, LVector3& V3 ) const;

	/// Create a linear list of vertices using GetTriangle() iterator
	scriptmethod void    CopyVerticesToArray( LArray<LVector3>& Out ) const;

#pragma endregion

#pragma region Miscelaneous routines

	/// Find clossest point on mesh, if there is an intersection with a mesh
	scriptmethod bool IntersectWithRay( const LVector3& P, const LVector3& A, LVector3& isect ) const;

	/// Find closest triangle intersecting with a given ray
	scriptmethod bool IntersectWithRayAndFindTriangle( const LVector3& P, const LVector3& A, LVector3& isect, int& TriangleIdx ) const;

#pragma endregion

	static clVertexAttribs*    CreateEmpty();
	static clVertexAttribs*    Create( size_t Vertices, int VertexTypeBits );
	static clVertexAttribs*    CreateFromTriangles( const std::vector<int>& Indices, const std::vector<LVector3>& Vertices, int VertexTypeBits );

	/// recalculate vertices positions according to skeleton joints
	void        CalcPoints( clJointsSet* Joints1, clJointsSet* Joints2, float Blend );
private:
	void        CalcPointsFromBitmap( sEnvironment* Env, int Frame1, int Frame2, float Blend );
	clBitmap*   PackSkeletonsToBitmap( sEnvironment* Env ) const;
public:
	/// type of geometry primitive to iterate indices/vertices
	LPrimitiveType             FPrimitiveType;

	/// true if data will be updated over time
	bool                       FDynamic;

	/// current number of acive vertices used for rendering
	size_t                     FActiveVertexCount;

	/// vertices processing order
	sAttribStream<Luint>       FIndices;

	/// vertex position X, Y, Z
	sAttribStream<LVector3>    FVertices;

	/// vertex tex coord S, T, R, Q
	sAttribStream<LVector4>    FTexCoords;

	/// vertex normal in object space
	sAttribStream<LVector3>    FNormals;

	/// vertex binormal in object space
	sAttribStream<LVector3>    FBinormals;

	/// vertex tangent in object space
	sAttribStream<LVector3>    FTangents;

	/// vertex RGBA colors
	sAttribStream<LVector4>    FColors;

	/**
	  Skeletal animation parameters. Joints reference
	  X - joint index, Y - joints count
	**/
	sAttribStream<LVector2>    FBlendingCoefs;

	/// collection of skeleton frames
	sAttribStream<clJointsSet>   FSkeletonFrames;

	/// collection of per-frame bounding boxes
	sAttribStream<LAABoundingBox> FBoundingBoxes;

	/// collection of joints weights
	sAttribStream<sWeight>     FWeights;

	bool                       FMotionBlur;
	bool                       FCastShadow;
	bool                       FReceiveShadow;
private:
	/// per-vertex generation
	LVector4    FCurrentTexCoords;
	LVector3    FCurrentNormal;
	LVector4    FCurrentColor;
	int         FVTBits;
	/// precached vertex streams
	mutable std::vector<const void*> FStreams;
	/// packed skeleton
	mutable clBitmap*   FMorphTargetsBitmap;
};

#endif

/*
 * 11/01/2011
     SplitTriangles()
     CreateFromTriangles()
 * 04/11/2010
     Fixed gcc issues with caching
 * 22/08/2010
     Fixed caching of skeleton frames
 * 01/08/2010
     PrecalculateBoundingBoxes()
 * 09/02/2010
     Interface extended
 * 08/02/2010
     It's here
*/

