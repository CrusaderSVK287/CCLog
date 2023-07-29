#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define opt_error(msg) fprintf(stderr, "\e[0;31m" msg " %s\e[0m\n", strerror(errno))

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
        if (!path)
                return -1;

        if (log_file)
                fclose(log_file);

        log_file = fopen(path, "a");

        if (!log_file) {
                opt_error("Failed to set option LOG_FILE");
                return -1;
        }

        return 0;
}


/*   option api functions    */

/* Function returns the value of option specified in opt */
void* get_opt(option_t opt)
{
        switch (opt) {
                case OPTIONS_LOG_FILE: return log_file_get();
                default: return NULL;
        }
}

/* Function will set an options (opt) value (value) */
int set_opt(option_t opt, void *value)
{
        switch (opt) {
                case OPTIONS_LOG_FILE: return log_file_set((const char *)value);
                default: return -1;
        }
}

/* Function cleans up the options */
void cleanup_opt()
{
        if (log_file)
                fclose(log_file);
}
