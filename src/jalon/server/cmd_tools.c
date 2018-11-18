#include "cmd_tools.h"


#define MSG_MAXLEN 200
#define CMD_MAXLEN 10

/* -------------- First pseudo -------------- */
char* first_pseudo(struct clt* client, char* msg){

  char* rep = malloc(sizeof (char) * MSG_MAXLEN);
  char* rep2 = malloc(sizeof (char) * MSG_MAXLEN);

  client->psd=msg;
  //strcpy(client->psd,msg);
  printf(">> Le pseudo du client %d est : %s\n",client->sockfd, client->psd);
  rep2 = "Welcome on the chat";
  sprintf(rep, "  %s %s\n" , rep2, client->psd);
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
    printf(">> Le nouveau pseudo du client %d est : %s\n",client->sockfd, client->psd);
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



/* -------------- Give the channels created -------------- */
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


/* -------------- Create the list of members in a channel -------------- */
char* who_channels(struct channel* list_channel, char * msg, struct clt* list_client) {

  struct channel* channel=channel_find_name(list_channel, msg);
  struct clt* client=list_client;
  char* list_pseudo=malloc(sizeof (char) *200);

  if (channel==NULL) {
    list_pseudo = "This channel doesn't exist\n";
  }

  else {

    if (client_find_channel(client, channel->name) ==NULL){
      list_pseudo="No members in this channel";
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
char* do_write_unicast(int sock, char* pseudo, char* msg, struct clt* first_client, int test_send){
    // Find the receiver's pseudo and the message
    char* rcver_psd= malloc(sizeof (char) * MSG_MAXLEN);
    char* message= malloc(sizeof (char) * MSG_MAXLEN);
    char* rep= malloc(sizeof (char) * MSG_MAXLEN);

    // Find the receiver
    printf("je cherche\n");
    struct clt* rcver=client_find_pseudo(first_client, pseudo);
    printf("j'ai trouvé\n");

    if (rcver==NULL){
      rep = "This user doesn't exist.";
    }

    else {
      rep=NULL;

      // Recovery of the sender's pseudo to send : "[User]: msg"
      struct clt* sender=client_find_sock(first_client, sock);
      char* psd=sender->psd;
      char* msg_pseudo = malloc(sizeof (char) * MSG_MAXLEN);
      if (test_send==0) {
        sprintf(msg_pseudo, "[%s]> %s\n" , psd, msg);
      }
      else {
        sprintf(msg_pseudo, "[Server]> %s\n" , msg);
      }


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
char* create_channel(struct clt* client, char* msg, struct channel* list_channel){

  char* rep = malloc(sizeof (char) * MSG_MAXLEN);
  char* server_rep = malloc(sizeof (char) * MSG_MAXLEN);

  if (strcmp(client->channel, "Server")==0){

    if ((list_channel!=NULL) && (channel_find_name(list_channel,msg)!=NULL))
      sprintf(rep, "Channel %s already exists\n", msg);

    else {
      list_channel=channel_add(list_channel, msg);
      printf("pseudo channel =%s\n", list_channel->name);
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
char* join_channel(struct clt* client, char* msg, struct channel* list_channel){

  char* rep = malloc(sizeof (char) * MSG_MAXLEN);
  char* server_rep = malloc(sizeof (char) * MSG_MAXLEN);
  struct channel* channel= malloc(sizeof (struct channel) * MSG_MAXLEN);

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


/* -------------- Send a file  -------------- */
char* send_file(struct clt* first_client, struct clt* client, char* user_rcv, char* file) {

  char* rep = malloc(sizeof (char) * MSG_MAXLEN);
  char* server_rep = malloc(sizeof (char) * MSG_MAXLEN);
  char* rep_user = malloc(sizeof (char) * MSG_MAXLEN);
  char* file_name = malloc(sizeof (char) * MSG_MAXLEN);
  char* msg = malloc(sizeof (char) * MSG_MAXLEN);

  // Si pseudo n'existe pas
  if (client_find_pseudo(first_client, user_rcv)==NULL) {
    sprintf(rep, "Pseudo doesn't exist");
    sprintf(server_rep, " [Server]> %s\n" , rep);
    return server_rep;
  }

  // // Si mauvais chemin
  // printf("file = %s\n", file); /*ENLEVER "" avant open ? */
  // int ficfd;
  // if ((ficfd=open(file, O_RDONLY))== -1){ // N'existe pas.. Probleme avec open
  //   sprintf(rep, "File doesn't exist");
  //   sprintf(server_rep, " [Server]> %s\n" , rep);
  //   return server_rep;
  // }

  // Verif du nom du file et du user

  // // Recupération du nom du fichier
  // char c="";
  // char tab[];
  // while (strcmp(c, '\\')!=0){
  //   c=file[strlen(file)-i];
  //   tab[i]=c;
  //   i++;
  // }

  file_name=file; // A changer pour avoirjuste le nom et pas tout le chemin
  sprintf(msg, "? %s wants you to accept the transfer of the file named \"%s\". Do you accept? [y/n]", client->psd, file_name);
  do_write_unicast(client->sockfd, user_rcv, msg, first_client, 1);
  // nouvelle fonction qui attend reponse et qui la renvoie
  if (strcmp(rep_user, "Y")==0) {
    // Envoyer message à C1 pour création socket + envoi du fichier
    sprintf(rep, "%s accepted file transfert.", user_rcv);
  }
  else if (strcmp(rep_user, "n")==0) {
    sprintf(rep, "%s cancelled file transfer", user_rcv);
  }

  sprintf(server_rep, " [Server]> %s\n" , rep);
  return server_rep;
}


/* -------------- Quit channel -------------- */
char* quit(struct clt* first_client, struct clt* client, char* msg, struct channel* list_channel){

  char* rep = malloc(sizeof (char) * MSG_MAXLEN);
  char* server_rep = malloc(sizeof (char) * MSG_MAXLEN);

  if(strcmp(client->channel, "Server")!=0) {

    if (strcmp(client->channel, msg)!=0) {
      sprintf(rep, "You can only quit the channel %s\n",client->channel);
      sprintf(server_rep, " [%s]> %s\n" , client->channel, rep);
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
