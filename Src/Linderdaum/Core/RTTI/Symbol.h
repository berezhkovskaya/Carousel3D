/**
 * \file Symbol.h
 * \brief Symbol declaration
 * \version 0.5.80
 * \date 29/11/2007
 * \author Viktor Latypov, 2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __Symbol__h__included__
#define __Symbol__h__included__

#include "Platform.h"

/// Abstract type for pointer representation in VM
typedef void* VMPTR;

const int VMPTR_SIZE = sizeof( VMPTR );

/// Symbol declaration
class iSymbolDeclaration
{
public:
	iSymbolDeclaration() {}
	virtual ~iSymbolDeclaration() {}

	virtual VMPTR    GetSymbolAddress() = 0;
	virtual void     Resize( int NewSize ) = 0;
};

#endif

/*
 * 29/11/2007
     VMPTR moved here
*/
