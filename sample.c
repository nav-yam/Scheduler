#define _POSIX_SOURCE
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include "simpleShell.c"

struct node
{
    int val;
};
int main()
{
    // int f= fork();
    // if(f == 0){
    //     int j = 1;
    //     while (1)
    //     {
    //         printf("*");
    //         for (int i = 0; i < 50000; i++)
    //         {
    //             j++;
    //         }

    //     }

    // }
    // else {
    //     sleep(1);
    //     int g = fork();
    //     if (g == 0) {
    //         printf("Calling SIGSTOP\n");
    //         kill(f, SIGSTOP);
    //         sleep(1);
    //         exit(12);
    //     }
    //     else {
    //         wait(NULL);
    //         printf("Calling SIGCONT\n");
    //         kill(f, SIGCONT);
    //         sleep(1);
    //         printf("Calling SIGKILL\n");
    //         kill(f, SIGKILL);
    //     }
    // }
    // printf("ok 1\n");

    int f = fork();
    if (f == 0)
    {
        int i = launch("submit ./fib");
        exit(0);
    }

    int g = fork();
    if (g == 0)
    {
        int j = launch("submit ./hello");
        exit(0);
    }

    int h = fork();
    if (h == 0)
    {
        int k = launch("ls");
        printf("ok\n");
        exit(0);
    }

    return 0;
}