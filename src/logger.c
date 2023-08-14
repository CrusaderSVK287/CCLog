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

#define STR_EMPTY ""

#define VAR_FILE "FILE"
#define VAR_LINE "LINE"
#define VAR_FUNC "FUNCTION"
#define VAR_DATE "DATE"
#define VAR_TIME "TIME"
#define VAR_MSG "MSG"

#define VAR_YEAR "YYYY"
#define VAR_MONTH "MM"
#define VAR_DAY "DD"
#define VAR_HOUR "HOUR"
#define VAR_MIN "MIN"
#define VAR_SEC "SEC"

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

static const char *msg_buffer_variable_translate(const char *var, msg_buff_opt_t *opts)
{
        if (!var)
                return STR_EMPTY;

        struct tm *timeinfo = util_current_timeinfo();
        
        /* clear the static buffer before use */
        static char var_buff[MSG_SIZE + 1];
        memset(var_buff, 0, MSG_SIZE + 1);

        if (!strcmp(var, VAR_MSG)) {
                vsnprintf(var_buff, MSG_SIZE, opts->msg, *opts->valist);
                return var_buff;
        }
        if (!strcmp(var, VAR_FILE)) {
                return opts->file;
        }
        if (!strcmp(var, VAR_LINE)) {
                snprintf(var_buff, MSG_SIZE, "%d", opts->line);
                return var_buff;
        }
        if (!strcmp(var, VAR_FUNC)) {
                return opts->func;
        }
        if (!strcmp(var, VAR_DATE)) {
                snprintf(var_buff, MSG_SIZE, "%d-%d-%d", 
                        timeinfo->tm_year + 1900, 
                        timeinfo->tm_mon + 1, 
                        timeinfo->tm_mday);
                return var_buff;
        }
        if (!strcmp(var, VAR_TIME)) {
                snprintf(var_buff, MSG_SIZE, "%d:%d:%d", 
                        timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
                return var_buff;
        }
        if (!strcmp(var, VAR_YEAR)) {
                snprintf(var_buff, MSG_SIZE, "%d", timeinfo->tm_year + 1900);
                return var_buff;
        }
        if (!strcmp(var, VAR_MONTH)) {
                snprintf(var_buff, MSG_SIZE, "%d", timeinfo->tm_mon + 1);
                return var_buff;
        }
        if (!strcmp(var, VAR_DAY)) {
                snprintf(var_buff, MSG_SIZE, "%d", timeinfo->tm_mday);
                return var_buff;
        }
        if (!strcmp(var, VAR_HOUR)) {
                snprintf(var_buff, MSG_SIZE, "%d", timeinfo->tm_hour);
                return var_buff;
        }
        if (!strcmp(var, VAR_MIN)) {
                snprintf(var_buff, MSG_SIZE, "%d", timeinfo->tm_min);
                return var_buff;
        }
        if (!strcmp(var, VAR_SEC)) {
                snprintf(var_buff, MSG_SIZE, "%d", timeinfo->tm_sec);
                return var_buff;
        }

        const char *env = getenv(var);
        if (!env) {
                cclog_error("%s is not a built-in variable, "
                            "nor is it a defined environmental variable", var);
        }
        return (env) ? env : STR_EMPTY;
}

/* Function generates the result message buffer */
static const char *create_msg_buffer(msg_buff_opt_t *opts, log_level_t *log_level)
{
        if (!opts || !log_level)
                return STR_EMPTY;

        int ret = 0;
        
        /* since message buffer is static, we need to clear it on every function call */
        static char msg_buff[MSG_SIZE + 1];
        memset(msg_buff, 0, MSG_SIZE + 1);

        /* index of the end of result buffer */
        int msg_buff_i = 0;

        /* Format from which the log message will be generated */
        const char *string_format;
        if (!log_level->msg_format) {
                string_format = (const char *)get_opt(OPTIONS_DEF_MSG_FORMAT);
        } else {
                string_format = log_level->msg_format;
        }

        /* Buffer for translated variable returned by translation function */
        const char *translation = NULL;
        int len = strnlen(string_format, MSG_SIZE);
        
        /* Buffer for parsed variable name */
        char var_buff[128];
        memset(var_buff, 0, 128);

        for (int i = 0; i < len; i++) {
                if (string_format[i] == '$') {
                        ret = sscanf(string_format + i, "${%[^${}]}", var_buff);
                        cclog_debug("Parsing variable in message: %s, ret = %d", var_buff, ret);
                        
                        /* Exactly one string should be parsed, see format string above */
                        if (ret != 1) {
                                cclog_error("Message string format is incorrect,"
                                            " message may not be complete");
                                goto error;
                        }

                        translation = msg_buffer_variable_translate(var_buff, opts);
                        strncat(msg_buff, translation, MSG_SIZE - msg_buff_i);

                        msg_buff_i += strlen(translation);
                        /* var buff for var name and +3 for ${} */
                        i += strlen(var_buff) + 2;
                } else {
                        msg_buff[msg_buff_i] = string_format[i];
                        msg_buff_i++;
                }
        }

error:
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
        int rv = -1;
        const char *msg_buff;

        va_list args;
        va_start(args, msg);

        if (!file || !func || !msg)
                goto exit;

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
                    "\tcallback:    %s\n"
                    "\tformat:      %s\n",
                    level, log_level->color, log_level->log_to_file, log_level->log_to_tty,
                    (log_level->callback) ? "Yes" : "No", 
                    (log_level->msg_format) ? log_level->msg_format : "NULL");

        /* Create message buffer */
        msg_buff = create_msg_buffer(&buff_opts, log_level);

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

        /* Set the last callback */
        set_opt(OPTIONS_LAST_CALLBACK, log_level->callback);
exit:
        va_end(args);

        /* Set the last return value */
        set_opt(OPTIONS_LAST_LOG_RET, &rv);
        return rv;
}

