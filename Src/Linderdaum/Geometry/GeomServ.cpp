/**
 * \file GeomServ.cpp
 * \brief Geometry server implementation
 * \version 0.6.00
 * \date 27/01/2011
 * \author Sergey Kosarevsky, 2005-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "GeomServ.h"

#include "Core/Linker.h"
#include "Math/LVector.h"
#include "Math/LGeomUtils.h"
#include "Math/LGears.h"
#include "Geometry/Joints.h"

#include "VertexAttribs.h"

//(vX,vY) dX
//    |---/
//    |  /
// dY | /
//    |/
clVertexAttribs* clGeomServ::CreateTriangle2D( float vX, float vY,
                                               float dX, float dY,
                                               float Z )
{
	clVertexAttribs* VA = clVertexAttribs::CreateEmpty();

	VA->Restart( L_PT_TRIANGLE, 3, L_TEXCOORDS_BIT | L_NORMALS_BIT );

	VA->SetNormalV( LVector3( 0, 0, 1 ) );

	VA->SetTexCoord( 1, 1, 0 );
	VA->EmitVertexV( LVector3( vX   , vY   , Z ), -1, -1 );
	VA->SetTexCoord( 1, 0, 0 );
	VA->EmitVertexV( LVector3( vX   , vY - dY, Z ), -1, -1 );
	VA->SetTexCoord( 0, 1, 0 );
	VA->EmitVertexV( LVector3( vX + dX, vY   , Z ), -1, -1 );

	return VA;
}

float Coord( float A, float A1, float A2 )
{
	return ( A - A1 ) / ( A2 - A1 );
}

clVertexAttribs* clGeomServ::CreateRect2D( float X1, float Y1,
                                           float X2, float Y2,
                                           float Z, bool FlipTexCoordsVertical, int Subdivide )
{
	int SegmentsX = Subdivide + 1;
	int SegmentsY = Subdivide + 1;

	bool Flip = FlipTexCoordsVertical;

	clVertexAttribs* VA = clVertexAttribs::CreateEmpty();

	VA->Restart( L_PT_TRIANGLE, SegmentsX * SegmentsY * 6, L_TEXCOORDS_BIT | L_NORMALS_BIT );

	VA->SetNormalV( LVector3( 0, 0, 1 ) );


	float dX = ( X2 - X1 ) / static_cast<float>( SegmentsX );
	float dY = ( Y2 - Y1 ) / static_cast<float>( SegmentsY );

	for ( int i = 0; i != SegmentsX; ++i )
	{
		for ( int j = 0; j != SegmentsY; ++j )
		{
			float U1 = Coord( X1 + dX * i, X1, X2 );
			float U2 = Coord( X1 + dX * i, X1, X2 );
			float U3 = Coord( X1 + dX * ( i + 1 ), X1, X2 );
			float U4 = Coord( X1 + dX * ( i + 1 ), X1, X2 );
			float U5 = Coord( X1 + dX * i, X1, X2 );
			float U6 = Coord( X1 + dX * ( i + 1 ), X1, X2 );

			float V1 = Coord( Y1 + dY * j, Y1, Y2 );
			float V2 = Coord( Y1 + dY * ( j + 1 ), Y1, Y2 );
			float V3 = Coord( Y1 + dY * j, Y1, Y2 );
			float V4 = Coord( Y1 + dY * j, Y1, Y2 );
			float V5 = Coord( Y1 + dY * ( j + 1 ), Y1, Y2 );
			float V6 = Coord( Y1 + dY * ( j + 1 ), Y1, Y2 );

			VA->SetTexCoord( U1, Flip ? 1.0f - V1 : V1, 0 );
			VA->EmitVertex( X1 + dX * i,     Y1 + dY * j,     Z, -1, -1 );
			VA->SetTexCoord( U2, Flip ? 1.0f - V2 : V2, 0 );
			VA->EmitVertex( X1 + dX * i,     Y1 + dY * ( j + 1 ), Z, -1, -1 );
			VA->SetTexCoord( U3, Flip ? 1.0f - V3 : V3, 0 );
			VA->EmitVertex( X1 + dX * ( i + 1 ), Y1 + dY * j,     Z, -1, -1 );

			VA->SetTexCoord( U4, Flip ? 1.0f - V4 : V4, 0 );
			VA->EmitVertex( X1 + dX * ( i + 1 ), Y1 + dY * j,     Z, -1, -1 );
			VA->SetTexCoord( U5, Flip ? 1.0f - V5 : V5, 0 );
			VA->EmitVertex( X1 + dX * i,     Y1 + dY * ( j + 1 ), Z, -1, -1 );
			VA->SetTexCoord( U6, Flip ? 1.0f - V6 : V6, 0 );
			VA->EmitVertex( X1 + dX * ( i + 1 ), Y1 + dY * ( j + 1 ), Z, -1, -1 );
		}
	}

	return VA;
}

//   8______________7
//  9/              \6
//10/                \5
//  |                |
//  |                |
//  |                |
//11\                /4
// 12\______________/3
//   1              2
// unite vertices with triangles
// clock-wise
clVertexAttribs* clGeomServ::CreateBevelRect2D( float X1, float Y1,
                                                float X2, float Y2,
                                                float Z,
                                                float Radius )
{
	clVertexAttribs* VA = clVertexAttribs::CreateEmpty();

#define SpawnPoint(X, Y) VA->SetTexCoord( \
                                          ((X)-X1)/(X2-X1), \
                                          (Y1-(Y))/(Y1-Y2),0); \
                         VA->EmitVertex((X), (Y), (Z), -1, -1 );

	VA->Restart( L_PT_TRIANGLE, 30, L_TEXCOORDS_BIT | L_NORMALS_BIT );

	VA->SetNormalV( LVector3( 0, 0, 1 ) );

	float RR = Radius * ( 1 - 0.707106f );

	//1-3-2
	SpawnPoint( X1 + Radius, Y2 );
	SpawnPoint( X2 - RR, Y2 - RR );
	SpawnPoint( X2 - Radius, Y2 );

	//1-12-3
	SpawnPoint( X1 + Radius, Y2 );
	SpawnPoint( X1 + RR, Y2 - RR );
	SpawnPoint( X2 - RR, Y2 - RR );

	//3-6-4
	SpawnPoint( X2 - RR, Y2 - RR );
	SpawnPoint( X2 - RR, Y1 + RR );
	SpawnPoint( X2, Y2 - Radius );

	//4-6-5
	SpawnPoint( X2, Y2 - Radius );
	SpawnPoint( X2 - RR, Y1 + RR );
	SpawnPoint( X2, Y1 + Radius );

	//6-9-7
	SpawnPoint( X2 - RR, Y1 + RR );
	SpawnPoint( X1 + RR, Y1 + RR );
	SpawnPoint( X2 - Radius, Y1 );

	//7-9-8
	SpawnPoint( X2 - Radius, Y1 );
	SpawnPoint( X1 + RR, Y1 + RR );
	SpawnPoint( X1 + Radius, Y1 );

	//9-12-11
	SpawnPoint( X1 + RR, Y1 + RR );
	SpawnPoint( X1 + RR, Y2 - RR );
	SpawnPoint( X1, Y2 - Radius );

	//9-11-10
	SpawnPoint( X1 + RR, Y1 + RR );
	SpawnPoint( X1, Y2 - Radius );
	SpawnPoint( X1, Y1 + Radius );

	//12-9-6
	SpawnPoint( X1 + RR, Y2 - RR );
	SpawnPoint( X1 + RR, Y1 + RR );
	SpawnPoint( X2 - RR, Y1 + RR );

	//12-6-3
	SpawnPoint( X1 + RR, Y2 - RR );
	SpawnPoint( X2 - RR, Y1 + RR );
	SpawnPoint( X2 - RR, Y2 - RR );
#undef SpawnPoint

	return VA;
}

void clGeomServ::AddPlane( clVertexAttribs* VA, float SizeX,
                           float SizeY,
                           int SegmentsX,
                           int SegmentsY,
                           float Z )
{
	VA->SetNormalV( LVector3( 0, 0, 1 ) );

	float X1 = -SizeX * 0.5f;
	float Y1 = -SizeY * 0.5f;

	float dX = SizeX / static_cast<float>( SegmentsX );
	float dY = SizeY / static_cast<float>( SegmentsY );

	for ( int i = 0; i != SegmentsX; ++i )
	{
		for ( int j = 0; j != SegmentsY; ++j )
		{
			VA->EmitVertex( X1 + dX * i,     Y1 + dY * j,     Z, -1, -1 );
			VA->EmitVertex( X1 + dX * i,     Y1 + dY * ( j + 1 ), Z, -1, -1 );
			VA->EmitVertex( X1 + dX * ( i + 1 ), Y1 + dY * j,     Z, -1, -1 );

			VA->EmitVertex( X1 + dX * ( i + 1 ), Y1 + dY * j,     Z, -1, -1 );
			VA->EmitVertex( X1 + dX * i,     Y1 + dY * ( j + 1 ), Z, -1, -1 );
			VA->EmitVertex( X1 + dX * ( i + 1 ), Y1 + dY * ( j + 1 ), Z, -1, -1 );
		}
	}
}

clVertexAttribs* clGeomServ::CreatePlane( float SizeX,
                                          float SizeY,
                                          int SegmentsX,
                                          int SegmentsY,
                                          float Z )
{
	clVertexAttribs* VA = clVertexAttribs::CreateEmpty();

	VA->Restart( L_PT_TRIANGLE, SegmentsX * SegmentsY * 6, L_TEXCOORDS_BIT | L_NORMALS_BIT );

	AddPlane( VA, SizeX, SizeY, SegmentsX, SegmentsY, Z );
	/*
	   VA->SetNormalV( LVector3( 0, 0, 1 ) );

	   float X1 = -SizeX * 0.5f;
	   float Y1 = -SizeY * 0.5f;

	   float dX = SizeX / static_cast<float>( SegmentsX );
	   float dY = SizeY / static_cast<float>( SegmentsY );

	   for ( int i = 0; i != SegmentsX; ++i )
	   {
	      for ( int j = 0; j != SegmentsY; ++j )
	      {
	         VA->EmitVertex( X1 + dX * i,     Y1 + dY * j,     Z, -1, -1 );
	         VA->EmitVertex( X1 + dX * i,     Y1 + dY * ( j + 1 ), Z, -1, -1 );
	         VA->EmitVertex( X1 + dX * ( i + 1 ), Y1 + dY * j,     Z, -1, -1 );

	         VA->EmitVertex( X1 + dX * ( i + 1 ), Y1 + dY * j,     Z, -1, -1 );
	         VA->EmitVertex( X1 + dX * i,     Y1 + dY * ( j + 1 ), Z, -1, -1 );
	         VA->EmitVertex( X1 + dX * ( i + 1 ), Y1 + dY * ( j + 1 ), Z, -1, -1 );
	      }
	   }
	*/
	return VA;
}

