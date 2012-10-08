/**
 * \file Canvas.h
 * \brief Drawing canvas
 * \version 0.5.99c
 * \date 25/04/2011
 * \author Sergey Kosarevsky, 2007-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clCanvas_
#define _clCanvas_

#include "Core/iObject.h"

#include "Math/LVector.h"
#include "Math/LMatrix.h"
#include "Math/LBox.h"
#include "Math/LAABB.h"
#include "Math/LOBB.h"
#include "Math/LRect.h"
#include "Math/LSphere.h"
#include "Math/LCurve.h"
#include "Math/LPoly.h"
#include "Math/LHull.h"
#include "Math/Collision.h"
#include "Images/TextRenderer.h"
#include "Renderer/iRenderContext.h"

class iTexture;
class iShaderProgram;
class clRenderState;
class iVertexArray;
class clBitmap;
class clScene;
class clFontFace;
class clTextRenderer;
class clRigidBody;
class LCollisionShape;

enum LCanvasOp
{
	Canvas_Rect,
	Canvas_Rect3D,
	Canvas_Line3D,
	Canvas_Circle3D,
	Canvas_Arrow3D,
	Canvas_BoundingBox,
	Canvas_OrientedBox,
	Canvas_Sphere,
	Canvas_Plane,
	Canvas_Curve3D,
	Canvas_Curve3DFrame,
	Canvas_TextStr,
	Canvas_TextStrFreeType
};

enum LTextAlignment
{
	TextAlign_Left         = 0,
	TextAlign_Right        = 1,
	TextAlign_CenterWidth  = 2,
	TextAlign_CenterHeight = 3,
	TextAlign_Center       = 4,
	TextAlign_FitWidth     = 5,
	TextAlign_FitHeight    = 6,
	TextAlign_Fit          = 7,
};

class clGeom;
class clCanvas;

/// generic canvas drawing operation
struct sCanvasOp
{
public:
	sCanvasOp(  clCanvas* Canvas,
	            LCanvasOp Op,
	            const LMatrix4& Projection,
	            const LMatrix4& ModelView
	         )
		: FCanvas( Canvas ),
		  FCanvasOp( Op ),
		  FProjection( Projection ),
		  FModelView( ModelView ) {};
private:
	clCanvas* FCanvas;
	LCanvasOp FCanvasOp;
	LMatrix4  FProjection;
	LMatrix4  FModelView;
};

class scriptfinal iFontProperties: public iObject
{
public:
	//
	// iObject interface
	//

	NET_EXPORTABLE();
	SERIALIZABLE_CLASS();

	virtual bool           IsFixedFont() const = 0;
	virtual clRenderState* GetFontShader() const { return NULL; }
};

/// font properties
class scriptfinal clFixedFontProperties: public iFontProperties
{
public:
	clFixedFontProperties()
		: FGlyphSizeX( 0.4f ),
		  FGlyphSizeY( 1.0f ),
		  FFontWidth(  3.0f / 128.0f ),
		  FFontHeight( 4.0f / 128.0f ),
		  FFontShaderName( "Shaders/text_system.shader" ) {};
	//
	// iObject interface
	//

	NET_EXPORTABLE();
	SERIALIZABLE_CLASS();

	virtual void AfterConstruction() { EndLoad(); }
	virtual bool EndLoad();

	//
	// iFontProperties interface
	//
	virtual bool           IsFixedFont() const { return true; }
	virtual clRenderState* GetFontShader() const
	{
		return FFontShader;
	}

	//
	// clFixedFontProperties interface
	//

	/* Property( Description="Glyph size X", Category="Glyphs", Type=float,  Name=GlyphSizeX, FieldName=FGlyphSizeX ) */
	/* Property( Description="Glyph size Y", Category="Glyphs", Type=float,  Name=GlyphSizeY, FieldName=FGlyphSizeY ) */
	/* Property( Description="Font width",   Category="Font",   Type=float,  Name=FontWidth,  FieldName=FFontWidth  ) */
	/* Property( Description="Font height",  Category="Font",   Type=float,  Name=FontHeight, FieldName=FFontHeight ) */
	/* Property( Description="Shader name",  Category="Font",   Type=string, Name=ShaderName, FieldName=FFontShaderName ) */
