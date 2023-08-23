#include "utils.h"
#include <stdbool.h>
#include <string.h>

struct tm *util_current_timeinfo()
{
        /* Get current time */
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        /* Return the struct */
        return timeinfo;
}

const char *bool_to_str(bool b)
{
        return (b) ? "true" : "false";
}

bool str_to_bool(const char *s)
{
        return (!strcmp(s, "true")) ? true : false;
}

