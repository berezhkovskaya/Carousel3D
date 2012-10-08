/**
 * \file LPoly.h
 * \brief Polylines
 * \version 0.6.00
 * \date 07/01/2011
 * \author Viktor Latypov, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __LPoly__h__included__
#define __LPoly__h__included__

#include "Math/LPlane.h"
#include "Math/LBox.h"
#include "LAABB.h"

#include "Utils/LArray.h"

class LCurve;

/// Splitting results code
enum eSplitResult
{
	/// Two new polygons
	Split_Two      = 0,

	/// Front polygon, no split
	Split_Front    = 1,

	/// Back polygon, no split
	Split_Back     = 2,

	/// Coplanar with split plane, no split
	Split_Coplanar = 3,

	/// No vertices in source polygon
	Split_NoVertices = 4,
};

namespace Linderdaum
{
	namespace Math
	{
		class LPlaneHull;

		const int MAX_PLANE_HULL_PLANES = 32;
		const int MAX_VERTS_IN_POLY     = 32;

		inline float SignedTriangleArea( const LVector3& A, const LVector3& B, const LVector3& C )
		{
			float Sgn = ( ( B - A ).Dot( C - A ) > 0.0f ) ? 1.0f : -1.0f;

			return 0.5f * ( B - A ).Cross( C - A ).Length() * Sgn;
		}

		bool IsInsideTriangle2D( const LVector3& A, const LVector3& B, const LVector3& C, const LVector3& P );

		/**
		   \brief Single polygon capable of splitting and triangulating itself
		*/
		class LPoly
		{
		public:
			LPoly(): FNumVertices( 0 ), FTexV1( 0 ), FTexV2( 0 ) {}

			/// Texture vectors for planar texture mapping
			LVector3 FTexV1, FTexV2;

			/// Number of vertices
			Lint32  FNumVertices;

			/// Fixed array of vertices
			LVector3     FVertices[MAX_VERTS_IN_POLY];

			/// Precalculated normal to this polygon
			LVector3     FNormal;

			/// Remove equal sequential points
			Lint32 RemoveDuplicates();

			inline float GetArea() const
			{
				LVector3 Side1, Side2;
				float Area  = 0.f;
				Side1 = FVertices[1] - FVertices[0];

				for ( int i = 2; i < FNumVertices; i++ )
				{
					Side2 = FVertices[i] - FVertices[0];
					Area += ( Side1.Cross( Side2 ) ).Length();
					Side1 = Side2;
				}

				return Area;
			}

			/// Make a polygon from curve (NumSegments must be <= MAX_VERTS_IN_POLY)
			void FromClosedCurve( const LCurve& Curve, int NumSegments );

			/// Make a triangle from vertices
			inline void MakeTri( const LVector3& V1, const LVector3& V2, const LVector3& V3 )
			{
				FNumVertices = 0;
				AddVertexQuick( V1 );
				AddVertexQuick( V2 );
				AddVertexQuick( V3 );
			}

			/// Make a quad from vertices
			inline void MakeQuad( const LVector3& V1, const LVector3& V2, const LVector3& V3, const LVector3& V4 )
			{
				FNumVertices = 0;
				AddVertexQuick( V1 );
				AddVertexQuick( V2 );
				AddVertexQuick( V3 );
				AddVertexQuick( V4 );
			}

			/// Calculate the normal to this polygon
			inline LVector3 GetNormal() const
			{
				LVector3 C = GetCenter();

				LVector3 N = ( C - FVertices[0] ).Cross( C - FVertices[1] ).GetNormalized();

				return N;
			}

			/// Construct plane equation for this polygon
			inline LPlane GetPlane() const
			{
				LVector3 N = GetNormal();

				LPlane Res( N.X, N.Y, N.Z, -N.Dot( FVertices[0] ) );

				return Res;
			}

			// FIXME: unstable, should iterative summation scheme
			inline LVector3 GetCenter() const
			{
				LVector3 C( 0, 0, 0 );

				if ( FNumVertices <= 0 ) { return C; }

				for ( int i = 0 ; i < FNumVertices ; i++ ) { C += FVertices[i]; }

				return C / static_cast<float>( FNumVertices );
			}

			/// Shift all vertices by Delta
			inline void Shift( const LVector3& Delta ) { for ( int i = 0 ; i < FNumVertices ; i++ ) { FVertices[i] += Delta; } }

			/// Transform all vertices by mtx
			inline void Transform( const LMatrix4& Mtx ) { for ( int i = 0 ; i < FNumVertices ; i++ ) { FVertices[i] = Mtx * FVertices[i]; } }

			/// Split the polygon into two other polygons and get the split result
			eSplitResult SplitByPlane( const LVector3& PlanePoint, const LVector3& PlaneNormal, LPoly* Front, LPoly* Back, float Thresh );

			/// Clip this polygon by some plane. UseFront specifies which part to use
			inline void ClipByPlane( const LVector3& PlanePoint, const LVector3& PlaneNormal, bool UseFront = true, float Thresh = 0.0001f )
			{
				LPoly Front, Back;

				eSplitResult Res = SplitByPlane( PlanePoint, PlaneNormal, &Front, &Back, Thresh );

				if ( Res == Split_Two )
				{
					*this = UseFront ? Front : Back;
				}
				else if ( Res == Split_NoVertices )
				{
					FNumVertices = 0;
				}
				else if ( Res == Split_Back )
				{
					if ( UseFront ) { FNumVertices = 0; }
				}
				else if ( Res == Split_Front )
				{
					if ( !UseFront ) { FNumVertices = 0; }
				}
			}

			/// Quickly add the vertex
			inline void AddVertexQuick( const LVector3& Vtx ) { FVertices[FNumVertices++] = Vtx; }

			/// Invert vertex order
			void Invert();

			/// Project each vertex to plane
			void ProjectToPlane( const LVector3& N, const LVector3& P );

			/// Use the (x,y) coordinates as a plane coords
			void ConvertTo3D( const LVector3& N, const LVector3& BasePoint );

			/// Make all the dirty work (project to the plane) and return the number of triangles
			int Triangulate( Lint32* Points ) const;

			/// Try to optimize this polygon. Returns false if nonconvex or collapsed
			bool RemoveColinearPoints();

			/// Inserts a vertex
			void InsertVertex( Lint32 Pos, const LVector3& Vtx );

			/// Remove a vertex
			void RemoveVertex( const LVector3& Vtx );

			/// Check if polygons are equal
			inline bool operator==( const LPoly& Other )
			{
				if ( FNumVertices != Other.FNumVertices ) { return false; }

				for ( int i = 0 ; i < FNumVertices ; i++ )
				{
					if ( FVertices[i] != Other.FVertices[i] )
					{
						return false;
					}
				}

				return true;
			}

			/// Check if polygons are not equal
			inline bool operator!=( const LPoly& Other ) { return !( *this == Other ); }

		private:
			/// Returns the area of the contour
			inline float Area2D() const
			{
				float A = 0.0f;

				for ( int p = FNumVertices - 1, q = 0; q < FNumVertices ; p = q++ )
				{
					A += FVertices[p].X * FVertices[q].Y - FVertices[q].X * FVertices[p].Y;
				}

				return( A * 0.5f );
			}

			/// Treat this poly as a (x,y)-based one and output the list of (FNumVertices-2) triangles. Ear-clipping, returns number of vertices
			int Triangulate2D( Lint32* Points ) const;

			/// Internal routine for triangulation
			bool CheckInternal( int u, int v, int w, int n, int* V ) const;
		};

		/**
		   Texture mapping type is only planar (FTexV1/FTexV2 vectors are multiplied by Vtx to get U/V)

		   Later the coordinates can be remapped in VertexAttribs
		*/

		/// A "linear" polyhedron with no connectivity/adjacency information. Not suitable for fast calculations.
		class LPolyList
		{
		public:
			LPolyList(): FPolys() {}

#pragma region Geometry construction routines

			/// Build a polyhedral representation of the convex plane hull
			void FromPlaneHull( const LPlaneHull& Hull );

			/// Given two curves build a "cobordism" for them
			void MakeSegmentBetweenCurves( const LCurve& Curve1, const LCurve& Curve2, int NumSegments );

			/// Duplicate the Base curve, "drag" it along the path
			void Loft( const LCurve& Base, const LCurve& Path, int NumSteps, int NumSegments );

			/// Build a Loft() with a circle as base
			void MakeTubularNeighbourhood( const LCurve& Path, float Radius, int NumSteps, int NumSegments );

			/// Make a polylist from base polygon
			void ExtrudeLinear( const LPoly& Poly, const LVector3& Direction, float Length );

#pragma endregion

#pragma region Clipping

			/// Clip the poly list by plane
			void ClipByPlane( const LPlane& P, bool UseFront = false, float Thresh = 0.001f )
			{
				for ( size_t i = 0 ; i != FPolys.size() ; i++ )
				{
					FPolys[i].ClipByPlane( P.GetPointOnPlane(), P.GetNormal(), UseFront, Thresh );
				}

				RemoveRedundant();
			}

			/// Clip the poly list by plane
			void ClipByHull( const LPlaneHull& Hull, bool UseFront = false, float Thresh = 0.001f );

#pragma endregion

			/// Non-optimal triangulation (does not reuse common vertices). Points must contain at least (NumPolys * MAX_VERTS_IN_POLY * 3)
			int  Triangulate( LVector3* Points ) const;

			/// Output only non-duplicated indices with some threshold. Return the number of generated triangles. If OutTexCoords is not null, tex coords are generated
			int  TriangulateIndexed( LVector3* Points, int& NumPoints, int* Indices, float Thresh, LVector3* Normals, LVector4* OutTexCoords ) const;

			/// Remove empty polys
			void RemoveRedundant();

		public:
			/// List of polygons
			LArray<LPoly> FPolys;
		};

		/// Create a single polygon lying in a given plane whose extents are really big (Size1 and Size2) and which is relatively close to the hint point
		LPoly CreateLargePolyForPlane( const LPlane& Plane, const LVector3& HintPoint, float HalfSize1, float HalfSize2 );

	}; // namespace Math

}; // namespace Linderdaum

#endif

/*
 * 07/01/2011
     It's here
*/
