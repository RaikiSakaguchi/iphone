#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "client.h"
#include "server.h"
#include "validate.h"

#define BUFSIZE 2048

int main(int argc, char const *argv[]) {
  if (!is_command_line_valid(argc, argv)) return 1;

  int s;
  int is_server = (argc == 3);

  // connection to server or client
  if (is_server) {
    // server mode
    client_info client = accept_client(atoi(argv[1]));
    s = client.s;
    printf("Connected to client: %s:%d\n", inet_ntoa(client.addr.sin_addr),
           ntohs(client.addr.sin_port));
  } else {
    // client mode
    s = connect_to_server(argv[1], atoi(argv[2]));
    printf("Connected to server: %s:%d\n", argv[1], atoi(argv[2]));
  }

  FILE *rec_fp = popen(
      "rec -t raw -r 44100 -b 16 -c 1 -e s - | sox -t raw -r 44100 -b 16 -c 1 "
      "-e s - -t raw - pitch 500",
      "r");
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
      break;
    }
    if (send(s, read_buf, read_n, 0) < 0) {
      perror("send");
      break;
    }

    // then receive and write data
    write_n = recv(s, write_buf, BUFSIZE, 0);
    if (write_n < 0) {
      perror("recv");
      break;
    }
    fwrite(write_buf, 1, write_n, play_fp);
  }
  close(s);
  pclose(rec_fp);
  pclose(play_fp);
  return 0;
}
