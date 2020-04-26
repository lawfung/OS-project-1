#include <linux/linkage.h>
#include <linux/kernel.h>

asmlinkage void sys_my_print_info(int pid, long long a, long long b, long long c, long long d) {
	printk("[Project1] %d %lld.%09lld %lld.%09lld\n", pid, a, b, c, d);
}
