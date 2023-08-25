#include <stdio.h>
#include <cclog.h>

int main()
{
	/* 
	 * Initialise the logger.
	 * We will be using a single file called my_log_file
	 * We dont need to specify proc_name since we are sure
	 * path isnt NULL
	 */
	cclogger_init(LOGGING_SINGLE_FILE, "./my_log_file", NULL);
	
	/* We write a message to the file only */
	cclog(CCLOG_LEVEL_MSG, NULL, "Lets do some logging");
	
	/* We write a message both to file and to terminal */
	if (2 + 2 != 10) {
		cclog(CCLOG_LEVEL_ERR, NULL, "Error, bad result");
	}

	/* Uninitialise logger */
	cclogger_uninit();

	return 0;
}

