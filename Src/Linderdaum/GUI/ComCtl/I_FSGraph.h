/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _clGUIFullScreenGraph_
#define _clGUIFullScreenGraph_

#include "GUI/ComCtl/I_Panel.h"

static const int LIST_SIZE = 1000;

class scriptfinal clGUIFullScreenGraph: public clGUIPanel
{
public:
	clGUIFullScreenGraph(): FReadVarName( "FPS" ), FStart( LIST_SIZE - 1 )
	{
		for ( int i = 0; i != LIST_SIZE; ++i )
		{
			FList[i] = 100;
		}
	}

//	NET__EXPORTABLE()
	SERIALIZABLE_CLASS()
	//
	// iGUIRegion interface
	//
	virtual bool    Contains( const LVector2& Pnt ) const { return false; }
	virtual bool    ContainsCaption( const LVector2& Pnt ) const { return false; }
	virtual bool    ContainsResizeCorner( const LVector2& Pnt ) const { return false; }
	virtual bool    ContainsCloseButton( const LVector2& Pnt ) const { return false; }
	virtual bool    CanFocus() const { return false; }

public:

	/** Property(Name=ReadVarName, Type=string, FieldName=FReadVarName) */

	LString FReadVarName;

	virtual void    Event_Registered( iGUIResponder* Source );
	virtual void    Event_Timer( iGUIResponder* Source, float DeltaTime );
private:
	float    GetAverage() const;
	float    GetMax() const;
	float    GetValue( int Index ) const;
private:
	float    FList[ LIST_SIZE ];
	int      FStart;
};

#endif

/*
 * 18/11/2005
     It's here
*/
