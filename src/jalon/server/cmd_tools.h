#ifndef CMD_TOOLS_H_
#define CMD_TOOLS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <time.h>

#include "channel_tools.h"
#include "server_tools.h"

/* -------------- First pseudo -------------- */
char* first_pseudo(struct clt* client, char* msg);

/* -------------- Change a pseudo -------------- */
char* nick(struct clt* client, char* msg);

/* -------------- Create the list of online users -------------- */
char* who(struct clt* first_client);

/* -------------- Give information about a user -------------- */
char* whois(struct clt* first_client, char* pseudo);

/* -------------- Create the list of online users -------------- */
char* what_channels(struct channel* first_channel);

/* -------------- Create the list of online members in a channel -------------- */
char* who_channels(struct channel* list_channel, char * msg, struct clt* list_client);

/* -------------- Write a broadcast message -------------- */
void do_write_broadcast(int sock, char* msg, struct clt* first_client);

/* -------------- Write a unicast message -------------- */
char* do_write_unicast(int sock_sender, char* pseudo_rcv, char* msg, struct clt* first_client, int test_send);

/* -------------- Write a multicast message -------------- */
void do_write_multicast(struct clt* first_client, struct clt* client, char* msg, struct channel* first_channel);

/* -------------- Create a channel -------------- */
char* create_channel(struct clt* client, char* msg, struct channel* first_channel);

/* -------------- Join channel -------------- */
char* join_channel(struct clt* client, char* msg, struct channel* first_channel);

/* -------------- Send a file -------------- */
char* send_file(struct clt* first_client, struct clt* client, char* user_rcv, char* file);

/* -------------- Join channel -------------- */
char* quit(struct clt* first_client, struct clt* client, char* msg, struct channel* first_channel);


#endif
