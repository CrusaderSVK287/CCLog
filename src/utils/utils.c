#include "utils.h"
#include <stdbool.h>
#include <string.h>

struct tm *cclog_util_current_timeinfo()
{
        /* Get current time */
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        /* Return the struct */
        return timeinfo;
}

const char *cclog_bool_to_str(bool b)
{
        return (b) ? "true" : "false";
}

bool cclog_str_to_bool(const char *s)
{
        return (!strcmp(s, "true")) ? true : false;
}

char *cclog_replace_last_char(char *s, char old_c, char new_c)
{
        for (int i = strlen(s); i >= 0; i--) {
                if (s[i] == old_c) {
                        s[i] = new_c;
                        return s;
                }
        }

        return s;
}

