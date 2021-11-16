
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
int main()
{
    int pipe_fd[2];
    if(pipe(pipe_fd) < 0)
    {
        printf("Failed pipe open\n");
        exit(-1);
    }


    pid_t pid = fork();
    if(pid == 0)
    {
        close(pipe_fd[1]);
        long int array[2];
        read(pipe_fd[0], array, sizeof(long int) * 2);
        time_t timer = time(0);
        printf("child: %s", ctime(&timer));
        timer = array[0];
        printf("parent: %s", ctime(&timer));
        printf("parent pid: %ld", array[1]);
    }
    else
    {
        close(pipe_fd[0]);
        long int array[2];
        array[0] = time(0);
        array[1] = getpid();
        sleep(5);
        write(pipe_fd[1], array , sizeof(long int) * 2);
        close(pipe_fd[1]);
        wait(NULL);

    }

    return 0;
}
