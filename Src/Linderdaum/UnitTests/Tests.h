#ifndef _Tests_H_
#define _Tests_H_

#define TEST_ASSERT(cond) if ( cond ) Env->Logger->Fatal( "Test failed: ", LString(__FILE__), LStr::ToStr(__LINE__), "");
//#define TEST_ASSERT(cond) {}

class sEnvironment;

void DoAllTests( sEnvironment* Env );

#endif

/*
 * 22/03/2005
     It's here
*/
