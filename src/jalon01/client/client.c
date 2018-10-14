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

    //get address info from the server
    //get_addr_info()
    char* sv_addr=argv[1]; // adresse du serveur (127.0.0.1)
    short n_port=atoi(argv[2]); //numéro de port de la socket côté serveur (quand connect)

    int sock=do_socket();
    struct sockaddr_in sock_host=init_host_addr(sv_addr, n_port);
    //struct sockaddr_in* p=&sock_host;
    do_connect(sock_host, sock);
    printf("connect\n");
    for (;;){
      char* msg=readline(sock);
      handle_client_message(msg, sock);
      if (strcmp(msg,"/quit\n")==0)
        break;
      int too_clients=handle_server_message(sock);
      if (too_clients==1){
        break;
      }
    }
    printf("=== Socket closed === \n");
    close(sock);

    return 0;
}
