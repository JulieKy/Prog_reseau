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
#include "channel_tools.h"

/* -------------- Create a new list of channels-------------- */
struct channel* channel_list_init() {
  struct channel* list_channel= malloc(sizeof(*list_channel));
  list_channel=NULL;
  return list_channel;
}


/* -------------- Create a new channel-------------- */
struct channel* channel_new(char* name){
  struct channel* new_channel=malloc(sizeof(*new_channel));
  new_channel->name=name;
  new_channel->nb_members=0;
  return new_channel;
}

/* -------------- Add a channel to the list -------------- */
struct channel* channel_add(struct channel* first_channel, char* name){

  printf(">> Addition of a new channel\n");

  struct channel* new_channel=channel_new(name);
  new_channel->next=first_channel;
  first_channel=new_channel;

  return first_channel;
}

/* -------------- Find a channel thanks to his name -------------- */
struct channel* channel_find_name(struct channel* first_channel, char* name){
  if (first_channel==NULL){
    return NULL;
  }

  struct channel* temp=first_channel;

  if (strcmp(name, temp->name)==0)
    return temp;

  while (temp!=NULL){
    if (strcmp(name, temp->name)==0)
      return temp;
    temp=temp->next;
  }
  return NULL;
}

/* -------------- Remove a channel from the list -------------- */
struct channel* remove_channel(struct channel* first_channel, struct channel* removed_channel){

  struct channel* temp = first_channel;
  if (first_channel==NULL){
    return NULL;
  }
  //Suppression premier element
  if(temp==removed_channel){
    return temp->next;
  }

  while(temp->next!=NULL){
    //Suppression dernier element
    if ((temp->next)->next==NULL && temp->next==removed_channel) {
      temp->next=NULL;
      break;
    }
    // Suppression element au milieu
    if(temp->next==removed_channel) {
      temp->next=(temp->next)->next;
    }
    temp=temp->next;
  }
  free(removed_channel);
  return first_channel;
}


/* -------------- Return the number of connected clients -------------- */
int nbre_channel(struct channel* first_channel){
  if (first_channel==NULL)
    return 0;
  else {
    struct channel* temp=first_channel;
    int c=1;
    while (temp->next!=NULL){
      temp=temp->next;
      c++;
    }
    return c;
  }
}
