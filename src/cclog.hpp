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

} //namespace cclog

#endif // !__CCLOG_HPP__

