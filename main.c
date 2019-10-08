#include <stdio.h>

#include "http.h"


int main() {

    HttpServer *server;

    if(InitHttpServer(&server, 10000, "127.0.0.1") == -1){
        perror("init http server error");
    }

    AcceptClient(server);

    return 0;
}