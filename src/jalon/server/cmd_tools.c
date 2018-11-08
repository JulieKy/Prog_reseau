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
#include "cmd_tools.h"

#define MSG_MAXLEN 200
#define CMD_MAXLEN 10

/* -------------- First pseudo -------------- */
char* first_pseudo(struct clt* client, char* msg){

  char* rep = malloc(sizeof (char) * MSG_MAXLEN);

  client->psd=msg;
  //strcpy(client->psd,msg);
  printf(">> Le pseudo du client %d est : %s\n",sock, client->psd);
  rep = "Welcome on the chat";
  return rep;
}


/* -------------- Change a pseudo -------------- */
char* nick(struct clt* client, char* msg){

  char* rep = malloc(sizeof (char) * MSG_MAXLEN);
  char* rep2 = malloc(sizeof (char) * MSG_MAXLEN);

  if (strlen(msg)<2) {
    rep ="Your pseudo must be 2 letters long\n";
  }

  else {
    client->psd=msg;
    //strcpy(client->psd,msg);
    printf(">> Le nouveau pseudo du client %d est : %s\n",sock, client->psd);
    rep2 = "Your new pseudo is";
    sprintf(rep, "  %s %s\n" , rep2, client->psd);
  }
  return rep;
}


/* -------------- Create the list of online users -------------- */
char* who(struct clt* first_client) {

  if (first_client->next==NULL)
    return "You are the only client\n";

  else {
    char* list_pseudo=malloc(sizeof (char) *200);
    strcat(list_pseudo, "Online users are :\n");

    struct clt* temp=first_client;

    while (temp!=NULL){
     char* pseudo=malloc(sizeof (char) *200);
     sprintf(pseudo, "             - %s\n",temp->psd);
     strcat(list_pseudo, pseudo);
     temp=temp->next;
    }
    return list_pseudo;
  }
}


/* -------------- Give information about a user -------------- */
char* whois(struct clt* first_client, char* pseudo) {
    struct clt* whois_client=client_find_pseudo(first_client,pseudo);
    if (whois_client==NULL){
      char* rep="This user is not logged in";
      return rep;
    }

    char* client_info=malloc(sizeof (char) *200);
    sprintf(client_info, "%s connected since %s with IP address %s and port %d",whois_client->psd, whois_client->date, whois_client->IP, whois_client->port);
    return client_info;
}



/* -------------- Create the list of online users -------------- */
char* what_channels(struct channel* first_channel) {
  char* list_pseudo=malloc(sizeof (char) *200);

  if (first_channel!=NULL){

      strcat(list_pseudo, "Current channels are :\n");

      struct channel* temp=first_channel;

      while (temp!=NULL){
       char* pseudo=malloc(sizeof (char) *200);
       sprintf(pseudo, "             - %s\n",temp->name);
       strcat(list_pseudo, pseudo);
       temp=temp->next;
      }
  }

  else {
    list_pseudo="No channel created\n";
  }

  return list_pseudo;

}


/* -------------- Create the list of online members in a channel -------------- */
char* who_channels(struct channel* list_channel, char * msg, struct clt* list_client) {

  struct channel* channel=channel_find_name(list_channel, msg);
  struct clt* client=list_client;
  char* list_pseudo=malloc(sizeof (char) *200);

  if (client==NULL) {
    list_pseudo = "This channel doesn't exist\n";
  }

  else {

    sprintf(list_pseudo, "Members in channel %s :\n", channel->name);
    struct clt* client_found;
    char* pseudo=malloc(sizeof (char) *200);

    while (client!=NULL){
      client_found=client_find_channel(client, channel->name);
      if (client_found!=NULL) {
        sprintf(pseudo, "                      - %s\n",client_found->psd);
        strcat(list_pseudo, pseudo);
    }
    client=client->next;
  }
    return list_pseudo;
  }
}


/* -------------- Write a broadcast message -------------- */
void do_write_broadcast(int sock, char* msg, struct clt* first_client){
  struct clt* temp=first_client;

  char* msg_pseudo = malloc(sizeof (char) * MSG_MAXLEN);


  // Write a message to everyone
  if(sock==0){
    while (temp!=NULL){
        sprintf(msg_pseudo, "[%s]> %s\n", temp->channel, msg);
      write(temp->sockfd, msg_pseudo, MSG_MAXLEN);
    temp=temp->next;
    }
  }

  // Write a message to eveyone except the one who write the message
  else {
    struct clt* sender=client_find_sock(first_client, sock);

    char* name=sender->psd;
    sprintf(msg_pseudo, "[%s]: %s\n" , name, msg);
    printf("[write broadcast] : %s\n", msg_pseudo);
    while (temp!=NULL){
      if (sock!=temp->sockfd) {
        write(temp->sockfd, msg_pseudo, MSG_MAXLEN);
      }
    temp=temp->next;
    }
  }
}


