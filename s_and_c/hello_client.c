#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#define STR_LEN 40

void error_handing(char *msg)
{
    fputs(msg, stderr);
    fputs("\n", stderr);

    return;
}
int main(int argc, char *argv[])
{
    int  client_sock;
    struct sockaddr_in client_addr;
    int len;
    int str_len = 0;
    char msg[STR_LEN];
    int index = 0;
    int count = 0;

    if (argc != 3)
    {
        error_handing("please enter ip and port!");
        exit(1);
    }
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == client_sock)
    {
        error_handing("socket() error!");
        exit(1);
    }

    client_addr.sin_family = AF_INET;
    //client_addr.sin_addr.s_addr = htonl());
    inet_pton(AF_INET, argv[1], &client_addr.sin_addr.s_addr);
    client_addr.sin_port = htons(atoi(argv[2]));

    if (-1 == connect(client_sock, (struct sockaddr*)&client_addr, sizeof(client_addr)))
    {
        error_handing("connect() error!");
        exit(1);
    }

    /*TCP套接字，传输从数据不存在数据边界
    服务器发来12个字节， 客户端一次读取2个字节共读取了6次*/
    while(len = read(client_sock, msg + index, 2))
    {
        if(-1 == len)
        {
            error_handing("read() error!");
            exit(1);
        }
        str_len += len;
        index += 2;
        count ++;

    }
    msg[STR_LEN -1] = '\0';
    fputs(msg, stdout);
    printf("read_count:%d, str_len:%d\n", count, str_len);

    close(client_sock);

    return 0;
}