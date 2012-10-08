/**
 * \file Collision.h
 * \brief Basic collision detection
 * \version 0.5.93
 * \date 09/10/2010
 * \author Viktor Latypov, 2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __Collision__h__included__
#define __Collision__h__included__

#include "Math/LSphere.h"
#include "Math/LPlane.h"

#include "Math/LBox.h"

#include <algorithm>

namespace Linderdaum
{

	/// Linderdaum Math Library

	namespace Math
	{

		/// Detailed information about collision
		struct sCollisionInfo
		{
			/// Collision normal for the first body
			LVector3 N1;

			/// Collision normal for the second body
			LVector3 N2;

			/// Number of contact points
			int FNumPoints;

			/// Contact points for body 1 in world coordinates
			LVector3 FPoints1[8];

			/// Contact points for body 2 in world coordinates
			LVector3 FPoints2[8];

			/// Average contact points (for sphere-to-sphere it would be a single point coinciding with both Points1/Points2)
			LVector3 FPoints[8];

			/// Penetration for each point
			float FPenetration[8];

			/// Swap points1/points2
			inline void Swap()
			{
				for ( int i = 0 ; i < 8 ; i++ ) { std::swap( FPoints1[i], FPoints2[i] ); }
			}
		};

		/// Calculate collision point for 2D Circle-Circle intersection
		bool Collision_CircleToCircle( const LCircle& S1, const LCircle& S2, LVector2* ISect, LVector2* Normal, float* Depth );

#pragma region Sphere collisions

		/**
		   Calculate sphere-to-plane intersection point giving contact normal and penetration depth
		*/
		bool Collision_SphereToPlane( const LSphere& S, const LPlane& P, LVector3* ISect, LVector3* Normal, float* Depth );

		/**
		   \brief Calculate sphere-to-plane intersection point giving contact normal and penetration depth

		   Remark: 'ISect' and 'Normal' are given in the world space
		*/
		bool Collision_SphereToSphere( const LSphere& S1, const LSphere& S2, LVector3* ISect, LVector3* Normal, float* Depth );

		/**
		   \brief Calculate collision points for the Box-To-Sphere intersection
		*/
		bool Collision_SphereToBox( const LSphere& Sphere, const LBox& Box, sCollisionInfo* Info );

#pragma endregion

#pragma region Box collisions

		/**
		   \brief Calculate collision points for the Box-To-Box intersection

		   Boxes are given in global coordinates (world space), so are the collision points/normals
		*/
		bool Collision_BoxToBox( const LBox& ABox, const LBox& BBox, Linderdaum::Math::sCollisionInfo* Info );

		/**
		   Calculate box-to-plane intersection points giving contact normals and penetration depths
		*/
		bool Collision_BoxToPlane( const LBox& B, const LPlane& P, sCollisionInfo* Info );

#pragma endregion
	}

}

#endif

/*
 * 09/10/2010
     Collision_CircleToCircle()
 * 16/06/2010
     It's here
*/
