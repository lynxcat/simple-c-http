//
// Created by lynxcat on 2019/10/6.
//
#include <string.h>
#include "request.h"
#include <stdio.h>
#include <stdlib.h>

char *text = "GET /favicon.ico HTTP/1.1\nHost: 127.0.0.1:10000\nConnection: keep-alive\nUpgrade-Insecure-Requests: 1\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.90 Safari/537.36\nSec-Fetch-Mode: navigate\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3\nSec-Fetch-Site: none\nAccept-Encoding: gzip, deflate, br\nAccept-Language: zh-CN,zh;q=0.9,zh-TW;q=0.8\n\n\n";


int parseHttpRequest(Request *request, char *text){

    int i = parseMethod(request, text);
    if(i == -1){
        printf("http method parse error.");
        return -1;
    }

    text = text + i;
    i = parsePath(request, text);
    if(i == -1){
        printf("http path parse error.");
        return -1;
    }

    text = text + i;
    i = parseProtocolVersion(request, text);
    if(i == -1){
        printf("http version parse error.");
        return -1;
    }

    text = text + i;
    i = parseRequestHeader(request, text);
    if(i == -1){
        printf("http header parse error.");
        return -1;
    }

    if(request->method == POST || request->method == PUT) {
        text = text + i;
        i = parseRequestBody(request, text);
        if (i == -1) {
            printf("http body parse error.");
            return -1;
        }
    }

    return 0;
}

int parseRequestBody(Request *request, char *text){
    int len = strlen(text);
    if (len == 0) {
        return -1;
    }

    request->body = text;
    return len;
}

int parseRequestHeader(Request *request, char *text){
    int i = 0;
    while (text[i] != '\0' && !(text[i] == '\r' && text[i + 1] == '\n' && text[i + 2] == '\r' && text[i + 3] == '\n')){
        i++;
    }

    if (i == 0) {
        return -1;
    }

    request->header = malloc(sizeof(char) * i + 1);
    strncpy(request->header, text, i);
    request->header[i] = '\0';

    return i + 4;
}

int parseProtocolVersion(Request *request, char *text){
    int i = 0;
    while (text[i] != '\0' && text[i] != '\r' && text[i + 1] != '\n'){
        i++;
    }

    if (i == 0) {
        return -1;
    }

    char *str = malloc(sizeof(char) * i + 1);
    strncpy(str, text, i);
    str[i] = '\0';

    char *res = strchr(str, '/');
    request->version = atof(res + 1);

    free(str);

    return i + 2;
}

int parsePath(Request *request, char *text){
    int i = 0;
    while (text[i] != ' ' && text[i] != '\0'){
        i++;
    }

    if (i == 0) {
        return -1;
    }

    request->path = malloc(sizeof(char) * i + 1);
    strncpy(request->path, text, i);
    request->path[i] = '\0';

    return i + 1;
}

int parseMethod(Request *request, char *text){
    int i = 0;
    while (text[i] != ' ' && text[i] != '\0'){
        i++;
    }

    if (i == 0) {
        return -1;
    }

    char *str = malloc(sizeof(char) * i);
    strncpy(str, text, i);

    if(strcmp(str, "GET") == 0){
        request->method = GET;
    }else if(strcmp(str, "POST") == 0){
        request->method = POST;
    }else if(strcmp(str, "HEAD") == 0){
        request->method = HEAD;
    }else if(strcmp(str, "OPTIONS") == 0){
        request->method = OPTIONS;
    }else if(strcmp(str, "PUT") == 0){
        request->method = PUT;
    }else if(strcmp(str, "PATCH") == 0){
        request->method = PATCH;
    }else if(strcmp(str, "DELETE") == 0){
        request->method = DELETE;
    }else if(strcmp(str, "TRACE") == 0){
        request->method = TRACE;
    }else if(strcmp(str, "CONNECT") == 0){
        request->method = CONNECT;
    }else{
        return -1;
    }

    free(str);

    return i + 1;
}

void FreeRequest(Request *request){
    if(request->path != NULL){
        free(request->path);
        request->path = NULL;
    }

    if(request->header != NULL){
        free(request->header);
        request->header = NULL;
    }

    free(request);
    request = NULL;
}