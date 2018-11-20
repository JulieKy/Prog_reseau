#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <poll.h>

#include "client_tools.h"

// Main
int main(int argc,char** argv)
{

    if (argc != 3)
    {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        return 1;
    }

    // Definition du tableau de structures pollfd
    struct pollfd fds[3];
    int n;
    int nfds=3;
    int timeout=-1;
    memset(fds, 0 , sizeof(fds));

    // Get address info from the server
    char* sv_addr=argv[1]; // Server adress (127.0.0.1)
    short n_port=atoi(argv[2]); // Port number

    // Socket creation and connection to the server with a pseudo
    int sock=do_socket();
    struct sockaddr_in sock_host=init_host_addr(sv_addr, n_port);
    do_connect(sock_host, sock);

    // Initialisation du tableau avec la l'entrée standard et la socket
    fds[0].fd=0;
    fds[0].events = POLLIN;
    fds[1].fd=sock;
    fds[1].events = POLLIN;

    send_first_pseudo(sock);
    do_read(sock, fds[0].fd);

    for (;;) {
      // Nombre de socket en activité
      int n=poll(fds, nfds, timeout);

      if(fds[0].revents == POLLIN) {

        // Get a line from the terminal
        char* msg=readline(fds[0].fd);

        // Write to the server
        do_write(msg, fds[1].fd);

        // Close the socket if /quit
        if (strcmp(msg,"/quit\n")==0)
          break;
      }

      if(fds[1].revents == POLLIN) {

        // Read a message and check that there aren't too many users
        char* read=do_read(fds[1].fd, fds[0].fd);

        // If there are too many clients -------------------------------
        if (strcmp(read, "too many clients")==0) {
          break;
        }
        printf("aaaa\n");
        char* psd_sender= malloc(sizeof (char) * 60);
        char* rep = malloc(sizeof (char) * 60);
        sscanf(read, "%s %s", rep, psd_sender);
        printf("aaaaaaaaaaaaaaaa\n");
        printf("rep=%s\n", rep);

        // If the client wants to receive a file ------------------------
        if (strcmp(rep, "y")==0) {
          printf("je suis dans le yes\n");
          //int sock=create_listenning_socket(sock, sv_addr, fds);
          // Création d'une socket d'écoute
            int sockfd=do_socket();
            int port=12345;
            struct sockaddr_in saddr_in = init_host_addr(sv_addr,port);
            do_bind(sockfd, saddr_in);

            // Récupération du numéro de port de la socket
            /*socklen_t len = sizeof(struct sockaddr_in);
            getsockname(fds, (struct sockaddr *) saddr_in, &len);
            port = ntohs(saddr_in->sin_port);*/

            // Acceptation
            do_listen(sockfd, saddr_in);
            int new_sock=do_accept(saddr_in,sockfd);
            printf(">> Le numéro de socket du nouveau client est : %d\n", new_sock);
        }

        // If the client don't want to receive a file ---------------------
        if (strcmp(rep, "n")==0) {
          file_answer_no(psd_sender, fds[1].fd);
        }

        free(rep);
        free(psd_sender);
      }
    }
    printf("=== Socket closed === \n");
    close(sock);

    return 0;
}
