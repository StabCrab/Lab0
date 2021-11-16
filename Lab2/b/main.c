#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>


int main ()
{
    pid_t pid;
    char buff[1000] = {'\0'};

    unlink("FIFO");
    if((mkfifo("FIFO", 0777)) == -1)
    {
        printf("ERROR");
        exit(-1);
    }

    pid = fork();
    if (pid == 0)
    {
        int fd = open("FIFO", O_RDONLY);
        if(fd == -1)
        {
            printf("ERROR");
            exit(-1);
        }

        read(fd, buff, 1000);
        close(fd);
        time_t timer = time(0);
        printf("child time: %s", ctime(&timer));
        printf("parent time: %s", buff);
        exit(0);
    }
    else //PARENT - target
    {
        int fd = open("FIFO", O_WRONLY);
        if(fd == -1)
        {
            printf("ERROR");
            exit(0);
        }

        time_t timer = time(0);
        sleep(5);
        sprintf(buff, "%sparent pid: %d", ctime(&timer) , getpid());
        write(fd, buff, 1000);
        close(fd);
        wait(NULL);

    }
}
