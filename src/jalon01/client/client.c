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

// Main
int main(int argc,char** argv)
{

    if (argc != 3)
    {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        return 1;
    }

    // Get address info from the server
    char* sv_addr=argv[1]; // Server adress (127.0.0.1)
    short n_port=atoi(argv[2]); // Port number

    // Socket creation and connection to the server with a pseudo
    int sock=do_socket();
    struct sockaddr_in sock_host=init_host_addr(sv_addr, n_port);
    do_connect(sock_host, sock);
    send_first_pseudo(sock);
    do_read(sock);

    for (;;){

      // Get a line from the terminal
      char* msg=readline(sock);

      // Write to the server
      do_write(msg, sock);

      // Close the socket if /quit
      if (strcmp(msg,"/quit\n")==0)
        break;

      // Read a message and chack that there aren't too many users
      int too_clients=do_read(sock);
      if (too_clients==1){
        break;
      }
    }
    printf("=== Socket closed === \n");
    close(sock);

    return 0;
}
