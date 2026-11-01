#ifndef PET_ELEVATOR_H
#define PET_ELEVATOR_H

#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/wait.h>

#define MIN_FLOOR 1
#define MAX_FLOOR 5
#define MAX_PETS 5
#define MAX_WEIGHT 50

enum elevator_state { OFFLINE, IDLE, LOADING, UP, DOWN };

struct pet {
	struct list_head list;
	int type, weight, start_floor, dest_floor;
	u64 sequence;
};

struct elevator {
	enum elevator_state state;
	int current_floor, load_weight, load_count;
	bool stopping;
	struct list_head riders;
	struct list_head floors[MAX_FLOOR + 1];
	unsigned long waiting_count, serviced_count;
	u64 next_sequence;
	int pickup_floor;
	struct task_struct *thread;
	struct mutex lock;
	wait_queue_head_t wake;
};

extern struct elevator elev;
int start_elevator(void);
int issue_request(int start_floor, int destination_floor, int type);
int stop_elevator(void);
int elevator_thread_fn(void *data);
/* Caller holds elev.lock. Return milliseconds until the next completed action. */
unsigned int elevator_step(void);
char pet_letter(int type);

#endif
