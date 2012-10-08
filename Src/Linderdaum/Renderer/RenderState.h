/**
 * \file RenderState.h
 * \brief Generic render state
 * \version 0.5.93
 * \date 04/11/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clRenderState_
#define _clRenderState_

#include "Math/LMatrix.h"
#include "Resources/iResource.h"

#include <string.h>

class iTexture;
class iRenderContext;
class iShaderProgram;
class iRenderTarget;

enum LBlendMode
{
	L_ZERO                     = 0,
	L_ONE                      = 1,
	L_SRC_COLOR                = 2,
	L_ONE_MINUS_SRC_COLOR      = 3,
	L_SRC_ALPHA                = 4,
	L_ONE_MINUS_SRC_ALPHA      = 5,
	L_DST_ALPHA                = 6,
	L_ONE_MINUS_DST_ALPHA      = 7,
	L_DST_COLOR                = 8,
	L_ONE_MINUS_DST_COLOR      = 9,
	L_SRC_ALPHA_SATURATE       = 10,
	L_CONSTANT_COLOR           = 11,
	L_ONE_MINUS_CONSTANT_COLOR = 12,
	L_CONSTANT_ALPHA           = 13,
	L_ONE_MINUS_CONSTANT_ALPHA = 14,
	L_INVALID_ENUM             = 15,
};

enum LStateMasks
{
	L_STATE_BLENDED         = 0x0001,
	L_STATE_DEPTH_TEST      = 0x0002,
	L_STATE_DEPTH_WRITES    = 0x0004,
	L_STATE_COLOR_WRITES    = 0x0008,
	L_STATE_BACKFACECULLING = 0x0010,
	L_STATE_BACKFACING      = 0x0020,
};

/// Low-level API-specific material description (a primitive material)
class netexportable scriptfinal clRenderState: public iResource
{
public:
	clRenderState();
	virtual ~clRenderState();

	NET_EXPORTABLE();
	SERIALIZABLE_CLASS();

	//
	// iObject interface
	//
	virtual void       AfterConstruction();
	/// Compile shader program, create textures and combine defines
	virtual bool       EndLoad();
	//
	// iResource interface
	//
	virtual LString    GetCachingDir() const;
	virtual bool       IsSameResource( iResource* Other );
	//
	// clRenderState
	//
	/// Get shader program
	virtual iShaderProgram*    GetShaderProgram() const { return FShaderProgram; };
	/// Set shader program (no deallocation)
	virtual void               SetShaderProgram( iShaderProgram* ShaderProgram ) { FShaderProgram = ShaderProgram; };
	/// Get texture binded to Tex-th texture unit
	virtual iTexture*          GetTextureForTextureUnit( int Tex ) const;
	/// Get number of binded textures
	virtual size_t             GetTexturesCount() const { return FTextureID.size(); };
	/**
	    Bind texture to a texture unit and pass owenership.
	    If Ownership is 'true' the texture will be deallocated on exit or upon replacement
	**/
	virtual void               SetTexture( int TexUnit, iTexture* Tex, bool Ownership );
	/// Set blending mode for 0-th render buffer
	virtual void               SetBlending( LBlendMode BlendSrc, LBlendMode BlendDst );
	/// Shortcut to bind 2 textures (color and depth) of a RenderTarget to this render share
	virtual void               UpdateTextures( iRenderTarget* RenderTarget );
	/// Copy values from this render state to the other. Ownership of textures is not passed.
	virtual void               CopyTo( clRenderState* Other );

#pragma region Property setters
	/// Helper functions for the serialization (migrating to new scheme, a lot of hand-written code remains)

	scriptmethod LString GetBlendFunc() const;
	scriptmethod void    SetBlendFunc( const LString& ParamValue );

	scriptmethod void    SetConsoleVar( const LString& ParamValue );
	scriptmethod LString GetConsoleVar() const { return ""; /* dummy, since we need a list - or just keep another vector<string> */}

	scriptmethod LString GetShaderProgramName() const;
	scriptmethod void    SetShaderProgramName( const LString& ParamValue );

	/// Set texture name/type/params
	scriptmethod void    SetImage( int i, const LString& Img ) { FImages[i] = Img; }

	/// Get texture name/type/params
	scriptmethod LString GetImage( int i ) const { return FImages[i]; }

	/// Get number of texture descriptions
	scriptmethod int     GetNumImages() const { return static_cast<int>( FImages.size() ); }

	/// Set define
	scriptmethod void    SetDefine( int i, const LString& def ) { FDefines[i] = def; }

	/// Get define
	scriptmethod LString GetDefine( int i ) const { return FDefines[i]; }

	/// Get number of defines
	scriptmethod int     GetNumDefines() const { return static_cast<int>( FDefines.size() ); }

	/// Set uniform
	scriptmethod void    SetUniform( int i, const LString& Uni ) { FUniforms[i] = Uni; }

	/// Get uniform
	scriptmethod LString GetUniform( int i ) const { return FUniforms[i]; }

	/// Get number of uniforms
	scriptmethod int     GetNumUniforms() const { return static_cast<int>( FUniforms.size() ); }

	/// Calculate the mask of this RenderState where every bit corresponds to a single state (blending, write modes, ets) enabled or disabled
	scriptmethod void    UpdateRenderStateMask();
	scriptmethod size_t  GetRenderStateMask() const { return FRenderStateMask; };