void clGeomServ::AddTube( clVertexAttribs* VA, int Segments, LVector3 Center, LVector3 Direction, float Radius, bool Cone )
{
	// not an optimal generation, I just forgot the API once again
//	VA->Restart( L_PT_TRIANGLE, Segments * 6, L_TEXCOORDS_BIT | L_NORMALS_BIT );

	VA->SetNormalV( LVector3( 0, 0, 1 ) );

	float dA = 2.0f * Math::PI / static_cast<float>( Segments - 1 );

	float Angle = 0.0f;

	LVector3 V1, V2;
	BuildComplementaryBasis( Direction, V1, V2 );

	TODO( "store previous values, do not use sin/cos - use multiples of sin(dA), cos(dA)" )

	for ( int i = 0; i < Segments ; i++, Angle += dA )
	{
		// 0,1 - bottom
		// 2,3 - top

		float RCA1 = Radius * cos( Angle );
		float RSA1 = Radius * sin( Angle );

		float RCA2 = Radius * cos( Angle + dA );
		float RSA2 = Radius * sin( Angle + dA );

		LVector3 p0 = Center + V1 * RSA1 + V2 * RCA1;
		LVector3 p1 = Center + V1 * RSA2 + V2 * RCA2;

		LVector3 p2 = Cone ? ( Center + Direction ) : ( p0 + Direction );
		LVector3 p3 = Cone ? ( Center + Direction ) : ( p1 + Direction );

		VA->EmitVertexV( p0, -1, -1 );
		VA->EmitVertexV( p1, -1, -1 );
		VA->EmitVertexV( p2, -1, -1 );

		VA->EmitVertexV( p1, -1, -1 );
		VA->EmitVertexV( p2, -1, -1 );
		VA->EmitVertexV( p3, -1, -1 );
	}
}

