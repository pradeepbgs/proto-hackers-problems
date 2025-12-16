#include "request_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


HttpRequest request_parser(int client_fd) {
  HttpRequest request = {0};

  char buffer[4096];
  int bytes_read;
  int total_read = 0;

  while ((bytes_read = read(client_fd, buffer + total_read,
                            sizeof(buffer) - total_read)) > 0) {
    total_read += bytes_read;
    buffer[total_read] = '\0';

    char *header_end = strstr(buffer, "\r\n\r\n");
    if (header_end) {
      break;
    }
  }

  if (bytes_read <= 0)
    return request;


  // it parses method and path
  sscanf(buffer, "%7s %255s", request.method, request.path);

  char *header_head = strstr(buffer, "\r\n\r\n");
  if (!header_head)
    return request;

  int header_len = header_head - buffer;
  strncpy(request.headers, buffer, header_len);
  request.headers[header_len] = '\0';

  char *cl = strcasestr(request.headers, "Content-Length:");
  if (cl) {
    request.content_length = atoi(cl + 15);
  } else {
    request.content_length = 0;
  }

  if (request.content_length == 0) {
    request.body = NULL;
    return request;
  }

  // parse the body
  char *body_start = header_head + 4;
  int body_received = total_read - ( body_start - buffer);

  request.body = malloc(request.content_length + 1);
  memcpy(request.body, body_start, body_received);

  while (body_received < request.content_length) {
        int r = read(client_fd,
                     request.body + body_received,
                     request.content_length - body_received);

        if (r <= 0) break;

        body_received += r;
    }
  request.body[request.content_length] = '\0';

  return request;
}