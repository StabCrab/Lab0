#include <stdio.h>
#include <string.h>
#include <sys/sem.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct Package
{
    time_t time;
    pid_t pid;
} pack;

struct sembuf sem_lock = { 0, -1, 0 };

int main()
{
    key_t key = ftok("file", 'a');
    if (key == -1)
    {
        printf("ERROR");
        return -1;
    }

    int shmemID = shmget(key, sizeof(pack), 0666);

    if (shmemID < 0)
    {
        printf("ERROR");
        return -1;
    }

    int semID = semget(key, 1, IPC_CREAT | 0666);
    if (semID < 0)
    {
        printf("ERROR");
        exit(-1);
    }

    semop(semID, &sem_lock, 1);
    void* buf = shmat(shmemID, NULL, 0);
    struct Package package = *((pack*)buf);

    if (buf < 0) {
        printf("ERROR");
        exit(-1);
    }

    time_t timer = time(NULL);
    printf("Input time: %sInput PID: %d\n", ctime(&timer), getpid());
    printf("Output time: %sOutput PID: %d\n", ctime(&(package.time)), package.pid);
    return 0;
}
