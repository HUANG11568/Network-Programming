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
    struct sockaddr_in sock_addr;
    int sock;
    char msg[MSG_LEN] = {0};
    int rcv_len;
    int read_len;
    int send_len = 0;
    FILE *fd;
    int i = 0;

    if (argc != 3)
    {
        error_handing("please enter ip and port!");
        exit(1);
    }

    fd = fopen("recv.txt", "wb");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock)
    {
        error_handing("socket() error!");
        exit(1);
    }
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &sock_addr.sin_addr.s_addr);

    if(-1 == connect(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr)))
    {
        error_handing("connect() error!");
        exit(1);
    }

    //读取到EOF时结束
    while(read_len = read(sock, msg, MSG_LEN))
    {
        if(-1 == read_len)
        {
            error_handing("read() error");
            exit(1);
        }
        else
        {
            fwrite(msg, read_len, 1, fd);
        }       
        printf("msg from server times:%d\n", ++i);
    }

    write(sock, "recv ok!", 9);
    fclose(fd);
    close(sock);

    return 0;
}