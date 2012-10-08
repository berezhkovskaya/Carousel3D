/**
 * \file SoftShaderProgram.h
 * \brief Software shading, empty implementation
 * \version 0.5.95
 * \date 11/06/2010
 * \author Viktor Latypov, 2010
 * \author viktor@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clSoftShaderProgram_
#define _clSoftShaderProgram_

#include "Renderer/iShaderProgram.h"

class scriptfinal clSoftShaderProgram: public iShaderProgram
{
public:
	clSoftShaderProgram() {}
	virtual ~clSoftShaderProgram() {}
	//
	// iResource interface
	//
	virtual bool    IsSameResource( iResource* Other )
	{
		return false;
	}
	//
	// iShaderProgram interface
	//
	virtual bool       RelinkShaderProgram()
	{
		return true;
	}
	virtual void       Bind() {}
	virtual void       UnBind() const {}
	virtual void       BindUniforms() {}
	virtual Lint       CreateUniform( const LString& Name )
	{
		return 1;
	}
	virtual void       SetUniformFloat( Lint Uniform, const float Float ) {}
	virtual void       SetUniformFloatArray( Lint Uniform, int Count, const float* Float ) {};
	virtual void       SetUniformVec3Array( Lint Uniform, int Count, const LVector3& Vector ) {}
	virtual void       SetUniformVec4Array( Lint Uniform, int Count, const LVector4& Vector ) {}
	virtual void       SetUniformMat3Array( Lint Uniform, int Count, const LMatrix3& Matrix ) {}
	virtual void       SetUniformMat4Array( Lint Uniform, int Count, const LMatrix4& Matrix ) {}
	virtual void       SetUniformNameFloat( const LString& Name, const float Float ) {}
	virtual void       SetUniformNameFloatArray( const LString& Name, int Count, const float* Float ) {};
	virtual void       SetUniformNameVec3Array( const LString& Name, int Count, const LVector3& Vector ) {}
	virtual void       SetUniformNameVec4Array( const LString& Name, int Count, const LVector4& Vector ) {}
	virtual void       SetUniformNameMat3Array( const LString& Name, int Count, const LMatrix3& Matrix ) {}
	virtual void       SetUniformNameMat4Array( const LString& Name, int Count, const LMatrix4& Matrix ) {}
	virtual void       SetTransformationUniforms( const sMatrices& Matrices ) {}
	virtual void       SetMaterialUniforms( const sMaterialDesc& Desc, bool ReceiveShadow ) {};
	virtual void       SetGUIUniforms( const LVector4& GUIPos, float Opacity, float ViewLifeTime ) {};
	virtual void       SetSceneUniformsPerRigid( const sMaterialDesc& Desc,
	                                             const LVector4& PickingColor,
	                                             const LVector4& KeyFrame,
	                                             bool Skinned,
	                                             bool ReceiveShadow ) {}
	virtual void       SetSceneUniformsPerShader( const LMatrix4* ShadowMatrix,
	                                              const LArray<LVector3>& LightPos,
	                                              const LArray<LVector4>& LightColor,
	                                              float EnableClipPlane,
	                                              const LVector4* ClipPlane,
	                                              bool ShowNormals ) {}
	virtual void       BindConsoleVariable( const LString& Name, clCVar* CVar ) {};
	virtual void       SetFragDataLocationName( int Location, const LString& Name ) {};
	virtual void       SetAttribLocationName( int Location, const LString& Name ) {};
	virtual void       SetInputPrimitiveType( const LString& Type ) {}
	virtual void       SetOutputPrimitiveType( const LString& Type ) {}
	virtual void       SetOutputVerticesCount( Luint OutputVerticesCount ) {}
	virtual void       SetDefinesList( const LString& DefinesList ) { };
	virtual LString    GetDefinesList() const
	{
		return "";
	};
};

#endif

/*
 * 11/06/2010
     Initial version
*/
