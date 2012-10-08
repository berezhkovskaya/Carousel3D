#pragma once

#ifndef __IMtxView__h__included__
#define __IMtxView__h__included__

#include "I_Panel.h"

class clRenderState;
class iShaderProgram;

/**
   \brief Orientation (x,y,z) axis triple
*/
/** netexportable */
class scriptfinal clGUIMtxView: public clGUIPanel
{
public:
	clGUIMtxView();
	virtual ~clGUIMtxView() {}

	virtual void AfterConstruction();

	/** Property(Description="Line thickness", Name=LineThickness, Type=float, FieldName=FLineThickness) */

	virtual void    Event_Timer( iGUIResponder* Source, float DeltaTime );

public:
	/// The name of input mtx4 variable
	LString FInVariable;

	/// Direct pointer to matrix. CVar is used if this is NULL
	LMatrix4* FMtxPtr;

	/// Line thickness
	float FLineThickness;

private:
	/// Current value
	LMatrix4 FView;

	/// Shader to render this component
	clRenderState* ViewShader;

	/// Shortcut to ViewShader->GetShaderProgram()
	iShaderProgram* ViewSP;

	/// Handle of the mtx uniform
	Lint FMatrixUniform;

	/// Handle of the line thickness uniform
	Lint FThicknessUniform;
};

#endif
