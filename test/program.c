#include "tests.h"
#include "../src/cclog.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define LOG_FILE_PATH  "/home/lukas/Repositories/CCLog/TestLogFile"
#define JSON_FILE_PATH "/home/lukas/TestExort.json"

int callback_test(const char *msg, void *priv)
{
        printf("Callback msg: %s\n", msg);

        int *num = (int *)priv;

        printf("Num: %d\n", *num);

        return 42;
}

static void no_levels(const char **argv)
{
        printf("\n\nStarting no levels test reseting the log levels, 2 errors expected\n\n");

        cclogger_init(LOGGING_SINGLE_FILE, LOG_FILE_PATH, "test");

        cclogger_reset_log_levels();

        cclog(0, NULL, "Test 1");
        cclog(0, NULL, "TEST 2");
        
        cclogger_uninit();
}

static void no_init()
{
        printf("\n\nStarting no init test wihtout cclogger_init, critical error expected\n\n");

        cclog(0, NULL, "This should fail");
}

static cclog_callback_mapping_t cclog_cb_maps[] = {
        {"callback_test", callback_test}
};

void generic_usage_test(const char **argv)
{
        int a = 18;
        printf("\n\nStarting generic usage test, no errors should pop up\n\n");
        cclogger_init(LOGGING_SINGLE_FILE, LOG_FILE_PATH , "test");

        cclogger_add_log_level(true, true, CCLOG_TTY_CLR_BLU, &cclog_cb_maps[0], "${DATE} ${MSG}", 10);
        cclogger_add_log_level(true, false, CCLOG_TTY_CLR_DEF, NULL, "String ${MSG}", 10);
        cclogger_export_config_json(JSON_FILE_PATH);
        
        cclog(0, NULL, "Testing message without format");
        printf("Recalling callback, rv should be -1. RV = %d\n", cclogger_recall_last_callback(&a));
        cclog(1, NULL, "Testing message %d", 2);
        cclog(2, NULL, "Testing %s %d", "message", 3);

        int ret = cclog(3, &a, "Testing callback with priv data %d", a);

        printf("ret = %d %d\n", ret, cclogger_last_log_return_value());

        cclogger_recall_last_callback(&a);

        cclogger_uninit();
}

void default_message_set_test(const char **argv)
{
        printf("\n\nStarting %s \n\n", __FUNCTION__);
        cclogger_init(LOGGING_SINGLE_FILE, LOG_FILE_PATH, "test");

        cclogger_add_log_level(false, true, CCLOG_TTY_CLR_DEF, NULL, "Override msg format ${MSG}", 10);

        puts("Valid string");
        cclogger_set_default_message_format("${DATE} Testing ok ${LINE} string ${TIME}");
        puts("Missing bracket after dollar");
        cclogger_set_default_message_format("Testing missing ${TIME} bracket after $ dollar");
        puts("Dollar before closing bracket");
        cclogger_set_default_message_format("Testing dollar ${TIME ${DATE}} before closing bracket");
        puts("opening bracket before closing bracket {{}");
        cclogger_set_default_message_format("Testing opening bracket ${TIME{} before closing bracket");
        puts("Missing closing bracket");
        cclogger_set_default_message_format("Testing missing ${DATE} closing ${TIME bracket");
        puts("Environment var");
        cclogger_set_default_message_format("Testing env var. User is ${USER}. ${MSG}");
        cclog(CCLOG_LEVEL_INFO, NULL, "Test");

        cclog(3, NULL, "Override test");

        cclogger_uninit();
}

void program(const char **argv)
{
        generic_usage_test(argv);
        no_init();
        no_levels(argv);
        default_message_set_test(argv);

        // cclogger_json_load(JSON_FILE_PATH); 
}

