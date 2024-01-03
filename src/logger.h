#ifndef __CCLOG_LOGGER_H_
#define __CCLOG_LOGGER_H_

#include <stdbool.h>
#include "cclog.h"

/**
 * Log level struct containing information about defined log levels 
 * @log_to_file: boolean determining whether the message is going to be logged to file 
 * @log_to_tty: boolean determining whether the message will be displayed to the stderr
 * @color: enum indicating which color will be used to display the message
 *      to stderr. Has no effect if log_to_tty is false 
 * @callback: callback function called at the end of log function. Wont be called if NULL
 * @cb_name: string name of callback 
 * @msg_format: message format to override the default message format
 * @verbosity: verbosity level of the log level. Check API function for more information
 */
typedef struct log_level {
    bool log_to_file;
    bool log_to_tty;
    cclog_tty_log_color_t color;
    cclog_cb callback;
    const char *cb_name;
    const char *msg_format;
    int verbosity;
} cclog_log_level_t;

/**
 * Helper function. Loads current config in json format to json buffer. 
 * Buffer must be first inited and than freed 
 */
void cclog_json_load_current_config();

#endif // !__CCLOG_LOGGER_H_add_default_log_levels
