#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSIZE 2048

int main(int argc, char const *argv[]) {
  if (argc != 2 && argc != 3) {
    fprintf(stderr,
            "Usage: %s <port> (for server) or %s <IP> <port> (for client)\n",
            argv[0], argv[0]);
    return 1;
  }

  int s;

  // connection to server or client
  if (argc == 2) {
    // server mode
    int ss = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(ss, (struct sockaddr *)&addr, sizeof(addr));

    listen(ss, 10);

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    s = accept(ss, (struct sockaddr *)&client_addr, &len);
    if (s < 0) {
      perror("accept");
      close(ss);
      return 1;
    }
    close(ss);

    printf("Connected to client: %s:%d\n", inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));
  } else if (argc == 3) {
    // client mode
    s = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_aton(argv[1], &addr.sin_addr);
    addr.sin_port = htons(atoi(argv[2]));
    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      perror("connect");
      return 1;
    }

    printf("Connected to server: %s:%d\n", argv[1], atoi(argv[2]));
  } else {
    fprintf(stderr, "Invalid arguments.\n");
    return 1;
  }

  FILE *rec_fp = popen("rec -t raw -r 44100 -b 16 -c 1 -e s -", "r");
  FILE *play_fp = popen("play -t raw -r 44100 -b 16 -c 1 -e s -", "w");
  if (rec_fp == NULL || play_fp == NULL) {
    perror("popen");
    close(s);
    return 1;
  }

  char read_buf[BUFSIZE];
  char write_buf[BUFSIZE];
  ssize_t read_n;
  ssize_t write_n;
  while (1) {
    // read and send data first
    read_n = fread(read_buf, 1, BUFSIZE, rec_fp);
    if (read_n < 0) {
      perror("fread");
      close(s);
      pclose(rec_fp);
      pclose(play_fp);
      return 1;
    }
    if (send(s, read_buf, read_n, 0) < 0) {
      perror("send");
      close(s);
      pclose(rec_fp);
      pclose(play_fp);
      return 1;
    }

    // then receive and write data
    write_n = recv(s, write_buf, BUFSIZE, 0);
    if (write_n < 0) {
      perror("recv");
      close(s);
      pclose(rec_fp);
      pclose(play_fp);
      return 1;
    }
    fwrite(write_buf, 1, write_n, play_fp);
  }
  return 0;
}
