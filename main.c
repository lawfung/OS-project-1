#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "process.h"
#include <sched.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
	char policy[9];
	scanf("%s", policy);
	int n;
	scanf("%d", &n);
//	struct process *arr = malloc(sizeof(struct process) * n);
	struct process arr[n];
	for(int i = 0; i < n; ++ i ) {
		scanf("%s%d%d", arr[i].name, &arr[i].ready, &arr[i].exe);
		arr[i].id = i;
		arr[i].pid = -1;
	}
	if(strcmp(policy, "FIFO") == 0)
		scheduling(arr, n, 0);
	else if(strcmp(policy, "RR") == 0)
		scheduling(arr, n, 1);
	else if(strcmp(policy, "SJF") == 0)
		scheduling(arr, n, 2);
	else if(strcmp(policy, "PSJF") == 0)
		scheduling(arr, n, 3);
	else {
		fprintf(stderr, "policy fault\n");
	}
	exit(0);
}
