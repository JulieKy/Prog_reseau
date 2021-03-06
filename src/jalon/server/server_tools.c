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
    write(new_sock,refused,MSG_MAXLEN);
    close(new_sock);
    free(refused);
    return 0;
  }
}

/* -------------- Read what the client has to say -------------- */
char* do_read(int new_sock){
  char* buf = malloc(sizeof (char) * MSG_MAXLEN);
  memset(buf, '\0', MSG_MAXLEN);
  read(new_sock, buf, MSG_MAXLEN);
  return buf;
  }

/* -------------- We write back to the client -------------- */
void do_write(char* buf, int new_sock){
    write(new_sock,buf,MSG_MAXLEN);
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
  char* rep2 = malloc(sizeof (char) * MSG_MAXLEN);
  char* server_rep = malloc(sizeof (char) * MSG_MAXLEN);

  sscanf(buf, "%s %s" , cmd, msg);


  // first pseudo ------------------------------------------------------
  if(strcmp("psd",cmd)==0){
    rep=first_pseudo(client, msg);
    sprintf(server_rep, " [Server]> %s\n" , rep);
  }

  // nick ------------------------------------------------------------
  else if(strcmp("/nick", cmd) == 0) {
    rep=nick(client, msg);
    sprintf(server_rep, " [Server]> %s %s\n" , rep, client->psd);
  }

  // who -------------------------------------------------------------
  else if (strcmp("/who", cmd) == 0) {
      rep =who(first_client) ;
      sprintf(server_rep, " [Server]> %s\n" , rep);
  }

  // whois ---------------------------------------------------------
  else if(strcmp("/whois", cmd) == 0) {
    if (strlen(msg)==0)
      rep = "You have to specify the name of the user\n";
    else {
      rep =whois(first_client, msg);
    }
    sprintf(server_rep, " [Server]> %s\n" , rep);
  }

  // whatchannels -------------------------------------------------------------
  else if (strcmp("/whatchannels", cmd) == 0) {
      rep =what_channels(list_channel);
      sprintf(server_rep, " [Server]> %s\n" , rep);
  }

  // whochannel -------------------------------------------------------------
  else if (strcmp("/whochannel", cmd) == 0) {
    struct channel* channel=channel_find_name(list_channel, msg);
    if (channel==NULL){
      rep="This channel doesn't exist";
    }
    else {
      rep =who_channels(list_channel, msg, first_client);
    }
    sprintf(server_rep, " [Server]> %s\n" , rep);
  }

    // msgall ------------------------------------------------------
  else if(strcmp("/msgall", cmd) == 0) {
    char* msg_sent=malloc(sizeof (char) * MSG_MAXLEN);
    strcpy(msg_sent,strchr(buf,32)+1);
    do_write_broadcast(sock, msg_sent, first_client);
    server_rep=NULL;
  }

    // msg ---------------------------------------------------------
  else if(strcmp("/msg", cmd) == 0) {
    char* pseudo = malloc(sizeof (char) * MSG_MAXLEN);
    char* msg_sent=malloc(sizeof (char) * MSG_MAXLEN);
    char* msgtmp =malloc(sizeof (char) * MSG_MAXLEN);
    sscanf(buf, "%s %s" , cmd, pseudo);
    strcpy(msgtmp,strchr(buf,32)+1);
    strcpy(msg_sent,strchr(msgtmp,32)+1);
    server_rep=do_write_unicast(sock, pseudo, msg_sent, first_client, 0);
  }

  // create channel ------------------------------------------------
  else if(strcmp("/create", cmd) == 0) {

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
  }

  // join channel ------------------------------------------------
  else if(strcmp("/join", cmd) == 0) {
    server_rep=join_channel(client, msg, list_channel);
  }

  // send a file ------------------------------------------------
  else if(strcmp("/send", cmd) == 0) {

    char* mot1 = malloc(sizeof (char) * MSG_MAXLEN);
    char* mot2 = malloc(sizeof (char) * MSG_MAXLEN);
    sscanf(buf, "%s %s %s" , cmd,mot1, mot2);

    // If receiver respond for the  file transfer
    if ((strcmp("no", msg) == 0) || (strcmp("yes", msg) == 0)) {
      struct clt* rcv= client_find_pseudo(first_client, mot2);
      char* msg2 = malloc(sizeof (char) * MSG_MAXLEN);
      struct clt* sender=client_find_sock(first_client, sock);

      if (strcmp("no", msg) == 0) {
        sprintf(msg2, "%s cancelled file transfer", sender->psd);
      }

      else {

        char* port = malloc(sizeof (char) * MSG_MAXLEN);
        sscanf(buf, "%s %s %s %s" , cmd,mot1, mot2, port);

        char* msg_create_sock_file= malloc(sizeof (char) * MSG_MAXLEN);
        sprintf(msg_create_sock_file, " create_sock_file %s", port);
        do_write_unicast(sock, rcv->psd, msg_create_sock_file, first_client, 1);

        sprintf(msg2, "%s accepted file transfer", sender->psd);
        free(port);
      }

      do_write_unicast(sock, rcv->psd, msg2, first_client, 1);
      free(msg2);
    }

    // If a user want to send a file
    else {
      server_rep=send_file1(first_client, client, mot1, mot2);
    }

  free(mot1); free(mot2);
  }

    // quit --------------------------------------------------------------
    else if(strcmp("/quit",cmd)==0){
      server_rep=quit(first_client, client, msg, list_channel);
    }

  else {
    server_rep=NULL;
    // multicast --------------------------------------------------------------
    if (strcmp(client->channel, "Server")!=0){
      do_write_multicast(first_client, client, cmd, list_channel);
    }
  }

  // write back to the client --------------------------------------------------------------
  do_write(server_rep, sock);

  free(sender); free(cmd); free(rep2); free(server_rep);


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
}
