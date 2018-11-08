int do_socket();

struct sockaddr_in init_serv_addr(int);

void do_bind(int, struct sockaddr_in);

void do_listen(int, struct sockaddr_in);

int do_accept(struct sockaddr_in*, int);

int test_nb_users(struct pollfd*, int, int, int, struct sockaddr_in);

char* do_read(int);

// Write a broadcast message
void do_write_broadcast(int, char*, struct clt*);

// Write a multicast message
void do_write_multicast(struct clt*, struct clt*, char*, struct channel*);

// Write a unicast message
char* do_write_unicast(int sock, char* pseudo, char* msg, struct clt* first_client);

void do_write(char*, int);

/* -------------- Create the list of online users -------------- */
char* what_channels(struct channel* first_channel);

/* -------------- Create the list of online members in a channel -------------- */
char* who_channels(struct channel* list_channel, char * msg, struct clt* list_client);

void do_close(int, struct clt*);

/* -------------- Test the different queries when client isn't in a channel -------------- */
struct channel* treat_writeback(char *buf, struct clt* first_client, int sock, struct channel* first_channel);

/* -------------- Test the different queries when client is in a channel -------------- */
struct channel* treat_writeback_channel(char *buf, struct clt* first_client, int sock, struct channel* first_channel);
