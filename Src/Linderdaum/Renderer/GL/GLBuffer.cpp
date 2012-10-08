/**
 * \file GLBuffer.cpp
 * \brief OpenCL buffer object
 * \version 0.5.91
 * \date 30/04/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "GLBuffer.h"

#include "Generated/LGL/LGL.h"

clGLBuffer::clGLBuffer(): FBufferID( 0 ),
	FTargetType( GL_UNIFORM_BUFFER )
{
}

clGLBuffer::~clGLBuffer()
{
	if ( FBufferID )
	{
		LGL3->glDeleteBuffers( 1, &FBufferID );
	}
}

void clGLBuffer::AfterConstruction()
{
	iGPUBuffer::AfterConstruction();

	GetAPI( Env );
}

void clGLBuffer::CreateBuffer( bool Read, bool Write, bool CopyHostMemory, size_t Size, void* HostPtr )
{
	FSize = Size;

	if ( FBufferID )
	{
		LGL3->glDeleteBuffers( 1, &FBufferID );
	}

	LGL3->glGenBuffers( 1, &FBufferID );

	LGL3->glBindBuffer( FTargetType, FBufferID );
	LGL3->glBufferData( FTargetType, Size, HostPtr, GL_DYNAMIC_DRAW );
}

void clGLBuffer::ReadBuffer( bool BlockingRead, size_t Offset, size_t Size, void* ToPtr )
{
	FIXME( "implement" )
}

void clGLBuffer::BufferData( bool BlockingRead, void* FromPtr )
{
	LGL3->glBindBuffer( FTargetType, FBufferID );
	LGL3->glBufferData( FTargetType, FSize, FromPtr, GL_DYNAMIC_DRAW );
}

void clGLBuffer::BufferSubData( bool BlockingRead, size_t Offset, size_t Size, void* FromPtr )
{
	LGL3->glBindBuffer( FTargetType, FBufferID );
	LGL3->glBufferSubData( FTargetType, Offset, Size, FromPtr );
}

void clGLBuffer::BindAsUniformBuffer()
{
	LGL3->glBindBufferBase( GL_UNIFORM_BUFFER, 0, FBufferID );
}

/*
 * 30/04/2010
     It's here
*/
