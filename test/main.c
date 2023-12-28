#include "tests.h"
#include "../src/cclog.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LOG_FILE_PATH  "/home/lukas/Repositories/CCLog/TestLogFile"
#define JSON_FILE_PATH "/home/lukas/Repositories/CCLog/TestExport.json"

int callback(const char *msg, void *priv) {
        puts("\nCallback test\n");
        return 0;
}

// static cclog_callback_mapping_t maps[] = {
//         {"callback_test", callback}, 
//         {NULL, NULL}
// };


// void make_config() {
//         cclogger_add_log_level(true, true, CCLOG_TTY_CLR_WHT, NULL, NULL, 8);
//         cclogger_add_log_level(true, true, CCLOG_TTY_CLR_WHT, NULL, NULL, 12);
//         cclogger_add_log_level(true, true, CCLOG_TTY_CLR_WHT, NULL, NULL, 20);
//         cclogger_set_verbosity_level(15);
//         cclogger_set_default_message_format("(${DATE}) ${TIME} : ${MSG}");
//         cclogger_server_start(8000);
// }

int main(int argc, char **argv)
{
        cclogger_init(LOGGING_SINGLE_FILE, LOG_FILE_PATH, "testcclog");

        // make_config();

        cclogger_load_config_json(JSON_FILE_PATH, NULL);

	cclog(2, NULL, "Should appear");
	cclog(3, NULL, "verbosity 8");
	cclog(4, NULL, "verbosity 12");
	cclog(5, NULL, "High verbosity, should not appear");

        for (int i = 0; i < 30; i ++) {
                //printf("%d\n", i + 1);
		cclog(3, NULL, "Test uptime in seconds %d", i);
                sleep(1);
        }
        // cclogger_export_config_json(JSON_FILE_PATH);
        
        cclogger_server_stop();
        cclogger_uninit();

        return 0;
}

