/**
 * \file I_Panel.h
 * \brief GUI panel
 * \version 0.5.74
 * \date 06/05/2007
 * \author Sergey Kosarevsky, 2005-2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clGUIPanel_
#define _clGUIPanel_

#include "GUI/iGUIView.h"

#include "Images/Image.h"
#include "Renderer/iVertexArray.h"
#include "Renderer/RenderState.h"

#include "Generated/Functors.h"


// can be used as icon
class scriptfinal clGUIPanel: public iGUIView
{
public:
	typedef ::Linderdaum::Utils::clFunctor<void, TYPELIST_0() >             clHookProc;
public:
	clGUIPanel();
	virtual ~clGUIPanel();

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS();

	//
	// iObject interface
	//
	virtual void    AfterConstruction();
	//
	// iGUIView interface
	//
	virtual void    PreRender();
	virtual void    PostRender();
	//
	// clGUIPanel
	//
	virtual void    SetBevelRadius( float Radius );
	virtual void    SetShader( clRenderState* Shader ) { FShader = Shader; };
	virtual clRenderState* GetShader() const { return FShader; };
	virtual void    SetShaderName( const LString& FileName );
	virtual void    SetImage( clImage* Image );
	virtual void    SetImageName( const LString& FileName );
	virtual LString GetShaderName() const { return FShader ? FShader->GetFileName() : ""; };
	virtual LString GetImageName() const { return FImage ? FImage->GetFileName() : ""; };
	virtual void    SetShaderMouseOver( clRenderState* Shader ) { FShaderMouseOver = Shader; };
	virtual clRenderState* GetShaderMouseOver() const { return FShaderMouseOver; };
	virtual void    SetShaderMouseOverName( const LString& FileName );
	virtual void    SetImageMouseOver( clImage* Image );
	virtual void    SetImageMouseOverName( const LString& FileName );
	virtual LString GetShaderMouseOverName() const { return FShaderMouseOver ? FShaderMouseOver->GetFileName() : ""; };
	virtual LString GetImageMouseOverName() const { return FImageMouseOver ? FImageMouseOver->GetFileName() : ""; };
	virtual void    SetShaderSwitchedOn( clRenderState* Shader ) { FShaderSwitchedOn = Shader; };
	virtual clRenderState* GetShaderSwitchedOn() const { return FShaderSwitchedOn; };
	virtual void    SetShaderSwitchedOnName( const LString& FileName );
	virtual void    SetImageSwitchedOn( clImage* Image );
	virtual void    SetImageSwitchedOnName( const LString& FileName );
	virtual LString GetShaderSwitchedOnName() const { return FShaderSwitchedOn ? FShaderSwitchedOn->GetFileName() : ""; };
	virtual LString GetImageSwitchedOnName() const { return FImageSwitchedOn ? FImageSwitchedOn->GetFileName() : ""; };
	virtual void    SetHook( const clHookProc& Hook )
	{
		FHook = Hook;
		FHookSet = true;
	};
	virtual void    SetUniformsV( const LVector4& Uniforms ) { FUniformsV = Uniforms; };
	//
	// iGUIRegion interface
	//
	virtual bool    Event_Key( iGUIResponder* Source, const int Key, const bool KeyState );
	virtual void    Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState );
	virtual void    Event_LDoubleClick( iGUIResponder* Source, const LVector2& Pnt );
public:
	/* Property( Name="Icon",                Type=string,  Setter=SetShaderName,           Getter=GetShaderName           ) */
	/* Property( Name="IconMouseOver",       Type=string,  Setter=SetShaderMouseOverName,  Getter=GetShaderMouseOverName  ) */
	/* Property( Name="IconSwitchedOn",      Type=string,  Setter=SetShaderSwitchedOnName, Getter=GetShaderSwitchedOnName ) */
	/* Property( Name="ExecCommandsOnClick", Type=bool,    FieldName=FExecCommandsOnClick                                 ) */
	// this has priority to override Icon properties
	/* Property( Name="Image",               Type=string,  Setter=SetImageName,           Getter=GetImageName           ) */
	/* Property( Name="ImageMouseOver",      Type=string,  Setter=SetImageMouseOverName,  Getter=GetImageMouseOverName  ) */
	/* Property( Name="ImageSwitchedOn",     Type=string,  Setter=SetImageSwitchedOnName, Getter=GetImageSwitchedOnName ) */
	/* Property( Name="Shader",              Type=clRenderState,  Setter=SetShader,           Getter=GetShader          ) */
	/* Property( Name="ShaderMouseOver",     Type=clRenderState,  Setter=SetShaderMouseOver,  Getter=GetShaderMouseOver ) */
	/* Property( Name="ShaderSwitchedOn",    Type=clRenderState,  Setter=SetShaderSwitchedOn, Getter=GetShaderSwitchedOn) */
	bool          FExecCommandsOnClick;
private:
	bool          FHookSet;
	clHookProc    FHook;
	float         FBevelRadius;
	LVector4      FUniformsV;
protected:
	clRenderState*    FShader;
	clRenderState*    FShaderMouseOver;
	clRenderState*    FShaderSwitchedOn;

	clImage*    FImage;
	clImage*    FImageMouseOver;
	clImage*    FImageSwitchedOn;

	iVertexArray*     FGeometry;
};

#endif

/*
 * 06/05/2007
     SetShaderSwitchedOn()
 * 05/04/2007
     SaveToXLMLStream()
 * 30/01/2005
     It's here
*/
