/**
 * \file iShaderProgram.h
 * \brief Shader program interface
 * \version 0.5.91
 * \date 03/04/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iShaderProgram_
#define _iShaderProgram_

#include "Resources/iResource.h"
#include "Utils/LArray.h"
#include "Scene/Material.h"
#include "Math/LMatrix.h"
#include "Core/CVars.h"

class clCVar;
class iIStream;

class clScene;

// Stages order: vertex -> tess control -> tess eval -> geometry -> fragment

enum LShaderStage
{
	L_VERTEX_STAGE      = 0,
	L_TESSCONTROL_STAGE = 1,
	L_TESSEVAL_STAGE    = 2,
	L_GEOMETRY_STAGE    = 3,
	L_FRAGMENT_STAGE    = 4
};

struct sMatrices;

/// Vertex/Fragment/Geometry shaders container
class scriptfinal netexportable iShaderProgram: public iResource
{
public:
	iShaderProgram();

	//
	// iCacheable interface
	//
	virtual LString   GetCachingDir() const;
	//
	// iShaderProgram
	//
	/// Force shader program relink
	virtual bool      RelinkShaderProgram();
	/// binded to the context as a current shader program, used in AddBuffer()
	virtual void      Bind() = 0;
	/// unbinded from the context
	virtual void      UnBind() const = 0;
	/// must be called before any block of calls SetUniform*()
	virtual void      BindUniforms() = 0;
#pragma region Shader programs text management
	virtual void      SetShaderCode( LShaderStage Stage, const LString& ShaderCode );
	virtual LString   GetShaderCode( LShaderStage Stage ) const;
	virtual bool      IsRelinkPending() const
	{
		return FRelinkPending;
	};
	virtual void      SetRelinkPending( bool Relink )
	{
		FRelinkPending = Relink;
	};
#pragma endregion

#pragma region Uniforms management
	/// return ID of the uniform. Create uniform if it doesnot exist
	virtual Lint      CreateUniform( const LString& Name ) = 0;
	/// assign int value to a variable
	virtual void      SetUniformInt( Lint Uniform, const int Int ) = 0;
	virtual void      SetUniformIntArray( Lint Uniform, int Count, const int& Int ) = 0;
	/// assign float value to a variable
	virtual void      SetUniformFloat( Lint Uniform, const float Float ) = 0;
	virtual void      SetUniformFloatArray( Lint Uniform, int Count, const float& Float ) = 0;
	/// assign Count values to a vec3 array or variable. For vec3 variables Count should be set to 1
	virtual void      SetUniformVec3Array( Lint Uniform, int Count, const LVector3& Vector ) = 0;
	/// assign Count values to a vec4 array or variable. For vec4 variables Count should be set to 1
	virtual void      SetUniformVec4Array( Lint Uniform, int Count, const LVector4& Vector ) = 0;
	/// assign Count values to a mat3 array or variable. For mat3 variables Count should be set to 1
	virtual void      SetUniformMat3Array( Lint Uniform, int Count, const LMatrix3& Matrix ) = 0;
	/// assign Count values to a mat4 array or variable. For mat4 variables Count should be set to 1
	virtual void      SetUniformMat4Array( Lint Uniform, int Count, const LMatrix4& Matrix ) = 0;
	/// assign int value to a variable
	virtual void      SetUniformNameInt( const LString& Name, const int Int ) = 0;
	virtual void      SetUniformNameIntArray( const LString& Name, int Count, const int& Int ) = 0;
	/// assign float values to a variable
	virtual void      SetUniformNameFloat( const LString& Name, const float Float ) = 0;
	virtual void      SetUniformNameFloatArray( const LString& Name, int Count, const float& Float ) = 0;
	/// assign Count values to a vec3 array or variable. For vec3 variables Count should be set to 1
	virtual void      SetUniformNameVec3Array( const LString& Name, int Count, const LVector3& Vector ) = 0;
	/// assign Count values to a vec4 array or variable. For vec4 variables Count should be set to 1
	virtual void      SetUniformNameVec4Array( const LString& Name, int Count, const LVector4& Vector ) = 0;
	/// assign Count values to a mat3 array or variable. For mat3 variables Count should be set to 1
	virtual void      SetUniformNameMat3Array( const LString& Name, int Count, const LMatrix3& Matrix ) = 0;
	/// assign Count values to a mat4 array or variable. For mat4 variables Count should be set to 1
	virtual void      SetUniformNameMat4Array( const LString& Name, int Count, const LMatrix4& Matrix ) = 0;
	/// update transformations
	virtual void      SetTransformationUniforms( const sMatrices& Matrices ) = 0;
	/// update material parameters
	virtual void      SetMaterialUniforms( const sMaterialDesc& Desc, bool ReceiveShadow ) = 0;
	/// update GUI parameters
	virtual void      SetGUIUniforms( const LVector4& GUIPos, float Opacity, float ViewLifeTime ) = 0;
	/// update per-rigid scene parameters
	virtual void      SetSceneUniformsPerRigid( const sMaterialDesc& Desc, const LVector4& PickingColor, const LVector4& KeyFrame, bool Skinned, bool ReceiveShadow ) = 0;

	/// Load light parameters to shader
	virtual void      SetLightUniformsPerShader( clScene* Scene ) = 0;
	/// update per-shader scene parameters
	virtual void      SetSceneUniformsPerShader( bool UseShadowMatrix, const LMatrix4& ShadowMatrix, float EnableClipPlane, bool UseClipPlane, const LVector4& ClipPlane, bool ShowNormals ) = 0;

	/// assign symbolic name to an output fragment data location
	virtual void      SetFragDataLocationName( int Location, const LString& Name ) = 0;
	/// assign symbolic name to an input attribute location
	virtual void      SetAttribLocationName( int Location, const LString& Name ) = 0;

	/// return the number of active vertex attrib streams used in this shader program
	virtual int       GetActiveAttribStreams() const { return 0; };

	/// tell if vertex attribs (vertices, texcoords, normals, etc) are used in this shader program
	virtual const LArray<bool>& EnumerateVertexStreams() const { return FStreams; }

	/// Bind float shader variable to a float clCVar
	scriptmethod void BindConsoleVariable( const LString& Name, clCVar* CVar ) { BindConsoleVariableType( Name, CVar, L_VARIANT_FLOAT ); }

	/// Bind vec3 shader variable to a float clCVar
	scriptmethod void BindConsoleVariableVec3( const LString& Name, clCVar* CVar ) { BindConsoleVariableType( Name, CVar, L_VARIANT_VEC3 ); }

	/// Bind vec4 shader variable to a float clCVar
	scriptmethod void BindConsoleVariableVec4( const LString& Name, clCVar* CVar ) { BindConsoleVariableType( Name, CVar, L_VARIANT_VEC4 ); }

	/// Bind mat4 shader variable to a float clCVar
	scriptmethod void BindConsoleVariableMat4( const LString& Name, clCVar* CVar ) { BindConsoleVariableType( Name, CVar, L_VARIANT_MAT4 ); }

	/// Internal variable bind routine
	virtual void      BindConsoleVariableType( const LString& Name, clCVar* CVar, LVariantType VarType ) = 0;

#pragma endregion

#pragma region Geometry shaders
	virtual void      SetInputPrimitiveType( const LString& Type ) = 0;
	virtual void      SetOutputPrimitiveType( const LString& Type ) = 0;
	virtual void      SetOutputVerticesCount( Luint OutputVerticesCount ) = 0;
#pragma endregion

	/// returns true if tesselation control & evaluation shaders are attached
	virtual bool IsTesselationEnabled() const { return false; }

#pragma region Defines list for conditional compilation of shader code
	virtual void      SetDefinesList( const LString& DefinesList ) = 0;
	virtual LString   GetDefinesList() const = 0;
#pragma endregion

#pragma region Multiple render targets management
	/// set the name of the i-th render target, like "out_FragColor", targets array will grow automatically if no i-th target exists
	virtual void       SetRenderTargetName( size_t i, const LString& Name );

	/// get the name of the i-th render target, empty string return if no such target exists
	virtual LString    GetRenderTargetName( size_t i ) const;

	/// get the number of currenly active render targets
	virtual size_t     GetNumRenderTargets() const;

	/// set the number of currenly active render targets, existing targets will be truncated or extended with empty names
	virtual void       SetNumRenderTargets( size_t NumTargets );
#pragma endregion

	FWD_EVENT_HANDLER( Event_POSTINIT )
	{
		RelinkShaderProgram();
	}
protected:
	virtual bool    LoadFromFile( const LString& FileName );
private:
	void    ReadSegment( iIStream* IStream, const LString& IgnoreString, LString* Stage );
private:

	/// Source codes for each shader stage
	std::vector<LString>    FShaderText;

	/// Internal "dirty" flag indicating the need to relink shader program (set when something changes in the source code or uniforrm binding)
	bool                    FRelinkPending;

	LStr::clStringsVector   FRenderTargets;
protected:

	// Uniforms IDs
	Lint FIDin_ProjectionMatrix;
	Lint FIDin_ViewMatrix;
	Lint FIDin_ViewMatrixInverse;
	Lint FIDin_ModelMatrix;
	Lint FIDin_ModelMatrixInverse;
	Lint FIDin_ModelViewMatrix;
	Lint FIDin_ModelViewMatrixInverse;
	Lint FIDin_ModelViewProjectionMatrix;
	Lint FIDin_NormalMatrix;

	Lint FID_MTL_AMBIENT_COLOR;
	Lint FID_MTL_DIFFUSE_COLOR;
	Lint FID_MTL_SPECULAR_COLOR;
	Lint FID_MTL_SHININESS;
	Lint FID_MTL_TRANSPARENCY;
	Lint FID_MTL_REFLECTIVITY;
	Lint FID_MTL_ROUGHNESS;

	Lint FID_MTL_RECEIVESHADOW;
	Lint FID_MTL_PLANARREFLECTION;
	Lint FID_MTL_USEAMBIENTONLY;

	Lint FID_MTL_TESSINNERLEVEL;
	Lint FID_MTL_TESSOUTERLEVEL;

	Lint FID_GUI_Pos;
	Lint FID_GUI_Opacity;
	Lint FID_GUI_ViewLifeTime;

#pragma region Per-shader scene params

	Lint FID_EnableClipPlane;
	Lint FID_ClipPlane;
	Lint FID_ShowNormals;

	/// Projection matrix (?) for this light
	Lint FID_ShadowMatrix;

#pragma region Lighting properties (each ID corresponds to a single array of parameters)

	/// 'Enable' flag
	Lint FID_LightEnabledFlag;

	/// 0 - Spot, 1 - Point, 2 - Directional
	Lint FID_LightType;

	/// Position, Direction, etc. ()
	Lint FID_LightTransform;

	/// k0, k1, k2 - const, linear, quadratic attenuation
	Lint FID_LightAttenuation;

	/// Spot cutoff and Spot exponent
	Lint FID_LightSpotParams;

	/// Spot direction
	Lint FID_LightSpotDirection;

	Lint FID_LightAmbientColor;
	Lint FID_LightDiffuseColor;
	Lint FID_LightSpecularColor;

	/// Total number of active lights
	Lint FID_NumLights;

#pragma endregion

	// per-rigid scene params
	Lint FID_PICKING_COLOR;
	Lint FID_KeyFrame;
	Lint FID_Skinned;

	/// precached vertex streams
	LArray<bool>      FStreams;
};

#endif

/*
 * 21/02/2011
     SetFragDataLocationName()
     SetAttribLocationName()
 * 03/04/2010
     Arrays of vectors
 * 12/03/2010
     GetTessEvalShaderCode()
     GetTessControlShaderCode()
 * 24/04/2009
     CreateUniform()
 * 22/04/2009
     SetUniformVec3()
 * 21/04/2009
     SetUniformFloat()
     SetUniformVec4()
     SetUniformMat4()
 * 17/04/2009
     UnBind()
 * 22/02/2009
     SetDefinesList()
     GetDefinesList()
 * 13/12/2008
     BindCVarToUniform()
 * 06/10/2007
     SetInputPrimitiveType()
     SetOutputPrimitiveType()
     SetOutputVerticesCount()
 * 25/03/2005
     AttachVertexShader()
     AttachFragmentShader()
 * 21/03/2005
     It's here
*/
