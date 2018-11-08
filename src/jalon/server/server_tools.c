#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/poll.h>

#include "channel_tools.h"
#include "server_tools.h"

#define MSG_MAXLEN 200
#define CMD_MAXLEN 10

/* -------------- Create the socket and check the validity -------------- */
int do_socket(){
  int sock;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
  if (sock==-1){
    perror("socket");
    exit(EXIT_FAILURE);
  }
  return sock;
}

/* -------------- Init the serv_add structure -------------- */
struct sockaddr_in init_serv_addr(int sv_port){
  struct sockaddr_in saddr_in;
  memset(&saddr_in,0,sizeof(saddr_in));
  saddr_in.sin_family=AF_INET;
  saddr_in.sin_port=htons(sv_port);
  saddr_in.sin_addr.s_addr=INADDR_ANY;
  return saddr_in;
}

/* -------------- Perform the binding -------------- */
void do_bind(int sock, struct sockaddr_in saddr_in){
  int i_bind = bind (sock, (struct sockaddr*)&saddr_in, sizeof(saddr_in));
  if (i_bind == -1){
      perror("bind");
      exit(EXIT_FAILURE);
    }
  }

/* -------------- Listen to clients -------------- */
void do_listen(int sock, struct sockaddr_in saddr_in){
  int i_listen=listen(sock,20); // Peut être mettre SOMAXCONN
  if (i_listen == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
}

/* -------------- Accept connection from client -------------- */
int do_accept(struct sockaddr_in* saddr_in, int sock) {
  socklen_t length = sizeof(saddr_in);
  socklen_t* addrlen = &length;
  int new_sock;
  new_sock=accept(sock, (struct sockaddr*)saddr_in, addrlen);
  if (new_sock < 0){
    perror("accept");
    exit(EXIT_FAILURE);
  }
  return new_sock;
}

/* -------------- Accept 20 concurrent client -------------- */
int test_nb_users(struct pollfd* fds, int nfds, int sock, int new_sock, struct sockaddr_in saddr_in){
  if (nfds<20){
    fds[nfds].fd = new_sock;
    fds[nfds].events = POLLIN;
    nfds++;
    return nfds;
  }
  else {
    char* refused = malloc(sizeof (char) * (100));
    refused="Server cannot accept incoming connections anymore. Try again later.";
    //memset(new_sock, '\0', MSG_MAXLEN);
    write(new_sock,refused,MSG_MAXLEN);
    close(new_sock);
    return 0;
  }
}

/* -------------- Read what the client has to say -------------- */
char* do_read(int new_sock){
  char* buf = malloc(sizeof (char) * MSG_MAXLEN);
  memset(buf, '\0', MSG_MAXLEN);
  char* cmd = malloc(sizeof (char) * CMD_MAXLEN);
  read(new_sock, buf, MSG_MAXLEN);
  return buf;
  }

/* -------------- We write back to the client -------------- */
void do_write(char* buf, int new_sock){
    printf("[write] : %s\n", buf);
    write(new_sock,buf,MSG_MAXLEN);
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

/* -------------- Write a broadcast message -------------- */
void do_write_multicast(struct clt* first_client, struct clt* client, char* msg, struct channel* first_channel){

    struct channel* channel= channel_find_name(first_channel, client->channel);

    char* name=client->psd;
    char* msg_pseudo = malloc(sizeof (char) * MSG_MAXLEN);
    sprintf(msg_pseudo, "[%s]: %s\n" , name, msg);
    printf("[write multicast] : %s\n", msg_pseudo);

    struct clt* temp=first_client;
    while (temp!=NULL){
      if (client->sockfd!=temp->sockfd) {
        write(temp->sockfd, msg_pseudo, MSG_MAXLEN);
      }
    temp=temp->next;
    }
}

/* -------------- Write a unicast message -------------- */
char* do_write_unicast(int sock, char* msg, struct clt* first_client){
    // Find the receiver's pseudo and the message
    char* rcver_psd= malloc(sizeof (char) * MSG_MAXLEN);
    char* message= malloc(sizeof (char) * MSG_MAXLEN);
    char* rep= malloc(sizeof (char) * MSG_MAXLEN);

    sscanf(msg, "%s" , rcver_psd);
    sprintf(message, "%s", msg+sizeof(rcver_psd));
    printf("msg=%s\n",message);

    // Find the receiver
    struct clt* rcver=client_find_pseudo(first_client, rcver_psd);

    if (rcver==NULL){
      rep = "This user doesn't exist.";
      printf("pas ce pseudo, rep =%s\n", rep);
    }

    else {
      rep=NULL;

      // Recovery of the sender's pseudo to send : "[User0]: msg"
      struct clt* sender=client_find_sock(first_client, sock);
      char* psd=sender->psd;
      char* msg_pseudo = malloc(sizeof (char) * MSG_MAXLEN);
      sprintf(msg_pseudo, "[%s]: %s\n" , psd, message);

      //Write to the receiver
      write(rcver->sockfd, msg_pseudo, MSG_MAXLEN);
    }

    return rep;
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
        sprintf(pseudo, "             - %s\n",client_found->psd);
        strcat(list_pseudo, pseudo);
    }
    client=client->next;
  }
    return list_pseudo;
  }
}


/* -------------- Test the different queries when client isn't in a channel -------------- */
struct channel* treat_writeback(char *buf, struct clt* first_client, int sock, struct channel* first_channel) {

  struct channel* list_channel= first_channel;
  struct clt* client=client_find_sock(first_client, sock);
  struct channel* channel=channel_find_name(list_channel, client->channel);

