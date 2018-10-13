struct clt;

// Create a new list of clients
struct clt* client_list_init();

// Create a new client
struct clt* client_new(int, char, char*, int, int);

// Add a client to the list
struct clt* client_add(struct clt*, int, char, char*, int, int);

// Remove a client from the list
void client_free(struct clt*, struct clt*);
