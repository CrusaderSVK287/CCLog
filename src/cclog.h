#ifndef __CCLOG_H__
#define __CCLOG_H__

#define if_failed(exp, label) {if (exp != 0) {goto label;}}

/**
 * Enum indicating which type of logging is going to be used
 * @LOGGING_SINGLE_FILE: All logs are going into a single file
 * @LOGGING_MULTIPLE_FILES: Logs are separated to multiple files which are
 * prepended with the date and time of start of the program
 */
typedef enum logging_type {
    LOGGING_SINGLE_FILE,
    LOGGING_MULTIPLE_FILES
} logging_type_t;

/**
 * Function initializes logger by opening the required log file.
 * This function must be called before any logging is done.
 * @type: Type of logging. Single/Multi file. See logging_type enum
 * @return: 0 on success, -1 on failure
 */
int cclogger_init(logging_type_t type, const char *path, const char **argv);

/**
 * Function deinits the logger, put this at the end of your program
 * @return: 0 on success, -1 on failure
 */
int cclogger_uninit();

#endif /* __CCLOG_H__ */

