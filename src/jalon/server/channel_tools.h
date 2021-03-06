#ifndef CHANNEL_TOOLS_H_
#define CHANNEL_TOOLS_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>
#include <time.h>

#include "user_tools.h"

/* -------------- Definition de la structure channel------------- */
struct channel {
  char* name;
  int nb_members;
  struct channel* next;
};

/* -------------- Create a new list of channels-------------- */
struct channel* channel_list_init();


/* -------------- Create a new channel-------------- */
struct channel* channel_new(char* name);


/* -------------- Add a channel to the list -------------- */
struct channel* channel_add(struct channel*, char* name);


/* -------------- Find a channel thanks to his name -------------- */
struct channel* channel_find_name(struct channel* first_channel, char* name);


/* -------------- Remove a channel from the list -------------- */
struct channel* remove_channel(struct channel* first_channel, struct channel* removed_channel);


/* -------------- Return the number of connected clients -------------- */
int nbre_channel(struct channel* first_channel);

#endif
