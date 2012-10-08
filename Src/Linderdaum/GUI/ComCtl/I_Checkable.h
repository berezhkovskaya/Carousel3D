/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _iGUICheckable_
#define _iGUICheckable_

#include "GUI/ComCtl/I_Button.h"

class clRenderState;

class scriptfinal iGUICheckable: public iGUIButton
{
public:
	iGUICheckable(): FCheckedShader( NULL ),
		FUnCheckedShader( NULL ),
		FChecked( false ) {};
	virtual ~iGUICheckable() {};


//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS()

	//
	// iObject interface
	//
	virtual void    AfterConstruction();
	//
	// iGUIRegion interface
	//
	virtual LVector4  GetUniformsVec() const;
	//
	// iGUIView interface
	//
	virtual void    PreRender();
	//
	// iGUICheckable
	//
	virtual void    Check();
	virtual void    UnCheck();
	virtual void    Toggle();
	virtual bool    IsChecked()
	{
		return FChecked;
	};
	virtual void    SetCheckedShader( clRenderState* Shader );
	virtual void    SetCheckedShaderName( const LString& FileName );
	virtual LString GetCheckedShaderName() const { return FCheckedShader ? FCheckedShader->GetFileName() : ""; };
	virtual void    SetUnCheckedShader( clRenderState* Shader );
	virtual void    SetUnCheckedShaderName( const LString& FileName );
	virtual LString GetUnCheckedShaderName() const { return FUnCheckedShader ? FUnCheckedShader->GetFileName() : ""; };
	virtual void    SetBoxWidth( float W );
	virtual float   GetBoxWidth() const;

	PROPERTY( Name="HDelta1",  Type=float,   FieldName=FHDelta1 )
	PROPERTY( Name="HDelta2",  Type=float,   FieldName=FHDelta2 )
	PROPERTY( Name="BoxWidth", Type=float,   Setter=SetBoxWidth, Getter=GetBoxWidth )
public:
	float FHDelta1;
	float FHDelta2;
	float FBoxWidth;
public:
	virtual void    Event_Timer( iGUIResponder* Source, float DeltaTime );
private:
	clRenderState*    FCheckedShader;
	clRenderState*    FUnCheckedShader;
	bool              FChecked;
};

#endif

/*
 * 12/02/2005
     It's here
*/
