#include "server.h"

client_info accept_client(int port) {
  struct sockaddr_in addr;
  struct sockaddr_in client_addr;
  client_info client;
  client.s = -1;
  client.addr = client_addr;
  int ss = socket(PF_INET, SOCK_STREAM, 0);
  if (ss < 0) {
    perror("socket");
    return client;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(ss, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    close(ss);
    return client;
  }

  if (listen(ss, 10) < 0) {
    perror("listen");
    close(ss);
    return client;
  }

  socklen_t len = sizeof(struct sockaddr_in);
  int s = accept(ss, (struct sockaddr *)&client_addr, &len);
  if (s < 0) {
    perror("accept");
    close(ss);
    return client;
  }

  close(ss);
  client.s = s;
  client.addr = client_addr;
  return client;
}