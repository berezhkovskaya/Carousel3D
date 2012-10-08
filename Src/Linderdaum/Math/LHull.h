#pragma once

#ifndef __LHull_h__included__
#define __LHull_h__included__

#include "Math/LFrustum.h"
#include "Math/LPlane.h"
#include "Math/LPoly.h"

namespace Linderdaum
{
	namespace Math
	{
		/// The convex hull defined by the (small and fixed) set of planes
		class LPlaneHull
		{
		public:
			LPlaneHull() { FNumPlanes = 0; }

			/// Take 6 planes from frustum
			LPlaneHull( const LFrustum& F )
			{
				FNumPlanes = 6;

				for ( int i = 0 ; i < FNumPlanes ; i++ )
				{
					FPlanes[i].GetEquation() = F.GetPlane( i );
				}
			}

			/// Take 6 planes from axis-aligned box
			LPlaneHull( const LAABoundingBox& Box )
			{
				HullForBox( Box.ToBox() );
			}

			/// Take 6 planes from the oriented box
			LPlaneHull( const LBox& Box )
			{
				HullForBox( Box );
			}

			/// Take 6 planes from the oriented box
			void HullForBox( const LBox& Box )
			{
				FNumPlanes = 6;

				for ( int i = 0 ; i != FNumPlanes ; i++ )
				{
					float Sgn = ( i % 2 ) ? -1.0f : +1.0f;

					LVector3 N = Sgn * Box.FAxis[i / 2];
					LVector3 P = Box.FCenter + Box.FExtent[i / 2] * N;

					FPlanes[i] = LPlane( N.X, N.Y, N.Z, +N.Dot( P ) );
				}
			}

			/**
			   \brief Build a four-plane hull for the Rect-By-Rect clipping

			   This hull consists of four planes orthogonal to the given V1/V2 vectors
			*/
			void HullForQuad( const LVector3& P, const LVector3& N, const LVector3& V1, const LVector3& V2, float Size1, float Size2 )
			{
				( void )N;

				FNumPlanes = 4;
				FPlanes[0] = LPlane(  V1, P + V1 * Size1 );
				FPlanes[1] = LPlane( -V1, P - V1 * Size1 );
				FPlanes[2] = LPlane(  V2, P + V2 * Size2 );
				FPlanes[3] = LPlane( -V2, P - V2 * Size2 );

				// TODO : check if we should fix normal directions ?
			}

			/// Turn the hull "inside out"
			inline void Invert()
			{
				for ( int i = 0 ; i < FNumPlanes ; i++ )
					for ( int j = 0 ; j < 4 ; j++ )
					{
						FPlanes[i].GetEquation()[j] = -( FPlanes[i].GetEquation()[j] );
					}
			}

			/// Clip the polygon using this hull
			void ClipPolygon( LPoly& Poly, bool UseFront = false, float Thresh = 0.001f ) const
			{
				LPoly Front, Back;

				for ( int i = 0 ; i < FNumPlanes ; i++ )
				{
					Poly.ClipByPlane( FPlanes[i].GetPointOnPlane(), FPlanes[i].GetNormal(), UseFront, Thresh );
				}
			}

		public:
			/// Number of planes in this hull
			Lint32 FNumPlanes;

			/// List of planes for this hull
			LPlane FPlanes[MAX_PLANE_HULL_PLANES];
		};
	}
}

#endif

/*
 * 08/01/2011
     It's here
*/
