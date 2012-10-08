/*
   (Part of Linderdaum Engine)
   Version 0.5.73
   (05/04/2007)
   (C) Sergey Kosarevsky, 2006-2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUISlider_
#define _clGUISlider_

#include "GUI/ComCtl/I_Panel.h"

#include "Renderer/iVertexArray.h"
#include "Renderer/RenderState.h"

/// Slider
class scriptfinal clGUISlider: public clGUIPanel
{
public:
	clGUISlider();

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS()
	//
	// iObject interface
	//
	virtual void    AfterConstruction();
	//
	// iGUIView interface
	//
	virtual void    PostRender();
	//
	// clGUISlider
	//
	virtual float   GetCurrentValue() const;
	virtual void    SetCurrentValue( float Value );
public:
	/* Property( Name="MinValue",     Type=float,  FieldName=FMinValue     ) */
	/* Property( Name="MaxValue",     Type=float,  FieldName=FMaxValue     ) */
	/* Property( Name="CurrentValue", Type=float,  Setter=SetCurrentValue, Getter=GetCurrentValue ) */
	/* Property( Name="ShowValue",    Type=bool,   FieldName=FShowValue    ) */
	float            FCurrentValue;
	float            FMinValue;
	float            FMaxValue;
	bool             FShowValue;
public:
	virtual void    Event_Timer( iGUIResponder* Source, float DeltaTime );
	virtual void    Event_CaptureChanged( iGUIResponder* Source );
	virtual void    Event_MouseInside( iGUIResponder* Source, const LVector2& Pnt );
	virtual void    Event_MouseLeft( iGUIResponder* Source, const LVector2& Pnt, const bool KeyState );
	virtual void    Event_LDoubleClick( iGUIResponder* Source, const LVector2& Pnt );
private:
	void        UpdateCVar();
	LVector4    GetSliderCoords();
	bool        ContainsSlider( const LVector2& Pnt );
private:
	bool             FSliderIsMoving;
	LVector2         FSliderStartPoint;
	float            FSliderStartValue;
	iVertexArray*    FSlideBarGeometry;
	clRenderState*   FSlideBarShader;
};

#endif

/*
 * 05/04/2007
     SaveToXLMLStream()
 * 14/11/2006
     It's here
*/
