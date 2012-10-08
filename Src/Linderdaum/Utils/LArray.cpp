/**
 * \file LArray.cpp
 * \brief Fast array, replacement of std::vector
 * \version 0.5.93
 * \date 04/11/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "LArray.h"

#ifndef PLATFORM_MSVC
template class LArray<const void*>;
template class LArray<int>;
template class LArray<Luint>;
template class LArray<LVector2>;
template class LArray<LVector3>;
template class LArray<LVector4>;
template class LArray<LVector4i>;
template class LArray<LMatrix4>;
#endif

/*
 * 04/11/2010
     It's here
*/
