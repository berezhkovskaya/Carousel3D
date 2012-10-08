#pragma once

#ifndef __IColorDial__h__included__
#define __IColorDial__h__included__

#include "I_Panel.h"

class clRenderState;
class iShaderProgram;

/**
   \brief GTK-like color dial component

   Color dial (rainbow circle) for base color selection and an inner triangle for color fine tuning
*/
/*net_exportable*/
class scriptfinal clGUIColorDial: public clGUIPanel
{
public:
	clGUIColorDial();
	virtual ~clGUIColorDial() {}

	virtual void AfterConstruction();

	virtual void    Event_MouseInside( iGUIResponder* Source, const LVector2& Pnt )
	{
		if ( FPressed ) { SetAlpha( Pnt ); }
	}

	virtual void Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KS )
	{
		if ( FPressed ) { SetAlpha( Pnt ); }

		FPressed = KS;
	}

	/** Property(Description="Name of console variable with selected color value", Name=OutVarName, Type=string, FieldName=FOutVarName) */

	/** Property(Description="Inner circle radius", Name=RMin, Type=float, FieldName=FRMin) */
	/** Property(Description="Outer circle radius", Name=RMax, Type=float, FieldName=FRMax) */

public:
	/// The name of output vec4 variable
	LString FOutVarName;

	/// Inner dial radius
	float FRMin;

	/// Outer dial radius
	float FRMax;

private:
	/// Check mouse click and adjust current color
	void SetAlpha( const LVector2& Pnt );

	/// Current dial angle
	float FAlpha;

	/// Current barycentric coordinates inside the triangle
	LVector3 FMousePoint;

	/// Mouse state
	bool FPressed;

private:
	/// Shader to render this component
	clRenderState* DialShader;

	/// Shortcut to DialShader->GetShaderProgram()
	iShaderProgram* DialSP;

	/// Handle of the angle uniform
	Lint FAlphaUniform;

	/// Handle of the mouse selection point uniform
	Lint FMousePtUniform;

	/// Handles for rmin/rmax parameters (circle's inner and outer radius)
	Lint FRMinUniform, FRMaxUniform;
};

#endif

/*
 * 12/01/2011
     It's here
*/
