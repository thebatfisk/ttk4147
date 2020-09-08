#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <sys/time.h>
#include <sys/times.h> // timespec structure
#include <time.h>      // sleep() nanosleep() etc.
#include <unistd.h>
#include <x86intrin.h> // __rdtsc()

void busy_wait(struct timespec t);
void busy_wait_new(int val);
void estimate_ltcy();
void est_resolution();
void context_switch();
