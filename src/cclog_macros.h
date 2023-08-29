#ifndef __CCLOG_MACROS_H__
#define __CCLOG_MACROS_H__

/**
 * Macros for conditional jumping if expression evaluates to true
 */

/* Fires if val is 0 */
#define if_success(val, label) {if (val == 0) {goto label;}}
/* Fires if val is greater than zero */
#define if_successp(val, label) {if (val > 0) {goto label;}}
/* Fires if val is less than zero */
#define if_successn(val, label) {if (val < 0) {goto label;}}
/* Fires if val is not zero */
#define if_failed(val, label) {if (val != 0) {goto label;}}
/* Fires if val less than zero */
#define if_failedn(val, label) {if (val < 0) {goto label;}}
/* Fires if val is greater than zero */
#define if_failedp(val, label) {if (val > 0) {goto label;}}
/* Fires if val is null */
#define if_null(val, label) {if (val == (void*)0) {goto label;}}
/* Fires if val is not null*/
#define if_not_null(val, label) {if (val == (void*)0) {goto label;}}

/**
 * Macros for logging and jumping if expression evaluates to true 
 */
#define if_failed_log(val, label, log_level, priv, msg, ...) {\
    if (val != 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}

#define if_failed_logn(val, label, log_level, priv, msg, ...) {\
    if (val < 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}

#define if_failed_logp(val, label, log_level, priv, msg, ...) {\
    if (val > 0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}

#define if_null_log(val, label, log_level, priv, msg, ...) {\
    if (val == (void*)0) {\
        cclog(log_level, priv, msg, ## __VA_ARGS__);\
        goto label;\
    }\
}

#endif // !__CCLOG_MACROS_H__
