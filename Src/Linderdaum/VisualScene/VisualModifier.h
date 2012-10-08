/**
 * \file VisualModifier.h
 * \brief Visual modifier
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __VisualModifer__h__included__
#define __VisualModifer__h__included__

#include "VisualObject.h"

class scriptfinal netexportable clVisualModifier : public clVisualObject
{
public:
	clVisualModifier() {}
	virtual ~clVisualModifier() {}

	SERIALIZABLE_CLASS()
};

#endif

/*
 * 09/12/2010
     Log section added
*/
