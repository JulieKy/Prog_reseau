#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

// Prototypes
int do_socket();
struct sockaddr_in init_host_addr(char*,int);
void do_connect (struct sockaddr_in, int);
char* readline(int);
void handle_client_message(char*, int);
void handle_server_message(int);

int main(int argc,char** argv)
{


    if (argc != 3)
    {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        return 1;
    }

    //get address info from the server
    //get_addr_info()
    char* sv_addr=argv[1]; // adresse du serveur
    short n_port=atoi(argv[2]); //numéro de port de la socket côté serveur (quand connect)

    int sock=do_socket();
    struct sockaddr_in sock_host=init_host_addr(sv_addr, n_port);
    do_connect(sock_host, sock);
    char* msg=readline(sock);
    handle_client_message(msg, sock);
    handle_server_message(sock);

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
}

// Get user input
char* readline(int sock){
  char* msg = malloc(sizeof (char) * 30);
  //bzero(msg, 30);
  printf("Que voulez-vous envoyer au serveur?\n");
  fgets(msg,30,stdin); // 30 pour avoir une ligne
  if (strcmp(msg,"/quit")==0){
    close(sock);
  }
  return msg;
}

// Send message to the server
void handle_client_message(char* msg, int sock){
  int sent=0, to_send=strlen(msg);
  do{
    sent+= write(sock,msg+sent,strlen(msg)-sent);
  } while (sent!=to_send);
}

//read what the client has to say
//do_read()
//char buf[30];
void handle_server_message(int sock){
char* bufc = malloc(sizeof (char) * 30);
bzero(bufc,30);
int nb_rcv =0;
int to_rcv=strlen(bufc);
//do{
  nb_rcv+=read(sock,bufc+nb_rcv, strlen(bufc)-nb_rcv);// PAS DU TOUT SUR QUE CE SOIT CA
  read(sock,bufc, 30);
  printf("readclient\n");
//} while (nb_rcv!=to_rcv);



printf("Le message est : %s", bufc);
}
