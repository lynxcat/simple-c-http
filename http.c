//
// Created by lynxcat on 2019-09-30.
//
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "http.h"
#include "request.h"

char *get_result = "HTTP/1.1 200 OK\nDate: Mon Sep 30 2019 15:02:27 GMT\nContent-Type: text/html;charset=ISO-8859-1\nContent-Length: 62\n\n<html><title>lynx http.</title><body>hello, get!</body></html>";
char *other_result = "HTTP/1.1 200 OK\nDate: Mon Sep 30 2019 15:02:27 GMT\nContent-Type: text/html;charset=ISO-8859-1\nContent-Length: 64\n\n<html><title>lynx http.</title><body>hello, other!</body></html>";

int InitHttpServer(HttpServer **server, int port, char *ipaddr){
    *server = (HttpServer *) malloc(sizeof(HttpServer));
    struct sockaddr_in addr;

    //创建socket
    if(((*server)->server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket error.");
        return -1;
    }


    //初始化服务器地址信息
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ipaddr);


    if(bind((*server)->server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        perror("connect error.");
        return -1;
    }

    if(port == 0){
        if(getsockname((*server)->server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1){
            perror("get socket name");
            return -1;
        }
        port = ntohs(addr.sin_port);
    }

    (*server)->port = port;
    (*server)->ipaddr = ipaddr;

    if(listen((*server)->server_fd, 5) < 0){
        perror("listen error");
        return -1;
    }

    return 0;
}

void AcceptClient(HttpServer *server){
    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    int fd;

    printf("开始监听监听: %s:%d\n", server->ipaddr, server->port);

    while (1) {
        fd = accept(server->server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
        if(fd < 0){
            perror("accept");
            continue;
        }

        printf("客户端已连接,FD is %d IP is %s, port is %d\n", fd, inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));
        HandleClientRequest(fd);
        close(fd);
    }
}

void HandleClientRequest(int client_fd){
    char buf[2048];
    //如果是文件请求，则直接读取文件进行返回
    //如果是CGI请求，则请求对应CGI
    //返回完成后对FD进行管理，加入销毁堆。按时间关闭

    Request *request = (Request *) malloc(sizeof(Request));
    request->fd = client_fd;

    while (1){
        buf[0] = '\0';
        int num;
        //接收HTTP报文
        num = recv(client_fd, buf, 2048, 0);
        if (num < 0){
            perror("recv null");
            continue;
        }
        buf[num] = '\0';
        printf("接收到客户端报文:\n%s\n", buf);


        //分析HTTP报文
        if(parseHttpRequest(request, buf) != 0){
            break;
        }
        printf("--method start--\n%d\n--method end--\n\n", request->method);
        printf("--path start--\n%s\n--path end--\n\n", request->path);
        printf("--version start--\n%.1f\n--version end--\n\n", request->version);
        printf("--header start--\n%s\n--header end--\n\n", request->header);
        if(request->method == POST || request->method == PUT){
            printf("--body start--\n%s\n--body end--\n\n", request->body);
        }

        //根据不同的请求进行分别处理
        if(request->method == GET){
            send(client_fd, get_result, strlen(get_result), 0);
        }else{
            send(client_fd, other_result, strlen(other_result), 0);
        }

        FreeRequest(request);
        break;
    }
}