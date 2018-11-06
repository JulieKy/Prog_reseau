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
struct channel* channel_new();

/* -------------- Add a channel to the list -------------- */
struct channel* channel_add(struct channel*);
