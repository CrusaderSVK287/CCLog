#include "cclog.h"
#include "options.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "utils/utils.h"

/* handler function for one file logging type */
static int open_single_file(const char *path, const char **argv)
{
        char buff[BUFSIZ];
        char *home_path;
        
        /* If path is not specified, create a default log file at home */
        if (!path) {
                home_path = getenv("HOME");
                sprintf(buff, "%s/%s.log", home_path, strrchr(argv[0], '/')+1);
        } else { 
                /* If path is specified, open the log file at the path */
                sprintf(buff, "%s.log", path);
        }

        /* Set the log file option */
        cclog_debug("Opening log file: %s", buff);
        if_failed(set_opt(OPTIONS_LOG_FILE, buff), error);
        
        return 0;
error:
        return -1;

}

/* handler funcion for multiple files logging type*/
static int open_multiple_file_mode(const char *path, const char **argv)
{
        char buff[BUFSIZ];
        char *home_path;
        char time_buf[128];

        /* Get current time to prepend to filename */
        struct tm *timeinfo = util_current_timeinfo();

        sprintf(time_buf ,"[%04d-%02d-%02d_%02d:%02d:%02d]", 
                timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, 
                timeinfo->tm_mday, 
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

        /* If path is not specified, create a default log file at home */
        if (!path) {
                home_path = getenv("HOME");
                sprintf(buff, "%s/%s-%s.log", home_path, strrchr(argv[0], '/')+1, time_buf);
        } else {
                /* If it is, create a file in the path*/
                sprintf(buff, "%s-%s.log", path, time_buf);
        }

        /* Set the log file option */
        cclog_debug("Opening log file: %s\n", buff);
        if_failed(set_opt(OPTIONS_LOG_FILE, buff), error);
       
        return 0; 
error:
        cclog_debug("Failed opening log file\n");
        return -1;
}

static int add_default_log_levels()
{
        /* Info - print msg to tty only */
        if_failed(cclogger_add_log_level(false, true, CCLOG_TTY_CLR_DEF, NULL), error);
         /* Msg - print to file only */
        if_failed(cclogger_add_log_level(true, false, CCLOG_TTY_CLR_DEF, NULL), error);
        /* Err - print to file and to tty in red*/
        if_failed(cclogger_add_log_level(true, false, CCLOG_TTY_CLR_RED, NULL), error);  
        return 0;
error:
        cclog_error("Failed to initialise default log_levels");
        return -1;
}

/* cclogger API functions */

int cclogger_init(logging_type_t type, const char* path, const char **argv)
{
        /* Add default log levels */
        if_failed(add_default_log_levels(), error);

        /* If all other init functions passed, open/crete a log file */
        if (type == LOGGING_SINGLE_FILE) {
                open_single_file(path, argv);
        }
        else if (type == LOGGING_MULTIPLE_FILES) {
                open_multiple_file_mode(path, argv);
        }
        else {
                cclog_error("Unknown logging type specified");
                goto error;
        }

        /* Check whether file was opened */
        if (get_opt(OPTIONS_LOG_FILE) == NULL) {
                cclog_error("File could not be opened");
                goto error;
        }

        return 0;
error:
        cclog_error("CCLogger failed to be initialised");
        return -1;
}

int cclogger_uninit()
{
        cleanup_opt();
        cclogger_reset_log_levels();
        return 0;
}

