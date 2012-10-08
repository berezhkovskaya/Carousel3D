/**
 * \file iGPUBuffer.h
 * \brief Generic GPU buffer
 * \version 0.5.91
 * \date 30/04/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iGPUBuffer_
#define _iGPUBuffer_

#include "Core/iObject.h"

class netexportable scriptfinal iGPUBuffer: public iObject
{
public:
	iGPUBuffer() {};
	//
	// iGPUBuffer
	//
	/** Create new GPGPU buffer. Read/write define the access mode.
	    If CopyHostMemory is true the buffer content will be initialized with data from HostPtr
	**/
	virtual void    CreateBuffer( bool Read, bool Write, bool CopyHostMemory, size_t Size, void* HostPtr ) = 0;
	virtual void    ReadBuffer( bool BlockingRead, size_t Offset, size_t Size, void* ToPtr ) = 0;
	virtual void    BufferData( bool BlockingRead, void* FromPtr ) = 0;
	virtual void    BufferSubData( bool BlockingRead, size_t Offset, size_t Size, void* FromPtr ) = 0;
	virtual void    BindAsUniformBuffer() {};
};

#endif

/*
 * 30/04/2010
     It's here
*/
