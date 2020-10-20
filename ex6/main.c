#define _GNU_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>
#include "io.h"

#define DISTURB_NUM 5

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
}

struct timespec timespec_add(struct timespec lhs, struct timespec rhs)
{
	return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
}

int set_cpu(int cpu_number)
{
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

// void *task_a_thread(void *arg)
// {
// 	while (1)
// 	{
// 		if (io_read(*(int*)arg) == 0)
// 		{
// 			io_write(*(int*)arg, 0);
// 			usleep(80);
// 			io_write(*(int*)arg, 1);
// 		}
// 	}
// }

// void *task_b_c_thread(void *arg)
// {
// 	set_cpu(0);

// 	while (1)
// 	{
// 		if (io_read(*(int*)arg) == 0)
// 		{
// 			io_write(*(int*)arg, 0);
// 			usleep(80);
// 			io_write(*(int*)arg, 1);
// 		}
// 	}
// }

void *disturbance_thread(void *arg)
{
	set_cpu(0);
	while (1)
	{
		asm volatile("" ::
						 : "memory");
	}
}

void *task_d_thread(void *arg)
{
	struct timespec waketime;
	// "The period should be 1ms" - "Test with shorter periods"
	struct timespec period = {.tv_sec = 0, .tv_nsec = 1};
	clock_gettime(CLOCK_REALTIME, &waketime);
	set_cpu(0);

	while (1)
	{
		/* Testing */
		// printf("TASK D - %d\n", *(int*)arg);
		// fflush(stdout);

		if (io_read(*(int*)arg) == 0)
		{
			io_write(*(int*)arg, 0);
			usleep(80);
			io_write(*(int*)arg, 1);
		}

		waketime = timespec_add(waketime, period);
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
	}
}

int main()
{
	io_init();

	int io_1 = 1, io_2 = 2, io_3 = 3;

	/* Task A */

	// pthread_t thread_0;
	// pthread_t thread_1;
	// pthread_t thread_2;

	// pthread_create(&thread_0, NULL, &task_a_thread, &io_1);
	// pthread_create(&thread_1, NULL, &task_a_thread, &io_2);
	// pthread_create(&thread_2, NULL, &task_a_thread, &io_3);

	// pthread_join(thread_0, NULL);
	// pthread_join(thread_1, NULL);
	// pthread_join(thread_2, NULL);

	/* Task B */

	// pthread_t thread_0;
	// pthread_t thread_1;
	// pthread_t thread_2;

	// pthread_create(&thread_0, NULL, &task_b_c_thread, &io_1);
	// pthread_create(&thread_1, NULL, &task_b_c_thread, &io_2);
	// pthread_create(&thread_2, NULL, &task_b_c_thread, &io_3);

	// pthread_join(thread_0, NULL);
	// pthread_join(thread_1, NULL);
	// pthread_join(thread_2, NULL);

	/* Task C */

	// pthread_t disturbance[DISTURB_NUM];
	// pthread_t thread_0;
	// pthread_t thread_1;
	// pthread_t thread_2;

	// for (int i = 0; i < DISTURB_NUM; i++)
	// {
	// 	pthread_create(&disturbance[i], NULL, &disturbance_thread, NULL);
	// }

	// pthread_create(&thread_0, NULL, &task_b_c_thread, &io_1);
	// pthread_create(&thread_1, NULL, &task_b_c_thread, &io_2);
	// pthread_create(&thread_2, NULL, &task_b_c_thread, &io_3);

	// for (int i = 0; i < DISTURB_NUM; i++)
	// {
	// 	pthread_join(disturbance[i], NULL);
	// }

	// pthread_join(thread_0, NULL);
	// pthread_join(thread_1, NULL);
	// pthread_join(thread_2, NULL);

	/* Task D */

	pthread_t disturbance[DISTURB_NUM];
	pthread_t thread_0;
	pthread_t thread_1;
	pthread_t thread_2;

	for (int i = 0; i < DISTURB_NUM; i++)
	{
		pthread_create(&disturbance[i], NULL, &disturbance_thread, NULL);
	}

	pthread_create(&thread_0, NULL, &task_d_thread, &io_1);
	pthread_create(&thread_1, NULL, &task_d_thread, &io_2);
	pthread_create(&thread_2, NULL, &task_d_thread, &io_3);

	for (int i = 0; i < DISTURB_NUM; i++)
	{
		pthread_join(disturbance[i], NULL);
	}

	pthread_join(thread_0, NULL);
	pthread_join(thread_1, NULL);
	pthread_join(thread_2, NULL);

	return 0;
}
