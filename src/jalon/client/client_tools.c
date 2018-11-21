#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "client_tools.h"

#define MSG_MAXLEN 200


/* -------------- Get the socket -------------- */
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


/* -------------- Init the sock_host structure -------------- */
struct sockaddr_in init_host_addr(char* sv_addr, int sv_port){

  struct sockaddr_in sock_host;
  memset(& sock_host, '\0', sizeof(sock_host));
  sock_host.sin_family=AF_INET;
  sock_host.sin_port=htons(sv_port);
  int sv_addr_bin = inet_aton(sv_addr, &sock_host.sin_addr);

  return sock_host;
}


/* -------------- Connect to remote socket -------------- */
void do_connect (struct sockaddr_in sock_host, int sock){
  int con=connect(sock,(struct sockaddr*)&sock_host, sizeof(sock_host));
  if (con!=0){
    perror("connection");
    exit(EXIT_FAILURE);
  }
  printf("Connecting to server ... done!\n\n");
}


/* -------------- Perform the binding  -------------- */
void do_bind(int sock, struct sockaddr_in saddr_in){
  int i_bind = bind (sock, (struct sockaddr*)&saddr_in, sizeof(saddr_in));
  if (i_bind == -1){
      perror("bind");
      exit(EXIT_FAILURE);
    }
  }


  /* -------------- Listen  -------------- */
  void do_listen(int sock, struct sockaddr_in saddr_in){
    int i_listen=listen(sock,20);
    if (i_listen == -1) {
      perror("listen");
      exit(EXIT_FAILURE);
    }
  }

  /* -------------- Accept connection -------------- */
  int do_accept(struct sockaddr_in saddr_in, int sock) {
    socklen_t length = sizeof(saddr_in);
    socklen_t* addrlen = &length;
    int new_sock=accept(sock, (struct sockaddr*)&saddr_in, addrlen); // Surement pas les bons paramètres (il faut mettre ceux clients)
    if (new_sock < 0){
      perror("accept");
      exit(EXIT_FAILURE);
    }
    return new_sock;
  }

/* -------------- Create listenning socket -------------- */
int create_listenning_socket(char* sv_addr, int port) {
// Création d'une socket d'écoute
  int sockfd=do_socket();
  struct sockaddr_in saddr_in = init_host_addr(sv_addr,port);
  do_bind(sockfd, saddr_in);

  // Acceptation
  printf("listenning...\n");
  do_listen(sockfd, saddr_in);
  int new_sock=do_accept(saddr_in,sockfd);
  printf(">> La sockfd du client qui va nous envoyer le fichier est: %d\n", new_sock);

  return sockfd;
}


/* -------------- Send the first pseudo to the server -------------- */
void send_first_pseudo(int sock){

  char* buf = malloc(sizeof (char) * MSG_MAXLEN);
  memset(buf, '\0', MSG_MAXLEN);

  char cmd[MSG_MAXLEN];
  char pseudo[MSG_MAXLEN];
  //char psd[MSG_MAXLEN];

  printf(">> Please login with /nick <yourpseudo>.\n");
  buf=readline(sock);
  sscanf(buf, "%s %s" , cmd, pseudo);
  //send_pseudo(cmd, pseudo, sock);

  while ((strcmp("/nick", cmd) != 0) || (strlen(pseudo)<2)){
    printf("[Server] : Please logon with /nick <yourpseudo>. Pseudo size: more than 1 letter/\n\n");
    buf=readline(sock);
    sscanf(buf, "%s %s" , cmd, pseudo);
    //sprintf(psd, "%s %s\n" , cmd, pseudo);
  }
  char cmd_pseudo[MSG_MAXLEN]= "psd";
  char* msg_con = malloc(sizeof (char) * MSG_MAXLEN);
  sprintf(msg_con, "%s %s" , cmd_pseudo, pseudo);
  write(sock,msg_con,MSG_MAXLEN);
  memset(msg_con, '\0', MSG_MAXLEN);
}

