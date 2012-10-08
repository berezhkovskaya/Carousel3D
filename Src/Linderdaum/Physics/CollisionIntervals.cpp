/**
 * \file CollisionIntervals.cpp
 * \brief Collision intervals
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "CollisionIntervals.h"
#include "CollisionShape.h"

LAxisList::LAxisList()
{
}

LAxisList::~LAxisList()
{
}

void LAxisList::Insert( int ShapeIndex, LCollisionShape* Shape )
{
}

void LAxisList::Remove( int ShapeIndex )
{
}

void LAxisList::Clear()
{
}

bool LAxisList::CollidesWithSomeone( LInterval* Int )
{
	return false;
}

void LAxisList::CheckCollisions()
{
}

LInterval* LAxisList::ObjectToInterval( LCollisionShape* Shape )
{
	return NULL;
}

/*
 * 09/12/2010
     Log section added
*/
