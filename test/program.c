#include "tests.h"
#include <cclog.h>
#include <stdlib.h>
#include <stdio.h>

#define LOG_FILE_PATH  "/home/lukas/Repositories/CCLog/TestLogFile"

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

        cclogger_init(LOGGING_SINGLE_FILE, LOG_FILE_PATH, argv);

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

void generic_usage_test(const char **argv)
{
        printf("\n\nStarting generic usage test, no errors should pop up\n\n");
        cclogger_init(LOGGING_SINGLE_FILE, LOG_FILE_PATH , (const char **)argv);

        cclogger_add_log_level(true, true, CCLOG_TTY_CLR_BLU, callback_test);

        cclog(0, NULL, "Testing message without format");
        cclog(1, NULL, "Testing message %d", 2);
        cclog(2, NULL, "Testing %s %d", "message", 3);

        int a = 18;
        int ret = cclog(3, &a, "Testing callback with priv data %d", a);

        printf("ret = %d\n", ret);

        cclogger_uninit();
}

void program(const char **argv)
{
        generic_usage_test(argv);
        no_init();
        no_levels(argv);
}

