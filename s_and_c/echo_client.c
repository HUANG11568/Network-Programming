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

    if (argc != 3)
    {
        error_handing("please enter ip and port!");
        exit(1);
    }
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

    while(1)
    {
        fputs("enter q to quit\n", stdout);
        fgets(msg, MSG_LEN, stdin);
        if (!strcasecmp("q\n", msg))
        {
            fputs("connect end!", stdin);
            break;
        }
        send_len = write(sock, msg, strlen(msg));

        rcv_len = 0;
        while(rcv_len < send_len)
        {
            read_len = read(sock, &msg[rcv_len], MSG_LEN -1);
            if(-1 == read_len)
            {
                error_handing("read() error");
                exit(1);
            }
            else
            {
                rcv_len += read_len;
            }
        }
        
        msg[rcv_len] = '\0';
        printf("msg from server:%s\n", msg);
    }
    
    close(sock);

    return 0;
}