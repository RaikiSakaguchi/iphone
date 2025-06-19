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
  struct sockaddr_in addr;

  // connection to server or client
  int is_server = (argc == 2);
  if (is_server) {
    // server mode
    s = socket(PF_INET, SOCK_DGRAM, 0);
    // int ss = socket(PF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    // bind(ss, (struct sockaddr *)&addr, sizeof(addr));

    // listen(ss, 10);

    // socklen_t len = sizeof(struct sockaddr_in);
    // s = accept(ss, (struct sockaddr *)&addr, &len);
    // if (s < 0) {
    //   perror("accept");
    //   close(ss);
    //   return 1;
    // }
    // close(ss);

    printf("Connected to client: %s:%d\n", inet_ntoa(addr.sin_addr),
           ntohs(addr.sin_port));
  } else {
    // client mode
    s = socket(PF_INET, SOCK_DGRAM, 0);
    // s = socket(PF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    inet_aton(argv[1], &addr.sin_addr);
    addr.sin_port = htons(atoi(argv[2]));
    // if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    //   perror("connect");
    //   return 1;
    // }

    printf("Connected to server: %s:%d\n", argv[1], atoi(argv[2]));
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

  if (is_server) {
    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof(peer_addr);
    while (1) {
      write_n = recvfrom(s, write_buf, BUFSIZE, 0,
                         (struct sockaddr *)&peer_addr, &peer_addr_len);
      if (write_n < 0) {
        perror("recvfrom");
        break;
      }
      fwrite(write_buf, 1, write_n, play_fp);

      read_n = fread(read_buf, 1, BUFSIZE, rec_fp);
      if (read_n < 0) {
        perror("fread");
        break;
      }
      if (sendto(s, read_buf, read_n, 0, (struct sockaddr *)&peer_addr,
                 peer_addr_len) < 0) {
        perror("sendto");
        break;
      }
    }
  } else {
    while (1) {
      // read and send data first
      read_n = fread(read_buf, 1, BUFSIZE, rec_fp);
      if (read_n < 0) {
        perror("fread");
        break;
      }
      if (sendto(s, read_buf, read_n, 0, (struct sockaddr *)&addr,
                 sizeof(addr)) < 0) {
        // if (send(s, read_buf, read_n, 0) < 0) {
        perror("send");
        break;
      }

      // then receive and write data
      write_n = recvfrom(s, write_buf, BUFSIZE, 0, NULL, NULL);
      // write_n = recv(s, write_buf, BUFSIZE, 0);
      if (write_n < 0) {
        perror("recv");
        break;
      }
      fwrite(write_buf, 1, write_n, play_fp);
    }
  }
  close(s);
  pclose(rec_fp);
  pclose(play_fp);
  return 0;
}
