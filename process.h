#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#define run_unit_time(){volatile unsigned long ii; for(ii = 0; ii < 1000000UL; ii++);} 
struct process {
	char name[35];
	int ready;
	int exe;
	pid_t pid;
	int id;
};
int scheduling(struct process arr[], int n, int type);
