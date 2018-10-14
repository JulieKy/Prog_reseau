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

// Get the socket
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

// Init the sock_host structure
struct sockaddr_in init_host_addr(char* sv_addr, int sv_port){

  struct sockaddr_in sock_host;
  memset(& sock_host, '\0', sizeof(sock_host));
  sock_host.sin_family=AF_INET;
  sock_host.sin_port=htons(sv_port);
  int sv_addr_bin = inet_aton(sv_addr, &sock_host.sin_addr);

  return sock_host;
}

// Connect to remote socket
void do_connect (struct sockaddr_in sock_host, int sock){
  int con=connect(sock,(struct sockaddr*)&sock_host, sizeof(sock_host));
  if (con!=0){
    perror("connection");
    exit(EXIT_FAILURE);
  }
  printf("Connecting to server ... done!\n");
}

// Get user input
char* readline(int sock){
  char* msg = malloc(sizeof (char) * MSG_MAXLEN);
  memset(msg, '\0', MSG_MAXLEN);
  printf("\n------- \nSending : ");
  fgets(msg,MSG_MAXLEN,stdin);
  return msg;
}

// char size_msg[MSG_MAXLEN];
  // sprintf(size_msg, "%d", msg_intsize);
  // strcat(size_msg, "|");
  // strcat(size_msg, msg);
  // printf("%s\n",size_msg);
  // sent= write(sock,size_msg,strlen(msg));

// Send message to the server
void handle_client_message(char* msg, int sock){
   int sent=0, msg_intsize=strlen(msg);
  sent= write(sock,msg,strlen(msg));
}

// Read what the client has to say
int handle_server_message(int sock){
  char* bufc = malloc(sizeof (char) * MSG_MAXLEN);
  bzero(bufc,MSG_MAXLEN);
  read(sock,bufc, MSG_MAXLEN);
  printf("\nReceiving : \n   [Server] : %s\n", bufc);
  if (strcmp(bufc,"Server cannot accept incoming connections anymore. Try again")==0){
    return 1;
  }
  return 0;
}
