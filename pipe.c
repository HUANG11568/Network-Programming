#include<stdio.h>
#include<unistd.h>
#define MAXBUF 30

int main()
{
    int fd[2] = {0};
    pid_t pid;
    char buf[MAXBUF] = {};
    char input[] = "hello pipe!";
    char input2[] = "i am fine! and you?";
    int read_len = 0;

    /*管道初始化*/
    pipe(fd);

    pid = fork();
    if (0 == pid)
    {
        /*子进程*/
        write(fd[1], input, sizeof(input));
        sleep(2); /*管道内的数据为无主数据， 任何人均可读走， 此处保证父进程能读到数据*/

        read_len = read(fd[0], buf, MAXBUF);
        //puts(buf);
        printf("child buf:%s, readlen:%d\n", buf, read_len);
    }
    else
    {
        /*父进程*/
        read_len = read(fd[0], buf, MAXBUF);
        write(fd[1], input2, sizeof(input2));
        //puts(buf);
        printf("father buf:%s, readlen:%d\n", buf, read_len);
        sleep(3);
    }

    return 0;
}