
/* -------------- Create a socket ------------------ */
int do_socket();

/* -------------- Initialize the socket------------- */
struct sockaddr_in init_host_addr(char*,int);

/* -------------- Connection ----------------------- */
void do_connect (struct sockaddr_in, int);

/* -------------- Perform the binding -------------- */
void do_bind(int sock, struct sockaddr_in saddr_in);

/* -------------------- Listen  -------------------- */
void do_listen(int sock, struct sockaddr_in saddr_in);

/* -------------- Accept connection ---------------- */
int do_accept(struct sockaddr_in saddr_in, int sock);

/* -------------- Ask the pseudo ------------------- */
void send_first_pseudo(int);

/* -------------- Read a line from STDIN ----------- */
char* readline(int);

/* -------------- Write in the socket -------------- */
void  do_write(char*, int);

/* -------------- Read from the socket-------------- */
char* do_read(int, int in);

/* --------- --- Answer a yes/no question ---------- */
char* answer_send_file(int sock, int in);

/* -------------- Create socket -------------------- */
int create_socket(char* sv_addr, int port, int test_con_servclt);

/* ------------- Create a listenning socket -------- */
int create_listenning_socket(char* sv_addr, int port);

/* -------- Answer no (receiving file) ------------- */
void file_answer(char* psd_sender, int fd, char* rep, int port);

/* ----------------- C1 send file ------------------ */
void send_file(int sock, char* file_path);

/* --------------- C2 receive file ----------------- */
void receive_file(int sock);