  char* sender = malloc(sizeof (char) * MSG_MAXLEN);
  strcpy(sender, client->channel);
  char* cmd = malloc(sizeof (char) * MSG_MAXLEN);
  char* msg = malloc(sizeof (char) * MSG_MAXLEN);
  char* rep = malloc(sizeof (char) * MSG_MAXLEN);
  char* server_rep = malloc(sizeof (char) * MSG_MAXLEN);

  sscanf(buf, "%s %s" , cmd, msg);


  // first pseudo ------------------------------------------------------
  if(strcmp("psd",cmd)==0){
    client->psd=msg;
    //strcpy(client->psd,msg);
    printf(">> Le pseudo du client %d est : %s\n",sock, client->psd);
    char welcome[MSG_MAXLEN] = "Welcome on the chat";
    sprintf(server_rep, "[Server] : %s %s\n" , welcome, client->psd);
  }

  // nick ------------------------------------------------------------
  else if(strcmp("/nick", cmd) == 0) {
    if (strlen(msg)<2){
      rep ="Your pseudo must be 2 letters long\n";
      sprintf(server_rep, "[%s]> %s", sender, rep);
    }
    else{
      client->psd=msg;
      //strcpy(client->psd,msg);
      printf(">> Le nouveau pseudo du client %d est : %s\n",sock, client->psd);
      rep = "Your new pseudo is";
      sprintf(server_rep, "[Server] : %s %s\n" , rep, client->psd);
    }
  }

  // who -------------------------------------------------------------
  else if (strcmp("/who", cmd) == 0) {
      rep =who(first_client) ;
      sprintf(server_rep, "[%s]> %s",sender, rep);
  }

  // whatchannels -------------------------------------------------------------
  else if (strcmp("/whatchannels", cmd) == 0) {
      rep =what_channels(list_channel);
      sprintf(server_rep, "[%s]> %s",sender, rep);
  }

  // whochannel -------------------------------------------------------------
  else if (strcmp("/whochannel", cmd) == 0) {
      struct channel* channel=channel_find_name(list_channel, msg);
      if (channel==NULL){
        sprintf(server_rep, "[%s]> This channel doesn't exist\n",sender);
      }
      else {
        server_rep =who_channels(list_channel, msg, first_client);
      }
  }

  // whois ---------------------------------------------------------
  else if(strcmp("/whois", cmd) == 0) {
    if (strlen(msg)==0)
      rep = "You have to specify the name of the user\n";
    else {
      rep =whois(first_client, msg);
    }
    sprintf(server_rep, "[%s]> %s", sender, rep);
  }

    // msgall ------------------------------------------------------
  else if(strcmp("/msgall", cmd) == 0) {
    printf(">> Broadcast\n");
    do_write_broadcast(sock, msg, first_client);
    server_rep=NULL;
  }

    // msg ---------------------------------------------------------
  else if(strcmp("/msg", cmd) == 0) {
    printf(">> Unicast\n");
    server_rep=do_write_unicast(sock, msg, first_client);
  }

  // create channel ------------------------------------------------
  else if(strcmp("/create", cmd) == 0) {
    if (strcmp(client->channel, "Server")==0){
      if ((list_channel!=NULL) && (channel_find_name(list_channel,msg)!=NULL))
        sprintf(server_rep, "[Server]: Channel %s already exists\n", msg);
      else {
        list_channel=channel_add(list_channel, msg);
        printf(">> Number of channels : %d\n", nbre_channel(list_channel));
        sprintf(server_rep, "[Serveur]: You have created channel %s\n", list_channel->name);
      }
    }
    else {
    sprintf(server_rep, "[%s]> You need to quit the channel\n", sender);
    }
  }

  // join channel ------------------------------------------------
  else if(strcmp("/join", cmd) == 0) {
    printf("message : %s\n",msg );
    if (strcmp(client->channel, "Server")==0){
      if (channel_find_name(list_channel,msg)==NULL){
        printf("coucou\n" );
        sprintf(server_rep, "[Server]: Channel %s doesn't exist\n", msg);
      }
      else {
        channel=channel_find_name(list_channel,msg);
        client->channel=channel->name;
        channel->nb_members=channel->nb_members+1;
        sprintf(server_rep, "[%s]> You have joined channel %s\n", channel->name, channel->name);
        printf("%d clients in channel %s\n", channel->nb_members, channel->name);
      }
    }
    else {
      sprintf(server_rep, "[%s]> You need to quit the channel\n", sender);
    }
  }

    // quit --------------------------------------------------------------
    else if(strcmp("/quit",cmd)==0){

      if(strcmp(client->channel, "Server")!=0) {

        if (strcmp(client->channel, msg)!=0) {
          sprintf(server_rep, "You can only quit the channel %s\n",client->channel);
        }

        else {
          sprintf(rep, "[Server]> Channel %s quited\n", client->channel);
          do_write(rep, sock);
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
    }

  else {
    server_rep=NULL;
    // Write multicast
    if (strcmp(client->channel, "Server")!=0){
      do_write_multicast(first_client, client, cmd, list_channel);
    }
  }

  do_write(server_rep, sock);

  return list_channel;
}



/* -------------- Cleanup socket -------------- */
void do_close(int sock_closed, struct clt* first_client){
  printf("=== Socket %d closed === \n", sock_closed);
  fflush(stdout);
  close(sock_closed);
  struct clt* removed_client;
  removed_client=client_find_sock(first_client,sock_closed);
  first_client=remove_client(first_client, removed_client);
  int nb_clt= nbre_client(first_client);
  printf(">>> Nombre de client connecté au serveur : %d\n",nb_clt);
  // Supprimer la socket de la structure de tableau fds
}
