/**
 * \file GLQuery.cpp
 * \brief OpenGL generic query implementation
 * \version 0.5.93
 * \date 05/11/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "GLQuery.h"

#include "Generated/LGL/LGL.h"

clGLQuery::clGLQuery(): FQuery( 0 )
{
}

clGLQuery::~clGLQuery()
{
	if ( FQuery ) { LGL3->glDeleteQueries( 1, &FQuery ); }
}

void clGLQuery::AfterConstruction()
{
	iQuery::AfterConstruction();

	GetAPI( Env );
}

void clGLQuery::BeginQuery( LQueryType QueryType )
{
	if ( !FQuery ) { LGL3->glGenQueries( 1, &FQuery ); }

	FType = LQ_OCCLUSION;

	if ( QueryType == LQ_TIMESTAMP )
	{
		FType = GL_TIMESTAMP;
	}
	else if ( QueryType == LQ_TIME_ELAPSED )
	{
		FType = GL_TIME_ELAPSED;
	}

	LGL3->glBeginQuery( FType, FQuery );
}

void clGLQuery::EndQuery()
{
	LGL3->glEndQuery( FType );
}

Luint64 clGLQuery::GetQuery() const
{
	Luint64 Result = 0;

	LGL3->glGetQueryObjectui64v( FQuery, GL_QUERY_RESULT, &Result );

	return Result;
}

bool clGLQuery::IsAvailable() const
{
	Lint Available = 0;

	LGL3->glGetQueryObjectiv( FQuery, GL_QUERY_RESULT_AVAILABLE, &Available );

	return Available != 0;
}

/*
 * 05/11/2010
     LQ_TIME_ELAPSED
 * 31/03/2010
     It's here
*/
