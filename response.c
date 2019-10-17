//
// Created by lynxcat on 2019/10/11.
//

#include "response.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

char *CRLF = "\r\n";

char *contents = "/Users/lynxcat/Sites/code/c/http/contents";

char *startLine = "HTTP/1.1 200 OK";

char *contentLength = "Content-Length: %i";

char *server = "server: v server.";
char *date = "Date: %s";

char *error = "<html><title>lynx http.</title><body>file not find!</body></html>";

void ResponseGet(Request* request){

    char buf[256] = {0};
    strcat(buf, contents);
    strcat(buf, request->path);

    printf("%s", buf);

    FILE *stream = fdopen(request->fd, "r+");

    if(access(buf, R_OK) == 0){

        FILE *fp = fopen(buf,"rb");

        fseek(fp, 0L, SEEK_END);
        long flen = ftell(fp);

        char *body = (char *) malloc(flen + 1);

        if(body == NULL){
            fclose(fp);
            fclose(stream);
        }
        fseek(fp, 0L, SEEK_SET); /* 定位到文件开头 */
        fread(body, flen,1, fp); /* 一次性读取全部文件内容 */
        body[flen] = '\0';
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

    }else{
        fprintf(stream, "%s", startLine);
        fprintf(stream, "%s", CRLF);
        fprintf(stream, contentLength, strlen(error));
        fprintf(stream, "%s", CRLF);
        fprintf(stream, "%s", CRLF);
        fprintf(stream, "%s", error);
    }

    fclose(stream);
}