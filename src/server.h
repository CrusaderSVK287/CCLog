#ifndef __CCLOG_SERVER_H__
#define __CCLOG_SERVER_H__

/**
 * Function creates a non-blocking socket on given port and returns fd for it 
 */
int cclog_server_create_socket(int port);

/**
 * Function creates main server process and returns its pid
 */
int cclog_server_create_process(int fd);

#endif // !__CCLOG_SERVER_H__

