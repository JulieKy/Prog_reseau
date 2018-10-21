// Definition de la structure client
struct clt {
  int sockfd;
  char* psd;
  time_t date;
  //struct tm* date;
  //int port;
  //char* IP; 
  struct clt* next;
};

// Create a new list of clients
struct clt* client_list_init();

// Create a new client
//struct clt* client_new(int, char, char*, int, int);
struct clt* client_new(int, struct sockaddr_in);

// Add a client to the list
//struct clt* client_add(struct clt*, int, char, char*, int, int);
struct clt* client_add(struct clt*, int, struct sockaddr_in);

// Find a client thanks to his sockfd
struct clt* client_find_sock(struct clt*, int sock);

// Find a client thanks to his pseudo 
struct clt* client_find_pseudo(struct clt*, char*);

char* who(struct clt*);

char* whois(struct clt*, char*);

// Remove a client from the list
void client_free(struct clt*t, int);

// Count the number of clients
int nbre_client(struct clt*);
