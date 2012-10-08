/**
 * \file GLQuery.h
 * \brief OpenGL generic query implementation
 * \version 0.5.91
 * \date 31/03/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clGLQuery_
#define _clGLQuery_

#include "Renderer/iQuery.h"

#include "GLAPIContainer.h"

class scriptfinal clGLQuery: public iQuery, public clGLAPIContainer
{
public:
	clGLQuery();
	virtual ~clGLQuery();
	//
	// iObject interface
	//
	virtual void    AfterConstruction();

	//
	// iQuery interface
	//
	virtual void    BeginQuery( LQueryType QueryType );
	virtual void    EndQuery();
	virtual Luint64 GetQuery() const;
	virtual bool    IsAvailable() const;
private:
	Luint   FQuery;
	Lenum   FType;
};

#endif

/*
 * 31/03/2010
     It's here
*/
