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

        cclogger_set_default_message_format("(${TIME}) ${FILE} : ${MSG}");

        cclog(CCLOG_LEVEL_INFO, NULL, "This is my message");

	/* Uninitialise logger */
	cclogger_uninit();

	return 0;
}

