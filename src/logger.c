#include "logger.h"
#include "cclog.h"
#include "options.h"
#include "utils/llist.h"
#include "utils/defines.h"
#include "utils/utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

/* Max size of the log message */
#define MSG_SIZE 1000

/* convenience struct containing arguments passed to log function */
typedef struct msg_buff_opt {
        const int line;
        const char *file;
        const char *func;
        const char *msg;
        va_list *valist;
} msg_buff_opt_t;

/* struct containing key-value pair of enum and string */
typedef struct color_pairs {
        const char *string;
        int n;
} color_pairs_t;

/* list of color key-valie pairs */
static color_pairs_t color_pair_list[] = {
        {STR_RESET, CCLOG_TTY_CLR_DEF},
        {STR_BLK, CCLOG_TTY_CLR_BLK}, 
        {STR_RED, CCLOG_TTY_CLR_RED}, 
        {STR_GRN, CCLOG_TTY_CLR_GRN},
        {STR_YEL, CCLOG_TTY_CLR_YEL},
        {STR_BLU, CCLOG_TTY_CLR_BLU},
        {STR_MAG, CCLOG_TTY_CLR_MAG},
        {STR_CYN, CCLOG_TTY_CLR_CYN},
        {STR_WHT, CCLOG_TTY_CLR_WHT}
};
#define NUM_OF_CLR_PAIRS (sizeof(color_pair_list) / sizeof(color_pairs_t))

/* Function returns a string value to enum(int) key */
static const char *pair_clr(int num)
{
        for (int i = 0; i < NUM_OF_CLR_PAIRS; i++) {
                if (color_pair_list[i].n == num) {
                        return color_pair_list[i].string;
                }
        }

        return "";
}

/* Function generates the result message buffer */
static const char *create_msg_buffer(msg_buff_opt_t *opts)
{
        if (!opts)
                return NULL;

        static char msg_buff[MSG_SIZE + 1];
        memset(msg_buff, 0, MSG_SIZE + 1);
        char va_msg_buff[MSG_SIZE + 1];
        memset(va_msg_buff, 0, MSG_SIZE + 1);

        /* get current timeinfo */
        struct tm *timeinfo = util_current_timeinfo();

        /* create a buffer for message from user */
        vsnprintf(va_msg_buff, MSG_SIZE, opts->msg, *opts->valist);

        /* create the result buffer containing the message from user */
        snprintf(msg_buff, MSG_SIZE, 
                "[%04d-%02d-%02d_%02d:%02d:%02d]%s:%d %s", 
                timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, 
                timeinfo->tm_mday, 
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, 
                opts->file, opts->line ,va_msg_buff);

        return msg_buff;
}

/* linked list of log levels */
static cclog_llist_t *log_levels_list = NULL;

/**
 * Undefining _cclogger_log is needed to define the function, 
 * otherwise DO_NOT_CALL_DIRECTLY_USE_cclog_MACRO would be replaced
 * and compilation will fail 
 */
#ifdef _cclogger_log
#undef _cclogger_log
#endif /* ifdef _cclogger_log */

int _cclogger_log(int line, const char* file, const char *func,
        int level, void* priv, const char *msg, ...)
{
        if (!file || !func || !msg)
                return -1;

        int rv = -1;
        const char *msg_buff;

        va_list args;
        va_start(args, msg);

        /* convenience struct to hold arguments */
        msg_buff_opt_t buff_opts = {
                .msg = msg,
                .file = file,
                .func = func,
                .line = line,
                .valist = &args
        };

        /* getting log file */
        FILE* log_file = (FILE*)get_opt(OPTIONS_LOG_FILE);
        if (!log_file) {
                cclog_error("CRITICAL: Log file is not opened, did you call cclog_init()?");
                goto exit;
        }

        /**
         * checking whether any log levels are defined, this can happen if 
         * cclogger_reset_log_levels is called and no levels are specified after
         */
        if (!log_levels_list || log_levels_list->num_of_entries == 0) {
                cclog_error("No log levels are defined");
                goto exit;
        }

        /* Getting the log level */
        log_level_t *log_level = llist_get_index(log_levels_list, level);
        if (!log_level) {
                cclog_error("Log level with index %d doesnt exist", level);
                goto exit;
        }
        cclog_debug("log level %d dump:\n"
                    "\tcolor:       %d\n"
                    "\tlog_to_file: %d\n"
                    "\tlog_to_tty:  %d\n"
                    "\tcallback:    %s",
                    level, log_level->color, log_level->log_to_file, log_level->log_to_tty,
                    (log_level->callback) ? "Yes" : "No");

        /* Create message buffer */
        msg_buff = create_msg_buffer(&buff_opts);

        /* Write to file */
        if (log_level->log_to_file) {
                fprintf(log_file, "%s\n", msg_buff);
        }

        /* Write to tty in color*/
        if (log_level->log_to_tty) {
                fprintf(stderr, "%s%s%s\n", pair_clr(log_level->color), 
                                                msg_buff,
                                                pair_clr(CCLOG_TTY_CLR_DEF));
        }

        rv = 0;

        /**
         * If callback function is specified in the log level, it will be called 
         * here, passing in reference to the generated message and any private
         * data that user passed to the log function. 
         * The _cclogger_log function will then return whatever was returned 
         * by the callback function
         */
        if (log_level->callback) {
                cclog_debug("Calling callback of log level %d", level);
                rv = log_level->callback(msg_buff, priv);
        }

exit:
        va_end(args);
        return rv;
}

/* redefine the _cclogger_log function as not directly usable */
#define _cclogger_log(...) DO_NOT_CALL_DIRECTLY_USE_cclog_MACRO

int cclogger_add_log_level(bool log_to_file, bool log_to_tty,
        cclog_tty_log_color_t color, cclog_cb callback)
{
        /* Allocate the level struct on the heap */
        log_level_t *level = calloc(1, sizeof(log_level_t)); 
        if (!level) {
                cclog_error("Failed to allocate space for log level");
                goto error;
        }

        /* fill the struct with data */
        level->log_to_file = log_to_file;
        level->log_to_tty = log_to_tty;
        level->color = color;
        level->callback = callback;

        /**
         * If the log level list is no initialised, initialise it with the level,
         * otherwise, just add the level to the linked list
         */
        if (!log_levels_list) {
                cclog_debug("Initialising new log_levels_list");
                log_levels_list = llist_init(level);
                if (!log_levels_list) {
                        cclog_error("CRITICAL: Failed to initialise a list of log_levels");
                        goto error;
                }
        } else {
                if_failed(llist_add(log_levels_list, level), error);
        }

        return 0;
error:
        cclog_error("Failed to add log level to the list of log levels");
        return -1;
}

void cclogger_reset_log_levels()
{
        /* If list exists, clean it and set its pointer to NULL */
        if (log_levels_list) {
                llist_clean(log_levels_list);
                log_levels_list = NULL;
        }
}

