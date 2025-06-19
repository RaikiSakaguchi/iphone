#include <netinet/in.h>

typedef struct client {
  int s;
  struct sockaddr_in addr;
} client_info;


