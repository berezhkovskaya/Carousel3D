/**
 * \file VolumeRenderer.h
 * \brief Linderdaum Volume Rendering Library - object-oriented interface
 * \version 0.5.89a
 * \date 23/03/2009
 * \author Sergey Kosarevsky, 2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LVVolume_
#define _LVVolume_

#include "Core/iObject.h"
#include "Renderer/iRenderContext.h"
#include "Math/LMatrix.h"

#include <map>

class clImage;
class clGeom;
class clScene;
class iTexture;
class iVertexArray;
class iRenderTarget;
class iShaderProgram;
class clRenderState;

#pragma region Enumerations

/// Predefined volume rendering modes
enum LVolumeRenderMode
{
	// for debugging
	L_VRMODE_RESERVED   = 0,
	L_VRMODE_BACKFACES  = 1,
	L_VRMODE_FRONTFACES = 2,
	L_VRMODE_BACKGROUND = 3,
	// for users
	L_VRMODE_ISOSURFACE = 4,
	L_VRMODE_OPACITY    = 5,
	L_VRMODE_MIP        = 6
};

/// Predefined volume rendering materials
enum LVolumeRenderMaterial
{
	// for debugging
	L_VRMATERIAL_RESERVED     = 0,
	L_VRMATERIAL_GRADIENTS    = 1,
	// default material (Phong model)
	L_VRMATERIAL_DEFAULT      = 2,
	// procedural materials
	L_VRMATERIAL_MARBLE       = 3,
	L_VRMATERIAL_WOOD         = 4,
	L_VRMATERIAL_IVORY        = 5
};

/// Gradients calculation modes
enum LGradientsMode
{
	L_VRGRADIENTS_CENDERDIFF   = 0,
	L_VRGRADIENTS_FORWARDDIFF  = 1,
	L_VRGRADIENTS_PRECALCSOBEL = 3,
	L_VRGRADIENTS_SOBEL        = 4  // do not enable on cards below GeForce 260
};

/// Transfer function modes
enum LTransferFunctionMode
{
	L_TFMODE_NONE     = 0,
	L_TFMODE_1D       = 1,
	L_TFMODE_1DPREINT = 2,
	L_TFMODE_2D       = 3
};

#pragma endregion

/// Internal selector for transfer function mode. Used automatically.
enum LTransferFunctionMode;

/// Internal helper class for LVLib implementation
class netexportable scriptfinal clLVLibVolume: public iObject
{
public:
	clLVLibVolume();
	virtual ~clLVLibVolume();

	//
	// iObject interface
	//
	virtual void    AfterConstruction();
	//
	// clLVLibVolume
	//
	virtual void SetOffscreenRenderTarget( iRenderTarget* RT );
	virtual bool AutoLoadVolumeAndGradients( const LString& FileName,
	                                         int SizeX,
	                                         int SizeY,
	                                         int SizeZ,
	                                         int BitsPerPixel,
	                                         bool Scale,
	                                         bool PrecomputeGradient,
	                                         bool CreateESLMap );

	/** Property(Category="Rendering parameters", Description="Number of tracing steps", Name=Iterations, Type=int, Getter= GetIterations, Setter=SetIterations) */

	/** Property(Category="Transformations", Description="ModelView matrix", Name=ModelView, Type=mtx4, Setter=SetModelView) */
	/** Property(Category="Transformations", Description="Projection matrix", Name=Projection, Type=mtx4, Setter=SetProjection) */

	/** Pro_per_ty(Category="Textures", Description="Gradients image", Name=Gradients, Type=clImage, Setter=SetGradients) */
	/** Pro_per_ty(Category="Textures", Description="Volume image", Name=Volume, Type=clImage, Setter=SetVolume) */
	/** Pro_per_ty(Category="Textures", Description="Empty space leaping map", Name=ESLMap, Type=clImage, Setter=SetESLMap) */

	/** Property(Category="Rendering parameters", Description="Scattering flag", Name=Scattering, Type=bool, Getter=GetScattering, Setter=SetScattering) */
	/** Property(Category="Rendering parameters", Description="Isosurface value [0..1]", Name=IsoValue, Type=float, Getter=GetIsosurfaceValue, Setter=SetIsosurfaceValue) */
	/** Property(Category="Rendering parameters", Description="Isosurface color", Name=IsoColor, Type=vec4, Getter=GetIsosurfaceColor, Setter=SetIsosurfaceColor ) */

	/** Prop_erty(Category="Rendering parameters", Description="Isosurface material", Name=IsoMaterial, Type=LVolumeRenderMaterial, Getter=GetIsosurfaceMaterial, Setter=SetIsosurfaceMaterial ) */
	/** Property(Category="Rendering parameters", Description="Low cutoff", Name=LowCutoff, Type=float, Getter=GetLowCutoff, Setter=SetLowCutoff ) */
	/** Property(Category="Rendering parameters", Description="Opacity saturation", Name=Saturation, Type=float, Getter=GetOpacitySaturation, Setter=SetOpacitySaturation ) */
	/** Prop_erty(Category="Rendering parameters", Description="Render mode", Name=RenderMode, Type=LVolumeRenderMode, Getter=GetRenderMode, Setter=SetRenderMode) */

	/// Render the volume
	virtual void           Render();
	/// Set image representing volume
	virtual void           SetVolume( clImage* Volume );
	/// Get volume image
	virtual clImage*       GetVolume();
	/// Set volume texture and update size parameters
	virtual void           SetVolumeTexture( iTexture* Volume );
	/// Change the volume without affecting size parameters (used in dynamic simulations with ping-pong schemes)
	virtual void           ChangeVolumeTexture( iTexture* Volume );
	/// Set actual texture box size (in logical units, not in pixels). This specifies proportions, not the absolute size
	virtual void           SetVolumeSize( float SizeX, float SizeY, float SizeZ );
	/// Set volume gradients image
	virtual void           SetGradients( clImage* Gradients );
	/// Set volume EmptySpaceLeaping map
	virtual void           SetESLMap( clImage* ESLMap );
	/// Assign used projection matrix
	virtual void           SetProjection( const LMatrix4& Projection );
	/// Assign used modelview matrix
	virtual void           SetModelView( const LMatrix4& ModelView );
	/// Enable rendering of the downscaled offscreen buffer (low-res image)
	virtual void           SetRenderOffscreen( bool Offscreen );
	/// Select gradient calculation mode
	virtual void           SetGradientsMode( LGradientsMode Mode );
	virtual void           SetScattering( bool Scattering );
	virtual bool           GetScattering() const { return FScattering; };
	virtual void           SetIsosurfaceValue( const float IsoValue );
	virtual float          GetIsosurfaceValue() const { return FL_ISO_VALUE; };
	virtual void           SetIsosurfaceColor( const LVector4& RGBColor );
	virtual LVector4       GetIsosurfaceColor() const { return FL_MAT_COLOR; };
	virtual void           SetLightPosition( const LVector3& LightPos );
	virtual void           EnableClipPlane( const LVector4& ClippingPlane, const LVector4& ClipColor, float Thickness );
	virtual void           DisableClipPlane();
	virtual void           SetClipPlaneParams( const LVector4& ClippingPlane, const LVector4& ClipColor, float Thickness );
	virtual void           SetIsosurfaceMaterial( LVolumeRenderMaterial Material );
	virtual LVolumeRenderMaterial GetIsosurfaceMaterial() const { return FRenderMaterial; };
	virtual void           SetLowCutoff( const float Value );
	virtual float          GetLowCutoff() const { return FL_CUT_OFF_LOW; };
	virtual void           SetOpacitySaturation( float Value );
	virtual float          GetOpacitySaturation() const { return FL_SATURATION; };
	virtual void           SetIterations( int Iterations );
	virtual int            GetIterations() const { return static_cast<int>( FL_TRACING_STEPS ); };
	virtual void           SetRenderMode( LVolumeRenderMode Mode );
	virtual LVolumeRenderMode GetRenderMode() const { return FRenderMode; };
	virtual void           SetTransferFunction( clImage* TF, LTransferFunctionMode TFMode );
	virtual void           Set2DTextureMap( clImage* Map, const LVector2& Scale );
	virtual clGeom*        CreateVolumeGeom() const;
	// return handle of the added geom inside the scene
	virtual int            AddToScene( clScene* Scene ) const;
	FWD_EVENT_HANDLER( Event_VIEWPORTRESIZED );
	FWD_EVENT_HANDLER( Event_CHANGED );
	FWD_EVENT_HANDLER( Event_CHANGED_VOLUME );