clVertexAttribs* clGeomServ::CreateTube( int Segments, LVector3 Center, LVector3 Direction, float Radius, bool Cone )
{
	clVertexAttribs* VA = clVertexAttribs::CreateEmpty();

	// not an optimal generation, I just forgot the API once again
	VA->Restart( L_PT_TRIANGLE, Segments * 6, L_TEXCOORDS_BIT | L_NORMALS_BIT );

	AddTube( VA, Segments, Center, Direction, Radius, Cone );
	/*
	   VA->SetNormalV( LVector3( 0, 0, 1 ) );

	   float dA = 2.0f * Math::PI / static_cast<float>( Segments - 1 );

	   float Angle = 0.0f;

	   LVector3 V1, V2;
	   BuildComplementaryBasis( Direction, V1, V2 );

	   TODO( "store previous values, do not use sin/cos - use multiples of sin(dA), cos(dA)" )

	   for ( int i = 0; i < Segments ; i++, Angle += dA )
	   {
	      // 0,1 - bottom
	      // 2,3 - top

	      float RCA1 = Radius * cos( Angle );
	      float RSA1 = Radius * sin( Angle );

	      float RCA2 = Radius * cos( Angle + dA );
	      float RSA2 = Radius * sin( Angle + dA );

	      LVector3 p0 = Center + V1 * RSA1 + V2 * RCA1;
	      LVector3 p1 = Center + V1 * RSA2 + V2 * RCA2;

	      LVector3 p2 = Cone ? ( Center + Direction ) : ( p0 + Direction );
	      LVector3 p3 = Cone ? ( Center + Direction ) : ( p1 + Direction );

	      VA->EmitVertexV( p0, -1, -1 );
	      VA->EmitVertexV( p1, -1, -1 );
	      VA->EmitVertexV( p2, -1, -1 );

	      VA->EmitVertexV( p1, -1, -1 );
	      VA->EmitVertexV( p2, -1, -1 );
	      VA->EmitVertexV( p3, -1, -1 );
	   }
	*/
	return VA;
}

const int NUM_CUBE_VTX = 36;

