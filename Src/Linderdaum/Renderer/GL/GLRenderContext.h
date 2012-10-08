/**
 * \file GLRenderContext.h
 * \brief OpenGL rendering context
 * \version 0.5.91
 * \date 23/08/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _GLRenderContext_
#define _GLRenderContext_

#include "Platform.h"

#ifdef OS_WINDOWS
#include <windows.h>
#undef min
#undef max
#endif

#if defined( OS_ANDROID ) && defined( ANDROID9 )
#include <EGL/egl.h>
#endif

#include "Renderer/iRenderContext.h"
#include "Generated/LGL/LGL.h"

#include "GLAPIContainer.h"

class clCVar;
class clViewport;

namespace LGL
{
	class clGLExtRetriever;
};

class clGLDebugger;

/// OpenGL rendering context
class scriptfinal clGLRenderContext: public iRenderContext, public clGLAPIContainer
{
public:
	clGLRenderContext();
	virtual ~clGLRenderContext();
	//
	// iObject interface
	//
	virtual void                AfterConstruction();
	//
	// iRenderContext interface
	//
	virtual bool                InitContext( clViewport* Viewport, int BitsPerPixel, int zBufferBits, int StencilBits, int Multisample, bool TakeoverContext );
	virtual void                EndFrame( bool SwapBuffer );
	virtual void                CheckError( const LString& File, const LString& Line, const LString& Info ) const;
	virtual Luint               GetPixel( const LVector2& Pnt ) const;
	virtual void                SetViewport( int X, int Y, int Width, int Height );
	virtual void                SetViewportV( const LVector4& ViewportSize );
	virtual void                RestoreViewport();
	virtual void                SetClearColor4v( const LVector4& Color ) const;
	virtual int                 GetScreenshotSize() const;
	virtual void                GetScreenshot( void* Ptr ) const;
	virtual sVideoSystemInfo    GetVideoSystemInfo() const;
	virtual Lint64              GetTotalVideoMemory() const;
	virtual Lint64              GetFreeVideoMemory() const;
	virtual bool                GuardExtension( bool Optional, const LString& ExtName );
	virtual void                ClearRenderTarget( bool Color, bool Depth, bool Stencil );
	virtual void                SetPolygonFillMode( bool Fill );
	virtual void                Finish();
	virtual void                SetState( clRenderState* State );
	virtual void                UpdateState( clRenderState* State );
	virtual iTexture*           CreateTexture() const;
	//
	// clGLRenderContext
	//
	virtual sLGLAPI*            GetLGLAPI() const;
	virtual void                SetActiveTextureUnit( int TextureUnit );
	virtual void                SetActiveVBO( int ArrayBuffer, int ElementBuffer );

	FWD_EVENT_HANDLER( Event_SURFACE_CHANGED );
private:
	void             CheckExtensions();
	void             LogExtensions();
	sVideoSystemInfo RecheckVideoSystemInfo() const;

#pragma region Render state setup helpers
	inline void    SetupBackfaceCulling( bool BackfaceCulling, bool BackFacing );
	inline void    SetupDepthTest( bool DepthTest );
	inline void    SetupDepthWrites( bool DepthWrites );
	inline void    SetupColorWrites( bool ColorWrites );
#pragma endregion

private:
	LGL::clGLExtRetriever* FOpenGL;
	clGLDebugger*          FDebugger;
	clViewport*            FViewport;
	GL_CONTEXT_TYPE        FRenderContext;
	int                    FCurrentViewportWidth;
	int                    FCurrentViewportHeight;
	// caches
	int    FActiveTextureUnit;
	int    FActiveVBO_ArrayBuffer;
	int    FActiveVBO_ElementBuffer;
	// states
	sVideoSystemInfo   FCurrentVideoInfo;
};

#endif

/*
 * 23/08/2010
     SetState() and UpdateState() moved from clRenderState
 * 19/06/2010
     GetTotalVideoMemory()
     GetFreeVideoMemory()
 * 15/02/2009
     GetScreenshotSize()
     GetScreenshot()
 * 25/06/2007
     SetPolygonFillMode()
 * 06/04/2007
     UpdateStats() made public
 * 13/11/2006
     CreateContext() prototype changed
 * 13/07/2006
     SetCurrentColor()
 * 18/01/2006
     SetFOV() removed
     SetProjection() removed
 * 09/12/2005
     ShowLogo() is now a console command
 * 27/11/2005
     Decoupled from iRenderTarget
 * 14/12/2004
     Multiple inheritance from iRenderContext & iRenderTarget
 * 08/12/2004
     It's here
*/
