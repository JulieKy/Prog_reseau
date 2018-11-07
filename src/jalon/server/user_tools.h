// Definition de la structure client
struct clt {
  int sockfd;
  char* psd;
  char* date;
  unsigned short port;
  char* IP;
  char* channel;
  struct clt* next;
};

// Create a new list of clients
struct clt* client_list_init();

// Create a new client
struct clt* client_new(int, char*,unsigned short);

// Add a client to the list
struct clt* client_add(struct clt*, int, char*, unsigned short);

// Find a client thanks to his sockfd
struct clt* client_find_sock(struct clt*, int sock);

// Find a client thanks to his pseudo
struct clt* client_find_pseudo(struct clt*, char*);

// Create the list of online users
char* who(struct clt*);

// Create the list of online users
char* whois(struct clt*, char*);

// Remove a client from the list
struct clt* remove_client(struct clt*, struct clt*);

// Count the number of clients
int nbre_client(struct clt*);
