#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define MSG_MAXLEN 60

// Prototypes
int do_socket();
struct sockaddr_in init_host_addr(char*,int);
void do_connect (struct sockaddr_in, int);
char* readline(int);
void handle_client_message(char*, int);
void handle_server_message(int);
void do_close(msg, sock);

// Main
int main(int argc,char** argv)
{

    if (argc != 3)
    {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        return 1;
    }

    //get address info from the server
    //get_addr_info()
    char* sv_addr=argv[1]; // adresse du serveur (127.0.0.1)
    short n_port=atoi(argv[2]); //numéro de port de la socket côté serveur (quand connect)

    int sock=do_socket();
    struct sockaddr_in sock_host=init_host_addr(sv_addr, n_port);
    do_connect(sock_host, sock);
    for (;;){
      char* msg=readline(sock);
      handle_client_message(msg, sock);
      handle_server_message(sock);
    }

    return 0;
}

// Get the socket
int do_socket(){
  int sock;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock==-1){
    perror("socket");
    exit(EXIT_FAILURE);
  }
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
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
  printf("Sending : ");
  fgets(msg,MSG_MAXLEN,stdin); // 80 pour avoir une ligne
  return msg;
}

// Send message to the server
void handle_client_message(char* msg, int sock){
  int sent=0, to_send=strlen(msg);
  //sent= write(sock,to_send,4);
  //attendre que message recu  A CHANGER !!
  sent= write(sock,msg,strlen(msg));
  //printf("-> Sending : %s\n", msg);
  if (strcmp(msg,"/quit\n")==0)
    do_close(msg, sock);
}

// Read what the client has to say
void handle_server_message(int sock){
  char* bufc = malloc(sizeof (char) * MSG_MAXLEN);
  bzero(bufc,MSG_MAXLEN);
  int nb_rcv =0;
  int to_rcv=strlen(bufc);
  //do{
    nb_rcv+=read(sock,bufc+nb_rcv, strlen(bufc)-nb_rcv);// PAS DU TOUT SUR QUE CE SOIT CA
    read(sock,bufc, MSG_MAXLEN);
  //} while (nb_rcv!=to_rcv);
  printf("Receiving : %s\n", bufc);
}

void do_close(char* msg, int sock){
  if (strcmp(msg,"/quit\n")==0){
    printf(" === Socket closed ===\n");
    close(sock);
    exit(EXIT_SUCCESS);
    free(msg);
  }
}
