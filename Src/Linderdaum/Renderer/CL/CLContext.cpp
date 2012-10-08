/**
 * \file CLContext.cpp
 * \brief OpenCL GPUGPU context
 * \version 0.5.89
 * \date 20/04/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "CLContext.h"
#include "Core/Linker.h"
#include "Core/Logger.h"

#include "Generated/LCL/LCL.h"

static const int MAX_INFO_SIZE = 2048;

clCLBuffer::clCLBuffer()
	: FSize( 0 ),
	  FBuffer( 0 ),
	  FContext( NULL )
{
}

clCLBuffer::~clCLBuffer()
{
	if ( FBuffer )
	{
		clReleaseMemObject( FBuffer );
	}
}

void clCLBuffer::CreateBuffer( bool Read, bool Write, bool CopyHostMemory, size_t Size, void* HostPtr )
{
	if ( FBuffer )
	{
		clReleaseMemObject( FBuffer );
	}

	cl_mem_flags Flags = 0;

	if (  Read &&  Write )
	{
		Flags |= CL_MEM_READ_WRITE;
	}

	if ( !Read &&  Write )
	{
		Flags |= CL_MEM_WRITE_ONLY;
	}

	if (  Read && !Write )
	{
		Flags |= CL_MEM_READ_ONLY;
	}

	if (  CopyHostMemory && HostPtr )
	{
		Flags |= CL_MEM_COPY_HOST_PTR;
	}

	if ( !CopyHostMemory && HostPtr )
	{
		Flags |= CL_MEM_USE_HOST_PTR;
	}

	FSize = Size;
	FBuffer = clCreateBuffer( FContext->GetDeviceContext(), Flags, Size, HostPtr, FContext->GetLastErrorPtr() );
}

void clCLBuffer::ReadBuffer( bool BlockingRead, size_t Offset, size_t Size, void* ToPtr )
{
	FSize = Size;

	( *FContext->GetLastErrorPtr() ) = clEnqueueReadBuffer( FContext->GetCommandQueue(),
	                                                        FBuffer,
	                                                        BlockingRead ? CL_TRUE : CL_FALSE,
	                                                        Offset,
	                                                        Size,
	                                                        ToPtr,
	                                                        0,
	                                                        NULL,
	                                                        NULL );
}

void clCLBuffer::BufferData( bool BlockingRead, void* FromPtr )
{
	( *FContext->GetLastErrorPtr() ) = clEnqueueWriteBuffer( FContext->GetCommandQueue(),
	                                                         FBuffer,
	                                                         BlockingRead ? CL_TRUE : CL_FALSE,
	                                                         0,
	                                                         FSize,
	                                                         FromPtr,
	                                                         0,
	                                                         NULL,
	                                                         NULL );
}

void clCLBuffer::BufferSubData( bool BlockingRead, size_t Offset, size_t Size, void* FromPtr )
{
	( *FContext->GetLastErrorPtr() ) = clEnqueueWriteBuffer( FContext->GetCommandQueue(),
	                                                         FBuffer,
	                                                         BlockingRead ? CL_TRUE : CL_FALSE,
	                                                         Offset,
	                                                         Size,
	                                                         FromPtr,
	                                                         0,
	                                                         NULL,
	                                                         NULL );
}

clCLProgram::clCLProgram(): FProgram( 0 ),
	FContext( NULL )
{
}

clCLProgram::~clCLProgram()
{
	if ( FProgram )
	{
		clReleaseProgram( FProgram );
	}
}

void clCLProgram::SetSource( const LString& Source )
{
	if ( FProgram )
	{
		clReleaseProgram( FProgram );
	}

	const char* Data = Source.c_str();

	FProgram = clCreateProgramWithSource( FContext->GetDeviceContext(), 1, &Data, NULL, FContext->GetLastErrorPtr() );

	FContext->CheckError( __FILE__, LStr::ToStr( __LINE__ ), "Unable to create OpenCL program" );

	// build the program
	( *FContext->GetLastErrorPtr() ) = clBuildProgram( FProgram, 0, NULL, NULL, NULL, NULL );


	/*
	char *build_log;
	size_t ret_val_size;
	if (clGetProgramBuildInfo(OpenCLProgram, GPUDevices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size) != CL_SUCCESS)
	{
	   //if there was an error with the above source
	   //report the error and exit
	   build_log = new char[ret_val_size+1];
	   error = clGetProgramBuildInfo(OpenCLProgram, GPUDevices[0], CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL);
	   build_log[ret_val_size] = '\0';
	   printf("%s\n",build_log);
	   return 0;
	}
	*/
}

