/**
 * \file FlowUI.h
 * \brief 3D flow list renderer
 * \version 0.6.02
 * \date 24/12/2011
 * \author Viktor Latypov, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __FlowUI__h__included__
#define __FlowUI__h__included__

#include <vector>

#include "Environment.h"
#include "Core/Linker.h"
#include "Utils/Viewport.h"
#include "Input/FlowSelector.h"
#include "Input/Gestures.h"
#include "Images/Image.h"
#include "Renderer/Canvas.h"
#include "Renderer/RenderState.h"
#include "Renderer/iShaderProgram.h"
#include "Resources/ResourcesManager.h"
#include "Scene/Scene.h"
#include "Scene/Material.h"
#include "Geometry/Geom.h"
#include "Math/LPlane.h"

/**
   \brief Renderer for the image flow

   Uses FlowSelector object to set the transforms for the sequence of images
*/
class scriptfinal clFlowSelectRenderer: public iObject
{
public:
	clFlowSelectRenderer(): FImageDist( 1.5f ), FQuadSize( 3.5f ) {}

	//
	// iObject interface
	//
	virtual void AfterConstruction();

	//
	// clFlowSelectRenderer
	//

	virtual void SetTransforms( clFlowSelector* Control, float t_current, int CurImg );

	virtual void SetImages( const std::vector<clImage*>& Imgs );

	virtual size_t GetNumImages() const { return FImages.size(); }

	virtual void RenderDirect( const mtx4& Projection, const mtx4& View, int CurImg, const LVector4& OverlayColor );

	virtual bool IsSelected( int i ) const { return FSelected[i]; }
	virtual void SetSelected( int i, bool f ) { FSelected[i] = f; }

public:
	/// Distance between images
	float FImageDist;

	/// Size of one tile
	float FQuadSize;

	/// The size of one tile in the sequence. Used in calculations
	inline float OneImageSize() const { return ( FQuadSize + FImageDist ); }
private:

	/// Quad coordinates for each image
	std::vector<vec3> FCoords[4];

	/// Reflected quad coordinates for each image
	std::vector<vec3> FCoords_R[4];

	/// Normal shader
	clRenderState* FShader;

	/// Reflection shader
	clRenderState* FShader_R;

	/// Selected image shader
	clRenderState* FShader_S;

	/// Reflection mask
	clImage* FDarkImage;

	/// Selection mask
	clImage* FSelImage;

	/// Loaded images
	std::vector<clImage*> FImages;

	/// Selection flags for each image
	std::vector<bool> FSelected;
};

/**
   \brief Composite object containing scene, flowselector, flowrenderer and pos/angle camera calculation code

   TODO: add event handling
*/
class scriptfinal clFlowUI: public iObject
{
public:
	clFlowUI(): FFlinger( NULL ), FFlow( NULL ), FRenderer( NULL ) {}

	virtual ~clFlowUI()
	{
		delete( FFlinger );
		delete( FFlow );
		delete( FRenderer );
	}

	virtual void AfterConstruction()
	{
		iObject::AfterConstruction();

		FRenderer = Construct<clFlowSelectRenderer>( Env );

		mtx4 RotationMatrix;
		RotationMatrix.FromPitchPanRoll( 0.0f, -90.0f, 0.0f );

		FView = mtx4::GetTranslateMatrix( -vec3( 0.0f, -13.2f, 1.2f ) ) * RotationMatrix;
		FProjection = Math::Perspective( 45.0f, Env->Viewport->GetAspectRatio(), 0.4f, 2000.0f );

		FFlinger = Env->Linker->Instantiate( "clFlingHandler" );
		FFlinger->SetActive( true );
		FFlinger->SetStrokeMode();
		FFlinger->FStepGranularity = FFlinger->FValueGranularity = vec2( FRenderer->OneImageSize() );

		FFlow = Construct<clFlowSelector>( Env );
		FFlow->RebuildCurves();
	}

	float GetVal() const { return FFlinger->GetValue().X; }
	void SetVal( float ff ) { FFlinger->SetValue( vec2( ff, FFlinger->GetValue().Y ) ); }

	scriptmethod void Render( const LVector4& OverlayColor )
	{
		FRenderer->SetTransforms( FFlow, GetVal(), GetCurrentImage() );
		FRenderer->RenderDirect( FProjection, FView, GetCurrentImage(), OverlayColor );
	}

	/// Get selection flag for i-th image
	scriptmethod bool IsSelected( int i ) const { return ( i > -1 && ( i < ( int )GetNumImages() ) ) ? FRenderer->IsSelected( i ) : false; }

	/// Set selection flag for i-th image
	scriptmethod void SetSelected( int i, bool f ) { if ( i > -1 && ( i < ( int )GetNumImages() ) ) { FRenderer->SetSelected( i, f ); } }

	/// Get the number of assigned images
	scriptmethod size_t GetNumImages() const { return FRenderer->GetNumImages(); }

	/// Load new images. TODO: refactor
	void UpdateImages( const std::vector<clImage*>& Imgs )
	{
		/// Pass to renderer
		FRenderer->SetImages( Imgs );

		/// Set flinger limits
		FFlinger->FMaxValue = vec2( 0, 0 );
		FFlinger->FMinValue = vec2( -( ( float ) Imgs.size() - 1.0f ) * FRenderer->OneImageSize(), 0.0f );
	}

	/// Which image is currently selected
	scriptmethod int GetCurrentImage() const
	{
		float Val = GetVal();
		int Offset = ( int )( -Val / ( FRenderer->OneImageSize() ) );
		return Offset;
	}

	/// Move to the specified image with no animation
	scriptmethod void SetCurrentImage( int i )
	{
		SetVal( -( float )i * ( FRenderer->OneImageSize() ) );
	}

private:
	/// Fling handler
	clFlingHandler* FFlinger;

	/// Flow selector
	clFlowSelector* FFlow;

	/// Flow renderer
	clFlowSelectRenderer* FRenderer;

	/// Static camera matrices
	mtx4 FView;
	mtx4 FProjection;
};

#endif
