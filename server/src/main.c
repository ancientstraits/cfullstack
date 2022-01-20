#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static char* path(const char* resp) {
    char* start = strchr(resp, '/');
    char* end = strchr(start, ' ');
    char* ret = malloc(end - start);
    strncpy(ret, start, end - start);
    return ret;
}

static char* static_dir(const char* root, const char* req_path) {
    char filename[256];
    strcpy(filename, root);
    strcat(filename, req_path);

    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror(filename);
        return NULL;
    }

    fseek(fp, 0, SEEK_CUR);
    size_t filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buf = malloc(filesize + 1);
    fread(buf, filesize, 1, fp);
    buf[filesize] = '\0';

    fclose(fp);
    
    return buf;
}

int main(int argc, char** argv) {
    if (argc > 2)
        return 1;

    const char* send = 
        "HTTP/1.1 200 OK\n"
        "Connection: Keep-Alive\n"
        "Keep-Alive: timeout=1, max=10"
        "Context-Type: text/html\r\n\r\n"
        "<!DOCTYPE html>"
        "<h1>Hello World</h1>"
    ;
    char resp[2048];

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr = {
            .s_addr = htonl(INADDR_ANY),
        },
        .sin_port = htons((size_t)argv[1]),
    };

    if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Server");
        close(listenfd);
        return 1;
    }

    listen(listenfd, 10);

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));

    while (1) {
        int connfd = accept(listenfd, NULL, NULL);
        write(connfd, send, strlen(send));
        recv(connfd, resp, 2048, 0);
        char* s = path(resp);
        printf("'%s'\n", s);
        char* f = static_dir("client", s);
        if (f)
            printf("%s\n", f);
        free(f);
        free(s);
        close(connfd);
    }

    close(listenfd);

    return 0;
}