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
    strncpy(server_addr.sun_path, MSG_FILE, strlen(MSG_FILE)+1);
    unlink(MSG_FILE);

    sock_len = sizeof(server_addr);
    if (-1 == bind(server, (struct sockaddr*)&server_addr, sock_len))
    {
        printf("bind() error!\n");
        close(server);
        unlink(MSG_FILE);            
        exit(1);
    }

    if (-1 == listen(server, 5))
    {
        printf("listen() error!\n");
        close(server);
        unlink(MSG_FILE);          
        exit(1);        
    }
    while(1)
    {
        client = accept(server, (struct sockaddr*)&server_addr, &sock_len);
        if (-1 == client)
        {
            printf("accept() error!\n");
            close(server);
            unlink(MSG_FILE);
            exit(1);        
        }

        memset(buf, 0, sizeof(buf));
        len = read(client, buf, sizeof(buf));
        if(0 == len)
        {
            close(client);
            unlink(MSG_FILE);
        }

        printf("buf:%s\n", buf);
        
    }

    close(server);
    return 0;



}