/**
 * \file LVector.cpp
 * \brief 2D/3D/4D vectors
 * \version 0.5.93
 * \date 04/11/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "LMath.h"

LVector2::LVector2( const LVector2i& Vec )
 : X( static_cast<float>( Vec.X ) ),
	Y( static_cast<float>( Vec.Y ) )
{
}

void LVector2::Normalize()
{
	float VecLength = Length();

	if ( VecLength > ::Linderdaum::Math::EPSILON ) { *this *= ( 1.0f / VecLength ); }
}

void LVector3::Normalize()
{
	float VecLength = Length();

	if ( VecLength > ::Linderdaum::Math::EPSILON ) { *this *= ( 1.0f / VecLength ); }
}

void LVector4::Normalize()
{
	float VecLength = Length();

	if ( VecLength > ::Linderdaum::Math::EPSILON ) { *this *= ( 1.0f / VecLength ); }
}

/*
 * 04/11/2010
     LVector3 unary operator-
 * 03/11/2010
     LVector4i::operator==
     LVector4i::operator!=
 * 16/09/2010
     LVector3i
 * 03/06/2006
     Lerp()
 * 30/05/2005
     OrthogonalVector()
 * 19/03/2005
     More vector operators
 * 16/12/2004
     Normalize()
 * 15/12/2004
     It's here
*/
