#include <netinet/in.h>

int connect_to_server(const char *ip, int port) {
  int s = socket(PF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    perror("socket");
    return -1;
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  inet_aton(ip, &addr.sin_addr);
  addr.sin_port = htons(port);
  if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("connect");
    close(s);
    return -1;
  }

  return s;
}