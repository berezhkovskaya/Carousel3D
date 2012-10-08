/**
 * \file GeomServ.h
 * \brief Geometry server interface
 * \version 0.6.00
 * \date 27/01/2011
 * \author Sergey Kosarevsky, 2005-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clGeomServ_
#define _clGeomServ_

#include "Platform.h"
#include "Math/LVector.h"

#include <vector>

class clVertexAttribs;
class clJointsSet;

class netexportable clGeomServ
{
public:
	static void SplitTriangles( size_t NumTriangles,
	                            int Indices[],
	                            const LVector3 Vertices[],
	                            std::vector<int>* OutIndices,
	                            std::vector<LVector3>* OutVertices );

#pragma region Planar objects
	static clVertexAttribs* CreateTriangle2D( float vX, float vY,
	                                          float dX, float dY,
	                                          float Z );
	static clVertexAttribs* CreateRect2D( float X1, float Y1,
	                                      float X2, float Y2,
	                                      float Z, bool FlipTexCoordsVertical, int Subdivide );
	static clVertexAttribs* CreateBevelRect2D( float X1, float Y1,
	                                           float X2, float Y2,
	                                           float Z,
	                                           float Radius );
#pragma endregion

#pragma region Volumetric objects

	/// Add a new textured cube to vertex array (to avoid redundant scene items)
	static void AddAxisAlignedBox( clVertexAttribs* VA, const LVector3& Min, const LVector3& Max, bool Generate3DCoords );

	static void AddTube( clVertexAttribs* VA, int Segments, LVector3 Center, LVector3 Direction, float Radius, bool Cone );

	static void AddPlane( clVertexAttribs* VA, float SizeX, float SizeY, int SegmentsX, int SegmentsY, float Z );

	static void AddSphere( clVertexAttribs* VA, float Radius, LVector3 Center, int TessLevel );

	/**
	  Build a cylinder
	 \param Segments - number of subdivision of the base circle
	 \param Base - the "origin" vector
	 \param Direction - direction (and length) of the tube
	  \param Cone - if this param is true, then the upper circle is collapsed to point
	*/
	static clVertexAttribs* CreateTube( int Segments, LVector3 Base, LVector3 Direction, float Radius, bool Cone );

	/// Generate AABB using AddAxisAlignedBox()
	static clVertexAttribs* CreateAxisAlignedBox( const LVector3& Min, const LVector3& Max, bool Generate3DCoords );

	static clVertexAttribs* CreateSkyDome( float Radius, float dTheta, float dPhi, float VCoef, LVector3 Center );
	static clVertexAttribs* CreatePlane( float SizeX, float SizeY, int SegmentsX, int SegmentsY, float Z );
	static clVertexAttribs* CreateSphere( float Radius, LVector3 Center, int TessLevel );

	static clVertexAttribs* CreateTetrahedron( float Radius, LVector3 Center );

	static clVertexAttribs* CreateIcosahedron( float Radius, LVector3 Center );

	/// build an involute gear
	static clVertexAttribs* CreateGear( float BaseRadius, float RootRadius, float OuterRadius, float Width, int NumberOfTeeth, int InvoluteSteps );

	static clVertexAttribs* CreateISOGear( float Module, int NumberOfTeeth, float ProfileShift, float Width );

	/// Visual representation of a single pose
	static clVertexAttribs* CreateSkeletonPose( clJointsSet* Joints, float BaseBoneRadius, float JointRadius );

#pragma endregion
};

#endif

/*
 * 27/01/2011
     CreateGear()
 * 11/01/2011
     CreateTetrahedron()
     CreateIcosahedron()
 * 30/09/2010
     CreateSphere()
     SplitTriangles()
 * 26/07/2010
     CreateTube()
 * 12/02/2010
     Refactored
 * 21/11/2005
     CreateSkyDome()
 * 25/01/2005
     It's here
*/
