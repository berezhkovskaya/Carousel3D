/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUIGauge_
#define _clGUIGauge_

#include "LColors.h"
#include "GUI/ComCtl/I_BorderPanel.h"

class scriptfinal clGUIGauge: public clGUIBorderPanel
{
public:
	clGUIGauge(): FMaxValue( 0 ),
		FCurrentValue( 0 ) { SetDefaultColor( LC_Yellow ); };

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS();
	//
	// iGUIRegion interface
	//
	virtual LVector4  GetUniformsVec() const;
	//
	// iGUIView interface
	//
	virtual void    PreRender();
	//
	// clGUIGauge
	//
	virtual void    Increment( int Delta );
	virtual void    SetMaximalValue( int MaxValue ) { FMaxValue = MaxValue; };
	virtual void    SetCurrentValue( int CurrentValue ) { FCurrentValue = CurrentValue; };
	virtual int     GetMaximalValue() const { return FMaxValue; };
	virtual int     GetCurrentValue() const { return FCurrentValue; };
	/* Property( Name="MaximalValue",     Type=int,  Setter=SetMaximalValue,  Getter=GetMaximalValue  ) */
	/* Property( Name="CurrentValue",     Type=int,  Setter=SetCurrentValue,  Getter=GetCurrentValue  ) */
private:
	int      FMaxValue;
	int      FCurrentValue;
public:
	virtual void    Event_Timer( iGUIResponder* Source, float DeltaTime );
	virtual void    Event_Registered( iGUIResponder* Source );
};

#endif

/*
 * 05/04/2007
     SaveToXLMLStream()
 * 06/05/2005
     It's here
*/
