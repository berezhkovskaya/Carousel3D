/**
 * \file <!MAIN_CPP_FILE_NAME!>
 * \brief <!PROJECT_NAME!>
 * \version 1.0.00
 * \date <!CURRENT_DATE!>
 */

#include "Linderdaum.h"

sEnvironment* Env = NULL;
<!EVENT_HANDLERS!>
APPLICATION_ENTRY_POINT
{
	std::vector<LString> CommandLine;

	EXTRACT_COMMAND_LINE(CommandLine);

	Env = new sEnvironment();

	Env->DeployDefaultEnvironment( &CommandLine, "<!COMMON_MEDIA_FOLDER_NAME!>" );
	<!MOUNT_GAMEDATA!>
	<!REG_EVENT_HANDLERS!>
	Env->RunApplication( DEFAULT_CONSOLE_AUTOEXEC );

	APPLICATION_EXIT_POINT( Env );
}

APPLICATION_SHUTDOWN
{
}

/*
 * <!CURRENT_DATE!>
     Created with Linderdaum ProjectWizard
*/
