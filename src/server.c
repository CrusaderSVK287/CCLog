#include "server.h"
#include "cclog.h"
#include "json.h"
#include "options.h"
#include "logger.h"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#ifdef CCLOG_DEBUG
#define cclog_server_debug(MSG, ...) cclog_debug("Server pid %d : " MSG, getpid(), ## __VA_ARGS__);
#else 
#define cclog_server_debug(...)
#endif

#define DEFAULT_INDEX_PAGE \
        "<html><body>"\
        "  <h3>CClog index</h3>"\
        "  <a href=\"/config\">Configuration</a><br>"\
        "  <a href=\"/log\">Current Log</a>"\
        "</body></html>"

#define SIG_INT_MSG "Received sig int on main server process\n"

#define HEADER_CONTENT_LENGHT "Content-Length:"
#define HEADER_CONTENT_TYPE "Content-Type:"

#define SERVER_BACKLOG_SIZE 10

typedef char* (*server_mapping_data_fetch_t)(char *response);

typedef struct {
        const char *map;
        server_mapping_data_fetch_t func;
} server_page_mapping_t;

static void cleanup_server_process()
{
        json_free_file_buffer();
        cclogger_reset_log_levels();
        cleanup_opt();
}

int server_create_socket(int port)
{
        if (port < 0)
                return -1;

        int rv = 0;

        int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd < 0) {
                cclog_error("Server: Failed to create socket");
                goto error;
        }

        /* 
         * Set socket option to reuse local address. Prevents address being 
         * blocked for other sockets after the program is finished
         */
        int opt_reuse_addr = 1;
        rv = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt_reuse_addr, sizeof(int));
        if (rv < 0) {
                cclog_error("Server: Could not set socket option");
                goto error;
        }

        /* Set up non-blocking socket */
        int flags = fcntl(sock_fd, F_GETFL, 0);
        if (flags < 0) {
                cclog_error("Server: Failed to aquire socket fd flags");
                goto error;
        }

        rv = fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK);
        if (rv < 0) {
                cclog_error("Server: Failed to set up non blocking socket");
                goto error;
        }

        struct sockaddr_in addr = {0};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        rv = bind(sock_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
        if (rv < 0) {
                cclog_error("Server: Failed to bind socket to port %d", port);
                goto error;
        }

        rv = listen(sock_fd, SERVER_BACKLOG_SIZE);
        if (rv < 0) {
                cclog_error("Server: Socket failed listening on port %d", port);
                goto error;
        }

        return sock_fd;
error:
        return -1;
}

/* map handler for / */
static char *server_mapping_index(char *response)
{
        char buff[BUFSIZ];
        /* add header fields */
        snprintf(buff, BUFSIZ, "%s %ld\r\n", HEADER_CONTENT_LENGHT, strlen(DEFAULT_INDEX_PAGE));
        strcat(response, buff);

        snprintf(buff, BUFSIZ, "%s text/html\r\n\r\n", HEADER_CONTENT_TYPE);
        strcat(response, buff);

        ssize_t response_size = strlen(response);
        ssize_t data_size = strlen(DEFAULT_INDEX_PAGE) + 4;

        /* reallocate if needed */
        if (BUFSIZ < response_size + data_size) {
                response = realloc(response, response_size + data_size + 1);
        }

        if_null(response, error);

        /* add data to response */
        strcat(response, DEFAULT_INDEX_PAGE);
        strcat(response, "\r\n\r\n");

        return response;
error:
        return NULL;
}

/* map handler for /config */
static char *server_mapping_config(char *response)
{       
        char buff[BUFSIZ];
        ssize_t json_buffer_lenght = 0;

        json_buffer_lenght = strlen(json_get_buffer());

        /* add header fields*/
        snprintf(buff, BUFSIZ, "%s %ld\r\n", HEADER_CONTENT_LENGHT, json_buffer_lenght);
        strcat(response, buff);

        snprintf(buff, BUFSIZ, "%s application/json\r\n\r\n", HEADER_CONTENT_TYPE);
        strcat(response, buff);

        ssize_t response_size = strlen(response);
        ssize_t data_size = json_buffer_lenght + 4;

        /* if buffer is not big enough, allocate more data to it */
        if (BUFSIZ < response_size + data_size) {
                response = realloc(response, response_size + data_size + 1);
        }

        if_null(response, error);

        /* add json data */
        strcat(response, json_get_buffer());
        strcat(response, "\r\n\r\n");
error:
        return response;
}

/* map handler for /log */
static char *server_mapping_log(char *response)
{
        const char *file_path = (const char*)get_opt(OPTIONS_LOG_FILE_PATH);

        /* open log file at the start */
        int fd = open(file_path, O_RDONLY);
        if (!fd) {
                cclog_server_debug("Failed to open log file descriptor");
                return NULL;
        }
        lseek(fd, 0, SEEK_SET);

        char buff[BUFSIZ];
        ssize_t rv, bytes = 0;

        char *file_data = calloc(1, BUFSIZ + 1);

        /* read the file */
        while ((rv = read(fd, buff, BUFSIZ)) > 0) {
                bytes += rv;
                strncat(file_data, buff, rv);

                if (rv != BUFSIZ || rv == 0)
                        break;

                file_data = realloc(file_data, bytes + BUFSIZ + 1);
        }

        /* reallocate appropriate size of bytes to response */
        response = realloc(response, bytes + BUFSIZ);
        if (!response)
                return NULL;

        /* write the response */
        snprintf(response, bytes + BUFSIZ,
                 "HTTP/1.0 200 OK\r\n"
                 "%s text/html\r\n"
                 "%s %ld\r\n\r\n"
                 "<h3>Log entry: %s</h3>"
                 "<a href=\"/\">Back</a><br>"
                 "<pre>%s</pre>", 
                 HEADER_CONTENT_TYPE, HEADER_CONTENT_LENGHT, bytes, 
                 file_path, file_data);

        free(file_data);
        close(fd);

        return response;
}