public:
	float    FGlyphSizeX;
	float    FGlyphSizeY;
	float    FFontWidth;
	float    FFontHeight;
	LString  FFontShaderName;
private:
	clRenderState* FFontShader;
};

/// FreeType font properties
class scriptfinal clFreeTypeFontProperties: public iFontProperties
{
public:
	clFreeTypeFontProperties()
		: FFontSize( 0.1f ),
		  FFontName( "" ),
		  FAlignment( TextAlign_Fit ),
		  FFontFace( NULL ) {};
	~clFreeTypeFontProperties();
	//
	// iObject interface
	//

	NET_EXPORTABLE();
	SERIALIZABLE_CLASS();

	virtual void AfterConstruction() { EndLoad(); }
	virtual bool EndLoad();

	//
	// iFontProperties interface
	//
	virtual bool    IsFixedFont() const { return false; }

	//
	// clFreeTypeFontProperties
	//

	virtual LString GetAlignment() const;
	virtual void    SetAlignment( const LString& Alignment );

	/* Property( Description="Font size", Category="Glyphs", Type=float,  Name="Size", FieldName=FFontSize ) */
	/* Property( Description="Font name", Category="Glyphs", Type=string, Name="Name", FieldName=FFontName ) */
	/* Property( Description="Alignment", Category="Glyphs", Type=string, Name="Alignment", Setter=SetAlignment, Getter=GetAlignment ) */
public:
	float          FFontSize;
	LString        FFontName;
	LTextAlignment FAlignment;
	clFontFace* FFontFace;
};

class scriptfinal clRenderableString: public iObject
{
public:
	clRenderableString(): FBitmap( NULL ), FWidth( 0.0f ), FHeight( 0.0f ), FTextTexCoords( 0.0f ), FTexture( NULL ) {};
	clRenderableString( clBitmap* Bitmap, float Width, float Height, const LVector4& TextTexCoords )
		: FBitmap( Bitmap ),
		  FWidth( Width ),
		  FHeight( Height ),
		  FTextTexCoords( TextTexCoords ),
		  FTexture( NULL ) {};
	virtual ~clRenderableString();
	scriptmethod clBitmap* GetBitmap() const { return FBitmap; };
	scriptmethod float GetWidth() const { return FWidth; }
	scriptmethod float GetHeight() const { return FHeight; }
	scriptmethod const LVector4& GetTextTexCoords() const { return FTextTexCoords; }
	scriptmethod iTexture* GetTexture() const;
private:
	clBitmap* FBitmap;
	float     FWidth;
	float     FHeight;
	LVector4  FTextTexCoords;
	// lazy initialization
	mutable iTexture* FTexture;
};

/**
   \brief Drawing of geometry primitives. Not designed to be fast.

   All screen coordinates are normalized (0..1) unless stated otherwise.

      0,0          1,0
         ----------
         |        |
         |        |
         |        |
         ----------
      0,1          1,1

**/
class netexportable scriptfinal clCanvas: public iObject
{
public:
	clCanvas();
	virtual ~clCanvas();

	virtual void    AfterConstruction();

	//
	// clCanvas
	//

	/// called by the renderer to reset Canvas before shutdown
	scriptmethod void Cleanup();

	/**
	   \brief Draw a geom with specified rendering parameters. Slow.

	   Override shader provides possibility for volume rendering
	*/
	scriptmethod void Geom( const LMatrix4& Projection,
	                        const LMatrix4& ModelView,
	                        clGeom* Geom,
	                        clRenderState* OverrideShader );

	/// Draw a rectangle with solid color (alpha-transparent)
	scriptmethod void Rect( float X1,
	                        float Y1,
	                        float X2,
	                        float Y2,
	                        const LVector4& Color );
	/// Draw a textured rectangle in normalized screen coordinates with overlay Color (multiplied, alpha-transparent) applying shader program (if any)
	scriptmethod void TexturedRect( float X1,
	                                float Y1,
	                                float X2,
	                                float Y2,
	                                iTexture* Texture,
	                                iShaderProgram* ShaderProgram,
	                                const LVector4& Color );

