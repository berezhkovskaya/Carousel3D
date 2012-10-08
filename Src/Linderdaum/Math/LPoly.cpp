/**
 * \file LPoly.cpp
 * \brief Polylines
 * \version 0.6.00
 * \date 07/01/2011
 * \author Viktor Latypov, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include <string.h>

#include "LPoly.h"
#include "LCurve.h"
#include "LGeomUtils.h"

#include "LHull.h"

namespace Linderdaum
{

	namespace Math
	{

		// FIXME: these shitty constants are everywhere and there are much more to come. We must handle them somehow
		const float POINTS_SAME_THRESHOLD = 0.0001f;
		const float FLOAT_NORMAL_THRESH   = 0.0001f;

		bool IsInsideTriangle2D( const LVector3& A, const LVector3& B, const LVector3& C, const LVector3& P )
		{
			float ax = C.X - B.X;
			float ay = C.Y - B.Y;
			float bx = A.X - C.X;
			float by = A.Y - C.Y;
			float cx = B.X - A.X;
			float cy = B.Y - A.Y;
			float apx = P.X - A.X;
			float apy = P.Y - A.Y;
			float bpx = P.X - B.X;
			float bpy = P.Y - B.Y;
			float cpx = P.X - C.X;
			float cpy = P.Y - C.Y;

			float aCROSSbp = ax * bpy - ay * bpx;
			float cCROSSap = cx * apy - cy * apx;
			float bCROSScp = bx * cpy - by * cpx;

			return ( ( aCROSSbp >= 0.0f ) && ( bCROSScp >= 0.0f ) && ( cCROSSap >= 0.0f ) );
		}

		LPoly CreateLargePolyForPlane( const LPlane& Plane, const LVector3& HintPoint, float HalfSize1, float HalfSize2 )
		{
			LPoly Poly;

			LVector3 V1, V2;
			Plane.BuildBasis( &V1, &V2 );

			LVector3 HintProjection = Plane.ProjectPoint( HintPoint );

			Poly.FNumVertices = 0;

			// 1. Add four points

			Poly.AddVertexQuick( HintProjection - V1 * HalfSize1 - V2 * HalfSize2 );
			Poly.AddVertexQuick( HintProjection + V1 * HalfSize1 - V2 * HalfSize2 );
			Poly.AddVertexQuick( HintProjection + V1 * HalfSize1 + V2 * HalfSize2 );
			Poly.AddVertexQuick( HintProjection - V1 * HalfSize1 + V2 * HalfSize2 );

			// 2. Orient the normal
			if ( Plane.GetNormal().Dot( Poly.GetNormal() ) )
			{
				Poly.Invert();
			}

			return Poly;
		}

		bool  PointsAreApproxTheSame( const LVector3& Pt1, const LVector3& Pt2 )
		{
			float Tmp;

			for ( int i = 0 ; i < 3 ; i++ )
			{
				Tmp = Pt1[i] - Pt2[i];

				if ( ( Tmp < -Math::POINTS_SAME_THRESHOLD ) || ( Tmp > Math::POINTS_SAME_THRESHOLD ) ) { return false; }
			}

			return true;
		}

		bool  PointsAreNear( const LVector3& Pt1, const LVector3& Pt2, float Thresh )
		{
			float Tmp;

			for ( int i = 0 ; i < 3 ; i++ )
			{
				Tmp = Pt1[i] - Pt2[i];

				if ( ( Tmp < -Thresh ) || ( Tmp > Thresh ) ) { return false; }
			}

			return true;
		}

		/// Clip the poly list by plane
		void LPolyList::ClipByHull( const LPlaneHull& Hull, bool UseFront, float Thresh )
		{
			for ( int i = 0 ; i < Hull.FNumPlanes; i++ )
			{
				ClipByPlane( Hull.FPlanes[i], UseFront, Thresh );
			}
		}

		/// Project each vertex to plane
		void LPoly::ProjectToPlane( const LVector3& N, const LVector3& P )
		{
			LVector3 V1, V2;

			BuildComplementaryBasis( N, V1, V2 );

			for ( int i = 0 ; i < FNumVertices ; i++ )
			{
				LVector3 Pt = FVertices[i] - P;

				FVertices[i] = vec3( Pt.Dot( V1 ), Pt.Dot( V2 ), 0.0f );
			}
		}

		/// Use the (x,y) coordinates as a plane coords
		void LPoly::ConvertTo3D( const LVector3& N, const LVector3& BasePoint )
		{
			LVector3 V1, V2;

			BuildComplementaryBasis( N, V1, V2 );

			for ( int i = 0 ; i < FNumVertices ; i++ )
			{
				float u = FVertices[i].X;
				float v = FVertices[i].Y;

				FVertices[i] = BasePoint + u * V1 + v * V2;
			}
		}

		Lint32 LPoly::RemoveDuplicates()
		{
			Lint32 Count = 0;

			Lint32 Prev = FNumVertices - 1;

			for ( Lint32 i = 0 ; i < FNumVertices ; i++ )
			{
				if ( !PointsAreApproxTheSame( FVertices[i], FVertices[Prev] ) )
				{
					if ( Count != i ) { FVertices[Prev] = FVertices[i]; }

					Prev = Count++;
				}
			}

			return ( FNumVertices = ( Count < 3 ) ? 0 : Count );
		}

		eSplitResult LPoly::SplitByPlane( const LVector3& PlanePoint, const LVector3& PlaneNormal, LPoly* Front, LPoly* Back, float Thresh )
		{
			LVector3 IntersectionPoint;

			/// Current, max and min distances to plane
			float Dist, MaxDist = 0, MinDist = 0;

			/// Cached signed distances from vertices to the splitting plane
			float Dists[MAX_VERTS_IN_POLY];

			enum {V_FRONT, V_BACK, V_EITHER} Status, PrevStatus = V_EITHER;

			if ( FNumVertices < 3 ) { return Split_NoVertices; }

			/// Calculate min/max signed distances for each vertex to check if the entire polygon is coplanar/front/back
			Lint32 i;

			for ( i = 0 ; i < FNumVertices ; i++ )
			{
				Dist = Dists[i] = ( FVertices[i] - PlanePoint ).Dot( PlaneNormal );

				if ( i == 0 || Dist > MaxDist ) { MaxDist = Dist; }

				if ( i == 0 || Dist < MinDist ) { MinDist = Dist; }

				if      ( Dist > +Thresh ) { PrevStatus = V_FRONT; }
				else if ( Dist < -Thresh ) { PrevStatus = V_BACK; }
			}

			if ( MaxDist < Thresh && MinDist > -Thresh ) { return Split_Coplanar; }

			if ( MaxDist < Thresh ) { return Split_Back; }

			if ( MinDist > -Thresh ) { return Split_Front; }

			/// Caller only wanted status.
			if ( !Front ) { return Split_Two; }

			*Front = *this;
			Front->FNumVertices =  0;

			*Back = *this;
			Back->FNumVertices = 0;

			LPoly* Dest;

			LVector3 Vtx, PrevVtx = FVertices[FNumVertices - 1];

			float PrevDist;

			for ( i = 0 ; i < FNumVertices ; i++ )
			{
				Vtx = FVertices[i];

				PrevDist = Dist;
				Dist     = Dists[i];

				if      ( Dist > +Thresh ) { Status = V_FRONT; }
				else if ( Dist < -Thresh ) { Status = V_BACK; }
				else { Status = PrevStatus; }

				if ( Status != PrevStatus )
				{
					// Current edge (PrevVtx, Vtx) crosses the plane. Either Front-to-Back or Back-To-Front.
					// Intersection point is on both front and back polys.
					if ( ( Dist >= -Thresh ) && ( Dist < +Thresh ) )
					{
						// This point lies on plane.
						Front->AddVertexQuick( Vtx );
						Back ->AddVertexQuick( Vtx );
					}
					else if ( ( PrevDist >= -Thresh ) && ( PrevDist < +Thresh ) )
					{
						// Previous point lies on plane.
						Dest = ( Status == V_FRONT ) ? Front : Back;

						Dest->AddVertexQuick( PrevVtx );
						Dest->AddVertexQuick( Vtx );
					}
					else
					{
						// Intersection point is in between.
						IntersectionPoint = PrevVtx + ( Vtx - PrevVtx ) * ( PrevDist / ( PrevDist - Dist ) );

						Front->AddVertexQuick( IntersectionPoint );
						Back ->AddVertexQuick( IntersectionPoint );

						( ( PrevStatus == V_FRONT ) ? Back : Front ) ->AddVertexQuick( Vtx );
					}
				}
				else
				{
					( ( Status == V_FRONT ) ? Front : Back ) ->AddVertexQuick( Vtx );
				}

				PrevStatus = Status;
				PrevVtx    = Vtx;
			}

			/// Ignore front sliver
			if ( Front->RemoveDuplicates() < 3 ) { return Split_Back; }

			/// Ignore back sliver
			if ( Back->RemoveDuplicates() < 3 ) { return Split_Front; }

			return Split_Two;
		}

		void LPoly::InsertVertex( Lint32 Pos, const LVector3& Vtx )
		{
			if ( Pos > FNumVertices ) { return; }

			/// Shift every vertex after the Pos
			for ( Lint32 i = Pos ; i < FNumVertices ; i++ )
			{
				FVertices[i + 1] = FVertices[i];
			}

			FVertices[Pos] = Vtx;

			FNumVertices++;
		}

		void LPoly::RemoveVertex( const LVector3& Vtx )
		{
			Lint32 Ptr = 0;
			LVector3   New[MAX_VERTS_IN_POLY];

			for ( Lint32 i = 0 ; i < FNumVertices ; ++i )
				if ( FVertices[i] != Vtx )
				{
					New[Ptr++] = FVertices[i];
				}

			FNumVertices = Ptr;

			for ( Lint32 i = 0 ; i < Ptr ; ++i )
			{
				FVertices[i] = New[i];
			}
		}

		void LPoly::Invert()
		{
			int Mid = FNumVertices / 2;

			for ( int i = 0 ; i < Mid ; i++ )
			{
				LVector3 Tmp = FVertices[i];
				FVertices[i] = FVertices[FNumVertices - 1 - i];
				FVertices[FNumVertices - 1 - i] = Tmp;
			}

			FNormal = -FNormal;
		}


		bool LPoly::RemoveColinearPoints()
		{
			LVector3 SidePlaneNormal[MAX_VERTS_IN_POLY];
			LVector3 Side;
			int  i, j;

			for ( i = 0; i < FNumVertices; i++ )
			{
				j = i - 1;

				if ( j < 0 ) { j = FNumVertices - 1; }

				// Create cutting plane perpendicular to both this side and the polygon's normal.
				Side = FVertices[i] - FVertices[j];
				SidePlaneNormal[i] = Side.Cross( FNormal );

				if ( SidePlaneNormal[i].Length() < 0.00001f )
				{
					// Eliminate these nearly identical points.
					memcpy( &FVertices[i], &FVertices[i+1], ( FNumVertices - ( i + 1 ) ) * sizeof ( LVector3 ) );

					if ( --FNumVertices < 3 ) { FNumVertices = 0; return false; }

					i--;
				}
			}

			for ( i = 0; i < FNumVertices ; i++ )
			{
				j = i + 1;

				if ( j >= FNumVertices ) { j = 0; }

				if ( PointsAreNear( SidePlaneNormal[i], SidePlaneNormal[j], Math::FLOAT_NORMAL_THRESH ) )
				{
					int Sz = ( FNumVertices - ( i + 1 ) ) * sizeof ( LVector3 );
					// Eliminate colinear points.
					memcpy ( &FVertices[i], &FVertices[i+1], Sz );
					memcpy ( &SidePlaneNormal[i], &SidePlaneNormal[i+1], Sz );

					if ( --FNumVertices < 3 ) { FNumVertices = 0; return false; }; // Collapsed.

					i--;
				}
				else
				{
					for ( j = 0; j < FNumVertices; j++ )
					{
						if ( j != i )
						{
							switch ( SplitByPlane ( FVertices[i], SidePlaneNormal[i], NULL, NULL, 0.0001f ) )
							{
								case Split_Front:
									return 0; // Nonconvex + Numerical precision error
								case Split_Two:
									return 0; // Nonconvex
									// Split_Back: Means it's convex
									// Split_Coplanar: Means it's probably convex (numerical precision)
							}
						}
					}
				}
			}

			return true; // Ok.
		}

		int LPoly::Triangulate( Lint32* Points ) const
		{
			/*
			// remove colinears and check convexity
			if (RemoveColinearPoints())
			{
			   int Cnt = 0;
			   // no need to do anything, just output the indices (obvious O(n) complexity)
			   for (int i = 1 ; i < FNumVertices - 1 ; i++, Cnt++)
			   {
			      // add the (0, i, i+1) triple
			      Points[Cnt * 3 + 0] = 0;
			      Points[Cnt * 3 + 1] = i;
			      Points[Cnt * 3 + 2] = i + 1;
			   }

			   return FNumVertices - 2;
			} else
			{
			   // check if the polygon collapsed
			   if (FNumVertices <= 0) return 0;
			}
			*/
			// project to the plane using FVertices[0] as a basepoint
			LVector3 N = GetNormal();

			LPoly Tmp = *this;
			Tmp.ProjectToPlane( N, FVertices[0] );

			return Tmp.Triangulate2D( Points ) / 3;
		}

		int LPoly::Triangulate2D( Lint32* Points ) const
		{
			int Num = 0;

			if ( FNumVertices < 3 ) { return Num; }

			int V[MAX_VERTS_IN_POLY];

			// Clockwise or counterclockwise ?
			if ( 0.0f < Area2D() )
			{
				for ( int v = 0; v < FNumVertices ; v++ ) { V[v] = v; }
			}
			else
			{
				for ( int v = 0; v < FNumVertices ; v++ ) { V[v] = ( FNumVertices - 1 ) - v; }
			}

			int nv = FNumVertices;
			int count = 2 * nv;

			int s, t, u, w;

			for ( int m = 0, v = nv - 1; nv > 2; )
			{
				if ( 0 >= ( count-- ) ) { return Num; }

				u = v;

				if ( nv <= u ) { u = 0; }

				v = u + 1;

				if ( nv <= v ) { v = 0; }

				w = v + 1;

				if ( nv <= w ) { w = 0; }

				if ( CheckInternal( u, v, w, nv, V ) )
				{
					Points[Num++] = V[u];
					Points[Num++] = V[v];
					Points[Num++] = V[w];

					m++;

					for ( s = v, t = v + 1; t < nv; s++, t++ ) { V[s] = V[t]; }

					nv--;
					count = 2 * nv;
				}
			}

			return Num;
		}

		bool LPoly::CheckInternal( int u, int v, int w, int n, int* V ) const
		{
			LVector3 A = FVertices[V[u]];
			LVector3 B = FVertices[V[v]];
			LVector3 C = FVertices[V[w]];

			if ( 0.00001f > ( ( ( B.X - A.X ) * ( C.Y - A.Y ) ) - ( ( B.Y - A.Y ) * ( C.X - A.X ) ) ) ) { return false; }

			for ( int p = 0; p < n; p++ )
			{
				if ( ( p == u ) || ( p == v ) || ( p == w ) ) { continue; }

				if ( IsInsideTriangle2D( A, B, C, FVertices[V[p]] ) ) { return false; }
			}

			return true;
		}


		void LPolyList::FromPlaneHull( const LPlaneHull& Hull )
		{
			FPolys.clear();

			const float Sz = +100000.0f;

			for ( int i = 0 ; i < Hull.FNumPlanes ; i++ )
			{
				LPoly P = CreateLargePolyForPlane( Hull.FPlanes[i], vec3( 0, 0, 0 ), Sz, Sz );
				Hull.ClipPolygon( P );
				FPolys.push_back( P );
			}
		}

		void LPolyList::ExtrudeLinear( const LPoly& Poly, const LVector3& Direction, float Length )
		{
			FPolys.clear();

			// 1. add base polygon
			FPolys.push_back( Poly );

			// 2. add the "cap"
			FPolys.push_back( Poly );

			// adjust "cap" coordinates (shift in the direction)
			FPolys[1].Shift( Direction * Length );

			// 3. add prism sides
			for ( int i = 0 ; i < Poly.FNumVertices ; i++ )
			{
				int j = ( i + 1 ) % Poly.FNumVertices;

				LPoly P;
				P.FNumVertices = 0;

				P.AddVertexQuick( FPolys[0].FVertices[i] );
				P.AddVertexQuick( FPolys[1].FVertices[i] );
				P.AddVertexQuick( FPolys[1].FVertices[j] );
				P.AddVertexQuick( FPolys[0].FVertices[j] );

				FPolys.push_back( P );
			}

			// 4. invert the "cap"
			FPolys[1].Invert();
		}

		void LPolyList::RemoveRedundant()
		{
			/*
			size_t Sz = FPolys.size();

			for (size_t i = 0 ; i < Sz ; i++)
			{
			  if (FPolys[i].FNumVertices <= 0)
			  {
			     FPolys[i] = FPolys[FPolys.size() - 1];
			     FPolys.pop_back();
			     Sz--;
			  }
			}*/
		}

		int LPolyList::Triangulate( LVector3* Points ) const
		{
			int Count = 0;

			int Indices[MAX_VERTS_IN_POLY * 3];

			for ( int i = 0 ; i < static_cast<int>( FPolys.size() ) ; i++ )
			{
				// triangulate current polygon
				int Num = FPolys[i].Triangulate( Indices );

				// add points to the list
				for ( int j = 0 ; j < Num ; j++ )
				{
					Points[Count++] = FPolys[i].FVertices[Indices[j * 3 + 0]];
					Points[Count++] = FPolys[i].FVertices[Indices[j * 3 + 1]];
					Points[Count++] = FPolys[i].FVertices[Indices[j * 3 + 2]];
				}
			}

			return Count;
		}

		/// Add point to array checking for duplicates
		int AddToArray( LVector3* Pts, int& NumPoints, const LVector3& Pt, float Thresh )
		{
			if ( Thresh > 0.0f )
			{
				for ( int i = 0 ; i < NumPoints ; i++ )
				{
					if ( PointsAreNear( Pts[i], Pt, Thresh ) )
					{
						return i;
					}
				}

			}

			Pts[NumPoints++] = Pt;
			return NumPoints - 1;
		}

		void LPoly::FromClosedCurve( const LCurve& Curve, int NumSegments )
		{
			float TMin = Curve.GetMinimumParameter();
			float TMax = Curve.GetMaximumParameter();

			float dT = ( TMax - TMin ) / ( float )NumSegments;

			for ( int i = 0 ; i < NumSegments ; i++ )
			{
				AddVertexQuick( Curve.GetPosition( TMin + ( float )i * dT ) );
			}
		}

		/// Duplicate the Base curve, "drag" it along the path
		void LPolyList::Loft( const LCurve& Base, const LCurve& Path, int NumSteps, int NumSegments )
		{
			float PathMin = Path.GetMinimumParameter();

			float PathDT =  ( Path.GetMaximumParameter() - PathMin ) / ( float )NumSteps;

			LSpline LastCurve;
			LSpline ThisCurve;

			for ( int i = 0 ; i < NumSteps ; i++ )
			{
				float t = PathMin + ( float )i * PathDT;

				Base.CopyTo( &LastCurve );
				Base.CopyTo( &ThisCurve );

				LastCurve.TranslateAlongPath( Path, PathMin, t );
				ThisCurve.TranslateAlongPath( Path, PathMin, t + PathDT );

				MakeSegmentBetweenCurves( LastCurve, ThisCurve, NumSegments );
			}
		}

		/// Build a Loft() with a circle as base
		void LPolyList::MakeTubularNeighbourhood( const LCurve& Path, float Radius, int NumSteps, int NumSegments )
		{
			LSpline Base;
			Base.GenerateCircle( vec3( 0, 0, 0 ), Path.GetDerivative( Path.GetMinimumParameter() ), Radius, NumSegments );

			Loft( Base, Path, NumSteps, NumSegments );
		}

		void LPolyList::MakeSegmentBetweenCurves( const LCurve& Curve1, const LCurve& Curve2, int NumSegments )
		{
			float TMin1 = Curve1.GetMinimumParameter();
			float TMax1 = Curve1.GetMaximumParameter();
			float TMin2 = Curve2.GetMinimumParameter();
			float TMax2 = Curve2.GetMaximumParameter();

			float dT1 = ( TMax1 - TMin1 ) / ( float )NumSegments;
			float dT2 = ( TMax2 - TMin2 ) / ( float )NumSegments;

//			FPolys.reserve( NumSegments );

			LVector3 LastP1 = Curve1.GetPosition( TMin1 );
			LVector3 LastP2 = Curve2.GetPosition( TMin2 );

			LVector3 P1, P2;

//			FPolys.resize(NumSegments - 1);

			for ( int i = 1 ; i < NumSegments ; i++ )
			{
				P1 = Curve1.GetPosition( TMin1 + ( float )i * dT1 );
				P2 = Curve2.GetPosition( TMin2 + ( float )i * dT2 );

				FPolys.push_back( LPoly() );
				FPolys[FPolys.size() - 1].MakeQuad( LastP1, P1, P2, LastP2 );

				LastP1 = P1;
				LastP2 = P2;
			}
		}

		/// Output only non-duplicated indices with some threshold. Return the number of generated triangles
		int LPolyList::TriangulateIndexed( LVector3* Points, int& NumPoints, int* Indices, float Thresh, LVector3* OutNormals, LVector4* OutTexCoords ) const
		{
			int VtxCount = 0;

			int TmpIndices[MAX_VERTS_IN_POLY * 3];

			int i, j, k, Num, NewIdx;

			LVector3 Vtx, Norm;
			LVector4 TVtx( 0 );

			for ( i = 0 ; i < static_cast<int>( FPolys.size() ) ; i++ )
			{
				const LPoly* Poly = &FPolys[i];

				// triangulate current polygon
				Num = Poly->Triangulate( TmpIndices );

				Norm = Poly->GetNormal();

				LVector3 V0 = Poly->FVertices[0];

				// add points to the list
				for ( j = 0 ; j < Num ; j++ )
				{
					for ( k = 0 ; k < 3 ; k++ )
					{
						Vtx = Poly->FVertices[TmpIndices[j * 3 + k]];
						NewIdx = AddToArray( Points, NumPoints, Vtx, Thresh );
						Indices[VtxCount] = NewIdx;

						Vtx -= V0;

						if ( OutTexCoords )
						{
							TVtx.X = Vtx.Dot( Poly->FTexV1 );
							TVtx.Y = Vtx.Dot( Poly->FTexV2 );

							OutTexCoords[ NewIdx ] = TVtx;
						}

						if ( OutNormals )
						{
							OutNormals[NewIdx] = Norm;
						}

						VtxCount++;
					}
				}
			}

			return VtxCount / 3; // number of triangles
		}

	}; // namespace Math

}; // namespace Linderdaum

/*
 * 07/01/2011
     It's here
*/
