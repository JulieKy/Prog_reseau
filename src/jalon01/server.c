#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/poll.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

#define MSG_MAXLEN 200

// Décalaration des prototypes de fonctions
int do_socket();
struct sockaddr_in init_serv_addr(int);
void do_bind(int, struct sockaddr_in);
void do_listen(int, struct sockaddr_in);
int do_accept(struct sockaddr_in, int);
char* do_read(int);
void do_write(char*, int);
void do_close(char*, int);

// Main
int main(int argc, char** argv) {
  if (argc != 2) {
      fprintf(stderr, "usage: RE216_SERVER port\n"); // Il faut donner le port de la socket coté client ??
      return 1;
  }

  int sv_port=atoi(argv[1]);
  int sock=do_socket();
  struct sockaddr_in saddr_in=init_serv_addr(sv_port);
  do_bind(sock, saddr_in);
  do_listen(sock, saddr_in);

  char* buf;
  int res;


  struct pollfd fds[20];
  int n;
  int nfds=1;  // Peut être de type nfds_t
  int timeout=-1;

  memset(fds, 0 , sizeof(fds));

  // Initialisation du tableau avec la première socket serveur en activité
  fds[0].fd=sock;
  fds[0].events = POLLIN;

  for(;;)
  {
    // Poll
    printf("\n\ninf\n");
    n=poll(fds, nfds, timeout); // Donne le nombre de socket en activité

    int current_size = nfds;

    if(fds[0].revents == POLLIN) {// je ne sais pas si c'est events ou revents qu'il faut mettre
      printf("Nouveau client essaie de se connecter\n");

      if (nfds<2){ // Est ce que c'est bien n ?
        int new_sock=do_accept(saddr_in,sock);
        printf("etape : accept\n");
        fds[nfds].fd = new_sock;
        fds[nfds].events = POLLIN;
        nfds++;
      }
      else
        printf("Le serveur ne peut pas accepter plus de 20 connections en même temps");
    }

    // Test pour trouver les sockets en activité
    for(int i=0; i<current_size ; i++){

      if ( i!=0 && fds[i].revents == POLLIN){
        char* buf;
        //int res;
        buf=do_read(fds[i].fd);
        do_close(buf, fds[i].fd);
        do_write(buf, fds[i].fd);
      }
    }
  }

  return 0;
}

// Create the socket and check the validity
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


// Init the serv_add structure
struct sockaddr_in init_serv_addr(int sv_port){

  struct sockaddr_in saddr_in;
  memset(&saddr_in,0,sizeof(saddr_in));
  saddr_in.sin_family=AF_INET;
  saddr_in.sin_port=htons(sv_port);
  saddr_in.sin_addr.s_addr=INADDR_ANY;
  return saddr_in;
}

// Perform the binding (we bind on the tcp port specified)
void do_bind(int sock, struct sockaddr_in saddr_in){
  int i_bind = bind (sock, (struct sockaddr*)&saddr_in, sizeof(saddr_in));
  if (i_bind == -1){
      perror("bind");
      exit(EXIT_FAILURE);
    }
  }

// Specify the socket to be a server socket and listen for at most 20 concurrent client
void do_listen(int sock, struct sockaddr_in saddr_in){
  int i_listen=listen(sock,20); // Peut être mettre SOMAXCONN
  if (i_listen == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
}


// Accept connection from client
int do_accept(struct sockaddr_in saddr_in, int sock) {
  socklen_t length = sizeof(saddr_in);
  socklen_t* addrlen = &length;
  int new_sock;
  new_sock=accept(sock, (struct sockaddr*)&saddr_in, addrlen); // Surement pas les bons paramètres (il faut mettre ceux clients)
  printf("num_sock=%d\n",new_sock);
  if (new_sock < 0){
    perror("accept");
    exit(EXIT_FAILURE);
  }
  return new_sock;
}

// Read what the client has to say
char* do_read(int new_sock){
  char* buf = malloc(sizeof (char) * MSG_MAXLEN);
  char* size = malloc(sizeof (char) * MSG_MAXLEN);
  bzero(buf, MSG_MAXLEN);
  int nb_rcv =0;
  read(new_sock,buf, MSG_MAXLEN);
  strncpy(size, buf, 10);
  int to_rcv=atoi(size);
  printf("Final copied string : %d\n", to_rcv);
  //    do{
  //    nb_rcv+=read(new_sock,buf+nb_rcv, to_rcv-nb_rcv);// PAS DU TOUT SUR QUE CE SOIT CA
  //    printf("read\n");
  //  } while (nb_rcv!=to_rcv);
   return buf;
  }

  //clean up client socket
// Clean up server socket
void do_close(char* buf, int new_sock){
  //printf("retour strcmp : %d\n", strcmp(buf,"/quit")-10);
  if (strcmp(buf,"/quit")==0){
    printf("La socket côté serveur est fermée");
    fflush(stdout);
    close(new_sock);
    free(buf);
  }
}


// We write back to the client
void do_write(char* buf, int new_sock){
  //send(sock, &msg, ,0);
  int sent=0;
  // int to_send=strlen(&buf);
  //do{
    //sent+= write(new_sock,buf+sent,strlen(buf)-sent);
    write(new_sock,buf,MSG_MAXLEN);
  //  } while (sent!=to_send);

}
