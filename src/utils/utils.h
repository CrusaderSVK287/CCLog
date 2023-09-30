#ifndef __CCLOG_UTILS_H__
#define __CCLOG_UTILS_H__

#include <time.h>
#include <stdbool.h>

/**
 * Function creates a timeinfo struct with current time 
 * and returns the pointer to it 
 */
struct tm *util_current_timeinfo();

/* Converts boolean to string true/false and vice versa */
const char *bool_to_str(bool b);
bool str_to_bool(const char *s);
/* Replaces last old_c in s with new_c */
char *replace_last_char(char *s, char old_c, char new_c);

#endif // !__CCLOG_UTILS_H__