clCLKernel* clCLProgram::CreateKernel( const LString& KernelName )
{
	clCLKernel* Kernel = Env->Linker->Instantiate( "clCLKernel" );

	Kernel->SetProgram( FContext, this, KernelName );

	return Kernel;
}

clCLKernel::clCLKernel(): FKernel( 0 )
{
}

clCLKernel::~clCLKernel()
{
	if ( FKernel )
	{
		clReleaseKernel( FKernel );
	}
}

void clCLKernel::SetArgBuffer( Luint ArgIndex, clCLBuffer* Buffer )
{
	clSetKernelArg( FKernel, ArgIndex, sizeof( cl_mem ), Buffer->GetBufferIDPtr() );
}

void clCLKernel::Run( Luint WorkDim, const size_t* Global_WorkSize )
{
	( *FContext->GetLastErrorPtr() ) = clEnqueueNDRangeKernel( FContext->GetCommandQueue(),
	                                                           FKernel,
	                                                           WorkDim,
	                                                           NULL,
	                                                           Global_WorkSize,
	                                                           NULL,
	                                                           0,
	                                                           NULL,
	                                                           NULL );
}

void clCLKernel::SetProgram( clCLContext* Context, clCLProgram* Program, const LString& KernelName )
{
	if ( FKernel )
	{
		clReleaseKernel( FKernel );
	}

	FContext = Context;
	FProgram = Program;

	FKernel = clCreateKernel( FProgram->GetProgramID(), KernelName.c_str(), FContext->GetLastErrorPtr() );
}

clCLContext::clCLContext(): FLastError( CL_SUCCESS )
{
}

clCLContext::~clCLContext()
{
}

bool clCLContext::InitContext()
{
	LCL::clCLExtRetriever ExtRetriever;

	//FATAL_MSG( "Unable to load OpenCL library" );

	CreateContext();

	return true;
}

void clCLContext::CheckError( const LString& File, const LString& Line, const LString& Info ) const
{
	const int NumErrors = 13;

	const LString ErrStrings[NumErrors] =
	{
		"CL_SUCCESS",
		"CL_DEVICE_NOT_FOUND",
		"CL_DEVICE_NOT_AVAILABLE",
		"CL_COMPILER_NOT_AVAILABLE",
		"CL_MEM_OBJECT_ALLOCATION_FAILURE",
		"CL_OUT_OF_RESOURCES",
		"CL_OUT_OF_HOST_MEMORY",
		"CL_PROFILING_INFO_NOT_AVAILABLE",
		"CL_MEM_COPY_OVERLAP",
		"CL_IMAGE_FORMAT_MISMATCH",
		"CL_IMAGE_FORMAT_NOT_SUPPORTED",
		"CL_BUILD_PROGRAM_FAILURE",
		"CL_MAP_FAILURE"
	};


	if ( FLastError != CL_SUCCESS )
	{
		LString ES = "OpenCL error: " + LStr::ToStr( ( int )FLastError );

		if ( ( FLastError <= 0 ) && ( FLastError > -NumErrors ) )
		{
			ES += " (" + ErrStrings[ -FLastError ] + ")";
		}

		Env->Logger->Fatal( ES + "\nInfo: " + Info, File, Line, "" );
	}
}

void clCLContext::LogPlatformsInfo()
{
	Env->Logger->Log( L_NOTICE, "Available OpenCL platforms: " + LStr::ToStr( FNumPlatforms ) );

	char Info[MAX_INFO_SIZE];

	for ( cl_uint i = 0; i != FNumPlatforms; i++ )
	{
		if ( clGetPlatformInfo( FPlatforms[i], CL_PLATFORM_NAME, MAX_INFO_SIZE, Info, NULL ) == CL_SUCCESS )
		{
			Env->Logger->Log( L_NOTICE, "CL_PLATFORM_NAME: " + LString( Info ) );
		}

		if ( clGetPlatformInfo( FPlatforms[i], CL_PLATFORM_PROFILE, MAX_INFO_SIZE, Info, NULL ) == CL_SUCCESS )
		{
			Env->Logger->Log( L_NOTICE, "CL_PLATFORM_PROFILE: " + LString( Info ) );
		}

		if ( clGetPlatformInfo( FPlatforms[i], CL_PLATFORM_VERSION, MAX_INFO_SIZE, Info, NULL ) == CL_SUCCESS )
		{
			Env->Logger->Log( L_NOTICE, "CL_PLATFORM_VERSION: " + LString( Info ) );
		}

		if ( clGetPlatformInfo( FPlatforms[i], CL_PLATFORM_VENDOR, MAX_INFO_SIZE, Info, NULL ) == CL_SUCCESS )
		{
			Env->Logger->Log( L_NOTICE, "CL_PLATFORM_VENDOR: " + LString( Info ) );
		}

		if ( clGetPlatformInfo( FPlatforms[i], CL_PLATFORM_EXTENSIONS, MAX_INFO_SIZE, Info, NULL ) == CL_SUCCESS )
		{
			Env->Logger->Log( L_NOTICE, "CL_PLATFORM_EXTENSIONS: " + LString( Info ) );
		}
	}
}

