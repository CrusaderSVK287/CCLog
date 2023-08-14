#include "cclog.h"
#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Log file option functions */
static FILE *log_file;

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

        if (log_file)
                fclose(log_file);

        log_file = fopen(path, "a");

        if (!log_file) {
                cclog_error("Failed to set option LOG_FILE");
                return -1;
        }

        return 0;
}

/* Default message format string */
static char *def_msg_string;

static char *get_def_msg_string()
{
        return def_msg_string;
}

static int set_def_msg_string(const char *value)
{
        if (!value)
                return -1;

        def_msg_string = (char *)value;
        cclog_debug("Setting new default msg_string: %s", value);

        return 0;
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

/*   option api functions    */

/* Function returns the value of option specified in opt */
void* get_opt(option_t opt)
{
        cclog_debug("called get_opt for option %d", opt);
        switch (opt) {
                case OPTIONS_LOG_FILE: return get_log_file();
                case OPTIONS_DEF_MSG_FORMAT: return get_def_msg_string();
                case OPTIONS_LAST_LOG_RET: return get_log_ret_value();
                default: return NULL;
        }
}

/* Function will set an options (opt) value (value) */
int set_opt(option_t opt, void *value)
{
        cclog_debug("called set_opt for option %d", opt);
        switch (opt) {
                case OPTIONS_LOG_FILE: return set_log_file((const char *)value);
                case OPTIONS_DEF_MSG_FORMAT: return set_def_msg_string((const char *)value);
                case OPTIONS_LAST_LOG_RET: return set_log_ret_value(*(int*)value);
                default: return -1;
        }
}

/* Function cleans up the options */
void cleanup_opt()
{
        if (log_file) {
                fclose(log_file);
                log_file = NULL;
        }
}
