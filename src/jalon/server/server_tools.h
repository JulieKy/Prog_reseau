#ifndef SERVER_TOOLS_H_
#define SERVER_TOOLS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/poll.h>

#include "cmd_tools.h"

/* -------------- Create the socket and check the validity -------------- */
int do_socket();

/* -------------- Init the serv_add structure -------------- */
struct sockaddr_in init_serv_addr(int);

/* -------------- Perform the binding -------------- */
void do_bind(int, struct sockaddr_in);

/* -------------- Listen to clients -------------- */
void do_listen(int, struct sockaddr_in);

/* -------------- Accept connection from client -------------- */
int do_accept(struct sockaddr_in*, int);

/* -------------- Accept 20 concurrent client -------------- */
int test_nb_users(struct pollfd*, int, int, int, struct sockaddr_in);

/* -------------- Read what the client has to say -------------- */
char* do_read(int);

/* -------------- We write back to the client -------------- */
void do_write(char* buf, int new_sock);

/* -------------- Test the different queries when client isn't in a channel -------------- */
struct channel* treat_writeback(char *buf, struct clt* first_client, int sock, struct channel* first_channel);

/* -------------- Cleanup socket -------------- */
void do_close(int, struct clt*);

#endif
