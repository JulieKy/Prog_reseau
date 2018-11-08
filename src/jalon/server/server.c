#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/poll.h>
#include <arpa/inet.h>


#include "channel_tools.h"
#include "server_tools.h"

void error(const char *msg)
{
    perror(msg);
    exit(1);
}


/* -------------- Main -------------- */
int main(int argc, char** argv) {

  // Test nombre d'arguments
  if (argc != 2) {
      fprintf(stderr, "usage: RE216_SERVER port\n"); // Il faut donner le port de la socket coté client ??
      return 1;
  }

  // Mise en place de la socket d'écoute
  int sv_port=atoi(argv[1]);
  int sock=do_socket();
  struct sockaddr_in saddr_in = init_serv_addr(sv_port);

  do_bind(sock, saddr_in);
  do_listen(sock, saddr_in);


  // Initialisation de la liste de clients
  struct clt* first_client;
  first_client=client_list_init();

  // Initialisation de la liste de channels
  struct channel* list_channel;
  list_channel=channel_list_init();

  // Definition du tableau de structures pollfd
  struct pollfd fds[20];
  int n;
  int nfds=1;
  int timeout=-1;
  memset(fds, 0 , sizeof(fds));

  // Initialisation du tableau avec la première socket serveur en activité
  fds[0].fd=sock;
  fds[0].events = POLLIN;

  // fd de la socket à fermer
  int sock_closed;

  // Acceptation puis read et write tant que la socket n'est pas fermée
  for(;;)
  {
    // Nombre de socket en activité
    int n=poll(fds, nfds, timeout);

    // Acceptation de nouveaux clients si première socket en activité
    if(fds[0].revents == POLLIN) {

      int new_sock=do_accept(&saddr_in,sock);
      printf(">> Le numéro de socket du nouveau client est : %d\n", new_sock);
      int nfds_test=test_nb_users(fds, nfds, sock, new_sock, saddr_in);
      if (nfds_test!=0)
        nfds=nfds_test;

      // Récupération de l'@IP et du port de chaque client
      char* IP = inet_ntoa(saddr_in.sin_addr);
      unsigned short port=ntohs(saddr_in.sin_port);

      // Ajout du client à la liste
      first_client=client_add(first_client, new_sock, IP, port);
      int nb_clt= nbre_client(first_client);
      printf(">> Nombre de clients connecté au serveur: %d\n", nb_clt);

    }


    // Read et write sur les sockets en activité (sauf socket d'écoute)
    int current_size = nfds;
    for(int i=0; i<current_size ; i++){

      if ( i!=0 && fds[i].revents == POLLIN){

        // read
        char* buf;
        buf=do_read(fds[i].fd);
        //printf("[read] : %s\n", buf);
        list_channel= treat_writeback(buf, first_client, fds[i].fd, list_channel);


        struct clt* client=client_find_sock(first_client, fds[i].fd);
        if (client!=NULL) {
          char* channel_name = malloc(sizeof (char) * 60);
          sprintf(channel_name, "[%s]> ", client->channel);
          do_write(channel_name, fds[i].fd);
        }

        // Fermeture de la socket
        if(strcmp("/quit\n", buf) == 0) {
          sock_closed=fds[i].fd;
          do_close(sock_closed, first_client);
          break;
        }
      }
    }
  }

  return 0;
}
