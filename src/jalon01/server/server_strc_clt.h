struct clt;

// Create a new list of clients
struct clt* client_list_init();

// Create a new client
//struct clt* client_new(int, char, char*, int, int);
struct clt* client_new(int);

// Add a client to the list
//struct clt* client_add(struct clt*, int, char, char*, int, int);
struct clt* client_add(struct clt*, int);

// Find a client thanks to his sockfd
struct clt* client_find(struct clt* first_client, int sock);

// Remove a client from the list
void client_free(struct clt*t, int);

// Count the number of clients
int nbre_client(struct clt*);
