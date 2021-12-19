#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

char a = 0;
pthread_rwlock_t lock;
void* outputThreadFunc()
{
    while (1)
    {
        sleep(1);
        pthread_rwlock_rdlock(&lock);
        a++;
        pthread_rwlock_unlock(&lock);
    }
}

void* inputThreadFunc()
{
    while (1)
    {
        pthread_rwlock_wrlock(&lock);
        printf("tid: %ld, ", pthread_self());
        printf("%d\n", a);
        pthread_rwlock_unlock(&lock);
        sleep(1);
    }
}

int main() {
    pthread_t inputThread[10];
    pthread_t outputThread[1];

    pthread_rwlock_init(&lock, NULL);

    for (int i = 0; i < 1; i++)
    {
        pthread_create(&outputThread[i], NULL, outputThreadFunc, NULL);
    }
    for (int i = 0; i < 10; i++)
    {
        pthread_create(&inputThread[i], NULL, inputThreadFunc, NULL);
    }
    for (int i = 0; i < 10; i++)
    {
        pthread_join(inputThread[i], NULL);
    }
    for (int i = 0; i < 1; i++)
    {
        pthread_join(outputThread[i], NULL);
    }

    pthread_rwlock_destroy(&lock);

    return 0;
}