void clGeomServ::AddAxisAlignedBox( clVertexAttribs* VA, const LVector3& Min, const LVector3& Max, bool Generate3DCoords )
{
	//  0 --- 1
	//  | \   |
	//  |  \  |   for each side we generate (0,1,2) and (0,2,3) triangles
	//  |   \ |
	//  3 --- 2

	// Triangles go in order to produce correct normal facing
//	VA->Restart( L_PT_TRIANGLE, NUM_CUBE_VTX, L_TEXCOORDS_BIT | L_NORMALS_BIT );

	/*
	   // this almost works, not counting the strange top-bottom texture coordinates

	   LVector3 CubeNormals[6] =
	   {
	   // top
	   LVector3(0, 0, 1),
	    // bottom
	    LVector3(0, 0, -1),
	    // left
	    LVector3(0, 1, 0),
	      // right
	    LVector3(0, -1, 0),
	      // front
	    LVector3(1, 0, 0),
	    // back
	   LVector3(-1, 0, 0)
	   };

	   LVector3 CubeTex[NUM_CUBE_VTX] = {
	      LVector3(0, 0, 1),LVector3(1, 0, 1),LVector3(1, 1, 1),
	      LVector3(1, 1, 1),LVector3(0, 1, 1),LVector3(0, 0, 1),
	      LVector3(0, 0, 0),LVector3(0, 1, 0),LVector3(1, 1, 0),
	     LVector3(1, 1, 0),LVector3(1, 0, 0),LVector3(0, 0, 0),
	     LVector3(1, 1, 0),LVector3(0, 1, 0),LVector3(0, 1, 1),
	      LVector3(0, 1, 1),LVector3(1, 1, 1),LVector3(1, 1, 0),
	      LVector3(0, 0, 0),LVector3(1, 0, 0),LVector3(1, 0, 1),
	      LVector3(1, 0, 1),LVector3(0, 0, 1),LVector3(0, 0, 0),
	      LVector3(1, 0, 0),LVector3(1, 1, 0),LVector3(1, 1, 1),
	      LVector3(1, 1, 1),LVector3(1, 0, 1),LVector3(1, 0, 0),
	      LVector3(0, 1, 0),LVector3(0, 0, 0),LVector3(0, 0, 1),
	      LVector3(0, 0, 1),LVector3(0, 1, 1),LVector3(0, 1, 0)
	   };

	   LVector3 CubeVtx[NUM_CUBE_VTX] = {
	      LVector3(Min.X, Min.Y, Max.Z), LVector3(Max.X, Min.Y, Max.Z), LVector3(Max.X, Max.Y, Max.Z),
	      LVector3(Max.X, Max.Y, Max.Z), LVector3(Min.X, Max.Y, Max.Z), LVector3(Min.X, Min.Y, Max.Z),
	      LVector3(Min.X, Min.Y, Min.Z), LVector3(Min.X, Max.Y, Min.Z), LVector3(Max.X, Max.Y, Min.Z),
	      LVector3(Max.X, Max.Y, Min.Z), LVector3(Max.X, Min.Y, Min.Z), LVector3(Min.X, Min.Y, Min.Z),
	      LVector3(Max.X, Max.Y, Min.Z), LVector3(Min.X, Max.Y, Min.Z), LVector3(Min.X, Max.Y, Max.Z),
	      LVector3(Min.X, Max.Y, Max.Z), LVector3(Max.X, Max.Y, Max.Z), LVector3(Max.X, Max.Y, Min.Z),
	      LVector3(Min.X, Min.Y, Min.Z), LVector3(Max.X, Min.Y, Min.Z), LVector3(Max.X, Min.Y, Max.Z),
	      LVector3(Max.X, Min.Y, Max.Z), LVector3(Min.X, Min.Y, Max.Z), LVector3(Min.X, Min.Y, Min.Z),
	      LVector3(Max.X, Min.Y, Min.Z), LVector3(Max.X, Max.Y, Min.Z), LVector3(Max.X, Max.Y, Max.Z),
	      LVector3(Max.X, Max.Y, Max.Z), LVector3(Max.X, Min.Y, Max.Z), LVector3(Max.X, Min.Y, Min.Z),
	      LVector3(Min.X, Max.Y, Min.Z), LVector3(Min.X, Min.Y, Min.Z), LVector3(Min.X, Min.Y, Max.Z),
	      LVector3(Min.X, Min.Y, Max.Z), LVector3(Min.X, Max.Y, Max.Z), LVector3(Min.X, Max.Y, Min.Z)
	   };

	   // faces
	   LVector3 TT;
	   int Ofs = 0;

	   for (int i = 0 ; i < 6 ; i++)
	   {
	       VA->SetNormalV( CubeNormals[i] );
	      // pair of triangles
	      for (int j = 0 ; j < 6 ; j++)
	      {
	         TT = CubeTex[i * 6 + j];
	        VA->SetTexCoord( TT.X, TT.Y, Generate3DCoords ? TT.Z : 0.0f );
	        VA->EmitVertexV( CubeVtx[Ofs] , -1, -1 );

	        Ofs++;
	      }
	   }

	*/

	TODO( "Remove this copy'n'paste shit" )

	if ( Generate3DCoords )
	{
		// top
		VA->SetNormalV( LVector3( 0, 0, 1 ) );
		VA->SetTexCoord( 0, 0, 1 );
		VA->EmitVertex( Min.X, Min.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 1, 0, 1 );
		VA->EmitVertex( Max.X, Min.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 1, 1, 1 );
		VA->EmitVertex( Max.X, Max.Y, Max.Z, -1, -1 );

		VA->SetTexCoord( 1, 1, 1 );
		VA->EmitVertex( Max.X, Max.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 0, 1, 1 );
		VA->EmitVertex( Min.X, Max.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 0, 0, 1 );
		VA->EmitVertex( Min.X, Min.Y, Max.Z, -1, -1 );

		// bottom
		VA->SetNormalV( LVector3( 0, 0, -1 ) );
		VA->SetTexCoord( 0, 0, 0 );
		VA->EmitVertex( Min.X, Min.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 0, 1, 0 );
		VA->EmitVertex( Min.X, Max.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 1, 1, 0 );
		VA->EmitVertex( Max.X, Max.Y, Min.Z, -1, -1 );

		VA->SetTexCoord( 1, 1, 0 );
		VA->EmitVertex( Max.X, Max.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 1, 0, 0 );
		VA->EmitVertex( Max.X, Min.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 0, 0, 0 );
		VA->EmitVertex( Min.X, Min.Y, Min.Z, -1, -1 );

		// left
		VA->SetNormalV( LVector3( 0, 1, 0 ) );
		VA->SetTexCoord( 1, 1, 0 );
		VA->EmitVertex( Max.X, Max.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 0, 1, 0 );
		VA->EmitVertex( Min.X, Max.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 0, 1, 1 );
		VA->EmitVertex( Min.X, Max.Y, Max.Z, -1, -1 );

		VA->SetTexCoord( 0, 1, 1 );
		VA->EmitVertex( Min.X, Max.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 1, 1, 1 );
		VA->EmitVertex( Max.X, Max.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 1, 1, 0 );
		VA->EmitVertex( Max.X, Max.Y, Min.Z, -1, -1 );

		// right
		VA->SetNormalV( LVector3( 0, -1, 0 ) );
		VA->SetTexCoord( 0, 0, 0 );
		VA->EmitVertex( Min.X, Min.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 1, 0, 0 );
		VA->EmitVertex( Max.X, Min.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 1, 0, 1 );
		VA->EmitVertex( Max.X, Min.Y, Max.Z, -1, -1 );

		VA->SetTexCoord( 1, 0, 1 );
		VA->EmitVertex( Max.X, Min.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 0, 0, 1 );
		VA->EmitVertex( Min.X, Min.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 0, 0, 0 );
		VA->EmitVertex( Min.X, Min.Y, Min.Z, -1, -1 );

		// front
		VA->SetNormalV( LVector3( 1, 0, 0 ) );
		VA->SetTexCoord( 1, 0, 0 );
		VA->EmitVertex( Max.X, Min.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 1, 1, 0 );
		VA->EmitVertex( Max.X, Max.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 1, 1, 1 );
		VA->EmitVertex( Max.X, Max.Y, Max.Z, -1, -1 );

		VA->SetTexCoord( 1, 1, 1 );
		VA->EmitVertex( Max.X, Max.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 1, 0, 1 );
		VA->EmitVertex( Max.X, Min.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 1, 0, 0 );
		VA->EmitVertex( Max.X, Min.Y, Min.Z, -1, -1 );

		// back
		VA->SetNormalV( LVector3( -1, 0, 0 ) );
		VA->SetTexCoord( 0, 1, 0 );
		VA->EmitVertex( Min.X, Max.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 0, 0, 0 );
		VA->EmitVertex( Min.X, Min.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 0, 0, 1 );
		VA->EmitVertex( Min.X, Min.Y, Max.Z, -1, -1 );

		VA->SetTexCoord( 0, 0, 1 );
		VA->EmitVertex( Min.X, Min.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 0, 1, 1 );
		VA->EmitVertex( Min.X, Max.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 0, 1, 0 );
		VA->EmitVertex( Min.X, Max.Y, Min.Z, -1, -1 );
	}
	else
	{
		// top
		VA->SetNormalV( LVector3( 0, 0, 1 ) );
		VA->SetTexCoord( 0, 0, 0 );
		VA->EmitVertex( Min.X, Min.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 1, 0, 0 );
		VA->EmitVertex( Max.X, Min.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 1, 1, 0 );
		VA->EmitVertex( Max.X, Max.Y, Max.Z, -1, -1 );

		VA->SetTexCoord( 1, 1, 0 );
		VA->EmitVertex( Max.X, Max.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 0, 1, 0 );
		VA->EmitVertex( Min.X, Max.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 0, 0, 0 );
		VA->EmitVertex( Min.X, Min.Y, Max.Z, -1, -1 );

		// bottom
		VA->SetNormalV( LVector3( 0, 0, -1 ) );
		VA->SetTexCoord( 0, 0, 0 );
		VA->EmitVertex( Min.X, Min.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 0, 1, 0 );
		VA->EmitVertex( Min.X, Max.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 1, 1, 0 );
		VA->EmitVertex( Max.X, Max.Y, Min.Z, -1, -1 );

		VA->SetTexCoord( 1, 1, 0 );
		VA->EmitVertex( Max.X, Max.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 1, 0, 0 );
		VA->EmitVertex( Max.X, Min.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 0, 0, 0 );
		VA->EmitVertex( Min.X, Min.Y, Min.Z, -1, -1 );

		// left
		VA->SetNormalV( LVector3( 0, 1, 0 ) );
		VA->SetTexCoord( 1, 0, 0 );
		VA->EmitVertex( Max.X, Max.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 0, 0, 0 );
		VA->EmitVertex( Min.X, Max.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 0, 1, 0 );
		VA->EmitVertex( Min.X, Max.Y, Max.Z, -1, -1 );

		VA->SetTexCoord( 0, 1, 0 );
		VA->EmitVertex( Min.X, Max.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 1, 1, 0 );
		VA->EmitVertex( Max.X, Max.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 1, 0, 0 );
		VA->EmitVertex( Max.X, Max.Y, Min.Z, -1, -1 );

		// right
		VA->SetNormalV( LVector3( 0, -1, 0 ) );
		VA->SetTexCoord( 0, 0, 0 );
		VA->EmitVertex( Min.X, Min.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 1, 0, 0 );
		VA->EmitVertex( Max.X, Min.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 1, 1, 0 );
		VA->EmitVertex( Max.X, Min.Y, Max.Z, -1, -1 );

		VA->SetTexCoord( 1, 1, 0 );
		VA->EmitVertex( Max.X, Min.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 0, 1, 0 );
		VA->EmitVertex( Min.X, Min.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 0, 0, 0 );
		VA->EmitVertex( Min.X, Min.Y, Min.Z, -1, -1 );

		// front
		VA->SetNormalV( LVector3( 1, 0, 0 ) );
		VA->SetTexCoord( 0, 0, 0 );
		VA->EmitVertex( Max.X, Min.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 1, 0, 0 );
		VA->EmitVertex( Max.X, Max.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 1, 1, 0 );
		VA->EmitVertex( Max.X, Max.Y, Max.Z, -1, -1 );

		VA->SetTexCoord( 1, 1, 0 );
		VA->EmitVertex( Max.X, Max.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 0, 1, 0 );
		VA->EmitVertex( Max.X, Min.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 0, 0, 0 );
		VA->EmitVertex( Max.X, Min.Y, Min.Z, -1, -1 );

		// back
		VA->SetNormalV( LVector3( -1, 0, 0 ) );
		VA->SetTexCoord( 1, 0, 0 );
		VA->EmitVertex( Min.X, Max.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 0, 0, 0 );
		VA->EmitVertex( Min.X, Min.Y, Min.Z, -1, -1 );
		VA->SetTexCoord( 0, 1, 0 );
		VA->EmitVertex( Min.X, Min.Y, Max.Z, -1, -1 );

		VA->SetTexCoord( 0, 1, 0 );
		VA->EmitVertex( Min.X, Min.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 1, 1, 0 );
		VA->EmitVertex( Min.X, Max.Y, Max.Z, -1, -1 );
		VA->SetTexCoord( 1, 0, 0 );
		VA->EmitVertex( Min.X, Max.Y, Min.Z, -1, -1 );
	}
}

