
/* -------------- First pseudo -------------- */
char* first_pseudo(struct clt* client, char* msg);

/* -------------- Change a pseudo -------------- */
char* nick(struct clt* client, char* msg);

/* -------------- Create the list of online users -------------- */
char* who(struct clt* first_client);

/* -------------- Give information about a user -------------- */
char* whois(struct clt* first_client, char* pseudo);

/* -------------- Create the list of online users -------------- */
char* what_channels(struct channel* first_channel);

/* -------------- Create the list of online members in a channel -------------- */
char* who_channels(struct channel* list_channel, char * msg, struct clt* list_client);

/* -------------- Write a broadcast message -------------- */
void do_write_broadcast(int sock, char* msg, struct clt* first_client);

/* -------------- Write a unicast message -------------- */
char* do_write_unicast(int sock, char* pseudo, char* msg, struct clt* first_client);

/* -------------- Write a multicast message -------------- */
void do_write_multicast(struct clt* first_client, struct clt* client, char* msg, struct channel* first_channel);

/* -------------- Create a channel -------------- */
char* create_channel(struct clt* client, char* msg, struct channel* first_channel);

/* -------------- Join channel -------------- */
char* join_channel(struct clt* client, char* msg, struct channel* first_channel);

/* -------------- Join channel -------------- */
char* quit(struct clt* first_client, struct clt* client, char* msg, struct channel* first_channel);