	/// Draw a textured rectangle repeating texture TilesX*TilesY times, texture should have clamping set to L_REPEAT. Vertex array and UV can be NULL.
	scriptmethod void TexturedRectTiled( const LVector4& Pos,
	                                     float TilesX,
	                                     float TilesY,
	                                     iTexture* Texture,
	                                     iShaderProgram* ShaderProgram,
	                                     const LVector4& Color,
	                                     iVertexArray* VA,
	                                     const LRect* UV );

	/// Draw a 3D polygon with specified vertices
	scriptmethod void TexturedRect3D( const LVector3& v1, const LVector3& v2, const LVector3& v3, const LVector3& v4,
	                                  iTexture* Texture,
	                                  iShaderProgram* ShaderProgram, bool UseDepthTest );

	/// Draw the rectangle using some custom shader (renderstate)
	scriptmethod void Rect3DShader( const LVector3& v1, const LVector3& v2, const LVector3& v3, const LVector3& v4, clRenderState* State );

	/**
	   \brief Blend the rectangle with on-screen contents using alpha-channel

	   This method is used in RenderMeshDirect() and Canvas::Flush() to blend
	   rendered primitives with on-screen contens

	   The functionality is already implemented in Test_VolumesAndScene (two volumes and a poly-scene is blended there)?
	   but there are bugs for the Radeon cards
	*/
	scriptmethod void AlphaTexture( float X1, float Y1, float X2, float Y2, iTexture* Texture );

	/// Draw a 0,0-1,1 rect with a given shader. Useful to show splash screens.
	scriptmethod void FullscreenRect( clRenderState* Shader );
	/// Draw a 0,0-1,1 rect with a given shader program. Textures are undefined (left from previous drawcall)
	scriptmethod void FullscreenRectSP( iShaderProgram* ShaderProgram );
	/// Draw a line in 3D space from Point1 to Point2 with the current projection and movelview matrices (use SetMatrices to set)
	scriptmethod void Line3D( const LVector3& Point1,
	                          const LVector3& Point2,
	                          const LVector4& Color );
	/// Draw a thick line with glow in 3D space from Point1 to Point2 with the current projection and movelview matrices (use SetMatrices to set)
	scriptmethod void Line3DGlow( const LVector3& Point1,
	                              const LVector3& Point2,
	                              const LVector4& Color,
	                              float Thickness );
	/// Draw a circle in Z-plane with the current projection and movelview matrices (use SetMatrices to set)
	scriptmethod void Circle3D( const LVector3& Center,
	                            float Radius,
	                            const LVector4& Color,
	                            bool AspectCorrection );
	/// Draw a colored 3D arrow with the current projection and movelview matrices (use SetMatrices to set)
	scriptmethod void Arrow3D( const LVector3& Point1,
	                           const LVector3& Point2,
	                           const float     TipSize,
	                           const LVector4& TipColor,
	                           const LVector4& ArrowColor );
	/// Draw a wireframe axis-aligned bounding-box with the current projection and movelview matrices (use SetMatrices to set)
	scriptmethod void BoundingBox( const LAABoundingBox& Box,
	                               const LVector4& Color );
	/// Draw a wireframe oriented bounding-box with the current projection and movelview matrices (use SetMatrices to set)
	scriptmethod void OrientedBox( const LBox& B, const LVector4& Color );
	/// Draw a wireframe hemisphere with the current projection and movelview matrices (use SetMatrices to set)
	scriptmethod void Sphere( const LSphere&  Sphere, const LVector4& Color );

	/// Draw a wireframe sphere with the current projection and movelview matrices (use SetMatrices to set)
	scriptmethod void FullSphere( const LSphere&  Sphere, const LVector4& Color );

	/// Draw a wireframe plane with base vectors v1 and v2 with the current projection and movelview matrices (use SetMatrices to set)
	void Plane( const LVector3& p0, const LVector3& v1, const LVector3& v2, int numx, int numy, float step1, float step2, const LVector4& color );

	/// Render curve outline
	void Curve( const LMatrix4& _proj, const LMatrix4& _view, const LCurve& Curve );

	/// Render the body reference frame. RenderCollision flag tells if we should draw the collision shape (if any)
	scriptmethod void RigidBody( const LMatrix4& Proj, const LMatrix4& View, clRigidBody* Body,  bool RenderVelocities, bool RenderForces, bool RenderCollision );

