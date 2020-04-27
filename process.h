#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#define run_unit_time(){volatile unsigned long ii; for(ii = 0; ii < 1000000UL; ii++);} 
struct process {
	char name[35];
	int ready;
	int exe;
	pid_t pid;
	int id;
}
void assign_cpu(int pid, int co) {
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(co, &mask);
	if(sched_setaffinity(pid, sizeof(mask), &mask) < 0)
		fprintf(stderr, "sched error");
	return;
}
void set_priority(int pid, int poli){
	struct sched_param pa;
	pa.sched_priority = 0;
	if(sched_setscheduler(pid, poli, &pa) < 0)
		fprintf(stderr, "set error");
	return;
}
int new_pro(struct process pro) {
	int pid = fork();
	if(pid < 0)
		fprintf(stderr, "fork error");
	if(pid == 0) {
		assign_cpu(getpid(), 1);
		unsigned long s_sec, s_nsec, e_sec, e_nsec ;
		syscall(334, &s_sec, &s_nsec);
		for(int i = 0; i < pro.exe; ++ i)	run_unit_time();
		syscall(334, &e_sec, &e_nsec);
		syscall(335, s_sec, s_nsec, e_sec, e_nsec);
		exit(0);
	}
	return pid;
}
