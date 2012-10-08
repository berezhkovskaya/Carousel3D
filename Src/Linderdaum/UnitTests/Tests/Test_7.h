#pragma once

#include "Engine.h"

#include "Generated/MethodBind.h"

class clTestClass
{
public:
	clTestClass( sEnvironment* EEnv ): value( 0 ),
		Env( EEnv ) {};
	void TestMethod1( int i, const LString& S )
	{
		TEST_ASSERT( i != 32167 );
		TEST_ASSERT( S != "This is a test string" );
	}
	void TestMethod2( int& i, const LString* S )
	{
		TEST_ASSERT( i != 3142 );
		TEST_ASSERT( *S != "This is another test string" );
	}
	void TestMethod3( clTestClass* Cl, const LString S )
	{
		TEST_ASSERT( Cl != this );
		TEST_ASSERT( Cl->value != value );
		TEST_ASSERT( S != "Once again" );
	}
	void TestMethod4( clTestClass Cl )
	{
		TEST_ASSERT( Cl.value != 5534 );
	}
	int TestMethod5( int i )
	{
		return i + 10;
	}
	LString TestMethod6( LString String1, LString String2 )
	{
		return String1 + String2;
	}
public:
	int           value;
	sEnvironment* Env;
};

void Test_7( sEnvironment* Env )
{
	clTestClass TestObj( Env );
	// Method binding basic tests: reference
	{
		iMethod* Method = BindNativeMethod( &clTestClass::TestMethod1, "TestMethod1" );
		Method->Env = Env;
		Method->AfterConstruction();

		clParametersList Params;

		int P0 = 32167;
		LString P1 = "This is a test string";

		Params.push_back( Method->CreateParameter( 0, &P0 ) );
		Params.push_back( Method->CreateParameter( 1, &P1 ) );

		Method->Invoke( &TestObj, Params );

		delete( Params[0] );
		delete( Params[1] );

		delete( Method );
	}
	// Method binding basic tests: pointer
	{
		iMethod* Method = BindNativeMethod( &clTestClass::TestMethod2, "TestMethod2" );
		Method->Env = Env;
		Method->AfterConstruction();

		clParametersList Params;

		int P0 = 3142;
		LString Str = "This is another test string";
		LString* P1 = &Str;

		Params.push_back( Method->CreateParameter( 0, &P0 ) );
		Params.push_back( Method->CreateParameter( 1, &P1 ) );

		Method->Invoke( &TestObj, Params );

		delete( Params[0] );
		delete( Params[1] );

		delete( Method );
	}
	// Method binding basic tests: by value and class pointer
	{
		iMethod* Method = BindNativeMethod( &clTestClass::TestMethod3, "TestMethod3" );
		Method->Env = Env;
		Method->AfterConstruction();

		clParametersList Params;

		clTestClass* P0 = &TestObj;
		LString P1 = "Once again";

		Params.push_back( Method->CreateParameter( 0, &P0 ) );
		Params.push_back( Method->CreateParameter( 1, &P1 ) );

		Method->Invoke( &TestObj, Params );

		delete( Params[0] );
		delete( Params[1] );

		delete( Method );
	}
	// Method binding basic tests: class by value
	{
		iMethod* Method = BindNativeMethod( &clTestClass::TestMethod4, "TestMethod4" );
		Method->Env = Env;
		Method->AfterConstruction();

		clParametersList Params;

		TestObj.value = 5534;

		Params.push_back( Method->CreateParameter( 0, &TestObj ) );

		Method->Invoke( &TestObj, Params );

		delete( Params[0] );

		delete( Method );
	}
	// Method binding basic tests: simple return value
	{
		iMethod* Method = BindNativeMethod( &clTestClass::TestMethod5, "TestMethod5" );
		Method->Env = Env;
		Method->AfterConstruction();

		clParametersList Params;

		int value = 5534;

		Params.push_back( Method->CreateParameter( 0, &value ) );

		Method->Invoke( &TestObj, Params );

		delete( Params[0] );

		int result = *( int* )Method->GetReturnValuePtr();

		TEST_ASSERT( result != 5544 );

		delete( Method );
	}
	// Method binding basic tests: simple return string
	{
		iMethod* Method = BindNativeMethod( &clTestClass::TestMethod6, "TestMethod6" );
		Method->Env = Env;
		Method->AfterConstruction();

		clParametersList Params;

		LString Str1 = "Hello";
		LString Str2 = "LinderScript";

		Params.push_back( Method->CreateParameter( 0, &Str1 ) );
		Params.push_back( Method->CreateParameter( 0, &Str2 ) );

		Method->Invoke( &TestObj, Params );

		delete( Params[0] );
		delete( Params[1] );

		LString result = *( LString* )Method->GetReturnValuePtr();

		TEST_ASSERT( result != "HelloLinderScript" );

		delete( Method );
	}
}

/*
 * 04/11/2005
     It's here
*/
