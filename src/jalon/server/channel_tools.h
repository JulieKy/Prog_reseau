#include "user_tools.h"

/* -------------- Definition de la structure channel------------- */
struct channel {
  char* name;
  struct clt* client;
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