void clGeomServ::AddSphere( clVertexAttribs* VA, float Radius, LVector3 Center, int TessLevel )
{
	clVertexAttribs* Sphere = CreateSphere( Radius, Center, TessLevel );

	VA->MergeWith( Sphere );

//	delete Sphere;
}

clVertexAttribs* clGeomServ::CreateAxisAlignedBox( const LVector3& Min, const LVector3& Max, bool Generate3DCoords )
{
	clVertexAttribs* VA = clVertexAttribs::CreateEmpty();

	// Triangles go in order to produce correct normal facing
	VA->Restart( L_PT_TRIANGLE, NUM_CUBE_VTX, L_TEXCOORDS_BIT | L_NORMALS_BIT );

	AddAxisAlignedBox( VA, Min, Max, Generate3DCoords );

	return VA;
}

int CalculateSkyDomeVerticesCount( float dTheta, float dPhi )
{
	return static_cast<int>( ( 360 / dTheta ) * ( 90 / dPhi ) * 4 );
}

clVertexAttribs* clGeomServ::CreateSkyDome( float Radius,
                                            float dTheta,
                                            float dPhi,
                                            float VCoef,
                                            LVector3 Center )
{
#define SpawnVertex(Vec) VA->SetTexCoord( (Vec).X, (Vec).Y, 0 ); \
                         VA->SetNormalV( Center - (Vec) ); \
                         TempVec = ( (Vec) * Radius + Center ); \
                         TempVec.Z *= VCoef; \
                         VA->EmitVertexV( TempVec, -1, -1  );

	clVertexAttribs* VA = clVertexAttribs::CreateEmpty();

	VA->Restart( L_PT_TRIANGLE_STRIP, CalculateSkyDomeVerticesCount( dTheta, dPhi ), L_TEXCOORDS_BIT | L_NORMALS_BIT );


	LVector3 TempVec;

	float Phi   = 0;

	while ( Phi <= 90.0f - dPhi )
	{
		float Theta = 0;

		while ( Theta <= 360 - dTheta )
		{
			Theta += dTheta;

			// Calculate the vertex at phi, theta
			LVector3 V1( sin( Phi * Math::DTOR )*cos( Math::DTOR * Theta ),
			             sin( Phi * Math::DTOR )*sin( Math::DTOR * Theta ),
			             cosf( Phi * Math::DTOR ) );
			SpawnVertex( V1 );

			// Calculate the vertex at phi+dphi, theta
			LVector3 V2( sin( ( Phi + dPhi )*Math::DTOR ) * cos( Theta * Math::DTOR ),
			             sin( ( Phi + dPhi )*Math::DTOR ) * sin( Theta * Math::DTOR ),
			             cosf( ( Phi + dPhi )*Math::DTOR ) );
			SpawnVertex( V2 );

			// Calculate the vertex at phi, theta+dtheta
			LVector3 V3( sin( Math::DTOR * Phi ) * cos( Math::DTOR*( Theta + dTheta ) ),
			             sin( Math::DTOR * Phi ) * sin( Math::DTOR*( Theta + dTheta ) ),
			             cosf( Math::DTOR * Phi ) );
			SpawnVertex( V3 );

			if ( ( Phi > -90 ) && ( Phi < 90 ) )
			{
				// Calculate the vertex at phi+dphi, theta+dtheta
				LVector3 V4( sin( ( Phi + dPhi )*Math::DTOR ) * cos( Math::DTOR*( Theta + dTheta ) ),
				             sin( ( Phi + dPhi )*Math::DTOR ) * sin( Math::DTOR*( Theta + dTheta ) ),
				             cosf( ( Phi + dPhi )*Math::DTOR ) );
				SpawnVertex( V4 );
			}
		}

		Phi += dPhi;
	}

#undef SpawnVertex

	return VA;
}

