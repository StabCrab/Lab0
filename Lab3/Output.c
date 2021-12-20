#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/types.h>

typedef struct package
{
    time_t time;
    pid_t pid;
} package;

int main()
{
    key_t key = ftok("file", 'a');
    int shmid = shmget(key, 64, IPC_CREAT | 0666);
    if( shmid == -1 )
    {
        printf("Can't create shared memory\n");
        return -1;
    }

    time_t timer;

    package pack;
    pack.pid = getpid();
    while(1)
    {
        timer = time(0);

        pack.time = timer;

        void* data = shmat(shmid, NULL,0);
        if (data < 0)
        {
            printf("ERROR");
            return -1;
        }
        *((package*)data) = pack;
        sleep(5);
        shmdt(data);
    }
    return 0;
}
