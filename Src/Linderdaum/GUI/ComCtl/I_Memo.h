/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUIMemo_
#define _clGUIMemo_

#include "GUI/ComCtl/I_BorderPanel.h"

/// Multiline text memo
class scriptfinal clGUIMemo: public clGUIBorderPanel
{
public:
	clGUIMemo(): FLines() {};

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS()
	//
	// iGUIView interface
	//
	virtual void    PreRender();
	//
	// clGUIMemo
	//
	virtual void    AddLine( const LString& Line );
	virtual LString GetLastLine() const { return FLines.size() ? FLines.back().FLine : ""; };
	virtual void    push_back( const LString& Line, const LVector4& Color );
	virtual void    push_front( const LString& Line, const LVector4& Color );
	/* Property( Name="TextLine",     Type=string,  Setter=AddLine,  Getter=GetLastLine ) */
public:
	/// Internal representation of a colored text line inside a memo control
	struct sMemoLine
	{
		LString     FLine;
		LVector4    FColor;
	};
private:
	std::list<sMemoLine>    FLines;
};

#endif

/*
 * 24/04/2005
     It's here
*/
