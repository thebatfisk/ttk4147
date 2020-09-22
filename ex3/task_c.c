#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t sem;
long global = 0;

void *thread_0_handler(void *arg)
{
    long local_0 = 0;

    for (int i = 0; i < 50 * 1000; i++)
    {
        local_0++;
        sem_wait(&sem);
        global++;
        sem_post(&sem);
    }

    printf("Local 0: %ld\n", local_0);

    return NULL;
}

void *thread_1_handler(void *arg)
{
    long local_1 = 0;

    for (int i = 0; i < 50 * 1000; i++)
    {
        local_1++;
        sem_wait(&sem);
        global++;
        sem_post(&sem);
    }

    printf("Local 1: %ld\n", local_1);

    return NULL;
}

int main()
{
    pthread_t thread_0;
    pthread_t thread_1;

    sem_init(&sem, 0, 1);

    pthread_create(&thread_0, NULL, &thread_0_handler, NULL);
    pthread_create(&thread_1, NULL, &thread_1_handler, NULL);

    pthread_join(thread_0, NULL);
    pthread_join(thread_1, NULL);

    printf("Global: %ld\n", global);

    sem_destroy(&sem);

    return 0;
}
