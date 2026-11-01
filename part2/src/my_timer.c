// Based on the supplied my_timer.c; retain one complete snapshot per open.
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/timekeeping.h>
#include <linux/mutex.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robert Began");
MODULE_DESCRIPTION("Current and elapsed real time through /proc/timer");
MODULE_VERSION("1.1");

struct timer_snapshot {
	char buf[160];
	size_t len;
	struct mutex lock;
};

static struct proc_dir_entry *timer_entry;
static struct timespec64 last_time;
static bool has_last_time;
static DEFINE_MUTEX(timer_lock);

static int timer_open(struct inode *inode, struct file *file)
{
	struct timer_snapshot *snapshot;

	snapshot = kzalloc(sizeof(*snapshot), GFP_KERNEL);
	if (!snapshot)
		return -ENOMEM;
	mutex_init(&snapshot->lock);
	file->private_data = snapshot;
	return 0;
}

static ssize_t timer_read(struct file *file, char __user *ubuf,
			  size_t count, loff_t *ppos)
{
	struct timer_snapshot *snapshot = file->private_data;
	struct timespec64 now, elapsed;
	ssize_t result;
	size_t len;

	if (!count)
		return 0;
	mutex_lock(&snapshot->lock);
	if (snapshot->len) {
		result = simple_read_from_buffer(ubuf, count, ppos,
						snapshot->buf, snapshot->len);
		goto out;
	}

	mutex_lock(&timer_lock);
	ktime_get_real_ts64(&now);
	len = scnprintf(snapshot->buf, sizeof(snapshot->buf),
			"current time: %lld.%09ld\n",
			(long long)now.tv_sec, now.tv_nsec);
	if (has_last_time) {
		elapsed = timespec64_sub(now, last_time);
		/* Real time can be adjusted backwards. Format negative intervals
		 * as a signed decimal rather than e.g. -1.900000000 for -0.1.
		 */
		if (elapsed.tv_sec < 0) {
			long long seconds = -(elapsed.tv_sec + 1);
			long nanos = NSEC_PER_SEC - elapsed.tv_nsec;

			if (nanos == NSEC_PER_SEC) {
				seconds++;
				nanos = 0;
			}
			len += scnprintf(snapshot->buf + len,
				 sizeof(snapshot->buf) - len,
				 "elapsed time: -%lld.%09ld\n", seconds, nanos);
		} else {
			len += scnprintf(snapshot->buf + len,
				 sizeof(snapshot->buf) - len,
				 "elapsed time: %lld.%09ld\n",
				 (long long)elapsed.tv_sec, elapsed.tv_nsec);
		}
	}
	result = simple_read_from_buffer(ubuf, count, ppos, snapshot->buf, len);
	if (result > 0) {
		snapshot->len = len;
		last_time = now;
		has_last_time = true;
	}
	mutex_unlock(&timer_lock);
out:
	mutex_unlock(&snapshot->lock);
	return result;
}

static int timer_release(struct inode *inode, struct file *file)
{
	kfree(file->private_data);
	return 0;
}

static const struct proc_ops timer_ops = {
	.proc_open = timer_open,
	.proc_read = timer_read,
	.proc_release = timer_release,
};

static int __init my_timer_init(void)
{
	timer_entry = proc_create("timer", 0444, NULL, &timer_ops);
	return timer_entry ? 0 : -ENOMEM;
}

static void __exit my_timer_exit(void)
{
	proc_remove(timer_entry);
}

module_init(my_timer_init);
module_exit(my_timer_exit);
