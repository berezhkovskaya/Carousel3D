/*
   (Part of Linderdaum Engine)
   Version 0.5.93
   (13/10/2010)
   (C) Sergey Kosarevsky, 2010
   support@linderdaum.com
   http://www.linderdaum.com
*/

#ifndef _LRandom_
#define _LRandom_

#include "Platform.h"

namespace Linderdaum
{
	namespace Math
	{
		// http://burtleburtle.net/bob/hash/integer.html
		inline Luint32 HashInt32( Luint32 Value )
		{
			Value = ( Value ^ 61 ) ^ ( Value >> 16 );
			Value = Value + ( Value << 3 );
			Value = Value ^ ( Value >> 4 );
			Value = Value * 0x27d4eb2d;
			Value = Value ^ ( Value >> 15 );
			return Value;
		}
	};
};

#endif

/*
 * 13/10/2010
     It's here
*/
