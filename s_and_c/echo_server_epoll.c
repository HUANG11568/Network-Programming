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
#include<sys/epoll.h>
#include<fcntl.h>

#define MSG_LEN 3 
#define EPOLL_SIZE 100
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
    int epfd;
    int fd_max; //记录循环的最大值
    struct epoll_event event;
    struct epoll_event *ep_event;
    struct timeval time;
    int read_num;
    int epoll_cnt = 0;
    int fd_client = 0;
    int flag;

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
        printf("bind() error! %d : %s\n", errno, strerror(errno));
        exit(1);
    }

    if (-1 == listen(server_sock, 5))
    {
        error_handing("listen() error!");
        exit(1);
    }

    epfd = epoll_create(EPOLL_SIZE);
    event.data.fd = server_sock;
    event.events = EPOLLIN;

    ep_event = malloc(sizeof(struct epoll_event)*EPOLL_SIZE);
    memset(ep_event, 0, sizeof(struct epoll_event)*EPOLL_SIZE);

    epoll_ctl(epfd, EPOLL_CTL_ADD, server_sock, &event);

    while(1)
    {
        epoll_cnt = epoll_wait(epfd, ep_event, EPOLL_SIZE, -1);
        if (-1 == epoll_cnt)
        {
            printf("select error\n");
            break;
        }

        for (int j = 0; j < epoll_cnt; j++)
        {
            if(ep_event[j].data.fd == server_sock)
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
                    event.data.fd = client_sock;
                    flag = fcntl(client_sock, F_GETFL, 0);
                    fcntl(client_sock, F_SETFL, flag|O_NONBLOCK);
                    event.events = EPOLLIN|EPOLLET;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client_sock, &event);
                    printf("connect client:%d\n", ++i);
                }
            }
            else
            {
        
                while(1)
                {
                    fd_client = ep_event[j].data.fd;
                    printf("client_fd:%d\n", fd_client);
                    read_len = read(fd_client, msg, MSG_LEN);
                    
                    if(-1 == read_len)
                    {
                        if(EAGAIN != errno)
                            error_handing("read() error!");
                        break;
                    }
                    else if(0 == read_len)
                    {
                        fputs("read end!\n", stdout);
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd_client, NULL);
                        close(fd_client);
                        break;
                    }
                    else
                    {
                        write(fd_client, msg, read_len);
                    }
                }

                
            }
        }
    }
    close(server_sock);
    return 0;
}