/* server mappings */
static server_page_mapping_t maps[] = {
        {"/", server_mapping_index},
        {"/config", server_mapping_config}, 
        {"/log", server_mapping_log},
        {NULL, NULL}
};

/* helper function to translate map to function */
static server_mapping_data_fetch_t get_func_from_map(const char *map)
{
        if (!map)
                return NULL;

        int i = 0;
        while (maps[i].map && maps[i].func) {
                if (!strcmp(map, maps[i].map)) {
                        return maps[i].func;
                }
                i++;
        }

        return NULL;
}

/* Function builds response based on GET map */
static char *build_response(char *response, const char *http_mapping)
{
        if (!response || !http_mapping)
                return NULL;

        /* get function to create a buffer */
        server_mapping_data_fetch_t data_fetch_func = get_func_from_map(http_mapping);
        if_null(data_fetch_func, error404);

        /* catonate a 200 OK response */
        strcat(response, "HTTP/1.0 200 OK\r\n");

        /* catonate header fields and data */
        response = data_fetch_func(response);
        if_null(response, error);
      
        return response;

error404:
        strcat(response, "HTTP/1.0 404 Not Found\r\n\r\n<h1>404 Not Found</h1>\r\n\r\n");
        return response;

error:
        cclog_server_debug("Failed to build a response");
        return NULL;
}

/* Function reads request and serves the result */
static int server_serve(int fd)
{
        cclog_server_debug("Serving client with fd %d", fd);

        char recv_req[BUFSIZ + 1];
        memset(recv_req, 0, BUFSIZ + 1);

        float http_version = 0.0f;
        char http_mapping[BUFSIZ + 1];
        char http_method[BUFSIZ + 1];
        memset(http_mapping, 0, BUFSIZ + 1);
        memset(http_method, 0, BUFSIZ + 1);

        /* reading request */
        int rv = read(fd, recv_req, BUFSIZ);
        if (rv < 0) {
                cclog_server_debug("Failed to read request from client");
                goto error;
        }

        cclog_server_debug("client request:\n%s", recv_req);

        /* Parsing request data */
        rv = sscanf(recv_req, "%s %s HTTP/%f\r\n", http_method, http_mapping, &http_version);

        if (rv != 3) {
                cclog_server_debug("Failed to parse http request");
                goto error;
        }

        cclog_server_debug("request dump: %s %s %f", http_method, http_mapping, http_version);

        /* allocating response */
        char *response = calloc(1, BUFSIZ + 1);
        if (!response) {
                cclog_server_debug("Failed to allocate for response");
                goto error;
        }

        /* building response */
        response = build_response(response, http_mapping);
        if_null(response, error);

        /* writing response */
        write(fd, response, strlen(response));

        free(response);
        close(fd);
       
        return 0;
error:
        return -1;
}

static volatile int server_continue;

/**
 * Server loop for accepting connections 
 */
static int server_loop(int fd)
{
        if (fd < 0)
                return -1;

        int client_fd = 0;
        int pid = 0;
        int rv = 0;

        while (server_continue) {
                client_fd = accept(fd, NULL, NULL);

                if (client_fd < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                                /* No pending connections, continue */
                                continue;
                        } else {
                                cclog_server_debug("Failed to accept connection");
                                continue;
                        }
                }

                pid = fork();
                if (pid < 0) {
                        cclog_server_debug("Failed to create child to serve client");
                        continue;
                } else if (pid > 0) {
                        /* Parent */
                        cclog_server_debug("Created child with pid %d", pid);
                        close(client_fd);
                        continue;
                }

                /* Child */

                /* Close parent socket */
                close(fd);
                rv = server_serve(client_fd);
                
                close(client_fd);
                cleanup_server_process();

                exit(rv);
        }

        close(fd);

        return 0;
}

static void main_sig_int_handler(int num) {
#ifdef CCLOG_DEBUG
        write(STDOUT_FILENO, SIG_INT_MSG, sizeof(SIG_INT_MSG));
#endif /* ifdef CCLOG_DEBUG */
        server_continue = 0;
}

/**
 * "main" function of the server process 
 */
static int server_process_main(int fd)
{
        int rv = -1;
        server_continue = 1;
        
        if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
                cclog_error("Server: failed to set SIGCHLD signal");
                goto error;
        }

        if (signal(SIGINT, main_sig_int_handler) == SIG_ERR) {
                cclog_error("Server: failed to set SIG_INT signal");
                goto error;
        }

        if_failed(server_loop(fd), error);

        rv = 0;
error:
        /* Free memory allocated by the server process */
        cleanup_server_process();
        return rv;
}

int server_create_process(int fd)
{
        if (fd < 0)
                return -1;

        int pid = fork();

        /* Error */
        if (pid < 0) {
                cclog_error("Server: Could not create a child for server");
                return -1;
        }

        if (pid == 0) {
                cclog_server_debug("Created main server process with pid %d", getpid());
                server_process_main(fd);
                exit(0);
        }

        /* Parent */
        cclog_server_debug("Parent process returning %d", pid);
        return pid;
}

