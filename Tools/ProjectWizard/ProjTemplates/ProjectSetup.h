/**
 * \file ProjectSetup.h
 * \brief LSDC core package configuration file for <!PROJECT_NAME!>
 * \version 1.0.00
 * \date <!CURRENT_DATE!>
 */

#ifdef LSDC_PACKAGE_SETUP

PACKAGE_NAME( "<!PROJECT_NAME!>" )

PACKAGE_NET_NAME( "<!PROJECT_NAME!>.NET" )

PACKAGE_CPP_NAME( "<!PROJECT_NAME!>" )
PACKAGE_CPP_PREFIX( "_<!PROJECT_NAME!>" )

PACKAGE_CUSTOM_INCLUDE( "GameExport_Includes.h" )

PACKAGE_OUTPUT_DIRECTORY( "Out/LSDC_<!PROJECT_NAME!>" )

GENERATE_TUNNELLERS( true )
GENERATE_EXPORTS( true )
GENERATE_SCRIPT_EXPORTS( true )
GENERATE_SERIALIZATION( true )

GENERATE_NET_EXPORT( true )

// Generate exports for enumerations
GENERATE_ENUM_EXPORTS( true )

// Generate const export
GENERATE_CONST_EXPORTS( true )

#endif

/*
 * <!CURRENT_DATE!>
     It's here
*/
