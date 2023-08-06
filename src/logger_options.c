#include "cclog.h"
#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Log file option functions */
static FILE *log_file;

/* return the pointer to the file struct pointer */
static FILE *log_file_get()
{
        return (log_file) ? log_file : NULL;
}

/* open new log file specified in path */
static int log_file_set(const char *path)
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


/*   option api functions    */

/* Function returns the value of option specified in opt */
void* get_opt(option_t opt)
{
        cclog_debug("called get_opt for option %d", opt);
        switch (opt) {
                case OPTIONS_LOG_FILE: return log_file_get();
                default: return NULL;
        }
}

/* Function will set an options (opt) value (value) */
int set_opt(option_t opt, void *value)
{
        cclog_debug("called set_opt for option %d", opt);
        switch (opt) {
                case OPTIONS_LOG_FILE: return log_file_set((const char *)value);
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
