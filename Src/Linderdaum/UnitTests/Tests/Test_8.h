#pragma once

#include "Engine.h"

#include "Classes_Test_8.h"

int __TestFib( int n )
{
	if ( n < 2 )
	{
		return 1;
	}

	return __TestFib( n - 2 ) + __TestFib( n - 1 );
}

void Test_8( sEnvironment* Env )
{
	{
		Env->Linker->LoadStaticMethod( "Tests/static.method" );

		iMethod* Method = Env->Linker->FindStaticMethod( "TestFibonacci" );

		Env->Logger->Log( L_DEBUG, "loaded, trying to invoke" );

		for ( int N = 0; N != 11; ++N )
		{
			iParameter* Param = Method->CreateParameter( 0, &N );

			clParametersList Params;

			Params.push_back( Param );

			Method->Invoke( 0, Params );

			int Result;

			memcpy( &Result, Method->GetReturnValuePtr(), 4 );

			TEST_ASSERT( Result != __TestFib( N ) );

			delete( Param );
		}
	}
	/*
	   {
	      Environment->Linker->LoadStaticClass( "Tests\\clTestApplet.staticclass" );

	      iTestApplet* TestApplet = Environment->Linker->Instantiate<iTestApplet>( "clTestApplet" );

	      TEST_ASSERT( TestApplet->ClassName() != "clTestApplet" );
	      TEST_ASSERT( TestApplet->NativeClassName() != "iTestApplet_Tunneler" );

	      int TestIntRef = 123;

	      TEST_ASSERT( TestApplet->TestReturnIntRefByRef( TestIntRef ) != 100 );
	      TEST_ASSERT( TestApplet->TestReturnInt( 10 ) != 30 );
	      TEST_ASSERT( TestApplet->TestReturnIntRef( 55 ) != 85 );

	      TEST_ASSERT( TestApplet->TestReturnString( 1 ) != "This value is returned from LinderScript" );
	      TEST_ASSERT( TestApplet->TestReturnStringRef( 1 ) != "This value is returned from LinderScript by reference" );
	      TEST_ASSERT( *(TestApplet->TestReturnStringPtr( 1 ) ) != "This value is returned from LinderScript by pointer" );

	      clTestClient* TestClient = dynamic_cast<clTestClient*>( TestApplet->TestCreateClient() );

	      TEST_ASSERT( TestClient == NULL );

	      TestApplet->TestSaySomething( TestClient );

	      TEST_ASSERT( !TestApplet->TestString_ByValue( "Hello LinderScript by value" ) );

	      LString StringRef = "Hello LinderScript by reference";

	      TEST_ASSERT( !TestApplet->TestString_ByRef( StringRef ) );
	      TEST_ASSERT( !TestApplet->TestString_ByConstRef( "Hello LinderScript by const reference" ) );

	      LString StringPtr = "Hello LinderScript by pointer";

	      TEST_ASSERT( !TestApplet->TestString_ByPtr( &StringPtr ) );

	      // states
	      TEST_ASSERT( TestApplet->TestState() != "In default state" );

	      TestApplet->SetActiveState( "State1" );

	      TEST_ASSERT( TestApplet->TestState() != "In State1 state" );

	      TestApplet->SetActiveState( "State2" );

	      TEST_ASSERT( TestApplet->TestState() != "In State2 state" );

	      TestApplet->SetActiveState( "State3" );

	      TEST_ASSERT( TestApplet->TestState() != "In State3 state" );

	      TestApplet->SetActiveState( "" );

	      TEST_ASSERT( TestApplet->TestState() != "In default state" );

	      delete(TestClient);
	      delete(TestApplet);
	   }
	*/
}

/*
 * 20/12/2005
     Added test case for states
 * 23/11/2005
     It's here
*/
