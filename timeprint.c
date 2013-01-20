#include <linux/init.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/kthread.h>

static struct task_struct *timeprint_task;

#define log(fmt, ...) \
	printk(KERN_ALERT "%s: " fmt "\n", THIS_MODULE->name, ## __VA_ARGS__)

static int timeprint_thread(void *data)
{
	while (!kthread_should_stop()) {
		struct timespec tv;
		tv = current_kernel_time();
		log("%10ld", (long)tv.tv_sec);

		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ);
	}

	log("thread exiting");
	return 0;
}

static int __init timeprint_init(void)
{
	log("init");

	timeprint_task = kthread_run(timeprint_thread, NULL, "timeprint");
	if (IS_ERR(timeprint_task)) {
		log("kthread_run failed");
		return -1;
	}

	return 0;
}

static void __exit timeprint_exit(void)
{
	kthread_stop(timeprint_task);
}

module_init(timeprint_init);
module_exit(timeprint_exit);

MODULE_AUTHOR("Clemens Buchacher");
MODULE_DESCRIPTION("timeprint-experiment");
MODULE_LICENSE("Dual BSD/GPL");
