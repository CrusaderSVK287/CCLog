#ifndef __CCLOG_MACROS_H__
#define __CCLOG_MACROS_H__

#ifndef __CCLOG_H__
#include <cclog.h>
#endif /* __CCLOG_H__ */

#include <stdbool.h>

/**
 * Macros for conditional jumping if expression evaluates to true
 */

/* Fires if val is 0 */
#define if_success(val, label) {if (val == 0) {goto label;}}
/* Fires if val is greater than zero */
#define if_success_p(val, label) {if (val > 0) {goto label;}}
/* Fires if val is greater or equal to zero */
#define if_success_pe(val, label) {if (val >= 0) {goto label;}}
/* Fires if val is less than zero */
#define if_success_n(val, label) {if (val < 0) {goto label;}}
/* Fires if val is less or equal to zero */
#define if_success_ne(val, label) {if (val <= 0) {goto label;}}
/* Fires if val is not zero */
#define if_failed(val, label) {if (val != 0) {goto label;}}
/* Fires if val less than zero */
#define if_failed_n(val, label) {if (val < 0) {goto label;}}
/* Fires if val less or equal to zero */
#define if_failed_ne(val, label) {if (val <= 0) {goto label;}}
/* Fires if val is greater than zero */
#define if_failed_p(val, label) {if (val > 0) {goto label;}}
/* Fires if val is greater or equal to zero */
#define if_failed_pe(val, label) {if (val >= 0) {goto label;}}
/* Fires if val is null */
#define if_null(val, label) {if (val == (void*)0) {goto label;}}
/* Fires if val is not null*/
#define if_not_null(val, label) {if (val == (void*)0) {goto label;}}
/* Fires if val is true */
#define if_true(val, label) {if (val == true) {goto label;}}
/* Fires if val is false */
#define if_false(val, label) {if (val == false) {goto label;}}


/**
 * Macros for logging and jumping if expression evaluates to true 
 */

/* Fires if val is 0 */
#define if_success_log(val, label, log_level, priv, msg, ...) {\
    if (val == 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}
/* Fires of val is less than zero */
#define if_success_log_n(val, label, log_level, priv, msg, ...) {\
    if (val < 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}
/* Fires of val is less or equal to zero */
#define if_success_log_ne(val, label, log_level, priv, msg, ...) {\
    if (val <= 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}
/* Fires if val is greater than zero */
#define if_success_log_p(val, label, log_level, priv, msg, ...) {\
    if (val > 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}
/* Fires if val is greater or equal to zero */
#define if_success_log_pe(val, label, log_level, priv, msg, ...) {\
    if (val >= 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}
/* Fires if val is not zero */
#define if_failed_log(val, label, log_level, priv, msg, ...) {\
    if (val != 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}
/* Fires if val is less than zero */
#define if_failed_log_n(val, label, log_level, priv, msg, ...) {\
    if (val < 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}
/* Fires if val is less or equal to zero */
#define if_failed_log_ne(val, label, log_level, priv, msg, ...) {\
    if (val <= 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}
/* Fires if val is greater than zero */
#define if_failed_log_p(val, label, log_level, priv, msg, ...) {\
    if (val > 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}
/* Fires if val is greater or equal to zero */
#define if_failed_log_pe(val, label, log_level, priv, msg, ...) {\
    if (val >= 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}
/* Fires if val is null */
#define if_null_log(val, label, log_level, priv, msg, ...) {\
    if (val == (void*)0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}
/* Fires if val is not null */
#define if_not_null_log(val, log_level, priv, msg, ...) {\
    if (val != (void*)0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}
/* Fires if val is true */
#define if_true_log(val, log_level, priv, msg, ...) {\
    if (val == true) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}
/* Fires if val is not null */
#define if_false_log(val, log_level, priv, msg, ...) {\
    if (val == false) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}

/**
 * No goto variants of log Macros
 */

/* Fires if val is 0, doesnt jump to label */
#define if_success_log_ng(val, log_level, priv, msg, ...) {\
    if (val == 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
    }\
}
/* Fires of val is less than zero, doesnt jump to label */
#define if_success_log_n_ng(val, log_level, priv, msg, ...) {\
    if (val < 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
    }\
}
/* Fires of val is less or equal to zero, doesnt jump to label */
#define if_success_log_ne_ng(val, log_level, priv, msg, ...) {\
    if (val <= 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
    }\
}
/* Fires if val is greater than zero, doesnt jump to label  */
#define if_success_log_p_ng(val, log_level, priv, msg, ...) {\
    if (val > 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
    }\
}
/* Fires if val is greater or equal to zero, doesnt jump to label  */
#define if_success_log_pe_ng(val, log_level, priv, msg, ...) {\
    if (val >= 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
    }\
}
/* Fires if val is not zero, doesnt jump to label  */
#define if_failed_log_ng(val, log_level, priv, msg, ...) {\
    if (val != 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
    }\
}
/* Fires if val is less than zero, doesnt jump to label  */
#define if_failed_log_n_ng(val, log_level, priv, msg, ...) {\
    if (val < 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
    }\
}
/* Fires if val is less or equal to zero, doesnt jump to label  */
#define if_failed_log_ne_ng(val, log_level, priv, msg, ...) {\
    if (val <= 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
    }\
}
/* Fires if val is greater than zero, doesnt jump to label  */
#define if_failed_log_p_ng(val, log_level, priv, msg, ...) {\
    if (val > 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
    }\
}
/* Fires if val is greater or equal to zero, doesnt jump to label  */
#define if_failed_log_pe_ng(val, log_level, priv, msg, ...) {\
    if (val > 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
    }\
}
/* Fires if val is null, doesnt jump to label  */
#define if_null_log_ng(val, log_level, priv, msg, ...) {\
    if (val == (void*)0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
    }\
}
/* Fires if val is not null, doesnt jump to label  */
#define if_not_null_log_ng(val, log_level, priv, msg, ...) {\
    if (val != (void*)0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
    }\
}
/* Fires if val is true, doesnt jump to label  */
#define if_true_log_ng(val, log_level, priv, msg, ...) {\
    if (val == true) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
    }\
}
/* Fires if val is false, doesnt jump to label  */
#define if_false_log_ng(val, log_level, priv, msg, ...) {\
    if (val == falsa) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
    }\
}
#endif // !__CCLOG_MACROS_H__
