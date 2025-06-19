#include <netinet/in.h>
#include <stdio.h>

typedef struct client {
  int s;
  struct sockaddr_in addr;
} client_info;

client_info accept_client(int port);
