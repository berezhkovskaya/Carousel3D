/**
 * \file RenderingTechnique.h
 * \brief Default OpenGL rendering technique interface
 * \version 0.5.71
 * \date 01/01/2007
 * \author Sergey Kosarevsky, 2005-2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clRenderingTechnique_
#define _clRenderingTechnique_

#include "Core/iObject.h"
#include "Utils/LArray.h"

class iPostprocessor;
class iRenderTarget;
class iTexture;
class clCVar;
class clScene;

/// Default forward rendering technique with postprocessing pipeline
class scriptfinal netexportable clRenderingTechnique: public iObject
{
public:
	clRenderingTechnique();
	virtual ~clRenderingTechnique();

	virtual void    AfterConstruction();

	/// Render the scene to internal buffer and apply postprocessing pipeline to that buffer
	scriptmethod void      Render( clScene* Scene );

	/// Process the user-specified offscreen buffer. OutBuffer can be NULL - then the output is the screen
	scriptmethod void      ProcessBuffer( iRenderTarget* InBuffer, iRenderTarget* OutBuffer );

#pragma region  Manipulate postprocessing effects

	scriptmethod void            AddPostprocessor( iPostprocessor* Postprocessor );
	scriptmethod void            ClearPostprocessingPipeline();
	scriptmethod iPostprocessor* GetPostprocessor( int i ) const { return FPostprocessingPipeline[i]; }
	scriptmethod void            SetPostprocessor( int i, iPostprocessor* PP ) { FPostprocessingPipeline[i] = PP; }
	scriptmethod int             GetPostprocessorsCount() const { return static_cast<int>( FPostprocessingPipeline.size() ); }

	/// Delete all internal buffers
	scriptmethod void            Reset();

	/// Process user-specified render target with current postprocessing chain
	scriptmethod void            RenderPostprocessingPipeline( iRenderTarget* SceneRenderTarget ) { ProcessBuffer( SceneRenderTarget, NULL ); }

#pragma endregion

	scriptmethod iRenderTarget*  GetRenderTarget( int i ) const               { return FRenderTargets[i]; }
	scriptmethod void            SetRenderTarget( int i, iRenderTarget* Tgt ) { FRenderTargets[i] = Tgt;  }

	/// For debugging: show texture on screen
	scriptmethod void      Debug_ShowTexture( iTexture* Texture );

	///  For debugging: show depth buffer on screen
	scriptmethod void      Debug_ShowDepth( bool ShowDepth );
public:
	/* Property(Name="Postprocessors", Type=iPostprocessor, IndexType=int, FieldName=FPostprocessingPipeline, NetIndexedGetter=GetPostprocessor, NetIndexedSetter=SetPostprocessor ) */
	/* Property(Name="RenderTargets",  Type=iRenderTarget,  IndexType=int, FieldName=FRenderTargets,          NetIndexedGetter=GetRenderTarget,  NetIndexedSetter=SetRenderTarget  ) */
	LArray<iPostprocessor*>    FPostprocessingPipeline;
	LArray<iRenderTarget*>     FRenderTargets;
private:
	iRenderTarget*    GetCustomRenderTarget( const LString& Name );
private:
	iTexture*    FDebugShowTexture;
	bool         FDebugShowDepth;
	bool         FDebugEnablePostProcessing;
	/// enable/disable postprocessing
	clCVar*      FPostProcessing;
};

#endif

/*
 * 01/01/2007
     New fields for brightpass
 * 15/12/2005
     It's here
*/
