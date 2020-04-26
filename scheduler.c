#include "process.h"
int cmp(struct process* a, struct process* b) {
	if(a->ready != b->ready)	return a->ready > b->ready;
	return a->id > b->id;
}
int select(struct process arr[], int now,int last_run, int last_start, int my_time, int type, int n, int done_cnt) {
	if(now != -1 && type % 2 == 0)
		return now;
	if(type == 0) {
		if(done_cnt == n || arr[done_cnt].pid == -1)	return -1;
		return done_cnt;
	}
	else if(type == 1) {
		if(my_time - last_start  == 500 || now == -1) {
			for(int i = 1; i <= n; ++ i) {
				int x = (last_start + i) % n;
				if(arr[x].pid > 0) {
					return x;
				}
			}
		}
		else
			return now;
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
	qsort(arr, n, sizeof(struct process), cmp);
	int kk = 0;
	int now = -1, last_start = -1, done = 0, last_run = -1;
	for(int my_time = 0; ; ++ my_time) {
		if(now != -1) {
			int wpid = waitpid(arr[now].pid, 0, WNOHANG);
			if(wpid > 0) {
				printf("%s %d\n", arr[now].name, arr[now].pid);
				arr[now].pid = -2;
				now = -1;
				++ done;
				if(done == n)
					break;
			}

		}
		while(kk < n && arr[kk].ready == my_time) {
			arr[kk].pid = new_pro(arr[kk]);
			set_low(arr[kk].pid);
			++ kk;
		}
		int s = select();
		if(s != -1 && now != s) {
			set_high(arr[s].pid);
			if(now != -1)
				set_low(arr[now].pid);
			now = s;
			last_run = s;
			last_start = my_time;
		}
		run_unit_time();
		if(now != -1)
			-- arr[now].exe;
	}
}
