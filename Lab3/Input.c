#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/shm.h>
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
        printf("ERROR");
        return -1;
    }
    int shmid= shmget(key, 64, IPC_EXCL);
    if (shmid == -1)
    {
        printf("ERROR\n");
        return -1;
    }

    void* data = shmat(shmid, NULL, 0);
    if (data == (void*)-1)
    {
            printf("ERROR");
            return -1;
    }
    package pack = *((package*)data);
    if (data < 0)
    {
        printf("ERROR");
        return -1;
    }

    time_t timer = time(NULL);
    printf("Input time: %sInput pid: %d\n", ctime(&timer),getpid());
    printf("Output time: %sOutput pid: %d\n", ctime(&pack.time), pack.pid);
    shmdt(data);
    return 0;
}
