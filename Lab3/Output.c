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
    if (key == -1)
    {
        printf("Key creation error\n");
        return -1;
    }
    int shmid = shmget(key, 64, IPC_CREAT | 0666);
    if( shmid == -1 )
    {
        printf("Can't create shared memory\n");
        return -1;
    }

    time_t timer;

    package pack;
    pack.pid = getpid();

    struct shmid_ds check;
    shmctl(shmid, IPC_STAT, &check);
    if (check.shm_nattch >= 1)
    {
        printf("%s\n", "Output already exists");
        return -1;
    }
    void* data = shmat(shmid, NULL,0);
    if (data == (void*) -1)
    {
        printf("ERROR");
        return -1;
    }
    while(1)
    {
        timer = time(0);

        pack.time = timer;

        if (data < 0)
        {
            printf("ERROR");
            return -1;
        }
        *((package*)data) = pack;
        sleep(5);
    }
    shmdt(data);
    shmctl(shmid, IPC_RMID, &check);
    return 0;
}

