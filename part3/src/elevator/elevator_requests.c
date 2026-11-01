#include <linux/errno.h>
#include <linux/slab.h>
#include "elevator.h"

static const int pet_weights[] = { 3, 14, 10, 16 };

char pet_letter(int type)
{
	static const char letters[] = { 'C', 'P', 'H', 'D' };
	return letters[type];
}

int start_elevator(void)
{
	int ret = 0;

	mutex_lock(&elev.lock);
	if (elev.state != OFFLINE) {
		ret = 1;
	} else {
		/* The worker is created at module insertion; it sleeps while offline. */
		elev.current_floor = MIN_FLOOR;
		elev.load_count = 0;
		elev.load_weight = 0;
		elev.stopping = false;
		elev.pickup_floor = 0;
		elev.state = IDLE;
	}
	mutex_unlock(&elev.lock);
	wake_up(&elev.wake);
	return ret;
}

int issue_request(int start_floor, int destination_floor, int type)
{
	struct pet *pet;

	if (start_floor < MIN_FLOOR || start_floor > MAX_FLOOR ||
	    destination_floor < MIN_FLOOR || destination_floor > MAX_FLOOR ||
	    start_floor == destination_floor || type < 0 || type > 3)
		return 1;

	pet = kmalloc(sizeof(*pet), GFP_KERNEL);
	if (!pet)
		return -ENOMEM;
	pet->type = type;
	pet->weight = pet_weights[type];
	pet->start_floor = start_floor;
	pet->dest_floor = destination_floor;

	mutex_lock(&elev.lock);
	pet->sequence = elev.next_sequence++;
	list_add_tail(&pet->list, &elev.floors[start_floor]);
	elev.waiting_count++;
	mutex_unlock(&elev.lock);
	wake_up(&elev.wake);
	return 0;
}

int stop_elevator(void)
{
	int ret = 0;

	mutex_lock(&elev.lock);
	if (elev.stopping) {
		ret = 1;
	} else if (elev.state != OFFLINE) {
		elev.stopping = true;
	}
	mutex_unlock(&elev.lock);
	wake_up(&elev.wake);
	return ret;
}