	/// Draw the coordinate frame centered at point. Vector lengths give the arrows' sizes
	scriptmethod void CoordFrame( const LMatrix4& _proj, const LMatrix4& _view, const LVector3& Center, const LVector3& V1, const LVector3& V2, const LVector3& V3 );

	/// Draw some collision shape
	scriptmethod void CollisionShape( const LMatrix4& _proj, const LMatrix4& _view, LCollisionShape* Shape, clRigidBody* Body );

	/// If Depth is not -1 then only nodes at the Depth level are rendered
	void OBBTree( const LMatrix4& _proj, const LMatrix4& _view, LOBBTree* Tree, int Depth, bool ShowPlanes, const LVector4& Color, const LVector4& PlaneColor );

	/// Draw a single oriented polygon. If the normal size is more than zero, the normal is rendered
	void Poly( const LMatrix4& _proj, const LMatrix4& _view, const Math::LPoly& Poly, float NormalSize, const LVector4& PolyColor, const LVector4& NormalColor );

	/// Draw the list of wireframe polygons
	void PolyList( const LMatrix4& _proj, const LMatrix4& _view, const Math::LPolyList& Polys, float NormalSize, const LVector4& PolyColor, const LVector4& NormalColor );

	/// Draw the plane hull near the hint point Extents.FCenter not larger than the Extents. Use plane "cell" size of CellSize units
	void PlaneHullOutlines( const LMatrix4& _proj, const LMatrix4& _view, const Math::LPlaneHull& Hull, const LBox& Extents, float CellSize, const LVector4& Color );

	/// "Draw indexed primitive"
	void Wireframe( const LMatrix4& _proj, const LMatrix4& _view, int NumTris, LVector3* Pts, int* Idx, const LVector4& Color );

	/// Draw a little cross near specified point
	scriptmethod void Point( const LMatrix4& _proj, const LMatrix4& _view, const LVector3& Pt, float PtSize, const LVector4& Color );

	/// Draw the information about multi-point contact stored in Info structure
	void noexport ContactInfo( const LMatrix4& _proj, const LMatrix4& _view, Math::sCollisionInfo* Info, float PointSize, float NormalSize, const LVector4& PointColor1, const LVector4& PointColor2, const LVector4& NormalColor1, const LVector4& NormalColor2 );

	/**
	   \brief Draw a 2D text string using bitmap font

	   Default system font (\see FDefaultFont) is used if Font is NULL.
	*/
	scriptmethod void TextStr( float X, float Y, const LString& Str, const LVector4& Color, clFixedFontProperties* Font );

#pragma region Draw a text string using FreeType
	/**
	   \brief Draw a 2D text string using FreeType

	   Default font is used if FontFace is NULL.
	*/
	scriptmethod void TextStrFreeType( const LRect& Rect, const LString& Str, const float Size, const LVector4& Color, clFontFace* FontFace, LTextAlignment Alignment );
//	scriptmethod void TextStrFreeType_Fast( const LRect& Rect, const LString& Str, const float Size, const LVector4& Color, clFontFace* FontFace, LTextAlignment Alignment );
	scriptmethod void TextStrFreeType_Flush();
	/// precache FreeType-rendered text string into a bitmap/texture
	scriptmethod clRenderableString* TextStrGetBitmapFreeType( const LString& Str, const float Size, clFontFace* FontFace );
	/// draw precached text string from bitmap (using common texture)
	scriptmethod void TextStrBitmapFreeType( const LRect& Rect, clRenderableString* Str, const LVector4& Color, LTextAlignment Alignment );
	/// draw precached text string from texture (using precached texture in clRenderableString)
	scriptmethod void TextStrTextureFreeType( const LRect& Rect, clRenderableString* Str, const LVector4& Color, LTextAlignment Alignment );
	/// calculate dimensions os text string
	LRect GetTextStrRectFreeType( const LString& Str, const float Size, clFontFace* FontFace );
#pragma endregion

	/// Create a clFontFace from a given .ttf file
	scriptmethod clFontFace* CreateFontFace( const LString& FileName );

	/// Begin packetized output of text. No text is rendered until next call to EndTextPacket()
	scriptmethod void BeginTextPacket();

	/// Render and flush text output buffers
	scriptmethod void EndTextPacket();

	scriptmethod void TurnOnLineClipping();
	scriptmethod void TurnOffLineClipping();

