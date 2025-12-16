#ifndef REQUEST_PARSER_H
#define REQUEST_PARSER_H

typedef struct {
  char method[8];
  char path[256];
  char headers[2048];
  char *body;
  int content_length;
} HttpRequest;

HttpRequest request_parser(int client_fd);

#endif
