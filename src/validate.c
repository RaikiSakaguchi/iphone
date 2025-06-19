#include "validate.h"

int is_command_line_valid(int argc, const char **argv) {
  if ((argc == 3 && strcmp(argv[1], "-s") == 0) ||
      (argc == 4 && strcmp(argv[1], "-c") == 0)) {
    return 1;
  } else {
    fprintf(stderr, "Invalid command line arguments.\n");
    fprintf(stderr, "server: %s -s <port>\n", argv[0]);
    fprintf(stderr, "client: %s -c <IP> <port>\n", argv[0]);
    return 0;
  }
}