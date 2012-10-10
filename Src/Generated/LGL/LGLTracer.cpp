
// Autogenerated by GLExtractor 0.5.99c (gl3 mode) (C) Sergey Kosarevsky
// Part of Linderdaum Engine
// http://www.linderdaum.com

#include "LGLTracer.h"

#include "Engine.h"
#include "Environment.h"
#include "Utils/Exceptions.h"
#include "Renderer/iRenderContext.h"

sLGLAPI APIHook;
static sEnvironment* Env = NULL;

#ifdef OS_LINUX
#  define LGLAPI
#else
#  define LGLAPI _stdcall
#endif

void LGLAPI LGLTracer_glActiveTexture( GLenum texture )
{
	guard();
	APIHook.glActiveTexture( texture );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glAttachShader( GLuint program, GLuint shader )
{
	guard();
	APIHook.glAttachShader( program, shader );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glBeginQuery( GLenum target, GLuint id )
{
	guard();
	APIHook.glBeginQuery( target, id );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glBindAttribLocation( GLuint program, GLuint index, const GLchar* name )
{
	guard();
	APIHook.glBindAttribLocation( program, index, name );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glBindBuffer( GLenum target, GLuint buffer )
{
	guard();
	APIHook.glBindBuffer( target, buffer );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glBindBufferBase( GLenum target, GLuint index, GLuint buffer )
{
	guard();
	APIHook.glBindBufferBase( target, index, buffer );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glBindFragDataLocation( GLuint program, GLuint color, const GLchar* name )
{
	guard();
	APIHook.glBindFragDataLocation( program, color, name );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glBindFramebuffer( GLenum target, GLuint framebuffer )
{
	guard();
	APIHook.glBindFramebuffer( target, framebuffer );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glBindTexture( GLenum target, GLuint texture )
{
	guard( "0x%X, %u", target, texture );
	APIHook.glBindTexture( target, texture );
	CheckRendererError( "" );
	unguard();
}

#if defined(USE_OPENGL_3)
void LGLAPI LGLTracer_glBindVertexArray( GLuint array )
{
	guard();
	APIHook.glBindVertexArray( array );
	CheckRendererError( "" );
	unguard();
}
#endif

void LGLAPI LGLTracer_glBlendFunc( GLenum sfactor, GLenum dfactor )
{
	guard();
	APIHook.glBlendFunc( sfactor, dfactor );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glBufferData( GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage )
{
	guard();
	APIHook.glBufferData( target, size, data, usage );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glBufferSubData( GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data )
{
	guard();
	APIHook.glBufferSubData( target, offset, size, data );
	CheckRendererError( "" );
	unguard();
}

GLenum LGLAPI LGLTracer_glCheckFramebufferStatus( GLenum target )
{
	guard();
	return APIHook.glCheckFramebufferStatus( target );
	unguard();
}

void LGLAPI LGLTracer_glClear( GLbitfield mask )
{
	guard();
	APIHook.glClear( mask );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
{
	guard();
	APIHook.glClearColor( red, green, blue, alpha );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glClearDepth( GLclampd depth )
{
	guard();
	APIHook.glClearDepth( depth );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glClearStencil( GLint s )
{
	guard();
	APIHook.glClearStencil( s );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha )
{
	guard();
	APIHook.glColorMask( red, green, blue, alpha );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glCompileShader( GLuint shader )
{
	guard();
	APIHook.glCompileShader( shader );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glCompressedTexImage2D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data )
{
	guard();
	APIHook.glCompressedTexImage2D( target, level, internalformat, width, height, border, imageSize, data );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glCompressedTexImage3D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid* data )
{
	guard();
	APIHook.glCompressedTexImage3D( target, level, internalformat, width, height, depth, border, imageSize, data );
	CheckRendererError( "" );
	unguard();
}

GLuint LGLAPI LGLTracer_glCreateProgram()
{
	guard();
	return APIHook.glCreateProgram();
	unguard();
}

GLuint LGLAPI LGLTracer_glCreateShader( GLenum type )
{
	guard();
	return APIHook.glCreateShader( type );
	unguard();
}

void LGLAPI LGLTracer_glCullFace( GLenum mode )
{
	guard();
	APIHook.glCullFace( mode );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glDebugMessageCallbackARB( GLDEBUGPROCARB callback, const GLvoid* userParam )
{
	guard();
//	APIHook.glDebugMessageCallbackARB(callback, userParam);
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glDebugMessageControlARB( GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled )
{
	guard();
//	APIHook.glDebugMessageControlARB(source, type, severity, count, ids, enabled);
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glDeleteBuffers( GLsizei n, const GLuint* buffers )
{
	guard();
	APIHook.glDeleteBuffers( n, buffers );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glDeleteFramebuffers( GLsizei n, const GLuint* framebuffers )
{
	guard();
	APIHook.glDeleteFramebuffers( n, framebuffers );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glDeleteProgram( GLuint program )
{
	guard();
	APIHook.glDeleteProgram( program );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glDeleteQueries( GLsizei n, const GLuint* ids )
{
	guard();
	APIHook.glDeleteQueries( n, ids );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glDeleteShader( GLuint shader )
{
	guard();
	APIHook.glDeleteShader( shader );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glDeleteTextures( GLsizei n, const GLuint* textures )
{
	guard();
	APIHook.glDeleteTextures( n, textures );
	CheckRendererError( "" );
	unguard();
}

#if defined(USE_OPENGL_3)
void LGLAPI LGLTracer_glDeleteVertexArrays( GLsizei n, const GLuint* arrays )
{
	guard();
	APIHook.glDeleteVertexArrays( n, arrays );
	CheckRendererError( "" );
	unguard();
}
#endif

void LGLAPI LGLTracer_glDepthMask( GLboolean flag )
{
	guard();
	APIHook.glDepthMask( flag );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glDisable( GLenum cap )
{
	guard();
	APIHook.glDisable( cap );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glDisablei( GLenum target, GLuint index )
{
	guard();
	APIHook.glDisablei( target, index );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glDisableVertexAttribArray( GLuint index )
{
	guard();
	APIHook.glDisableVertexAttribArray( index );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glDrawArrays( GLenum mode, GLint first, GLsizei count )
{
	guard();
	APIHook.glDrawArrays( mode, first, count );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glDrawArraysInstanced( GLenum mode, GLint first, GLsizei count, GLsizei primcount )
{
	guard();
	APIHook.glDrawArraysInstanced( mode, first, count, primcount );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glDrawBuffers( GLsizei n, const GLenum* bufs )
{
	guard();
	APIHook.glDrawBuffers( n, bufs );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid* indices )
{
	guard();
	APIHook.glDrawElements( mode, count, type, indices );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glDrawElementsInstanced( GLenum mode, GLsizei count, GLenum type, const GLvoid* indices, GLsizei primcount )
{
	guard();
	APIHook.glDrawElementsInstanced( mode, count, type, indices, primcount );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glEnable( GLenum cap )
{
	guard();
	APIHook.glEnable( cap );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glEnablei( GLenum target, GLuint index )
{
	guard();
	APIHook.glEnablei( target, index );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glEnableVertexAttribArray( GLuint index )
{
	guard();
	APIHook.glEnableVertexAttribArray( index );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glEndQuery( GLenum target )
{
	guard();
	APIHook.glEndQuery( target );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glFinish()
{
	guard();
	APIHook.glFinish();
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glFlush()
{
	guard();
	APIHook.glFlush();
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glFramebufferTexture2D( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level )
{
	guard();
	APIHook.glFramebufferTexture2D( target, attachment, textarget, texture, level );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glFramebufferTexture3D( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset )
{
	guard();
	APIHook.glFramebufferTexture3D( target, attachment, textarget, texture, level, zoffset );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glGenBuffers( GLsizei n, GLuint* buffers )
{
	guard();
	APIHook.glGenBuffers( n, buffers );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glGenerateMipmap( GLenum target )
{
	guard();
	APIHook.glGenerateMipmap( target );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glGenFramebuffers( GLsizei n, GLuint* framebuffers )
{
	guard();
	APIHook.glGenFramebuffers( n, framebuffers );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glGenQueries( GLsizei n, GLuint* ids )
{
	guard();
	APIHook.glGenQueries( n, ids );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glGenTextures( GLsizei n, GLuint* textures )
{
	guard();
	APIHook.glGenTextures( n, textures );
	CheckRendererError( "" );
	unguard();
}

#if defined(USE_OPENGL_3)
void LGLAPI LGLTracer_glGenVertexArrays( GLsizei n, GLuint* arrays )
{
	guard();
	APIHook.glGenVertexArrays( n, arrays );
	CheckRendererError( "" );
	unguard();
}
#endif

void LGLAPI LGLTracer_glGetActiveAttrib( GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name )
{
	guard();
	APIHook.glGetActiveAttrib( program, index, bufSize, length, size, type, name );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glGetActiveUniform( GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name )
{
	guard();
	APIHook.glGetActiveUniform( program, index, bufSize, length, size, type, name );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glGetActiveUniformBlockiv( GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params )
{
	guard();
	APIHook.glGetActiveUniformBlockiv( program, uniformBlockIndex, pname, params );
	CheckRendererError( "" );
	unguard();
}

GLint LGLAPI LGLTracer_glGetAttribLocation( GLuint program, const GLchar* name )
{
	guard();
	return APIHook.glGetAttribLocation( program, name );
	unguard();
}

void LGLAPI LGLTracer_glGetCompressedTexImage( GLenum target, GLint level, GLvoid* img )
{
	guard();
	APIHook.glGetCompressedTexImage( target, level, img );
	CheckRendererError( "" );
	unguard();
}

GLuint LGLAPI LGLTracer_glGetDebugMessageLogARB( GLuint count, GLsizei bufsize, GLenum* sources, GLenum* types, GLuint* ids, GLenum* severities, GLsizei* lengths, GLchar* messageLog )
{
	guard();
//	return APIHook.glGetDebugMessageLogARB(count, bufsize, sources, types, ids, severities, lengths, messageLog);
	return 0;
	unguard();
}

GLenum LGLAPI LGLTracer_glGetError()
{
	guard();
	return APIHook.glGetError();
	unguard();
}

void LGLAPI LGLTracer_glGetIntegerv( GLenum pname, GLint* params )
{
	guard();
	APIHook.glGetIntegerv( pname, params );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glGetProgramBinary( GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, GLvoid* binary )
{
	guard();
	APIHook.glGetProgramBinary( program, bufSize, length, binaryFormat, binary );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glGetProgramInfoLog( GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog )
{
	guard();
	APIHook.glGetProgramInfoLog( program, bufSize, length, infoLog );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glGetProgramiv( GLuint program, GLenum pname, GLint* params )
{
	guard();
	APIHook.glGetProgramiv( program, pname, params );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glGetQueryObjectiv( GLuint id, GLenum pname, GLint* params )
{
	guard();
	APIHook.glGetQueryObjectiv( id, pname, params );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glGetQueryObjectui64v( GLuint id, GLenum pname, GLuint64* params )
{
	guard();
	APIHook.glGetQueryObjectui64v( id, pname, params );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glGetShaderInfoLog( GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog )
{
	guard();
	APIHook.glGetShaderInfoLog( shader, bufSize, length, infoLog );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glGetShaderiv( GLuint shader, GLenum pname, GLint* params )
{
	guard();
	APIHook.glGetShaderiv( shader, pname, params );
	CheckRendererError( "" );
	unguard();
}

const GLubyte* LGLAPI LGLTracer_glGetString( GLenum name )
{
	guard();
	return APIHook.glGetString( name );
	unguard();
}

#if defined(USE_OPENGL_3)
const GLubyte* LGLAPI LGLTracer_glGetStringi( GLenum name, GLuint index )
{
	guard();
	return APIHook.glGetStringi( name, index );
	unguard();
}
#endif

void LGLAPI LGLTracer_glGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid* pixels )
{
	guard();
	APIHook.glGetTexImage( target, level, format, type, pixels );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint* params )
{
	guard();
	APIHook.glGetTexLevelParameteriv( target, level, pname, params );
	CheckRendererError( "" );
	unguard();
}

GLint LGLAPI LGLTracer_glGetUniformLocation( GLuint program, const GLchar* name )
{
	guard();
	return APIHook.glGetUniformLocation( program, name );
	unguard();
}

GLboolean LGLAPI LGLTracer_glIsProgram( GLuint program )
{
	guard();
	return APIHook.glIsProgram( program );
	unguard();
}

GLboolean LGLAPI LGLTracer_glIsShader( GLuint shader )
{
	guard();
	return APIHook.glIsShader( shader );
	unguard();
}

void LGLAPI LGLTracer_glLinkProgram( GLuint program )
{
	guard();
	APIHook.glLinkProgram( program );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glPatchParameteri( GLenum pname, GLint value )
{
	guard();
	APIHook.glPatchParameteri( pname, value );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glPixelStorei( GLenum pname, GLint param )
{
	guard();
	APIHook.glPixelStorei( pname, param );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glPolygonMode( GLenum face, GLenum mode )
{
	guard();
	APIHook.glPolygonMode( face, mode );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glProgramBinary( GLuint program, GLenum binaryFormat, const GLvoid* binary, GLsizei length )
{
	guard();
	APIHook.glProgramBinary( program, binaryFormat, binary, length );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glProgramParameteri( GLuint program, GLenum pname, GLint value )
{
	guard();
	APIHook.glProgramParameteri( program, pname, value );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glReadBuffer( GLenum mode )
{
	guard();
	APIHook.glReadBuffer( mode );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels )
{
	guard();
	APIHook.glReadPixels( x, y, width, height, format, type, pixels );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glShaderSource( GLuint shader, GLsizei count, const GLchar* *string, const GLint* length )
{
	guard();
	APIHook.glShaderSource( shader, count, string, length );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels )
{
	guard();
	APIHook.glTexSubImage2D( target, level, xoffset, yoffset, width, height, format, type, pixels );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels )
{
	guard( "0x%X, %i, 0x%X, %u, %u, %i, 0x%X, 0x%X, %p", target, level, internalformat, width, height, border, format, type, pixels );
	APIHook.glTexImage2D( target, level, internalformat, width, height, border, format, type, pixels );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glTexImage3D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels )
{
	guard();
	APIHook.glTexImage3D( target, level, internalformat, width, height, depth, border, format, type, pixels );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glTexParameterf( GLenum target, GLenum pname, GLfloat param )
{
	guard();
	APIHook.glTexParameterf( target, pname, param );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glTexParameteri( GLenum target, GLenum pname, GLint param )
{
	guard( "0x%X, 0x%X, %i", target, pname, param );
	APIHook.glTexParameteri( target, pname, param );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glUniform1f( GLint location, GLfloat v0 )
{
	guard( "%i, %f", location, v0 );
	APIHook.glUniform1f( location, v0 );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glUniform1fv( GLint location, GLsizei Count, const GLfloat* v0 )
{
	guard();
	APIHook.glUniform1fv( location, Count, v0 );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glUniform1i( GLint location, GLint v0 )
{
	guard();
	APIHook.glUniform1i( location, v0 );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glUniform1iv( GLint location, GLsizei count, const GLint* v0 )
{
	guard();
	APIHook.glUniform1iv( location, count, v0 );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glUniform3fv( GLint location, GLsizei count, const GLfloat* value )
{
	guard();
	APIHook.glUniform3fv( location, count, value );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glUniform4fv( GLint location, GLsizei count, const GLfloat* value )
{
	guard();
	APIHook.glUniform4fv( location, count, value );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glUniformBlockBinding( GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding )
{
	guard();
	APIHook.glUniformBlockBinding( program, uniformBlockIndex, uniformBlockBinding );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glUniformMatrix3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
{
	guard();
	APIHook.glUniformMatrix3fv( location, count, transpose, value );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glUniformMatrix4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
{
	guard();
	APIHook.glUniformMatrix4fv( location, count, transpose, value );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glUseProgram( GLuint program )
{
	guard();
	APIHook.glUseProgram( program );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glValidateProgram( GLuint program )
{
	guard();
	APIHook.glValidateProgram( program );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer )
{
	guard();
	APIHook.glVertexAttribPointer( index, size, type, normalized, stride, pointer );
	CheckRendererError( "" );
	unguard();
}

void LGLAPI LGLTracer_glViewport( GLint x, GLint y, GLsizei width, GLsizei height )
{
	guard();
	APIHook.glViewport( x, y, width, height );
	CheckRendererError( "" );
	unguard();
}

#define INJECT(S) API.S = &LGLTracer_##S;

void LGL::InjectAPITracer( sEnvironment* E, sLGLAPI* OldAPI )
{
	APIHook = *OldAPI;
	Env = E;

	sLGLAPI API = *OldAPI;

	INJECT( glActiveTexture )
	INJECT( glAttachShader )
	INJECT( glBeginQuery )
	INJECT( glBindAttribLocation )
	INJECT( glBindBuffer )
	INJECT( glBindBufferBase )
	INJECT( glBindFragDataLocation )
	INJECT( glBindFramebuffer )
	INJECT( glBindTexture )
	INJECT( glBlendFunc )
	INJECT( glBufferData )
	INJECT( glBufferSubData )
	INJECT( glCheckFramebufferStatus )
	INJECT( glClear )
	INJECT( glClearColor )
	INJECT( glClearDepth )
	INJECT( glClearStencil )
	INJECT( glColorMask )
	INJECT( glCompileShader )
	INJECT( glCompressedTexImage2D )
	INJECT( glCompressedTexImage3D )
	INJECT( glCreateProgram )
	INJECT( glCreateShader )
	INJECT( glCullFace )
	INJECT( glDebugMessageCallbackARB )
	INJECT( glDebugMessageControlARB )
	INJECT( glDeleteBuffers )
	INJECT( glDeleteFramebuffers )
	INJECT( glDeleteProgram )
	INJECT( glDeleteQueries )
	INJECT( glDeleteShader )
	INJECT( glDeleteTextures )
	INJECT( glDepthMask )
	INJECT( glDisable )
	INJECT( glDisablei )
	INJECT( glDisableVertexAttribArray )
	INJECT( glDrawArrays )
	INJECT( glDrawArraysInstanced )
	INJECT( glDrawBuffers )
	INJECT( glDrawElements )
	INJECT( glDrawElementsInstanced )
	INJECT( glEnable )
	INJECT( glEnablei )
	INJECT( glEnableVertexAttribArray )
	INJECT( glEndQuery )
	INJECT( glFinish )
	INJECT( glFlush )
	INJECT( glFramebufferTexture2D )
	INJECT( glFramebufferTexture3D )
	INJECT( glGenBuffers )
	INJECT( glGenerateMipmap )
	INJECT( glGenFramebuffers )
	INJECT( glGenQueries )
	INJECT( glGenTextures )
	INJECT( glGetActiveAttrib )
	INJECT( glGetActiveUniform )
	INJECT( glGetActiveUniformBlockiv )
	INJECT( glGetAttribLocation )
	INJECT( glGetCompressedTexImage )
	INJECT( glGetDebugMessageLogARB )
	//INJECT(glGetError)
	INJECT( glGetIntegerv )
	INJECT( glGetProgramBinary );
	INJECT( glGetProgramInfoLog )
	INJECT( glGetProgramiv )
	INJECT( glGetQueryObjectiv )
	INJECT( glGetQueryObjectui64v )
	INJECT( glGetShaderInfoLog )
	INJECT( glGetShaderiv )
	INJECT( glGetString )
	INJECT( glGetTexImage )
	INJECT( glGetTexLevelParameteriv )
	INJECT( glGetUniformLocation )
	INJECT( glIsProgram )
	INJECT( glIsShader )
	INJECT( glLinkProgram )
	INJECT( glPatchParameteri )
	INJECT( glPixelStorei )
	INJECT( glPolygonMode )
	INJECT( glProgramBinary )
	INJECT( glProgramParameteri )
	INJECT( glReadBuffer )
	INJECT( glReadPixels )
	INJECT( glShaderSource )
	INJECT( glTexSubImage2D )
	INJECT( glTexImage2D )
	INJECT( glTexImage3D )
	INJECT( glTexParameterf )
	INJECT( glTexParameteri )
	INJECT( glUniform1f )
	INJECT( glUniform1fv )
	INJECT( glUniform1i )
	INJECT( glUniform1iv )
	INJECT( glUniform3fv )
	INJECT( glUniform4fv )
	INJECT( glUniformBlockBinding )
	INJECT( glUniformMatrix3fv )
	INJECT( glUniformMatrix4fv )
	INJECT( glUseProgram )
	INJECT( glValidateProgram )
	INJECT( glVertexAttribPointer )
	INJECT( glViewport );

	*OldAPI = API;
}

/*
 * 10/03/2011
     It's here
*/