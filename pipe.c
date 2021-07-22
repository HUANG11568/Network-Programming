#include<stdio.h>
#include<unistd.h>
#define MAXBUF 30

int main()
{
    int fd[2] = {0};
    pid_t pid;
    char buf[MAXBUF] = {};
    char input[] = "hello pipe!";
    int read_len = 0;

    /*管道初始化*/
    pipe(fd);

    pid = fork();
    if (0 == pid)
    {
        /*子进程*/
        write(fd[1], input, sizeof(input));
    }
    else
    {
        /*父进程*/
        read_len = read(fd[0], buf, MAXBUF);
        puts(buf);
        printf("buf:%s, readlen:%d\n", buf, read_len);
    }

    return 0;
}