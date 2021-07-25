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
    int client_sock;
    struct sockaddr_in send_addr, recv_addr;
    char msg[MSG_LEN] = {};
    socklen_t addr_len;
    int read_len;

    if(argc != 3)
    {
        error_handing("please enter ip and port!");
        exit(1);
    }
    client_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(-1 == client_sock)
    {
        error_handing("socket() error!");
        exit(1);
    }
    memset(&send_addr, 0, sizeof(send_addr));
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &send_addr.sin_addr.s_addr);

    while(1)
    {
        fputs("enter q to quit\n", stdout);
        fgets(msg, MSG_LEN, stdin);
        if (!strcasecmp("q\n", msg))
        {
            fputs("connect end!", stdin);
            break;
        }

        /*sendto会自动分配ip和端口；TCP使用connect函数完成*/
        /*
        1.向UDP套接字注册目标IP和端口号
        2.传输数据
        3.删除UDP套接字注册的目标地址信息

        备注：可以使用connect建立代替步骤1，此时后续只需传输数据（步骤1和步骤3占整个通讯过程1/3时间）
        */
        if(-1 == connect(client_sock, (struct sockaddr*)&send_addr, sizeof(send_addr)))
        {
            error_handing("connect error!");
            exit(1);
        }
        
        //使用connect后，可以使用read和write操作套接字
        write(client_sock, msg, strlen(msg));
        read_len = read(client_sock, msg, MSG_LEN);
        msg[read_len] = '\0';
        
        /*
        sendto(client_sock, msg, strlen(msg), 0, (struct sockaddr*)&send_addr, sizeof(send_addr));
        //recvfrom中的addr_len需要给准确值
        addr_len = sizeof(recv_addr);
        read_len = recvfrom(client_sock, msg, MSG_LEN, 0, (struct sockaddr*)&recv_addr, &addr_len);
        msg[read_len] = '\0';
        */
        printf("recv msg:%s\n", msg);
    }

    close(client_sock);

    return 0;
}