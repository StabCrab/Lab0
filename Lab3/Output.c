#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
int shmid;

void atExit(int sig)
{
    printf("Program finished\n");
    struct shmid_ds *buf = 0;
    shmctl(shmid, IPC_RMID, buf);

    exit(0);
}

int main()
{
    signal(SIGINT, atExit);
    key_t key = ftok("file", 'a');
    shmid = (shmget(key, 64, IPC_CREAT | 0666));
    if( shmid == -1 )
    {
        printf("Can't create shared memory\n");
        exit(-1);
    }
    char * address = shmat(shmid, NULL, 0 );
    if(address  == (char*)-1)
    {
        printf("Shmat err\n");
        exit(-1);
    }

    time_t timer = time(0);
    time_t bufTimer = timer;

    if (strlen(address) != 0)
    {
        printf("There is already output\n");
        exit(-1);
    }

    while(1)
    {
        timer = time(0);
        if (timer != bufTimer)
        {
            bufTimer = timer;
            sprintf(address, "time output: %spid output: %d\n", ctime(&timer), getpid());
        }
    }
    return 0;
}