/* -------------- Get user input -------------- */
char* readline(int sock){
  char* msg = malloc(sizeof (char) * MSG_MAXLEN);
  memset(msg, '\0', MSG_MAXLEN);
  fgets(msg,MSG_MAXLEN,stdin);
  return msg;
}


/* -------------- Send message to the server -------------- */
void do_write(char* msg, int sock){
  int sent=0, msg_intsize=strlen(msg);
  //memset(sock, '\0', MSG_MAXLEN);
  sent= write(sock,msg,strlen(msg));
}


/* -------------- Read a message from the server -------------- */
char* do_read(int sock, int in){

  char* bufc = malloc(sizeof (char) * MSG_MAXLEN);
  char* mot1 = malloc(sizeof (char) * MSG_MAXLEN);
  char* mot2 = malloc(sizeof (char) * MSG_MAXLEN);
  char* mot3= malloc(sizeof (char) * MSG_MAXLEN);
  char* rep = malloc(sizeof (char) * MSG_MAXLEN);
  char* rep2 = malloc(sizeof (char) * MSG_MAXLEN);

  bzero(bufc,MSG_MAXLEN);
  read(sock,bufc, MSG_MAXLEN);

  // Too many clients connected ------------------------------------------------------------
  if (strcmp(bufc,"Server cannot accept incoming connections anymore. Try again")==0){
    printf("%s\n", bufc);
    return "too many clients";
  }

  // If received question send file, then need to answer y or n ----------------------------
  sscanf(bufc, "%s %s", mot1, mot2);
  if ((strcmp(mot1,"[Server]>")==0)&&(strcmp(mot2,"?")==0)) {
    sscanf(bufc, "%s %s %s", mot1, mot2, mot3);

    printf("%s", bufc);  // Enlever le point d'interrogation !!

    char* rep2=readline(in);

    if ((strcmp(rep2,"y\n")!=0)&&(strcmp(rep2,"n\n")!=0)) {
      rep2=answer_send_file(sock, in);
    }
    sprintf(rep, "%s %s", rep2, mot3);

    return rep;
  }

  // Message d'acceptation du transfert de fichier -----------------------------------------
  if ((strcmp(mot1,"[Server]>")==0)&&(strcmp(mot2,"accepted")==0)) {
    sprintf(rep, "%s %s", rep2, mot3);

    return rep;
  }

  // Création d'un socket pour envoyer un fichier à un autre client -------------------------
  if ((strcmp(mot1,"[Server]>")==0)&&(strcmp(mot2,"create_sock_file")==0)) {
    sscanf(bufc, "%s %s %s", mot1, mot2, mot3);
    int port=atoi(mot3);
    printf("j'ai recuperer le num de port !! = %d\n", port);
    rep="socket_c2";
    
    return rep;
    // création de la socket !! ///
  }

  printf("%s\n", bufc);
  return "simple_read";
}

/* -------------- Answer a yes/no question -------------- */
char* answer_send_file(int sock, int in){

  char* rep = malloc(sizeof (char) * MSG_MAXLEN);

  do {

    printf("[Server]> [y/n] is required\n");
    bzero(rep,MSG_MAXLEN);
    rep=readline(in);

  } while ((strcmp(rep,"y\n")!=0)&&(strcmp(rep,"n\n")!=0));

  return rep;

}

/* -------------- Answer no (receiving file) -------------- */
void file_answer(char* psd_sender, int fd, char* rep, int port) {

  char* msg = malloc(sizeof (char) * 60);

  if (strcmp(rep, "no")==0) {
    sprintf(msg,"/send no %s",psd_sender);
  }

  if (strcmp(rep, "yes")==0) {
    sprintf(msg,"/send yes %s %d",psd_sender, port);
  }

  do_write(msg, fd);
  free(msg);
}
