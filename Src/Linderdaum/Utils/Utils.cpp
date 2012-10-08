/**
 * \file Utils.cpp
 * \brief Utility functions
 * \version 0.5.60
 * \date 17/12/2005
 * \author Sergey Kosarevsky, 2005
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Platform.h"
#include "Utils.h"
#include "LKeys.h"

int Linderdaum::Utils::TranslateKey( const int Key, const bool ShiftPressed )
{
	if ( ShiftPressed )
	{
		return ::Linderdaum::Utils::TranslatedKeys_Shifted[Key];
	}

	return ::Linderdaum::Utils::TranslatedKeys[Key];
}

/*
 * 03/02/2005
     TranslateKey()
 * 11/01/2005
     It's here
*/
