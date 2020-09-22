#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define EATING_TIME 500
#define NUMBER_OF_EATINGS 1000

struct forks
{
    pthread_mutex_t one;
    pthread_mutex_t two;
    pthread_mutex_t three;
    pthread_mutex_t four;
    pthread_mutex_t five;
};

struct forks forks;
pthread_mutex_t waiter;

void eating(void)
{
    usleep(EATING_TIME);
}

void *phil_1(void *arg)
{
    for (int i = 0; i <= NUMBER_OF_EATINGS; i++)
    {
        printf("Philosopher 1 THINKING\n");
        pthread_mutex_lock(&waiter);
        pthread_mutex_lock(&forks.five);
        pthread_mutex_lock(&forks.one);
        pthread_mutex_unlock(&waiter);
        printf("Philosopher 1 EATING\n");
        eating();
        pthread_mutex_unlock(&forks.one);
        pthread_mutex_unlock(&forks.five);
    }

    return NULL;
}

void *phil_2(void *arg)
{
    for (int i = 0; i <= NUMBER_OF_EATINGS; i++)
    {
        printf("Philosopher 2 THINKING\n");
        pthread_mutex_lock(&waiter);
        pthread_mutex_lock(&forks.one);
        pthread_mutex_lock(&forks.two);
        pthread_mutex_unlock(&waiter);
        printf("Philosopher 2 EATING\n");
        eating();
        pthread_mutex_unlock(&forks.two);
        pthread_mutex_unlock(&forks.one);
    }

    return NULL;
}

void *phil_3(void *arg)
{
    for (int i = 0; i <= NUMBER_OF_EATINGS; i++)
    {
        printf("Philosopher 3 THINKING\n");
        pthread_mutex_lock(&waiter);
        pthread_mutex_lock(&forks.two);
        pthread_mutex_lock(&forks.three);
        pthread_mutex_unlock(&waiter);
        printf("Philosopher 3 EATING\n");
        eating();
        pthread_mutex_unlock(&forks.three);
        pthread_mutex_unlock(&forks.two);
    }

    return NULL;
}

void *phil_4(void *arg)
{
    for (int i = 0; i <= NUMBER_OF_EATINGS; i++)
    {
        printf("Philosopher 4 THINKING\n");
        pthread_mutex_lock(&waiter);
        pthread_mutex_lock(&forks.three);
        pthread_mutex_lock(&forks.four);
        pthread_mutex_unlock(&waiter);
        printf("Philosopher 4 EATING\n");
        eating();
        pthread_mutex_unlock(&forks.four);
        pthread_mutex_unlock(&forks.three);
    }

    return NULL;
}

void *phil_5(void *arg)
{
    for (int i = 0; i <= NUMBER_OF_EATINGS; i++)
    {
        printf("Philosopher 5 THINKING\n");
        pthread_mutex_lock(&waiter);
        pthread_mutex_lock(&forks.four);
        pthread_mutex_lock(&forks.five);
        pthread_mutex_unlock(&waiter);
        printf("Philosopher 5 EATING\n");
        eating();
        pthread_mutex_unlock(&forks.five);
        pthread_mutex_unlock(&forks.four);
    }

    return NULL;
}

int main()
{
    pthread_t phil_1_thread;
    pthread_t phil_2_thread;
    pthread_t phil_3_thread;
    pthread_t phil_4_thread;
    pthread_t phil_5_thread;

    pthread_mutex_init(&forks.one, NULL);
    pthread_mutex_init(&forks.two, NULL);
    pthread_mutex_init(&forks.three, NULL);
    pthread_mutex_init(&forks.four, NULL);
    pthread_mutex_init(&forks.five, NULL);
    pthread_mutex_init(&waiter, NULL);

    pthread_create(&phil_1_thread, NULL, &phil_1, NULL);
    pthread_create(&phil_2_thread, NULL, &phil_2, NULL);
    pthread_create(&phil_3_thread, NULL, &phil_3, NULL);
    pthread_create(&phil_4_thread, NULL, &phil_4, NULL);
    pthread_create(&phil_5_thread, NULL, &phil_5, NULL);

    pthread_join(phil_1_thread, NULL);
    pthread_join(phil_2_thread, NULL);
    pthread_join(phil_3_thread, NULL);
    pthread_join(phil_4_thread, NULL);
    pthread_join(phil_5_thread, NULL);

    pthread_mutex_destroy(&forks.one);
    pthread_mutex_destroy(&forks.two);
    pthread_mutex_destroy(&forks.three);
    pthread_mutex_destroy(&forks.four);
    pthread_mutex_destroy(&forks.five);
    pthread_mutex_destroy(&waiter);
}
