/**
 * \file iQuery.h
 * \brief Generic query
 * \version 0.5.93
 * \date 05/11/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iQuery_
#define _iQuery_

#include "Core/iObject.h"

enum LQueryType
{
	LQ_OCCLUSION,
	LQ_TIMESTAMP,
	LQ_TIME_ELAPSED
};

class netexportable iQuery: public iObject
{
public:
	iQuery() {};
	//
	// iQuery
	//
	virtual void    BeginQuery( LQueryType QueryType ) = 0;
	virtual void    EndQuery() = 0;
	virtual Luint64 GetQuery() const = 0;
	virtual bool    IsAvailable() const = 0;
};

#endif

/*
 * 05/11/2010
     LQ_TIME_ELAPSED
 * 31/03/2010
     It's here
*/
