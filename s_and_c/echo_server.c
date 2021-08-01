#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/socket.h>
#include<signal.h>
#include<sys/wait.h>
#include<errno.h>

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
    struct sigaction act;
    int server_sock, client_sock;
    char msg[MSG_LEN] = {0};
    int read_len = 0;
    int i = 0;
    socklen_t ser_addr_len = 0;
    socklen_t opt_len = 0;
    int opt;
    int result;
    pid_t pid;

    memset(&act, 0, sizeof(act));
    act.sa_handler = process_exit;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    

    sigaction(SIGCHLD, &act, 0);
    
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
    while(1)
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
        }
        
        pid = fork();
        if (0 == pid)
        {
            //子进程负责通信
            //验证TCP的数据传输不存在边界，客户端多次write后服务器，只需一次read 
            //sleep(10);
            close(server_sock);/*子进程中关闭服务器套接字*/
            while (read_len = read(client_sock, msg, MSG_LEN))
            {
                if(-1 == read_len)
                {
                    error_handing("read() error!");
                    break;
                }

                write(client_sock, msg, read_len);
            }
            if(0 == read_len)
            {
                fputs("read end!\n", stdout);
                close(client_sock);
            }

            //子进程退出返回0
            close(client_sock);
            return 0;

        }
        else
        {
            close(client_sock);/*父进程中关闭客户端套接字*/
        }        
    }
    close(server_sock);
    return 0;
}