/**
 * \file GLSL.h
 * \brief OpenGL Shading Language
 * \version 0.6.00
 * \date 09/05/2011
 * \author Sergey Kosarevsky, 2005-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clGLSLShaderProgram_
#define _clGLSLShaderProgram_

#include "Renderer/iShaderProgram.h"

#include "GLAPIContainer.h"

struct sUniform
{
public:
	explicit sUniform( const LString& Name ): FName( Name ),
		FLocation( -1 ),
		FConsoleVariable( NULL ),
		FType( L_VARIANT_FLOAT ) {};
	sUniform( int Location, const LString& Name ): FName( Name ),
		FLocation( Location ),
		FConsoleVariable( NULL ),
		FType( L_VARIANT_FLOAT ) {};
	LString      FName;
	Lint         FLocation;
	LVariantType FType;
	clCVar*      FConsoleVariable;
};

class scriptfinal clGLSLShaderProgram: public iShaderProgram, public clGLAPIContainer
{
public:
	clGLSLShaderProgram();
	virtual ~clGLSLShaderProgram();
	//
	// iObject interface
	//
	virtual void    AfterConstruction();
	//
	// iResource interface
	//
	virtual bool    IsSameResource( iResource* Other );
	virtual LString GetCachingDir() const;
	virtual LString GetCachedFileName();
	virtual void    CacheTo( const LString& FileName );
	virtual bool    CacheFrom( const LString& FileName );
	//
	// iShaderProgram interface
	//
	virtual bool       RelinkShaderProgram();
	virtual void       Bind();
	virtual void       UnBind() const;
	virtual void       BindUniforms();
	virtual Lint       CreateUniform( const LString& Name );
	virtual void       SetUniformInt( Lint Uniform, const int Int );
	virtual void       SetUniformIntArray( Lint Uniform, int Count, const int& Int );
	virtual void       SetUniformFloat( Lint Uniform, const float Float );
	virtual void       SetUniformFloatArray( Lint Uniform, int Count, const float& Float );
	virtual void       SetUniformVec3Array( Lint Uniform, int Count, const LVector3& Vector );
	virtual void       SetUniformVec4Array( Lint Uniform, int Count, const LVector4& Vector );
	virtual void       SetUniformMat3Array( Lint Uniform, int Count, const LMatrix3& Matrix );
	virtual void       SetUniformMat4Array( Lint Uniform, int Count, const LMatrix4& Matrix );
	virtual void       SetUniformNameInt( const LString& Name, const int Int );
	virtual void       SetUniformNameIntArray( const LString& Name, int Count, const int& Int );
	virtual void       SetUniformNameFloat( const LString& Name, const float Float );
	virtual void       SetUniformNameFloatArray( const LString& Name, int Count, const float& Float );
	virtual void       SetUniformNameVec3Array( const LString& Name, int Count, const LVector3& Vector );
	virtual void       SetUniformNameVec4Array( const LString& Name, int Count, const LVector4& Vector );
	virtual void       SetUniformNameMat3Array( const LString& Name, int Count, const LMatrix3& Matrix );
	virtual void       SetUniformNameMat4Array( const LString& Name, int Count, const LMatrix4& Matrix );
	virtual void       SetTransformationUniforms( const sMatrices& Matrices );
	virtual void       SetMaterialUniforms( const sMaterialDesc& Desc, bool ReceiveShadow );
	virtual void       SetGUIUniforms( const LVector4& GUIPos, float Opacity, float ViewLifeTime );
	virtual void       SetSceneUniformsPerRigid( const sMaterialDesc& Desc, const LVector4& PickingColor, const LVector4& KeyFrame, bool Skinned, bool ReceiveShadow );

	virtual void       SetLightUniformsPerShader( clScene* Scene );
	virtual void       SetSceneUniformsPerShader( bool UseShadowMatrix, const LMatrix4& ShadowMatrix, float EnableClipPlane, bool UseClipPlane, const LVector4& ClipPlane, bool ShowNormals );

	virtual void       BindConsoleVariableType( const LString& Name, clCVar* CVar, LVariantType VarType );

	virtual void       SetFragDataLocationName( int Location, const LString& Name );
	virtual void       SetAttribLocationName( int Location, const LString& Name );

	virtual int        GetActiveAttribStreams() const { return FActiveAttribStreams; };

	virtual void       SetInputPrimitiveType( const LString& Type );
	virtual void       SetOutputPrimitiveType( const LString& Type );
	virtual void       SetOutputVerticesCount( Luint OutputVerticesCount );
	virtual bool       IsTesselationEnabled() const { return FTesselation; };
	virtual void       SetDefinesList( const LString& DefinesList ) { FDefinesList = DefinesList; };
	virtual LString    GetDefinesList() const { return FDefinesList; };

	FWD_EVENT_HANDLER( Event_SURFACE_DETACHED );
	FWD_EVENT_HANDLER( Event_SURFACE_ATTACHED );
private:
	void     BindDefaultLocations( Luint ProgramID );
	Lint     FindUniformLocation( const LString& Name );
	Luint    AttachShaderID( Luint Target, const LString& ShaderCode, Luint OldShaderID );
	bool     CheckStatus( Luint ObjectID, Lenum Target, const LString& Message ) const;
	bool     IsLinked() const;
	void     RebindAllUniforms();
private:
	std::vector<sUniform>    FUniforms;
	std::vector<sUniform>    FCVarsBindedUniforms;

	std::vector<sUniform>    FFragDataLocations;
	std::vector<sUniform>    FAttribsLocations;

	/// OpenGL shader program ID
	Luint                 FProgramID;
	/// OpenGL shader IDs
	std::vector<Luint>    FShaderID;

	bool              FTesselation;

	bool              FAlreadyLoaded;
	LString           FInputType;
	LString           FOutputType;
	Luint             FOutputVertices;
	LString           FDefinesList;

//   Luint             FMatricesBufferID;
	Lint              FMatricesBlockIndex;
	Lint              FMatricesBlockSize;

	/// how many vertex attribs are actually used in this shader program
	Lint              FActiveAttribStreams;

	/// stats
	clCVar*           FProgramRebinds;
};

#endif

/*
 * 09/05/2011
     SetRenderTargetName()
     GetRenderTargetName()
     GetNumRenderTargets()
 * 10/08/2010
     GetCachingDir()
     CacheTo()
     CacheFrom()
 * 12/03/2010
     Unified shaders
 * 11/03/2010
     SetUniformMat4Array()
     SetUniformNameMat4Array()
 * 21/04/2009
     SetUniformFloat()
     SetUniformVec4()
     SetUniformMat4()
 * 17/04/2009
     UnBind()
 * 06/09/2007
     AttachGeometryShader()
 * 02/06/2007
     SetInterpolationCoef()
 * 24/03/2005
     It's here
*/
