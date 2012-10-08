/**
 * \file CSM.cpp
 * \brief Helper functions for Cascaded Shadow Maps
 * \version 0.5.91
 * \date 09/07/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "CSM.h"
/*
const int   MAX_SPLITS   = 4;
const int   CurNumSplits = 2;

const float CurNumSplitsF = static_cast<float>( CurNumSplits );
const float SplitWeight   = 0.75f;

// Update near and far distances for every frustum slice in camera eye space
void UpdateSplitDistances( LFrustum F[MAX_SPLITS], float Near, float Far )
{
   float A = SplitWeight;
   float B = 1.0f - A;
   float Ratio  = Far / Near;

   F[0].FNearClipPlane = Near;

   for ( int i = 1; i < CurNumSplits; i++ )
   {
      float S = i / CurNumSplitsF;

      F[i].FNearClipPlane  = A * ( Near*powf(Ratio, S) ) + B * ( Near + (Far - Near)*S );
      F[i-1].FFarClipPlane = F[i].FNearClipPlane * 1.005f;
   }

   F[ CurNumSplits-1 ].FFarClipPlane = Far;
}

// From [1] http://developer.download.nvidia.com/SDK/10.5/opengl/src/cascaded_shadow_maps/doc/cascaded_shadow_maps.pdf
// page 7.
LMatrix4 CalculateCropMatrix( const LFrustum& FrustumSlice,
                              const LMatrix4& LightMVP )
{
   // transform frustum into light coordinates
   LAABoundingBox FrustumAABB;

   for ( int i = 0; i != 8; i++ )
   {
      LVector4 Pt = LVector4( FrustumSlice.GetCornerPoint( i ), 1.0f );

      Pt = Pt * MVP;

      // project x and y according to [1]
      Pt.X /= Pt.W;
      Pt.Y /= Pt.W;

      FrustumAABB.CombinePoint( Pt );
   }

   float Sx = 2.0f / FrustumAABB.GetSizeX();
   float Sy = 2.0f / FrustumAABB.GetSizeY();

   float Ox = -0.5f * ( FrustumAABB.FMax.X + FrustumAABB.FMin.X ) * Sx;
   float Oy = -0.5f * ( FrustumAABB.FMax.Y + FrustumAABB.FMin.Y ) * Sy;

   LMatrix4 CropMatrix = LMatrix4::Identity();

   CropMatrix[0][0] = Sx;
   CropMatrix[1][1] = Sy;
   CropMatrix[0][3] = Ox;
   CropMatrix[1][3] = Oy;

   // FrustumAABB.FMax.Z - new far plane for the light

   return CropMatrix;
}

*/

/*
// here all shadow map textures and their corresponding matrices are created
void makeShadowMap()
{
   float shad_modelview[16];

   gluLookAt(0, 0, 0, -light_dir[0], -light_dir[1], -light_dir[2], -1.0f, 0.0f, 0.0f);
   glGetFloatv(GL_MODELVIEW_MATRIX, shad_modelview);

   // redirect rendering to the depth texture
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, depth_fb);
   // store the screen viewport
   glPushAttrib(GL_VIEWPORT_BIT);
   // and render only to the shadowmap
   glViewport(0, 0, depth_size, depth_size);
   // offset the geometry slightly to prevent z-fighting
   // note that this introduces some light-leakage artifacts
   glPolygonOffset( 1.0f, 4096.0f);
   glEnable(GL_POLYGON_OFFSET_FILL);

   // draw all faces since our terrain is not closed.
   glDisable(GL_CULL_FACE);

   // compute the z-distances for each split as seen in camera space
   updateSplitDist(f, 1.0f, FAR_DIST);

   // for all shadow maps:
   for (int i=0; i<cur_num_splits; i++)
   {
      // compute the camera frustum slice boundary points in world space
      updateFrustumPoints(f[i], vec3f(cam_pos), vec3f(cam_view));
      // adjust the view frustum of the light, so that it encloses the camera frustum slice fully.
      // note that this function sets the projection matrix as it sees best fit
      // minZ is just for optimization to cull trees that do not affect the shadows
      float minZ = applyCropMatrix(f[i]);
      // make the current depth map a rendering target
      glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, depth_tex_ar, 0, i);

      // clear the depth texture from last time
      glClear(GL_DEPTH_BUFFER_BIT);

      // draw the scene
      terrain->Draw(minZ);

      glMatrixMode(GL_PROJECTION);
      // store the product of all shadow matries for later
      glMultMatrixf(shad_modelview);
      glGetFloatv(GL_PROJECTION_MATRIX, shad_cpm[i]);
   }
}
*/


/*
 * 09/07/2010
     It is here
*/
