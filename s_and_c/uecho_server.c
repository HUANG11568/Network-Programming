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
    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    char msg[MSG_LEN] = {};
    socklen_t addr_len;
    int read_len;

    if(argc != 2)
    {
        error_handing("please enter port!");
        exit(1);
    }
    server_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(-1 == server_sock)
    {
        error_handing("socket() error!");
        exit(1);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(-1 == bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        error_handing("bind error!");
        exit(1);
    }
    while(1)
    {
        sleep(5);
        /*UDP传输，使用recv到的地址；TCP传输时， 直接返回了套接字*/
        /*addr_len需要给准确值*/
        addr_len = sizeof(client_addr);
        read_len = recvfrom(server_sock, msg, MSG_LEN, 0, (struct sockaddr*)&client_addr, &addr_len);
        sendto(server_sock, msg, read_len, 0, (struct sockaddr*)&client_addr, addr_len);
    }
    close(server_sock);
    
    return 0;
}