/**
 * \file iVertexArray.cpp
 * \brief Vertex array interface
 * \version 0.5.90
 * \date 05/02/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Geometry/VertexAttribs.h"

#include "iVertexArray.h"

#include "Core/Logger.h"
#include "Renderer/iRenderContext.h"

iVertexArray::iVertexArray()
{
	FDefaultAttribs = clVertexAttribs::CreateEmpty();

	FAttribs = FDefaultAttribs;
}

iVertexArray::~iVertexArray()
{
	delete( FDefaultAttribs );
}

void iVertexArray::SetVertexAttribs( clVertexAttribs* Attribs )
{
	delete( FDefaultAttribs );

	FDefaultAttribs = NULL;

	// don't take ownership
	FAttribs = Attribs;
}

clVertexAttribs* iVertexArray::GetVertexAttribs() const
{
	return FAttribs;
}

/*
 * 05/02/2010
     It's here
*/
