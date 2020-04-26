#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/timer.h>
asmlinkage void sys_my_get_time(struct timespec* w) {
	getnstimeofday(w);
}
