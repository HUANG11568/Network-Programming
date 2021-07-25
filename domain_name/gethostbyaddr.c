#include<stdio.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<errno.h>

void error_handing(char *msg)
{
    fputs(msg, stderr);
    fputs("\n", stderr);
    return;
}
int main(int argc, char* argv[])
{
    struct hostent *host;
    struct in_addr ip_addr;
    struct sockaddr_in addr;
    int i = 0;
    if (argc != 2)
    {
        error_handing("please enter ip addr!");
        exit(1);
    }

    memset(&ip_addr, 0, sizeof(ip_addr));
    ip_addr.s_addr = inet_addr(argv[1]);
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    //gethostbyaddr成功时返回hostent结构体地址， 失败返回NULL
    //gethostbyaddr连续调用，返回值会被最后一次的值覆盖
    host = gethostbyaddr((char*)&addr.sin_addr, 4, AF_INET);
    if (!host)
    {
        error_handing("gethostbyname() fail!");
        herror("fail reason:");
        exit(1);
    }

    //官方域名
    printf("official name:%s\n", host->h_name);
    //IP地址类型
    printf("addr_type:%s\n", host->h_addrtype == AF_INET? "AF_INET":"AF_INET6");
    //IP地址地址长度， IPv4为4个字节， IPv6为16个字节
    printf("addr_length:%d\n", host->h_length);
    //其他域名
    while(host->h_aliases[i])
    {
        printf("aliases NO.%d:%s\n", i+1, host->h_aliases[i]);
        i++;
    }
    
    i = 0;
    //其他域名域名的IP
    //h_addr_list为char*类型实际存储的内容为IPv4或者IPv6的值
    while (host->h_addr_list[i])
    {
        printf("IP addr NO.%d:%s\n", i+1, inet_ntoa(*(struct in_addr*)(host->h_addr_list[i])));
        i++;
    }

    return 0;
}