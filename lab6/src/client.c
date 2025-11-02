#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <inttypes.h>

#include "bibl.h"

struct Server {
  char ip[255];
  int port;
};

struct ThreadArgs {
  struct Server server;
  uint64_t begin;
  uint64_t end;
  uint64_t mod;
  uint64_t result;
};

/*uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
  uint64_t result = 0;
  a = a % mod;
  while (b > 0) {
    if (b % 2 == 1)
      result = (result + a) % mod;
    a = (a * 2) % mod;
    b /= 2;
  }

  return result % mod;
}*/

/*bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}*/

void *ClientThread(void *args) {
  struct ThreadArgs *targs = (struct ThreadArgs *)args;

  struct hostent *hostname = gethostbyname(targs->server.ip);
  if (hostname == NULL) {
    fprintf(stderr, "gethostbyname failed with %s\n", targs->server.ip);
    pthread_exit(NULL);
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(targs->server.port);
  server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

  int sck = socket(AF_INET, SOCK_STREAM, 0);
  if (sck < 0) {
    fprintf(stderr, "Socket creation failed!\n");
    pthread_exit(NULL);
  }

  if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
    fprintf(stderr, "Connection failed to %s:%d\n", targs->server.ip,
            targs->server.port);
    close(sck);
    pthread_exit(NULL);
  }

  char task[sizeof(uint64_t) * 3];
  memcpy(task, &targs->begin, sizeof(uint64_t));
  memcpy(task + sizeof(uint64_t), &targs->end, sizeof(uint64_t));
  memcpy(task + 2 * sizeof(uint64_t), &targs->mod, sizeof(uint64_t));

  if (send(sck, task, sizeof(task), 0) < 0) {
    fprintf(stderr, "Send failed\n");
    close(sck);
    pthread_exit(NULL);
  }

  char response[sizeof(uint64_t)];
  if (recv(sck, response, sizeof(response), 0) < 0) {
    fprintf(stderr, "Receive failed\n");
    close(sck);
    pthread_exit(NULL);
  }

  memcpy(&targs->result, response, sizeof(uint64_t));
  close(sck);
  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'}; // TODO: explain why 255

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        ConvertStringToUI64(optarg, &k);
        // TODO: your code here
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        // TODO: your code here
        break;
      case 2:
        // TODO: your code here
        memcpy(servers, optarg, strlen(optarg));
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == -1 || mod == -1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    return 1;
  }

  FILE *file = fopen(servers, "r");
  if (file == NULL) {
    fprintf(stderr, "Cannot open servers file\n");
    return 1;
  }

  struct Server *to = NULL;
  unsigned int servers_num = 0;
  while (!feof(file)) {
    char line[255];
    if (fgets(line, sizeof(line), file) == NULL)
      break;
    if (strlen(line) < 3)
      continue;

    to = realloc(to, sizeof(struct Server) * (servers_num + 1));
    sscanf(line, "%[^:]:%d", to[servers_num].ip, &to[servers_num].port);
    servers_num++;
  }
  fclose(file);

  // TODO: delete this and parallel work between servers
  pthread_t threads[servers_num];
  struct ThreadArgs targs[servers_num];

  uint64_t part = k / servers_num;
  uint64_t current = 1;
  for (int i = 0; i < servers_num; i++) {
    targs[i].server = to[i];
    targs[i].mod = mod;
    targs[i].begin = current;
    targs[i].end = (i == servers_num - 1) ? k : current + part - 1;
    current = targs[i].end + 1;

    pthread_create(&threads[i], NULL, ClientThread, (void *)&targs[i]);
  }

  uint64_t total = 1;
  for (int i = 0; i < servers_num; i++) {
    pthread_join(threads[i], NULL);
    total = MultModulo(total, targs[i].result, mod);
  }

  printf("Result: %" PRIu64 "\n", total);

  free(to);
  return 0;
}
