#include "time_utils.h"

/* Given util functions */
struct timespec timespec_normalized(time_t sec, long nsec)
{
    while (nsec >= 1000000000)
    {
        nsec -= 1000000000;
        ++sec;
    }
    while (nsec < 0)
    {
        nsec += 1000000000;
        --sec;
    }
    return (struct timespec){sec, nsec};
};

struct timespec timespec_sub(struct timespec lhs, struct timespec rhs)
{
    return timespec_normalized(lhs.tv_sec - rhs.tv_sec, lhs.tv_nsec - rhs.tv_nsec);
};

struct timespec timespec_add(struct timespec lhs, struct timespec rhs)
{
    return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
};

int timespec_cmp(struct timespec lhs, struct timespec rhs)
{
    if (lhs.tv_sec < rhs.tv_sec)
        return -1;
    if (lhs.tv_sec > rhs.tv_sec)
        return 1;
    return lhs.tv_nsec - rhs.tv_nsec;
}

/* Task A */
void busy_wait(struct timespec t)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    struct timespec then = timespec_add(now, t);

    while (timespec_cmp(now, then) < 0)
    {
        for (int i = 0; i < 10000; i++)
        {
        }
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}

void busy_wait_new(int val)
{
    struct tms start;
    struct tms now;
    clock_t start_time = times(&start);
    clock_t now_time = times(&now);

    while (now_time - start_time < val)
    {
        for (int i = 0; i < 10000; i++)
        {
        }
        now_time = times(&now);
    }
}

/* Task B */

void estimate_ltcy()
{
    // struct timespec gettime;
    // struct tms sttime;

    for (int i = 0; i < 10 * 1000 * 1000; i++)
    {
        // unsigned long i = __rdtsc();
        // clock_gettime( CLOCK_REALTIME, &gettime);
        // clock_t st_time = times(&sttime);
    }
}

void est_resolution()
{
    int ns_max = 50;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int) * ns_max);

    // unsigned int t1;
    // unsigned int t2;

    // struct timespec t1;
    // struct timespec t2;
    
    struct tms tc1;
    struct tms tc2;
    clock_t t1;
    clock_t t2; 

    for (int i = 0; i < 10 * 1000 * 1000; i++)
    {
        // t1 = __rdtsc();
        // t2 = __rdtsc();
        // int ns = (t2 - t1) * 1 / (2.66);

        // clock_gettime( CLOCK_REALTIME, &t1);
        // clock_gettime( CLOCK_REALTIME, &t2);
        // int ns = (t2.tv_nsec - t1.tv_nsec);

        t1 = times(&tc1);
        t2 = times(&tc2);
        int ns = (t2 - t1) * 1000000;

        if (ns >= 0 && ns < ns_max)
        {
            histogram[ns]++;
        }
    }

    for (int i = 0; i < ns_max; i++)
    {
        printf("%d\n", histogram[i]);
    }
}

/* Task C */
void context_switch()
{
    int ns_max = 1000;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int) * ns_max);
    struct timespec t1;
    struct timespec t2;

    for (int i = 0; i < 10 * 1000 * 1000; i++)
    {

        clock_gettime(CLOCK_REALTIME, &t1);
        sched_yield();
        clock_gettime(CLOCK_REALTIME, &t2);

        int ns = (t2.tv_nsec - t1.tv_nsec);

        if (ns >= 0 && ns < ns_max)
        {
            histogram[ns]++;
        }
    }

    for (int i = 0; i < ns_max; i++)
    {
        printf("%d\n", histogram[i]);
    }
}