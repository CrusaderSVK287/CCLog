#include "cclog.h"
#include "options.h"
#include "json.h"
#include "logger.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

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

        if (log_file_path)
                free(log_file_path);
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

/* init time */
static struct timeval init_time;
static struct timeval *get_init_time() {return &init_time;}
static int set_init_time() {
        gettimeofday(&init_time, NULL);
        return 0;
}

/* log verbosity level */
static int verbosity_level = 0;

static int *get_verbosity_level() { return &verbosity_level; }
static int set_verbosity_level(int value) {
        verbosity_level = value;
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
                case OPTIONS_INIT_TIME: return get_init_time();
                case OPTIONS_VERBOSITY: return get_verbosity_level();
                default: return NULL;
        }
}

/* Function will set an options (opt) value (value) */
int set_opt(option_t opt, void *value)
{
        cclog_debug("called set_opt for option %d", opt);
        int ret = 0;

        switch (opt) {
                case OPTIONS_LOGGER_INITIALISED: ret = set_logger_initialised(*(int*)value); break;
                case OPTIONS_LOG_FILE: ret = set_log_file((const char *)value); break;
                case OPTIONS_LOG_METHOD: ret = set_log_method(*(int*)value); break;
                case OPTIONS_LOG_FILE_PATH: ret = set_log_file_path(value); break;
                case OPTIONS_DEF_MSG_FORMAT: ret = set_def_msg_string((const char *)value); break;
                case OPTIONS_LAST_LOG_RET: ret = set_log_ret_value(*(int*)value); break;
                case OPTIONS_LAST_CALLBACK: ret = set_last_callback((cclog_cb)value); break;
                case OPTIONS_LOADED_FROM_JSON: ret = set_loaded_from_json(*(int*)value); break;
                case OPTIONS_SERVER_ENABLED: ret = set_server_enabled(*(int*)value); break;
                case OPTIONS_SERVER_PORT: ret = set_server_port(*(int*)value); break;
                case OPTIONS_SERVER_PID: ret = set_server_pid(*(int*)value); break;
                case OPTIONS_INIT_TIME: ret = set_init_time(); break;
                case OPTIONS_VERBOSITY: ret = set_verbosity_level(*(int*)value); break;
                default: return -1;
        }

        /* This is needed for server to always have access to most recent config */
        if (server_enabled)
                json_load_current_config();

        return ret;
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

int is_initialised() {return logger_initialised;}
int is_server_enabled() {return server_enabled;}

