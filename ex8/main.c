#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <sys/mman.h>
#include <rtdk.h>
#include <sched.h>
#include <unistd.h>
#include <stdbool.h>

RT_SEM sem;
RT_MUTEX mutex;
RT_MUTEX mutex_a;
RT_MUTEX mutex_b;
RT_TASK rt_thread_1;
RT_TASK rt_thread_2;
RT_TASK rt_thread_3;

typedef struct resource
{
	RT_MUTEX *mutex;
	int priority;
} resource;

typedef struct thread_priorities
{
	RT_TASK *task;
	int base;
	int boost;
} thread_priorities;

struct resource resource_a = {.mutex = &mutex_a, .priority = 90};
struct resource resource_b = {.mutex = &mutex_b, .priority = 95};
struct thread_priorities thread_1_info = {.task = &rt_thread_1, .base = 10};
struct thread_priorities thread_2_info = {.task = &rt_thread_3, .base = 30};

static bool main_wait = true;

int set_cpu(int cpu_number)
{
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void busy_wait_us(unsigned long delay)
{
	for (; delay > 0; delay--)
	{
		rt_timer_spin(1000);
	}
}

void icpp_lock(resource *resc, thread_priorities *task_str)
{
	rt_task_set_priority(task_str->task, resc->priority);
	rt_mutex_acquire(resc->mutex, TM_INFINITE);
	task_str->boost = resc->priority;
}

void icpp_unlock(struct resource *resc, struct thread_priorities *task_str)
{
	rt_task_set_priority(task_str->task, task_str->base);
	rt_mutex_release(resc->mutex);
}

/* Task A */

void thread_tsk_a_wait_1(void *arg)
{
	set_cpu(T_CPU(0));

	rt_printf("Wait thread 1 waiting\n");
	rt_sem_p(&sem, TM_INFINITE);
	rt_printf("Wait thread 1 continuing\n");
}

void thread_tsk_a_wait_2(void *arg)
{
	set_cpu(T_CPU(0));

	rt_printf("Wait thread 2 waiting\n");
	rt_sem_p(&sem, TM_INFINITE);
	rt_printf("Wait thread 2 continuing\n");
}

void thread_tsk_a_sync(void *arg)
{
	set_cpu(T_CPU(0));

	rt_printf("Sync thread sleeping\n");
	rt_task_sleep(5000000000);
	rt_printf("Sync thread releasing\n");
	rt_sem_broadcast(&sem);
	rt_task_sleep(100000000);
	main_wait = false;
}

/* Task B */

void thread_tsk_b_low(void *arg)
{
	set_cpu(T_CPU(0));

	rt_printf("Low locking semaphore\n");
	rt_sem_p(&sem, TM_INFINITE);
	rt_printf("Low busy waiting\n");
	busy_wait_us(300000);
	rt_printf("Low releasing semaphore\n");
	rt_sem_v(&sem);
}

void thread_tsk_b_medium(void *arg)
{
	set_cpu(T_CPU(0));

	rt_printf("Medium sleeping\n");
	rt_task_sleep(100000000);
	rt_printf("Medium busy waiting\n");
	busy_wait_us(500000);
	rt_printf("Medium finished\n");
}

void thread_tsk_b_high(void *arg)
{
	set_cpu(T_CPU(0));

	rt_printf("High sleeping\n");
	rt_task_sleep(200000000);
	rt_printf("High locking semaphore\n");
	rt_sem_p(&sem, TM_INFINITE);
	rt_printf("High busy waiting\n");
	busy_wait_us(200000);
	rt_printf("High releasing semaphore\n");
	rt_sem_v(&sem);
}

/* Task C */

void thread_tsk_c_low(void *arg)
{
	set_cpu(T_CPU(0));

	rt_printf("Low acquire mutex\n");
	rt_mutex_acquire(&mutex, TM_INFINITE);
	rt_printf("Low busy waiting\n");
	busy_wait_us(300000);
	rt_printf("Low releasing mutex\n");
	rt_mutex_release(&mutex);
}

void thread_tsk_c_medium(void *arg)
{
	set_cpu(T_CPU(0));

	rt_printf("Medium sleeping\n");
	rt_task_sleep(100000000);
	rt_printf("Medium busy waiting\n");
	busy_wait_us(500000);
	rt_printf("Medium finished\n");
}

void thread_tsk_c_high(void *arg)
{
	set_cpu(T_CPU(0));

	rt_printf("High sleeping\n");
	rt_task_sleep(200000000);
	rt_printf("High acquire mutex\n");
	rt_mutex_acquire(&mutex, TM_INFINITE);
	rt_printf("High busy waiting\n");
	busy_wait_us(200000);
	rt_printf("High releasing mutex\n");
	rt_mutex_release(&mutex);
}

/* Task D */

void thread_tsk_d_low(void *arg)
{
	set_cpu(T_CPU(0));

	rt_printf("Low acquire mutex_a\n");
	rt_mutex_acquire(&mutex_a, TM_INFINITE);
	rt_printf("Low busy waiting\n");
	busy_wait_us(300000);
	rt_printf("Low acquire mutex_b\n");
	rt_mutex_acquire(&mutex_b, TM_INFINITE);
	rt_printf("Low busy waiting\n");
	busy_wait_us(300000);
	rt_printf("Low releasing mutex_b\n");
	rt_mutex_release(&mutex_b);
	rt_printf("Low releasing mutex_a\n");
	rt_mutex_release(&mutex_a);
	rt_printf("Low busy waiting\n");
	busy_wait_us(100000);
}

void thread_tsk_d_high(void *arg)
{
	set_cpu(T_CPU(0));

	rt_printf("High sleeping\n");
	rt_task_sleep(100000000);
	rt_printf("High acquire mutex_b\n");
	rt_mutex_acquire(&mutex_b, TM_INFINITE);
	rt_printf("High busy waiting\n");
	busy_wait_us(100000);
	rt_printf("High acquire mutex_a\n");
	rt_mutex_acquire(&mutex_a, TM_INFINITE);
	rt_printf("High busy waiting\n");
	busy_wait_us(200000);
	rt_printf("High releasing mutex_a\n");
	rt_mutex_release(&mutex_a);
	rt_printf("High releasing mutex_b\n");
	rt_mutex_release(&mutex_b);
	rt_printf("High busy waiting\n");
	busy_wait_us(100000);
}

/* Task E */

void thread_tsk_e_low(void *arg)
{
	set_cpu(T_CPU(0));

	rt_printf("Low lock a\n");
	icpp_lock(&resource_a, &thread_1_info);
	rt_printf("Low busy waiting\n");
	busy_wait_us(300000);
	rt_printf("Low lock b\n");
	icpp_lock(&resource_b, &thread_1_info);
	rt_printf("Low busy waiting\n");
	busy_wait_us(300000);
	rt_printf("Low unlock b\n");
	icpp_unlock(&resource_b, &thread_1_info);
	rt_printf("Low unlock a\n");
	icpp_unlock(&resource_a, &thread_1_info);
	rt_printf("Low busy waiting\n");
	busy_wait_us(100000);
}

void thread_tsk_e_high(void *arg)
{
	set_cpu(T_CPU(0));

	rt_printf("High sleeping\n");
	rt_task_sleep(100000000);
	rt_printf("High lock b\n");
	icpp_lock(&resource_b, &thread_2_info);
	rt_printf("High busy waiting\n");
	busy_wait_us(100000);
	rt_printf("High lock a\n");
	icpp_lock(&resource_a, &thread_2_info);
	rt_printf("High busy waiting\n");
	busy_wait_us(200000);
	rt_printf("High unlock a\n");
	icpp_unlock(&resource_a, &thread_2_info);
	rt_printf("High unlock b\n");
	icpp_unlock(&resource_b, &thread_2_info);
	rt_printf("High busy waiting\n");
	busy_wait_us(100000);
}

int main()
{
	mlockall(MCL_CURRENT | MCL_FUTURE);
	rt_print_auto_init(1);

	/* Task A */
// 	rt_sem_create(&sem, "semaphore", 0, S_PRIO);
// 
// 	rt_task_create(&rt_thread_1, "wait_thread_1", 0, 10, T_CPU(0));
// 	rt_task_create(&rt_thread_2, "wait_thread_2", 0, 20, T_CPU(0));
// 	rt_task_create(&rt_thread_3, "sync_thread", 0, 30, T_CPU(0));
// 
// 	rt_task_start(&rt_thread_1, &thread_tsk_a_wait_1, NULL);
// 	rt_task_start(&rt_thread_2, &thread_tsk_a_wait_2, NULL);
// 	rt_task_start(&rt_thread_3, &thread_tsk_a_sync, NULL);

	/* Task B */
// 	rt_sem_create(&sem, "semaphore", 1, S_PRIO);
// 
// 	rt_task_create(&rt_thread_1, "thread_low", 0, 10, T_CPU(0));
// 	rt_task_create(&rt_thread_2, "thread_medium", 0, 20, T_CPU(0));
// 	rt_task_create(&rt_thread_3, "thread_high", 0, 30, T_CPU(0));
// 
// 	rt_task_start(&rt_thread_1, &thread_tsk_b_low, NULL);
// 	rt_task_start(&rt_thread_2, &thread_tsk_b_medium, NULL);
// 	rt_task_start(&rt_thread_3, &thread_tsk_b_high, NULL);

	/* Task C */
// 	rt_mutex_create(&mutex, "mutex");
// 
// 	rt_task_create(&rt_thread_1, "thread_low", 0, 10, T_CPU(0));
// 	rt_task_create(&rt_thread_2, "thread_medium", 0, 20, T_CPU(0));
// 	rt_task_create(&rt_thread_3, "thread_high", 0, 30, T_CPU(0));
// 
// 	rt_task_start(&rt_thread_1, &thread_tsk_c_low, NULL);
// 	rt_task_start(&rt_thread_2, &thread_tsk_c_medium, NULL);
// 	rt_task_start(&rt_thread_3, &thread_tsk_c_high, NULL);

	/* Task D */
	
// 	rt_mutex_create(&mutex_a, "mutex_a");
// 	rt_mutex_create(&mutex_b, "mutex_b");
// 
// 	rt_task_create(&rt_thread_1, "thread_low", 0, 10, T_CPU(0));
// 	rt_task_create(&rt_thread_3, "thread_high", 0, 30, T_CPU(0));
// 
// 	rt_task_start(&rt_thread_1, &thread_tsk_d_low, NULL);
// 	rt_task_start(&rt_thread_3, &thread_tsk_d_high, NULL);
	
	/* Task E */
	
	rt_mutex_create(&mutex_a, "mutex_a");
	rt_mutex_create(&mutex_b, "mutex_b");

	rt_task_create(&rt_thread_1, "thread_low", 0, 10, T_CPU(0));
	rt_task_create(&rt_thread_3, "thread_high", 0, 30, T_CPU(0));

	rt_task_start(&rt_thread_1, &thread_tsk_e_low, NULL);
	rt_task_start(&rt_thread_3, &thread_tsk_e_high, NULL);

	while (main_wait)
	{
	}
	
	rt_printf("Deleting semaphore\n");
	rt_sem_delete(&sem);

	return 0;
}