/* redefine the _cclogger_log function as not directly usable */
#define _cclogger_log(...) DO_NOT_CALL_DIRECTLY_USE_cclog_MACRO

int cclogger_add_log_level(bool log_to_file, bool log_to_tty,
        cclog_tty_log_color_t color, cclog_cb callback, const char *msg_format)
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
        level->msg_format = msg_format;

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

/**
 * Simple function that checks whether a syntax for message format is met
 * For example "This is test ${MSG}" is valid while "This is not ${valid" is not 
 * Algorhitm explanation:
 * 1. Traverse every symbol in message format.
 * 2. if current symbol is $:
 *          check if next symbol is {
 *          continue traversing.
 *          if current symbol is }:
 *                  continue traversing from step 2
 *          if current symbol is $ or {:
 *                  display error, return -1 
 * 3. If traversed entire string with no errors, return 0
 */
static int message_format_parse(const char *msg)
{
        int len = strlen(msg);
        
        cclog_debug("Parsing message format %s", msg);
        /* 1. Traverse every symbol in message format */
        for (int i = 0; i < len; i++) {
                cclog_debug("\tToken %d: %c", i, msg[i]);
                
                /* 2. if current symbol is $ */
                if (msg[i] == '$') {
                        /* check if next symbol is { */
                        i++;
                        cclog_debug("\tToken %d: %c", i, msg[i]);
                        if (msg[i] != '{') {
                                cclog_error("Message format syntax error:"
                                            " Expected '{' after $ at index %d", i);
                                return -1;
                        }

                        /* continue traversing while symbol is not } or enf od string is reached */
                        i++;
                        while (msg[i] != '}' && i < len) {
                                /* If current symbol is $ or {, display error and return -1 */
                                cclog_debug("\tToken %d: %c", i, msg[i]);
                                if (msg[i] == '$') {
                                        cclog_error("Message format syntax error:"
                                                    " '$' Before closing bracket");
                                        return -1;
                                }
                                if (msg[i] == '{') {
                                        cclog_error("Message format syntax error:"
                                                    " '{' Before closing bracket");
                                        return -1;
                                }

                                i++;
                        }

                        /* In case the end of string is reached, check whether the last } is present */
                        if (msg[i] != '}') {
                                cclog_error("Message format syntax error: Missing '}' at %d", i);
                                return -1;
                        }
                }
        }

        /* No errors, return 0 */
        cclog_debug("Message format \"%s\" is OK", msg);
        return 0;
}

int cclogger_set_default_message_format(const char *str)
{
        if (!str)
                return -1;

        if_failed(set_opt(OPTIONS_DEF_MSG_FORMAT, (void*) str), error);

        if_failed(message_format_parse(str), error);

        return 0;
error:
        cclog_error("Failed to set default message format");
        return -1;
}

int cclogger_last_log_return_value() 
{
        return *(int*)get_opt(OPTIONS_LAST_LOG_RET);
}

int cclogger_recall_last_callback(void *priv)
{
        cclog_cb cb = (cclog_cb)get_opt(OPTIONS_LAST_CALLBACK);

        if (!cb) {
                return -1;
        }

        return cb(STR_EMPTY, priv);
}
