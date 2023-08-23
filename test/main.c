#include "tests.h"
#include "../src/cclog.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
        puts("Test start");
        // // llist_test();
        // program((const char **)argv);
        // json();
        // cclogger_init(LOGGING_SINGLE_FILE, LOG_FILE_PATH,(const char**) argv);

        cclogger_init(LOGGING_SINGLE_FILE , LOG_FILE_PATH, "Test_program");

        cclogger_load_config_json(JSON_FILE_PATH, maps);
        // cclogger_server_start(8000);



        for (int i = 0; i < 15; i ++) {
                printf("%d\n", i + 1);
                sleep(1);
        }
        // cclogger_export_config_json("/home/lukas/CompareExport.json");
        // cclogger_server_stop();
        cclogger_uninit();

        // puts("End of tests");

        return 0;
}