/* -------------- Write a unicast message -------------- */
char* do_write_unicast(int sock, char* pseudo, char* msg, struct clt* first_client){
    // Find the receiver's pseudo and the message
    char* rcver_psd= malloc(sizeof (char) * MSG_MAXLEN);
    char* message= malloc(sizeof (char) * MSG_MAXLEN);
    char* rep= malloc(sizeof (char) * MSG_MAXLEN);

    // Find the receiver
    struct clt* rcver=client_find_pseudo(first_client, pseudo);

    if (rcver==NULL){
      rep = "This user doesn't exist.";
    }

    else {
      rep=NULL;

      // Recovery of the sender's pseudo to send : "[User]: msg"
      struct clt* sender=client_find_sock(first_client, sock);
      char* psd=sender->psd;
      char* msg_pseudo = malloc(sizeof (char) * MSG_MAXLEN);
      sprintf(msg_pseudo, "[%s]: %s\n" , psd, msg);

      //Write to the receiver
      write(rcver->sockfd, msg_pseudo, MSG_MAXLEN);
    }

    return rep;
}


/* -------------- Write a multicast message -------------- */
void do_write_multicast(struct clt* first_client, struct clt* client, char* msg, struct channel* first_channel){

    struct channel* channel= channel_find_name(first_channel, client->channel);

    char* name=client->psd;
    char* msg_pseudo = malloc(sizeof (char) * MSG_MAXLEN);
    sprintf(msg_pseudo, "[%s]: %s\n" , name, msg);

    struct clt* temp=first_client;
    while (temp!=NULL){
      if ((client->sockfd!=temp->sockfd) && (strcmp(temp->channel, channel->name)==0)){
        write(temp->sockfd, msg_pseudo, MSG_MAXLEN);
      }
    temp=temp->next;
    }
}


/* -------------- Create a channel -------------- */
char* create_channel(struct clt* client, char* msg, struct channel* first_channel){

  char* rep = malloc(sizeof (char) * MSG_MAXLEN);

  if (strcmp(client->channel, "Server")==0){

    if ((list_channel!=NULL) && (channel_find_name(list_channel,msg)!=NULL))
      sprintf(rep, "Channel %s already exists\n", msg);

    else {
      list_channel=channel_add(list_channel, msg);
      printf(">> Number of channels : %d\n", nbre_channel(list_channel));
      sprintf(rep, "You have created channel %s\n", list_channel->name);
    }
  }

  else {
  rep= "You need to quit the channel\n";
  }

  sprintf(server_rep, " [Server]> %s\n" , rep);

  return server_rep;
}


/* -------------- Join channel -------------- */
char* join_channel(struct clt* client, char* msg, struct channel* first_channel){

  char* rep = malloc(sizeof (char) * MSG_MAXLEN);
  char* server_rep = malloc(sizeof (char) * MSG_MAXLEN);

  if (strcmp(client->channel, "Server")==0){
    if (channel_find_name(list_channel,msg)==NULL){
      sprintf(rep, "Channel %s doesn't exist\n", msg);
      sprintf(server_rep, " [Server]> %s\n" , rep);
    }
    else {
      channel=channel_find_name(list_channel,msg);
      client->channel=channel->name;
      channel->nb_members=channel->nb_members+1;
      sprintf(rep, "You have joined channel %s\n", channel->name);
      sprintf(server_rep, " [%s]> %s\n" , channel->name, rep);
      printf("%d clients in channel %s\n", channel->nb_members, channel->name);
    }
  }
  else {
    rep="You need to quit the channel\n";
    sprintf(server_rep, " [%s]> %s\n" , channel->name, rep);
  }
  return server_rep;
}

/* -------------- Join channel -------------- */
char* quit(struct clt* first_client, struct clt* client, char* msg, struct channel* first_channel){

  char* rep = malloc(sizeof (char) * MSG_MAXLEN);
  char* server_rep = malloc(sizeof (char) * MSG_MAXLEN);


  if(strcmp(client->channel, "Server")!=0) {

    if (strcmp(client->channel, msg)!=0) {
      sprintf(rep, "You can only quit the channel %s\n",client->channel);
      sprintf(server_rep, " [%s]> %s\n" , channel->name, rep);
    }

    else {
      sprintf(rep, "Channel %s quited\n", client->channel);
      sprintf(server_rep, " [Server]> %s\n", rep);
      do_write(rep, client->sockfd);
      struct channel* channel=channel_find_name(list_channel, msg);
      channel->nb_members=channel->nb_members-1;
      if (channel->nb_members==0){
        char* quit_channel=malloc(sizeof (char) *MSG_MAXLEN);
        sprintf(quit_channel,"Channel %s has been destroyed\n", channel->name);
        do_write_broadcast(0, quit_channel, first_client);
        list_channel=remove_channel(list_channel, channel);
      }
      client->channel="Server";
    }

  }
  else {
    printf("==== Close socket ==== \n"); // Marche pas
  }
  server_rep=NULL;

  return server_rep;
}