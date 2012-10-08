/**
 * \file CLContext.h
 * \brief OpenCL GPUGPU context
 * \version 0.5.89
 * \date 18/04/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _CLContext_
#define _CLContext_

#include "Platform.h"
#include "Renderer/iGPUBuffer.h"
#include "Renderer/iGPGPUContext.h"

#include "Generated/LCL/LCL.h"

TODO( "extract iGPUBuffer from here (and from iVertexArray)" )

class clCLContext;
class clCLKernel;

/// Generic data holder
class scriptfinal netexportable clCLBuffer: public iGPUBuffer
{
public:
	clCLBuffer();
	virtual ~clCLBuffer();
	//
	// iGPUBuffer interface
	//
	virtual void    CreateBuffer( bool Read, bool Write, bool CopyHostMemory, size_t Size, void* HostPtr );
	virtual void    ReadBuffer( bool BlockingRead, size_t Offset, size_t Size, void* ToPtr );
	virtual void    BufferData( bool BlockingRead, void* FromPtr );
	virtual void    BufferSubData( bool BlockingRead, size_t Offset, size_t Size, void* FromPtr );
	//
	// clCLBuffer
	//
	virtual void       SetContext( clCLContext* Context )
	{
		FContext = Context;
	};
	virtual cl_mem     GetBufferID() const
	{
		return FBuffer;
	};
	virtual cl_mem*    GetBufferIDPtr()
	{
		return &FBuffer;
	};
private:
	size_t         FSize;
	cl_mem         FBuffer;
	clCLContext*   FContext;
};

/// OpenCL program compiled from source
class scriptfinal netexportable clCLProgram: public iObject
{
public:
	clCLProgram();
	virtual ~clCLProgram();
	//
	//
	//
	virtual void           SetSource( const LString& Source );
	virtual clCLKernel*    CreateKernel( const LString& KernelName );
	//
	// clCLProgram
	//
	virtual void       SetContext( clCLContext* Context )
	{
		FContext = Context;
	};
	virtual cl_program GetProgramID() const
	{
		return FProgram;
	};
private:
	cl_program     FProgram;
	clCLContext*   FContext;
};

/// OpenCL kernel created from specified __kernel of the program
class scriptfinal netexportable clCLKernel: public iObject
{
public:
	clCLKernel();
	virtual ~clCLKernel();
	//
	// ... interface
	//
	virtual void    SetArgBuffer( Luint ArgIndex, clCLBuffer* Buffer );
	noexport virtual void    Run( Luint WorkDim, const size_t* Global_WorkSize );
	//
	// clCLKernel
	//
	virtual void    SetProgram( clCLContext* Context, clCLProgram* Program, const LString& KernelName );
private:
	cl_kernel      FKernel;
	clCLContext*   FContext;
	clCLProgram*   FProgram;
};

/// OpenCL GPGPU context
class scriptfinal netexportable clCLContext: public iGPGPUContext
{
public:
	clCLContext();
	virtual ~clCLContext();
	//
	// iGPGPUContext interface
	//
	virtual bool           InitContext();
	virtual void           CheckError( const LString& File, const LString& Line, const LString& Info ) const;
	virtual clCLBuffer*    CreateBuffer() const;
	virtual clCLProgram*   CreateProgramWithSource( const LString& Source ) const;
	//
	// clCLContext
	//
	virtual cl_context       GetDeviceContext() const
	{
		return FDeviceContext;
	};
	virtual cl_command_queue GetCommandQueue() const
	{
		return FCmdQueue;
	};
	virtual cl_int*          GetLastErrorPtr()
	{
		return &FLastError;
	};
private:
	void     CreateContext();
	void     DeleteContext();
	void     LogPlatformsInfo();
	void     LogDevicesInfo();
private:
	cl_int              FLastError;
	cl_platform_id*     FPlatforms;
	cl_uint             FNumPlatforms;
	cl_device_id*       FDevices;
	cl_uint             FNumDevices;
	cl_context          FDeviceContext;
	cl_command_queue    FCmdQueue;
};

#endif

/*
 * 18/04/2010
     It's here
*/
