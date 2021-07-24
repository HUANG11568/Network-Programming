#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

void error_handing(char *msg)
{
    fputs(msg, stderr);
    fputs("\n", stderr);

    return;
}
int main(int argc, char *argv[])
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sock_len;
    char msg[] = "hello word!!";

    if(argc != 2)
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
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    
    if (-1 == bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        error_handing("bind() error！");
        exit(1);
    }

    if (-1 == listen(server_sock, 5))
    {
        error_handing("listen() error!");
        exit(1);
    }

    sock_len = sizeof(server_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&server_addr, &sock_len);
    if (-1 == client_sock)
    {
        error_handing("accept() error!");
        exit(1);
    }

    write(client_sock, msg, sizeof(msg));
    close(client_sock);
    close(server_sock);

    return 0;

}