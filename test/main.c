#include "tests.h"
#include "../src/cclog.h"
#include <stdio.h>
#include <stdlib.h>

#define LOG_FILE_PATH  "/home/lukas/Repositories/CCLog/TestLogFile"
#define JSON_FILE_PATH "/home/lukas/TestExort.json"

int callback(const char *msg, void *priv) {
        puts("\nCallback test\n");
        return 0;
}

static cclog_callback_mapping_t maps[] = {
        {"callback_test", callback}, 
        {NULL, NULL}
};


int main(int argc, char **argv)
{
        // llist_test();
        // program((const char **)argv);
        // json();
        cclogger_init(LOGGING_SINGLE_FILE, LOG_FILE_PATH,(const char**) argv);

        cclogger_load_config_json(JSON_FILE_PATH, maps);

        cclog(3, NULL, "Message test");

        cclogger_export_config_json("/home/lukas/CompareExport.json");

        cclogger_uninit();

        return 0;
}

