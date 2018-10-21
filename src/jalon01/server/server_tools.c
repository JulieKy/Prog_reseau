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

// /* -------------- Ask clients to give their pseudo -------------- */
// void ask_pseudo(int new_sock){
//   char* msg_pseudo = malloc(sizeof (char) * MSG_MAXLEN);
//   msg_pseudo="pseudo";
//   write(new_sock,msg_pseudo,MSG_MAXLEN);
//   //memset(new_sock, '\0', MSG_MAXLEN);
// }


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
    printf("write : %s\n", buf);
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


/* -------------- Test the different queries -------------- */
char* test_cmd(char *buf, struct clt* first_client, int sock){

  // test_cmd is 1 if a query is sent by the client, 0 otherwise.
  int test_cmd;
  test_cmd=1;

  char* cmd = malloc(sizeof (char) * MSG_MAXLEN);
  char* msg = malloc(sizeof (char) * MSG_MAXLEN);
  char* rep = malloc(sizeof (char) * MSG_MAXLEN);

  sscanf(buf, "%s %s" , cmd, msg);

  if(strcmp("/quit",cmd)==0){
    printf("recu /quit\n");
  }

  else if(strcmp("first_pseudo",cmd)==0){
    struct clt* client=client_find(first_client, sock);
    client->psd=msg;
    printf(">> Le pseudo du client %d est : %s\n",sock, client->psd);
    char welcome[MSG_MAXLEN] = "Welcome on the chat";
    sprintf(rep, "%s %s\n" , welcome, client->psd);
  }

  else if(strcmp("/nick", cmd) == 0) {
    printf("Le client entre son pseudo\n");
    if (strlen(msg)<2){
      rep ="Your pseudo must be 2 letters long\n";
    }
    else{
      struct clt* client=client_find(first_client, sock);
      client->psd=msg;
      printf(">> Le nouveau pseudo du client %d est : %s\n",sock, client->psd);
      char a[MSG_MAXLEN] = "Your new pseudo is";
      sprintf(rep, "%s %s\n" , a, client->psd);
    }
  }

  else if (strcmp("/who", cmd) == 0) {
    printf("Le client veut consulter la liste des clients connectés\n");
    if (strlen(msg)!=0)
      rep = "To have the user list use /who and to have information about one user use /whois <username>\n";
    else {
      rep =who(first_client) ;
    }
  }
  else if(strcmp("/whois", cmd) == 0) {
    // envoyer un message cote client pour lui donner les informations sur le client qu'il demande
  }

  else
    rep=buf;

  return rep;

}
