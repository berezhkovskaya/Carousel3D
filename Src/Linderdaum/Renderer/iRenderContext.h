/**
 * \file iRenderContext.h
 * \brief Abstract render context representation
 * \version 0.5.91
 * \date 30/07/2010
 * \author Sergey Kosarevsky, 2004-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iRenderContext_
#define _iRenderContext_

#include "Platform.h"
#include "Core/iObject.h"
#include "Renderer/iRenderTarget.h"
#include "Renderer/RenderState.h"
#include "Math/LMatrix.h"
#include "Math/LProjection.h"
#include "Utils/LArray.h"

class iQuery;
class iGPUBuffer;
class iVertexArray;
class iHeightDataProvider;
class iHeightMapFacetter;
class clViewport;
class clCanvas;
class clVAMender;
class clCVar;
class clBitmap;
class clGlyphsCache;

class clRenderingTechnique;
class clFilterPostprocessor;
class iPostprocessor;

/// Information about current rendering system
struct sVideoSystemInfo
{
public:
	LString    FVersion;
	LString    FVendor;
	LString    FRenderer;
	LString    FExtensions;
	LString    FShadingLangVersion;
	/// maximal texture units available for the this context
	int        FMaxTextureUnits;
	/// maximal texture dimensions allowed for the this context
	int        FMaxTextureSize;
	/// maximal texture units available for vertex texture fetches
	int        FMaxVertexTextureUnits;
	/// maximal texture units available for both vertex and fragment programs
	int        FMaxCombinedTextureUnits;
	/// maximal number of fragment shader uniform variables
	int        FMaxFragmentUniformVectors;
};

/// Bitwise image of matrices layout in "Layout.sp"
struct sMatrices
{
	sMatrices(): in_ProjectionMatrix(          LMatrix4::Identity() ),
		in_ViewMatrix(                LMatrix4::Identity() ),
		in_ViewMatrixInverse(         LMatrix4::Identity() ),
		in_ModelMatrix(               LMatrix4::Identity() ),
		in_ModelMatrixInverse(        LMatrix4::Identity() ),
		in_ModelViewMatrix(           LMatrix4::Identity() ),
		in_ModelViewMatrixInverse(    LMatrix4::Identity() ),
		in_ModelViewProjectionMatrix( LMatrix4::Identity() ),
		in_NormalMatrix(              LMatrix4::Identity() )
	{
		FMatricesBuffer      = NULL;
		FMatricesBufferDirty = true;

		// default projection
		SetProjectionMatrixDefaultOrtho();
	};
	~sMatrices(); // non-virtual
	//
	//
	//
	inline void     SetProjectionMatrix( const LMatrix4& Matrix );
	inline void     SetProjectionMatrixDefaultOrtho();
	inline LMatrix4 GetProjectionMatrix() const
	{
		return in_ProjectionMatrix;
	};
	inline void     SetViewMatrix( const LMatrix4& Matrix );
	inline LMatrix4 GetViewMatrix() const
	{
		return in_ViewMatrix;
	};
	inline void     SetModelMatrix( const LMatrix4& Matrix );
	inline LMatrix4 GetModelMatrix() const
	{
		return in_ModelMatrix;
	};
	inline LMatrix4 GetModelViewMatrix() const
	{
		return in_ModelMatrix * in_ViewMatrix;
	};
//protected:
	void     UpdateMatricesBuffer() const;
private:
#pragma region Layout (see layout.sp)
	LMatrix4    in_ProjectionMatrix;
	LMatrix4    in_ViewMatrix;
	mutable LMatrix4    in_ViewMatrixInverse;
	LMatrix4    in_ModelMatrix;
	mutable LMatrix4    in_ModelMatrixInverse;
	mutable LMatrix4    in_ModelViewMatrix;
	mutable LMatrix4    in_ModelViewMatrixInverse;
	mutable LMatrix4    in_ModelViewProjectionMatrix;
	mutable LMatrix4    in_NormalMatrix;
#pragma endregion

	friend class clGLSLShaderProgram;
	friend class clSoftRenderContext;
private:
	iGPUBuffer*     FMatricesBuffer;
	mutable bool    FMatricesBufferDirty;
};

/// Abstract render context representation
class scriptfinal iRenderContext: public iObject
{
public:
	iRenderContext();
	virtual ~iRenderContext();

	NET_EXPORTABLE()

	//
	// iRenderContext
	//

#pragma region Initialization
	/**
	  Creates a rendering context with specified params and ataches it to the
	  viewport. Assumed that fullscreennes had been set prior to this call.
	**/
	virtual bool    InitContext( clViewport* Viewport, int BitsPerPixel, int zBufferBits, int StencilBits, int Multisample, bool TakeoverContext );
