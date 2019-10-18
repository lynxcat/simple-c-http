//
// Created by lynxcat on 2019/10/11.
//

#include "response.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>

char *CRLF = "\r\n";

char *contents = "/Users/lynxcat/Sites/code/c/http/contents";

char *startLine = "HTTP/1.1 200 OK";

char *contentLength = "Content-Length: %i";

char *server = "server: v server.";
char *date = "Date: %s";

char *err_not_find = "<html><title>error.</title><body>file not find!</body></html>";
char *err_not_file = "<html><title>error.</title><body>path is not file!</body></html>";

void ResponseGet(Request* request){

    char buf[256] = {0};
    strcat(buf, contents);
    strcat(buf, request->path);

    printf("%s\n", buf);

    FILE *stream = fdopen(request->fd, "r+");

    struct stat info;
    int ret = stat(buf, &info);

    if(ret == 0){

        if (info.st_mode & S_IFDIR){
            fprintf(stream, "%s", startLine);
            fprintf(stream, "%s", CRLF);
            fprintf(stream, contentLength, strlen(err_not_file));
            fprintf(stream, "%s", CRLF);
            fprintf(stream, "%s", CRLF);
            fprintf(stream, "%s", err_not_file);
        }else if(S_IFREG & info.st_mode){
            FILE *fp = fopen(buf,"rb");

            printf("length: %lld\n", info.st_size);

            char *body = (char *) malloc(info.st_size + 1);
            if(body == NULL){
                fclose(fp);
                fclose(stream);
            }

            fread(body, info.st_size,1, fp); /* 一次性读取全部文件内容 */
            body[info.st_size] = '\0';
            fprintf(stream, "%s", startLine);
            fprintf(stream, "%s", CRLF);
            fprintf(stream, "%s", server);
            fprintf(stream, "%s", CRLF);

            time_t rawtime;
            struct tm * timeinfo;
            time (&rawtime);
            timeinfo = localtime (&rawtime);
            strftime (buf, 256, "%a, %d %b %Y %H:%M:%S GMT",timeinfo);

            fprintf(stream, date, buf);
            fprintf(stream, "%s", CRLF);
            fprintf(stream, contentLength, strlen(body));
            fprintf(stream, "%s", CRLF);
            fprintf(stream, "%s", CRLF);
            fprintf(stream, "%s", body);
        }
    }else{
        fprintf(stream, "%s", startLine);
        fprintf(stream, "%s", CRLF);
        fprintf(stream, contentLength, strlen(err_not_find));
        fprintf(stream, "%s", CRLF);
        fprintf(stream, "%s", CRLF);
        fprintf(stream, "%s", err_not_find);
    }

    fclose(stream);
}