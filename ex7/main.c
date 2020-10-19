#include "io.h"
#include <native/task.h>
#include <native/timer.h>
#include <sys/mman.h>
#include <rtdk.h>
#include <pthread.h>

#define A 1
#define B 2
#define C 3

#define NUM_DISTURBANCES 10

#define BILLION 1000*1000*1000
#define MILLION 1000*1000
#define PERIOD 1000*1000

struct taskArgs {
    int pin;
    long period;
};

int set_cpu(int cpu_number)
{
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_number, &cpu);

    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void busyTask(void* args){
    
    unsigned long duration = 10000000000; // 10 second timeout
    unsigned long endTime = rt_timer_read() + duration;

    //int arg = ((struct taskArgs*)args)->pin;
    //long p = ((struct taskArgs*)args)->period;

	long arg = (long)args;
	//printf("struct: %p\npin: %d\n\rperiod: %lu\n\r", args, arg, p);
    //printf("pin: %lu\n\r", arg);
	
    while(1){
        // do work
        if(io_read(arg) == 0){
            io_write(arg, 0);
            rt_timer_spin(5*MILLION);
            io_write(arg, 1);
        }
        
        // the tasks must regularly yield
        // set a timeout for the maximum duration the task can run
        if(rt_timer_read() > endTime){ 
            rt_printf("Time expired\n");
            rt_task_delete(NULL);
        }

        /* rt_task_yield() - Manual round-robin.
        Move the current task to the end of its priority group, 
        so that the next equal-priority task in ready state is switched in.*/
        if(rt_task_yield()){
           rt_printf("Task failed to yield\n");
           rt_task_delete(NULL);
        }
        /* rt_task_wait_period() - Delay the current task until the next periodic release point is reached. 
        The periodic timer should have been previously started for task by a call to rt_task_set_periodic().
        */
		rt_task_wait_period(NULL);

    }
	rt_printf("Done in task\n");
}

void* disturb(void* args)
{
    set_cpu(1);
    while(1)
    {
        asm volatile("" ::: "memory");
    }
}

/* TASK CREATION
• rt task create(RT TASK *task, const char *name, int stksize, int prio, int mode)
– task: Address to an RT TASK variable, that you must create elsewhere
– name: Some descriptive name for this task
– stksize: Stack size for this task. 0 sets a default size.
– prio: Base priority. 0-99, where 0 is the lowest.
– mode: Task creation mode. Flags can be OR’d together. The flag you will need is: T CPU(cpuid): sets the CPU affinity for this task

• rt task start(RT TASK *task, void(*entry)(void *cookie), void *cookie)
– task: Address of the task, which has previously been passed to rt task create – entry: The function that is to be run in this task
– cookie: The arguments passed to the entry point function, or NULL if none

• rt task set periodic(RT TASK *task, RTIME idate, RTIME period)
• rt task wait period(unsigned long *overruns r)
*/

int main(){

    mlockall(MCL_CURRENT|MCL_FUTURE); // lock the current memory allocations and future memory allocations to the main memory

    rt_print_auto_init(1); // to use rtdk.h / rt_printf(), since "calling printf() will force your task to enter secondary mode"
    io_init();

    // initialize threads (rt and pt)
    RT_TASK busyTasks[3];
	pthread_t disturbance[NUM_DISTURBANCES];

    // TASK A: Create busy-wait tasks and disturbance threads
    for(int i = 0; i < 3; i++){
        rt_task_create(&busyTasks[i], (const char*)(i+"A"-1), 0, 1, T_CPU(1)); 
    }

	for(int i = 0; i < NUM_DISTURBANCES; i++){
		pthread_create(&disturbance[i], NULL, disturb, NULL);
	}

    // TASK B: Make real-time tasks periodic
	for(int i = 0; i < 3; i++){
		rt_task_set_periodic(&busyTasks[i], TM_NOW, PERIOD);
	}

    // Start real-time tasks
    for(long i = 0; i < 3; i++){
        rt_task_start(&busyTasks[i], &busyTask, (void*)(i+1)); //(&(struct taskArgs){i+1, 1000}));
    }

    // Join pthreads
	for (int i = 0; i < NUM_DISTURBANCES; i++) {
       pthread_join(disturbance[i], NULL);
    }

    // loop forever
	while(1);
    return 0;
}