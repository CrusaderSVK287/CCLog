#include "server.h"
#include "cclog.h"
#include "options.h"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

static char *server_mapping_index(char *response)
{
        char buff[BUFSIZ];
        snprintf(buff, BUFSIZ, "%s %ld\r\n", HEADER_CONTENT_LENGHT, strlen(DEFAULT_INDEX_PAGE));
        strcat(response, buff);

        snprintf(buff, BUFSIZ, "%s text/html\r\n\r\n", HEADER_CONTENT_TYPE);
        strcat(response, buff);

        ssize_t response_size = strlen(response);
        ssize_t data_size = strlen(DEFAULT_INDEX_PAGE) + 4;

        if (BUFSIZ < response_size + data_size) {
                response = realloc(response, response_size + data_size + 1);
        }

        if_null(response, error);

        strcat(response, DEFAULT_INDEX_PAGE);
        strcat(response, "\r\n\r\n");

        return response;
error:
        return NULL;
}

static char *server_mapping_config(char *response)
{
        return response;
}

static char *server_mapping_log(char *response)
{
        return response;
}

static server_page_mapping_t maps[] = {
        {"/", server_mapping_index},
        {"/config", server_mapping_config}, 
        {"/log", server_mapping_log},
        {NULL, NULL}
};

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

static void build_response(char *response, const char *http_mapping)
{
        if (!response || !http_mapping)
                return;

        server_mapping_data_fetch_t data_fetch_func = get_func_from_map(http_mapping);
        if_null(data_fetch_func, error404);

        strcat(response, "HTTP/1.0 200 OK\r\n");
        if_null(data_fetch_func(response), error);
       
        return;

error404:
        strcat(response, "HTTP/1.0 404 Not Found\r\n\r\n");
        return;

error:
        cclog_server_debug("Failed to build a response");
}

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

        int rv = read(fd, recv_req, BUFSIZ);
        if (rv < 0) {
                cclog_debug("Failed to read request from client");
                goto error;
        }

        cclog_server_debug("client request:\n%s", recv_req);

        rv = sscanf(recv_req, "%s %s HTTP/%f\r\n", http_method, http_mapping, &http_version);

        if (rv != 3) {
                cclog_debug("Failed to parse http request");
                goto error;
        }

        cclog_debug("request dump: %s %s %f", http_method, http_mapping, http_version);

        char *response = malloc(BUFSIZ + 1);
        if (!response) {
                cclog_server_debug("Failed to allocate for response");
                goto error;
        }

        build_response(response, http_mapping);
        if_null(response, error);
        
        write(fd, response, strlen(response));

        free(response);
        close(fd);
        return 0;
error:
        return -1;
}

static volatile int server_continue;

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

                close(fd);
                rv = server_serve(client_fd);
                close(client_fd);

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

static int server_process_main(int fd)
{
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

        return 0;
error:
        return -1;
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

