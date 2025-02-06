#ifndef __CCLOG_HPP__
#define __CCLOG_HPP__

#include <string>
#define __CCLOG_VERSION__ "v1.3.0"
#define __CCLOG_CXX_VERSION__ "v1.0.0"

namespace cclog {

extern "C" {
#include <cclog.h>
}

/***********************************************************************
 * This is a C++ wrapper for originaly C library.                      *
 * This wrapper provides bindings to the C functions                   *
 * that are more idiomatic torwards C++.                               *
 * typedefs and enums are accessible through the cclog namespace.      *
 * For documentation regarding those, look inside the cclog.h original *
 * file. Function mappings will have documentation above them          *
 ***********************************************************************/ 
/**
 * Function initializes logger by opening the required log file.
 * This function must be called before any other functions are used.
 * @type: Type of logging. Single/Multi file. See logging_type enum
 * @path: path to the file that is going to be used as a log file.
 *      Dont append ".log" at the end of path, it is done automatically
 *      Lets assume path is example/path/filename
 *      If logging type is single file:
 *          file will be opened as example/path/filename.log
 *      If logging type is multi files:
 *          file will be opened as example/path/filename-[YYYY-MM-DD_HH:MM:SS].log
 * @proc_name: Name of the process. If path is not specified (is null), 
 *      the log file will be created in home directory with the proc_name as filename. 
 *      This is meant to be a failsafe if path is null but can be used anyway.
 * @return: 0 on success, -1 on failure
 *
 * Note: Eight path or proc_name MUST not be null
 */
inline int init(logging_type_t type, std::string path, std::string proc_name) {
    return cclogger_init(type, path.c_str(), proc_name.c_str());
}

/**
 * Function deinits the logger, put this at the end of your program
 * @return: 0 on success, -1 on failure
 */
inline int uninit() {
    return cclogger_uninit();
}

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
inline int log(int level, void* priv_data, std::string message) {
    return cclog(level, priv_data, message.c_str());
}
inline int log(int level, std::string message) {
    return cclog(level, nullptr, message.c_str());
}
#ifdef cclog 
#undef cclog
#endif

/**
 * WARNING FOR C++ USERS: do not put "nullptr" as a message format override. 
 * Use the overloaded functions instead
 *
 * Function adds a custom log level to the list of log levels user can use. 
 * @log_to_file: boolean determining whether the message is going to be logged to file 
 * @log_to_tty: boolean determining whether the message will be displayed to the stderr
 * @color: enum indicating which color will be used to display the message
 *      to stderr. Has no effect if log_to_tty is false 
 * @callback: callback function called at the end of log function. Wont be called if NULL
 * @msg_format_override: If specified, will override the default message format. 
 *      See cclogger_set_default_message_format for further explanation.
 *      Specify NULL to not override.
 * @verbosity_level: Verbosity level of this log level. If the verbosity is 
 *      higher than the logger verbosity, the log will not be made, nor any 
 *      callbacks will be called.
 */

inline int add_log_level(bool log_to_file, bool log_to_tty,
        cclog_tty_log_color_t color, int verbosity_level) {
    return cclogger_add_log_level(log_to_file, log_to_tty, color, nullptr, 
         nullptr, verbosity_level);
}

inline int add_log_level(bool log_to_file, bool log_to_tty,
        cclog_tty_log_color_t color, int verbosity_level,
        std::string message_format_override) {
    return cclogger_add_log_level(log_to_file, log_to_tty, color, nullptr, 
            message_format_override.c_str(), verbosity_level);
}

inline int add_log_level(bool log_to_file, bool log_to_tty,
        cclog_tty_log_color_t color, int verbosity_level,
        cclog_callback_mapping_t *cbm) {
    return cclogger_add_log_level(log_to_file, log_to_tty, color, cbm, 
         nullptr, verbosity_level);
}

inline int add_log_level(bool log_to_file, bool log_to_tty,
        cclog_tty_log_color_t color, int verbosity_level,
        cclog_callback_mapping_t *cbm, std::string message_format_override) {
    return cclogger_add_log_level(log_to_file, log_to_tty, color, cbm, 
         message_format_override.c_str(), verbosity_level);
}
/**
 * Function removes all log levels, including default ones.
 * After invoking this function, user MUST add at least one custom log level 
 * to be able to use the logger. See cclogger_add_log_level() 
 */
inline void reset_log_levels() {
    cclogger_reset_log_levels();
}

/**
 * Warning for C++ wrapper: variables utilizing __LINE__ and __FILE__ macros wont work, dont use them.
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
 *      MSG - The message specified by user
 *      FILE - File in which the log was called
 *      LINE - Line of code on which the log was called
 *      FUNCTION - Name of function in which the log was called
 *      DATE - Current date in YYYY-MM-DD format
 *      TIME - Current time in HH:MM:SS format
 *      PID - pid of the running process
 *      ERRNO - error number as a digit
 *      ERMSG - error message indicated by errno, got from strerror()
 *      UPTIME - time elapsed since initialisation of the logger
 *      YYYY - Current year
 *      MM - Current month (1 to 12)
 *      DD - Current day (1 to 31)
 *      HOUR - Current hour 
 *      MIN - Current minute
 *      SEC - Current second
 * Note: All time and dates are local time
 */
inline int set_default_message_format(std::string format) {
    return cclogger_set_default_message_format(format.c_str());
}

/**
 * Returns the returned value of the last cclog() function. 
 * Usefull for example for getting callback return value 
 */
inline int last_log_retval() {
    return cclogger_last_log_return_value();
}

/**
 * Sets verbosity level for logger. Each log level has a verbosity assigned 
 * to it. If the log level used has higher verbosity level than is the verbosity 
 * level for logger, the log will not be made, nor any callbacks will be called.
 * Default verbosity level for logger is 10 and all default log levels have 
 * default verbosity level set to 5
 */
inline void set_verbosity_level(int verbosity) {
    cclogger_set_verbosity_level(verbosity);
}

// CALLBACKS NOT YET FIXED FOR C++ WRAPPER
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
//inline int recall_callback(void *priv) {
//    return cclogger_recall_last_callback(priv);
//}

/**
 * Function saves current cclogger config into a file provided in path. 
 * This file is stored in json format.
 */
inline int export_config(std::string path) {
    return cclogger_export_config_json(path.c_str());
}

/**
 * Function load a configuration of logger from json file provided in path. 
 * Function also requires a table of callback mappings if one exists.
 * If not, it can be specified as NULL.
 * See cclog_callback_mapping_t struct for more information how to declare 
 * sucj map.
 * Be advised that this function erases the current configuration.
 */
inline int load_config(std::string path , cclog_callback_mapping_t cb_mappings[]) {
    return cclogger_load_config_json(path.c_str(), cb_mappings);
}

/**
 * Function starts a http server on the given port. This server can provide 
 * current configuration and currently opened log file.
 * Mappings:
 *      /       - index page
 *      /config - json containing configuration 
 *      /log    - currently opened log file
 * @port: number of port for socket to be binded to 
 * @return: server PID on success, -1 on error
 */
inline int server_start(int port) {
    return cclogger_server_start(port);
}

/**
 * Stops the http server if it is started
 */
inline int server_stop() {
    return cclogger_server_stop();
}

/**
 * Returns the PID of the server process 
 */
inline int server_pid() {
    return cclogger_server_pid();
}

} //namespace cclog

#endif // !__CCLOG_HPP__

