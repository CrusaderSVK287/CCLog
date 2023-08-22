#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <stdio.h>

#ifdef CCLOG_DEBUG
#define cclog_debug(msg, ...) fprintf(stdout, "CCLOG_DEBUG_MSG: " msg "\n", ##  __VA_ARGS__);
#else
#define cclog_debug(msg , ...)
#endif /* CCLOG_DEBUG */

#ifdef CCLOG_NO_ERR
#define cclog_error(...) 
#else
#define cclog_error(msg, ...) \
    fprintf(stderr, "\e[0;31m***CCLOG ERROR*** : " msg "\n\e[0m", ## __VA_ARGS__);
#endif /* CCLOG_NO_ERR*/

/**
 * Enum containing all options that are available.
 * @OPTIONS_LOG_FILE: Getting this option will return a FILE* to the log file.
 *  Or NULL if the file is not opened. Setting thi value will close a log file
 *  if it is opened and open a new one on the path provided as a value
 */
typedef enum option {
    OPTIONS_LOGGER_INITIALISED,
    OPTIONS_LOG_FILE,
    OPTIONS_LOG_METHOD,
    OPTIONS_LOG_FILE_PATH,
    OPTIONS_DEF_MSG_FORMAT,
    OPTIONS_LAST_LOG_RET,
    OPTIONS_LAST_CALLBACK,
    OPTIONS_LOADED_FROM_JSON,
    OPTIONS_SERVER_ENABLED,
    OPTIONS_SERVER_PORT,
    OPTIONS_SERVER_PID,
} option_t;

/**
 * Function gets an option based on the opt argument (see enum option_t)
 * and return its value as a void pointer. See enum options to see which
 * option returns what to properly cast upon getting !
 * @opt: option of which value we want to get
 * @return: value of option as a void pointer.
 */
void* get_opt(option_t opt);

/**
 * Function sets and option value based on arguments (see enum option_t)
 * @opt: option to be set 
 * @value: value to be assigned to the option 
 * @return: 0 on success, -1 on failure
 */
int set_opt(option_t opt, void *value);

/**
 * Function cleans up everything that need to be from options (frees memory,
 * closes files etc)
 */
void cleanup_opt();

#endif // !__OPTIONS_H__

