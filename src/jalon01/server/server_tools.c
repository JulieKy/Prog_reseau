#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/poll.h>

#include "server_strc_clt.h"
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
int do_accept(struct sockaddr_in saddr_in, int sock) {
  socklen_t length = sizeof(saddr_in);
  socklen_t* addrlen = &length;
  int new_sock;
  new_sock=accept(sock, (struct sockaddr*)&saddr_in, addrlen);
  if (new_sock < 0){
    perror("accept");
    exit(EXIT_FAILURE);
  }
  return new_sock;
}

/* -------------- Accept 20 concurrent client -------------- */
int TestTooManyC(struct pollfd* fds, int nfds, int sock, int new_sock, struct sockaddr_in saddr_in){
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
    return 0;
  }
}

/* -------------- Read what the client has to say -------------- */
char* do_read(int new_sock){
  char* buf = malloc(sizeof (char) * MSG_MAXLEN);
  char* cmd = malloc(sizeof (char) * CMD_MAXLEN);
//  while ( (strcmp(c," ")==0) || (cpt>MSG_MAXLEN) ){ // Pas du tout optimisé


  //read(new_sock, cmd, CMD_MAXLEN);
  //printf("%s", cmd);

  read(new_sock, buf, MSG_MAXLEN);

  //int to_rcv=atoi(size);
  //do{
     //nb_rcv+=read(new_sock,buf+nb_rcv, to_rcv-nb_rcv);
   //} while (nb_rcv!=to_rcv);
  // return buf;
  return buf;
  }


/* -------------- We write back to the client -------------- */
void do_write(char* buf, int new_sock){
    printf("write : %s", buf);
    write(new_sock,buf,MSG_MAXLEN);
}

// Cleanup socket
void do_close(int sock_closed, struct clt* first_client){
  printf("=== Socket %d closed === \n", sock_closed);
  // Mettre client_free(first_client,sock_closed);
  fflush(stdout);
  close(sock_closed);
  // Supprimer la socket de la structure de tableau fds
}


/* -------------- Ask the pseudo to the client at the first connection -------------- */
void logon(int new_sock, struct clt* first_client){
  char* msg_con = malloc(sizeof (char) * MSG_MAXLEN);
  char* msg_con2 = malloc(sizeof (char) * MSG_MAXLEN);
  char* cmd = malloc(sizeof (char) * MSG_MAXLEN);
  char* pseudo = malloc(sizeof (char) * MSG_MAXLEN);
  char* buf = malloc(sizeof (char) * MSG_MAXLEN);

  buf = do_read(new_sock);
  msg_con="Please logon with /nick <yourpseudo>\n";
  sscanf(buf, "%s %s" , cmd, pseudo); // dans cmd on récupère le premier mot et dans pseudo le deuxieme
  msg_con2="Hello pseudo\n";

  while ((strcmp("/nick", cmd) != 0) && (strcmp("\n", pseudo) != 0)){
    write(new_sock,msg_con,MSG_MAXLEN);
    sscanf(buf, "%s %s" , cmd, pseudo);
  }

  write(new_sock,msg_con2,MSG_MAXLEN);
  printf("cmd:%s\n",cmd);
  printf("pseudo: %s\n", pseudo);
  //il reste à mettre dans la structure le pseudo récupéré (trouver le bon client et lui rentrer le pseudo)
}


/* -------------- Test the different queries possible -------------- */
void TestCmd(char *buf){

  char* cmd = malloc(sizeof (char) * MSG_MAXLEN);
  char* msg = malloc(sizeof (char) * MSG_MAXLEN);
  int l;

  sscanf(buf, "%s %s" , cmd, msg);
  if(strcmp("/quit",cmd)==0){
    printf("recu /quit\n");
  }

  if(strcmp("/nick", cmd) == 0) {
    printf("Le client entre son pseudo\n");
    if (strcmp("", msg) ==0){
      printf("Veuillez entrer un pseudo\n"); // en vrai on doit envoyer un message pour que ça s'affiche coté client
    }
    else{
      printf("Nouveau pseudo %s\n", msg);
      // on récupère ce qu'il y a dans msg et on le met dand pseudo de la structure client
    }
  }

  if(strcmp("/who", cmd) == 0) {
    printf("Le client veut consulter la liste des clients connectés\n");
    // envoyer un message cote client pour lui donner l'intégralité des clients connectés
  }
  if(strcmp("/whois", cmd) == 0) {
    // envoyer un message cote client pour lui donner les informations sur le client qu'il demande
  }
}
