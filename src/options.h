#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#ifdef CCLOG_DEBUG
#include <stdio.h>
#define cclog_debug(msg, ...) fprintf(stdout, "CCLOG_DEBUG_MSG: " msg, ##  __VA_ARGS__);
#else
#define cclog_debug(msg , ...)
#endif /* CCLOG_DEBUG */

/**
 * Enum containing all options that are available.
 * @OPTIONS_LOG_FILE: Getting this option will return a FILE* to the log file.
 *  Or NULL if the file is not opened. Setting thi value will close a log file
 *  if it is opened and open a new one on the path provided as a value
 */
typedef enum option {
    OPTIONS_LOG_FILE
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

