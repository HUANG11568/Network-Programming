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
    int sock;
    socklen_t len;
    int recv_len;
    int send_len;
    int result;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sock)
    {
        error_handing("socket error!");
        exit(1);
    }

    len = sizeof(recv_len);
    result = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &send_len, &len);
    if (-1 == result)
    {
        error_handing("getsockopt() error!");
        exit(1);
    }

    len = sizeof(recv_len);
    result = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recv_len, &len);
    if (-1 == result)
    {
        error_handing("getsockopt() error!");
        exit(1);
    }
    
    printf("send_buf:%d, recv_buf:%d\n", send_len, recv_len);

    recv_len = 3*1024;
    send_len = 0;
    result = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &send_len, sizeof(send_len));
    if (-1 == result)
    {
        error_handing("getsockopt() error!");
        exit(1);
    }

    result = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recv_len, sizeof(recv_len));
    if (-1 == result)
    {
        error_handing("getsockopt() error!");
        exit(1);
    }

    len = sizeof(recv_len);
    result = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &send_len, &len);
    if (-1 == result)
    {
        error_handing("getsockopt() error!");
        exit(1);
    }

    len = sizeof(recv_len);
    result = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recv_len, &len);
    if (-1 == result)
    {
        error_handing("getsockopt() error!");
        exit(1);
    }
    
    printf("send_buf:%d, recv_buf:%d\n", send_len, recv_len);

    return 0;
}