#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include "elevator.h"

/* Select the floor whose head request has waited longest. */
static int oldest_floor(void)
{
	struct pet *head, *oldest = NULL;
	int floor, selected = 0;

	for (floor = MIN_FLOOR; floor <= MAX_FLOOR; floor++) {
		if (list_empty(&elev.floors[floor]))
			continue;
		head = list_first_entry(&elev.floors[floor], struct pet, list);
		if (!oldest || head->sequence < oldest->sequence) {
			oldest = head;
			selected = floor;
		}
	}
	return selected;
}

static bool has_dropoff(void)
{
	struct pet *pet;

	list_for_each_entry(pet, &elev.riders, list)
		if (pet->dest_floor == elev.current_floor)
			return true;
	return false;
}

/* Complete one 1-second loading/unloading stop. Strict FIFO at each floor. */
static void finish_loading(void)
{
	struct pet *pet, *next;

	list_for_each_entry_safe(pet, next, &elev.riders, list) {
		if (pet->dest_floor != elev.current_floor)
			continue;
		list_del(&pet->list);
		elev.load_count--;
		elev.load_weight -= pet->weight;
		elev.serviced_count++;
		kfree(pet);
	}

	if (!elev.stopping && elev.pickup_floor == elev.current_floor) {
		while (!list_empty(&elev.floors[elev.current_floor])) {
			pet = list_first_entry(&elev.floors[elev.current_floor],
					       struct pet, list);
			/* Never skip a heavy head pet to board a lighter later pet. */
			if (elev.load_count == MAX_PETS ||
			    elev.load_weight + pet->weight > MAX_WEIGHT)
				break;
			list_move_tail(&pet->list, &elev.riders);
			elev.load_count++;
			elev.load_weight += pet->weight;
			elev.waiting_count--;
		}
	}
	elev.pickup_floor = 0;
}

unsigned int elevator_step(void)
{
	struct pet *first;
	int target;

	if (elev.state == OFFLINE)
		return 0;

	/* The previous iteration slept for the full duration of this action. */
	if (elev.state == UP)
		elev.current_floor++;
	else if (elev.state == DOWN)
		elev.current_floor--;
	else if (elev.state == LOADING)
		finish_loading();

	if (elev.stopping && !elev.load_count) {
		elev.pickup_floor = 0;
		elev.stopping = false;
		elev.state = OFFLINE;
		return 0;
	}

	/* Serve a finite batch, then select the oldest outstanding floor.
	 * New arrivals cannot indefinitely postpone an older request elsewhere.
	 */
	if (elev.load_count) {
		if (has_dropoff()) {
			elev.state = LOADING;
			return 1000;
		}
		first = list_first_entry(&elev.riders, struct pet, list);
		target = first->dest_floor;
	} else {
		if (!elev.pickup_floor)
			elev.pickup_floor = oldest_floor();
		target = elev.pickup_floor;
		if (!target) {
			elev.state = IDLE;
			return 0;
		}
		if (target == elev.current_floor) {
			elev.state = LOADING;
			return 1000;
		}
	}

	elev.state = target > elev.current_floor ? UP : DOWN;
	return 2000;
}

int elevator_thread_fn(void *data)
{
	unsigned int delay;

	while (!kthread_should_stop()) {
		mutex_lock(&elev.lock);
		delay = elevator_step();
		mutex_unlock(&elev.lock);

		if (delay) {
			/* No mutex is held during travel or loading delays. */
			msleep(delay);
		} else {
			wait_event(elev.wake, kthread_should_stop() ||
				(READ_ONCE(elev.state) != OFFLINE &&
				 (READ_ONCE(elev.stopping) ||
				  READ_ONCE(elev.waiting_count) > 0)));
		}
	}
	return 0;
}
