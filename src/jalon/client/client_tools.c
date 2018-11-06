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
int do_read(int sock){
  char* bufc = malloc(sizeof (char) * MSG_MAXLEN);
  bzero(bufc,MSG_MAXLEN);
  read(sock,bufc, MSG_MAXLEN);
  printf("%s\n", bufc);
  if (strcmp(bufc,"Server cannot accept incoming connections anymore. Try again")==0){
    return 1;
  }
  return 0;
}
