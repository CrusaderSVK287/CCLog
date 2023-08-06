#include "utils.h"

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