private:
	virtual void            SetVolumeSize_Internal( iShaderProgram* Prog, float SizeX, float SizeY, float SizeZ );
	virtual clRenderState*  CreateShader( iShaderProgram* Prog );
	virtual LString         GetPropertiesString();
	virtual void            RelinkShader();
	virtual void            BindParameters( iShaderProgram* Prog );
private:
	bool               FMVPChanged;
	bool               FViewportNeedResize;
	bool               FUpdateModel;
	bool               FCameraInsideVolume;
	LMatrix4           FOldProjection;
	LMatrix4           FOldModelView;
	sMatrices          FMatrices;
	bool               FVolumeLoaded;
	bool               FNeedRelink;
	bool               FRenderOffscreen;
	bool               FScattering;
	bool               FUse2DMap;
	bool               FUseESL;
	iRenderTarget*     FOffscreenBufferInternal;
	iRenderTarget*     FOffscreenBuffer;
	iRenderTarget*     FOffscreenBufferFinal;
	iVertexArray*      FProxyGeometry;

#pragma region Render states and shader programs
	clRenderState*                      FVolumeShader;
	clRenderState*                      FBlenderShader;
	iShaderProgram*                     FVolumeProg;
	std::map<LString, iShaderProgram*>  FPrecompiledPrograms;
#pragma endregion

#pragma region Rendering parameters
	LVector4 FL_MAT_COLOR;
	LVector3 FL_LIGHT_POS;
	float FL_SATURATION;
	float FL_CUT_OFF_LOW;
	float FL_ISO_VALUE;
	float FL_TRACING_STEPS;
	LVector4 FL_CUT_PLANE;
	float FL_CUT_PLANE_THICKNESS;
	LVector4 FL_CUT_PLANE_COLOR;
	LVector3 FL_SCALE_2DMAP;
#pragma endregion

#pragma region GLSL Properties
	float                    FSizeX;
	float                    FSizeY;
	float                    FSizeZ;
	LVolumeRenderMode        FRenderMode;
	LVolumeRenderMaterial    FRenderMaterial;
	LGradientsMode           FGradientsMode;
	LTransferFunctionMode    FTransferFunctionMode;
	bool                     FClipPlane;
#pragma endregion
};

#endif

/*
 * 23/03/2009
     It's here
*/
