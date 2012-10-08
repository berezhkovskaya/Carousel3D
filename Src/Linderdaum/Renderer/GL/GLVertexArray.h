/**
 * \file GLVertexArray.h
 * \brief OpenGL vertex array
 * \version 0.5.91
 * \date 02/07/2010
 * \author Sergey Kosarevsky, 2004-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clVertexArrayFeeder_
#define _clVertexArrayFeeder_

#include "Platform.h"
#include "Renderer/iVertexArray.h"
#include "Utils/LArray.h"

#include "GLAPIContainer.h"

class clCVar;

class scriptfinal clGLVertexArray: public iVertexArray, public clGLAPIContainer
{
public:
	clGLVertexArray();
	virtual ~clGLVertexArray();
	//
	// iObject interface
	//
	virtual void    AfterConstruction();

	//
	// iGPUBuffer interface
	//
	virtual void    FeedIntoGPU( bool Wireframe, iShaderProgram* SP ) const;
	virtual void    FeedIntoGPUInstanced( bool Wireframe, int NumInstances, iShaderProgram* SP ) const;
	//
	// iVertexArray interface
	//
	virtual void    CommitChanges();
	virtual void    SetVertexAttribs( clVertexAttribs* Attribs );

	FWD_EVENT_HANDLER( Event_SURFACE_DETACHED );
	FWD_EVENT_HANDLER( Event_SURFACE_ATTACHED );
private:
	void RegenerateOffsets();
	void BindVertexArray( iShaderProgram* SP ) const;
	void UnBindVertexArray( iShaderProgram* SP ) const;
private:
	/// type of the geometry primitive associated with this VA
	Lenum      FGLPrimitive;

#pragma region Performance counters
	clCVar*    FVALocksCount;
	size_t     FTriangles;
#pragma endregion

#pragma region VBO parameters
	Luint      FVBOID;
	Luint      FVBOIndicesID;
	Luint      FVAOID;
	/// VBO offsets
	std::vector<const void*>    FAttribVBOOffset;
#pragma endregion

	// precached values
	Lsizei     FCount;
	Lenum      FIndicesType;
	Luint*     FIndicesPtr;
	bool       FDontUseIndices;
	Luint      FVerticesPerPatch;
	/// this is equal to FAttribVBOOffset in non-VBO mode
	std::vector<const void*>    FEnumeratedStreams;
};

#endif

/*
 * 02/07/2010
     Precached fields
 * 22/12/2004
     It's here
*/
