#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/socket.h>

#define MSG_LEN 50 
void error_handing(char *msg)
{
    fputs(msg, stderr);
    fputs("\n", stderr);
    return;
}
int main(int argc, char* argv[])
{
    struct sockaddr_in server_addr, client_addr;
    int server_sock, client_sock;
    char msg[MSG_LEN] = {0};
    int read_len = 0;
    int i = 0;
    socklen_t ser_addr_len = 0;
    FILE *fd;

    if (argc != 2)
    {
        error_handing("please enter port!");
        exit(1);
    }

    fd = fopen("./test.txt", "rb");
    if (!fd)
    {
        error_handing("fopen error!");
        exit(1);
    }
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == server_sock)
    {
        error_handing("socket() error!");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (-1 == bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        error_handing("bind() error!");
        exit(1);
    }

    if (-1 == listen(server_sock, 5))
    {
        error_handing("listen() error!");
        exit(1);
    }
    while(1)
    {
        ser_addr_len = sizeof(server_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&server_addr, &ser_addr_len);
        if (-1 == client_sock)
        {
            error_handing("eccept() error!");
            exit(1);
        }
        else
        {
            printf("connect client:%d\n", ++i);
        }

        
        while (1)
        {
            //fread的返回值为n(读取的次数)
            read_len = fread(msg, 1, 5, fd);
            if(read_len < 5)
            {
                write(client_sock, msg, read_len);
                break;
            }

            write(client_sock, msg, 5);
        }

        shutdown(client_sock, SHUT_WR);
        read(client_sock, msg, MSG_LEN);
        printf("recv msg:%s\n", msg);

        fclose(fd);
        close(client_sock);    
    }
    
    return 0;
}