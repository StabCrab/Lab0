#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

char a = 0;
pthread_cond_t cond;
pthread_mutex_t mutex;
void* outputThreadFunc()
{
    while (1)
    {
        sleep(1);
        pthread_mutex_lock(&mutex);
        a++;
        pthread_mutex_unlock(&mutex);
        pthread_cond_broadcast(&cond);
    }
}

void* inputThreadFunc()
{
    while (1)
    {
        pthread_cond_wait(&cond, &mutex);
        printf("tid: %ld, ", pthread_self());
        printf("%d\n", a);
    }
}

int main() {
    pthread_t inputThread[10];
    pthread_t outputThread[1];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond,NULL);

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


    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
