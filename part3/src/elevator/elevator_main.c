#include <linux/init.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include "elevator.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cop4610t");
MODULE_DESCRIPTION("FIFO pet elevator with five floors and graceful stop");
MODULE_VERSION("1.0");

struct elevator elev;
static struct proc_dir_entry *elevator_entry;

extern int (*STUB_start_elevator)(void);
extern int (*STUB_issue_request)(int, int, int);
extern int (*STUB_stop_elevator)(void);

static int elevator_show(struct seq_file *m, void *v)
{
	static const char * const states[] = {
		"OFFLINE", "IDLE", "LOADING", "UP", "DOWN"
	};
	struct pet *pet;
	unsigned long count;
	int floor;

	mutex_lock(&elev.lock);
	seq_printf(m, "Elevator state: %s\n", states[elev.state]);
	seq_printf(m, "Current floor: %d\n", elev.current_floor);
	seq_printf(m, "Current load: %d lbs\n", elev.load_weight);
	seq_puts(m, "Elevator status:");
	list_for_each_entry(pet, &elev.riders, list)
		seq_printf(m, " %c%d", pet_letter(pet->type), pet->dest_floor);
	seq_putc(m, '\n');

	for (floor = MAX_FLOOR; floor >= MIN_FLOOR; floor--) {
		count = 0;
		list_for_each_entry(pet, &elev.floors[floor], list)
			count++;
		seq_printf(m, "[%c] Floor %d: %lu",
			   elev.current_floor == floor ? '*' : ' ', floor, count);
		list_for_each_entry(pet, &elev.floors[floor], list)
			seq_printf(m, " %c%d", pet_letter(pet->type), pet->dest_floor);
		seq_putc(m, '\n');
	}
	seq_printf(m, "Number of pets: %d\n", elev.load_count);
	seq_printf(m, "Number of pets waiting: %lu\n", elev.waiting_count);
	seq_printf(m, "Number of pets serviced: %lu\n", elev.serviced_count);
	mutex_unlock(&elev.lock);
	return 0;
}

static int elevator_open(struct inode *inode, struct file *file)
{
	return single_open(file, elevator_show, NULL);
}

static const struct proc_ops elevator_ops = {
	.proc_open = elevator_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};

static int __init elevator_init(void)
{
	int floor, ret;

	/* syscheck and elevator must never own the same hooks simultaneously. */
	if (READ_ONCE(STUB_start_elevator) || READ_ONCE(STUB_issue_request) ||
	    READ_ONCE(STUB_stop_elevator))
		return -EBUSY;

	mutex_init(&elev.lock);
	init_waitqueue_head(&elev.wake);
	INIT_LIST_HEAD(&elev.riders);
	for (floor = MIN_FLOOR; floor <= MAX_FLOOR; floor++)
		INIT_LIST_HEAD(&elev.floors[floor]);
	elev.current_floor = MIN_FLOOR;
	elev.state = OFFLINE;

	elevator_entry = proc_create("elevator", 0444, NULL, &elevator_ops);
	if (!elevator_entry)
		return -ENOMEM;
	elev.thread = kthread_run(elevator_thread_fn, NULL, "pet_elevator");
	if (IS_ERR(elev.thread)) {
		ret = PTR_ERR(elev.thread);
		proc_remove(elevator_entry);
		return ret;
	}

	WRITE_ONCE(STUB_start_elevator, start_elevator);
	WRITE_ONCE(STUB_issue_request, issue_request);
	WRITE_ONCE(STUB_stop_elevator, stop_elevator);
	return 0;
}

static void free_pets(struct list_head *head)
{
	struct pet *pet, *next;

	list_for_each_entry_safe(pet, next, head, list) {
		list_del(&pet->list);
		kfree(pet);
	}
}

static void __exit elevator_exit(void)
{
	int floor;

	WRITE_ONCE(STUB_start_elevator, NULL);
	WRITE_ONCE(STUB_issue_request, NULL);
	WRITE_ONCE(STUB_stop_elevator, NULL);
	proc_remove(elevator_entry);
	kthread_stop(elev.thread);
	mutex_lock(&elev.lock);
	free_pets(&elev.riders);
	for (floor = MIN_FLOOR; floor <= MAX_FLOOR; floor++)
		free_pets(&elev.floors[floor]);
	mutex_unlock(&elev.lock);
}

module_init(elevator_init);
module_exit(elevator_exit);
