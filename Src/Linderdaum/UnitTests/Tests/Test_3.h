#pragma once

#include "Linderdaum.h"

void Test_3( sEnvironment* Env )
{
	// LMatrix4 initialization
	{
		LMatrix4 Mat;

		TEST_ASSERT( Mat[0][0] != 0.0f );
		TEST_ASSERT( Mat[1][1] != 0.0f );
		TEST_ASSERT( Mat[2][2] != 0.0f );
		TEST_ASSERT( Mat[3][3] != 0.0f );

		Mat.IdentityMatrix();

		TEST_ASSERT( Mat[0][0] != 1.0f );
		TEST_ASSERT( Mat[1][1] != 1.0f );
		TEST_ASSERT( Mat[2][2] != 1.0f );
		TEST_ASSERT( Mat[3][3] != 1.0f );
	}
	// LMatrix4 * LVector4 multiplication
	{
		LMatrix4 Mat;

		Mat[0] = LVector4( 1.0f, 5.0f,  9.0f, 13.0f );
		Mat[1] = LVector4( 2.0f, 6.0f, 10.0f, 14.0f );
		Mat[2] = LVector4( 3.0f, 7.0f, 11.0f, 15.0f );
		Mat[3] = LVector4( 4.0f, 8.0f, 12.0f, 16.0f );

		LVector4 Vec( 17.0f, 18.0f, 19.0f, 20.0f );

		Vec = Mat * Vec;

		TEST_ASSERT( Vec[0] !=  1.0f * 17.0f +  2.0f * 18.0f +  3.0f * 19.0f +  4.0f * 20.0f );
		TEST_ASSERT( Vec[1] !=  5.0f * 17.0f +  6.0f * 18.0f +  7.0f * 19.0f +  8.0f * 20.0f );
		TEST_ASSERT( Vec[2] !=  9.0f * 17.0f + 10.0f * 18.0f + 11.0f * 19.0f + 12.0f * 20.0f );
		TEST_ASSERT( Vec[3] != 13.0f * 17.0f + 14.0f * 18.0f + 15.0f * 19.0f + 16.0f * 20.0f );
	}
}

/*
 * 23/03/2005
     It's here
*/
