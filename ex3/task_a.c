#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long global = 0;

void *thread_0_handler(void *arg)
{
    long local_0 = 0;

    for (int i = 0; i < 50 * 1000 * 1000; i++)
    {
        local_0++;
        global++;
    }

    printf("Local 0: %ld\n", local_0);

    return NULL;
}

void *thread_1_handler(void *arg)
{
    long local_1 = 0;

    for (int i = 0; i < 50 * 1000 * 1000; i++)
    {
        local_1++;
        global++;
    }

    printf("Local 1: %ld\n", local_1);

    return NULL;
}

int main()
{
    pthread_t thread_0;
    pthread_t thread_1;

    pthread_create(&thread_0, NULL, &thread_0_handler, NULL);
    pthread_create(&thread_1, NULL, &thread_1_handler, NULL);

    pthread_join(thread_0, NULL);
    pthread_join(thread_1, NULL);

    printf("Global: %ld\n", global);

    return 0;
}
