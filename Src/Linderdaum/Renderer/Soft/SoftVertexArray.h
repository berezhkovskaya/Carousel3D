/**
 * \file SoftVertexArray.h
 * \brief 'Software' vertex array
 * \version 0.5.90
 * \date 17/12/2010
 * \author Viktor Latypov, 2010
 * \author viktor@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clSoftVertexArray
#define _clSoftVertexArray

#include "Platform.h"
#include "Renderer/iVertexArray.h"

class scriptfinal clSoftVertexArray: public iVertexArray
{
public:
	clSoftVertexArray() {}
	virtual ~clSoftVertexArray() {}

	virtual void    FeedIntoGPU( bool Wireframe, iShaderProgram* SP ) const {};
	virtual void    FeedIntoGPUInstanced( bool Wireframe, int NumInstances, iShaderProgram* SP ) const {};
	virtual void    CommitChanges() {}
};

#endif

/*
 * 10/06/2010
     Initial release
*/
