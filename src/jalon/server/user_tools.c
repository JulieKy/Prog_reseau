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

//#define MSG_MAXLEN 200

/* -------------- Create a new list of clients-------------- */
struct clt* client_list_init() {
  struct clt* list_client= malloc(sizeof(*list_client));
  list_client=NULL;
  return list_client;
}

/* -------------- Create a new client-------------- */
struct clt* client_new(int sockfd, char* IP, unsigned short port){
  struct clt* new_client=malloc(sizeof(*new_client));
  if (!new_client)
    perror("Creation new client : memory error");
  new_client->sockfd=sockfd;

  // Pseudo
  //new_client->psd= malloc(sizeof (char) * MSG_MAXLEN);
  new_client->psd="unknown";

 // Date et heure
  time_t t;
  t=time(NULL);
  char* date=ctime(&t);
  new_client->date=date;

  // IP
  new_client->IP=IP;

  // Port
  new_client->port=port;

  // channel
  new_client->channel="Server";

  return new_client;
 }

/* -------------- Add a client to the list -------------- */
struct clt* client_add(struct clt* first_client, int sockfd, char* IP, unsigned short port){
  struct clt* new_client=client_new(sockfd, IP, port);
  // Ajout en début de liste
  new_client->next=first_client;
  first_client=new_client;
  return first_client;
}


/* -------------- Find a client thanks to his socket number -------------- */
struct clt* client_find_sock(struct clt* first_client, int sock){
  struct clt* found_client=first_client;
  while (found_client!=NULL && found_client->sockfd!=sock){
    found_client=found_client->next;
    printf("find client\n");
  }
  return found_client;
}

/* -------------- Find a client thanks to his pseudo -------------- */
struct clt* client_find_pseudo(struct clt* first_client, char* pseudo){

  struct clt* found_client=first_client;

  if (strcmp(pseudo, found_client->psd)==0)
    return found_client;

  while (found_client!=NULL && (strcmp(pseudo, found_client->psd)!=0)){
    found_client=found_client->next;
  }
  return found_client;
}

/* -------------- Test if the client belong to this channel -------------- */
struct clt* client_find_channel(struct clt* client, char* channel_name){

  if (strcmp(client->channel, channel_name)==0)
    return client;

  return NULL;
}


/* -------------- Remove a client from the list -------------- */
struct clt* remove_client(struct clt* first_client, struct clt* removed_client){

  struct clt* temp = first_client;
  if (first_client==NULL){
    return NULL;
  }
  //Suppression premier element
  if(temp==removed_client){
    return temp->next;
  }

  while(temp->next!=NULL){
    //Suppression dernier element
    if ((temp->next)->next==NULL && temp->next==removed_client) {
      temp->next=NULL;
      break;
    }
    // Suppression element au milieu
    if(temp->next==removed_client) {
      temp->next=(temp->next)->next;
    }
    temp=temp->next;
  }
  free(removed_client);
  return first_client;
}

/* -------------- Return the number of connected clients -------------- */
int nbre_client(struct clt* first_client){
  if (first_client==NULL)
    return 0;
  else {
    struct clt* temp=first_client;
    int c=1;
    while (temp->next!=NULL){
      temp=temp->next;
      c++;
    }
    return c;
  }
}