#pragma endregion

#pragma region Frames management
	virtual void    BeginFrame();
	virtual void    EndFrame( bool SwapBuffer );
#pragma endregion

#pragma region Overall statistics about all render targets
	virtual size_t    GetRenderedTrianglesCount() const;
	inline  void      UpdateStatsTriangles( size_t Triangles )
	{
		FRenderedTriangles += Triangles;
	};
#pragma endregion

#pragma region Utility

	/// Read single pixel from the framebuffer. Internal function for objects picking, do not use heavily.
	virtual Luint              GetPixel( const LVector2& Pnt ) const = 0;

	/// Resize viewport using specified parameters
	virtual void               SetViewport( int X, int Y, int Width, int Height ) = 0;

	/// Resize viewport using parameters in Vec4
	virtual void               SetViewportV( const LVector4& ViewportSize ) = 0;

	/// Set default values taken from Env.Viewport
	virtual void               RestoreViewport() = 0;

	/// Set "background" color
	virtual void               SetClearColor4v( const LVector4& Color ) const = 0;

	/// Determine screenshot size in bytes
	virtual int                GetScreenshotSize() const = 0;

	/// Internal screenshot to raw image
	virtual void               GetScreenshot( void* Ptr ) const = 0;

	/// Save the screenshot to RGB image
	virtual clBitmap*          MakeScreenshot();

#pragma endregion

#pragma region Capabilities and extensions
	/// Returns information about current renderer vendor, type and version, shading language version
	virtual sVideoSystemInfo   GetVideoSystemInfo() const = 0;

	/// Return total GPU memory
	virtual Lint64    GetTotalVideoMemory() const = 0;

	/// Return currently free GPU memory
	virtual Lint64    GetFreeVideoMemory() const = 0;

	/// Unique string for currently used graphics hardware. Used for caching
	virtual LString GetRendererSignature() const;

	/// Returns true if ExtName is supported. Terminated the program if Optional is false and ExtName is not supported.
	virtual bool GuardExtension( bool Optional, const LString& ExtName );

	/// Terminates application if the renderer version is lower then Major.Minor
	virtual void GuardVersion( int Major, int Minor, const LString& ErrorMessage );

	/// Terminates application if error occured
	virtual void    CheckError( const LString& File, const LString& Line, const LString& Info ) const = 0;
#pragma endregion

#pragma region Data feeding
	virtual void    AddBuffer( iVertexArray* VertexArray, clRenderState* Shader, int Instances, bool Wireframe );

	/// Clears specified render buffers of currently binded render target
	virtual void    ClearRenderTarget( bool Color, bool Depth, bool Stencil ) = 0;

	/// Toggles solid (fill) and wireframe rendering
	virtual void    SetPolygonFillMode( bool Fill ) = 0;

	/// Wait until all previous rendering operations are finished
	virtual void    Finish() = 0;
#pragma endregion

#pragma region Factories
	/**
	  Creates an offscreen render target.
	**/
	virtual iRenderTarget* CreateRenderTarget( const int  DimX,
	                                           const int  DimY,
	                                           const int  DimZ,
	                                           const int  BitsPerChannel,
	                                           const bool DepthBuffer,
	                                           const int  MRTCount ) const;
	virtual iRenderTarget* CreateRenderTargetV( const LArray<LVector4i>& WidthHeightDepthBitsPerChannel,
	                                            const bool HasDepthBuffer );

	/// Creates an empty buffer object
	virtual iGPUBuffer* CreateBuffer() const;

	/**
	  Creates an empty texture to which bitmap could be attached
	**/
	virtual iTexture* CreateTexture() const;
	/**
	  Creates an empty shader program which could be parametrized lately
	**/
	virtual iShaderProgram* CreateShaderProgram() const;
	/**
	  Returns a default empty shader
	**/
	virtual clRenderState* GetEmptyShader() const;
	/**
	  Returns a drawing canvas for instant 2D or 3D rendering
	**/
	virtual clCanvas*    GetCanvas() const;
	/**
	  Returns a glyphs cache
	**/
	virtual clGlyphsCache* GetGlyphsCache() const;
	/**
	  Creates new vertex array mender
	**/
	virtual clVAMender*        CreateVAMender() const;
	/**
	  Create new queue (occlusion, rendering time, etc)
	**/
	virtual iQuery*    CreateQuery() const;

	/// Create empty vertex array
	virtual iVertexArray*      AllocateEmptyVA() const;

	/// Create postprocessing chain
	virtual clRenderingTechnique* CreatePostprocessChain() const;

	/// Create a single postprocessor
	virtual iPostprocessor* CreatePostprocessor( const LString& PProcName ) const;

	/// Create a postprocessor for FilterName shader controlled by EnableVarName console var
	virtual clFilterPostprocessor* CreateFilterPostprocessor( const LString& FilterName, const LString& EnableVarName ) const;

	virtual clPtr<iHeightDataProvider>    CreateHeightDataProvider( const int SizeX,
	                                                                const int SizeY,
	                                                                const int Bits,
	                                                                const LString& RawFileName ) const;

	virtual clPtr<iHeightMapFacetter>     CreateHeightMapFacetter() const;
