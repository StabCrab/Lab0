#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>

void onExit(int sig)
{
    printf("End program\n");
    exit(0);
}

int main()
{
    signal(SIGINT, onExit);
    key_t key = ftok("file", 'a');
    int data = shmget(key, 64, IPC_EXCL);
    if (data == -1)
    {
        printf("ERROR\n");
        exit(-1);
    }
    char* address = shmat(data, NULL, 0 );
    if(address == (char*)-1)
    {
        printf("ERROR\n");
        exit(0);
    }
    time_t timer = time(0);
    printf("time input: %spid input: %d\n%s", ctime(&timer), getpid(), address);
    return 0;
}