void clCLContext::LogDevicesInfo()
{
	Env->Logger->Log( L_NOTICE, "Available OpenCL devices: " + LStr::ToStr( FNumDevices ) );

	char Info[MAX_INFO_SIZE];

	cl_uint Value = 0;

	for ( cl_uint i = 0; i != FNumDevices; i++ )
	{
		if ( clGetDeviceInfo( FDevices[i], CL_DEVICE_NAME, MAX_INFO_SIZE, Info, NULL ) == CL_SUCCESS )
		{
			Env->Logger->Log( L_NOTICE, "CL_DEVICE_NAME: " + LString( Info ) );
		}

		if ( clGetDeviceInfo( FDevices[i], CL_DRIVER_VERSION, MAX_INFO_SIZE, Info, NULL ) == CL_SUCCESS )
		{
			Env->Logger->Log( L_NOTICE, "CL_DRIVER_VERSION: " + LString( Info ) );
		}

		if ( clGetDeviceInfo( FDevices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof( cl_uint ), &Value, NULL ) == CL_SUCCESS )
		{
			Env->Logger->Log( L_NOTICE, "CL_DEVICE_MAX_CLOCK_FREQUENCY: " + LStr::ToStr( Value ) + " MHz" );
		}

		if ( clGetDeviceInfo( FDevices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof( cl_uint ), &Value, NULL ) == CL_SUCCESS )
		{
			Env->Logger->Log( L_NOTICE, "CL_DEVICE_MAX_COMPUTE_UNITS: " + LStr::ToStr( Value ) );
		}
	}
}

// Use: http://oscarbg.blogspot.com/2009/11/amd-opencl-samples-on-nvidia-195-opencl_05.html
void clCLContext::CreateContext()
{
	guard();

	// get platforms
	clGetPlatformIDs( 0, NULL, &FNumPlatforms );

	FPlatforms = ( cl_platform_id* )malloc( FNumPlatforms * sizeof( cl_platform_id ) );

	clGetPlatformIDs( FNumPlatforms, FPlatforms, NULL );

	LogPlatformsInfo();

	// get devices
	clGetDeviceIDs( FPlatforms[0], CL_DEVICE_TYPE_GPU, 0, 0, &FNumDevices );

	FDevices = ( cl_device_id* )malloc( FNumDevices * sizeof( cl_device_id ) );

	clGetDeviceIDs( FPlatforms[0], CL_DEVICE_TYPE_ALL, FNumDevices, FDevices, 0 );

	LogDevicesInfo();

	// create the OpenCL context on a GPU device
	FDeviceContext = clCreateContext( NULL, FNumDevices, FDevices, NULL, NULL, &FLastError );

	CheckError( __FILE__, LStr::ToStr( __LINE__ ), "Unable to create OpenCL GPU context" );

//   size_t NumBytes = 0;

	// get the list of the devices associated with the context
//   clGetContextInfo( FDeviceContext, CL_CONTEXT_DEVICES, 0, NULL, &NumBytes );

	// get devices
	//cl_device_id* Devices = (cl_device_id*)malloc( NumBytes );
	//clGetContextInfo( FDeviceContext, CL_CONTEXT_DEVICES, NumBytes, Devices, 0);

	// create a command queue for the first device the context reported
	FCmdQueue = clCreateCommandQueue( FDeviceContext, FDevices[0], 0, &FLastError );

	CheckError( __FILE__, LStr::ToStr( __LINE__ ), "Unable to create OpenCL command queue" );

	unguard();
}

void clCLContext::DeleteContext()
{
	clReleaseCommandQueue( FCmdQueue );
	clReleaseContext( FDeviceContext );

	free( FPlatforms );
	free( FDevices );
}

clCLBuffer* clCLContext::CreateBuffer() const
{
	clCLBuffer* Buffer = Env->Linker->Instantiate( "clCLBuffer" );

	Buffer->SetContext( const_cast<clCLContext*>( this ) );

	return Buffer;
}

clCLProgram* clCLContext::CreateProgramWithSource( const LString& Source ) const
{
	clCLProgram* Program = Env->Linker->Instantiate( "clCLProgram" );

	Program->SetContext( const_cast<clCLContext*>( this ) );
	Program->SetSource( Source );

	return Program;
}

/*
 * 20/04/2010
     Initial implementation of buffers/programs/kernels
 * 18/04/2010
     It's here
*/
