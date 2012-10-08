#pragma once

#ifndef __Line__h__included__
#define __Line__h__included__

#include "LVector.h"

/// Infinite line defined by two points
class LLine
{
public:
	LLine();

	LLine( const LVector3& P1, const LVector3& P2 );
};

/// 3D Line segment defined by two points
class LLineSegment
{
};

#endif

/*
 * 08/01/2011
     It's here
*/