#pragma endregion

public:
	bool          FBlended;
	LBlendMode    FBlendSrc;
	LBlendMode    FBlendDst;
	bool          FDepthTest;
	bool          FDepthWrites;
	bool          FColorWrites;
	bool          FBackfaceCulling;
	bool          FBackFacing;
private:
	size_t        FRenderStateMask;
	std::vector<iTexture*>    FTextureID;
	std::vector<bool>         FTextureDeallocateOnExit;
	iShaderProgram*           FShaderProgram;
	int                       FMaxSupportedTextures;
	/// Shader program file, used for double-stage loading
	LString  FShaderProgramName;

#pragma region Properties

public:
	/// List of uniforms binded to CVars
	std::vector<LString> FUniforms;

	/// List of defines for shader source code
	std::vector<LString> FDefines;

	/// List of textures ("images" here)
	std::vector<LString> FImages;

	/* Property(Description="Blending function",   Category="State", Type=string, Name=BlendFunc, Getter=GetBlendFunc, Setter=SetBlendFunc) */

	/* Property(Description="Depth testing",       Category="State", Type=bool, Name=DepthTest,       FieldName=FDepthTest) */
	/* Property(Description="Depth writing",       Category="State", Type=bool, Name=DepthWrites,     FieldName=FDepthWrites) */
	/* Property(Description="Color writing",       Category="State", Type=bool, Name=ColorWrites,     FieldName=FColorWrites) */
	/* Property(Description="Backface culling",    Category="State", Type=bool, Name=BackfaceCulling, FieldName=FBackfaceCulling) */
	/* Property(Description="CCW or CW backfaces", Category="State", Type=bool, Name=Backfacing,      FieldName=FBackFacing) */
	/* Property(Description="Texture count",  Category="State", Name=TexturesCount, Type=int, Getter=GetTexturesCount) */

	/** Property(Description="Number of textures", Category="Shader", Name=NumImages, Type=int, Getter=GetNumImages) */
	/** Property(Description="List of textures", Category="Shader", Name=Images, Type=string, IndexType=int, FieldName=FImages, NetIndexedGetter=GetImage, NetIndexedSetter=SetImage) */

	/** Property(Description="Number of uniforms", Category="Shader", Name=NumUniforms, Type=int, Getter=GetNumUniforms) */
	/** Property(Description="Shader uniforms binded to globals", Category="Shader", Name=Uniforms, Type=string, IndexType=int, FieldName=FUniforms, NetIndexedGetter=GetUniform, NetIndexedSetter=SetUniform) */

	/** Property(Description="Number of defines", Category="Shader", Name=NumDefines, Type=int, Getter=GetNumDefines) */
	/** Property(Description="List of defines", Category="Shader", Name=Defines, Type=string, IndexType=int, FieldName=FDefines, NetIndexedGetter=GetDefine, NetIndexedSetter=SetDefine) */

	/** Property(Description="Shader program file", Category="Shader", Name=ShaderProgram, Type=string, Getter=GetShaderProgramName, Setter=SetShaderProgramName ) */

#pragma endregion

};

#endif

/*
 * 23/03/2011
     UpdateRenderStateMask()
     GetRenderStateMask()
 * 04/11/2010
     Legacy code removed
 * 28/10/2010
     LoadFromFile() pushed up
 * 23/08/2010
     API-dependant parts moved to render context
 * 03/02/2010
     Renamed
 * 31/03/2009
     Fields pushed down
 * 10/11/2008
     Size hints for textures
 * 21/09/2007
     Clipping
 * 27/02/2007
     Rendering order
 * 10/02/2007
     Backface culling mode
 * 07/08/2006
     Fixed function vertex animation support removed
 * 06/08/2006
     GetShaderProgram()
     SetShaderProgram()
 * 25/06/2006
     FCubeMapCrossFormat field
 * 27/05/2006
     Interface changed
 * 19/02/2005
     Packed structs & unions for portable serialization
 * 07/01/2005
     Fields declaration
 * 19/12/2004
     It's here
*/