void clGeomServ::SplitTriangles( size_t NumTriangles,
                                 int Indices[],
                                 const LVector3 Vertices[],
                                 std::vector<int>* OutIndices,
                                 std::vector<LVector3>* OutVertices )
{
	/*
	       V1
	       /\
	   V31/__\V12
	     /\  /\
	  V3/__\/__\V2
	      V23

	*/
	int* Index = &Indices[0];

	int OutIndex = 0;

	OutIndices->clear();
	OutVertices->clear();

	for ( size_t i = 0; i != NumTriangles; i++ )
	{
		LVector3 V1 = Vertices[ *Index++ ];
		LVector3 V2 = Vertices[ *Index++ ];
		LVector3 V3 = Vertices[ *Index++ ];

		// calculate split vertices
		LVector3 V12 = 0.5f * ( V1 + V2 );
		LVector3 V23 = 0.5f * ( V2 + V3 );
		LVector3 V31 = 0.5f * ( V3 + V1 );

		// generate 6 vertices
		OutVertices->push_back( V1 );  // 0
		OutVertices->push_back( V2 );  // 1
		OutVertices->push_back( V3 );  // 2
		OutVertices->push_back( V12 ); // 3
		OutVertices->push_back( V23 ); // 4
		OutVertices->push_back( V31 ); // 5

		// generate 4 'out' triangles for every 'in' triangle: 0-3-5, 1-4-3, 4-2-5, 3-4-5
		OutIndices->push_back( OutIndex + 0 );
		OutIndices->push_back( OutIndex + 3 );
		OutIndices->push_back( OutIndex + 5 );

		OutIndices->push_back( OutIndex + 1 );
		OutIndices->push_back( OutIndex + 4 );
		OutIndices->push_back( OutIndex + 3 );

		OutIndices->push_back( OutIndex + 4 );
		OutIndices->push_back( OutIndex + 2 );
		OutIndices->push_back( OutIndex + 5 );

		OutIndices->push_back( OutIndex + 3 );
		OutIndices->push_back( OutIndex + 4 );
		OutIndices->push_back( OutIndex + 5 );

		OutIndex += 6;
	}
}

clVertexAttribs* clGeomServ::CreateTetrahedron( float Radius, LVector3 Center )
{
	LVector3 V0( cos(   0.0f * Math::DTOR ), sin(   0.0f * Math::DTOR ), 0.0f );
	LVector3 V1( cos( 120.0f * Math::DTOR ), sin( 120.0f * Math::DTOR ), 0.0f );
	LVector3 V2( cos( 240.0f * Math::DTOR ), sin( 240.0f * Math::DTOR ), 0.0f );
	LVector3 V3( 0.0f,  0.0f, sqrt( 2.0f / 3.0f ) * ( V1 - V0 ).Length() );

	V0 *= Radius;
	V1 *= Radius;
	V2 *= Radius;
	V3 *= Radius;

	const LVector3 C = ( V0 + V1 + V2 + V3 ) / 4.0f - Center;

	V0 -= C;
	V1 -= C;
	V2 -= C;
	V3 -= C;

	std::vector<int>      Indices;
	std::vector<LVector3> Vertices;

	Vertices.push_back( V0 );
	Vertices.push_back( V1 );
	Vertices.push_back( V2 );
	Vertices.push_back( V3 );

	Indices.push_back( 0 );
	Indices.push_back( 1 );
	Indices.push_back( 2 );
	Indices.push_back( 0 );
	Indices.push_back( 3 );
	Indices.push_back( 2 );
	Indices.push_back( 1 );
	Indices.push_back( 3 );
	Indices.push_back( 0 );
	Indices.push_back( 2 );
	Indices.push_back( 3 );
	Indices.push_back( 1 );

	clVertexAttribs* VA = clVertexAttribs::CreateFromTriangles( Indices, Vertices, L_TEXCOORDS_BIT | L_NORMALS_BIT );

	return VA;
}

clVertexAttribs* clGeomServ::CreateIcosahedron( float Radius, LVector3 Center )
{
	const float X = 0.525731112119133606f * Radius;
	const float Z = 0.850650808352039932f * Radius;

	std::vector<int>      Indices;
	std::vector<LVector3> V;

	const LVector3 C( Center );

	V.push_back( C + LVector3( -X, 0.0f, Z ) );
	V.push_back( C + LVector3( X, 0.0f, Z ) );
	V.push_back( C + LVector3( -X, 0.0f, -Z ) );
	V.push_back( C + LVector3( X, 0.0f, -Z ) );
	V.push_back( C + LVector3( 0.0f, Z, X ) );
	V.push_back( C + LVector3( 0.0f, Z, -X ) );
	V.push_back( C + LVector3( 0.0f, -Z, X ) );
	V.push_back( C + LVector3( 0.0f, -Z, -X ) );
	V.push_back( C + LVector3( Z, X, 0.0f ) );
	V.push_back( C + LVector3( -Z, X, 0.0f ) );
	V.push_back( C + LVector3( Z, -X, 0.0f ) );
	V.push_back( C + LVector3( -Z, -X, 0.0f ) );

	Indices.push_back( 0 );
	Indices.push_back( 4 );
	Indices.push_back( 1 );
	Indices.push_back( 0 );
	Indices.push_back( 9 );
	Indices.push_back( 4 );
	Indices.push_back( 9 );
	Indices.push_back( 5 );
	Indices.push_back( 4 );
	Indices.push_back( 4 );
	Indices.push_back( 5 );
	Indices.push_back( 8 );
	Indices.push_back( 4 );
	Indices.push_back( 8 );
	Indices.push_back( 1 );

	Indices.push_back( 8 );
	Indices.push_back( 10 );
	Indices.push_back( 1  );
	Indices.push_back( 8 );
	Indices.push_back( 3  );
	Indices.push_back( 10 );
	Indices.push_back( 5 );
	Indices.push_back( 3  );
	Indices.push_back( 8  );
	Indices.push_back( 5 );
	Indices.push_back( 2  );
	Indices.push_back( 3  );
	Indices.push_back( 2 );
	Indices.push_back( 7  );
	Indices.push_back( 3  );

	Indices.push_back( 7  );
	Indices.push_back( 10 );
	Indices.push_back( 3  );
	Indices.push_back( 7  );
	Indices.push_back( 6  );
	Indices.push_back( 10 );
	Indices.push_back( 7  );
	Indices.push_back( 11 );
	Indices.push_back( 6  );
	Indices.push_back( 11 );
	Indices.push_back( 0  );
	Indices.push_back( 6  );
	Indices.push_back( 0  );
	Indices.push_back( 1  );
	Indices.push_back( 6  );

	Indices.push_back( 6 );
	Indices.push_back( 1  );
	Indices.push_back( 10 );
	Indices.push_back( 9 );
	Indices.push_back( 0  );
	Indices.push_back( 11 );
	Indices.push_back( 9 );
	Indices.push_back( 11 );
	Indices.push_back( 2  );
	Indices.push_back( 9 );
	Indices.push_back( 5  );
	Indices.push_back( 2  );
	Indices.push_back( 7 );
	Indices.push_back( 2  );
	Indices.push_back( 11 );

	clVertexAttribs* VA = clVertexAttribs::CreateFromTriangles( Indices, V, L_TEXCOORDS_BIT | L_NORMALS_BIT );

	return VA;
}

