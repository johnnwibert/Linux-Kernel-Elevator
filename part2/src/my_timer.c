// my_timer.c - kernel module that exposes /proc/timer. Each read of /proc/timer prints the current time (seconds.nanoseconds since the Unix Epoch, from ktime_get_real_ts64()) and, after the first read, the time elapsed since the previous read.
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/timekeeping.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robert Began");
MODULE_DESCRIPTION("Prints current time and elapsed time via /proc/timer");
MODULE_VERSION("1.0");

#define ENTRY_NAME "timer"
#define PERMS 0444
#define BUF_LEN 128

static struct proc_dir_entry *timer_entry;
static struct timespec64 last_time;	//time recorded on previous read
static bool has_last_time;		//false until thefirst read
static DEFINE_MUTEX(timer_lock);	//protects last_time/has_lasttime

//called on every read and only produces new output at offset zero
static ssize_t timer_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
	char buf[BUF_LEN];
	struct timespec64 now;
	struct timespec64 elapsed;
	int len;

	//a second rea from the same open
	if (*ppos > 0)
		return 0;

	ktime_get_real_ts64(&now);

	mutex_lock(&timer_lock);
	len = scnprintf(buf, BUF_LEN, "current time: %lld.%09ld\n", (long long)now.tv_sec, now.tv_nsec);

	if (has_last_time) {
		elapsed = timespec64_sub(now, last_time);
		len += scnprintf(buf + len, BUF_LEN - len, "elapsed time: %lld.%09ld\n", (long long)elapsed.tv_sec, elapsed.tv_nsec);
	}

	last_time = now;
	has_last_time = true;
	mutex_unlock(&timer_lock);

	return simple_read_from_buffer(ubuf, count, ppos, buf, len);
}

static const struct proc_ops timer_fops = {
	.proc_read = timer_read,
};

static int __init my_timer_init(void)
{
	timer_entry = proc_create(ENTRY_NAME, PERMS, NULL, &timer_fops);
	if (!timer_entry)
		return -ENOMEM;
	return 0;
}

static void __exit my_timer_exit(void)
{
	proc_remove(timer_entry);
}

module_init(my_timer_init);
module_exit(my_timer_exit);
