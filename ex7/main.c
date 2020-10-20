// #define _GNU_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>
#include <native/task.h>
#include <native/timer.h>
#include <sys/mman.h>
#include <rtdk.h>
#include "io.h"

#define DISTURB_NUM 5

RT_TASK rt_thread_1;
RT_TASK rt_thread_2;
RT_TASK rt_thread_3;

int set_cpu(int cpu_number)
{
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void *disturbance_thread(void *arg)
{
	set_cpu(0);

	while (1)
	{
		asm volatile("" ::
						 : "memory");
	}
}

void *thread_unperiodic(void *arg)
{
	unsigned long duration = 20000000000; // 20 seconds timeout
	unsigned long endTime = rt_timer_read() + duration;

	set_cpu(T_CPU(0));

	while (1)
	{
		if (io_read(*(int *)arg) == 0)
		{
			io_write(*(int *)arg, 0);
			rt_timer_spin(80000);
			io_write(*(int *)arg, 1);
		}
		if (rt_timer_read() > endTime)
		{
			rt_printf("Time expired\n");
			rt_task_delete(NULL);
		}
		if (rt_task_yield())
		{
			rt_printf("Task failed to yield\n");
			rt_task_delete(NULL);
		}
	}
}

void *thread_periodic(void *arg)
{
	unsigned long duration = 20000000000; // 20 seconds timeout
	unsigned long endTime = rt_timer_read() + duration;

	set_cpu(T_CPU(0));

	while (1)
	{
		if (io_read(*(int *)arg) == 0)
		{
			io_write(*(int *)arg, 0);
			rt_timer_spin(80000);
			io_write(*(int *)arg, 1);
		}
		if (rt_timer_read() > endTime)
		{
			rt_printf("Time expired\n");
			rt_task_delete(NULL);
		}
		if (rt_task_yield())
		{
			rt_printf("Task failed to yield\n");
			rt_task_delete(NULL);
		}

		rt_task_wait_period(NULL);
	}
}

int main()
{
	mlockall(MCL_CURRENT | MCL_FUTURE);
	io_init();
	rt_print_auto_init(1);

	int io_1 = 1, io_2 = 2, io_3 = 3;

	/* Task A */

	rt_task_create(&rt_thread_1, "thread1", 0, 40, T_CPU(0));
	rt_task_create(&rt_thread_2, "thread2", 0, 40, T_CPU(0));
	rt_task_create(&rt_thread_3, "thread3", 0, 40, T_CPU(0));

	rt_task_set_periodic(&rt_thread_1, TM_NOW, (1 * 1000 * 1000));
	rt_task_set_periodic(&rt_thread_2, TM_NOW, (1 * 1000 * 1000));
	rt_task_set_periodic(&rt_thread_3, TM_NOW, (1 * 1000 * 1000));

	rt_task_start(&rt_thread_1, &thread_unperiodic, &io_1);
	rt_task_start(&rt_thread_2, &thread_unperiodic, &io_2);
	rt_task_start(&rt_thread_3, &thread_unperiodic, &io_3);

	// pthread_t disturbance[DISTURB_NUM];

	// for (int i = 0; i < DISTURB_NUM; i++)
	// {
	// 	pthread_create(&disturbance[i], NULL, &disturbance_thread, NULL);
	// }

	// for (int i = 0; i < DISTURB_NUM; i++)
	// {
	// 	pthread_join(disturbance[i], NULL);
	// }

	/* Task B */

	// rt_task_create(&rt_thread_1, "thread1", 0, 40, T_CPU(0));
	// rt_task_create(&rt_thread_2, "thread2", 0, 40, T_CPU(0));
	// rt_task_create(&rt_thread_3, "thread3", 0, 40, T_CPU(0));

	// rt_task_set_periodic(&rt_thread_1, TM_NOW, (1 * 1000 * 1000));
	// rt_task_set_periodic(&rt_thread_2, TM_NOW, (1 * 1000 * 1000));
	// rt_task_set_periodic(&rt_thread_3, TM_NOW, (1 * 1000 * 1000));

	// rt_task_start(&rt_thread_1, &thread_periodic, &io_1);
	// rt_task_start(&rt_thread_2, &thread_periodic, &io_2);
	// rt_task_start(&rt_thread_3, &thread_periodic, &io_3);

	// // pthread_t disturbance[DISTURB_NUM];

	// // for (int i = 0; i < DISTURB_NUM; i++)
	// // {
	// // 	pthread_create(&disturbance[i], NULL, &disturbance_thread, NULL);
	// // }

	// // for (int i = 0; i < DISTURB_NUM; i++)
	// // {
	// // 	pthread_join(disturbance[i], NULL);
	// // }

	return 0;
}
