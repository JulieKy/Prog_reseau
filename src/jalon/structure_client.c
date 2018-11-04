#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "structure_client.h"


// Definition de la structure client
struct clt {
  int sockfd;
  char pseudo;
  char* date;
  int port;
  int IP; // A CHANGER
  struct clt* next;
};

struct clt* client_list_init(){
  struct clt* list_client=NULL;
  return list_client;
}

struct clt* client_new(int sockfd, char pseudo, char* date, int port, int IP){
  struct clt* new_client=malloc(sizeof(new_client));
  if (!new_client)
    perror("Creation new client : memory error");
  new_client->sockfd=sockfd;
  new_client->pseudo=pseudo;
  new_client->date=date;
  new_client->port=port;
  new_client->IP=IP;
  new_client->next=NULL;
  return new_client;
}

struct clt* client_add(struct clt* client, int sockfd, char pseudo, char* date, int port, int IP){
  struct clt* new_client=client_new(sockfd, pseudo, date, port, IP);
  // Ajout à la fin
  if (client==NULL)
    new_client->next=client;
  else {
    struct clt* temp;
    temp=client;
    while(temp->next!=NULL)
      temp=temp->next;
    temp->next=new_client;
    new_client->next=NULL;
    return client;
  }
}

void client_free(struct clt* removed_client, struct clt* first_client){
  struct clt* temp=first_client;
  if (first_client==removed_client){
    if (first_client->next==NULL)
      first_client=NULL;
    else
      first_client=temp->next;
      free(removed_client); // Peut etre marche pas
    }
  else{
    while (temp->next!=removed_client)
      temp=temp->next;
    temp->next=removed_client->next;
    free(removed_client); // Peut etre marche pas
  }
}
