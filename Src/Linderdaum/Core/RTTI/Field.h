/**
 * \file Field.h
 * \brief Generic interface for fields in engine's reflection system
 * \version 0.5.91
 * \date 26/09/2010
 * \author Viktor Latypov, 2005-2010
 * \author viktor@linderdaum.com, support@linderdaum.com http://www.linderdaum.com
 */

// LV: I guess, this is a misprint. There were no fields in Linderdaum's VM in 2005

#ifndef _iField_h__included__
#define _iField_h__included__

#include "Core/iObject.h"

/// Generic interface for fields (properties) in engine's reflection system
class scriptfinal iField : public iObject
{
public:
	iField(): FFieldName( "" ) {};
	virtual ~iField() {};

	SERIALIZABLE_CLASS()
	NET_EXPORTABLE()

	//
	// iField
	//
	virtual void*       GetFieldPtr( iObject* obj ) = 0;
	virtual int         GetFieldSize() = 0;

	// iSymbol-like interface
	virtual void        Resize( iObject* obj, int NewSize );

	virtual LString     GetFieldName() const
	{
		return FFieldName;
	};
	virtual void        SetFieldName( const LString& FieldName )
	{
		FFieldName = FieldName;
	};

	virtual void        SetFieldSize( int SS ) {}

	/** Property(Name=FieldName, Type=string, Getter=GetFieldName, Setter=SetFieldName) */
	/** Property(Name=FieldSize, Type=int, Getter=GetFieldSize, Setter=SetFieldSize) */
private:
	LString FFieldName;
};


#endif

/*
 * 26/09/2010
     Adapted for serialization
 * 10/06/2007
     New interface
 * 06/07/2005
     It's here
*/