	/// Render a small coordinate frame (three axes) to show the camera's orientation
	scriptmethod void DrawOrientation( const LMatrix4& Projection, const LMatrix4& ModelView );

	/// Force everything to be drawn
	scriptmethod void Flush();

	scriptmethod void SetMatrices( const LMatrix4& Projection, const LMatrix4& ModelView );

	/// Get default matrices for rendering with orthographic projection. Useful for GUI, HUD etc.
	const sMatrices* GetOrthoMatrices() const { return &FMatrices; };
	scriptmethod clFixedFontProperties* GetDefaultFont() const;

	/** Property(Name=DefaultFontName, Type=string, FieldName=FDefaultFontName) */

	LString FDefaultFontName;

private:
	scriptmethod void InitCanvas();
	scriptmethod void FlushTextPacket();
private:
	iVertexArray*    FScreenQuad;
	sMatrices        FMatrices;

#pragma region Line drawing arrays, shader and uniforms handles
	iVertexArray*    FLinesArray[2]; // ping-pong buffer
	int              FCurrentLinesArray;
	clRenderState*   FLinesShader;
	Lint             FProjectionMatrixUniform;
	Lint             FModelViewMatrixUniform;
	Lint             FProjectionMatrixUniformTex;
	Lint             FModelViewMatrixUniformTex;
#pragma endregion

#pragma region Line/Rect rendering shaders
	int              FLineCount;
	iVertexArray*    FRect;
	clRenderState*   FRectShader;

	/// Simple textured rect shader
	clRenderState*   FTexRectShader;
	clRenderState*   FTexRectShader3D;
	clRenderState*   FGlowLinesShader;

	/// Uniform handle for the Colered rectangle
	Lint             FRectColorUniform;
	Lint             FRectPosUniform;
	Lint             FTexRectPosUniform;
	Lint             FTexRectColorUniform;
	Lint             FTexRectTilesUniform;
	Lint             FTexRectUVUniform;

	/// Alpha-blend shader for AlphaTexture
	clRenderState*   FAlphaBlendShader;
#pragma endregion

#pragma region Text rendering
	/// A collection of matrices used for text rendering
	sMatrices         FTextMatrices;

	/// A bitmap font rendering shader
	clRenderState*    FTextShader;
	clRenderState*    FTextAtlasShader;

	/// Temporary rectangular vertex array for text rendering
	iVertexArray*     FTextVA;
	bool              FTextPacketStarted;
	size_t            FTextPacketNumGlyphs;
	clCVar*           FGlyphSizeX;
	clCVar*           FGlyphSizeY;
	clCVar*           FFontWidth;
	clCVar*           FFontHeight;
	std::vector<sVAContainer> FTextFreeTypeGeometry;

	/// Default raster system font
	mutable clFixedFontProperties* FDefaultFont;
#pragma endregion

#pragma region FreeType text rendering
	/// Default FreeType font
	clFontFace* FDefaultFreeTypeFont;

	/// FreeType text rendering interface
	clTextRenderer* FTextRenderer;
	iTexture*      FFontTexture;
	clRenderState* FFontShader;
#pragma endregion

#pragma region Polygonal/Volumetric overlay rendering

	/// Switch for Flush() method. If true the contents of FOverlayScene is bleneded with the screen
	bool              FUseOverlayRendering;

	/**
	   \brief An instance of sequencer to produce overlay polygonal rendering

	   It has its own offscreen buffer which is bleneded with screen in Canvas::Flush()
	*/
	clScene*          FOverlayScene;
#pragma endregion
};

#endif

/*
 * 05/08/2011
     Merged with DebugDraw.h
 * 25/04/2011
     TextStrFreeType()
 * 16/12/2010
     Geom()
     Mesh()
 * 09/03/2010
     Text rendering grouped into packets
 * 18/10/2009
     Canvas interface changed
 * 20/06/2009
     DrawCircle3D()
 * 29/04/2007
     DrawFullscreenRect()
 * 30/03/2007
     DrawSphere()
 * 22/03/2007
     DrawArrow3D()
 * 08/02/2007
     DrawBillboard()
 * 14/01/2007
     DrawBoundingBox()
 * 06/01/2007
     DrawTexturedRect()
 * 03/01/2007
     It's here
*/
