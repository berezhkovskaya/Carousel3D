/**
 * \file LVLib.h
 * \brief Linderdaum Volume Rendering Library - procedural interface
 *
 * This library is a wraper for Linderdaum Engine to provide all users easy
 * out-of-the-box volume rendering capabilities.
 *
 * \version 0.5.89a
 * \date 06/05/2009
 * \author Sergey Kosarevsky, 2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LVLib_
#define _LVLib_

#include "Platform.h"
#include "LString.h"
#include "Renderer/VolumeRenderer.h"

class sEnvironment;
class iTexture;
class clImage;
class LVector2;
class LVector3;
class LVector4;
class LMatrix4;
class clLVLibVolume;


/// Linderdaum Volume Rendering Library
namespace LVLib
{

#pragma region Initialization methods
	/**
	   Initialize the engine and the volume rendering library.

	   \param CreateNewWindow - if 'false' the library will use already
	                            created OpenGL rendering context
	                            if 'true' then new OpenGL window and context will be created
	   \param ExternalWndHandle - handle to your existing OpenGL window
	**/
	void InitEngine_Simple( bool CreateNewWindow, void* ExternalWndHandle );

	/**
	   Initialize the volume rendering library using externaly initialized engine.
	   Can be used for manual control to do advanced things
	**/
	void InitEngine_External( sEnvironment* ExtEnvironment );

	/**
	   Shutdown the engine
	**/
	void DoneEngine();

	/**
	   Basic main loop (returns then application quit)
	**/
	void RunApplication();

	/**
	   Automatic rendering and update, should be called once per frame
	**/
	void RenderAndUpdate();

	/**
	   Render current frame without changing Engine's internal state
	**/
	void RenderFrame();

	/**
	   Environment is used to access instance of the engine operated inside LVLib
	**/
	sEnvironment* GetEnv();

#pragma endregion


#pragma region Volume rendering API

#pragma region Global settings, need to call before volume loading

	/**
	   This method returns recommended size of internal 3D textures. Can be
	   used as a parameter to SetVolumeDimensionLimit()
	**/
	int     GetRecommendedVolumeDimensionLimit();

	/**
	   This method returns recommended number of raycasting iterations.
	**/
	int     GetRecommendedIterations();

	/**
	   This method defines how large internal 3D textures can be. It depends
	   on available graphics memory and affects performance.

	   \param Limit - use 256 for 256 Mb video memory,
	                      512 for 1 Gb video memory,
	                      768 for 2 Gb video memory.
	                  or GetRecommendedVolumeDimensionLimit()
	**/
	void    SetVolumeDimensionLimit( int Limit );
#pragma endregion

#pragma region Volume initialization

	/**
	   This method allocates a new volume placeholder and returns its ID
	**/
	int     CreateVolume();

	/**
	   This method can hide and show allocated volume without unloading. Memory is not freed.
	**/
	bool    SetVolumeVisibility( int VolID, bool Visible );

	/**
	   This method destroys the volume. If failed then 'false' is returned
	**/
	bool    DeleteVolume( int VolID );

	/**
	   This method checks the validity of volume identifier.
	**/
	bool    IsValidID( int VolID );

	/**
	   This method returns object oriented interface to the volume.
	**/
	clLVLibVolume* GetVolume( int VolID );

	/**
	   This method loads volume from RAW file and precompute gradients if necessary

	   \param VolID - volume ID to attach data to
	   \param FileName - RAW file name
	   \param SizeX - width of volume in RAW file
	   \param SizeY - height of volume in RAW file
	   \param SizeZ - depth of volume in RAW file
	   \param BitsPerPixel - bits per pixel in RAW file
	   \param Scale - if 'false' volume will be rendered inside a unit cube 1,1,1
	                  if 'true' volume will be scaled proportionaly
	                  according to SizeX, SizeY, SizeZ
	   \param PrecomputeGradients - the engine will precompute gradients automaticaly
	                                if this is 'false' then L_VRGRADIENTS_PRECALCSOBEL gradients
	                                rendering mode will be unaccessable
	   \param CreateESLMap - the engine will precompute ESL map for empty space leaping
	**/
	bool    AutoLoadVolumeAndGradients( int VolID,
	                                    const LString& FileName,
	                                    int SizeX,
	                                    int SizeY,
	                                    int SizeZ,
	                                    int BitsPerPixel,
	                                    bool Scale,
	                                    bool PrecomputeGradient,
	                                    bool CreateESLMap );

	/**
	   This method can be used to manually attach volume data to volume,
	   i.e. if you want to load them from format not supported directly by the engine
	**/
	bool    SetVolume( int VolID, clImage* Volume );

	/**
	   Directly attach 3D texture as a volume
	**/
	bool    SetVolumeTexture( int VolID, iTexture* Volume );

	/**
	   This method can be used to manually attach precomputed gradients to volume,
	   i.e. if you want to precompute gradients with your proprietary algorithm
	**/
	bool    SetGradients( int VolID, clImage* Gradients );

	/**
	   This method can be used to manualy attach precimputed ESL map for empty scape leaping,
	   i.e. if you want to precompute ESL map with your proprietary algorithm
	**/
	bool    SetESLMap( int VolID, clImage* ESLMap );
