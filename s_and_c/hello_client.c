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
    char msg[STR_LEN];

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

    len = read(client_sock, msg, sizeof(msg));
    msg[STR_LEN -1] = '\0';
    fputs(msg, stdout);

    close(client_sock);

    return 0;
}