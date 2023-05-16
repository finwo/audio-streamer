#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked-list.h"
#include "finwo/http-parser.h"
#include "tidwall/evio.h"

#include "http.h"

struct conn_udata {
  struct evio_conn *connection;
  struct http_parser_pair *reqres;
};

LLIST(struct evio_conn, llist_conns);
struct llist_conns *listeners = NULL;

void route_data_get(struct http_parser_event *ev) {
  struct conn_udata *conndata = ev->udata;
  struct llist_conns *listener = malloc(sizeof(struct llist_conns));

  // Fetching the request
  // Has been wrapped in http_parser_event to support more features in the future
  struct http_parser_message *request  = ev->request;
  struct http_parser_message *response = ev->response;
  struct http_parser_header *header    = NULL;

  // Build response
  response->status = 200;
  http_parser_header_set(response, "Transfer-Encoding", "chunked"             );
  http_parser_header_set(response, "Content-Type"     , "application/x-ndjson");

  response->body     = strdup("2\r\n{}\r\n");
  response->bodysize = strlen(response->body);

  // Write headers (data will come later)
  char *response_buffer = http_parser_sprint_response(response);
  evio_conn_write(conndata->connection, response_buffer, strlen(response_buffer));
  free(response_buffer);

  // Register the connection as a listener to send data to
  listener->next = listeners;
  listener->data = conndata->connection;
  listeners      = listener;

  // Intentionally NOT closing the connection
}

void serving(const char **addrs, int naddrs, void *udata) {
  for (int i = 0; i < naddrs; i++) {
    printf("Serving at %s\n", addrs[i]);
  }
}

void error(const char *msg, bool fatal, void *udata) {
  fprintf(stderr, "%s\n", msg);
}

int64_t tick(void *udata) {
  /* printf("Tick\n"); */
  // next tick in 1 second. This can be any duration in nanoseconds.
  return 1e9;
}

static void onRequest(struct http_parser_event *ev) {
  struct conn_udata *conndata = ev->udata;

  // The request has been received
  // Answer the request directly or pass it to a route handler of sorts

  // Fetching the request
  // Has been wrapped in http_parser_event to support more features in the future
  struct http_parser_message *request  = ev->request;
  struct http_parser_message *response = ev->response;

  if (!(strcmp(request->method, "GET") || strcmp(request->path, "/api/v1/events"))) {
    return route_data_get(ev);
  }

  // Build response
  response->status = 200;
  http_parser_header_set(response, "Connection"  , "close"     );
  http_parser_header_set(response, "Content-Type", "text/plain");

  response->body = calloc(1, 8192);
  strcat(response->body, "Method: ");
  strcat(response->body, request->method  ? request->method : "(null)");
  strcat(response->body, "\nPath: ");
  strcat(response->body, request->path    ? request->path : "(null)");
  strcat(response->body, "\nQuery: ");
  strcat(response->body, request->query   ? request->query : "(null)");
  strcat(response->body, "\nHTTP Version: ");
  strcat(response->body, request->version ? request->version : "(null)");
  strcat(response->body, "\nHost: ");
  strcat(response->body, http_parser_header_get(request, "host"));
  strcat(response->body, "\n");
  response->bodysize = strlen(response->body);

  // Or you can free the whole pair
  char *response_buffer = http_parser_sprint_response(response);
  evio_conn_write(conndata->connection, response_buffer, strlen(response_buffer));
  free(response_buffer);
  evio_conn_close(conndata->connection);
}


void opened(struct evio_conn *conn, void *udata) {
  struct conn_udata *conndata = malloc(sizeof(struct conn_udata));
  conndata->connection        = conn;
  conndata->reqres            = http_parser_pair_init(conndata);
  conndata->reqres->onRequest = onRequest;
  evio_conn_set_udata(conn, conndata);
}

void closed(struct evio_conn *conn, void *udata) {
  struct conn_udata *conndata = evio_conn_udata(conn);
  http_parser_pair_free(conndata->reqres);
  free(conndata);
}

void data(struct evio_conn *conn, const void *data, size_t len, void *udata) {
  struct conn_udata *conndata = evio_conn_udata(conn);
  http_parser_pair_request_data(conndata->reqres, data, len);
}

void * task_http(void *arg) {

  struct evio_events evs = {
    .serving = serving,
    .error   = error,
    .tick    = NULL,
    .opened  = opened,
    .closed  = closed,
    .data    = data,
  };

  // Any number of addresses can be bound to the appliation.
  // Here we are listening on tcp port 9999 (ipv4 and ipv6), and at a local
  // unix socket file named "socket".
  // For ipv4 only use an ip address like tcp://127.0.0.1:8080
  // For ipv6 use something like tcp://[::1]:8080
  const char *addrs[] = {
    "tcp://localhost:8080",
  };

  // Run the application. This is a forever operation.
  evio_main(addrs, sizeof(addrs) / sizeof(void*), evs, NULL);

  // Exit without error
  return NULL;
}
