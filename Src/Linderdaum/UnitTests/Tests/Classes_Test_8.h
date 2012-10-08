#pragma once

#include "Core/iObject.h"
#include "Core/Logger.h"

#include "UnitTests/Tests.h"

class clTestClient: public iObject
{
public:
	virtual void    AcceptString( const LString& Param ) const
	{
		TEST_ASSERT( Param != "Test string from LinderScript" );
	}
};

class iApplet: public iObject
{
public:
	iApplet() {};
};

class iStringsApplet: public iApplet
{
public:
	virtual LString    TestReturnString() = 0;
};

class iTestApplet: public iApplet
{
public:
	virtual int&          TestReturnIntRefByRef( int& P0 ) = 0;
	virtual int           TestReturnInt( int P0 ) = 0;
	virtual int&          TestReturnIntRef( int P0 ) = 0;
	virtual LString       TestReturnString( int P0 ) = 0;
	virtual LString&      TestReturnStringRef( int P0 ) = 0;
	virtual LString*      TestReturnStringPtr( int P0 ) = 0;
	virtual clTestClient* TestCreateClient() = 0;
	virtual void          TestSaySomething( clTestClient* Client ) = 0;
	virtual LString       TestReturnStringArg( const LString& Str1 ) = 0;

	virtual bool          TestString_ByValue( LString P0 ) = 0;
	virtual bool          TestString_ByRef( LString& P0 ) = 0;
	virtual bool          TestString_ByConstRef( const LString& P0 ) = 0;
	virtual bool          TestString_ByPtr( LString* P0 ) = 0;

	virtual LString       TestState() = 0;
};

/*
 * 23/11/2005
     It's here
*/
