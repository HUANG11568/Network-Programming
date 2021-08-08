#include<stdio.h>
#include<unistd.h>
int main(int argc, char *argv[])
{
    int fd;
    char *str1 = "aaaaaaaaaaaaaaaa\n";
    char *str2 = "bbbbbbbbbbbbbbbb\n";

    fd = dup(fileno(stdout));
    FILE * w;

    w = fdopen(fd, "w");
    fputs(str1, w);

    fclose(w);

    fputs(str2, stdout);

    return 0;

}