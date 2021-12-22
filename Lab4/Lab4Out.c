#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>


typedef struct Package
{
    time_t time;
    pid_t pid;
} pack;

struct sembuf semLock = { 0, -1, 0 }, semOpen = { 0,1,0 };

int main() {

    struct Package package;
    package.time = time(NULL);
    package.pid = getpid();

    key_t key = ftok("file", 'a');
    if (key == -1)
    {
        printf("ERROR");
        return -1;
    }

    int shmemID = shmget(key, sizeof(package), IPC_CREAT | 0666);

    if (shmemID < 0)
    {
        printf("ERROR");
        return -1;
    }
    struct shmid_ds info;
    shmctl(shmemID, IPC_STAT, &info);
    if (info.shm_nattch >= 1)
    {
        printf("There is another process");
        return -1;
    }

    int semID = semget(key, 1, IPC_CREAT | 0666);
    if (semID < 0)
    {
        printf("ERROR");
        return -1;
    }

    void* buf = shmat(shmemID, NULL, 0);

    semop(semID, &semOpen, 1);
    while (1)
    {

        semop(semID, &semLock, 1);

        if (buf < 0)
        {
            printf("ERROR");
            return -1;
        }

        *((pack*)buf) = package;
        semop(semID, &semOpen, 1);
        sleep(5);
    }
    semctl(semID, 0, IPC_RMID);
    shmdt(buf);
    shmctl(shmemID, IPC_RMID, NULL);
    return 0;
}
