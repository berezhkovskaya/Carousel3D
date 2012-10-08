/**
 * \file iVertexArray.h
 * \brief Vertex array interface
 * \version 0.5.84
 * \date 20/12/2007
 * \author Sergey Kosarevsky, 2005-2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iVertexArray_
#define _iVertexArray_

#include "Math/LVector.h"

#include "Core/iObject.h"
#include "Geometry/Joints.h"

class iShaderProgram;
class clVertexAttribs;

/// GPU vertex array interface
class netexportable scriptfinal iVertexArray: public iObject
{
public:
	iVertexArray();
	virtual ~iVertexArray();

	//
	// iVertexArray
	//
	/// Render current vertex array once
	virtual void               FeedIntoGPU( bool Wireframe, iShaderProgram* SP ) const = 0;
	/// Render current vertex array NumInstances times
	virtual void               FeedIntoGPUInstanced( bool Wireframe, int NumInstances, iShaderProgram* SP ) const = 0;
	/// Commit changes from attached vertex attribs
	virtual void               CommitChanges() = 0;
	/// Attach new vertex attribs (replace with no deallocation)
	virtual void               SetVertexAttribs( clVertexAttribs* Attribs );
	/// Get currently attached vertex attribs
	virtual clVertexAttribs*   GetVertexAttribs() const;

private:
	clVertexAttribs*    FDefaultAttribs;
	clVertexAttribs*    FAttribs;
};

#endif

/*
 * 20/12/2007
     Initial support for mesh skinning
 * 30/06/2007
     sVAAllocationInfo is now packed
 * 02/06/2007
     GetInterpolationCoef()
 * 21/05/2007
     GetKeyframesCount()
 * 19/05/2007
     AllocVertices() accepts number of keyframes as parameter
 * 06/04/2007
     CommitChanges()
     AllocVertices() interface changed
 * 19/12/2004
     It's here
*/
