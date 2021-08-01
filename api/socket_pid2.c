#include<stdio.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<stdlib.h>

#define MSG_FILE "./msgfile"
#define MSG_MAX 50

int main()
{
    struct sockaddr_un server_addr;
    int server, client;
    socklen_t sock_len = 0;
    char buf[MSG_MAX] = {0};
    int len = 0;
    server = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (-1 == server)
    {
        printf("socket() error!\n");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_LOCAL;
    strncpy(server_addr.sun_path, MSG_FILE, strlen(MSG_FILE));
    //unlink(MSG_FILE);

    sock_len = sizeof(server_addr);
    if (-1 == connect(server, (struct sockaddr*)&server_addr, sock_len))
    {
        printf("connect() error!\n");
        exit(1);
    }

    fgets(buf, sizeof(buf), stdin);
    write(server, buf, sizeof(buf));
    printf("buf:%s\n", buf);
        
    

    close(server);
    return 0;



}