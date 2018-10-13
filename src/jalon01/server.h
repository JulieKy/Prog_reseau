// Décalaration des prototypes de fonctions
int do_socket();
struct sockaddr_in init_serv_addr(int);
void do_bind(int, struct sockaddr_in);
void do_listen(int, struct sockaddr_in);
int do_accept(struct sockaddr_in, int);
int TestTooManyC(struct pollfd*, int, int, int, struct sockaddr_in);
char* do_read(int);
void do_write(char*, int);
void do_close(int);
