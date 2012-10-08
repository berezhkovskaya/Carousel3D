/**
 * \file iGPGPUContext.h
 * \brief GPUGPU context
 * \version 0.5.89
 * \date 18/04/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iGPGPUContext_
#define _iGPGPUContext_

#include "Core/iObject.h"

/// OpenCL GPGPU context
class netexportable scriptfinal iGPGPUContext: public iObject
{
public:
	iGPGPUContext() {};
	virtual ~iGPGPUContext() {};
	//
	// iGPGPUContext interface
	//
	/// Create new GPGPU context (non-reenterable)
	virtual bool    InitContext() = 0;
	/// Check for GPGPU error. Usualy used with a macro to supply correct source code location (File, Line, Info)
	virtual void    CheckError( const LString& File, const LString& Line, const LString& Info ) const = 0;
};

#endif

/*
 * 18/04/2010
     It's here
*/
