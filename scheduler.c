#define _GNU_SOURCE
#include "process.h"
#include <sys/wait.h>
#include <sched.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/resource.h>
struct queue{
	int arr[32];
	int l, r;
} qu;

void assign_cpu(int pid, int co) {
	//fprintf(stderr, "cup : %d %d\n", pid, co);
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(co, &mask);
	if(sched_setaffinity(pid, sizeof(mask), &mask) < 0)
		fprintf(stderr, "sched error\n");
	return;
}
void set_priority(int pid, int pri){
	//fprintf(stderr, "pid=%d pri=%d %d\n", pid, pri, SCHED_OTHER);
	int rt;
	if(pri == 99){
		struct sched_param pa;
		pa.sched_priority = 0;
		rt = sched_setscheduler(pid, SCHED_OTHER, &pa);
		setpriority(PRIO_PROCESS, pid, -19);
	}
	else{
		struct sched_param pa;
		pa.sched_priority = 0;
		rt = sched_setscheduler(pid, SCHED_IDLE, &pa);
	}
	if(rt < 0)
		fprintf(stderr, "set error rt=%d pid=%d error=%d pri=%d\n", rt, pid, errno,pri);
	return;
}
int new_pro(struct process pro) {
	int pid = fork();
	if(pid < 0)
		fprintf(stderr, "fork error");
	if(pid == 0) {
		assign_cpu(getpid(), 1);
		//set_priority(getpid(), 1);
		unsigned long s_sec, s_nsec, e_sec, e_nsec ;
		syscall(334, &s_sec, &s_nsec);
		for(int i = 0; i < pro.exe; ++ i){
			run_unit_time();
			//if(i % 100 == 0)
				//fprintf(stderr, "check 2 id=%d\n",getpid()) ;
		}
		syscall(334, &e_sec, &e_nsec);
		syscall(335,getpid(),  s_sec, s_nsec, e_sec, e_nsec);
		exit(0);
	}
	else {
		//assign_cpu(pid, 1);
		set_priority(pid, 1);
	}
	return pid;
}
int cmp(const void* a, const void* b) {
	if(  ((struct process*)a)->ready != ((struct process*)b)->ready  )
		return ((struct process*)a)->ready > ((struct process*)b)->ready;
	return ((struct process*)a)->id > ((struct process*)b)->id;
}
int select_run(struct process arr[], int now,int last_run, int last_start, int my_time, int type, int n, int done_cnt) {
	if(now != -1 && type % 2 == 0)
		return now;
	if(type == 0) {
		if(done_cnt == n || arr[done_cnt].pid == -1)	return -1;
		return done_cnt;
	}
	else if(type == 1) {
/*
		if(my_time - last_start  == 500 || now == -1) {
			for(int i = 1; i <= n; ++ i) {
				int x = (last_run + i) % n;
				if(arr[x].pid > 0) {
					return x;
				}
			}
		}
		else
			return now;
*/
		if(qu.l == qu.r)	return -1;
		return qu.arr[qu.l];
	}
	else {
		int rt = -1;
		for(int i = 0; i < n; ++ i) {
			if(arr[i].pid > 0) {
				if(rt == -1 || arr[rt].exe > arr[i].exe) 
					rt = i;
			}
		}
		return rt;
	}
	return -1;
}
int scheduling(struct process arr[], int n, int type) {
	assign_cpu(getpid(), 0);
	set_priority(getpid(), 99);
	qu.l = qu.r = 0;
	qsort(arr, n, sizeof(struct process), cmp);
	int kk = 0;
	int now = -1, last_start = -1, done = 0, last_run = -1;
	int my_time;
	for(my_time = 0; ; ++ my_time) {
		//if(my_time % 500 == 201) 	fprintf(stderr, "%d now=%d left=%d\n", my_time, now, arr[now].exe);
		if(now != -1 && arr[now].exe == 0) {
			//fprintf(stderr, "check 1 now=%d\n", now);
			int wpid = waitpid(arr[now].pid, 0, 0);
			if(wpid > 0) {
				++qu.l;
				qu.l %= 32;
				printf("%s %d\n", arr[now].name, arr[now].pid);
				fflush(stdout);
				//fprintf(stderr, "%d\n", my_time);
				arr[now].pid = -2;
				now = -1;
				++ done;
				if(done == n)
					break;
			}

		}
		while(kk < n && arr[kk].ready == my_time) {
			arr[kk].pid = new_pro(arr[kk]);
			qu.arr[qu.r] = kk;
			++ qu.r; qu.r %= 32;
			++ kk;
		}
		if(now != -1 && (my_time - last_start) %500 == 0 ) {
			qu.arr[qu.r] = qu.arr[qu.l];
			++ qu.r; ++ qu.l; qu.r %= 32; qu.l %=32;
		}
		int s = select_run(arr, now, last_run, last_start, my_time, type, n, done);
		if(s != -1 && now != s) {
			set_priority(arr[s].pid, 99);
			if(now != -1){
				set_priority(arr[now].pid, 1);	
			}
			now = s;
			last_start = my_time;
		}
		last_run = s;
		run_unit_time();
		if(now != -1)
			-- arr[now].exe;
	}
	//fprintf(stderr, "my_time=%d\n", my_time);
}
