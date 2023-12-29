#include <stdio.h>
#include <cclog.h>

int log_callback(const char *msg, void *priv)
{
        if (priv) {
                printf("My private data in callback %d\n", *(int*)priv);
                return *(int*)priv;
        }

        return -1;
}


static cclog_callback_mapping_t cclog_cb_maps[] = {
        {"log_callback", log_callback},
        {NULL, NULL}
};

int main()
{
	/* 
	 * Initialise the logger.
	 * We will be using a single file called my_log_file
	 * We dont need to specify proc_name since we are sure
	 * path isnt NULL
	 */
	cclogger_init(LOGGING_SINGLE_FILE, "./my_log_file", NULL);

        cclogger_export_config_json("default.json");
       /*
        cclogger_set_default_message_format("(${TIME}) ${FILE} : ${MSG}");

        cclogger_add_log_level(true, true, CCLOG_TTY_CLR_MAG, NULL, 
                        "This log level has overriden message format: ${MSG}", 5);
        cclogger_add_log_level(false, true, CCLOG_TTY_CLR_BLU, &cclog_cb_maps[0], NULL, 5);
        cclogger_add_log_level(true, true, CCLOG_TTY_CLR_GRN, &cclog_cb_maps[0], 
                        "This log level has both message format override and a callback: ${MSG}", 5);
       */

        /* We can replace this entire configuration by loading a json config */
        cclogger_load_config_json("my_custom_config.json", cclog_cb_maps);

	/* We write a message to the file only */
	cclog(CCLOG_LEVEL_MSG, NULL, "Lets do some logging");

        cclog(3, NULL, "This message will appear both in file and terminal");
	
        int num = 30;
        printf("callback return: %d\n", cclog(4, &num, 
                                "Testing callback return value, should be %d", num));
        printf("callback return: %d\n", cclog(5, &num, 
                                "Testing callback return value, should be %d", num));

 
	/* Uninitialise logger */
	cclogger_uninit();

	return 0;
}

