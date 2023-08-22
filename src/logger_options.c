#include "cclog.h"
#include "options.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* logger initialised */
static int logger_initialised = 0;

static int *get_logger_initialised() { return &logger_initialised;}
static int set_logger_initialised(int value) {
        logger_initialised = value;
        return 0;
}

/* Log file option functions */
static FILE *log_file = NULL;
static char *log_file_path = NULL;

/* return the pointer to the file struct pointer */
static FILE *get_log_file()
{
        return (log_file) ? log_file : NULL;
}

/* open new log file specified in path */
static int set_log_file(const char *path)
{
        cclog_debug("Args: path = %s", path);
        if (!path)
                return -1;

        log_file_path = strdup(path);

        if (log_file)
                fclose(log_file);

        log_file = fopen(path, "a");

        if (!log_file) {
                cclog_error("Failed to set option LOG_FILE");
                return -1;
        }

        return 0;
}

static char *get_log_file_path() { return (char*)log_file_path ;}
static int set_log_file_path(void *path) { return -1 ;}

/* Log method */

static int log_method = 0;
static int *get_log_method() { return &log_method;}
static int set_log_method(int value) {
        log_method = value;
        return 0;
}

/* Default message format string */
static char *def_msg_string = NULL;

static char *get_def_msg_string()
{
        return def_msg_string;
}

static int set_def_msg_string(const char *value)
{
        if (!value)
                return -1;

        if (def_msg_string) {
                free(def_msg_string);
                def_msg_string = NULL;
        }

        def_msg_string = strdup(value);
        cclog_debug("Setting new default msg_string: %s", value);

        return (def_msg_string)? 0 : -1;
}

/* 
 * Last cclog() return value, usefull for example for getting return value 
 * of the callback function outside of if statement
 */

static int log_ret_value = 0;

static int *get_log_ret_value() { return &log_ret_value; }
static int set_log_ret_value(int value) {
        log_ret_value = value;
        return 0;
}

/* Last callback option*/
cclog_cb last_callback = NULL;

static cclog_cb get_last_callback() { return last_callback; }
static int set_last_callback(cclog_cb cb) {
        last_callback = cb;
        return 0;
}

/* loaded from json */

static int loaded_from_json = 0;

static int *get_loaded_from_json() { return &loaded_from_json; }
static int set_loaded_from_json(int value) {
        loaded_from_json = value;
        return 0;
}

/* Server related options */
static int server_enabled = 0;
static int server_port = 0;
static int server_pid = 0;

static int *get_server_enabled() { return &server_enabled; }
static int set_server_enabled(int value) {
        server_enabled = value;
        return 0;
}
static int *get_server_port() { return &server_port; }
static int set_server_port(int value) {
        server_port = value;
        return 0;
}
static int *get_server_pid() { return &server_pid; }
static int set_server_pid(int value) {
        server_pid = value;
        return 0;
}

/*   option api functions    */

/* Function returns the value of option specified in opt */
void* get_opt(option_t opt)
{
        cclog_debug("called get_opt for option %d", opt);
        switch (opt) {
                case OPTIONS_LOGGER_INITIALISED: return get_logger_initialised();
                case OPTIONS_LOG_FILE: return get_log_file();
                case OPTIONS_LOG_METHOD: return get_log_method();
                case OPTIONS_LOG_FILE_PATH: return get_log_file_path();
                case OPTIONS_DEF_MSG_FORMAT: return get_def_msg_string();
                case OPTIONS_LAST_LOG_RET: return get_log_ret_value();
                case OPTIONS_LAST_CALLBACK: return get_last_callback();
                case OPTIONS_LOADED_FROM_JSON: return get_loaded_from_json();
                case OPTIONS_SERVER_ENABLED: return get_server_enabled();
                case OPTIONS_SERVER_PORT: return get_server_port();
                case OPTIONS_SERVER_PID: return get_server_pid();
                default: return NULL;
        }
}

/* Function will set an options (opt) value (value) */
int set_opt(option_t opt, void *value)
{
        cclog_debug("called set_opt for option %d", opt);
        switch (opt) {
                case OPTIONS_LOGGER_INITIALISED: return set_logger_initialised(*(int*)value);
                case OPTIONS_LOG_FILE: return set_log_file((const char *)value);
                case OPTIONS_LOG_METHOD: return set_log_method(*(int*)value);
                case OPTIONS_LOG_FILE_PATH: return set_log_file_path(value);
                case OPTIONS_DEF_MSG_FORMAT: return set_def_msg_string((const char *)value);
                case OPTIONS_LAST_LOG_RET: return set_log_ret_value(*(int*)value);
                case OPTIONS_LAST_CALLBACK: return set_last_callback((cclog_cb)value);
                case OPTIONS_LOADED_FROM_JSON: return set_loaded_from_json(*(int*)value);
                case OPTIONS_SERVER_ENABLED: return set_server_enabled(*(int*)value);
                case OPTIONS_SERVER_PORT: return set_server_port(*(int*)value);
                case OPTIONS_SERVER_PID: return set_server_pid(*(int*)value);
                default: return -1;
        }
}

/* Function cleans up the options */
void cleanup_opt()
{
        cclog_debug("Cleaning options");
        if (log_file) {
                fclose(log_file);
                log_file = NULL;
        }

        if (log_file_path) {
                free(log_file_path);
                log_file_path = NULL;
        }

        if (def_msg_string) {
                free(def_msg_string);
                def_msg_string = NULL;
        }
}