#pragma endregion

#pragma region Render states setup
	/// Make the state current
	virtual void    SetState( clRenderState* State ) = 0;

	/** Make the state current at the specified RenderContext.
	    Only state differences with CurrentState are passed to graphical API
	**/
	virtual void    UpdateState( clRenderState* State ) = 0;
#pragma region

#pragma region Properties
	/* Property(Description="Clearing color",    Category="Parameters", Type=vec4, Name=ClearColor,       Setter=SetClearColor4v) */
	/* Property(Description="Fill mode",         Category="Parameters", Type=bool, Name=PolygonFillMode,  Setter=SetPolygonFillMode) */
	/* Property(Description="Native canvas",     Category="Subobjects", Name=Canvas, Type=clCanvas, Getter=GetCanvas) */
#pragma endregion

	// events
	FWD_EVENT_HANDLER( Event_DRAWOVERLAY );

	void     RemoveTexture( iTexture* Texture );
private:
	void     ShowLogoC( const LString& Param );
	void     ShowSplashC( const LString& Param );
	void     SaveScreenshotC( const LString& Param );
	void     SetClearColorC( const LString& Param );
	void     RendererInfoC( const LString& Param );
	void     DumpTexturesC( const LString& Param );
protected:
	void                   InitBuffers();
	inline void            SetCurrentBindedSP( iShaderProgram* SP ) { FCurrentBindedSP = SP; };
	inline iShaderProgram* GetCurrentBindedSP() const { return FCurrentBindedSP; };
	friend class clGLSLShaderProgram;
private:
	iShaderProgram* FCurrentBindedSP;
	mutable clRenderState*     FEmptyShader;
	/// drawing canvas for immediate mode
	mutable clCanvas*         FCanvas;
	/// cache for glyphs rendering
	mutable clGlyphsCache*    FGlyphsCache;
	/// true if we are within BeginFrame()/EndFrame() pair
	bool               FInsideFrame;
#pragma region Stats counters and modes
private:
	clCVar*            FDips;
	clCVar*            FDipsBlended;
	clCVar*            FWireFrame;
	mutable std::vector<iTexture*> FCreatedTextures;
protected:
	clCVar*            FStateChanges;
#pragma endregion
protected:
	bool               FTakeoverContext;
	clRenderState*     FCurrentState;
	// counters
	size_t             FRenderedTriangles;
	// metainfo
	LString    FClassName_RenderTarget;
	LString    FClassName_ShaderProgram;
	LString    FClassName_Texture;
	LString    FClassName_VertexArray;
	LString    FClassName_Query;
	LString    FClassName_Buffer;
};

void sMatrices::SetProjectionMatrix( const LMatrix4& Matrix )
{
	in_ProjectionMatrix = Matrix;

	FMatricesBufferDirty = true;
}

void sMatrices::SetProjectionMatrixDefaultOrtho()
{
	SetProjectionMatrix( Math::Ortho2D( 0.0f, 1.0f, 1.0f, 0.0f ) );
}

void sMatrices::SetViewMatrix( const LMatrix4& Matrix )
{
	in_ViewMatrix = Matrix;

	FMatricesBufferDirty = true;
}

void sMatrices::SetModelMatrix( const LMatrix4& Matrix )
{
	in_ModelMatrix = Matrix;

	FMatricesBufferDirty = true;
}

#endif

/*
 * 30/07/2010
     RendererInfoC()
 * 19/06/2010
     GetTotalVideoMemory()
     GetFreeVideoMemory()
 * 11/08/2009
     ShowLogoC()
     SaveScreenshotC()
 * 15/02/2009
     GetScreenshotSize()
     GetScreenshot()
 * 06/04/2007
     UpdateStats()
 * 26/12/2005
     SetViewport()
     RestoreViewport()
 * 25/03/2005
     GetDriverSignature()
 * 12/01/2005
     BeginTransform()
     EndTransform()
 * 15/12/2004
     CreateTTFont()
     SetProjection()
 * 13/12/2004
     GetFrameBuffer()
     CreateRenderTarget()
 * 09/12/2004
     SwapBuffers()
 * 08/12/2004
     It's here
*/
