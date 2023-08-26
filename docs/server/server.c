#include <signal.h>
#include <stdio.h>
#include <cclog.h>
#include <time.h>
#include <unistd.h>
/* Ignore this variable and function, it is just here 
 * so we can stop the server properly
 * */
static int keep_running = 1;
void sigint_handler(int n) {keep_running = 0;}



#define SERVER_PORT 8080

int main()
{
        /* Ignore this function call */
        signal(SIGINT, sigint_handler);

	/* 
	 * Initialise the logger.
	 * We will be using a single file called my_log_file
	 * We dont need to specify proc_name since we are sure
	 * path isnt NULL
	 */
	cclogger_init(LOGGING_SINGLE_FILE, "./my_log_file", NULL);

        /* start the server on port SERVER_PORT */
        cclogger_server_start(SERVER_PORT);

        pid_t server_pid = cclogger_server_pid();
        printf("Started server on port %d and process id is %d\n", SERVER_PORT, server_pid);

        /*
         * We will run the server for 60 seconds, visit localhost:SERVER_PORT to see the page 
         */
        cclog(CCLOG_LEVEL_MSG, NULL, "Server started");
        for(int i = 0; i < 60 && keep_running; i++) {
                sleep(1);
                cclog(CCLOG_LEVEL_MSG, NULL, "Server has been running for %d seconds", i + 1);
        }

        /* stop the server */
        cclogger_server_stop();
        printf("Server has been stopped\n");

	/* Uninitialise logger */
	cclogger_uninit();

	return 0;
}

