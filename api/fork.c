#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<signal.h>
#include<string.h>

//父子进程拥有自己独立的全局变量和局部变量，修改互不干扰
int g_a = 100;

/*
SIGALRM  已到alarm调用时间
SIGINT   输入ctrl+c
SIGCHLD  子进程终止
*/
void time_out(int sig)
{
    if(SIGALRM == sig)
    {
        printf("time out!\n");
        alarm(2);
    }  
    return;
}
void process_exit(int sig)
{
    int status;
    int result = 0;
    if(SIGCHLD == sig)
    {
        result = waitpid(-1, &status, WNOHANG);
        if(-1 == result)
        {
            printf("waitpid error!");
            exit(1);
        }
        if (WIFEXITED(status))
        {
            printf("child process exit! exit value:%d\n", WEXITSTATUS(status));
        }
        printf("process exit\n");
    }  

    if(SIGINT == sig)
    {
        printf("enter ctrl+c\n");
    }  

    return;
}
int main()
{
    pid_t pid;
    int a = 10;
    int status;
    int result = 0;
    struct sigaction timeout;
    struct sigaction processeixt;
    /*signal(SIGALRM, time_out);
    signal(SIGINT, process_exit);
    signal(SIGCHLD, process_exit);*/

    //经实验使用memset初始化sigaction结构体也可到达目的
    //memset(&timeout, 0, sizeof(timeout));
    timeout.sa_handler = time_out;
    sigemptyset(&timeout.sa_mask);
    timeout.sa_flags = 0;

    //memset(&processeixt, 0, sizeof(processeixt));
    processeixt.sa_handler = process_exit;
    sigemptyset(&processeixt.sa_mask);
    processeixt.sa_flags = 0;
    
    //第一个参数：信号信息 第二个参数： 当前设置的sigaction结构体 第三个参数：取得之前设置的sigaction结构体
    sigaction(SIGALRM, &timeout, 0);
    sigaction(SIGINT, &processeixt, 0);
    sigaction(SIGCHLD, &processeixt, 0);
    alarm(3);

    pid = fork();
    if(0 == pid)
    {
        //子进程
        sleep(3);
        g_a++;
        a++;
        printf("this child process! g_a:%d, a:%d\n", g_a, a);
    }
    else
    {
        //父进程
        g_a--;
        a--;         
        printf("this father process! g_a:%d, a:%d\n", g_a, a);

        //wait为阻塞的方式等待子进程结束
        //wait(&status);

        //waitpid以非阻塞的方法运行
        //参数1表示进程号 -1表示任何进程
        //返回值 -1失败 进程id 0未有进程结束

        /*result = 0;
        while(!result)
        {
            result = waitpid(-1, &status, WNOHANG);
            if(-1 == result)
            {
                printf("waitpid error!");
                exit(1);
            }
            sleep(3);
            printf("wait......\n");
        }
        if (WIFEXITED(status))
        {
            printf("child process exit! exit value:%d\n", WEXITSTATUS(status));
        }*/
        while(1)
        {
            printf("main wait\n");
            sleep(20);
        }
        
    }

    return 0;
}