/**
 * \file Field.cpp
 * \brief Generic interface for fields (properties) in engine's reflection system
 * \version 0.5.74
 * \date 24/07/2005
 * \author Sergey Kosarevsky, 2005
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Field.h"
#include "Environment.h"
#include "Utils/Exceptions.h"

void iField::Resize( iObject* obj, int NewSize )
{
	FATAL_MSG_ENV( obj->Env, "Cannot resize native field : " + GetFieldName() );
}

/*
 * 24/07/2005
     Log section added
*/
