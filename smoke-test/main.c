#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 4096

void *handle_client(void *args) {
  int client_fd = *(int *)args;
  free(args);

  // we don't need to parse req here , just read the body and bounce/return
  // that.
  char buffer[BUF_SIZE];
  int read_count = 0;

  while ((read_count = read(client_fd, buffer, sizeof(buffer))) > 0) {
    int sent = 0;
    while (sent < read_count) {
      ssize_t m = write(client_fd, buffer + sent, read_count - sent);
      if (m <= 0) {
        close(client_fd);
        return 0;
      }
      sent += m;
    }
  }
  close(client_fd);
  return NULL;
}

int main() {
  signal(SIGPIPE, SIG_IGN);

  const char *port = getenv("PORT");
  if (!port)
    port = "8080";

  int server_fd;

  struct addrinfo hints, *res, *p;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int err = getaddrinfo(NULL, port, &hints, &res);
  if (err != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
    exit(EXIT_FAILURE);
  }

  for (p = res; p != NULL; p = p->ai_next) {
    server_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (server_fd < 0)
      continue;

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(server_fd, p->ai_addr, p->ai_addrlen) == 0) {
      break;
    }
    // if bind fails then close server_fd;
    close(server_fd);
    server_fd = -1;
  }

  freeaddrinfo(res);

  if (server_fd < 0) {
    fprintf(stderr, "Failed to bind\n");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 50) < 0) {
    printf("listen failed on PORT: %s", port);
    exit(EXIT_FAILURE);
  }

  printf("Server running on http://localhost:%s\n", port);

  while (1) {
    struct sockaddr_storage client_addr;
    socklen_t addrlen = sizeof(client_addr);

    int *client_fd = malloc(sizeof(int));
    *client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
    if (*client_fd < 0) {
      perror("accept failed");
      free(client_fd);
      continue;
    }

    pthread_t tid;
    pthread_create(&tid, NULL, handle_client, client_fd);
    pthread_detach(tid);
  }
  close(server_fd);
  return 0;
}