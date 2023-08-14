#ifndef __CCLOG_H__
#define __CCLOG_H__

#include <stdbool.h>

/* convenience macros */
#define if_failed(exp, label) {if (exp != 0) {goto label;}}

/** 
 * Log callback function pointer typedef. Declare such function like this:
 * int name_of_cb_func(const char* msg, void *priv)
 * where msg is the generated log message and priv is any data user passed to 
 * the log function (se cclog() )
 */
typedef int (*cclog_cb)(const char* msg, void* priv);

typedef enum cclog_default_log_levels {
    CCLOG_LEVEL_INFO,   /* Print to stderr in default color */
    CCLOG_LEVEL_MSG,    /* Print to file only */
    CCLOG_LEVEL_ERR,    /* Print to file and to stderr in red color */
} cclog_default_log_levels_t;

/* enum containing terminal colors that can be used to display message to tty */
typedef enum cclog_tty_log_color {
    CCLOG_TTY_CLR_DEF,  /* Default - use the default color of tty */
    CCLOG_TTY_CLR_BLK,  /* Black */
    CCLOG_TTY_CLR_RED,  /* Red */
    CCLOG_TTY_CLR_GRN,  /* Green */
    CCLOG_TTY_CLR_YEL,  /* Yellow */
    CCLOG_TTY_CLR_BLU,  /* Blue */
    CCLOG_TTY_CLR_MAG,  /* Magenta */
    CCLOG_TTY_CLR_CYN,  /* Cyan */
    CCLOG_TTY_CLR_WHT,  /* White */
} cclog_tty_log_color_t;

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
 * This function must be called before any other functions are used.
 * @type: Type of logging. Single/Multi file. See logging_type enum
 * @return: 0 on success, -1 on failure
 */
int cclogger_init(logging_type_t type, const char *path, const char **argv);

/**
 * Function deinits the logger, put this at the end of your program
 * @return: 0 on success, -1 on failure
 */
int cclogger_uninit();

/* DO NOT CALL DIRECTLY, USE cclog() MACRO */
int _cclogger_log(int line, const char* file, const char *func,
        int level, void* priv, const char *msg, ...);

#ifdef _cclogger_log
#undef _cclogger_log
#endif /* ifdef _cclogger_log */

/**
 * Function performs logging operations specified in the log_level, e.g. writes 
 * message to file and/or to tty. If callback function is specified in the log level,
 * calls it at the end.
 * @return If no callback is called, returns 0 on success, -1 on failure.
 *      If a callback is specified, returns whatever is returned by the callback
 *
 * @log_level: Defines where and how a message should be logged. Refer to the
 *      cclogger_add_log_level() function for explanation of the parameters.
 *      By default there are 3 levels indexed from 0 to 2 (see log_level enum)
 *          LOG_LEVEL_INFO  - prints to stderr in default tty color
 *          LOG_LEVEL_MSG   - prints message to file only
 *          LOG_LEVEL_ERR   - prints message to file and to stderr in red color
 *      These levels have indexes 0, 1, 2 respectively. Therfore any other log
 *      levels user defines will start at index 3, 4, ...
 *      Alternatively, you can remove all of the default levels by calling
 *      cclogger_reset_log_levels(). After that you must create at least one custom
 *      log level. Since there are no levels, this custom level will now start
 *      at index 0
 *
 *  @priv_data: this is a void pointer. User can send a pointer to any data to 
 *      the log function, this data will not be touched in the function itself.
 *      Rather, it will be passed as priv parameter to the callback function 
 *      called at the end, if user specified it in the log level.
 *      NOTE: none of the default log levels call any callback function.
 *      
 *  @msg: Users log message, this is a typical formated string, used the same
 *      way as with printf for example, after this argument, the variable
 *      arguments for the formated string are passed
 */
#define cclog(log_level, priv_data, msg, ...) (_cclogger_log)(__LINE__, __FILE__, \
        __FUNCTION__, log_level, priv_data, msg, ## __VA_ARGS__)

/* to prevent usage of the function itself */
#define _cclogger_log(...) DO_NOT_CALL_DIRECTLY_USE_cclog_MACRO

/**
 * Function adds a custom log level to the list of log levels user can use. 
 * @log_to_file: boolean determining whether the message is going to be logged to file 
 * @log_to_tty: boolean determining whether the message will be displayed to the stderr
 * @color: enum indicating which color will be used to display the message
 *      to stderr. Has no effect if log_to_tty is false 
 * @callback: callback function called at the end of log function. Wont be called if NULL
 * @msg_format: If specified, will override the default message format. 
 *      See cclogger_set_default_message_format for further explanation.
 *      Specify NULL to not override
 */
int cclogger_add_log_level(bool log_to_file, bool log_to_tty,
        cclog_tty_log_color_t color, cclog_cb callback, 
        const char *msg_format_override);

/**
 * Function removes all log levels, including default ones.
 * After invoking this function, user MUST add at least one custom log level 
 * to be able to use the logger. See cclogger_add_log_level() 
 */
void cclogger_reset_log_levels();

/**
 * Function sets default message format for log messages. This format string 
 * looks like this: "[${DATE}_${TIME}]${FILE}:${LINE}:${MSG}"
 * This string dictates how log messages will look like. It contains variables
 * inside ${} blocks that you can use. Instead of built-in variables you can 
 * also use an environment variable (eighter already defined or exported)
 * Syntax for this string is really simple. Every character except ${} blocks
 * is inluded in the result log message and variables inside ${} blocks are 
 * inserted. Example:
 *      format: "( ${DATE} ) ${TIME} : ${MSG}"
 *      result: "( 2023-14-8 ) 9:17:23 : log message specified in cclog"
 * List of build in variables:
 *      FILE - File in which the log was called
 *      LINE - Line of code on which the log was called
 *      FUNCTION - Name of function in which the log was called
 *      DATE - Current date in YYYY-MM-DD format
 *      TIME - Current time in HH:MM:SS format
 *      YYYY - Current year
 *      MM - Current month (1 to 12)
 *      DD - Current day (1 to 31)
 *      HOUR - Current hour 
 *      MIN - Current minute
 *      SEC - Current second
 * Note: All time and dates are local time
 */
int cclogger_set_default_message_format(const char *str);

/**
 * Returns the returned value of the last cclog() function. 
 * Usefull for example for getting callback return value 
 */
int cclogger_last_log_return_value();

/**
 * Calls the callback function from the last log event with priv data.
 * NOTE: msg will be empty string instead of generated message
 * The last callback is also updated if the log level doesnt have a callback 
 * specified, in this case, the function will just not call anything and
 * return -1 immediately
 *
 * @priv: private data that will be passed into the callback 
 * @return: -1 if the last callback is NULL, otherwise returns whatever
 *      the callback returns
 */
int cclogger_recall_last_callback(void *priv);

#endif /* __CCLOG_H__ */

