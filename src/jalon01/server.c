#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// Décalaration des prototypes de fonctions
int do_socket();
struct sockaddr_in init_serv_addr(int);
void do_bind(int, struct sockaddr_in);
void do_listen(int, struct sockaddr_in);

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

    return 0;
  }

// Create the socket and check the validity
int do_socket(){
  int sock;
  sock = socket(AF_INET, SOCK_STREAM, 0);
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
  printf("port serveur: %d\nfini\n", sv_port);
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
  int i_listen=listen(sock,SOMAXCONN); // Peut être mettre 20 à la place de SOMAXCONN?
  if (i_listen == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }


    //specify the socket to be a server socket and listen for at most 20 concurrent client
    //listen()

    //for (int i=0;i<20;i++)
    //{

        //accept connection from client
        //do_accept()
        socklen_t length = sizeof(saddr_in);
        socklen_t* addrlen = &length;
        int new_sock;
        new_sock=accept(sock, (struct sockaddr*)&saddr_in, addrlen); // Surement pas les bons paramètres (il faut mettre ceux clients)
        if (new_sock < 0){
          //printf("accept");
          perror("accept");
          exit(EXIT_FAILURE);
        }
        //read what the client has to say
        //do_read()
        //char* buf = malloc(sizeof (char) * 30);
        char buf[30];
        //bzero(buf, 30);
        int nb_rcv =0;
        int to_rcv=30;
        printf("%d", to_rcv);
      //  do{
        //  nb_rcv+=read(new_sock,buf+nb_rcv, strlen(buf)-nb_rcv);// PAS DU TOUT SUR QUE CE SOIT CA
          read(new_sock,buf, 30);
          printf("hello\n");
      //  } while (nb_rcv!=to_rcv);

        //we write back to the client
        //do_write()
        //send(sock, &msg, ,0);
        int sent=0;
        int to_send=strlen(buf);
        //do{
          //sent+= write(new_sock,buf+sent,strlen(buf)-sent);
          write(new_sock,buf,30);
          printf("boucle send\n");
      //  } while (sent!=to_send);

      //  i++;
    //}

}

  //clean up client socket
    //clean up server socket

    //return 0;
//}
