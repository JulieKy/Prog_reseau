// Prototypes
int do_socket();
struct sockaddr_in init_host_addr(char*,int);
void do_connect (struct sockaddr_in, int);

void send_first_pseudo(int);
char* readline(int);
void  do_write(char*, int);
int do_read(int);
