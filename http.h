//
// Created by lynxcat on 2019-09-30.
//

#ifndef HTTP_HTTP_H
#define HTTP_HTTP_H

#include "request.h"

typedef struct _http_server HttpServer;

struct _http_server {
    int server_fd;
    int port;
    char *ipaddr;
};

int InitHttpServer(HttpServer **server, int port, char *ipaddr);

void AcceptClient(HttpServer *server);

void HandleClientRequest(int client_fd);

#endif //HTTP_HTTP_H
