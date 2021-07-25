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

    if (argc != 2)
    {
        error_handing("please enter port!");
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
        //验证TCP的数据传输不存在边界，客户端多次write后服务器，只需一次read 
        //sleep(10);
        while (read_len = read(client_sock, msg, MSG_LEN))
        {
            if(-1 == read_len)
            {
                error_handing("read() error!");
                break;
            }

            write(client_sock, msg, read_len);
        }
        if(0 == read_len)
        {
            fputs("read end!\n", stdout);
            close(client_sock);
        }
        
        
    }
    




    return 0;
}