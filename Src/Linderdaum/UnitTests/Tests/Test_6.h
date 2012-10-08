#pragma once

#include "Engine.h"
/*
#define DECLARE_TEST_VISITOR(ReferenceValue) class clTestVisitor: public iTestVisitor \
      { \
      public: \
         virtual void    AcceptParameter(const LString& ParamName, const LString& ParamValue) \
         { \
            TEST_ASSERT( ReferenceValue != ParamValue ); \
         } \
      } TestVisitor; \
      Param.AcceptVisitor( &TestVisitor );
*/
void Test_6( sEnvironment* Env )
{
	/*
	   class iTestVisitor: public iParametrizable
	   {
	   public:
	      virtual void    BeginSection(const LString& SectionName, const LString& SectionParam) {};
	      virtual void    EndSection(const LString& SectionName) {};
	   };

	   // XLML builder testing: parameters
	   {
	      clXLMLParameter Param("TempParam", "1");

	      DECLARE_TEST_VISITOR( "1" );
	   }
	   {
	      clXLMLParameter Param("TempParam", " 1 ");

	      DECLARE_TEST_VISITOR( "1" );
	   }
	   {
	      clXLMLParameter Param("TempParam", "\" 1 \"");

	      DECLARE_TEST_VISITOR( " 1 " );      // note the difference
	   }
	   // XLML builder testing: sections
	   {
	      clXLMLSection Param("TempSection", "1");

	      DECLARE_TEST_VISITOR( "1" );
	   }
	   {
	      clXLMLSection Param("TempSection", " 1 ");

	      DECLARE_TEST_VISITOR( "1" );
	   }
	   {
	      clXLMLSection Param("TempSection", "\" 1 \"");

	      DECLARE_TEST_VISITOR( "1" );        // note the difference
	   }
	*/
}

/*
 * 24/05/2005
     It's here
*/
