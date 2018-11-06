#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>
#include <time.h>

#include "channel_tools.h"

/* -------------- Create a new list of channels-------------- */
struct channel* channel_list_init() {
  struct channel* list_channel= malloc(sizeof(*list_channel));
  list_channel=NULL;
  struct clt* list_client = client_list_init();
  list_channel->client= list_client;
  return list_channel;
}


/* -------------- Create a new channel-------------- */
struct channel* channel_new(){
  struct channel* new_channel=malloc(sizeof(*new_channel));
  new_channel->name="unknown";
  printf("channel new");
  return new_channel;
}

/* -------------- Add a channel to the list -------------- */
struct channel* channel_add(struct channel* first_channel){
  struct channel* new_channel=channel_new();
  // Ajout en début de liste
  new_channel->next=first_channel;
  first_channel=new_channel;
  return first_channel;
}
