#include<stdio.h>
#include<unistd.h>
#define MAXBUF 30

int main()
{
    int fd[2] = {0};
    int fd2[2] = {0};
    pid_t pid;
    char buf[MAXBUF] = {};
    char input[] = "hello pipe!";
    char input2[] = "i am fine! and you?";
    int read_len = 0;

    /*管道初始化*/
    pipe(fd);
    pipe(fd2);

    pid = fork();
    if (0 == pid)
    {
        /*子进程*/
        write(fd[1], input, sizeof(input));
       
        read_len = read(fd2[0], buf, MAXBUF);
        //puts(buf);
        printf("child buf:%s, readlen:%d\n", buf, read_len);
    }
    else
    {
        /*父进程*/
        write(fd2[1], input2, sizeof(input2));
        read_len = read(fd[0], buf, MAXBUF);       
        //puts(buf);
        printf("father buf:%s, readlen:%d\n", buf, read_len);
        sleep(3);
    }

    return 0;
}