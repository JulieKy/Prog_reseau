#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>


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

    //get the socket
    //s = do_socket()
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // Init the sock_host structure
    struct sockaddr_in sock_host;
    memset(& sock_host, '\0', sizeof(sock_host)); // Initilalisation de l'adresse de la socket à 0
    sock_host.sin_family=AF_INET; // Ipv4
    sock_host.sin_port=htons(0); // ATTENTION, PEUT ETRE PAS 0 !!!  Le port client est alloué dynamiquement
    int sv_addr_ok;
    sv_addr_ok = inet_aton(sv_addr, &sock_host.sin_addr); // Converti l'adresse en une donnée binaire (renvoie 0 si pas bon)

    printf("adresse serveur ok?: %d\nport serveur: %d\nport client : %d\nfini\n", sv_addr_ok,n_port, sock_host.sin_port);


    //connect to remote socket
    connect(sock,(struct sockaddr*)& sock_host, sizeof(sock_host));


//get address info from the server
//get_addr_info()

//get the socket
//s = do_socket()

//connect to remote socket
//do_connect()


//get user input
//readline()
char msg;
printf("Que voulez-vous envoyer au serveur?\n");
scanf("%s",&msg);
printf("%s",&msg); // PROBLEME : NE PREND QU'UN SEUL MOT

//send message to the server
//handle_client_message()


    return 0;


}