clVertexAttribs* clGeomServ::CreateSphere( float Radius, LVector3 Center, int TessLevel )
{
	// start with a tetrahedron then subdivide and normalize
	LVector3 V0( cos(   0.0f * Math::DTOR ), sin(   0.0f * Math::DTOR ), 0.0f );
	LVector3 V1( cos( 120.0f * Math::DTOR ), sin( 120.0f * Math::DTOR ), 0.0f );
	LVector3 V2( cos( 240.0f * Math::DTOR ), sin( 240.0f * Math::DTOR ), 0.0f );
	LVector3 V3( 0.0f,  0.0f, sqrt( 2.0f / 3.0f ) * ( V1 - V0 ).Length() );

	const LVector3 C = ( V0 + V1 + V2 + V3 ) / 4.0f;

	V0 -= C;
	V1 -= C;
	V2 -= C;
	V3 -= C;

	std::vector<int>      Indices;
	std::vector<LVector3> Vertices;

	Vertices.push_back( V0 );
	Vertices.push_back( V1 );
	Vertices.push_back( V2 );
	Vertices.push_back( V3 );

	Indices.push_back( 0 );
	Indices.push_back( 1 );
	Indices.push_back( 2 );
	Indices.push_back( 0 );
	Indices.push_back( 3 );
	Indices.push_back( 2 );
	Indices.push_back( 1 );
	Indices.push_back( 3 );
	Indices.push_back( 0 );
	Indices.push_back( 2 );
	Indices.push_back( 3 );
	Indices.push_back( 1 );

	std::vector<int>      OutIndices;
	std::vector<LVector3> OutVertices;

	// 3 iterations
	for ( int i = 0; i != TessLevel / 3; i++ )
	{
		clGeomServ::SplitTriangles( Indices.size() / 3,    &Indices[0],    &Vertices[0],    &OutIndices, &OutVertices );
		clGeomServ::SplitTriangles( OutIndices.size() / 3, &OutIndices[0], &OutVertices[0], &Indices,    &Vertices    );
		clGeomServ::SplitTriangles( Indices.size() / 3,    &Indices[0],    &Vertices[0],    &OutIndices, &OutVertices );
	}

	clVertexAttribs* VA = clVertexAttribs::CreateEmpty();

	VA->Restart( L_PT_TRIANGLE, OutIndices.size(), L_TEXCOORDS_BIT | L_NORMALS_BIT );

	for ( size_t i = 0; i != OutIndices.size(); i++ )
	{
		int Index  = OutIndices[i];
		LVector3 V = OutVertices[ Index ].GetNormalized();

		VA->SetNormalV( V );
		VA->SetTexCoord( 1, 1, 0 );
		VA->EmitVertexV( V * Radius, -1, -1 );
	}

	return VA;
}

void DrawSegment( clVertexAttribs* VA,
                  const LVector2& Pt1,
                  const LVector2& Pt2,
                  float Width,
                  bool CircleInterpolate, // not used
                  bool CircleNormal )
{
	LVector3 Normal1( Pt1, 0.0f );
	LVector3 Normal2( Pt2, 0.0f );

	LVector3 P1( Pt1, +0.5f * Width );
	LVector3 P2( Pt2, +0.5f * Width );
	LVector3 P3( Pt1, -0.5f * Width );

	Normal1 = LVector3( P2 - P1 ).Cross( P3 - P1 ).GetNormalized();
	Normal2 = Normal1;

	if ( !CircleInterpolate )
	{
		if ( CircleNormal ) { VA->SetNormalV( Normal1 ); }

		VA->SetTexCoord( Pt1.X, Pt1.Y, 0 );
		VA->EmitVertex( Pt1.X, Pt1.Y, +0.5f * Width, -1, -1 );
		VA->SetTexCoord( Pt1.X, Pt1.Y - Width, 0 );
		VA->EmitVertex( Pt1.X, Pt1.Y, -0.5f * Width, -1, -1 );
		VA->SetTexCoord( Pt2.X, Pt2.Y - Width, 0 );
		VA->EmitVertex( Pt2.X, Pt2.Y, -0.5f * Width, -1, -1 );

		if ( CircleNormal ) { VA->SetNormalV( Normal2 ); }

		VA->SetTexCoord( Pt1.X, Pt1.Y, 0 );
		VA->EmitVertex( Pt1.X, Pt1.Y, +0.5f * Width, -1, -1 );
		VA->SetTexCoord( Pt2.X, Pt2.Y, 0 );
		VA->EmitVertex( Pt2.X, Pt2.Y, +0.5f * Width, -1, -1 );
		VA->SetTexCoord( Pt2.X, Pt2.Y - Width, 0 );
		VA->EmitVertex( Pt2.X, Pt2.Y, -0.5f * Width, -1, -1 );
	}
}

void DrawTriangleCenter( clVertexAttribs* VA, const LVector2& Pt1, const LVector2& Pt2, float Width )
{
	VA->SetNormalV( LVector3( 0.0f, 0.0f, 1.0f ) );
	VA->SetTexCoord( 0.0f, 0.0f,   0 );
	VA->EmitVertex( 0.0f,   0.0f, +0.5f * Width, -1, -1 );
	VA->SetTexCoord( Pt1.X, Pt1.Y, 0 );
	VA->EmitVertex( Pt1.X, Pt1.Y, +0.5f * Width, -1, -1 );
	VA->SetTexCoord( Pt2.X, Pt2.Y, 0 );
	VA->EmitVertex( Pt2.X, Pt2.Y, +0.5f * Width, -1, -1 );

	VA->SetNormalV( LVector3( 0.0f, 0.0f, -1.0f ) );
	VA->SetTexCoord( 0.0f, 0.0f,   0 );
	VA->EmitVertex( 0.0f,   0.0f, -0.5f * Width, -1, -1 );
	VA->SetTexCoord( Pt1.X, Pt1.Y, 0 );
	VA->EmitVertex( Pt1.X, Pt1.Y, -0.5f * Width, -1, -1 );
	VA->SetTexCoord( Pt2.X, Pt2.Y, 0 );
	VA->EmitVertex( Pt2.X, Pt2.Y, -0.5f * Width, -1, -1 );
}

