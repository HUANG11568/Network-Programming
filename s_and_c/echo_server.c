#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/socket.h>
#include<signal.h>
#include<sys/wait.h>
#include<errno.h>
#include<sys/time.h>
#include<sys/select.h>

#define MSG_LEN 50 
void error_handing(char *msg)
{
    fputs(msg, stderr);
    fputs("\n", stderr);
    return;
}
//子进程退出时防止产生僵尸进程
void process_exit(int sig)
{
    int status;
    int return_value;

    waitpid(-1, &status, WNOHANG);

    //判断子进程是否正常返回，打印返回值
    if(WIFEXITED(status))
    {
        printf("child process exit! return:%d\n", WEXITSTATUS(status));
    }
    else
    {
        printf("child process exit error!\n");        
    }
}

int main(int argc, char* argv[])
{
    struct sockaddr_in server_addr, client_addr;
    int server_sock, client_sock;
    char msg[MSG_LEN] = {0};
    int read_len = 0;
    int i = 0;
    socklen_t ser_addr_len = 0;
    socklen_t opt_len = 0;
    int opt;
    int result;
    pid_t pid;
    fd_set reads, cp_reads;  //由于每次select调用成功后，文件描述符集合会被写，搞个副本
    int fd_max; //记录循环的最大值
    struct timeval time;
    int read_num;

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

    opt_len = sizeof(opt);
    opt = 1;
    //time_wait的端口能分配使用
    result = setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, opt_len);
    if (-1 == result)
    {
        printf("setsockopt() error! %d : %s\n", errno, strerror(errno));
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

   
    FD_ZERO(&reads);
    FD_SET(server_sock, &reads);
    fd_max = server_sock;

    while(1)
    {
        //由于select阻塞运行，此处设置超时时间
        time.tv_sec = 3;
        time.tv_usec = 0;

        cp_reads = reads;
        //第一个参数+1是因为文件描述符从0算起多一个
        read_num = select(fd_max+1, &cp_reads, 0, 0,&time);
        if(0 == read_num)
        {
            printf("select time out\n");
            continue;
        }
        else if(-1 == read_num)
        {
            printf("select error\n");
            break;
        }
        for (int j = 0; j <= fd_max; j++)
        {
            if(!FD_ISSET(j, &cp_reads))
            {
                continue;
            }
            if(j == server_sock)
            {
                ser_addr_len = sizeof(server_addr);
                client_sock = accept(server_sock, (struct sockaddr*)&server_addr, &ser_addr_len);
                if (-1 == client_sock)
                {
                    //若子进程退出时，会产生中断，erron为EINTR，此时重新调用accept
                    if(EINTR != errno)
                    {
                        printf("eccept() error! %d : %s\n", errno, strerror(errno));
                    }       
                    continue;
                }
                else
                {
                    printf("connect client:%d\n", ++i);
                    FD_SET(client_sock, &reads);
                    if(client_sock > fd_max)
                    {
                        fd_max = client_sock;
                    }
                }
            }
            else
            {
                //验证TCP的数据传输不存在边界，客户端多次write后服务器，只需一次read 
                //sleep(10);
                while (read_len = read(j, msg, MSG_LEN))
                {
                    if(-1 == read_len)
                    {
                        error_handing("read() error!");
                        break;
                    }

                    write(j, msg, read_len);
                }
                if(0 == read_len)
                {
                    fputs("read end!\n", stdout);
                    FD_CLR(j, &reads);
                    close(j);
                }
            }
        }
    }
    close(server_sock);
    return 0;
}