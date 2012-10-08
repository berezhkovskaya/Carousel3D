/**
 * \file Gestures.h
 * \brief Gestures handling helpers
 * \version 0.6.02
 * \date 27/12/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

//
// WORK IN PROGRESS. Will replace classes from Gestures.h in the near future.
//

#ifndef __Gestures2__
#define __Gestures2__

#include "Core/iObject.h"
#include "Math/LMatrix.h"
#include "Gestures.h"

/**
   \brief Fling gesture handler 2 - experimental
*/
class clFlingHandler2: public clGestureHandler
{
public:
	clFlingHandler2()
	{
		FlingThresholdSensitivity = 0.1f;
		FlingStartSensitivity  = 0.2f;

		FFlingWasValid = false;
	}

protected:
	virtual void UpdateGesture();
private:
	/// pointer movements below this value are ignored
	float FlingThresholdSensitivity;

	/// pointer movements below this value will not generate a fling
	float FlingStartSensitivity;
private:
	bool FFlingWasValid;
};

#endif

/*
 * 22/07/2011
     Added center point
 * 21/07/2011
     It's here
*/