clVertexAttribs* clGeomServ::CreateGear( float BaseRadius, float RootRadius, float OuterRadius, float Width, int TotalTeeth, int InvoluteSteps )
{
	clVertexAttribs* VA = clVertexAttribs::CreateEmpty();

	float DA    = ( 360.0f / TotalTeeth ) * Math::DTOR;
	float Shift = DA * 2.0f / 3.0f;

	float InvoluteAngle = acos( BaseRadius / OuterRadius );
	float InvoluteDelta = InvoluteAngle / InvoluteSteps;

	const int NUM_GEAR_TRIANGLES_CAPS  = 2 * ( ( InvoluteSteps + 1 ) * 2 + 2 );
	const int NUM_GEAR_TRIANGLES_TEETH = 2 * ( ( InvoluteSteps + 1 ) * 2 + 2 );
	const int NUM_GEAR_TRIANGLES = NUM_GEAR_TRIANGLES_CAPS + NUM_GEAR_TRIANGLES_TEETH;

	// Triangles go in order to produce correct normal facing
	VA->Restart( L_PT_TRIANGLE, NUM_GEAR_TRIANGLES * 3, L_TEXCOORDS_BIT | L_NORMALS_BIT );

	// draw teeth
	for ( int Angle = 0; Angle != TotalTeeth; ++Angle )
	{
		// left side
		LVector2 R1Prev = Math::GetRotatedInvolute( BaseRadius, 0, DA * Angle );

		for ( int i = 0; i != InvoluteSteps + 1; ++ i )
		{
			LVector2 R1 = Math::GetRotatedInvolute( BaseRadius,  i * InvoluteDelta, DA * Angle );

			DrawSegment( VA, R1Prev, R1, Width, false, false );

			R1Prev = R1;
		}

		// connect tops
		DrawSegment( VA, Math::GetRotatedInvolute( BaseRadius,  InvoluteAngle, DA * Angle ),
		             Math::GetRotatedInvolute( BaseRadius, -InvoluteAngle, DA * Angle + Shift ), Width, false, true );

		// connect roots
		DrawSegment( VA, Math::GetRotatedInvolute( BaseRadius, 0, DA*( Angle + 1 ) ),
		             Math::GetRotatedInvolute( BaseRadius, 0, DA * Angle + Shift ), Width, false, true );

		// right side
		LVector2 R2Prev = Math::GetRotatedInvolute( BaseRadius, 0, DA * Angle + Shift );

		for ( int i = 0; i != InvoluteSteps + 1; ++ i )
		{
			LVector2 R2 = Math::GetRotatedInvolute( BaseRadius, -i * InvoluteDelta, DA * Angle + Shift );

			DrawSegment( VA, R2Prev, R2, Width, false, false );

			R2Prev = R2;
		}
	}

	// draw covers
	for ( int Angle = 0; Angle != TotalTeeth; ++Angle )
	{
		LVector2 A1 = Math::GetRotatedInvolute( BaseRadius, 0, DA * ( Angle + 1 ) );
		LVector2 A2 = Math::GetRotatedInvolute( BaseRadius, 0, DA * Angle + Shift );

		DrawTriangleCenter( VA, A1, A2, Width );

		LVector2 R1Prev = Math::GetRotatedInvolute( BaseRadius, 0, DA * Angle );

		for ( int i = 0; i != InvoluteSteps + 1; ++ i )
		{
			LVector2 R1 = Math::GetRotatedInvolute( BaseRadius,  i * InvoluteDelta, DA * Angle );

			DrawTriangleCenter( VA, R1, R1Prev, Width );

			R1Prev = R1;
		}

		LVector2 R2Prev = Math::GetRotatedInvolute( BaseRadius, 0, DA * Angle + Shift );

		for ( int i = 0; i != InvoluteSteps + 1; ++ i )
		{
			LVector2 R2 = Math::GetRotatedInvolute( BaseRadius, -i * InvoluteDelta, DA * Angle + Shift );

			DrawTriangleCenter( VA, R2, R2Prev, Width );

			R2Prev = R2;
		}

		LVector2 B1 = Math::GetRotatedInvolute( BaseRadius,  InvoluteAngle, DA * Angle );
		LVector2 B2 = Math::GetRotatedInvolute( BaseRadius, -InvoluteAngle, DA * Angle + Shift );

		DrawTriangleCenter( VA, B1, B2, Width );
	}

	return VA;
}

clVertexAttribs* clGeomServ::CreateISOGear( float Module, int NumberOfTeeth, float ProfileShift, float Width  )
{
	float BaseDiameter, RootDiameter, OutsideDiameter;

	Math::GetGearParams( Module, NumberOfTeeth, ProfileShift, &BaseDiameter, &RootDiameter, &OutsideDiameter, NULL );

	return CreateGear( 0.5f * BaseDiameter, 0.5f * RootDiameter, 0.5f * OutsideDiameter, Width, NumberOfTeeth, 5 );
}

clVertexAttribs* clGeomServ::CreateSkeletonPose( clJointsSet* Joints, float BaseBoneRadius, float JointRadius )
{
	clVertexAttribs* VA = clVertexAttribs::CreateEmpty();

	VA->Restart( L_PT_TRIANGLE, 1, L_NORMALS_BIT | L_TEXCOORDS_BIT );

	int Sz = Joints->GetJointsCount();

	for ( int i = 0 ; i < Sz ; i++ )
	{
		const sJoint& J = Joints->GetJoint( i );

		LVector3 Src = J.FPosition;

		AddSphere( VA, JointRadius, Src, 3 );

		if ( J.FParentIndex != -1 )
		{
			LVector3 Dest = Joints->GetJoint( J.FParentIndex ).FPosition;

			AddTube( VA, 10, Src, Dest - Src, BaseBoneRadius, true );
		}
	}

	return VA;
}

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
 * 03/11/2007
     Fixed triangles order in CreateAxisAlignedBox()
 * 03/12/2005
     CreateRect2D() supports vertical fliping of tex coords
 * 21/11/2005
     CreateSkyDome()
 * 08/03/2005
     ScaleVA()
     TranslateVA()
 * 25/01/2005
     It's here
*/
