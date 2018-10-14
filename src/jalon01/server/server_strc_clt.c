#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server_strc_clt.h"

struct clt* client_list_init() {
  struct clt* list_client= malloc(sizeof(*list_client));
  list_client=NULL;
  return list_client;
}


//struct clt* client_new(int sockfd, char pseudo, char* date, int port, int IP){
struct clt* client_new(int sockfd){
  struct clt* new_client=malloc(sizeof(*new_client));
  if (!new_client)
    perror("Creation new client : memory error");
  new_client->sockfd=sockfd;
  new_client->psd="";
  // new_client->date=date;
  // new_client->port=port;
  // new_client->IP=IP;
  new_client->next=NULL;
  return new_client;
}

//struct clt* client_add(struct clt* client, int sockfd, char pseudo, char* date, int port, int IP){
struct clt* client_add(struct clt* first_client, int sockfd){
  //struct clt* new_client=client_new(sockfd, pseudo, date, port, IP);
  struct clt* new_client=client_new(sockfd);
  // Ajout en début de liste
  new_client->next=first_client;
  first_client=new_client;
  printf("client add\n");
  return first_client;
}

struct clt* client_find(struct clt* first_client, int sock){
  struct clt* found_client=first_client;
  while (found_client!=NULL && found_client->sockfd!=sock){
    found_client=found_client->next;
  }
  return found_client;
}

void client_free(struct clt* first_client, int sock){
  struct clt* removed_client=client_find(first_client,sock);
  struct clt* temp=first_client;
  if (first_client==removed_client){
    printf("suppression 1\n");
    if (first_client->next==NULL)
      first_client=NULL;
    else
      first_client=temp->next;
      free(removed_client); // Peut etre marche pas
    }
  else{
    while (temp->next!=removed_client)
      printf("cherche el à suppr\n");
      temp=temp->next;
    temp->next=removed_client->next;
    free(removed_client); // Peut etre marche pas
  }
}

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




  // // Ajout à la fin
  // if (client==NULL)
  //   new_client->next=client;
  // else {
  //   struct clt* temp;
  //   temp=client;
  //   while(temp->next!=NULL)
  //     temp=temp->next;
  //   temp->next=new_client;
  //   new_client->next=NULL;
  //   return client;
  // }
