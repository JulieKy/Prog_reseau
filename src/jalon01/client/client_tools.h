// Prototypes
int do_socket();
struct sockaddr_in init_host_addr(char*,int);
void do_connect (struct sockaddr_in, int);
char* readline(int);
void handle_client_message(char*, int);
int handle_server_message(int);
