#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
void exitOutput(){
    printf("Process %d ended. Parent is %d \n", getpid(), getppid());
}

int main()
{
    pid_t p;
    int status;
    atexit(exitOutput);
    p = fork();
    if (p == -1)
    {
        exit(1);
    }
    else if (p == 0)
    {
        printf("Child process. PID: %d, PPID: %d \n", getpid(), getppid());
        exit(0);
    }
    else
    {
        printf("Parent process. PID: %d, PPID: %d.\n", getpid(), getppid());
        printf("Waiting child process to end\n");
        wait(&status);
        printf("Child exit status: %d\n",WEXITSTATUS(status));
    }
}
