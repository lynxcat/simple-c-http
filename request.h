//
// Created by lynxcat on 2019-09-30.
//

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H


typedef struct _request Request;

struct _request{
    int fd;

    char *path;

    enum {
        GET=1, POST, HEAD, OPTIONS, PUT, PATCH, DELETE, TRACE, CONNECT
    } method;

    float version;

    char *header;

    char *body;
};

int parseHttpRequest(Request *request, char *text);

int parseMethod(Request *request, char *text);

int parsePath(Request *request, char *text);

int parseProtocolVersion(Request *request, char *text);

int parseRequestHeader(Request *request, char *text);

int parseRequestBody(Request *request, char *text);

void FreeRequest(Request *request);

#endif //HTTP_REQUEST_H