#pragma endregion

#pragma region Rendering parameters

	/**
	   This method sets the projection matrix used to render this volume
	**/
	bool    SetProjection( int VolID, const LMatrix4& Projection );

	/**
	   This method sets the projection matrix used to render this volume
	**/
	bool    SetModelView( int VolID, const LMatrix4& ModelView );

	/**
	   This method enables offscreen rendering mode. The volume is rendered
	   into an offscreen render target and then copied on screen. The render
	   target can be of low resolution and can improve performance in exchange for quality
	   (you can turn this on i.e. while rotating the volume etc.)
	**/
	bool    SetRenderOffscreen( int VolID, bool Offscreen );

	/**
	   This method changes gradients rendering mode. To use precomputed gradients
	   they should be loaded beforehand
	**/
	bool    SetGradientsMode( int VolID, LGradientsMode Mode );

	/**
	   Adds scattering while computing surface lighting
	**/
	bool    SetScattering( int VolID, bool Scattering );

	/**
	   This method sets the isovalue used for isosurface rendering
	**/
	bool    SetIsosurfaceValue( int VolID, const float IsoValue );

	/**
	   This method sets the color of isosurface. Does not affect
	   procedural materials (like wood, marble etc.)
	**/
	bool    SetIsosurfaceColor( int VolID, const LVector4& RGBColor );

	/**
	   This method sets the light position
	**/
	bool    SetLightPosition( int VolID, const LVector3& LightPos );

	/**
	   This method enables clipping plane

	   \param VolID - volume ID
	   \param ClippingPlane - equation of clipping plane A,B,C,D in volume
	                          coordinates
	   \param ClipColor - color of material that is in the clipping plane
	   \param Thickness - thickness of the volume segment to be clipped,
	                      anything above 2.0f will disable segment clipping
	**/
	bool    EnableClipPlane( int VolID,
	                         const LVector4& ClippingPlane,
	                         const LVector4& ClipColor,
	                         float Thickness );

	/**
	   This method updated clipping plane parameters but don't enable or disable current clip plane

	   \param VolID - volume ID
	   \param ClippingPlane - equation of clipping plane A,B,C,D in volume
	                          coordinates
	   \param ClipColor - color of material that is in the clipping plane
	   \param Thickness - thickness of the volume segment to be clipped,
	                      anything above 2.0f will disable segment clipping
	**/
	bool    SetClipPlaneParams( int VolID,
	                            const LVector4& ClippingPlane,
	                            const LVector4& ClipColor,
	                            float Thickness );

	/**
	   This method disables clipping plane
	**/
	bool    DisableClipPlane( int VolID );

	/**
	   This method sets current material of isosurface
	**/
	bool    SetIsosurfaceMaterial( int VolID, const LVolumeRenderMaterial Material );

	/**
	   This method sets the lowest density inside the volume that will be
	   bypassed during opacity accumulation and MIP rendering. This can be used
	   to bypass noise in the volume data
	**/
	bool    SetLowCutoff( int VolID, const float Value );

	/**
	   This method sets how bright opacity accumulated image will be.
	   \param VolID - volume ID
	   \param Value - saturation multiplier
	**/
	bool    SetOpacitySaturation( int VolID, const float Value );

	/**
	   This method sets number of iterations for raycasting process. It
	   can be used to find a tradeoff between image quality and rendering
	   performance. Use GetRecommendedIterations() to get the initial guess
	   on how many iterations you want.

	   Number of iteration depends on the size of loaded volume.

	   For fastest rendering use 1:2 of the volume size, i.e. 128 for 256 volume
	   For normal rendering use 1:1 of the volume size, i.e. 256 for 256 volume
	   For highest quality rendering use 2:1 of the volume size, i.e. 512 for 256 volume
	**/
	bool    SetIterations( int VolID, const int Iterations );

	/**
	   This methods sets current rendering mode for the volume
	**/
	bool    SetRenderMode( int VolID, const LVolumeRenderMode Mode );

	/**
	   This method attaches transfer function to the volume and uses it.
	   Any previously attached transfer function will be abandoned.
	   NULL disables any transfer function

	   \param VolID - volume ID
	   \param TF - transfer function texture
	   \param TFMode - transfer function type (1D, 2D or 1D preintegrated)

	   Preintegrated transfer function can be used for high quality transparent isosurfaces rendering.
	   2D transfer function can be used for density/gradients lookup for high quality organic datasets rendering.
	**/
	bool    SetTransferFunction( int VolID, clImage* TF, LTransferFunctionMode TFMode );

	/**
	   This method applies 2D texture mapping for isosurface rendering with default material.
	   NULL disables 2D texture mappint.

	   \param VolID - volume ID
	   \param Map - texture map
	   \param Scale - scaling coefficient for texture (can be used to control tiling), use LVector2(1.0f) for default value
	**/
	bool    Set2DTextureMap( int VolID, clImage* Map, const LVector2& Scale );

#pragma endregion

#pragma endregion
};

#endif

/*
 * 06/05/2009
     GetVolume()
 * 01/05/2009
     Scattering and texturing
 * 24/03/2009
     Clipping planes interface
 * 23/03/2009
     It's here
*/
