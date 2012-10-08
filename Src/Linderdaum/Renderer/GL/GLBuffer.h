/**
 * \file GLBuffer.h
 * \brief OpenCL buffer object
 * \version 0.5.91
 * \date 30/04/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _GLBuffer_
#define _GLBuffer_

#include "Platform.h"
#include "Renderer/iGPUBuffer.h"

#include "Generated/LCL/LCL.h"

#include "GLAPIContainer.h"

/// Generic data holder
class scriptfinal clGLBuffer: public iGPUBuffer, public clGLAPIContainer
{
public:
	clGLBuffer();
	virtual ~clGLBuffer();
	//
	// iObject interface
	//
	virtual void    AfterConstruction();
	//
	// iGPUBuffer interface
	//
	virtual void    CreateBuffer( bool Read, bool Write, bool CopyHostMemory, size_t Size, void* HostPtr );
	virtual void    ReadBuffer( bool BlockingRead, size_t Offset, size_t Size, void* ToPtr );
	virtual void    BufferData( bool BlockingRead, void* FromPtr );
	virtual void    BufferSubData( bool BlockingRead, size_t Offset, size_t Size, void* FromPtr );
	virtual void    BindAsUniformBuffer();
	//
	// clGLBuffer
	//
	virtual Luint    GetBufferID() const
	{
		return FBufferID;
	};
private:
	Luint    FBufferID;
	Lenum    FTargetType;
	size_t   FSize;
};


#endif

/*
 * 18/04/2010
     It's here
*/
